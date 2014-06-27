
//OpenSCADA OPC_UA implementation library file: libOPC_UA.cpp
/******************************************************************************
 *   Copyright (C) 2009-2014 by Roman Savochenko, <rom_as@oscada.org>	      *
 *									      *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as	      *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *									      *
 *   This library is distributed in the hope that it will be useful,	      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of	      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	      *
 *   GNU Lesser General Public License for more details.		      *
 *									      *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the			      *
 *   Free Software Foundation, Inc.,					      *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.		      *
 ******************************************************************************/

#include <sys/time.h>
#include <ieee754.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/aes.h"
#include "openssl/err.h"
#include <openssl/rand.h>
#include "openssl/hmac.h"
#include "openssl/evp.h"

#include "libOPC_UA.h"

namespace OPC
{
//*************************************************
//* Static methods, mostly from OpenSCADA::TSYS   *
//*************************************************
int64_t curTime( )
{
    timeval cur_tm;
    gettimeofday(&cur_tm, NULL);
    return (int64_t)cur_tm.tv_sec*1000000 + cur_tm.tv_usec;
}

string int2str( int val )
{
    char buf[250];
    snprintf(buf, sizeof(buf), "%d", val);
    return buf;
}

string uint2str( unsigned val )
{
    char buf[250];
    snprintf(buf, sizeof(buf), "%u", val);
    return buf;
}

string ll2str( int64_t val )
{
    char buf[250];
    snprintf(buf, sizeof(buf), "%lld", (long long int)val);
    return buf;
}

string real2str( double val, int prec, char tp )
{
    char buf[250];
    prec = std::max(0, prec);
    switch(tp)
    {
	case 'g': snprintf(buf, sizeof(buf), "%.*g", prec, val);	break;
	case 'e': snprintf(buf, sizeof(buf), "%.*e", prec, val);	break;
	default:  snprintf(buf, sizeof(buf), "%.*f", prec, val);	break;
    }
    return buf;
}

string strParse( const string &path, int level, const string &sep, int *off, bool mergeSepSymb )
{
    int an_dir = off ? *off : 0;
    int t_lev = 0;
    size_t t_dir;

    if(an_dir >= (int)path.size() || sep.empty()) return "";
    while(true)
    {
	t_dir = path.find(sep, an_dir);
	if(t_dir == string::npos)
	{
	    if(off) *off = path.size();
	    return (t_lev == level) ? path.substr(an_dir) : "";
	}
	else if(t_lev == level)
	{
	    if(off) *off = t_dir+sep.size();
	    return path.substr(an_dir, t_dir-an_dir);
	}
	if(mergeSepSymb && sep.size() == 1)
	    for(an_dir = t_dir; an_dir < (int)path.size() && path[an_dir] == sep[0]; ) an_dir++;
	else an_dir = t_dir+sep.size();
	t_lev++;
    }

    return "";
}

string strLine( const string &str, int level, int *off )
{
    int an_dir = off ? *off : 0;
    int t_lev = 0, edLnSmbSz = 1;
    size_t t_dir;

    if(an_dir >= (int)str.size()) return "";
    while(true)
    {
	for(t_dir = an_dir; t_dir < str.size(); t_dir++)
	    if(str[t_dir] == '\x0D' || str[t_dir] == '\x0A')
	    { edLnSmbSz = (str[t_dir] == '\x0D' && ((t_dir+1) < str.size()) && str[t_dir+1] == '\x0A') ? 2 : 1; break; }
	if(t_dir >= str.size())
	{
	    if(off) *off = str.size();
	    return (t_lev==level) ? str.substr(an_dir) : "";
	}
	else if(t_lev == level)
	{
	    if(off) *off = t_dir+edLnSmbSz;
	    return str.substr(an_dir,t_dir-an_dir);
	}
	an_dir = t_dir+edLnSmbSz;
	t_lev++;
    }
    return "";
}

string strMess( const char *fmt, ... )
{
    char str[10000];
    va_list argptr;

    va_start(argptr, fmt);
    vsnprintf(str, sizeof(str), fmt, argptr);
    va_end(argptr);

    return str;
}

inline uint16_t getUnalign16( const void *p )
{
    struct su16 { uint16_t x; } __attribute__((packed));
    const struct su16 *ptr = (const struct su16 *)p;
    return ptr->x;
}

inline uint32_t getUnalign32( const void *p )
{
    struct su32 { uint32_t x; } __attribute__((packed));
    const struct su32 *ptr = (const struct su32 *)p;
    return ptr->x;
}

inline uint64_t getUnalign64( const void *p )
{
    struct su64 { uint64_t x; } __attribute__((packed));
    const struct su64 *ptr = (const struct su64 *)p;
    return ptr->x;
}

inline float getUnalignFloat( const void *p )
{
    struct sFloat64 { float x; } __attribute__((packed));
    const struct sFloat64 *ptr = (const struct sFloat64 *)p;
    return ptr->x;
}

inline double getUnalignDbl( const void *p )
{
    struct sDbl { double x; } __attribute__((packed));
    const struct sDbl *ptr = (const struct sDbl *)p;
    return ptr->x;
}

float floatLE( float in )
{
#if __BYTE_ORDER == __BIG_ENDIAN
    ieee754_float ieee754_be;
    union ieee754_le
    {
	float f;
	struct
	{
	    unsigned int mantissa:23;
	    unsigned int exponent:8;
	    unsigned int negative:1;
	} ieee;
    } ieee754_le;

    ieee754_be.f = in;
    ieee754_le.ieee.mantissa	= ieee754_be.ieee.mantissa;
    ieee754_le.ieee.exponent	= ieee754_be.ieee.exponent;
    ieee754_le.ieee.negative	= ieee754_be.ieee.negative;

    return ieee754_le.f;
#endif

    return in;
}

float floatLErev(float in)
{
#if __BYTE_ORDER == __BIG_ENDIAN
    ieee754_float ieee754_be;
    union ieee754_le
    {
	float f;
	struct
	{
	    unsigned int mantissa:23;
	    unsigned int exponent:8;
	    unsigned int negative:1;
	} ieee;
    } ieee754_le;

    ieee754_le.f = in;
    ieee754_be.ieee.mantissa	= ieee754_le.ieee.mantissa;
    ieee754_be.ieee.exponent	= ieee754_le.ieee.exponent;
    ieee754_be.ieee.negative	= ieee754_le.ieee.negative;

    return ieee754_be.f;
#endif

    return in;
}

double doubleLE( double in )
{
#if __BYTE_ORDER == __BIG_ENDIAN || __FLOAT_WORD_ORDER == __BIG_ENDIAN
    ieee754_double ieee754_be;
    union ieee754_le
    {
	double d;
	struct
	{
	    unsigned int mantissa1:32;
	    unsigned int mantissa0:20;
	    unsigned int exponent :11;
	    unsigned int negative :1;
	} ieee;
    } ieee754_le;

    ieee754_be.d = in;
    ieee754_le.ieee.mantissa0	= ieee754_be.ieee.mantissa0;
    ieee754_le.ieee.mantissa1	= ieee754_be.ieee.mantissa1;
    ieee754_le.ieee.exponent	= ieee754_be.ieee.exponent;
    ieee754_le.ieee.negative	= ieee754_be.ieee.negative;

    return ieee754_le.d;
#endif

    return in;
}

double doubleLErev( double in )
{
#if __BYTE_ORDER == __BIG_ENDIAN || __FLOAT_WORD_ORDER == __BIG_ENDIAN
    ieee754_double ieee754_be;
    union ieee754_le
    {
	double d;
	struct
	{
	    unsigned int mantissa1:32;
	    unsigned int mantissa0:20;
	    unsigned int exponent:11;
	    unsigned int negative:1;
	} ieee;
    } ieee754_le;

    ieee754_le.d = in;
    ieee754_be.ieee.mantissa0	= ieee754_le.ieee.mantissa0;
    ieee754_be.ieee.mantissa1	= ieee754_le.ieee.mantissa1;
    ieee754_be.ieee.exponent	= ieee754_le.ieee.exponent;
    ieee754_be.ieee.negative	= ieee754_le.ieee.negative;

    return ieee754_be.d;
#endif

    return in;
}

//*************************************************
//* NodeId object				  *
//*************************************************
NodeId::NodeId( uint32_t in, uint16_t ins ) : mNs(ins), mTp(NodeId::Numeric)
{
    setNumbVal(in);
}

NodeId::NodeId( const string &istr, uint16_t ins, NodeId::Type tp ) : mNs(ins), mTp(NodeId::Numeric)
{
    setStrVal(istr, tp);
}

NodeId::~NodeId( )
{
    if(type() != NodeId::Numeric)
    {
#ifdef DEBUG_SPEC
	delete str;
#endif
	mTp = NodeId::Numeric;
    }
}

NodeId &NodeId::operator=( const NodeId &node )
{
    setNs(node.ns());
    switch(node.type())
    {
	case NodeId::Numeric: setNumbVal(node.numbVal());	break;
	case NodeId::String: case NodeId::Guid: case NodeId::Opaque:
	    setStrVal(node.strVal(),node.type());
	    break;
    }
    return *this;
}

uint32_t NodeId::numbVal( ) const
{
    if(type() == NodeId::Numeric) return numb;
#ifdef DEBUG_SPEC
    return strtoul(str->c_str(), NULL, 0);
#else
    return strtoul(str.c_str(), NULL, 0);
#endif
}

string NodeId::strVal( ) const
{
    if(type() == NodeId::Numeric) return uint2str(numb);

#ifdef DEBUG_SPEC
    return *str;
#else
    return str;
#endif
}

void NodeId::setNumbVal( uint32_t in )
{
    if(type() != NodeId::Numeric)
    {
#ifdef DEBUG_SPEC
	delete str;
#endif
	mTp = NodeId::Numeric;
    }
    numb = in;
}

void NodeId::setStrVal( const string &istr, NodeId::Type tp )
{
    if(tp == NodeId::Numeric) return;
#ifdef DEBUG_SPEC
    if(type() == NodeId::Numeric)
    {
	str = new string;
	//Strange allocing equal to previous NodeId object
	printf("TEST 42: str='%s' > alloc=%ph(this=%ph)\n", istr.c_str(), str, this);
    }
    mTp = tp;
    *str = istr;
#else
    mTp = tp;
    str = istr;
#endif
}

NodeId NodeId::fromAddr( const string &strAddr )
{
    int off = 0;
    string vl, dt, rez;
    char bf[3];
    uint16_t ns = strtoul(strParse(strAddr,0,":",&off).c_str(), NULL, 0);
    if(off < (int)strAddr.size()) vl = strAddr.substr(off);
    else { vl = strAddr; ns = 0; }

    //Check for Guid
    if(vl.size() == 38 && vl[0] == '{' && vl[vl.size()-1] == '}' &&
	vl[9] == '-' && vl[14] == '-' && vl[19] == '-' && vl[24] == '-')
    {
	bf[2] = 0;
	// Get Data1
	dt = vl.substr(1,8);
	for(int i_s = (dt.size()-2); i_s >= 0; i_s -= 2)
	{ bf[0] = dt[i_s]; bf[1] = dt[i_s+1]; rez += (char)strtol(bf,NULL,16); }
	// Get Data2
	dt = vl.substr(10,4);
	for(int i_s = (dt.size()-2); i_s >= 0; i_s -= 2)
	{ bf[0] = dt[i_s]; bf[1] = dt[i_s+1]; rez += (char)strtol(bf,NULL,16); }
	// Get Data3
	dt = vl.substr(15,4);
	for(int i_s = (dt.size()-2); i_s >= 0; i_s -= 2)
	{ bf[0] = dt[i_s]; bf[1] = dt[i_s+1]; rez += (char)strtol(bf,NULL,16); }
	// Get Data4a
	dt = vl.substr(20,4);
	for(int i_s = 0; i_s < (int)dt.size(); i_s += 2)
	{ bf[0] = dt[i_s]; bf[1] = dt[i_s+1]; rez += (char)strtol(bf,NULL,16); }
	// Get Data4b
	dt = vl.substr(25,12);
	for(int i_s = 0; i_s < (int)dt.size(); i_s += 2)
	{ bf[0] = dt[i_s]; bf[1] = dt[i_s+1]; rez += (char)strtol(bf,NULL,16); }
	return NodeId(rez, ns, NodeId::Guid);
    }

    //Check for string or opaque
    if(vl.size() >= 2 && vl[0] == '\"' && vl[vl.size()-1] == '\"')
    {
	bf[2] = 0;
	char *endptr = 0;
	rez = "";
	for(unsigned i_s = 1; !(vl.size()%2) && (!endptr || *endptr == 0) && i_s < (vl.size()-1); i_s += 2)
	{ bf[0] = vl[i_s]; bf[1] = vl[i_s+1]; rez += (char)strtol(bf,&endptr,16); }
	if(rez.size() == (vl.size()-2)/2)	return NodeId(rez, ns, NodeId::Opaque);
	return NodeId(vl.substr(1,vl.size()-2), ns);
    }

    //Check for number
    bool isStr = false;
    for(unsigned i_s = 0; i_s < vl.size() && !isStr; i_s++)
	if(!isdigit(vl[i_s])) isStr = true;
    if(isStr) return NodeId(vl,ns);
    return NodeId((uint32_t)strtoul(vl.c_str(),NULL,0), ns);
}

string NodeId::toAddr( ) const
{
    string vl;
    if(ns()) vl = uint2str(ns()) + ":";
    switch(type())
    {
	case NodeId::Numeric:	vl += uint2str(numbVal());	break;
	case NodeId::String:	vl += "\""+strVal()+"\"";	break;
	case NodeId::Guid:
	{
	    vl += "{";
	    //Get Data1
	    string svl = strVal().substr(0,4);
	    for(int i_sz = (svl.size()-1); i_sz >= 0; i_sz--)
		vl += strMess("%0.2x",(unsigned char)svl[i_sz]);
	    //Get Data2
	    vl += "-";
	    svl = strVal().substr(4,2);
	    for(int i_sz = (svl.size()-1); i_sz >= 0; i_sz--)
		vl += strMess("%0.2x",(unsigned char)svl[i_sz]);
	    //Get Data3
	    vl += "-";
	    svl = strVal().substr(6,2);
	    for(int i_sz = (svl.size()-1); i_sz >= 0; i_sz--)
		vl += strMess("%0.2x",(unsigned char)svl[i_sz]);
	    //Get Data4a
	    vl += "-";
	    svl = strVal().substr(8,2);
	    for(int i_sz = 0; i_sz < (int)svl.size(); i_sz++)
		vl += strMess("%0.2x",(unsigned char)svl[i_sz]);
	    //Get Data4b
	    vl += "-";
	    svl = strVal().substr(10,6);
	    for(int i_sz = 0; i_sz < (int)svl.size(); i_sz++)
		vl += strMess("%0.2x",(unsigned char)svl[i_sz]);
	    vl += "}";
	    break;
	}
	case NodeId::Opaque:
	{
	    vl += "\"";
	    string svl = strVal();
	    for(unsigned i_sz = 0; i_sz < svl.size(); i_sz++)
		vl += strMess("%0.2x",(unsigned char)svl[i_sz]);
	    vl += "\"";
	    break;
	}
    }
    return vl;
}

//*************************************************
//* UA OPC object				  *
//*************************************************
UA::UA( )	{ }

UA::~UA( )	{ }

string UA::iErr( const string &buf, int &off )
{
    uint32_t ecod = iNu(buf, off, 4);
    string   estr = iS(buf, off);
    return strMess("0x%x:%s", ecod, estr.c_str());
}

const char *UA::iVal( const string &rb, int &off, char vSz )
{
    off += vSz;
    if(off > (int)rb.size())
	throw OPCError(OpcUa_BadDecodingError, "Buffer size is lesser requested value.");
    return rb.data()+off-vSz;
}

int32_t UA::iN( const string &rb, int &off, char vSz )
{
    off += vSz;
    if(off > (int)rb.size())
	throw OPCError(OpcUa_BadDecodingError, "Buffer size is lesser requested value.");
    switch(vSz)
    {
	case 1: return *(int8_t*)(rb.data()+off-vSz);
	case 2: return (int16_t)getUnalign16(rb.data()+off-vSz);
	case 4: return (int32_t)getUnalign32(rb.data()+off-vSz);
    }
    throw OPCError(OpcUa_BadDecodingError, "Number size '%d' error.", vSz);
}

uint32_t UA::iNu( const string &rb, int &off, char vSz )
{
    off += vSz;
    if(off > (int)rb.size())
	throw OPCError(OpcUa_BadDecodingError, "Buffer size is lesser requested value.");
    switch(vSz)
    {
	case 1: return *(uint8_t*)(rb.data()+off-vSz);
	case 2: return getUnalign16(rb.data()+off-vSz);
	case 4: return getUnalign32(rb.data()+off-vSz);
    }
    throw OPCError(OpcUa_BadDecodingError, "Number size '%d' error.", vSz);
}

double UA::iR( const string &rb, int &off, char vSz )
{
    if(vSz == 4) return floatLErev(getUnalignFloat(iVal(rb,off,vSz)));
    else if(vSz == 8) return doubleLErev(getUnalignDbl(iVal(rb,off,vSz)));
    throw OPCError(OpcUa_BadDecodingError, "Real number size '%d' error.", vSz);
}

string UA::iS( const string &rb, int &off )
{
    int sSz = iN(rb, off, 4); sSz = std::max(0, sSz);
    off += sSz;
    if(off > (int)rb.size())
	throw OPCError(OpcUa_BadDecodingError, "Buffer size is lesser requested string.");
    return rb.substr(off-sSz,sSz);
}

string UA::iSl( const string &rb, int &off, string *locale )
{
    char encMsk = iN(rb, off, 1);
    string sloc;
    if(encMsk & 0x01)
    {
	sloc = iS(rb, off);
	if(locale) *locale = sloc;
    }
    if(encMsk & 0x02) return iS(rb, off);
    return "";//sloc;
}

string UA::iSqlf( const string &rb, int &off, uint16_t *nsIdx )
{
    uint16_t tNsIdx = iNu(rb, off, 2);
    if(nsIdx) *nsIdx = tNsIdx;
    return iS(rb,off);
}

int64_t UA::iTm( const string &rb, int &off )
{
    int64_t tmStamp = (int64_t)getUnalign64(iVal(rb,off,8));
    return (tmStamp/10ll)-11644473600000000ll;
}

NodeId UA::iNodeId( const string &rb, int &off )
{
    off += 1;
    if(off > (int)rb.size())
	throw OPCError(OpcUa_BadDecodingError, "Buffer size is lesser requested NodeId.");
    char enc = rb[off-1];
    switch(enc)
    {
	case 0x00:	//Two Byte
	    return NodeId(iNu(rb,off,1));
	case 0x01:	//Four Byte
	{
	    uint8_t ns = iNu(rb,off,1);
	    return NodeId(iNu(rb,off,2),ns);
	}
	case 0x02:	//Numeric
	{
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(iNu(rb,off,4),ns);
	}
	case 0x03:	//String
	{
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(iS(rb,off),ns);
	}
	case 0x04:	//Guid
	{
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(string(iVal(rb,off,16),16),ns,NodeId::Guid);
	}
	case 0x05:	//Opaque
	{
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(iS(rb,off),ns,NodeId::Opaque);
	}
    }

    throw OPCError(OpcUa_BadDecodingError, "NodeId type %d error or isn't supported.", enc);
}

void UA::iDataValue( const string &buf, int &off, XML_N &nd )
{
    nd.setAttr("Status", "");
    //Data Value
    uint8_t em = iNu(buf, off, 1);			//Encoding Mask
    if(em&0x01)						//Value
    {
	// Variant
	uint8_t emv = iNu(buf, off, 1);			//Encoding Mask
	nd.setAttr("EncodingMask", uint2str(emv));
	int32_t arrL = 1;
	if(emv&0x80)	arrL = iNu(buf, off, 4);	//ArrayLength
	string rezVl;
	for(int i_v = 0; i_v < arrL; i_v++)
	{
	    if(arrL > 1 && i_v) rezVl += "\n";
	    switch(emv&0x3F)
	    {
		case OpcUa_Boolean:
		case OpcUa_SByte:	rezVl += int2str(iN(buf,off,1));	break;
		case OpcUa_Byte:	rezVl += int2str(iNu(buf,off,1));	break;
		case OpcUa_Int16:	rezVl += int2str(iN(buf,off,2));	break;
		case OpcUa_UInt16:	rezVl += uint2str(iNu(buf,off,2));	break;
		case OpcUa_Int32:	rezVl += int2str(iN(buf,off,4));	break;
		case OpcUa_UInt32:	rezVl += uint2str(iNu(buf,off,4));	break;
		case OpcUa_Int64:	rezVl += strMess("%lld", (int64_t)getUnalign64(iVal(buf,off,8)));	break;
		case OpcUa_UInt64:	rezVl += strMess("%llu", getUnalign64(iVal(buf,off,8)));	break;
		case OpcUa_Float:	rezVl += real2str(iR(buf,off,4));	break;
		case OpcUa_Double:	rezVl += real2str(iR(buf,off,8));	break;
		case OpcUa_String:
		case OpcUa_ByteString:	rezVl += iS(buf,off);	break;
		case OpcUa_NodeId:	rezVl += iNodeId(buf,off).toAddr();	break;
		case OpcUa_StatusCode:	rezVl += strMess("0x%x",iNu(buf,off,4));	break;
		case OpcUa_QualifiedName:
		{
		    uint16_t ns;
		    string vl = iSqlf(buf,off,&ns);
		    rezVl += uint2str(ns)+":"+vl;
		    break;
		}
		case OpcUa_LocalizedText:
		{
		    string loc, vl;
		    vl = iSl(buf, off, &loc);
		    rezVl += loc+":"+vl;
		    break;
		}
		default: throw OPCError(OpcUa_BadDecodingError, "Data type '%d' isn't supported.", emv&0x3F);
	    }
	}
	nd.setText(rezVl);
	// ArrayDimension
	if(emv&0x40) throw OPCError(OpcUa_BadDecodingError, "ArrayDimensions field isn't supported.");
	//????
    }
    if(em&0x02)	nd.setAttr("Status", strMess("0x%x",iNu(buf,off,4)));
    if(em&0x04)	nd.setAttr("SourceTimestamp", ll2str(iTm(buf,off)));
    if(em&0x10)	nd.setAttr("SourcePicoseconds", uint2str(iNu(buf,off,2)));
    if(em&0x08)	nd.setAttr("ServerTimestamp", ll2str(iTm(buf,off)));
    if(em&0x20)	nd.setAttr("ServerPicoseconds", uint2str(iNu(buf,off,2)));
}

void UA::oN( string &buf, int32_t val, char sz, int off )
{
    if(off < 0 || (off+sz) > (int)buf.size()) buf.append((char*)&val, sz);
    else buf.replace(off, sz, (char*)&val, sz);
}

void UA::oNu( string &buf, uint32_t val, char sz, int off )
{
    if(off < 0 || (off+sz) > (int)buf.size()) buf.append((char*)&val, sz);
    else buf.replace(off, sz, (char*)&val, sz);
}

void UA::oR( string &buf, double val, char sz )
{
    if(sz == 4)		{ float vl = floatLE(val); buf.append((char*)&vl, sz); }
    else if(sz == 8)	{ val = doubleLE(val); buf.append((char*)&val, sz); }
    else throw OPCError(OpcUa_BadEncodingError, "Real number size '%d' error.", sz);
}

void UA::oS( string &buf, const string &val, int off )
{
    int prevSz = 0;
    bool toRepl = (off >= 0 && (off+4) <= (int)buf.size() && (off+4+(prevSz=getUnalign32(buf.data()+off)) <= (int)buf.size()));
    oN(buf, (val.size()?(int)val.size():-1), 4, (toRepl?off:-1));
    if(toRepl) buf.replace(off+4, std::max(0,prevSz), val);
    else buf.append(val);
}

void UA::oSl( string &buf, const string &val, const string &locale )
{
    oN(buf, (locale.empty()?0:0x01)|(val.empty()?0:0x02), 1);
    if(!locale.empty())	oS(buf, locale);
    if(!val.empty())	oS(buf, val);
}

void UA::oSqlf( string &buf, const string &val, uint16_t nsIdx )
{
    oN(buf, nsIdx, 2);
    oS(buf, val);
}

void UA::oNodeId( string &buf, const NodeId &val )
{
    switch(val.type())
    {
	case NodeId::Numeric:
	    if(val.ns() == 0 && val.numbVal() <= 255)
	    {
		buf += (char)0x00;
		buf += (char)val.numbVal();
	    }
	    else if(val.ns() <= 255 && val.numbVal() <= 65535)
	    {
		buf += (char)0x01;
		buf += (char)val.ns();
		oNu(buf, val.numbVal(), 2);
	    }
	    else
	    {
		buf += (char)0x02;
		oNu(buf, val.ns(), 2);
		oNu(buf, val.numbVal(), 4);
	    }
	    break;
	case NodeId::String:
	    buf += (char)0x03;
	    oNu(buf, val.ns(), 2);
	    oS(buf, val.strVal());
	    break;
	case NodeId::Guid:
	    buf += (char)0x04;
	    oNu(buf, val.ns(), 2);
	    buf += val.strVal().substr(0, 16);
	    break;
	case NodeId::Opaque:
	    buf += (char)0x05;
	    oNu(buf, val.ns(), 2);
	    oS(buf, val.strVal());
	    break;
    }
}

void UA::oTm( string &buf, int64_t val )
{
    int64_t tmStamp = 10ll*(val+11644473600000000ll);
    buf.append((char*)&tmStamp, sizeof(tmStamp));
}

void UA::oRef( string &buf, uint32_t resMask, const NodeId &nodeId, const NodeId &refTypeId,
    bool isForward, const string &name, uint32_t nodeClass, const NodeId &typeDef )
{
    if(resMask&RdRm_RefType) oNodeId(buf, refTypeId);		else oNodeId(buf,0);
    if(resMask&RdRm_IsForward) oNu(buf, isForward, 1);		else oNu(buf,0,1);
    oNodeId(buf, nodeId);
    if(resMask&RdRm_BrowseName) oSqlf(buf, name, nodeId.ns());	else oSqlf(buf, "");
    if(resMask&RdRm_DisplayName) oSl(buf, name, "en");		else oSl(buf, "");
    if(resMask&RdRm_NodeClass) oNu(buf, nodeClass, 4);		else oNu(buf, 0, 4);
    if(resMask&RdRm_TypeDef) oNodeId(buf, typeDef);		else oNodeId(buf, 0);
}

void UA::oDataValue( string &buf, uint8_t eMsk, const string &vl, uint8_t vEMsk, int64_t srcTmStmp )
{
    eMsk = eMsk&(~0x30);	//Exclude picoseconds parts
    if(eMsk&0x02) eMsk = eMsk&(~0x01);

    oNu(buf, eMsk, 1);		//Encoding Mask
    if(eMsk&0x01)		//Variant
    {
	oNu(buf, vEMsk, 1);	//Encoding Mask
	int32_t arrL = 1;
	if(vEMsk&0x80)		//Array process
	{
	    arrL = 0;
	    for(int off = 0; strParse(vl,0,"\n",&off).size(); ) arrL++;
	    oNu(buf, arrL, 4);	//ArrayLength
	}
	for(int i_v = 0, off = 0; i_v < arrL; i_v++)
	{
	    string setVl = (arrL==1) ? vl : strParse(vl,0,"\n",&off);
	    switch(vEMsk&0x3F)
	    {
		case OpcUa_Boolean:
		case OpcUa_SByte:	oN(buf, atoi(setVl.c_str()), 1);	break;
		case OpcUa_Byte:	oNu(buf, atoi(setVl.c_str()), 1);	break;
		case OpcUa_Int16:	oN(buf, atoi(setVl.c_str()), 2);	break;
		case OpcUa_UInt16:	oNu(buf, atoi(setVl.c_str()), 2);	break;
		case OpcUa_Int32:	oN(buf, atoi(setVl.c_str()), 4);	break;
		case OpcUa_UInt32:	oNu(buf, strtoul(setVl.c_str(),NULL,10), 4);	break;
		case OpcUa_Int64:	oN(buf, strtoll(setVl.c_str(),NULL,10), 8);	break;
		case OpcUa_UInt64:	oNu(buf, strtoull(setVl.c_str(),NULL,10), 8);	break;
		case OpcUa_Float:	oR(buf, atof(setVl.c_str()), 4);	break;
		case OpcUa_Double:	oR(buf, atof(setVl.c_str()), 8);	break;
		case OpcUa_String:
		case OpcUa_ByteString:	oS(buf, setVl);				break;
		case OpcUa_NodeId:	oNodeId(buf, NodeId::fromAddr(setVl));	break;
		case OpcUa_StatusCode:	oNu(buf, strtoll(setVl.c_str(),NULL,0), 4);	break;
		case OpcUa_QualifiedName: oSqlf(buf, setVl);			break;
		case OpcUa_LocalizedText: oSl(buf, setVl, lang2CodeSYS());	break;
		default: //oS(buf, setVl);	break;
		    throw OPCError(OpcUa_BadDecodingError, "Data type '%d' isn't supported.", vEMsk&0x3F);
	    }
	}
	//ArrayDimension
	if(vEMsk&0x40) throw OPCError(OpcUa_BadDecodingError, "ArrayDimensions field isn't supporteded.");
	//????
    }
    if(eMsk&0x02) oN(buf, strtoul(vl.c_str(),NULL,10), 4);	//Status
    if(eMsk&0x04) oTm(buf, srcTmStmp ? srcTmStmp : curTime());	//SourceTimestamp
    if(eMsk&0x08) oTm(buf, curTime());				//ServerTimestamp
}

string UA::randBytes( int num )
{
    unsigned char buf[num];
    if(RAND_bytes(buf,num) != 1)
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	throw OPCError("randBytes error: %s", err);
    }
    return string((char*)buf,num);
}

string UA::certPEM2DER( const string &spem )
{
    int len = 0;
    BIO *bm = NULL;
    X509 *x = NULL;
    string rez = "";

    if(spem.empty())	return rez;
    bm = BIO_new(BIO_s_mem());
    if(bm && BIO_write(bm,spem.data(),spem.size()) == (int)spem.size())
	x = PEM_read_bio_X509_AUX(bm,NULL,NULL,NULL);
    if(x) len = i2d_X509(x,NULL);
    if(len > 0)
    {
	char *buf = (char*)malloc(len);
	unsigned char *p = (unsigned char *)buf;
	if(buf)
	{
	    i2d_X509(x, &p);
	    rez.assign(buf, len);
	    free(buf);
	}
    }
    //Free temporary data
    if(bm) BIO_free(bm);
    if(x)  X509_free(x);

    if(len <= 0)
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	throw OPCError("certPEM2DER error: %s", err);
    }

    return rez;
}

string UA::certDER2PEM( const string &certDer )
{
    BIO *bm = NULL;
    X509 *x = NULL;
    string rez = "";

    if(certDer.empty()) return rez;
#if OPENSSL_VERSION_NUMBER >= 0x00908000L
    const unsigned char *ind = (const unsigned char *)certDer.data();
#else
    unsigned char *ind = (unsigned char *)certDer.data();
#endif
    x = d2i_X509(NULL, &ind, certDer.size());
    if(x) bm = BIO_new(BIO_s_mem());
    if(bm && PEM_write_bio_X509(bm,x) > 0)
    {
	char buf[4000];
	for(int i_r = 0; (i_r=BIO_read(bm,buf,sizeof(buf))) > 0; ) rez.append(buf, i_r);
    }
    //Free temporary data
    if(bm) BIO_free(bm);
    if(x)  X509_free(x);

    if(rez.empty())
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	throw OPCError("certDER2PEM error: %s", err);
    }

    return rez;
}

string UA::certThumbprint( const string &spem )
{
    BIO *bm = NULL;
    X509 *x = NULL;
    unsigned int n = 0;
    unsigned char md[EVP_MAX_MD_SIZE];

    if(spem.empty()) return "";
    bm = BIO_new(BIO_s_mem());
    if(bm && BIO_write(bm,spem.data(),spem.size()) == (int)spem.size())
	x = PEM_read_bio_X509_AUX(bm, NULL, NULL, NULL);
    if(x) X509_digest(x, EVP_sha1() ,md, &n);

    //Free temporary data
    if(bm) BIO_free(bm);
    if(x)  X509_free(x);

    if(n <= 0)
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	throw OPCError("certThumbprint: %s", err);
    }

    return string((char*)md, n);
}

string UA::asymmetricEncrypt( const string &mess, const string &certPem, const string &secPolicy )
{
    int keysize = 0;
    string rez;
    X509 *x = NULL;
    BIO *bm = NULL;
    EVP_PKEY *pkey = NULL;
    RSA *rsa = NULL;

    int paddSize = 11, padd = RSA_PKCS1_PADDING;
    if(secPolicy.find("Rsa15") == string::npos) { paddSize = 42; padd = RSA_PKCS1_OAEP_PADDING; }

    if(!certPem.empty() && !mess.empty()) bm = BIO_new(BIO_s_mem());
    if(bm && BIO_write(bm,certPem.data(),certPem.size()) == (int)certPem.size());
	x = PEM_read_bio_X509_AUX(bm, NULL, NULL, NULL);
    if(x)	pkey = X509_get_pubkey(x);
    if(pkey)	rsa = EVP_PKEY_get1_RSA(pkey);
    if(rsa)	keysize = RSA_size(rsa);
    if(keysize && !(mess.size()%(keysize-paddSize)))
    {
	unsigned char rsaOut[keysize];
	for(unsigned i_b = 0; i_b < mess.size()/(keysize-paddSize); i_b++)
	{
	    int blen = RSA_public_encrypt((keysize-paddSize),
		(const unsigned char *)(mess.data()+i_b*(keysize-paddSize)), rsaOut, rsa, padd);
	    if(blen <= 0) break;
	    rez.append((char*)rsaOut, blen);
	}
    }
    //Free temporary data
    if(pkey)	EVP_PKEY_free(pkey);
    if(bm)	BIO_free(bm);
    if(rsa)	RSA_free(rsa);
    if(x)	X509_free(x);

    if(rez.empty())
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	throw OPCError("asymmetricEncrypt: %s", err);
    }

    return rez;
}

string UA::asymmetricDecrypt( const string &mess, const string &keyPem, const string &secPolicy )
{
    int keysize = 0;
    BIO *bm = NULL;
    RSA *rsa = NULL;
    EVP_PKEY *pkey = NULL;
    string rez = "";

    if(!keyPem.empty() && !mess.empty()) bm = BIO_new(BIO_s_mem());
    if(bm && BIO_write(bm,keyPem.data(),keyPem.size()) == (int)keyPem.size());
	pkey = PEM_read_bio_PrivateKey(bm, NULL, 0, (char*)"keypass");
    if(pkey)	rsa = EVP_PKEY_get1_RSA(pkey);
    if(rsa)	keysize = RSA_size(rsa);
    if(keysize && !(mess.size()%keysize))
    {
	unsigned char rsaOut[keysize];
	for(unsigned i_b = 0; i_b < mess.size()/keysize; i_b++)
	{
	    int blen = RSA_private_decrypt(keysize, (const unsigned char *)(mess.data()+i_b*keysize), rsaOut, rsa,
		((secPolicy.find("Rsa15") == string::npos) ? RSA_PKCS1_OAEP_PADDING : RSA_PKCS1_PADDING));
	    if(blen <= 0) break;
	    rez.append((char*)rsaOut, blen);
	}
    }

    //Free temporary data
    if(pkey)	EVP_PKEY_free(pkey);
    if(bm)	BIO_free(bm);
    if(rsa)	RSA_free(rsa);

    if(rez.empty())
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	throw OPCError("asymmetricDecrypt: %s", err);
    }

    return rez;
}

bool UA::asymmetricVerify( const string &mess, const string &sign, const string &certPem )
{
    int rez = -1;
    X509 *x = NULL;
    BIO *bm = NULL, *mdtmp = NULL;
    EVP_PKEY *pkey = NULL;
    EVP_MD_CTX *ctx;

    if(!certPem.empty() && !sign.empty() && !mess.empty()) bm = BIO_new(BIO_s_mem());
    if(bm && BIO_write(bm,certPem.data(),certPem.size()) == (int)certPem.size());
	x = PEM_read_bio_X509_AUX(bm, NULL, NULL, NULL);
    if(x) pkey = X509_get_pubkey(x);
    if(pkey && (int)sign.size() == EVP_PKEY_size(pkey)) mdtmp = BIO_new(BIO_f_md());
    if(mdtmp)
    {
	BIO_set_md(mdtmp, EVP_sha1());
	mdtmp = BIO_push(mdtmp, bm);
	if(BIO_write(mdtmp,mess.data(),mess.size()) == (int)mess.size())
	{
	    BIO_get_md_ctx(mdtmp, &ctx);
	    if(ctx) rez = EVP_VerifyFinal(ctx, (unsigned char*)sign.data(), sign.size(), pkey);
	}
    }

    //Free temporary data
    if(x)	X509_free(x);
    if(mdtmp)	BIO_free(mdtmp);
    if(bm)	BIO_free(bm);
    if(pkey)	EVP_PKEY_free(pkey);

    if(rez == -1)
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	throw OPCError("asymmetricVerify: %s", err);
    }
    return (rez == 1);
}

string UA::asymmetricSign( const string &mess, const string &pvPem )
{
    BIO *bm = NULL, *mdtmp = NULL;
    EVP_PKEY *pkey = NULL;
    EVP_MD_CTX *ctx;
    unsigned char buf[10000];
    unsigned int blen = 0;

    if(!pvPem.empty() && !mess.empty()) bm = BIO_new(BIO_s_mem());
    if(bm && BIO_write(bm,pvPem.data(),pvPem.size()) == (int)pvPem.size());
	pkey = PEM_read_bio_PrivateKey(bm, NULL, 0, (char*)"keypass");
    if(pkey) mdtmp = BIO_new(BIO_f_md());
    if(mdtmp)
    {
	BIO_set_md(mdtmp, EVP_sha1());
	mdtmp = BIO_push(mdtmp, bm);
	if(BIO_write(mdtmp,mess.data(),mess.size()) == (int)mess.size())
	{
	    BIO_get_md_ctx(mdtmp, &ctx);
	    if(ctx) EVP_SignFinal(ctx, buf, &blen, pkey);
	}
    }

    //Free temporary data
    if(mdtmp)	BIO_free(mdtmp);
    if(bm)	BIO_free(bm);
    if(pkey)	EVP_PKEY_free(pkey);

    if(!blen)
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	throw OPCError("asymmetricSign: %s", err);
    }

    return string((char*)buf, blen);
}

int UA::asymmetricKeyLength( const string &keyCertPem )
{
    int rez = -1;
    X509 *x = NULL;
    BIO *bm = NULL;
    EVP_PKEY *pkey = NULL;

    if(!keyCertPem.empty()) bm = BIO_new(BIO_s_mem());
    if(bm && BIO_write(bm,keyCertPem.data(),keyCertPem.size()) == (int)keyCertPem.size());
	x = PEM_read_bio_X509_AUX(bm,NULL,NULL,NULL);
    if(x)	pkey = X509_get_pubkey(x);
    if(pkey)	rez = EVP_PKEY_size(pkey);

    //Free temporary data
    if(x)	X509_free(x);
    if(bm)	BIO_free(bm);
    if(pkey)	EVP_PKEY_free(pkey);

    if(rez == -1)
    {
	char err[255];
	ERR_error_string_n(ERR_peek_last_error(), err, sizeof(err));
	throw OPCError("keyCertPem: %s", err);
    }

    return rez;
}

string UA::deriveKey( const string &secret, const string &seed, int keyLen )
{
    int hashCnt = (keyLen+19)/20;
    unsigned char hashRez[20*hashCnt], hashTmp[20+seed.size()];

    memcpy(hashTmp+20, seed.data(), seed.size());
    HMAC(EVP_sha1(), secret.data(), secret.size(), (const unsigned char*)seed.data(), seed.size(), hashTmp, 0);
    for(int i_c = 0; i_c < hashCnt; i_c++)
    {
	HMAC(EVP_sha1(), secret.data(), secret.size(), hashTmp, 20+seed.size(), hashRez+20*i_c, 0);
	HMAC(EVP_sha1(), secret.data(), secret.size(), hashTmp, 20, hashTmp, 0);
    }

    return string((const char*)hashRez, keyLen);
}

string UA::symmetricEncrypt( const string &mess, const string &keySet, const string &secPolicy )
{
    if(mess.empty() || keySet.size() < (3*16)) return "";
    int keySize = keySet.size()/3;
    int signSize = keySize;
    if(secPolicy == "Basic256") signSize = 24;

    unsigned char obuf[mess.size()], ivecb[keySize];

    AES_KEY key;
    AES_set_encrypt_key((const unsigned char*)(keySet.data()+signSize), keySize*8, &key);
    memcpy(ivecb, keySet.data()+signSize+keySize, keySize);
    AES_cbc_encrypt((const unsigned char*)mess.data(), obuf, mess.size(), &key, ivecb, 1);

    return string((char*)obuf, mess.size());
}

string UA::symmetricDecrypt( const string &mess, const string &keySet, const string &secPolicy )
{
    if(mess.empty() || keySet.size() < (3*16)) return "";
    int keySize = keySet.size()/3;
    int signSize = keySize;
    if(secPolicy == "Basic256") signSize = 24;

    unsigned char obuf[mess.size()], ivecb[keySize];

    AES_KEY key;
    AES_set_decrypt_key((const unsigned char*)(keySet.data()+signSize), keySize*8, &key);
    memcpy(ivecb, keySet.data()+signSize+keySize, keySize);
    AES_cbc_encrypt((const unsigned char*)mess.data(), obuf, mess.size(), &key, ivecb, 0);

    return string((char*)obuf, mess.size());
}

string UA::symmetricSign( const string &mess, const string &keySet, const string &secPolicy )
{
    unsigned char hashRez[20];
    int keySize = keySet.size()/3;
    int signSize = keySize;
    if(secPolicy == "Basic256") signSize = 24;

    HMAC(EVP_sha1(), keySet.data(), signSize, (const unsigned char*)mess.data(), mess.size(), hashRez, 0);
    return string((const char*)hashRez, 20);
}

//*************************************************
//* Protocol OPC UA client part			  *
//*************************************************
Client::Client( )	{ }

Client::~Client( )	{ }

void Client::protIO( XML_N &io )
{
    string rez, err;
    char buf[1000];
    int stIdx = 0;

    try
    {
	bool debug = (bool)atoi(io.attr("debug").c_str());
	if(io.name() == "opc.tcp")
	{
	    if(io.attr("id") == "HEL")
	    {
		rez = "HELF";				//> HELLO message type
		rez.reserve(50);
		oNu(rez, 0, 4);				//Message size
		oNu(rez, OpcUa_ProtocolVersion, 4);	//Protocol version
		oNu(rez, OpcUa_ReciveBufferSize, 4);	//Recive buffer size
		oNu(rez, OpcUa_SendBufferSize, 4);	//Send buffer size
		oNu(rez, OpcUa_MaxMessageSize, 4);	//Max message size
		oNu(rez, OpcUa_MaxChunkCount, 4);	//Max chunk count
		oS(rez, io.attr("EndPoint"));		//EndpointURL
		oNu(rez, rez.size(), 4, 4);		//Real message size

		if(debug) debugMess(strMess("HELLO Req(%d): ep='%s'",rez.size(),io.attr("EndPoint").c_str()));

		//Send request
		int resp_len = messIO(rez.data(), rez.size(), buf, sizeof(buf));
		rez.assign(buf, resp_len);

		if(debug) debugMess(strMess("HELLO Resp(%d)",rez.size()));

		int off = 4;
		if(rez.size() < 8 || rez.size() > 4096 || iNu(rez,off,4) != rez.size())
		    err = strMess("0x%x:%s", OpcUa_BadTcpMessageTooLarge, "Respond size is not coincidence.");
		else if(rez.compare(0,4,"ERRF") == 0) err = iErr(rez,off);
		else if(rez.compare(0,4,"ACKF") != 0)
		    err = strMess("0x%x:%s", OpcUa_BadTcpMessageTypeInvalid, "Respond don't acknowledge.");
		else
		{
		    iNu(rez, off, 4);			//Protocol version
		    iNu(rez, off, 4);			//Recive buffer size
		    iNu(rez, off, 4);			//Send buffer size
		    iNu(rez, off, 4);			//Max message size
		    iNu(rez, off, 4);			//Max chunk count
		}
	    }
	    else if(io.attr("id") == "OPN")
	    {
		bool isSecNone = false;
		int symKeySz = 0, asymKeyPad = 0;
		string secPlc = io.attr("SecPolicy");
		if(secPlc == "None")			isSecNone = true;
		else if(secPlc == "Basic128Rsa15")	{ symKeySz = 16; asymKeyPad = 11; }
		else if(secPlc == "Basic256")		{ symKeySz = 32; asymKeyPad = 42; }
		else throw OPCError(OpcUa_BadSecurityPolicyRejected, "");

		rez = "OPNF";				//OpenSecureChannel message type
		rez.reserve(200);
		oNu(rez, 0, 4);				//Message size
		oNu(rez, strtoul(io.attr("SecChnId").c_str(),NULL,10), 4);	//Secure channel identifier
							//> Security Header
		oS(rez, "http://opcfoundation.org/UA/SecurityPolicy#"+secPlc);	//Security policy URI
		oS(rez, certPEM2DER(io.attr("ClntCert")));	//ClientCertificate
		oS(rez, certThumbprint(io.attr("ServCert")));	//ServerCertificateThumbprint
		uint32_t begEncBlck = rez.size();
							//> Sequence header
		oNu(rez, strtoul(io.attr("SeqNumber").c_str(),NULL,10), 4);	//Sequence number
		oNu(rez, strtoul(io.attr("SeqReqId").c_str(),NULL,10), 4);	//RequestId
							//> Extension body object
		oNodeId(rez, NodeId(OpcUa_OpenSecureChannelRequest));	//TypeId
							//>> Request Header
		oNodeId(rez, 0);			//Session AuthenticationToken
		oTm(rez, curTime());			//timestamp
		oN(rez, atoi(io.attr("ReqHandle").c_str()),4);	//requestHandle
		oNu(rez, 0, 4);				//returnDiagnostics
		oS(rez, "");				//auditEntryId
		oNu(rez, 0, 4);				//timeoutHint
							//>>> Extensible parameter
		oNodeId(rez, 0);				//TypeId (0)
		oNu(rez, 0, 1);				//Encoding
							//>>>> Standard request
		oNu(rez, OpcUa_ProtocolVersion, 4);	//ClienUAocolVersion
		oNu(rez, atoi(io.attr("ReqType").c_str()), 4);	//RequestType
		oN(rez, atoi(io.attr("SecurityMode").c_str()), 4);	//SecurityMode
		string clNonce = isSecNone ? string("\000",1) : randBytes(symKeySz);
		oS(rez, clNonce);			//ClientNonce
		oN(rez, atoi(io.attr("SecLifeTm").c_str()), 4);	//RequestedLifetime
		oNu(rez, rez.size(), 4, 4);		//> Real message size

		if(!isSecNone)
		{
		    // Padding place
		    int kSz = asymmetricKeyLength(io.attr("ServCert")),
			signSz = asymmetricKeyLength(io.attr("ClntCert")),
			encSz = (rez.size() - begEncBlck) + 1 + signSz,
			paddingSize = (encSz/(kSz-asymKeyPad) + (encSz%(kSz-asymKeyPad)?1:0))*(kSz-asymKeyPad) - encSz;
		    rez += string(paddingSize+1, char(paddingSize));
		    // Real message size calc and place
		    oNu(rez, begEncBlck + kSz*((rez.size()-begEncBlck+signSz)/(kSz-asymKeyPad)), 4, 4);
		    // Signature
		    rez += asymmetricSign(rez, io.attr("PvKey"));
		    // Encoding
		    if(debug) debugMess("OPN Req (decoded)");
		    rez.replace(begEncBlck, rez.size()-begEncBlck, asymmetricEncrypt(rez.substr(begEncBlck),io.attr("ServCert"),secPlc));
		}
		if(debug) debugMess("OPN Req");
		//Send request and wait respond
		int resp_len = messIO(rez.data(), rez.size(), buf, sizeof(buf));
		rez.assign(buf, resp_len);
		int off = 4;
		for( ; rez.size() < 8 || rez.size() < iNu(rez,off,4); off = 4)
		{
		    resp_len = messIO(NULL, 0, buf, sizeof(buf));
		    if(!resp_len) throw OPCError(OpcUa_BadCommunicationError, "Not full respond.");
		    rez.append(buf, resp_len);
		}

		off = 4;
		if(rez.size() < 8 || iNu(rez,off,4) != rez.size())
		    err = strMess("0x%x:%s", OpcUa_BadTcpMessageTooLarge, "Respond size is not coincidence.");
		else if(rez.compare(0,4,"ERRF") == 0) err = iErr(rez,off);
		else if(rez.compare(0,4,"OPNF") != 0)
		    err = strMess("0x%x:%s", OpcUa_BadTcpMessageTypeInvalid, "Respond don't acknowledge.");
		else
		{
		    if(debug) debugMess("OPN Resp");
		    iNu(rez, off, 4);					//Secure channel identifier
		    iS(rez, off);					//Security policy URI
		    string servCert = iS(rez, off);			//ServerCertificate
		    string clntCertThmb = iS(rez, off);			//ClientCertificateThumbprint

		    if(!isSecNone)
		    {
			if(clntCertThmb != certThumbprint(io.attr("ClntCert")))
			    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Client certificate thumbprint error.");
			// Decoding
			rez.replace(off, rez.size()-off, asymmetricDecrypt(rez.substr(off),io.attr("PvKey"),secPlc));
		    }

		    iNu(rez, off, 4);					//Sequence number
		    iNu(rez, off, 4);					//RequestId
									//> Extension Object
		    if(iNodeId(rez,off).numbVal() != OpcUa_OpenSecureChannelResponse)	//TypeId
			throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Respond NodeId don't acknowledge");
									//>> Body
									//>>> RespondHeader
		    iTm(rez, off);					//timestamp
		    iN(rez, off, 4);					//requestHandle
		    iN(rez, off, 4);					//StatusCode
		    iN(rez, off, 1);					//serviceDiagnostics
		    iS(rez, off);					//stringTable
									//>>> Extensible parameter
		    iNodeId(rez, off);					//TypeId (0)
		    iNu(rez, off, 1);					//Encoding
									//>>>> Standard respond
		    iNu(rez, off, 4);					//ServerProtocolVersion
		    io.setAttr("SecChnId", uint2str(iNu(rez,off,4)));	//Secure channel identifier
		    io.setAttr("SecTokenId", uint2str(iNu(rez,off,4)));	//TokenId
		    iTm(rez, off);					//CreatedAt
		    io.setAttr("SecLifeTm", int2str(iN(rez,off,4)));	//RevisedLifeTime
		    string servNonce = iS(rez, off);			//nonce
		    // Signature
		    if(!isSecNone)
		    {
			io.setAttr("clKey", deriveKey(clNonce,servNonce,symKeySz*3));
			io.setAttr("servKey", deriveKey(servNonce,clNonce,symKeySz*3));
			off += iNu(rez, off, 1);			//Pass padding
			if(!asymmetricVerify(rez.substr(0,off),rez.substr(off),io.attr("ServCert")))	//Check Signature
			    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Signature error");
		    }
		}
	    }
	    else if(io.attr("id") == "CLO")
	    {
		rez = "CLOF";						//OpenSecureChannel close
		rez.reserve(200);
		oNu(rez, 0, 4);						//Message size
		oNu(rez, strtoul(io.attr("SecChnId").c_str(),NULL,10), 4);	//Secure channel identifier
		oNu(rez, strtoul(io.attr("SecTokenId").c_str(),NULL,10), 4);	//TokenId
		int begEncBlck = rez.size();
									//> Sequence header
		uint32_t SeqNumber = strtoul(io.attr("SeqNumber").c_str(),NULL,10)+1;
		uint32_t SeqReqId = strtoul(io.attr("SeqReqId").c_str(),NULL,10)+1;
		oNu(rez, SeqNumber, 4);					//Sequence number
		oNu(rez, SeqReqId, 4);					//RequestId
									//> Extension body object
		oNodeId(rez, OpcUa_CloseSecureChannelRequest);		//TypeId request: CloseSecureChannel
									//>> Request Header
		oNodeId(rez, 0);					//Session AuthenticationToken
		oTm(rez, curTime());					//timestamp
		oN(rez, 0, 4);						//requestHandle
		oNu(rez, 0, 4);						//returnDiagnostics
		oS(rez, "");						//auditEntryId
		oNu(rez, 0, 4);						//timeoutHint
									//>>> Extensible parameter
		oNodeId(rez, 0);					//TypeId (0)
		oNu(rez, 0, 1);						//Encoding
		oNu(rez,rez.size(), 4, 4);				//> Real message size
		string secPolicy = io.attr("SecPolicy");
		char secMessMode = atoi(io.attr("SecurityMode").c_str());
		if(secMessMode == MS_Sign || secMessMode == MS_SignAndEncrypt)
		{
		    string servKey = io.attr("servKey");
		    // Padding place
		    if(secMessMode == MS_SignAndEncrypt)
		    {
			int kSz = servKey.size()/3;
			int paddingSize = ((rez.size()-begEncBlck+1+20+kSz-1)/kSz)*kSz-(rez.size()-begEncBlck+20);
			rez += string(paddingSize, (char)(paddingSize-1));
		    }
		    // Real message size calc and place
		    oNu(rez, rez.size()+20, 4, 4);
		    // Signature
		    rez += symmetricSign(rez, servKey, secPolicy);
		    // Encoding
		    if(secMessMode == MS_SignAndEncrypt)
			rez.replace(begEncBlck, rez.size()-begEncBlck, symmetricEncrypt(rez.substr(begEncBlck),servKey,secPolicy));
		}

		//Parameters clear
		io.setAttr("SecChnId", ""); io.setAttr("SecTokenId", "");
		io.setAttr("SeqNumber", ""); io.setAttr("SeqReqId", "");

		if(debug) debugMess("CLO Req");

		//Send request and don't wait response
		messIO(rez.data(), rez.size(), NULL, 0);
	    }
	    else
	    {
		nextReq:
		int iTpId = 0;
		string mReq;
		if(io.attr("id") == "FindServers")
		{
		    iTpId = OpcUa_FindServersRequest;
		    oS(mReq, io.attr("EndPoint"));			//endpointUrl
		    oS(mReq, "");					//localeIds []
		    oS(mReq, "");					//serverUris []
		}
		else if(io.attr("id") == "GetEndpoints")
		{
		    iTpId = OpcUa_GetEndpointsRequest;
		    oS(mReq, io.attr("EndPoint"));			//endpointUrl
		    oS(mReq, "");					//localeIds []
		    oS(mReq, "");					//profileUris []
		}
		else if(io.attr("id") == "CreateSession")
		{
		    iTpId = OpcUa_CreateSessionRequest;
									//> clientDescription (Application Description)
		    oS(mReq, "");					//applicationUri
		    oS(mReq, "");					//productUri
		    oSl(mReq, "");					//applicationName
		    oNu(mReq, 1, 4);					//applicationType (CLIENT_1)
		    oS(mReq, "");					//gatewayServerUri
		    oS(mReq, "");					//discoveryProfileUri
		    oS(mReq, "");					//discoveryUrls

		    oS(mReq, "");					//serverUri
		    oS(mReq, io.attr("EndPoint"));			//endpointUrl
		    oS(mReq, sessionName());				//sessionName
		    io.setAttr("Nonce", randBytes(32));			//???? check for policy
		    oS(mReq, io.attr("Nonce"));				//clientNonce
		    oS(mReq, certPEM2DER(io.childGet("ClientCert")->text()));	//clientCertificate
		    oR(mReq, atof(io.attr("sesTm").c_str()), 8);	//Requested SessionTimeout, ms
		    oNu(mReq, 0x1000000, 4);				//maxResponse MessageSize
		}
		else if(io.attr("id") == "ActivateSession")
		{
		    iTpId = OpcUa_ActivateSessionRequest;
									//> clientSignature
		    if(io.attr("SecPolicy") == "None")
		    {
			oS(mReq, "");					//algorithm
			oS(mReq, "");					//signature
		    }
		    else
		    {
			oS(mReq, "http://www.w3.org/2000/09/xmldsig#rsa-sha1");	//algorithm
			oS(mReq, asymmetricSign(certPEM2DER(io.attr("ServCert"))+io.attr("servNonce"),io.attr("PvKey")));	//signature
		    }

		    oNu(mReq, 0, 4);					//clientSoftwareCertificates []
									//> localeIds []
		    oNu(mReq, 1, 4);					//List number 1
		    oS(mReq, "en");					//localeId
									//> userIdentityToken
		    if(authData().empty())
		    {
			oNodeId(mReq, OpcUa_AnonymousIdentityToken);	//TypeId
			oNu(mReq, 1, 1);				//Encode
			oNu(mReq, 4+10, 4);				//Length
			oS(mReq, "anonPolicy");				//policyId: force to anonPolicy
		    }
		    else
		    {
			oNodeId(mReq, OpcUa_UserNameIdentityToken);	//TypeId
			oNu(mReq, 1, 1);				//Encode
			int tkOff = mReq.size(); oNu(mReq, 0, 4);	//Length
			oS(mReq, "UserNameIdentityToken");		//policyId: force to UserNameIdentityToken
			oS(mReq, strLine(authData(),0));		//UserName
			oS(mReq, strLine(authData(),1));		//Password
			oS(mReq, "");					//EncryptionAlgorithm
			oNu(mReq, mReq.size()-tkOff-4, 4, tkOff);	//Real length
		    }
									//> userTokenSignature
		    oS(mReq, "");					//signature
		    oS(mReq, "");					//algorithm
		}
		else if(io.attr("id") == "CloseSession")
		{
		    iTpId = OpcUa_CloseSessionRequest;
		    oN(mReq, 1, 1);					//deleteSubscriptions
		}
		else if(io.attr("id") == "Read")
		{
		    iTpId = OpcUa_ReadRequest;
		    oR(mReq, atof(io.attr("maxAge").c_str()), 8);	//maxAge 0 ms
		    oN(mReq, atoi(io.attr("timestampsToReturn").c_str()), 4);//timestampsTo Return (SERVER_1)
									//> nodesToRead []
		    oNu(mReq, std::min(25u,io.childSize()-stIdx), 4);	//nodes
		    for(unsigned i_n = stIdx; i_n < io.childSize() && (i_n-stIdx) < 25; i_n++)
		    {
			oNodeId(mReq, NodeId::fromAddr(io.childGet(i_n)->attr("nodeId")));	//nodeId
			oNu(mReq, strtoul(io.childGet(i_n)->attr("attributeId").c_str(),NULL,0), 4);	//attributeId (Value)
			oS(mReq, "");					//indexRange
			oSqlf(mReq, "");				//dataEncoding
		    }
		}
		else if(io.attr("id") == "Write")
		{
		    iTpId = OpcUa_WriteRequest;
									//> nodesToWrite []
		    oNu(mReq,io.childSize(),4);				//nodes
		    for(unsigned i_n = 0; i_n < io.childSize(); i_n++)
		    {
			XML_N *nd = io.childGet(i_n);
			oNodeId(mReq, NodeId::fromAddr(nd->attr("nodeId")));			//nodeId
			oNu(mReq, strtoul(nd->attr("attributeId").c_str(),NULL,0), 4);		//attributeId (Value)
			oS(mReq, "");								//indexRange
			oDataValue(mReq, 0x0D, nd->text(), atoi(nd->attr("EncodingMask").c_str()));//value
		    }
		}
		else if(io.attr("id") == "Browse")
		{
		    iTpId = OpcUa_BrowseRequest;
									//> view
		    oNodeId(mReq, 0);					//viewId (0)
		    oTm(mReq, 0);					//timestamp
		    oNu(mReq, 0, 4);					//viewVersion

		    oNu(mReq, 0, 4);					//requestedMax ReferencesPerNode
									//> nodesToBrowse
		    oNu(mReq, io.childSize(), 4);			//Nodes 1
		    for(unsigned i_n = 0; i_n < io.childSize(); i_n++)
		    {
			oNodeId(mReq, NodeId::fromAddr(io.childGet(i_n)->attr("nodeId")));	//nodeId (RootFolder)
			oNu(mReq, strtoul(io.childGet(i_n)->attr("browseDirection").c_str(),NULL,0), 4);	//browseDirection (FORWARD_0)
			oNodeId(mReq, NodeId::fromAddr(io.childGet(i_n)->attr("referenceTypeId")));	//referenceTypeId (HierarchicalReferences)
			oNu(mReq, 1, 1);				//includeSubtypes (true)
			oNu(mReq, strtoul(io.childGet(i_n)->attr("nodeClassMask").c_str(),NULL,0), 4);	//nodeClassMask
			oNu(mReq, strtoul(io.childGet(i_n)->attr("resultMask").c_str(),NULL,0), 4);	//resultMask
		    }
		}
		else throw OPCError(OpcUa_BadNotSupported, "Request '%s' isn't supported.", io.attr("id").c_str());

		rez = "MSGF";							//SecureChannel message
		rez.reserve(200);
		oNu(rez, 0, 4);							//Message size
		oNu(rez, strtoul(io.attr("SecChnId").c_str(),NULL,10), 4);	//Secure channel identifier
		oNu(rez, strtoul(io.attr("SecTokenId").c_str(),NULL,10), 4);	//TokenId
		int begEncBlck = rez.size();
									//> Sequence header
		uint32_t SeqNumber = strtoul(io.attr("SeqNumber").c_str(),NULL,10)+1;
		io.setAttr("SeqNumber", uint2str(SeqNumber));
		uint32_t SeqReqId = strtoul(io.attr("SeqReqId").c_str(),NULL,10)+1;
		io.setAttr("SeqReqId", uint2str(SeqReqId));
		oNu(rez, SeqNumber, 4);					//Sequence number
		oNu(rez, SeqReqId, 4);					//RequestId
									//> Extension body object
		oNodeId(rez, iTpId);					//TypeId request
									//>> Request Header
		oNodeId(rez, NodeId::fromAddr(io.attr("authTokenId")));	//Session AuthenticationToken
		oTm(rez, curTime());				//timestamp
		uint32_t ReqHandle = strtoul(io.attr("ReqHandle").c_str(),NULL,10)+1;
		io.setAttr("ReqHandle", uint2str(ReqHandle));
		oN(rez, ReqHandle, 4);					//requestHandle
		oNu(rez, 0, 4);						//returnDiagnostics
		oS(rez, "");						//auditEntryId
		oNu(rez, 10000, 4);					//timeoutHint
									//>>> Extensible parameter
		oNodeId(rez, 0);					//TypeId (0)
		oNu(rez, 0, 1);						//Encoding
		rez.append(mReq);					//Same request
		oNu(rez, rez.size(), 4, 4);				//> Real message size
		string secPolicy = io.attr("SecPolicy");
		char secMessMode = atoi(io.attr("SecurityMode").c_str());
		if(secMessMode == MS_Sign || secMessMode == MS_SignAndEncrypt)
		{
		    string servKey = io.attr("servKey");

		    // Padding place
		    if(secMessMode == MS_SignAndEncrypt)
		    {
			int kSz = servKey.size()/3;
			int paddingSize = ((rez.size()-begEncBlck+1+20+kSz-1)/kSz)*kSz-(rez.size()-begEncBlck+20);
			rez += string(paddingSize,(char)(paddingSize-1));
		    }

		    // Real message size calc and place
		    oNu(rez, rez.size()+20, 4, 4);

		    // Signature
		    rez += symmetricSign(rez, servKey, secPolicy);

		    // Encoding
		    if(secMessMode == MS_SignAndEncrypt)
			rez.replace(begEncBlck, rez.size()-begEncBlck, symmetricEncrypt(rez.substr(begEncBlck),servKey,secPolicy));
		}

		if(debug) debugMess(io.attr("id")+" Req");
		//Send request and wait respond
		int resp_len = messIO(rez.data(), rez.size(), buf, sizeof(buf));
		rez.assign(buf, resp_len);
		int off = 4;
		for( ; rez.size() < 8 || rez.size() < iNu(rez,off,4); off = 4)
		{
		    resp_len = messIO(NULL, 0, buf, sizeof(buf));
		    if(!resp_len) throw OPCError(OpcUa_BadCommunicationError, "Not full respond.");
		    rez.append(buf, resp_len);
		}

		if(debug) debugMess(io.attr("id")+" Resp");

		off = 4;
		if(rez.size() < 8 || iNu(rez,off,4) != rez.size())
		    err = strMess("0x%x:%s", OpcUa_BadTcpMessageTooLarge, "Respond size is not coincidence.");
		else if(rez.compare(0,4,"ERRF") == 0) err = iErr(rez, off);
		else if(rez.compare(0,4,"MSGF") != 0)
		    err = strMess("0x%x:%s", OpcUa_BadTcpMessageTypeInvalid, "Respond don't acknowledge.");
		else
		{
		    iNu(rez, off, 4);					//Secure channel identifier
		    iNu(rez, off, 4);					//Symmetric Algorithm Security Header : TokenId
		    // Decrypt message block and signature check
		    if(secMessMode == MS_Sign || secMessMode == MS_SignAndEncrypt)
		    {
			string clKey = io.attr("clKey");
			if(secMessMode == MS_SignAndEncrypt)
			    rez.replace(off, rez.size()-off, symmetricDecrypt(rez.substr(off),clKey,secPolicy));
			if(rez.substr(rez.size()-20) != symmetricSign(rez.substr(0,rez.size()-20),clKey,secPolicy))	//Check Signature
			    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Signature error");
		    }
									//> Sequence header
		    iNu(rez, off, 4);					//Sequence number
		    iNu(rez, off, 4);					//RequestId
									//> Extension Object
		    uint32_t oTpId = iNodeId(rez, off).numbVal();	//TypeId
									//>> Body
									//>>> RespondHeader
		    iTm(rez, off);					//timestamp
		    iN(rez, off, 4);					//requestHandle
		    uint32_t stCode = iNu(rez, off, 4);			//StatusCode
		    iN(rez, off, 1);					//serviceDiagnostics
		    iS(rez, off);					//stringTable
									//>>> Extensible parameter
		    iNodeId(rez, off);					//TypeId (0)
		    iNu(rez, off, 1);					//Encoding

		    switch(oTpId)
		    {
			case OpcUa_FindServersResponse:
			{
			    if(iTpId != OpcUa_FindServersRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Respond NodeId don't acknowledge");
			    int AppDescrNub = iNu(rez, off, 4);				//List items
			    for(int i_l = 0; i_l < AppDescrNub; i_l++)
			    {
				XML_N *ad = io.childAdd("ApplicationDescription");
				ad->setAttr("applicationUri", iS(rez,off));		//applicationUri
				ad->setAttr("productUri", iS(rez,off));			//productUri
				ad->setAttr("applicationName", iSl(rez,off));		//applicationName
				ad->setAttr("applicationType", uint2str(iNu(rez,off,4)));//applicationType
				ad->setAttr("gatewayServerUri", iS(rez,off));		//gatewayServerUri
				ad->setAttr("discoveryProfileUri", iS(rez,off));	//discoveryProfileUri
											//>>>> discoveryUrls
				int discoveryUrlsN = iNu(rez, off, 4);			//List items
				for(int i_l2 = 0; i_l2 < discoveryUrlsN; i_l2++)
				    ad->childAdd("discoveryUrl")->setText(iS(rez,off));	//discoveryUrl
			    }
			    break;
			}
			case OpcUa_GetEndpointsResponse:
			{
			    if(iTpId != OpcUa_GetEndpointsRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Respond NodeId don't acknowledge");
			    int EndpointDescrNub = iNu(rez, off, 4);		//List items
			    for(int i_l = 0; i_l < EndpointDescrNub; i_l++)
			    {
				XML_N *xep = io.childAdd("EndpointDescription");
										//>>> EndpointDescription
				xep->setAttr("endpointUrl", iS(rez,off));	//endpointUrl
										//>>>> server (ApplicationDescription)
				xep->setAttr("applicationUri", iS(rez,off));	//applicationUri
				xep->setAttr("productUri", iS(rez,off));	//productUri
				xep->setAttr("applicationName", iSl(rez,off));	//applicationName
				xep->setAttr("applicationType", uint2str(iNu(rez,off,4)));	//applicationType
				xep->setAttr("gatewayServerUri", iS(rez,off));	//gatewayServerUri
				xep->setAttr("discoveryProfileUri", iS(rez,off));//discoveryProfileUri
										//>>>> discoveryUrls
				int discoveryUrlsN = iNu(rez, off, 4);		//List items
				for(int i_l2 = 0; i_l2 < discoveryUrlsN; i_l2++)
				    xep->childAdd("discoveryUrl")->setText(iS(rez,off));	//discoveryUrl
				xep->childAdd("serverCertificate")->setText(certDER2PEM(iS(rez,off)));	//>>> serverCertificate
				xep->setAttr("securityMode", uint2str(iNu(rez,off,4)));	//securityMode
				xep->setAttr("securityPolicyUri", iS(rez,off));	//securityPolicyUri
										//>>>> userIdentityTokens
				int userIdentityTokensN = iNu(rez, off, 4);	//List items
				for(int i_l2 = 0; i_l2 < userIdentityTokensN; i_l2++)
				{
				    XML_N *xit = xep->childAdd("userIdentityToken");
				    xit->setAttr("policyId", iS(rez,off));		//policyId
				    xit->setAttr("tokenType", uint2str(iNu(rez,off,4)));//tokenType
				    xit->setAttr("issuedTokenType", iS(rez,off));	//issuedTokenType
				    xit->setAttr("issuerEndpointUrl", iS(rez,off));	//issuerEndpointUrl
				    xit->setAttr("securityPolicyUri", iS(rez,off));	//securityPolicyUri
				}
				xep->setAttr("transportProfileUri", iS(rez,off));	//transportProfileUri
				xep->setAttr("securityLevel", uint2str(iNu(rez,off,1)));//securityLevel
			    }
			    break;
			}
			case OpcUa_CreateSessionResponse:
			{
			    if(iTpId != OpcUa_CreateSessionRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Respond NodeId don't acknowledge");
			    io.setAttr("sesId", iNodeId(rez,off).toAddr());		//sessionId
			    io.setAttr("authTokenId", iNodeId(rez,off).toAddr());	//authentication Token
			    io.setAttr("sesTm", real2str(iR(rez,off,8)));		//revisedSession Timeout, ms
			    io.setAttr("servNonce", iS(rez,off));			//serverNonce
			    string servCert = certDER2PEM(iS(rez,off));	//serverCertificate
			    int EndpointDescrNub = iNu(rez, off, 4);	//List items
			    for(int i_l = 0; i_l < EndpointDescrNub; i_l++)
			    {
									//> EndpointDescription
				iS(rez, off);				//endpointUrl
									//>> server (ApplicationDescription)
				iS(rez, off);				//applicationUri
				iS(rez, off);				//productUri
				iSl(rez, off);				//applicationName
				iNu(rez, off, 4);			//applicationType
				iS(rez, off);				//gatewayServerUri
				iS(rez, off);				//discoveryProfileUri
									//>> discoveryUrls
				int discoveryUrlsN = iNu(rez, off, 4);	//List items
				for(int i_l2 = 0; i_l2 < discoveryUrlsN; i_l2++)
				    iS(rez, off);			//discoveryUrl
				iS(rez, off);				//> serverCertificate
				iNu(rez, off, 4);				//securityMode
				iS(rez, off);				//securityPolicyUri
									//>> userIdentityTokens
				int userIdentityTokensN = iNu(rez, off, 4);//List items
				for(int i_l2 = 0; i_l2 < userIdentityTokensN; i_l2++)
				{
				    iS(rez, off);			//policyId
				    iNu(rez, off, 4);			//tokenType
				    iS(rez, off);			//issuedTokenType
				    iS(rez, off);			//issuerEndpointUrl
				    iS(rez, off);			//securityPolicyUri
				}
				iS(rez, off);				//transportProfileUri
				iNu(rez, off, 1);			//securityLevel
			    }
			    iS(rez, off);				//serverSoftware Certificates []
									//> serverSignature
			    string alg = iS(rez, off);			//algorithm
			    string sign = iS(rez, off);			//signature
			    if(io.attr("SecPolicy") != "None" &&
				    !asymmetricVerify(certPEM2DER(io.childGet("ClientCert")->text())+io.attr("Nonce"),sign,servCert))
				throw OPCError(OpcUa_BadApplicationSignatureInvalid, "Application signature error");
			    iNu(rez, off, 4);				//maxRequest MessageSize
			    break;
			}
			case OpcUa_ActivateSessionResponse:
			{
			    if(iTpId != OpcUa_ActivateSessionRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Respond NodeId don't acknowledge");
			    iS(rez, off);				//serverNonce
			    iS(rez, off);				//results []
			    iS(rez, off);				//diagnosticInfos []
			    break;
			}
			case OpcUa_CloseSessionResponse:
			    if(iTpId != OpcUa_CloseSessionRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Respond NodeId don't acknowledge");
			    io.setAttr("sesId", "");			//sessionId
			    io.setAttr("authTokenId", "");		//authentication Token
			    break;
			case OpcUa_ReadResponse:
			{
			    if(iTpId != OpcUa_ReadRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Respond NodeId don't acknowledge");
									//> results []
			    int resN = iNu(rez, off, 4);		//Nodes number
			    for(int i_r = 0; i_r < resN && stIdx < (int)io.childSize(); i_r++, stIdx++)
				iDataValue(rez, off, *io.childGet(stIdx));
									//>> diagnosticInfos []
			    iNu(rez, off, 4);				//Items number
			    if(stIdx < (int)io.childSize()) goto nextReq;
			    break;
			}
			case OpcUa_WriteResponse:
			{
			    if(iTpId != OpcUa_WriteRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Respond NodeId don't acknowledge");
									//> results []
			    int resN = iNu(rez, off, 4);		//Number
			    for(int i_r = 0; i_r < resN && i_r < (int)io.childSize(); i_r++)
				io.childGet(i_r)->setAttr("Status", strMess("0x%x",iNu(rez,off,4)));
									//>> diagnosticInfos []
			    iNu(rez, off, 4);				//Items number
			    break;
			}
			case OpcUa_BrowseResponse:
			{
			    if(iTpId != OpcUa_BrowseRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Respond NodeId don't acknowledge");
									//> results []
			    int resN = iNu(rez, off, 4);		//Numbers
			    for(int i_r = 0; i_r < resN && i_r < (int)io.childSize(); i_r++)
			    {
				XML_N *rno = io.childGet(i_r);
				//strtoul(rno->attr("resultMask").c_str(), NULL, 0);	//resultMask
				rno->setAttr("statusCode", uint2str(iNu(rez,off,4)));	//statusCode
				iS(rez, off);				//continuationPoint
									//>> References []
				int refN = iNu(rez, off, 4);		//Numbers
				for(int i_rf = 0; i_rf < refN; i_rf++)
				{
				    XML_N *bno = rno->childAdd("bNode");
				    bno->setAttr("referenceTypeId", uint2str(iNodeId(rez,off).numbVal()));
				    bno->setAttr("isForward", iNu(rez,off,1)?"1":"0");
				    bno->setAttr("nodeId", iNodeId(rez,off).toAddr());
				    bno->setAttr("browseName", iSqlf(rez,off));
				    bno->setAttr("displayName", iSl(rez,off));
				    bno->setAttr("nodeClass", uint2str(iNu(rez,off,4)));
				    bno->setAttr("typeDefinition", uint2str(iNodeId(rez,off).numbVal()));
				}
			    }
			    iS(rez, off);				//diagnosticInfos []
			    break;
			}
			case OpcUa_ServiceFault: err = strMess("0x%x:%s", stCode, "Service fault");	break;
		    }
		}
	    }
	}
	else err = strMess("0x%x:%s", OpcUa_BadServiceUnsupported, strMess("OPC_UA protocol '%s' isn't supported.", io.name().c_str()).c_str());
    }
    catch(OPCError er) { err = strMess("0x%x:%s", er.cod, er.mess.c_str()); }

    io.setAttr("err", err);
}

void Client::reqService( XML_N &io )
{
    io.setAttr("err", "");
    XML_N req("opc.tcp");
    if(!sess.secChnl || !sess.secToken || (1e-3*(curTime()-sess.sessOpen) >= sess.secLifeTime) ||
	sess.endPoint != endPoint() || sess.secPolicy != secPolicy() || sess.secMessMode != secMessMode())
    {
	//Close previous session for policy or endpoint change
	if(sess.authTkId.size())
	{
	    req.setAttr("id", "CloseSession")->
		setAttr("SecChnId", uint2str(sess.secChnl))->setAttr("SecTokenId", uint2str(sess.secToken))->
		setAttr("authTokenId", sess.authTkId)->setAttr("ReqHandle", uint2str(sess.reqHndl++))->
		setAttr("SeqNumber", uint2str(sess.sqNumb++))->setAttr("SeqReqId", uint2str(sess.sqReqId++))->
		setAttr("SecPolicy", sess.secPolicy)->setAttr("SecurityMode", int2str(sess.secMessMode))->
		setAttr("clKey", sess.clKey)->setAttr("servKey", sess.servKey);
	    protIO(req);
	    sess.clearSess();
	}

	//Close previous secure channel
	if(sess.secChnl && sess.secToken)
	{
	    req.setAttr("id", "CLO")->
		setAttr("SecChnId", uint2str(sess.secChnl))->setAttr("SecTokenId", uint2str(sess.secToken))->
		setAttr("SeqNumber", uint2str(sess.sqNumb++))->setAttr("SeqReqId", uint2str(sess.sqReqId++))->
		setAttr("SecPolicy", sess.secPolicy)->setAttr("SecurityMode", int2str(sess.secMessMode))->
		setAttr("clKey", sess.clKey)->setAttr("servKey", sess.servKey);
	    protIO(req);
	    sess.clearFull();
	}

	// Send HELLO message
	req.setAttr("id", "HEL")->setAttr("EndPoint", endPoint());
	protIO(req);
	if(!req.attr("err").empty())	{ io.setAttr("err", req.attr("err")); return; }

	// Send Open SecureChannel message for no secure policy
	req.setAttr("id", "OPN")->setAttr("SecChnId", "0")->setAttr("ReqType", int2str(SC_ISSUE))->
	    setAttr("SecPolicy", "None")->setAttr("SecurityMode", "1")->setAttr("SecLifeTm", "300000")->
	    setAttr("SeqNumber", "51")->setAttr("SeqReqId", "1")->setAttr("ReqHandle", "0");
	protIO(req);
	if(!req.attr("err").empty())	{ io.setAttr("err", req.attr("err")); return; }
	sess.sessOpen = curTime();
	sess.sqNumb = 51;
	sess.sqReqId = 1;
	sess.reqHndl = 0;
	sess.secChnl = strtoul(req.attr("SecChnId").c_str(), NULL, 10);
	sess.secToken = strtoul(req.attr("SecTokenId").c_str(), NULL, 10);
	sess.secLifeTime = atoi(req.attr("SecLifeTm").c_str());

	if(secPolicy() != "None")
	{
	    // Send GetEndpoints request for certificate retrieve and for secure policy check
	    req.setAttr("id", "GetEndpoints")->setAttr("EndPoint", endPoint());
	    protIO(req);
	    if(!req.attr("err").empty()) { io.setAttr("err", req.attr("err")); return; }
	    //  Find endoint with needed secure policy
	    unsigned i_ep;
	    for(i_ep = 0; i_ep < req.childSize(); i_ep++)
		if(req.childGet(i_ep)->name() == "EndpointDescription" &&
			strParse(req.childGet(i_ep)->attr("securityPolicyUri"),1,"#") == secPolicy() &&
			atoi(req.childGet(i_ep)->attr("securityMode").c_str()) == secMessMode())
		    break;
	    if(i_ep >= req.childSize())
	    { io.setAttr("err",strMess("0x%x:%s",OpcUa_BadSecurityPolicyRejected,"No secure policy found")); return; }
	    string servCert = req.childGet(i_ep)->childGet("serverCertificate")->text();
	    int secMessMode = atoi(req.childGet(i_ep)->attr("securityMode").c_str());
	    req.childClear();

	    // Send Close request for no secure channel
	    req.setAttr("id", "CLO");
	    protIO(req);
	    if(!req.attr("err").empty()) { io.setAttr("err",req.attr("err")); return; }
	    sess.clearFull();

	    // Send HELLO message
	    req.setAttr("id", "HEL")->setAttr("EndPoint", endPoint());
	    protIO(req);
	    if(!req.attr("err").empty()) { io.setAttr("err",req.attr("err")); return; }

	    // Send Open SecureChannel message for secure policy
	    req.setAttr("id", "OPN")->setAttr("SecChnId", "0")->setAttr("ReqType", int2str(SC_ISSUE))->
		setAttr("ClntCert", cert())->setAttr("ServCert", servCert)->setAttr("PvKey", pvKey())->
		setAttr("SecPolicy", secPolicy())->setAttr("SecurityMode", int2str(secMessMode))->
		setAttr("SecLifeTm", "3600000")->setAttr("SeqNumber", "51")->setAttr("SeqReqId", "1")->setAttr("ReqHandle", "0");
	    protIO(req);
	    if(!req.attr("err").empty()) { io.setAttr("err",req.attr("err")); return; }
	    sess.sessOpen = curTime();
	    sess.sqNumb = 51;
	    sess.sqReqId = 1;
	    sess.reqHndl = 0;
	    sess.secChnl = strtoul(req.attr("SecChnId").c_str(),NULL,10);
	    sess.secToken = strtoul(req.attr("SecTokenId").c_str(),NULL,10);
	    sess.secLifeTime = atoi(req.attr("SecLifeTm").c_str());
	    sess.servCert = servCert;
	    sess.secMessMode = secMessMode;
	    sess.clKey = req.attr("clKey");
	    sess.servKey = req.attr("servKey");
	}
	sess.endPoint = endPoint( );
	sess.secPolicy = secPolicy( );
    }
    //Renew channel request send
    else if(1e-3*(curTime()-sess.sessOpen) >= 0.75*sess.secLifeTime)
    {
	req.setAttr("id", "OPN")->setAttr("SecChnId", uint2str(sess.secChnl))->setAttr("ReqType", int2str(SC_RENEW))->
	    setAttr("ClntCert", cert())->setAttr("ServCert", sess.servCert)->setAttr("PvKey", pvKey())->
	    setAttr("SecPolicy", secPolicy())->setAttr("SecurityMode", int2str(sess.secMessMode))->
	    setAttr("SecLifeTm", "3600000")->setAttr("SeqNumber", uint2str(sess.sqNumb++))->
	    setAttr("SeqReqId", uint2str(sess.sqReqId++))->setAttr("ReqHandle", uint2str(sess.reqHndl++));
	protIO(req);
	if(!req.attr("err").empty()) { io.setAttr("err",req.attr("err")); return; }
	sess.sessOpen = curTime();
	sess.secChnl = strtoul(req.attr("SecChnId").c_str(),NULL,10);
	sess.secToken = strtoul(req.attr("SecTokenId").c_str(),NULL,10);
	sess.secLifeTime = atoi(req.attr("SecLifeTm").c_str());
	sess.clKey = req.attr("clKey");
	sess.servKey = req.attr("servKey");
    }

    io.setAttr("SecChnId", uint2str(sess.secChnl))->setAttr("SecTokenId", uint2str(sess.secToken))->
	setAttr("SecPolicy", sess.secPolicy)->setAttr("SecurityMode", int2str(sess.secMessMode));

    string ireq = io.attr("id");
    if(ireq != "FindServers" && ireq != "GetEndpoints" && (!sess.authTkId.size() /*|| 1e-3*(curTime()-sess.sessOpen) >= sess.sesLifeTime*/))
    {
	// Send CreateSession message
	req.setAttr("id", "CreateSession")->setAttr("EndPoint", endPoint())->setAttr("sesTm", "1.2e6")->
	    setAttr("PvKey", pvKey())->setAttr("ServCert", sess.servCert)->
	    setAttr("SecChnId", uint2str(sess.secChnl))->setAttr("SecTokenId", uint2str(sess.secToken))->
	    setAttr("SeqNumber", uint2str(sess.sqNumb++))->setAttr("SeqReqId", uint2str(sess.sqReqId++))->
	    setAttr("ReqHandle", uint2str(sess.reqHndl++))->setAttr("authTokenId", sess.authTkId)->
	    setAttr("SecPolicy", sess.secPolicy)->setAttr("SecurityMode", int2str(sess.secMessMode))->
	    setAttr("clKey", sess.clKey)->setAttr("servKey", sess.servKey)->
	    childAdd("ClientCert")->setText(cert());
	protIO(req);
	if(!req.attr("err").empty())	{ io.setAttr("err",req.attr("err")); sess.clearFull(); return; }
	sess.sesId = req.attr("sesId");
	sess.authTkId = req.attr("authTokenId");
	sess.sesLifeTime = atof(req.attr("sesTm").c_str());

	// Send ActivateSession message
	req.setAttr("id", "ActivateSession");
	protIO(req);
	if(!req.attr("err").empty())	{ io.setAttr("err",req.attr("err")); return; }
    }

    io.setAttr("authTokenId", sess.authTkId)->setAttr("ReqHandle", uint2str(sess.reqHndl++))->
	setAttr("SeqNumber", uint2str(sess.sqNumb++))->setAttr("SeqReqId", uint2str(sess.sqReqId++))->
	setAttr("clKey", sess.clKey)->setAttr("servKey", sess.servKey);
    protIO(io);

    if(strtoul(io.attr("err").c_str(),NULL,0) == OpcUa_BadInvalidArgument) sess.clearFull();
    else if(io.attr("id") == "CloseSession") sess.clearSess();
}

//*************************************************
//* Protocol OPC UA server part			  *
//*************************************************
Server::Server( ) : mSecCnlIdLast(1)	{ }

Server::~Server( )			{ }

int Server::chnlSet( int cid, const string &iEp, int32_t lifeTm, const string& iClCert, const string &iSecPolicy, char iSecMessMode )
{
    //Check for renew
    if(cid && mSecCnl.find(cid) != mSecCnl.end())
    {
	mSecCnl[cid].tLife = lifeTm;
	mSecCnl[cid].TokenIdPrev = mSecCnl[cid].TokenId;
	if((++mSecCnl[cid].TokenId) == 0) mSecCnl[cid].TokenId = 1;
	mSecCnl[cid].tCreate = curTime();
	return cid;
    }

    //New channel create
    do
    {
	if(!(++mSecCnlIdLast)) mSecCnlIdLast = 2;
    } while(mSecCnl.find(mSecCnlIdLast) != mSecCnl.end());

    mSecCnl[mSecCnlIdLast] = SecCnl(iEp, 1, lifeTm, iClCert, iSecPolicy, iSecMessMode);

    return mSecCnlIdLast;
}

void Server::chnlClose( int cid )	{ mSecCnl.erase(cid); }

Server::SecCnl Server::chnlGet( int cid )
{
    if(mSecCnl.find(cid) == mSecCnl.end()) return SecCnl();
    return mSecCnl[cid];
}

void Server::chnlSecSet( int cid, const string &iServKey, const string &iClKey )
{
    if(mSecCnl.find(cid) == mSecCnl.end()) return;
    mSecCnl[cid].servKey = iServKey;
    mSecCnl[cid].clKey = iClKey;
}

string Server::mkError( uint32_t errId, const string &err )
{
    string rez;
    rez.reserve(50);
    rez.append("ERRF");		//Error message type
    oNu(rez, 16+err.size(), 4);	//Message size
    oNu(rez, errId, 4);		//Error code
    oS(rez, err);		//Error message

    return rez;
}

bool Server::inReq( string &rba, const string &inPrtId, string *answ )
{
    uint32_t mSz = 0;
    int off = 0;
    bool dbg = debug(), holdConn = true;

nextReq:
    if(rba.size() <= 0) return holdConn;
    string rb, out;
    off = 4;

    try {
	if(rba.size() < 8) return false;
	mSz = iNu(rba, off, 4);
	rb = rba.substr(0, std::min(mSz,(uint32_t)rba.size()));

	//Check for hello message type
	if(rb.compare(0,4,"HELF") == 0) {
	    if(rb.size() > 4096) throw OPCError(OpcUa_BadTcpMessageTooLarge, "", "");
	    if(rb.size() < mSz) return false;

	    if(dbg) debugMess("HELLO Req");

	    off = 8;
	    iNu(rb, off, 4);				//Protocol version
	    iNu(rb, off, 4);				//Recive buffer size
	    iNu(rb, off, 4);				//Send buffer size
	    iNu(rb, off, 4);				//Max message size
	    iNu(rb, off, 4);				//Max chunk count
	    string EndpntURL = iS(rb, off);		//EndpointURL

	    // Find accessable endpoint
	    vector<string> epLs;
	    epEnList(epLs);
	    if(!epLs.size()) throw OPCError(OpcUa_BadTcpEndpointUrlInvalid, "", "");

	    // Prepare acknowledge message
	    out.reserve(28);
	    out.append("ACKF");				//Acknowledge message type
	    oNu(out, 28, 4);				//Message size
	    oNu(out, OpcUa_ProtocolVersion, 4);		//Protocol version
	    oNu(out, OpcUa_ReciveBufferSize, 4);	//Recive buffer size
	    oNu(out, OpcUa_SendBufferSize, 4);		//Send buffer size
	    oNu(out, OpcUa_MaxMessageSize, 4);		//Max message size
	    oNu(out, OpcUa_MaxChunkCount, 4);		//Max chunk count

	    if(dbg) debugMess("HELLO Resp");
	}
	//Check for Open SecureChannel message type
	else if(rb.compare(0,4,"OPNF") == 0) {
	    if(rb.size() < mSz) return false;
	    if(dbg) debugMess("OPN Req");

	    off = 8;
	    uint32_t chnlId = iNu(rb, off, 4);			//>SecureChannelId
	    string secPlcURI = iS(rb, off);			//>SecurityPolicyURI
	    string secPlc = strParse(secPlcURI, 1, "#");
	    bool isSecNone = false;
	    int symKeySz = 0, asymKeyPad = 0;
	    if(secPlc == "None")		isSecNone = true;
	    else if(secPlc == "Basic128Rsa15")	{ symKeySz = 16; asymKeyPad = 11; }
	    else if(secPlc == "Basic256")	{ symKeySz = 32; asymKeyPad = 42; }
	    else throw OPCError(OpcUa_BadSecurityPolicyRejected, "", "");

	    // Find server with that policy
	    vector<string> epLs;
	    epEnList(epLs);
	    int i_epOk = -1;
	    EP *wep = NULL;
	    for(int i_ep = 0; i_epOk < 0 && i_ep < (int)epLs.size(); i_ep++) {
		if(!(wep=epEnAt(epLs[i_ep])))	continue;
		for(int i_s = 0; i_epOk < 0 && i_s < wep->secSize(); i_s++)
		    if(wep->secPolicy(i_s) == secPlc)
			i_epOk = i_ep;
	    }
	    if(i_epOk < 0) throw OPCError(OpcUa_BadSecurityPolicyRejected, "", "");

	    string clntCert = certDER2PEM(iS(rb,off));		//>SenderCertificate
	    string serverCertThmbp = iS(rb, off);		//>ReceiverCertificateThumbrint
	    if(!isSecNone) {
		if(serverCertThmbp != certThumbprint(wep->cert()))//>ServerCertificateThumbprint
		    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Server certificate thumbprint error.");
		// Decode message block
		rb.replace(off, rb.size()-off, asymmetricDecrypt(rb.substr(off),wep->pvKey(),secPlc));
		if(dbg) debugMess("OPN Req (decrypted)");
	    }
	    uint32_t secNumb = iNu(rb, off, 4);			//>Sequence number
	    uint32_t reqId = iNu(rb, off, 4);			//>RequestId
								//>Extension body object
	    if(iNodeId(rb,off).numbVal() != OpcUa_OpenSecureChannelRequest)	//> TypeId
		throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Requested OpenSecureChannel NodeId don't acknowledge");
								//> Request Header
	    iNodeId(rb, off);					//>  AuthenticationToken
	    iTm(rb, off);					//>  timestamp
	    int32_t rqHndl = iN(rb, off, 4);			//>  requestHandle
	    iNu(rb, off, 4);					//>  returnDiagnostics
	    iS(rb, off);					//>  auditEntryId
	    iNu(rb, off, 4);					//>  timeoutHint
								//>  AdditionalHeader
	    iNodeId(rb, off);					//>   TypeId (0)
	    iNu(rb, off, 1);					//>   Encoding
	    iNu(rb, off, 4);					//> ClientProtocolVersion
	    int32_t reqTp = iNu(rb, off, 4);			//>  RequestType
	    char secMode = iNu(rb, off, 4);			//>  SecurityMode
	    string clNonce = iS(rb, off);			//>  ClientNonce
	    int32_t reqLifeTm = iN(rb, off, 4);			//>  RequestedLifetime

	    if(!isSecNone) {
		off += iNu(rb,off,1);				//Pass padding
		if(!asymmetricVerify(rb.substr(0,off),rb.substr(off),clntCert))	//Check Signature
		    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Signature error");
	    }

	    // Find message secure mode
	    bool secModOK = false;
	    for(int i_s = 0; !secModOK && i_s < wep->secSize(); i_s++)
		if(wep->secPolicy(i_s) == secPlc && wep->secMessageMode(i_s) == secMode)
		    secModOK = true;
	    if(!secModOK) throw OPCError(OpcUa_BadSecurityModeRejected, "", "");

	    chnlId = chnlSet((reqTp==SC_RENEW?chnlId:0), wep->id(), reqLifeTm, clntCert, secPlc, secMode);

	    // Prepare respond message
	    out.reserve(200);
	    out.append("OPNF");					//OpenSecureChannel message type
	    oNu(out, 0, 4);					//Message size
	    oNu(out, chnlId, 4);				//Secure channel identifier
	    oS(out, secPlcURI);					//Security policy URI
	    oS(out, isSecNone?string(""):certPEM2DER(wep->cert()));	//ServerCertificate
	    oS(out, isSecNone?string(""):certThumbprint(clntCert));	//ClientCertificateThumbprint
	    int begEncBlck = out.size();
	    oNu(out, secNumb, 4);				//Sequence number
	    oNu(out, reqId, 4);					//RequestId
								//> Extension Object
	    oNodeId(out, NodeId(OpcUa_OpenSecureChannelResponse));	//TypeId
								//>> Body
								//>>> RespondHeader
	    oTm(out, curTime());				//timestamp
	    oN(out, rqHndl, 4);				 	//requestHandle
	    oN(out, 0, 4);					//StatusCode
	    oN(out, 0, 1);					//serviceDiagnostics
	    oS(out, "");					//stringTable
								//>>> Extensible parameter
	    oNodeId(out, 0u);					//TypeId (0)
	    oNu(out, 0, 1);					//Encoding
								//>>>> Standard respond
	    oNu(out, OpcUa_ProtocolVersion, 4);			//ServerProtocolVersion
	    oNu(out, chnlId, 4);				//Secure channel identifier
	    oNu(out, chnlGet(chnlId).TokenId, 4);		//TokenId
	    oTm(out, chnlGet(chnlId).tCreate);			//CreatedAt
	    oN(out, chnlGet(chnlId).tLife, 4);			//RevisedLifeTime (600000, minimum)

	    if(!isSecNone) {
		// Generate nonce
		string servNonce = randBytes(symKeySz);
		oS(out, servNonce);				//nonce
		// Padding place
		int kSz = asymmetricKeyLength(clntCert),
		    signSz = asymmetricKeyLength(wep->cert()),
		    encSz = (out.size() - begEncBlck) + 1 + signSz,
		    paddingSize = (encSz/(kSz-asymKeyPad) + (encSz%(kSz-asymKeyPad)?1:0))*(kSz-asymKeyPad) - encSz;

		out += string(paddingSize+1, char(paddingSize));

		// Real message size calc and place
		oNu(out, begEncBlck + kSz*((out.size()-begEncBlck+signSz)/(kSz-asymKeyPad)), 4, 4);
		// Signature
		out += asymmetricSign(out, wep->pvKey());
		// Encoding
		out.replace(begEncBlck, out.size()-begEncBlck, asymmetricEncrypt(out.substr(begEncBlck),clntCert,secPlc));
		// Set channel secure properties
		chnlSecSet(chnlId, deriveKey(servNonce,clNonce,symKeySz*3), deriveKey(clNonce,servNonce,symKeySz*3));
	    }
	    else {
		oS(out, "\001");				//nonce
		oNu(out, out.size(), 4, 4);			//Real message size
	    }

	    if(dbg) debugMess("OPN Resp");
	}
	//Check for Close SecureChannel message type
	else if(rb.compare(0,4,"CLOF") == 0) {
	    if(rb.size() < mSz) return false;
	    if(dbg) debugMess("CLO Req");

	    off = 8;
	    uint32_t secId = iNu(rb, off, 4);			//Secure channel identifier
	    uint32_t tokId = iNu(rb, off, 4);			//TokenId
	    SecCnl scHd = chnlGet(secId);
	    // Secure channel and token check
	    if(!scHd.TokenId) throw OPCError(0, "", "");
	    if(scHd.TokenId != tokId)	throw OPCError(OpcUa_BadSecureChannelTokenUnknown, "Secure channel unknown");
	    // Decrypt message block
	    if(scHd.secMessMode == MS_SignAndEncrypt)
		rb.replace(off, rb.size()-off, symmetricDecrypt(rb.substr(off),scHd.servKey,scHd.secPolicy));
								//> Sequence header
	    iNu(rb, off, 4);					//Sequence number
	    iNu(rb, off, 4);					//RequestId
								//> Extension body object
	    if(iNodeId(rb,off).numbVal() != OpcUa_CloseSecureChannelRequest)	//TypeId
		throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Requested OpenSecureChannel NodeId don't acknowledge");
								//>> Request Header
	    iNodeId(rb, off);					//Session AuthenticationToken
	    iTm(rb, off);					//timestamp
	    iN(rb, off, 4);					//requestHandle
	    iNu(rb, off, 4);					//returnDiagnostics
	    iS(rb, off);					//auditEntryId
	    iNu(rb, off, 4);					//timeoutHint
								//>>> Extensible parameter
	    iNodeId(rb, off);					//TypeId (0)
	    iNu(rb, off, 1);					//Encoding
	    if(scHd.secMessMode == MS_Sign || scHd.secMessMode == MS_SignAndEncrypt)
	    {
		if(scHd.secMessMode == MS_SignAndEncrypt) off += iNu(rb, off, 1);	//Pass padding
		if(rb.substr(off) != symmetricSign(rb.substr(0,off),scHd.servKey,scHd.secPolicy))	//Check Signature
		    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Signature error");
	    }

	    chnlClose(secId);

	    // No respond. Close socket
	    return false;	//Close socket
	}
	//Check for SecureChannel message type
	else if(rb.compare(0,4,"MSGF") == 0) {
	    if(rb.size() < mSz) return false;
	    off = 8;
	    uint32_t stCode = 0;
	    uint32_t secId = iNu(rb, off, 4);			//Secure channel identifier
	    uint32_t tokId = iNu(rb, off, 4);			//TokenId
	    SecCnl scHd = chnlGet(secId);
	    // Secure channel and token check
	    if(!scHd.TokenId) throw OPCError(OpcUa_BadSecureChannelClosed, "Secure channel closed");
	    if(!(tokId == scHd.TokenId || (tokId == scHd.TokenIdPrev && (curTime() < 1000ll*(scHd.tCreate+0.25*scHd.tLife)))))
		throw OPCError(OpcUa_BadSecureChannelTokenUnknown, "Secure channel unknown");
	    if(curTime() > (scHd.tCreate+(int64_t)scHd.tLife*1000)) throw OPCError(OpcUa_BadSecureChannelIdInvalid, "Secure channel renew expired");
	    EP *wep = epEnAt(scHd.endPoint);
	    if(!wep) throw OPCError(OpcUa_BadTcpEndpointUrlInvalid, "No propper Endpoint present");
	    // Decrypt message block and signature check
	    if(scHd.secMessMode == MS_Sign || scHd.secMessMode == MS_SignAndEncrypt)
	    {
		if(scHd.secMessMode == MS_SignAndEncrypt)
		    rb.replace(off, rb.size()-off, symmetricDecrypt(rb.substr(off),scHd.servKey,scHd.secPolicy));
		if(rb.substr(rb.size()-20) != symmetricSign(rb.substr(0,rb.size()-20),scHd.servKey,scHd.secPolicy))    //Check Signature
		    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Signature error");
	    }
								//> Sequence header
	    uint32_t seqN = iNu(rb, off, 4);			//Sequence number
	    uint32_t reqId = iNu(rb, off, 4);			//RequestId
								//> Extension body object
	    int reqTp = iNodeId(rb,off).numbVal();		//TypeId request
								//>> Request Header
	    uint32_t sesTokId = iNodeId(rb, off).numbVal();	//Session AuthenticationToken
	    // Session check
	    if(!(reqTp == OpcUa_CreateSessionRequest || reqTp == OpcUa_FindServersRequest || reqTp == OpcUa_GetEndpointsRequest ||
		    reqTp == OpcUa_ActivateSessionRequest) && (stCode=wep->sessActivate(sesTokId,secId,true,inPrtId)))
		reqTp = OpcUa_ServiceFault;
	    //if(sesTokId && reqTp != OpcUa_CreateSessionRequest && !wep->sessActivate(sesTokId,secId,reqTp!=OpcUa_ActivateSessionRequest,inPrtId))
	    //{ stCode = OpcUa_BadSessionIdInvalid; reqTp = OpcUa_ServiceFault; }
	    iTm(rb, off);					 //timestamp
	    int32_t reqHndl = iN(rb, off, 4);			//requestHandle
	    iNu(rb, off, 4);					//returnDiagnostics
	    iS(rb, off);					//auditEntryId
	    iNu(rb, off, 4);					//timeoutHint
								//>>> Extensible parameter
	    iNodeId(rb, off);					//TypeId (0)
	    iNu(rb, off, 1);					//Encoding

	    if(dbg) debugMess(strMess("MSG Req: %d",reqTp));

	    // Prepare respond message
	    string respEp;
	    switch(reqTp)
	    {
		case OpcUa_FindServersRequest:
		{
		    //  Request
		    iS(rb, off);				//endpointUrl
		    iS(rb, off);				//localeIds []
		    iS(rb, off);				//serverUris []

		    //  Respond
		    reqTp = OpcUa_FindServersResponse;
		    oNu(respEp, 1, 4);				//ApplicationDescription list items
								//>>>> ApplicationDescription 1
		    oS(respEp, applicationUri());		//applicationUri
		    oS(respEp, productUri());			//productUri
		    oSl(respEp, applicationName(), "en");	//applicationName
		    oNu(respEp, 0, 4);				//applicationType (SERVER)
		    oS(respEp, "");				//gatewayServerUri
		    oS(respEp, "");				//discoveryProfileUri
								//>>>> discoveryUrls
		    vector<string> duLs;
		    discoveryUrls(duLs);
		    oNu(respEp, duLs.size(), 4);		//List items
		    for(unsigned i_du = 0; i_du < duLs.size(); i_du++)
			oS(respEp, duLs[i_du]);			//discoveryUrl
		    break;
		}
		case OpcUa_GetEndpointsRequest:
		{
		    //  Request
		    iS(rb, off);				//endpointUrl
		    iS(rb, off);				//localeIds []
		    iS(rb, off);				//profileUris []

		    //  Respond
		    reqTp = OpcUa_GetEndpointsResponse;

		    respEp.reserve(2000);
		    oNu(respEp, 0, 4);				//EndpointDescrNubers list items
		    //  Get enpoints policies list
		    vector<string> epLs;
		    epEnList(epLs);
		    unsigned epCnt = 0;
		    for(unsigned i_ep = 0; i_ep < epLs.size(); i_ep++)
		    {
			EP *ep = epEnAt(epLs[i_ep]);
			if(!ep) continue;
								//>>> EndpointDescription
			for(int i_sec = 0; i_sec < ep->secSize(); i_sec++, epCnt++)
			{
			    oS(respEp, ep->url()+"/OSCADA_OPC/"+ep->secPolicy(i_sec)/*+"/"+ep->secMessMode(i_sec)*/);    //endpointUrl
								//>>>> server (ApplicationDescription)
			    oS(respEp, applicationUri());	//applicationUri
			    oS(respEp, productUri());		//productUri
			    oSl(respEp, applicationName(),"en");//applicationName
			    oNu(respEp, 0, 4);			//applicationType (SERVER)
			    oS(respEp, "");			//gatewayServerUri
			    oS(respEp, "");			//discoveryProfileUri

								//>>>> discoveryUrls
			    vector<string> duLs;
			    discoveryUrls(duLs);
			    oNu(respEp, duLs.size(), 4);	//List items
			    for(unsigned i_du = 0; i_du < duLs.size(); i_du++)
				oS(respEp,duLs[i_du]);		//discoveryUrl

			    oS(respEp, certPEM2DER(ep->cert()));//>>> serverCertificate
			    oNu(respEp, ep->secMessageMode(i_sec), 4);	//securityMode:MessageSecurityMode
			    oS(respEp, "http://opcfoundation.org/UA/SecurityPolicy#"+ep->secPolicy(i_sec));	//securityPolicyUri

								//>>>> userIdentityTokens
			    oNu(respEp, 2, 4);			//List items
								//>>>> userIdentityToken 1
			    oS(respEp, "Anonymous");		//policyId
			    oNu(respEp, 0, 4);			//tokenType
			    oS(respEp, "");			//issuedTokenType
			    oS(respEp, "");			//issuerEndpointUrl
			    oS(respEp, "");			//securityPolicyUri
								//>>>> userIdentityToken 2
			    oS(respEp, "UserName");		//policyId
			    oNu(respEp, 1, 4);			//tokenType
			    oS(respEp, "");			//issuedTokenType
			    oS(respEp, "");			//issuerEndpointUrl
			    oS(respEp, "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15");	//securityPolicyUri

			    oS(respEp, "");			//transportProfileUri
			    oNu(respEp, 0, 1);			//securityLevel
			}
		    }
		    oNu(respEp, epCnt, 4, 0);			//EndpointDescrNubers list items

		    break;
		}
		case OpcUa_CreateSessionRequest:
		{
		    //  Request
								//> clientDescription (Application Description)
		    iS(rb, off);				//applicationUri
		    iS(rb, off);				//productUri
		    iSl(rb, off);				//applicationName
		    iNu(rb, off, 4);				//applicationType (CLIENT_1)
		    iS(rb, off);				//gatewayServerUri
		    iS(rb, off);				//discoveryProfileUri
		    iS(rb, off);				//discoveryUrls

		    iS(rb, off);				//serverUri
		    iS(rb, off);				//endpointUrl
		    string sessNm = iS(rb, off);		//sessionName
		    string clNonce = iS(rb, off);		//clientNonce
		    string clCert = iS(rb, off);		//clientCertificate
		    double rStm = iR(rb, off, 8);		//Requested SessionTimeout, ms
		    iNu(rb, off, 4);				//maxResponse MessageSize

		    //  Try for session reusing
		    int sessId = 0;
		    if(!sesTokId && !wep->sessActivate(sesTokId,secId,true,inPrtId)) sessId = sesTokId;
		    //  Create new session
		    if(!sessId) sessId = wep->sessCreate(sessNm, rStm);
		    string servNonce = randBytes(32);
		    wep->sessServNonceSet(sessId, servNonce);

		    //  Respond
		    reqTp = OpcUa_CreateSessionResponse;

		    respEp.reserve(2000);
		    oNodeId(respEp, NodeId(sessId,NS_SERVER));	//sessionId
		    oNodeId(respEp, NodeId(sessId,NS_OPC_UA));	//authentication Token
		    oR(respEp, wep->sessGet(sessId).tInact, 8);	//revisedSession Timeout, ms
		    oS(respEp, servNonce);			//serverNonce
		    oS(respEp, certPEM2DER(wep->cert()));	//serverCertificate
								//> EndpointDescr []
		    int enpNumperPos = respEp.size();
		    oNu(respEp, 0, 4);				//EndpointDescrNubers list items
		    //  Get enpoints policies list
		    vector<string> epLs;
		    epEnList(epLs);
		    unsigned epCnt = 0;
		    for(unsigned i_ep = 0; i_ep < epLs.size(); i_ep++) {
			EP *ep = epEnAt(epLs[i_ep]);
			if(!ep) continue;
								//>>> EndpointDescription
			for(int i_sec = 0; i_sec < ep->secSize(); i_sec++, epCnt++)
			{
			    oS(respEp, ep->url());		//endpointUrl
								//>>>> server (ApplicationDescription)
			    oS(respEp, applicationUri());	//applicationUri
			    oS(respEp, productUri());		//productUri
			    oSl(respEp, applicationName(), "en");//applicationName
			    oNu(respEp, 0, 4);			//applicationType (SERVER)
			    oS(respEp, "");			//gatewayServerUri
			    oS(respEp, "");			//discoveryProfileUri

								//>>>> discoveryUrls
			    vector<string> duLs;
			    discoveryUrls(duLs);
			    oNu(respEp, duLs.size(), 4);	//List items
			    for(unsigned i_du = 0; i_du < duLs.size(); i_du++)
				oS(respEp, duLs[i_du]);		//discoveryUrl

			    oS(respEp, certPEM2DER(ep->cert()));//>>> serverCertificate
			    oNu(respEp, ep->secMessageMode(i_sec), 4);	//securityMode:MessageSecurityMode
			    oS(respEp, "http://opcfoundation.org/UA/SecurityPolicy#"+ep->secPolicy(i_sec));	//securityPolicyUri

								//>>>> userIdentityTokens
			    oNu(respEp, 2, 4);			//List items
								//>>>> userIdentityToken 1
			    oS(respEp, "Anonymous");		//policyId
			    oNu(respEp, 0, 4);			//tokenType
			    oS(respEp, "");			//issuedTokenType
			    oS(respEp, "");			//issuerEndpointUrl
			    oS(respEp, "");			//securityPolicyUri
								//>>>> userIdentityToken 2
			    oS(respEp, "UserName");		//policyId
			    oNu(respEp, 1, 4);			//tokenType
			    oS(respEp, "");			//issuedTokenType
			    oS(respEp, "");			//issuerEndpointUrl
			    oS(respEp, "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15");	//securityPolicyUri

			    oS(respEp, "");			//transportProfileUri
			    oNu(respEp, 0, 1);			//securityLevel
			}
		    }
		    oNu(respEp, epCnt, 4, enpNumperPos);	//EndpointDescrNubers real list items

		    oS(respEp, "");				//serverSoftware Certificates []
								//> serverSignature
		    if(scHd.secPolicy == "None") {
			oS(respEp, "");				//algorithm
			oS(respEp, "");				//signature
		    }
		    else {
			oS(respEp, "http://www.w3.org/2000/09/xmldsig#rsa-sha1");	//algorithm
			oS(respEp, asymmetricSign(clCert+clNonce,wep->pvKey()));	//signature
		    }

		    oNu(respEp, 0, 4);				//maxRequest MessageSize
		    break;
		}
		case OpcUa_ActivateSessionRequest:
		{
		    //  Request
								//>clientSignature
		    string alg = iS(rb, off);			//> algorithm
		    string sign = iS(rb, off);			//> signature
		    if(scHd.secPolicy != "None") {
			if(!asymmetricVerify(certPEM2DER(wep->cert())+wep->sessGet(sesTokId).servNonce, sign, scHd.clCert))
			    throw OPCError(OpcUa_BadApplicationSignatureInvalid, "Application signature error");
		    }

		    iNu(rb, off, 4);				//>clientSoftwareCertificates []
		    uint32_t ln = iNu(rb, off, 4);		//>localeIds []
		    for(unsigned i_l = 0; i_l < ln; i_l++)
			iS(rb, off);				//> localeId
								//>userIdentityToken
		    uint32_t userIdTk = iNodeId(rb, off).numbVal();	//> TypeId
		    iNu(rb, off, 1);				//> Encode
		    iNu(rb, off, 4);				//> Length
		    XML_N userIdent("IdentityToken");
		    switch(userIdTk)
		    {
			case OpcUa_AnonymousIdentityToken:		//>  AnonymousIdentityToken
			    userIdent.setAttr("policyId", iS(rb,off));	//>   policyId
			    break;
			case OpcUa_UserNameIdentityToken:		//>  UserNameIdentityToken
			    userIdent.setAttr("policyId", iS(rb,off));	//>   policyId
			    userIdent.setAttr("userName", iS(rb,off));	//>   userName
			    userIdent.setAttr("password", iS(rb,off));	//>   password
			    userIdent.setAttr("encryptionAlgorithm", iS(rb,off)); //>   encryptionAlgorithm
			    break;
			case OpcUa_X509IdentityToken:			//>   X509IdentityToken
			    userIdent.setAttr("policyId", iS(rb,off));	//>   policyId
			    userIdent.setAttr("certificateData", iS(rb,off));	//>   certificateData
			    break;
			default: reqTp = OpcUa_ServiceFault; stCode = OpcUa_BadUserAccessDenied;	break;
		    }
		    if(stCode || (stCode=wep->sessActivate(sesTokId,secId,false,inPrtId,userIdent))) reqTp = OpcUa_ServiceFault;
		    if(reqTp != OpcUa_ActivateSessionRequest)	break;
								//> userTokenSignature
		    iS(rb, off);				//signature
		    iS(rb, off);				//algorithm

		    //  Respond
		    reqTp = OpcUa_ActivateSessionResponse;
		    respEp.reserve(100);
		    string servNonce = randBytes(32);
		    wep->sessServNonceSet(sesTokId, servNonce);
		    oS(respEp, servNonce);			//serverNonce
		    oS(respEp, "");				//results []
		    oS(respEp, "");				//diagnosticInfos []
		    break;
		}
		case OpcUa_CloseSessionRequest:
		{
		    //  Request
		    bool subScrDel = iNu(rb, off, 1);		//deleteSubscriptions
		    wep->sessClose(sesTokId);
		    if(subScrDel) {
			pthread_mutex_lock(&wep->mtxData);
			for(unsigned i_ss = 0; i_ss < wep->mSubScr.size(); ++i_ss)
			    if(wep->mSubScr[i_ss].st != SS_CLOSED && wep->mSubScr[i_ss].sess == (int)sesTokId)
			    {
				wep->mSubScr[i_ss].setState(SS_CLOSED);
				if(dbg) debugMess(strMess("EP: SubScription %d closed.",i_ss));
			    }
			pthread_mutex_unlock(&wep->mtxData);
		    }

		    //  Respond
		    reqTp = OpcUa_CloseSessionResponse;
		    break;
		}
		case OpcUa_CreateSubscriptionRequest:
		{
		    //  Request
		    double pi = iR(rb, off, 8);			//requestedPublishingInterval
		    uint32_t lt = iNu(rb, off, 4);		//requestedLifetimeCount
		    uint32_t ka = iNu(rb, off, 4);		//requestedMaxKeepAliveCount
		    uint32_t npp = iNu(rb, off, 4);		//maxNotificationsPerPublish
		    bool en = iNu(rb, off, 1);			//publishingEnabled
		    uint8_t pr = iNu(rb, off, 1);		//priority

		    uint32_t subScrId = wep->subscrSet(0, SS_CREATING, en, sesTokId, pi, lt, ka, npp, pr);
		    if(!subScrId) { reqTp = OpcUa_ServiceFault; stCode = OpcUa_BadTooManySubscriptions; break; }
		    if(dbg) debugMess(strMess("EP: SubScription %d created.",subScrId));
		    Subscr ss = wep->subscrGet(subScrId);

		    //  Respond
		    reqTp = OpcUa_CreateSubscriptionResponse;
		    respEp.reserve(20);
		    oNu(respEp, subScrId, 4);			//subscriptionId
		    oR(respEp, ss.publInterv, 8);		//revisedPublishingInterval
		    oNu(respEp, ss.cntrLifeTime, 4);		//revisedLifetimeCount
		    oNu(respEp, ss.cntrKeepAlive, 4);		//revisedMaxKeepAliveCount
		    break;
		}
		case OpcUa_ModifySubscriptionRequest:
		{
		    //  Request
		    uint32_t subScrId = iNu(rb, off, 4);	//subscriptionId
		    double pi = iR(rb, off, 8);			//requestedPublishingInterval
		    uint32_t lt = iNu(rb, off, 4);		//requestedLifetimeCount
		    uint32_t ka = iNu(rb, off, 4);		//requestedMaxKeepAliveCount
		    uint32_t npp = iNu(rb, off, 4);		//maxNotificationsPerPublish
		    bool en = iNu(rb, off, 1);			//publishingEnabled
		    uint8_t pr = iNu(rb, off, 1);		//priority

		    Subscr ss = wep->subscrGet(subScrId);
		    if(ss.st == SS_CLOSED) { reqTp = OpcUa_ServiceFault; stCode = OpcUa_BadSubscriptionIdInvalid; break; }
		    wep->subscrSet(subScrId, SS_CUR, en, sesTokId, pi, lt, ka, npp, pr);
		    ss = wep->subscrGet(subScrId);

		    //  Respond
		    reqTp = OpcUa_ModifySubscriptionResponse;
		    respEp.reserve(20);
		    oR(respEp, ss.publInterv, 8);		//revisedPublishingInterval
		    oNu(respEp, ss.cntrLifeTime, 4);		//revisedLifetimeCount
		    oNu(respEp, ss.cntrKeepAlive, 4);		//revisedMaxKeepAliveCount
		    break;
		}
		/*case OpcUa_TransferSubscriptionsRequest:
		{
		    //>> Request
		    uint32_t sn = iNu(rb, off, 4);		//subscriptionIds []

		    //>> Respond
		    reqTp = OpcUa_TransferSubscriptionsResponse;
		    respEp.reserve(20);
		    oNu(respEp, sn, 4);				//results []

		    for(uint32_t i_s = 0; i_s < sn; i_s++)
		    {
			uint32_t subScrId = iNu(rb, off, 4);	//subscriptionId
			Subscr ss = wep->subscrGet(subScrId);
			oNu(respEp, ((ss.st==SS_CLOSED)?OpcUa_BadSubscriptionIdInvalid:0), 4);	//statusCode
		    }
		    iNu(rb, off, 1);				//sendInitialValues

		    break;
		}*/
		case OpcUa_DeleteSubscriptionsRequest:
		{
		    //  Request
		    uint32_t sn = iNu(rb, off, 4);		//subscriptionIds []

		    //  Respond
		    reqTp = OpcUa_DeleteSubscriptionsResponse;
		    respEp.reserve(20);
		    oNu(respEp, sn, 4);				//results []

		    for(uint32_t i_s = 0; i_s < sn; i_s++) {
			uint32_t subScrId = iNu(rb, off, 4);	//subscriptionId
			Subscr ss = wep->subscrGet(subScrId);
			oNu(respEp, ((ss.st==SS_CLOSED)?OpcUa_BadSubscriptionIdInvalid:0), 4);	//statusCode
			wep->subscrSet(subScrId, SS_CLOSED);
			if(dbg) debugMess(strMess("EP: SubScription %d closed.",subScrId));
		    }
		    oN(respEp, 0, 4);				//diagnosticInfos []
		    break;
		}
		case OpcUa_CreateMonitoredItemsRequest:
		{
		    //  Request
		    uint32_t subScrId = iNu(rb, off, 4);	//>subscriptionId
		    if(wep->subscrGet(subScrId).st == SS_CLOSED) { reqTp = OpcUa_ServiceFault; stCode = OpcUa_BadSubscriptionIdInvalid; break; }

		    TimestampsToReturn tmStRet = (TimestampsToReturn)iNu(rb, off, 4);	//>timestampsToReturn
		    uint32_t ni = iNu(rb, off, 4);		//>itemsToCreate []

		    //  Respond
		    reqTp = OpcUa_CreateMonitoredItemsResponse;
		    oNu(respEp, ni, 4);				//<results []

		    //  Nodes list process and request form
		    for(uint32_t i_m = 0; i_m < ni; i_m++) {
								//> itemToMonitor
			NodeId nid = iNodeId(rb, off);		//>  nodeId
			uint32_t aid = iNu(rb, off, 4);		//>  attributeId
			iS(rb, off);				//>  indexRange
			iSqlf(rb, off);				//>  dataEncoding

			MonitoringMode mM = (MonitoringMode)iNu(rb, off, 4);	//> monitoringMode
								//> requestedParameters
			uint32_t cH = iNu(rb, off, 4);		//>  clientHandle
			double sI = iR(rb, off, 8);		//>  samplingInterval
			NodeId fid = iNodeId(rb, off);		//>  filter
			iNu(rb, off, 1);			//>   EncodingMask
			if(fid.numbVal() != 0) {
			    uint32_t eSz = iNu(rb, off, 4);	//>   ExtObj
			    iVal(rb, off, eSz);			//>    No filters support - simple pass
			}
			uint32_t qSz = iNu(rb, off, 4);		//>  queueSize
			bool dO = iNu(rb, off, 1);		//>  discardOldest

			//   Node result
			uint32_t st = 0;
			uint32_t mIt = 0;

			if(fid.numbVal() != 0)	st = OpcUa_BadFilterNotAllowed;
			else {
			    //   Create new monitored item
			    mIt = wep->mItSet(subScrId, 0, mM, nid, aid, tmStRet, sI, qSz, dO, cH);
			    if(!mIt) st = OpcUa_BadSubscriptionIdInvalid;
			    else if(mIt > wep->limMonitItms()) st = OpcUa_BadTooManyOperations;
			    else {
				Subscr::MonitItem mItO = wep->mItGet(subScrId, mIt);
				if(mItO.nd.isNull()) st = OpcUa_BadNodeIdInvalid;
				else if(mItO.aid == Aid_Error) st = OpcUa_BadAttributeIdInvalid;
				else {
				    sI = mItO.smplItv;
				    qSz = mItO.qSz;
				}
			    }
			    if(st) { wep->mItSet(subScrId, mIt, MM_DISABLED); mIt = 0; }
			}

			oNu(respEp, st, 4);				//< statusCode
			oN(respEp, mIt, 4);				//< monitoredItemId
			oR(respEp, sI, 8);				//< revisedSamplingInterval
			oNu(respEp, qSz, 4);				//< revisedQueueSize
			oNodeId(respEp, 0u);				//< filterResult
			oNu(respEp, 0, 1);				//< encodingMask
		    }
		    oN(respEp, 0, 4);					//<diagnosticInfos []
		    break;
		}
		case OpcUa_ModifyMonitoredItemsRequest:
		{
		    //  Request
		    uint32_t subScrId = iNu(rb, off, 4);		//>subscriptionId
		    if(wep->subscrGet(subScrId).st == SS_CLOSED) { reqTp = OpcUa_ServiceFault; stCode = OpcUa_BadSubscriptionIdInvalid; break; }
		    TimestampsToReturn tmStRet = (TimestampsToReturn)iNu(rb, off, 4);	//>timestampsToReturn
		    uint32_t ni = iNu(rb, off, 4);			//>itemsToModify []

		    //  Respond
		    reqTp = OpcUa_ModifyMonitoredItemsResponse;
		    oNu(respEp, ni, 4);				//<results []
		    //  Nodes list process and request form
		    for(uint32_t i_m = 0; i_m < ni; i_m++) {
			uint32_t mIt = iNu(rb, off, 4);		//> monitoredItemId
								//> requestedParameters
			uint32_t cH = iNu(rb, off, 4);		//>  clientHandle
			double sI = iR(rb, off, 8);		//>  samplingInterval
			NodeId fid = iNodeId(rb, off);		//>  filter
			iNu(rb, off, 1);			//>   EncodingMask
			if(fid.numbVal() != 0) {
			    uint32_t eSz = iNu(rb, off, 4);	//>   ExtObj
			    iVal(rb, off, eSz);			//>    No filters support - simple pass
			}
			uint32_t qSz = iNu(rb, off, 4);		//>  queueSize
			bool dO = iNu(rb, off, 1);		//>  discardOldest

			//   Node result
			uint32_t st = 0;
			if(fid.numbVal() != 0)	st = OpcUa_BadFilterNotAllowed;
			//   Modify monitored item
			else if(wep->mItGet(subScrId,mIt).md == MM_DISABLED) st = OpcUa_BadMonitoredItemIdInvalid;
			else {
			    wep->mItSet(subScrId, mIt, MM_CUR, NodeId(), OpcUa_NPosID, tmStRet, sI, qSz, dO, cH);
			    Subscr::MonitItem mItO = wep->mItGet(subScrId, mIt);
			    sI = mItO.smplItv;
			    qSz = mItO.qSz;
			}

			oNu(respEp, st, 4);				//< statusCode
			oN(respEp, mIt, 4);				//< monitoredItemId
			oR(respEp, sI, 8);				//< revisedSamplingInterval
			oNu(respEp, qSz, 4);				//< revisedQueueSize
			oNodeId(respEp, 0u);				//< filterResult
			oNu(respEp, 0, 1);				//< encodingMask
		    }
		    oN(respEp, 0, 4);					//<diagnosticInfos []
		    break;
		}
		case OpcUa_SetMonitoringModeRequest:
		case OpcUa_DeleteMonitoredItemsRequest:
		{
		    //  Request
		    uint32_t subScrId = iNu(rb, off, 4);		//>subscriptionId
		    MonitoringMode mM = MM_DISABLED;
		    if(wep->subscrGet(subScrId).st == SS_CLOSED) { reqTp = OpcUa_ServiceFault; stCode = OpcUa_BadSubscriptionIdInvalid; break; }
		    if(reqTp == OpcUa_SetMonitoringModeRequest) {
			mM = (MonitoringMode)iNu(rb, off, 4);		//>monitoringMode
			reqTp = OpcUa_SetMonitoringModeResponse;
		    }
		    else reqTp = OpcUa_DeleteMonitoredItemsResponse;
		    uint32_t ni = iNu(rb, off, 4);			//>itemsToModify []

		    //  Respond
		    oNu(respEp, ni, 4);					//<results []
		    //  Nodes list process and request form
		    for(uint32_t i_m = 0; i_m < ni; i_m++) {
			uint32_t mIt = iNu(rb, off, 4);			//> monitoredItemId

			//   Node result
			uint32_t st = 0;
			//   Modify monitored item
			if(wep->mItGet(subScrId,mIt).md == MM_DISABLED) st = OpcUa_BadMonitoredItemIdInvalid;
			else wep->mItSet(subScrId, mIt, mM);
			oNu(respEp, st, 4);				//< statusCode
		    }
		    oN(respEp, 0, 4);					//<diagnosticInfos []
		    break;
		}
		case OpcUa_SetPublishingModeRequest:
		{
		    //  Request
		    bool en = iNu(rb, off, 1);			//publishingEnabled
		    uint32_t sn = iNu(rb, off, 4);		//subscriptionIds []

		    //  Respond
		    reqTp = OpcUa_SetPublishingModeResponse;
		    respEp.reserve(20);
		    oNu(respEp, sn, 4);				//results []

		    for(uint32_t i_s = 0; i_s < sn; i_s++) {
			uint32_t subScrId = iNu(rb, off, 4);	//subscriptionId
			Subscr ss = wep->subscrGet(subScrId);
			if(ss.st != SS_CLOSED) wep->subscrSet(subScrId, SS_CUR, en);
			oNu(respEp, ((ss.st==SS_CLOSED)?OpcUa_BadSubscriptionIdInvalid:0), 4);	//statusCode
		    }
		    oN(respEp, 0, 4);			//diagnosticInfos []
		    break;
		}
		case OpcUa_TranslateBrowsePathsToNodeIdsRequest:
		{
		    //  Request
							//> browsePaths []
		    uint32_t ip = iNu(rb, off, 4); 	//paths number

		    //  Respond
		    reqTp = OpcUa_TranslateBrowsePathsToNodeIdsResponse;
		    oNu(respEp, ip, 4);			//results []

		    //  Pathes list process and request form
		    for(unsigned i_p = 0; i_p < ip; i_p++) {
			NodeId sN = iNodeId(rb, off);	//startingNode
							//>> relativePath
			uint32_t irp = iNu(rb, off, 4);	//rpaths number

			bool nOK = true;
			XML_N req("data");
			for(unsigned i_rp = 0; nOK && i_rp < irp; i_rp++) {
			    NodeId rTpId = iNodeId(rb, off);	//referenceTypeId
			    bool inv = iNu(rb, off, 1);		//isInverse
			    iNu(rb, off, 1);			//includeSubtypes
			    string tNm = iSqlf(rb, off);	//targetName

			    //    Browse request for nodes
			    req.clear()->setAttr("node", sN.toAddr())->
					 setAttr("BrDir", uint2str(inv?BD_INVERSE:BD_FORWARD))->
					 setAttr("RefTpId", rTpId.toAddr());
			    wep->reqData(OpcUa_BrowseRequest, req);
			    unsigned i_ref;
			    for(i_ref = 0; i_ref < req.childSize(); i_ref++)
				if(req.childGet(i_ref)->attr("name") == tNm) {
				    sN = NodeId::fromAddr(req.childGet(i_ref)->attr("NodeId"));
				    break;
				}
			    nOK = (i_ref < req.childSize());
			}

			//   Path result
			oNu(respEp, (nOK?0:OpcUa_BadNoMatch), 4);	//<< statusCode, 0x806f0000 (BadNoMatch)
			oNu(respEp, (nOK?1:0), 4);			//<< targets [], 0
			if(nOK) {
			    oNodeId(respEp, sN);		//ExpandedNodeId
			    oNu(respEp, 0xFFFFFFFF, 4);		//Index
			}
		    }
		    oN(respEp, 0, 4);			//diagnosticInfos []
		    break;
		}
		case OpcUa_BrowseRequest:
		{
		    XML_N req("data");
		    //  Request
							//> view
		    iNodeId(rb, off);			//viewId
		    iTm(rb, off);			//timestamp
		    iNu(rb, off, 4);			//viewVersion

		    uint32_t rPn = iNu(rb, off, 4);	//requestedMax ReferencesPerNode
							//> nodesToBrowse
		    uint32_t nc = iNu(rb, off, 4);	//Nodes

		    //  Respond
							//> results []
		    oNu(respEp, nc, 4);			//Nodes

		    //   Nodes list processing
		    for(uint32_t i_c = 0; i_c < nc; i_c++) {
			NodeId nid = iNodeId(rb, off);		//nodeId
			uint32_t bd = iNu(rb, off, 4);		//browseDirection
			NodeId rtId = iNodeId(rb, off);		//referenceTypeId
			iNu(rb, off, 1);			//includeSubtypes
			uint32_t nClass = iNu(rb, off, 4);	//nodeClassMask
			uint32_t resMask = iNu(rb, off, 4);	//resultMask

			uint32_t stCode = 0, refNumb = 0;
			int stCodeOff = respEp.size();	oNu(respEp, stCode, 4);	//statusCode
			int cPntOff = respEp.size();	oS(respEp, "");		//continuationPoint
			int refNumbOff = respEp.size();	oNu(respEp, refNumb, 4);//References [] = 0

			if(rtId.numbVal() && rtId.numbVal() != OpcUa_HierarchicalReferences && rtId.numbVal() != OpcUa_References) continue;

			req.clear()->setAttr("node", nid.toAddr())->setAttr("BrDir", uint2str(bd))->setAttr("RefTpId", rtId.toAddr())->
				    setAttr("ClassMask", uint2str(nClass))->setAttr("rPn", uint2str(rPn));
			stCode = wep->reqData(reqTp, req);
			refNumb = req.childSize();
			for(unsigned i_ref = 0; !stCode && i_ref < req.childSize(); i_ref++)
			{
			    XML_N *chN = req.childGet(i_ref);
			    oRef(respEp, resMask, NodeId::fromAddr(chN->attr("NodeId")),
				NodeId::fromAddr(chN->attr("referenceTypeId")),
				atoi(chN->attr("dir").c_str()), chN->attr("name"), atoi(chN->attr("NodeClass").c_str()),
				NodeId::fromAddr(chN->attr("typeDefinition")));
			}

			if(stCode)	oNu(respEp, stCode, 4, stCodeOff);
			if(rPn && refNumb >= rPn && req.attr("LastNode").size()) //continuationPoint prepare
			{
			    string cP;
			    while(!wep->sessCpGet(sesTokId,(cP=randBytes(16))).empty()) ;
			    wep->sessCpSet(sesTokId, cP, Sess::ContPoint(nid.toAddr(),req.attr("LastNode"),bd,rPn,rtId.toAddr(),nClass,resMask));
			    oS(respEp, cP, cPntOff);
			    refNumbOff += cP.size();
			}
			if(refNumb)	oNu(respEp, refNumb, 4, refNumbOff);
		    }
		    oS(respEp, "");			//diagnosticInfos []

		    reqTp = OpcUa_BrowseResponse;

		    break;
		}
		case OpcUa_BrowseNextRequest:
		{
		    XML_N req("data");
		    iNu(rb, off, 1);			//releaseContinuationPoints
		    uint32_t nCp = iNu(rb, off, 4);	//continuationPoints []

		    //  Respond
							//> results []
		    oNu(respEp, nCp, 4);		//continuationPoints

		    //   Continuation points list processing
		    for(uint32_t i_cp = 0; i_cp < nCp; i_cp++) {
			string cp = iS(rb, off);	//continuationPoint

			uint32_t stCode = 0, refNumb = 0;
			int stCodeOff = respEp.size();	oNu(respEp, stCode, 4);	//statusCode
			int cPntOff = respEp.size();	oS(respEp, "");		//continuationPoint
			int refNumbOff = respEp.size();	oNu(respEp, refNumb, 4);//References [] = 0
			Sess::ContPoint cPo = wep->sessCpGet(sesTokId, cp);

			if(!cPo.empty()) {
			    req.clear()->setAttr("node", cPo.brNode)->setAttr("LastNode", cPo.lstNode)->
					setAttr("BrDir", uint2str(cPo.brDir))->setAttr("RefTpId", cPo.refTypeId)->
					setAttr("ClassMask", uint2str(cPo.nClassMask))->setAttr("rPn", uint2str(cPo.refPerN));
			    stCode = wep->reqData(reqTp, req);
			    refNumb = req.childSize();
			    for(unsigned i_ref = 0; !stCode && i_ref < req.childSize(); i_ref++)
			    {
				XML_N *chN = req.childGet(i_ref);
				oRef(respEp, cPo.resMask, NodeId::fromAddr(chN->attr("NodeId")),
				    NodeId::fromAddr(chN->attr("referenceTypeId")),
				    atoi(chN->attr("dir").c_str()), chN->attr("name"), atoi(chN->attr("NodeClass").c_str()),
				    NodeId::fromAddr(chN->attr("typeDefinition")));
			    }
			    wep->sessCpSet(sesTokId, cp);	//Free previous continuationPoint. Unknown using "rCp" here!!!!
			}
			else stCode = OpcUa_BadContinuationPointInvalid;

			if(stCode)	oNu(respEp, stCode, 4, stCodeOff);
			if(refNumb >= cPo.refPerN && req.attr("LastNode").size()) //continuationPoint prepare
			{
			    string cP;
			    while(!wep->sessCpGet(sesTokId,(cP=randBytes(16))).empty()) ;
			    wep->sessCpSet(sesTokId, cP,
				Sess::ContPoint(cPo.brNode,req.attr("LastNode"),cPo.brDir,cPo.refPerN,cPo.refTypeId,cPo.nClassMask,cPo.resMask));
			    oS(respEp, cP, cPntOff);
			    refNumbOff += cP.size();
			}
			if(refNumb)	oNu(respEp, refNumb, 4, refNumbOff);
		    }
		    oS(respEp, "");			//diagnosticInfos []

		    reqTp = OpcUa_BrowseNextResponse;

		    break;
		}
		case OpcUa_ReadRequest:
		{
		    XML_N req("data");
		    //  Request
		    iR(rb, off, 8);				//maxAge
		    uint32_t tmStRet = iNu(rb, off, 4);		//timestampsTo Return
								//> nodesToRead []
		    uint32_t nc = iNu(rb, off, 4);		//nodes
		    uint8_t eMsk = 0x01;
		    switch(tmStRet)
		    {
			case TS_SOURCE: eMsk |= 0x04;   break;
			case TS_SERVER: eMsk |= 0x08;   break;
			case TS_BOTH:   eMsk |= 0x0C;   break;
		    }

		    //  Respond
		    oNu(respEp, nc, 4);				//Numbers
		    //   Nodes list processing
		    for(uint32_t i_c = 0; i_c < nc; i_c++) {
			NodeId nid = iNodeId(rb, off);		//nodeId
			uint32_t aid = iNu(rb, off, 4);		//attributeId
			iS(rb, off);				//indexRange
			iSqlf(rb, off);				//dataEncoding

			req.setAttr("node", nid.toAddr())->setAttr("aid",uint2str(aid));
			int rez = wep->reqData(reqTp, req);
			if(!rez) oDataValue(respEp, eMsk, req.text(), atoi(req.attr("type").c_str()));
			else oDataValue(respEp, 0x02, int2str(rez));
		    }

		    oS(respEp, "");				//diagnosticInfos []

		    reqTp = OpcUa_ReadResponse;

		    break;
		}
		case OpcUa_WriteRequest:
		{
		    XML_N req("data");
		    //  Request
							//> nodesToWrite []
		    uint32_t nc = iNu(rb, off, 4);	//nodes

		    //  Respond
		    oNu(respEp, nc, 4);			//Numbers
		    for(unsigned i_n = 0; i_n < nc; i_n++) {
			NodeId nid = iNodeId(rb, off);	//nodeId
			uint32_t aid = iNu(rb, off, 4);	//attributeId (Value)
			iS(rb, off);			//indexRange
			XML_N nVal;
			iDataValue(rb, off, nVal);	//value

			req.setAttr("node", nid.toAddr())->setAttr("aid",uint2str(aid))->setText(nVal.text());
			int rez = wep->reqData(reqTp, req);

			//   Write result status code
			oNu(respEp, rez, 4);		// StatusCode
		    }
		    oS(respEp, "");			//diagnosticInfos []

		    reqTp = OpcUa_WriteResponse;

		    break;
		}
		case OpcUa_PublishRequest:
		{
		    pthread_mutex_lock(&wep->mtxData);

		    //  The publish request queue and/or process
		    Sess *s = wep->sessGet_(sesTokId);
		    if(s) {
			unsigned i_p = 0;
			bool findOK = false;
			for( ; i_p < s->publishReqs.size(); ++i_p)
			    if((findOK=(rba.compare(0,mSz,s->publishReqs[i_p])==0))) break;
			if(i_p >= s->publishReqs.size()) s->publishReqs.push_back(rba.substr(0,mSz));
			if(findOK || s->publishReqs.size() == 1) {
			    unsigned prSS = wep->mSubScr.size();
			    for(unsigned i_ss = 0; i_ss < wep->mSubScr.size(); ++i_ss)
				if((wep->mSubScr[i_ss].st == SS_LATE || wep->mSubScr[i_ss].st == SS_KEEPALIVE) &&
					(prSS == wep->mSubScr.size() || wep->mSubScr[i_ss].pr > wep->mSubScr[prSS].pr))
				    prSS = i_ss;
			    if(prSS < wep->mSubScr.size()) {
				Subscr &ss = wep->mSubScr[prSS];

				//   Request
				string respAck;
				int32_t sa = iN(rb, off, 4);			//>subscription Acknowledgements []
				oN(respAck, sa, 4);				//<results []
				for(int i_a = 0; i_a < sa; i_a++) {
				    uint32_t prSSAck = iNu(rb, off, 4);		//> subscriptionId
				    uint32_t seqN = iNu(rb, off, 4);		//> sequenceNumber
				    uint32_t st = OpcUa_BadSequenceNumberUnknown;
				    if((--prSSAck) < wep->mSubScr.size())
					for(deque<string>::iterator iRQ = wep->mSubScr[prSSAck].retrQueue.begin();
						iRQ != wep->mSubScr[prSSAck].retrQueue.end(); ++iRQ)
					{
					    int rOff = 0;
					    if(iNu(*iRQ,rOff,4) == seqN) { wep->mSubScr[prSSAck].retrQueue.erase(iRQ); st = 0; break; }
					}
				    oNu(respAck, st, 4);			//< results
				}

				respEp.reserve(100);
				oNu(respEp, prSS+1, 4);				//<subscriptionId

				if(ss.st == SS_LATE) {
				    ss.setState(SS_NORMAL);

				    int aSeqOff = respEp.size(), aSeqN = 1;
				    oN(respEp, aSeqN, 4);	//<availableSequence Numbers [], reserve
				    for(deque<string>::iterator iRQ = ss.retrQueue.begin(); iRQ != ss.retrQueue.end(); )
				    {
					int rOff = 0;
					uint32_t rSeq = iNu(*iRQ, rOff, 4);	//>sequenceNumber
					int64_t rPblTm = iTm(*iRQ, rOff);	//>publishTime
					//Check for remove from queue by long age
					if((curTime()-rPblTm) > ((int64_t)ss.cntrKeepAlive*ss.publInterv*1000))
					{
					    iRQ = ss.retrQueue.erase(iRQ);
					    continue;
					}
					oNu(respEp, rSeq, 4);			//< sequenceNumber
					++iRQ;
					aSeqN++;
				    }
				    oN(respEp, aSeqN, 4, aSeqOff);		//<availableSequence Numbers [], real
				    oNu(respEp, ss.seqN, 4);			//< sequenceNumber, current

				    int moreNtfOff = respEp.size();
				    oNu(respEp, 0, 1);				//<moreNotifications, FALSE
				    int ntfMsgOff = respEp.size();		//<notificationMessage
				    oNu(respEp, ss.seqN, 4);			//< sequenceNumber
				    oTm(respEp, curTime());			//< publishTime
				    //int nNtfOff = respEp.size();
				    oN(respEp, 1, 4);				//< notificationData []
				    oNodeId(respEp, NodeId(OpcUa_DataChangeNotification));	//<  TypeId
				    oNu(respEp, 1, 1);				//<  encodingMask
				    int extObjOff = respEp.size();
				    oNu(respEp, 0, 4);				//<  extension object size
				    int mItOff = respEp.size();
				    oN(respEp, 0, 4);				//<  monitoredItems []
				    unsigned i_mIt = 0;
				    bool maxNotPerPublLim = false;
				    for(unsigned i_m = 0; !maxNotPerPublLim && i_m < ss.mItems.size(); i_m++)
				    {
					Subscr::MonitItem &mIt = ss.mItems[i_m];
					if(!(mIt.md == MM_REPORTING && mIt.vQueue.size())) continue;
					uint8_t eMsk = 0x01;
					switch(mIt.tmToRet)
					{
					    case TS_SOURCE: eMsk |= 0x04;	break;
					    case TS_SERVER: eMsk |= 0x08;	break;
					    case TS_BOTH:   eMsk |= 0x0C;	break;
					    default:				break;
					}
					while(mIt.vQueue.size()) {
					    if(ss.maxNotPerPubl && i_mIt >= ss.maxNotPerPubl) { maxNotPerPublLim = true; break; }
					    oNu(respEp, mIt.cH, 4);		//<   clientHandle
					    oDataValue(respEp, eMsk, mIt.vQueue.front().vl, mIt.vTp, mIt.vQueue.front().tm);	//<   value
					    mIt.vQueue.pop_front();
					    i_mIt++;
					}
				    }
				    oS(respEp, "");				//<   diagnosticInfos []
				    oNu(respEp, maxNotPerPublLim, 1, moreNtfOff);//<moreNotifications, real value write
				    oN(respEp, i_mIt, 4, mItOff);		//<  monitoredItems [], real items number write
				    oNu(respEp, respEp.size()-extObjOff-4, 4, extObjOff);	//<  extension object real size write

				    ss.retrQueue.push_back(respEp.substr(ntfMsgOff));	//Queue to retranslation
				    ss.seqN++;
				}
				else if(ss.st == SS_KEEPALIVE) {
				    ss.setState(SS_NORMAL);
				    oN(respEp, 0, 4);			//<availableSequence Numbers []
				    oNu(respEp, 0, 1);			//moreNotifications, FALSE
									//notificationMessage
				    oNu(respEp, ss.seqN, 4);		// sequenceNumber
				    oTm(respEp, curTime());		// publishTime
				    oN(respEp, 0, 4);			// notificationData []
				}
				s->publishReqs.erase(s->publishReqs.begin()+i_p);	//Remove the publish request from queue

				//oN(respEp, -1, 4);			//<results [], empty
				respEp += respAck;			//<results []
				oS(respEp, "");				//<diagnosticInfos []
			    }
			}
		    }
		    pthread_mutex_unlock(&wep->mtxData);

		    if(respEp.empty())	throw OPCError(0, "", "");	//> No response now

		    reqTp = OpcUa_PublishResponse;

		    break;
		}
		case OpcUa_RepublishRequest:
		{
		    //  Request
		    uint32_t prSS = iNu(rb, off, 4);			//> subscriptionId
		    uint32_t seqN = iNu(rb, off, 4);			//> retransmitSequenceNumber
		    if(wep->subscrGet(prSS).st == SS_CLOSED) { stCode = OpcUa_BadSubscriptionIdInvalid; reqTp = OpcUa_ServiceFault; break; }

		    pthread_mutex_lock(&wep->mtxData);
		    Subscr &ss = wep->mSubScr[prSS-1];
		    deque<string>::iterator iRQ = ss.retrQueue.begin();
		    for( ; iRQ != ss.retrQueue.end(); ++iRQ) {
			int rOff = 0;
			if(iNu(*iRQ,rOff,4) == seqN)	break;
		    }
		    if(iRQ == ss.retrQueue.end()) {
			stCode = OpcUa_BadSubscriptionIdInvalid; reqTp = OpcUa_ServiceFault;
			pthread_mutex_unlock(&wep->mtxData);
			break;
		    }

		    //  Respond
		    reqTp = OpcUa_RepublishResponse;
		    respEp = *iRQ;

		    pthread_mutex_unlock(&wep->mtxData);
		    break;
		}
		case OpcUa_ServiceFault:	break;
		default:
		    throw OPCError(OpcUa_BadNotSupported, "No supported request id '%d'.", reqTp);
	    }

	    out.reserve(200);
	    out.append("MSGF");					//OpenSecureChannel message type
	    oNu(out, 0, 4);					//Message size
	    oNu(out, secId, 4);					//Secure channel identifier
	    oNu(out, tokId, 4);					//Symmetric Algorithm Security Header : TokenId
	    int begEncBlck = out.size();
								//> Sequence header
	    oNu(out, seqN, 4);					//Sequence number
	    oNu(out, reqId, 4);					//RequestId
								//> Extension Object
	    oNodeId(out, reqTp);				//TypeId
								//>> Body
								//>>> RespondHeader
	    oTm(out, curTime());				//timestamp
	    oN(out, reqHndl, 4);				//requestHandle
	    oN(out, stCode, 4);					//StatusCode
	    oN(out, 0, 1);					//serviceDiagnostics
	    oS(out, "");					//stringTable
								//>>> Extensible parameter
	    oNodeId(out, 0u);					//TypeId (0)
	    oNu(out, 0, 1);					//Encoding
	    out.append(respEp);
	    oNu(out, out.size(), 4, 4);				//Real message size

	    if(scHd.secMessMode == MS_Sign || scHd.secMessMode == MS_SignAndEncrypt)
	    {
		//Padding place
		if(scHd.secMessMode == MS_SignAndEncrypt) {
		    int kSz = scHd.clKey.size()/3;
		    int paddingSize = ((out.size()-begEncBlck+1+20+kSz-1)/kSz)*kSz-(out.size()-begEncBlck+20);
		    out += string(paddingSize, (char)(paddingSize-1));
		}

		//Real message size calc and place
		oNu(out, out.size()+20, 4, 4);

		//Signature
		out += symmetricSign(out, scHd.clKey, scHd.secPolicy);

		//Encoding
		if(scHd.secMessMode == MS_SignAndEncrypt)
		    out.replace(begEncBlck, out.size()-begEncBlck, symmetricEncrypt(out.substr(begEncBlck),scHd.clKey,scHd.secPolicy));
	    }

	    if(dbg) debugMess(strMess("MSG Resp: %d",reqTp));
	}
	else throw OPCError(OpcUa_BadNotSupported, "", "");
    }
    catch(OPCError er) {
	if(dbg) debugMess(strMess("MSG Error: %xh:%s",er.cod,er.mess.c_str()));
	if(er.cod) { out = mkError(er.cod, er.mess); holdConn = false; }
	mSz = rba.size();	//Drop request for prevent broken requests hang
    }

    if(answ) answ->append(out);
    else writeToClient(inPrtId, out);

    rba.erase(0, mSz);
    goto nextReq;

    return true;
}

//*************************************************
//* SecCnl					  *
//*************************************************
Server::SecCnl::SecCnl( const string &iEp, uint32_t iTokenId, int32_t iLifeTm,
	const string &iClCert, const string &iSecPolicy, char iSecMessMode ) :
    endPoint(iEp), secPolicy(iSecPolicy), secMessMode(iSecMessMode), tCreate(curTime()),
    tLife(std::max(600000,iLifeTm)), TokenId(iTokenId), TokenIdPrev(0), clCert(iClCert)
{

}

Server::SecCnl::SecCnl( ) : secMessMode(MS_None), tCreate(curTime()), tLife(600000), TokenId(0), TokenIdPrev(0)
{

}

//*************************************************
//* Server::Sess				  *
//*************************************************
Server::Sess::Sess( const string &iName, double iTInact ) : name(iName), tInact(std::max(iTInact,1.0)), tAccess(curTime())
{

}

Server::Sess::Sess( ) : tInact(0), tAccess(0)	{ }

//*************************************************
//* Server::Subscr                                *
//*************************************************
Server::Subscr Server::Subscr::copy( bool noWorkData )
{
    Subscr rez;
    if(!noWorkData) rez = *this;
    else
    {
	rez.st = st;
	rez.sess = sess;
	rez.en = en;
	rez.publInterv = publInterv;
	rez.cntrLifeTime = cntrLifeTime;
	rez.cntrKeepAlive = cntrKeepAlive;
	rez.maxNotPerPubl = maxNotPerPubl;
	rez.pr = pr;
    }

    return rez;
}

SubScrSt Server::Subscr::setState( SubScrSt ist )
{
    if(ist == st) return st;
    switch(ist)
    {
	case SS_CUR:	return st;
	case SS_CLOSED:	//Clear the object
	    mItems.clear();
	    retrQueue.clear();
	    en = false;
	    seqN = 1;
	    wLT = wKA = 0;
	    break;
	case SS_NORMAL:
	    switch(st)
	    {
		case SS_KEEPALIVE: wKA = 0; break;
		default: break;
	    }
	    break;
	case SS_CREATING: case SS_LATE: case SS_KEEPALIVE:
	    break;
    }

    st = ist;

    return st;
}

//*************************************************
//* Server::EP					  *
//*************************************************
Server::EP::EP( Server *iserv ) : mEn(false), cntReq(0), objTree("root"), serv(iserv)
{
    pthread_mutex_init(&mtxData, NULL);
}

Server::EP::~EP( )
{
    pthread_mutex_lock(&mtxData);
    pthread_mutex_destroy(&mtxData);
}

void Server::EP::setEnable( bool vl )
{
    if(mEn == vl) return;

    cntReq = 0;

    if(vl)
    {
     //Objects tree and nodes map init
     nodeReg(0u,OpcUa_RootFolder,"Root",NC_Object,OpcUa_Organizes,OpcUa_FolderType);
      nodeReg(OpcUa_RootFolder,OpcUa_ViewsFolder,"Views",NC_Object,OpcUa_Organizes,OpcUa_FolderType);
      nodeReg(OpcUa_RootFolder,OpcUa_ObjectsFolder,"Objects",NC_Object,OpcUa_Organizes,OpcUa_FolderType);
       nodeReg(OpcUa_ObjectsFolder,OpcUa_Server,"Server",NC_Object,OpcUa_Organizes,OpcUa_ServerType);
	nodeReg(OpcUa_Server,OpcUa_Server_ServerStatus,"ServerStatus",NC_Variable,OpcUa_HasComponent,OpcUa_ServerStatusType)->
	    setAttr("Value","Running")->setAttr("DataType",int2str(OpcUa_String/*OpcUa_ServerStatusDataType*/));
	 nodeReg(OpcUa_Server_ServerStatus,OpcUa_Server_ServerStatus_State,"State",NC_Variable,OpcUa_HasComponent,OpcUa_BaseDataVariableType)->
	    setAttr("Value","0")->setAttr("DataType",int2str(OpcUa_Int32));
	nodeReg(OpcUa_Server,OpcUa_Server_NamespaceArray,"NamespaceArray",NC_Variable,OpcUa_HasProperty,OpcUa_PropertyType)->
	    setAttr("ValueRank","1")->setAttr("Value","http://opcfoundation.org/UA/\n"+serv->applicationUri())->setAttr("DataType",int2str(0x80|OpcUa_String));
	nodeReg(OpcUa_Server,OpcUa_Server_ServerArray,"ServerArray",NC_Variable,OpcUa_HasProperty,OpcUa_PropertyType)->
	    setAttr("ValueRank","1")->setAttr("Value",serv->applicationUri())->setAttr("DataType",int2str(0x80|OpcUa_String));
      nodeReg(OpcUa_RootFolder,OpcUa_TypesFolder,"Types",NC_Object,OpcUa_Organizes,OpcUa_FolderType);
       nodeReg(OpcUa_TypesFolder,OpcUa_ObjectTypesFolder,"ObjectTypes",NC_Object,OpcUa_Organizes,OpcUa_FolderType);
	nodeReg(OpcUa_ObjectTypesFolder,OpcUa_BaseObjectType,"BaseObjectType",NC_ObjectType,OpcUa_Organizes);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_BaseEventType,"BaseEventType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_ServerCapabilitiesType,"ServerCapabilitiesType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_ServerDiagnosticsType,"ServerDiagnosticsType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_SessionsDiagnosticsSummaryType,"SessionsDiagnosticsSummaryType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_SessionDiagnosticsObjectType,"SessionDiagnosticsObjectType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_VendorServerInfoType,"VendorServerInfoType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_ServerRedundancyType,"ServerRedundancyType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_ModellingRuleType,"ModellingRuleType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_FolderType,"FolderType",NC_ObjectType,OpcUa_HasTypeDefinition);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_DataTypeEncodingType,"DataTypeEncodingType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_DataTypeSystemType,"DataTypeSystemType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_StateType,"StateType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_TransitionType,"TransitionType",NC_ObjectType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseObjectType,OpcUa_ServerType,"ServerType",NC_ObjectType,OpcUa_HasSubtype);
       nodeReg(OpcUa_TypesFolder,OpcUa_VariableTypesFolder,"VariableTypes",NC_Object,OpcUa_Organizes,OpcUa_FolderType);
	nodeReg(OpcUa_VariableTypesFolder,OpcUa_BaseVariableType,"BaseVariableType",NC_VariableType,OpcUa_Organizes);
	 nodeReg(OpcUa_BaseVariableType,OpcUa_BaseDataVariableType,"BaseDataVariableType",NC_VariableType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseVariableType,OpcUa_PropertyType,"PropertyType",NC_VariableType,OpcUa_HasSubtype);
       nodeReg(OpcUa_TypesFolder,OpcUa_ReferenceTypesFolder,"ReferenceTypes",NC_Object,OpcUa_Organizes,OpcUa_FolderType);
	nodeReg(OpcUa_ReferenceTypesFolder,OpcUa_References,"References",NC_ReferenceType,OpcUa_Organizes,OpcUa_FolderType)->setAttr("IsAbstract","1")->setAttr("Symmetric","1");
	 nodeReg(OpcUa_References,OpcUa_HierarchicalReferences,"HierarchicalReferences",NC_ReferenceType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	  nodeReg(OpcUa_HierarchicalReferences,OpcUa_HasChild,"HasChild",NC_ReferenceType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	   nodeReg(OpcUa_HasChild,OpcUa_Aggregates,"Aggregates",NC_ReferenceType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	    nodeReg(OpcUa_Aggregates,OpcUa_HasComponent,"HasComponent",NC_ReferenceType,OpcUa_HasSubtype)->setAttr("InverseName","ComponentOf");
	    nodeReg(OpcUa_Aggregates,OpcUa_HasProperty,"HasProperty",NC_ReferenceType,OpcUa_HasSubtype)->setAttr("InverseName","PropertyOf");
	   nodeReg(OpcUa_HasChild,OpcUa_HasSubtype,"HasSubtype",NC_ReferenceType,OpcUa_HasSubtype)->setAttr("InverseName","SubtypeOf");
	  nodeReg(OpcUa_HierarchicalReferences,OpcUa_HasEventSource,"HasEventSource",NC_ReferenceType,OpcUa_HasSubtype)->setAttr("InverseName","EventSourceOf");
	  nodeReg(OpcUa_HierarchicalReferences,OpcUa_Organizes,"Organizes",NC_ReferenceType,OpcUa_HasSubtype)->setAttr("InverseName","OrganizedBy");
	 nodeReg(OpcUa_References,OpcUa_NonHierarchicalReferences,"NonHierarchicalReferences",NC_ReferenceType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	  nodeReg(OpcUa_NonHierarchicalReferences,OpcUa_HasTypeDefinition,"HasTypeDefinition",NC_ReferenceType,OpcUa_HasSubtype)->setAttr("InverseName","Defines");
       nodeReg(OpcUa_TypesFolder,OpcUa_DataTypesFolder,"DataTypes",NC_Object,OpcUa_Organizes,OpcUa_FolderType);
	nodeReg(OpcUa_DataTypesFolder,OpcUa_BaseDataType,"BaseDataType",NC_DataType,OpcUa_Organizes)->setAttr("IsAbstract","1");
	 nodeReg(OpcUa_BaseDataType,OpcUa_Boolean,"Boolean",NC_DataType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseDataType,OpcUa_Number,"Number",NC_DataType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	  nodeReg(OpcUa_Number,OpcUa_Integer,"Integer",NC_DataType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	   nodeReg(OpcUa_Integer,OpcUa_SByte,"SByte",NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_Integer,OpcUa_Int16,"Int16",NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_Integer,OpcUa_Int32,"Int32",NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_Integer,OpcUa_Int64,"Int64",NC_DataType,OpcUa_HasSubtype);
	  nodeReg(OpcUa_Number,OpcUa_UInteger,"UInteger",NC_DataType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	   nodeReg(OpcUa_UInteger,OpcUa_Byte,"Byte",NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_UInteger,OpcUa_UInt16,"UInt16",NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_UInteger,OpcUa_UInt32,"UInt32",NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_UInteger,OpcUa_UInt64,"UInt64",NC_DataType,OpcUa_HasSubtype);
	  nodeReg(OpcUa_Number,OpcUa_Double,"Double",NC_DataType,OpcUa_HasSubtype);
	  nodeReg(OpcUa_Number,OpcUa_Float,"Float",NC_DataType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseDataType,OpcUa_String,"String",NC_DataType,OpcUa_HasSubtype);
    }
    else
    {
	ndMap.clear();
	objTree.clear();
    }

    mEn = vl;
}

void Server::EP::subScrCycle( unsigned cntr )
{
    pthread_mutex_lock(&mtxData);

    //Subscription process
    vector<int>	sls;
    Sess *s = NULL;
    int64_t vTm = 0;
    for(unsigned i_sc = 0; i_sc < mSubScr.size(); i_sc++)
    {
	Subscr &scr = mSubScr[i_sc];
	if(scr.st == SS_CLOSED) continue;
	if(!(s=sessGet_(scr.sess)) || !s->tAccess) { scr.setState(SS_CLOSED); continue; }
	// Monitored items processing
	bool hasData = false;
	XML_N req("data");
	for(unsigned i_m = 0; i_m < scr.mItems.size(); ++i_m)
	{
	    Subscr::MonitItem &mIt = scr.mItems[i_m];
	    if(mIt.md == MM_DISABLED || (cntr%(unsigned)(mIt.smplItv/subscrProcPer())))	continue;
	    //  Read data
	    req.setAttr("node", mIt.nd.toAddr())->setAttr("aid", uint2str(mIt.aid))->setAttr("dtTmGet","1");
		//setAttr("dtTmGet",(mIt.tmToRet==TS_SOURCE||mIt.tmToRet==TS_BOTH)?"1":"0");
	    if(!reqData(OpcUa_ReadRequest, req) && (vTm=strtoll(req.attr("dtTm").c_str(),NULL,10)) > mIt.dtTm)
	    {
		mIt.vTp = atoi(req.attr("type").c_str());
		mIt.dtTm = vTm;
		mIt.vQueue.push_back(Subscr::MonitItem::Val(req.text(),vTm));
		if(mIt.vQueue.size() > mIt.qSz)
		{
		    if(mIt.dO) mIt.vQueue.pop_front();
		    else mIt.vQueue.pop_back();
		}
		hasData = true;
	    }
	}
	if(hasData) scr.setState(SS_LATE);
	// Publish processing
	if((cntr%(unsigned)(scr.publInterv/subscrProcPer())))	continue;
	if(s->publishReqs.size())
	{
	    scr.wLT = 0;
	    if(scr.st == SS_LATE)			{ scr.wKA = 0; sls.push_back(scr.sess); }
	    else if((++scr.wKA) >= scr.cntrKeepAlive)	{ scr.setState(SS_KEEPALIVE); sls.push_back(scr.sess); }
	}
	else if((++scr.wLT) >= scr.cntrLifeTime)
	{
	    // Send StatusChangeNotification with Bad_Timeout
	    //????
	    scr.setState(SS_CLOSED);	//Free Subscription
	}
    }

    //Publish call
    for(size_t i_s = 0; i_s < sls.size(); i_s++)
    {
	if(!(s=sessGet_(sls[i_s])) || s->publishReqs.empty()) continue;
	string req = s->publishReqs.front(), inPrt = s->inPrtId;
	pthread_mutex_unlock(&mtxData);
	serv->inReq(req, inPrt);
	pthread_mutex_lock(&mtxData);
    }

    pthread_mutex_unlock(&mtxData);
}

string Server::EP::secPolicy( int isec )
{
    string rez;
    pthread_mutex_lock(&mtxData);
    if(isec >= 0 && isec < (int)mSec.size()) rez = mSec[isec].policy;
    pthread_mutex_unlock(&mtxData);

    return rez;
}

MessageSecurityMode Server::EP::secMessageMode( int isec )
{
    MessageSecurityMode rez = (MessageSecurityMode)0;
    pthread_mutex_lock(&mtxData);
    if(isec >= 0 || isec < (int)mSec.size()) rez = mSec[isec].messageMode;
    pthread_mutex_unlock(&mtxData);

    return rez;
}

int Server::EP::sessCreate( const string &iName, double iTInact )
{
    int i_s;
    pthread_mutex_lock(&mtxData);
    for(i_s = 0; i_s < (int)mSess.size(); i_s++)
	if(!mSess[i_s].tAccess || 1e-3*(curTime()-mSess[i_s].tAccess) > mSess[i_s].tInact)
	    break;
    if(i_s < (int)mSess.size()) mSess[i_s] = Sess(iName, iTInact);
    else mSess.push_back(Sess(iName,iTInact));
    pthread_mutex_unlock(&mtxData);

    return i_s+1;
}

void Server::EP::sessServNonceSet( int sid, const string &servNonce )
{
    pthread_mutex_lock(&mtxData);
    if(sid > 0 && sid <= (int)mSess.size()) mSess[sid-1].servNonce = servNonce;
    pthread_mutex_unlock(&mtxData);
}

uint32_t Server::EP::sessActivate( int sid, uint32_t secCnl, bool check, const string &inPrtId, const XML_N &identTkn )
{
    uint32_t rez = OpcUa_BadSessionIdInvalid;

    pthread_mutex_lock(&mtxData);
    //Check for target session present
    if(sid > 0 && sid <= (int)mSess.size() && mSess[sid-1].tAccess)
    {
	mSess[sid-1].tAccess = curTime();
	mSess[sid-1].inPrtId = inPrtId;
	int i_s;
	for(i_s = 0; i_s < (int)mSess[sid-1].secCnls.size(); i_s++)
	    if(mSess[sid-1].secCnls[i_s] == secCnl)
		break;
	if(!check || i_s < (int)mSess[sid-1].secCnls.size())
	{
	    if(i_s >= (int)mSess[sid-1].secCnls.size()) mSess[sid-1].secCnls.push_back(secCnl);
	    rez = 0;
	}
    }
    pthread_mutex_unlock(&mtxData);

    return rez;
}

void Server::EP::sessClose( int sid )
{
    pthread_mutex_lock(&mtxData);
    if(sid > 0 && sid <= (int)mSess.size() && mSess[sid-1].tAccess) mSess[sid-1] = Sess();
    pthread_mutex_unlock(&mtxData);
}

Server::Sess Server::EP::sessGet( int sid )
{
    Server::Sess rez, *ws;

    pthread_mutex_lock(&mtxData);
    if((ws=sessGet_(sid))) rez = *ws;
    pthread_mutex_unlock(&mtxData);

    return rez;
}

Server::Sess *Server::EP::sessGet_( int sid )
{
    if(sid > 0 && sid <= (int)mSess.size()) return &mSess[sid-1];

    return NULL;
}

Server::Sess::ContPoint Server::EP::sessCpGet( int sid, const string &cpId )
{
    Server::Sess::ContPoint rez;

    pthread_mutex_lock(&mtxData);
    map<string, Server::Sess::ContPoint>::iterator cpPrev;
    if(sid > 0 && sid <= (int)mSess.size() && (cpPrev=mSess[sid-1].cntPnts.find(cpId)) != mSess[sid-1].cntPnts.end())
	rez = cpPrev->second;
    pthread_mutex_unlock(&mtxData);

    return rez;
}

void Server::EP::sessCpSet( int sid, const string &cpId, const Server::Sess::ContPoint &cp )
{
    pthread_mutex_lock(&mtxData);
    if(sid > 0 && sid <= (int)mSess.size())
    {
	if(cp.empty() && mSess[sid-1].cntPnts.find(cpId) != mSess[sid-1].cntPnts.end())
	    mSess[sid-1].cntPnts.erase(cpId);
	else mSess[sid-1].cntPnts[cpId] = cp;
    }
    pthread_mutex_unlock(&mtxData);
}

uint32_t Server::EP::subscrSet( uint32_t ssId, SubScrSt st, bool en, int sess, double publInterv,
    uint32_t cntrLifeTime, uint32_t cntrKeepAlive, uint32_t maxNotPerPubl, int pr )
{
    pthread_mutex_lock(&mtxData);

    if(ssId == 0 || ssId > mSubScr.size())
    {
	uint32_t nSubScrPerSess = 0;
	ssId = mSubScr.size();

	//Find for Subscription on CLOSED state for reusing and subscriptions per session calculation
	for(uint32_t i_ss = 0; i_ss < mSubScr.size(); i_ss++)
	{
	    if(ssId >= mSubScr.size() && mSubScr[i_ss].st == SS_CLOSED) ssId = i_ss;
	    if(sess >= 0 && mSubScr[i_ss].sess == sess) nSubScrPerSess++;
	}
	if(nSubScrPerSess >= limSubScr()) { pthread_mutex_unlock(&mtxData); return 0; }
	if(ssId >= mSubScr.size()) { ssId = mSubScr.size(); mSubScr.push_back(Subscr()); }
    }
    else ssId--;

    Subscr &ss = mSubScr[ssId];

    //Set parameters
    ss.en = en;
    if(sess >= 0) ss.sess = sess;
    if(publInterv != 0) ss.publInterv = ceil(publInterv/subscrProcPer())*subscrProcPer();
    if(cntrKeepAlive != 0) ss.cntrKeepAlive = cntrKeepAlive;
    if(cntrLifeTime != 0) ss.cntrLifeTime = std::max(ss.cntrKeepAlive*3,cntrLifeTime);
    if(maxNotPerPubl != OpcUa_NPosID) ss.maxNotPerPubl = maxNotPerPubl;
    if(pr < 0) ss.pr = pr;
    ss.setState(st);

    pthread_mutex_unlock(&mtxData);

    return ssId+1;
}

Server::Subscr Server::EP::subscrGet( uint32_t ssId, bool noWorkData )
{
    pthread_mutex_lock(&mtxData);

    Subscr ss;
    if((--ssId) < mSubScr.size()) ss = mSubScr[ssId].copy(noWorkData);

    pthread_mutex_unlock(&mtxData);

    return ss;
}

uint32_t Server::EP::mItSet( uint32_t ssId, uint32_t mItId, MonitoringMode md, const NodeId &nd, uint32_t aid,
    TimestampsToReturn tmToRet, double smplItv, uint32_t qSz, int8_t dO, uint32_t cH )
{
    pthread_mutex_lock(&mtxData);

    if((--ssId) >= mSubScr.size()) { pthread_mutex_unlock(&mtxData); return 0; }
    else
    {
	Subscr &ss = mSubScr[ssId];
	if((--mItId) >= ss.mItems.size())
	{
	    //Find for MonitItem on DISABLED state for reusing
	    for(mItId = 0; mItId < ss.mItems.size(); mItId++)
		if(ss.mItems[mItId].md == MM_DISABLED) break;
	    if(mItId >= ss.mItems.size()) { mItId = ss.mItems.size(); ss.mItems.push_back(Subscr::MonitItem()); }
	}
	Subscr::MonitItem &mIt = ss.mItems[mItId];
	if(md != MM_CUR)
	{
	    if(md == MM_DISABLED && md != mIt.md) mIt = Subscr::MonitItem();
	    mIt.md = md;
	}
	if(!nd.isNull())	mIt.nd = nd;
	if(aid != OpcUa_NPosID)	mIt.aid = aid;
	if(tmToRet != -1)	mIt.tmToRet = tmToRet;
	if(qSz != OpcUa_NPosID)	mIt.qSz = std::max(uint32_t(1), std::min(uint32_t(1000),qSz));	//!!!! Make upper limit configurable
	if(dO >= 0)		mIt.dO = dO;
	if(cH != OpcUa_NPosID)	mIt.cH = cH;

	//Checkings for data
	XML_N req("data");
	req.setAttr("node", mIt.nd.toAddr())->setAttr("aid", uint2str(mIt.aid))->setAttr("dtPerGet",(smplItv==0)?"1":"0");
	uint32_t rez = reqData(OpcUa_ReadRequest, req);
	if(rez == OpcUa_BadNodeIdUnknown)		mIt.nd = NodeId();
	else if(rez == OpcUa_BadAttributeIdInvalid)	mIt.aid = Aid_Error;
	if(smplItv == 0)  smplItv = atof(req.attr("dtPer").c_str())*1000;
	if(smplItv == -1) smplItv = ss.publInterv;
	if(smplItv != -2) mIt.smplItv = ceil(std::max(smplItv,subscrProcPer())/subscrProcPer())*subscrProcPer();
    }

    pthread_mutex_unlock(&mtxData);

    return mItId+1;
}

Server::Subscr::MonitItem Server::EP::mItGet( uint32_t ssId, uint32_t mItId )
{
    pthread_mutex_lock(&mtxData);

    Subscr::MonitItem mIt;
    if((--ssId) < mSubScr.size() && (--mItId) < mSubScr[ssId].mItems.size()) mIt = mSubScr[ssId].mItems[mItId];

    pthread_mutex_unlock(&mtxData);

    return mIt;
}

XML_N *Server::EP::nodeReg( const NodeId &parent, const NodeId &ndId, const string &name,
	int ndClass, const NodeId &refTypeId, const NodeId &typeDef )
{
    XML_N *cNx = NULL;
    map<string,XML_N*>::iterator rN, cN;

    //Find already presented node
    cN = ndMap.find(ndId.toAddr());
    if(cN != ndMap.end()) cNx = cN->second;
    else
    {
	if(parent.isNull()) { cNx = &objTree; cNx->clear(); }
	else
	{
	    rN = ndMap.find(parent.toAddr());
	    if(rN == ndMap.end())
		throw OPCError("Parent node '%s' no present for node '%s'.",parent.toAddr().c_str(),ndId.toAddr().c_str());
	    cNx = rN->second->childAdd("nd");
	}
    }
    cNx->setAttr("NodeId", ndId.toAddr())->
	setAttr("name", name)->
	setAttr("NodeClass",int2str(ndClass))->
	setAttr("referenceTypeId", refTypeId.toAddr())->
	setAttr("typeDefinition", typeDef.toAddr());

    /*switch(ndClass)
    {
	case NC_Object:	cNx->setAttr("EventNotifier", "0");				break;
	case NC_ObjectType: case NC_DataType: cNx->setAttr("IsAbstract", "0");		break;
	case NC_ReferenceType: cNx->setAttr("IsAbstract", "0")->setAttr("Symmetric", "0");	break;
	case NC_VariableType: cNx->setAttr("IsAbstract", "0")->setAttr("DataType", "0:0")->setAttr("ValueRank", "-2");	break;
    }*/

    ndMap[ndId.toAddr()] = cNx;

    return cNx;
}

uint32_t Server::EP::reqData( int reqTp, XML_N &req )
{
    switch(reqTp)
    {
	case OpcUa_BrowseRequest: case OpcUa_BrowseNextRequest:
	{
	    NodeId nid = NodeId::fromAddr(req.attr("node"));
	    map<string, XML_N*>::iterator ndX = ndMap.find(nid.toAddr());
	    if(ndX == ndMap.end()) return OpcUa_BadBrowseNameInvalid;
	    int rPn = atoi(req.attr("rPn").c_str());
	    NodeId rtId = NodeId::fromAddr(req.attr("RefTpId"));
	    uint32_t bd = atoi(req.attr("BrDir").c_str());
	    uint32_t nClass = atoi(req.attr("ClassMask").c_str());
	    string lstNd = req.attr("LastNode"); req.setAttr("LastNode","");

	    //if(debug()) debugMess(strMess("Browse '%s'",req.save().c_str()));

	    //typeDefinition reference process
	    if(lstNd.empty() && rtId.numbVal() == OpcUa_References && (bd == BD_FORWARD || bd == BD_BOTH))       //!!!! Check for other call
	    {
		map<string, XML_N*>::iterator ndTpDef = ndMap.find(ndX->second->attr("typeDefinition"));
		if(ndTpDef != ndMap.end())
		{
		    unsigned cnClass = atoi(ndTpDef->second->attr("NodeClass").c_str());
		    if(!nClass || nClass&cnClass)
			req.childAdd("ref")->setAttr("NodeId", ndTpDef->second->attr("NodeId"))->
			    setAttr("referenceTypeId", ndTpDef->second->attr("referenceTypeId"))->
			    setAttr("dir", "1")->setAttr("name", ndTpDef->second->attr("name"))->
			    setAttr("NodeClass", int2str(cnClass))->setAttr("typeDefinition", ndTpDef->second->attr("typeDefinition"));
		}
	    }

	    //Inverse hierarchical references process (only parent)
	    if(lstNd.empty() && (bd == BD_INVERSE || bd == BD_BOTH) && ndX->second->parent())
	    {
		XML_N *chNd = ndX->second->parent();
		unsigned cnClass = atoi(chNd->attr("NodeClass").c_str());
		if(!nClass || nClass&cnClass)
		    req.childAdd("ref")->setAttr("NodeId", chNd->attr("NodeId"))->
			setAttr("referenceTypeId", chNd->attr("referenceTypeId"))->
			setAttr("dir", "0")->setAttr("name", chNd->attr("name"))->
			setAttr("NodeClass", int2str(cnClass))->setAttr("typeDefinition", chNd->attr("typeDefinition"));
	    }

	    //Forward hierarchical references process
	    bool lstOK = lstNd.empty() ? true : false;
	    for(unsigned i_ch = 0; (bd == BD_FORWARD || bd == BD_BOTH) && i_ch < ndX->second->childSize(); i_ch++)
	    {
		XML_N *chNd = ndX->second->childGet(i_ch);
		if(!lstOK) { lstOK = (lstNd==chNd->attr("NodeId")); continue; }
		unsigned cnClass = atoi(chNd->attr("NodeClass").c_str());
		if(nClass && !(nClass&cnClass)) continue;
		req.childAdd("ref")->setAttr("NodeId", chNd->attr("NodeId"))->
		    setAttr("referenceTypeId", chNd->attr("referenceTypeId"))->
		    setAttr("dir", "1")->setAttr("name", chNd->attr("name"))->
		    setAttr("NodeClass", int2str(cnClass))->setAttr("typeDefinition", chNd->attr("typeDefinition"));
		if(rPn && (int)req.childSize() >= rPn && (i_ch+1) < ndX->second->childSize())
		{
		    req.setAttr("LastNode", chNd->attr("NodeId"));
		    break;
		}
	    }
	    return 0;
	}
	case OpcUa_ReadRequest:
	{
	    NodeId nid = NodeId::fromAddr(req.attr("node"));
	    map<string, XML_N*>::iterator ndX = ndMap.find(nid.toAddr());
	    if(ndX == ndMap.end()) return OpcUa_BadNodeIdUnknown;
	    uint32_t aid = atoi(req.attr("aid").c_str());
	    switch(aid)
	    {
		// Generic attributes process
		case AId_NodeId: req.setAttr("type", int2str(OpcUa_NodeId))->setText(nid.toAddr());				return 0;
		case AId_NodeClass: req.setAttr("type", int2str(OpcUa_Int32))->setText(ndX->second->attr("NodeClass"));		return 0;
		case AId_BrowseName: req.setAttr("type", int2str(OpcUa_QualifiedName))->setText(ndX->second->attr("name"));	return 0;
		case AId_DisplayName:
		    req.setAttr("type", int2str(OpcUa_LocalizedText))->setText(ndX->second->attr(ndX->second->attr("DisplayName").empty()?"name":"DisplayName"), OpcUa_LocalizedText);
		    return 0;
		case AId_Descr: req.setAttr("type", int2str(OpcUa_LocalizedText))->setText(ndX->second->attr("Descr"));		return 0;
		case AId_WriteMask: case AId_UserWriteMask: req.setAttr("type", int2str(OpcUa_UInt32))->setText("0");		return 0;
		case AId_IsAbstract: req.setAttr("type", int2str(OpcUa_Boolean))->setText(ndX->second->attr("IsAbstract"));	return 0;
		case AId_Symmetric: req.setAttr("type", int2str(OpcUa_Boolean))->setText(ndX->second->attr("Symmetric"));	return 0;
		case AId_InverseName: req.setAttr("type", int2str(OpcUa_LocalizedText))->setText(ndX->second->attr("InverseName"));	return 0;
		case AId_EventNotifier: req.setAttr("type", int2str(OpcUa_Byte))->setText(ndX->second->attr("EventNotifier"));	return 0;
		default:
		{
		    string dtType = ndX->second->attr("DataType");
		    if(dtType.empty()) return OpcUa_BadAttributeIdInvalid;
		    // Values' attributes processing
		    switch(aid)
		    {
			case AId_Value:
			    req.setAttr("type", dtType)->setText(req.attr("Value").empty()?ndX->second->attr("Value"):req.attr("Value"));
			    return 0;
			case AId_DataType: req.setAttr("type", int2str(OpcUa_NodeId))->setText(int2str(atoi(dtType.c_str())&(~0x80)));	return 0;
			case AId_ValueRank:
			{
			    string val = ndX->second->attr("ValueRank");
			    req.setAttr("type", int2str(OpcUa_Int32))->setText(val.empty()?"-1":val);
			    return 0;
			}
			case AId_ArrayDimensions:
			{
			    string val = ndX->second->attr("Value");
			    int cnt = 0;
			    if(atoi(dtType.c_str())&0x80)	//Array flag
				for(int off = 0; strParse(val,0,"\n",&off).size(); cnt++) ;
			    req.setAttr("type", int2str(0x80|OpcUa_Int32))->setText(int2str(cnt));
			    return 0;
			}
			case AId_AccessLevel:
			{
			    string val = ndX->second->attr("AccessLevel");
			    req.setAttr("type", int2str(OpcUa_Byte))->setText(val.empty()?int2str(ACS_Read):val);
			    return 0;
			}
			case AId_UserAccessLevel:
			{
			    string val = ndX->second->attr("UserAccessLevel");
			    req.setAttr("type", int2str(OpcUa_Byte))->setText(val.empty()?int2str(ACS_Read|ACS_Write):val);
			    return 0;
			}
			case AId_MinimumSamplingInterval:
			{
			    string val = ndX->second->attr("MinimumSamplingInterval");
			    req.setAttr("type", int2str(OpcUa_Double))->setText(val.empty()?"-1":val);
			    return 0;
			}
			case AId_Historizing:
			    req.setAttr("type", int2str(OpcUa_Boolean))->setText(ndX->second->attr("Historizing"));
			    return 0;
		    }
		    return OpcUa_BadAttributeIdInvalid;
		}
	    }
	    return OpcUa_BadAttributeIdInvalid;
	}
	case OpcUa_WriteRequest:
	{
	    NodeId nid = NodeId::fromAddr(req.attr("node"));
	    map<string,XML_N*>::iterator ndX = ndMap.find(nid.toAddr());
	    if(ndX == ndMap.end()) return OpcUa_BadNodeIdUnknown;
	    return OpcUa_BadNothingToDo;
	}
    }

    return OpcUa_BadNodeIdUnknown;
}

//*************************************************
//* XML_N				       *
//*************************************************
XML_N::XML_N( const XML_N &nd ) : mName("nd"), mText(""), mParent(NULL)
{
    this->operator=(nd);
}

XML_N &XML_N::operator=( const XML_N &prm )
{
    //Delete self children and attributes
    mAttr.clear();
    for(unsigned i_ch = 0; i_ch < mChildren.size(); i_ch++)
	delete mChildren[i_ch];
    mChildren.clear();

    //Copy params (name,text, attributes and instructions)
    mName = prm.mName;
    mText = prm.mText;
    vector<string> ls;
    prm.attrList(ls);
    for(unsigned i_a = 0; i_a < ls.size(); i_a++)
	setAttr(ls[i_a],prm.attr(ls[i_a]));

    //Recursive copy children
    for(unsigned i_ch = 0; i_ch < prm.childSize(); i_ch++)
	*childAdd() = *prm.childGet(i_ch);

    return *this;
}

void XML_N::childAdd( XML_N *n )
{
    if(!n) return;
    mChildren.push_back(n);
    n->mParent = this;
}

XML_N* XML_N::childAdd( const string &name )
{
    XML_N *n = new XML_N(name);
    childAdd(n);

    return n;
}

void XML_N::childDel( const unsigned id )
{
    if(id >= childSize()) throw OPCError("Child %d is not present.", id);
    delete mChildren[id];
    mChildren.erase(mChildren.begin()+id);
}

void XML_N::childDel( XML_N *nd )
{
    for(unsigned i_ch = 0; i_ch < mChildren.size(); i_ch++)
	if(mChildren[i_ch] == nd)
	{
	    delete mChildren[i_ch];
	    mChildren.erase(mChildren.begin()+i_ch);
	    break;
	}
}

void XML_N::childClear( const string &name )
{
    for(unsigned i_ch = 0; i_ch < mChildren.size(); )
	if(name.empty() || mChildren[i_ch]->name() == name) childDel(i_ch);
	else i_ch++;
}

int XML_N::childIns( unsigned id, XML_N * n )
{
    if(!n) return -1;

    if(id > childSize()) id = childSize();
    mChildren.insert(mChildren.begin()+id, n);
    n->mParent = this;

    return id;
}

XML_N* XML_N::childIns( unsigned id, const string &name )
{
    XML_N *n = new XML_N( name );
    childIns(id,n);

    return n;
}

XML_N* XML_N::childGet( const int index, bool noex ) const
{
    if(index >= 0 && index < (int)childSize()) return mChildren[index];
    if(noex) return NULL;
    throw OPCError("Child %d is not present.", index);
}

XML_N* XML_N::childGet( const string &name, const int numb, bool noex ) const
{
    for(int i_ch = 0, i_n = 0; i_ch < (int)childSize(); i_ch++)
	if(strcasecmp(childGet(i_ch)->name().c_str(),name.c_str()) == 0 && i_n++ == numb)
	    return childGet(i_ch);

    if(noex) return NULL;
    throw OPCError("Child %s:%d is not found!", name.c_str(), numb);
}

XML_N* XML_N::childGet( const string &attr, const string &val, bool noex ) const
{
    for(unsigned i_f = 0; i_f < childSize(); i_f++)
	if(childGet(i_f)->attr(attr) == val) return childGet(i_f);

    if(noex) return NULL;
    throw OPCError("Child with attribut %s=%s is not present.", attr.c_str(), val.c_str());
}

XML_N* XML_N::getElementBy( const string &iattr, const string &val )
{
    if(attr(iattr) == val)	return this;

    XML_N* rez = NULL;
    for(unsigned i_ch = 0; !rez && i_ch < childSize(); i_ch++)
	rez = childGet(i_ch)->getElementBy(iattr,val);

    return rez;
}

XML_N* XML_N::root( )
{
    XML_N *cur = this;
    while(cur->parent()) cur = cur->parent();
    return cur;
}

string	XML_N::text( bool childs, bool recursive ) const
{
    if(!childs || mName == "<*>") return mText;

    string rez;
    for(unsigned i_f = 0; i_f < childSize(); i_f++)
	if(childGet(i_f)->name() == "<*>") rez += childGet(i_f)->text();
	else if(recursive) rez += childGet(i_f)->text(childs, recursive);

    return rez;
}

XML_N* XML_N::setText( const string &s, bool childs )
{
    if(!childs || mName == "<*>") { mText = s; return this; }

    int i_ch = -1;
    for(int i_f = 0; i_f < (int)childSize(); i_f++)
	if(childGet(i_f)->name() == "<*>")
	{
	    if(i_ch < 0) childGet(i_f)->mText = s;
	    else childDel(i_f--);
	    i_ch = i_f;
	}
    if(i_ch < 0) childAdd("<*>")->mText = s;

    return this;
}

void XML_N::attrList( vector<string> & list ) const
{
    list.clear();
    for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	list.push_back(mAttr[i_a].first);
}

XML_N* XML_N::attrDel( const string &name )
{
    for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	if(mAttr[i_a].first == name)
	{ mAttr.erase(mAttr.begin()+i_a); break; }

    return this;
}

void XML_N::attrClear( )	{ mAttr.clear(); }

string XML_N::attr( const string &name, bool caseSens ) const
{
    if(caseSens)
    {
	for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	    if(mAttr[i_a].first == name) return mAttr[i_a].second;
    }
    else for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	if(strcasecmp(mAttr[i_a].first.c_str(),name.c_str()) == 0) return mAttr[i_a].second;

    return "";
}

XML_N* XML_N::setAttr( const string &name, const string &val )
{
    for(unsigned i_a = 0; i_a < mAttr.size(); i_a++)
	if(mAttr[i_a].first == name)
	{
	    mAttr[i_a].second = val;
	    return this;
	}

    mAttr.push_back(pair<string,string>(name,val));

    return this;
}

XML_N* XML_N::clear()
{
    attrClear();
    mText.clear();
    childClear();

    return this;
}

//*************************************************
//* OPCError					*
//*************************************************
OPCError::OPCError( const char *fmt, ... )
{
    char str[1024];		  //!!!!
    va_list argptr;

    va_start(argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);

    cod  = 0;
    mess = str;
}

OPCError::OPCError( int icod, const char *fmt, ... )
{
    char str[1024];		  //!!!!
    va_list argptr;

    va_start(argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);

    cod  = icod;
    mess = str;
}

}
