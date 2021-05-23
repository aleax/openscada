
//OpenSCADA OPC_UA implementation library file: libOPC_UA.cpp
/******************************************************************************
 *   Copyright (C) 2009-2021 by Roman Savochenko, <roman@oscada.org>	      *
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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <endian.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

#include "ieee754.h"
#include "libOPC_UA.h"

namespace OPC
{
//*************************************************
//* Static methods, mostly from OpenSCADA::TSYS   *
//*************************************************
int64_t curTime( clockid_t clc )
{
    struct timespec tm;
    clock_gettime(clc, &tm);
    return 1000000ll*tm.tv_sec + tm.tv_nsec/1000;

    /*timeval cur_tm;
    gettimeofday(&cur_tm, NULL);
    return (int64_t)cur_tm.tv_sec*1000000 + cur_tm.tv_usec;*/
}

string int2str( int val )
{
    char buf[250];
    snprintf(buf, sizeof(buf), "%d", val);
    return buf;
}

int str2int( const string &val )	{ return atoi(val.c_str()); }

string uint2str( unsigned val )
{
    char buf[250];
    snprintf(buf, sizeof(buf), "%u", val);
    return buf;
}

unsigned long str2uint( const string &val )	{ return strtoul(val.c_str(), NULL, 0); }

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
    switch(tp) {
	case 'g': snprintf(buf, sizeof(buf), "%.*g", prec, val);	break;
	case 'e': snprintf(buf, sizeof(buf), "%.*e", prec, val);	break;
	default:  snprintf(buf, sizeof(buf), "%.*f", prec, val);	break;
    }
    return buf;
}

double str2real( const string &val )	{ return atof(val.c_str()); }
/*{
    const char *chChr = val.c_str();

    //Pass spaces before
    for( ; true; ++chChr) {
	switch(*chChr) {
	    case ' ': case '\t': continue;
	}
	break;
    }

    //Check and process the base
    bool isNeg = false, isExpNeg = false;
    double tVl = 0;
    int16_t nAftRdx = 0, tAftRdx = 0;
    if(*chChr && ((*chChr >= '0' && *chChr <= '9') || *chChr == '-' || *chChr == '+')) {
	if(*chChr == '+')       ++chChr;
	else if(*chChr == '-')  { isNeg = true; ++chChr; }
	for(bool notFirst = false; *chChr >= '0' && *chChr <= '9'; ++chChr, notFirst = true) {
	    if(notFirst) tVl *= 10;
	    tVl += *chChr - '0';
	}
    }
    if(*chChr == '.' || *chChr == ',') {
	for(++chChr; *chChr >= '0' && *chChr <= '9'; ++chChr, ++nAftRdx)
	    tVl = tVl*10 + (*chChr - '0');
    }
    if(isNeg) tVl *= -1;

    //Read exponent
    if(*chChr && (*chChr == 'e' || *chChr == 'E')) {
	++chChr;
	if(*chChr == '+')       ++chChr;
	else if(*chChr == '-')  { isExpNeg = true; ++chChr; }
	for(bool notFirst = false; *chChr >= '0' && *chChr <= '9'; ++chChr, notFirst = true) {
	    if(notFirst) tAftRdx *= 10;
	    tAftRdx += *chChr - '0';
	}
	if(isExpNeg) tAftRdx *= -1;
    }

    //Combine
    return tVl * pow(10, tAftRdx-nAftRdx);
}*/

string strParse( const string &path, int level, const string &sep, int *off, bool mergeSepSymb )
{
    int an_dir = off ? *off : 0;
    int t_lev = 0;
    size_t t_dir;

    if(an_dir >= (int)path.size() || sep.empty()) return "";
    while(true) {
	t_dir = path.find(sep, an_dir);
	if(t_dir == string::npos) {
	    if(off) *off = path.size();
	    return (t_lev == level) ? path.substr(an_dir) : "";
	}
	else if(t_lev == level) {
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
    while(true) {
	for(t_dir = an_dir; t_dir < str.size(); t_dir++)
	    if(str[t_dir] == '\x0D' || str[t_dir] == '\x0A')
	    { edLnSmbSz = (str[t_dir] == '\x0D' && ((t_dir+1) < str.size()) && str[t_dir+1] == '\x0A') ? 2 : 1; break; }
	if(t_dir >= str.size()) {
	    if(off) *off = str.size();
	    return (t_lev==level) ? str.substr(an_dir) : "";
	}
	else if(t_lev == level) {
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
    union ieee754_le {
	float f;
	struct {
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
    union ieee754_le {
	float f;
	struct {
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
    union ieee754_le {
	double d;
	struct {
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
    union ieee754_le {
	double d;
	struct {
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
    //if(type() != NodeId::Numeric) mTp = NodeId::Numeric;
}

bool NodeId::operator==( const NodeId &node )
{
    if(type() != node.type())	return false;
    switch(type()) {
	case NodeId::Numeric:	return (numbVal() == node.numbVal());
	case NodeId::String:	return (strVal() == node.strVal());
    }
    return true;
}

NodeId &NodeId::operator=( const NodeId &node )
{
    setNs(node.ns());
    switch(node.type()) {
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
    return strtoul(str.c_str(), NULL, 0);
}

string NodeId::strVal( ) const
{
    if(type() == NodeId::Numeric) return uint2str(numb);

    return str;
}

void NodeId::setNumbVal( uint32_t in )
{
    if(type() != NodeId::Numeric) mTp = NodeId::Numeric;
    numb = in;
}

void NodeId::setStrVal( const string &istr, NodeId::Type tp )
{
    if(tp == NodeId::Numeric) return;
    mTp = tp;
    str = istr;
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
	for(int iS = (dt.size()-2); iS >= 0; iS -= 2)
	{ bf[0] = dt[iS]; bf[1] = dt[iS+1]; rez += (char)strtol(bf,NULL,16); }
	// Get Data2
	dt = vl.substr(10,4);
	for(int iS = (dt.size()-2); iS >= 0; iS -= 2)
	{ bf[0] = dt[iS]; bf[1] = dt[iS+1]; rez += (char)strtol(bf,NULL,16); }
	// Get Data3
	dt = vl.substr(15,4);
	for(int iS = (dt.size()-2); iS >= 0; iS -= 2)
	{ bf[0] = dt[iS]; bf[1] = dt[iS+1]; rez += (char)strtol(bf,NULL,16); }
	// Get Data4a
	dt = vl.substr(20,4);
	for(int iS = 0; iS < (int)dt.size(); iS += 2)
	{ bf[0] = dt[iS]; bf[1] = dt[iS+1]; rez += (char)strtol(bf,NULL,16); }
	// Get Data4b
	dt = vl.substr(25,12);
	for(int iS = 0; iS < (int)dt.size(); iS += 2)
	{ bf[0] = dt[iS]; bf[1] = dt[iS+1]; rez += (char)strtol(bf,NULL,16); }
	return NodeId(rez, ns, NodeId::Guid);
    }

    //Check for string or opaque
    if(vl.size() >= 2 && vl[0] == '\"' && vl[vl.size()-1] == '\"') {
	bf[2] = 0;
	char *endptr = 0;
	rez = "";
	for(unsigned iS = 1; !(vl.size()%2) && (!endptr || *endptr == 0) && iS < (vl.size()-1) && isxdigit(vl[iS]) && isxdigit(vl[iS+1]); iS += 2)
	{ bf[0] = vl[iS]; bf[1] = vl[iS+1]; rez += (char)strtol(bf,&endptr,16); }
	if(rez.size() == (vl.size()-2)/2)	return NodeId(rez, ns, NodeId::Opaque);
	return NodeId(vl.substr(1,vl.size()-2), ns);
    }

    //Check for number
    bool isStr = false;
    for(unsigned iS = 0; iS < vl.size() && !isStr; iS++)
	if(!isdigit(vl[iS])) isStr = true;
    if(isStr) return NodeId(vl,ns);
    return NodeId((uint32_t)strtoul(vl.c_str(),NULL,0), ns);
}

string NodeId::toAddr( ) const
{
    string vl;
    if(ns()) vl = uint2str(ns()) + ":";
    switch(type()) {
	case NodeId::Numeric:	vl += uint2str(numbVal());	break;
	case NodeId::String:	vl += "\""+strVal()+"\"";	break;
	case NodeId::Guid: {
	    vl += "{";
	    //Get Data1
	    string svl = strVal().substr(0,4);
	    for(int iSz = (svl.size()-1); iSz >= 0; iSz--)
		vl += strMess("%0.2x",(unsigned char)svl[iSz]);
	    //Get Data2
	    vl += "-";
	    svl = strVal().substr(4,2);
	    for(int iSz = (svl.size()-1); iSz >= 0; iSz--)
		vl += strMess("%0.2x",(unsigned char)svl[iSz]);
	    //Get Data3
	    vl += "-";
	    svl = strVal().substr(6,2);
	    for(int iSz = (svl.size()-1); iSz >= 0; iSz--)
		vl += strMess("%0.2x",(unsigned char)svl[iSz]);
	    //Get Data4a
	    vl += "-";
	    svl = strVal().substr(8,2);
	    for(int iSz = 0; iSz < (int)svl.size(); iSz++)
		vl += strMess("%0.2x",(unsigned char)svl[iSz]);
	    //Get Data4b
	    vl += "-";
	    svl = strVal().substr(10,6);
	    for(int iSz = 0; iSz < (int)svl.size(); iSz++)
		vl += strMess("%0.2x",(unsigned char)svl[iSz]);
	    vl += "}";
	    break;
	}
	case NodeId::Opaque: {
	    vl += "\"";
	    string svl = strVal();
	    for(unsigned iSz = 0; iSz < svl.size(); iSz++)
		vl += strMess("%0.2x",(unsigned char)svl[iSz]);
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

int64_t UA::iN( const string &rb, int &off, char vSz )
{
    off += vSz;
    if(off > (int)rb.size())
	throw OPCError(OpcUa_BadDecodingError, "Buffer size is lesser requested value.");
    switch(vSz) {
	case 1: return *(int8_t*)(rb.data()+off-vSz);
	case 2: return (int16_t)getUnalign16(rb.data()+off-vSz);
	case 4: return (int32_t)getUnalign32(rb.data()+off-vSz);
	case 8: return (int64_t)getUnalign64(rb.data()+off-vSz);
    }
    throw OPCError(OpcUa_BadDecodingError, "Number size '%d' error.", vSz);
}

uint64_t UA::iNu( const string &rb, int &off, char vSz )
{
    off += vSz;
    if(off > (int)rb.size())
	throw OPCError(OpcUa_BadDecodingError, "Buffer size is lesser requested value.");
    switch(vSz) {
	case 1: return *(uint8_t*)(rb.data()+off-vSz);
	case 2: return getUnalign16(rb.data()+off-vSz);
	case 4: return getUnalign32(rb.data()+off-vSz);
	case 8: return getUnalign64(rb.data()+off-vSz);
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
    if(encMsk & 0x01) {
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
    switch(enc) {
	case 0x00:	//Two Byte
	    return NodeId(iNu(rb,off,1));
	case 0x01: {	//Four Byte
	    uint8_t ns = iNu(rb,off,1);
	    return NodeId(iNu(rb,off,2),ns);
	}
	case 0x02: {	//Numeric
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(iNu(rb,off,4),ns);
	}
	case 0x03: {	//String
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(iS(rb,off),ns);
	}
	case 0x04: {	//Guid
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(string(iVal(rb,off,16),16),ns,NodeId::Guid);
	}
	case 0x05: {	//Opaque
	    uint16_t ns = iNu(rb,off,2);
	    return NodeId(iS(rb,off),ns,NodeId::Opaque);
	}
    }

    throw OPCError(OpcUa_BadDecodingError, "NodeId type %d error or isn't supported.", enc);
}

string UA::iVariant( const string &buf, int &off, uint8_t *tp )
{
    uint8_t emv = iNu(buf, off, 1);			//type
    if(tp) *tp = emv;
    int32_t arrL = 1;
    if(emv&OpcUa_Array) arrL = iNu(buf, off, 4);	//ArrayLength
    string rezVl;
    for(int iV = 0; iV < arrL; iV++) {
	if(arrL > 1 && iV) rezVl += "\n";
	switch(emv&OpcUa_VarMask) {
	    case OpcUa_Boolean:
	    case OpcUa_SByte:	rezVl += int2str(iN(buf,off,1));	break;
	    case OpcUa_Byte:	rezVl += int2str(iNu(buf,off,1));	break;
	    case OpcUa_Int16:	rezVl += int2str(iN(buf,off,2));	break;
	    case OpcUa_UInt16:	rezVl += uint2str(iNu(buf,off,2));	break;
	    case OpcUa_Int32:	rezVl += int2str(iN(buf,off,4));	break;
	    case OpcUa_UInt32:	rezVl += uint2str(iNu(buf,off,4));	break;
	    case OpcUa_Int64:	rezVl += ll2str(iN(buf,off,8));		break;
	    case OpcUa_UInt64:	rezVl += strMess("%llu", iNu(buf,off,8));	break;
	    case OpcUa_Float:	rezVl += real2str(iR(buf,off,4));	break;
	    case OpcUa_Double:	rezVl += real2str(iR(buf,off,8));	break;
	    case OpcUa_String:
	    case OpcUa_ByteString:	rezVl += iS(buf,off);	break;
	    case OpcUa_NodeId:	rezVl += iNodeId(buf,off).toAddr();	break;
	    case OpcUa_StatusCode:	rezVl += strMess("0x%x",iNu(buf,off,4));	break;
	    case OpcUa_QualifiedName: {
		uint16_t ns;
		string vl = iSqlf(buf,off,&ns);
		rezVl += uint2str(ns)+":"+vl;
		break;
	    }
	    case OpcUa_LocalizedText: {
		string loc, vl;
		vl = iSl(buf, off, &loc);
		rezVl += loc+":"+vl;
		break;
	    }
	    default: throw OPCError(OpcUa_BadDecodingError, "Variant type '%d' is not supported.", emv&OpcUa_VarMask);
	}
    }
    // ArrayDimension
    if(emv&OpcUa_ArrayDimension) throw OPCError(OpcUa_BadDecodingError, "ArrayDimensions field of Variant is not supported.");

    return rezVl;
}

void UA::iDataValue( const string &buf, int &off, XML_N &nd )
{
    nd.setAttr("Status", "");
    //Data Value
    uint8_t em = iNu(buf, off, 1);			//Encoding Mask
    if(em&0x01) {					//Value
	uint8_t emv;
	nd.setText(iVariant(buf,off,&emv));
	nd.setAttr("VarTp", uint2str(emv))->setAttr("nodata", "");
    } else nd.setAttr("nodata", "1");
    if(em&0x02)	nd.setAttr("Status", strMess("0x%x",iNu(buf,off,4)));
    if(em&0x04)	nd.setAttr("SourceTimestamp", ll2str(iTm(buf,off)));
    if(em&0x10)	nd.setAttr("SourcePicoseconds", uint2str(iNu(buf,off,2)));
    if(em&0x08)	nd.setAttr("ServerTimestamp", ll2str(iTm(buf,off)));
    if(em&0x20)	nd.setAttr("ServerPicoseconds", uint2str(iNu(buf,off,2)));
}

void UA::oN( string &buf, int64_t val, char sz, int off )
{
    if(off < 0 || (off+sz) > (int)buf.size()) buf.append((char*)&val, sz);
    else buf.replace(off, sz, (char*)&val, sz);
}

void UA::oNu( string &buf, uint64_t val, char sz, int off )
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
    switch(val.type()) {
	case NodeId::Numeric:
	    if(val.ns() == 0 && val.numbVal() <= 255) {
		buf += (char)0x00;
		buf += (char)val.numbVal();
	    }
	    else if(val.ns() <= 255 && val.numbVal() <= 65535) {
		buf += (char)0x01;
		buf += (char)val.ns();
		oNu(buf, val.numbVal(), 2);
	    }
	    else {
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
    if(resMask&RdRm_RefType) oNodeId(buf, refTypeId);		else oNodeId(buf, 0);
    if(resMask&RdRm_IsForward) oNu(buf, isForward, 1);		else oNu(buf, 0, 1);
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
    if(eMsk&0x01) {		//Variant
	//Some type correction
	switch(vEMsk&OpcUa_VarMask) {
	    case OpcUa_IntAuto: {
		int64_t tVl = llabs(strtoll(vl.c_str(),NULL,10));
		uint8_t vTp = OpcUa_SByte;
		if(tVl >= (1ll<<31))		vTp = OpcUa_Int64;
		else if(tVl >= (1ll<<15))	vTp = OpcUa_Int32;
		else if(tVl >= (1ll<<7))	vTp = OpcUa_Int16;
		vEMsk = (vEMsk&(~OpcUa_VarMask)) | vTp;
		break;
	    }
	    case OpcUa_UIntAuto: {
		uint64_t tVl = strtoull(vl.c_str(), NULL, 10);
		uint8_t vTp = OpcUa_Byte;
		if(tVl >= (1ll<<32))		vTp = OpcUa_UInt64;
		else if(tVl >= (1ll<<16))	vTp = OpcUa_UInt32;
		else if(tVl >= (1ll<<8))	vTp = OpcUa_UInt16;
		vEMsk = (vEMsk&(~OpcUa_VarMask)) | vTp;
		break;
	    }
	}

	//Generic processing
	oNu(buf, vEMsk, 1);	//Encoding Mask
	int32_t arrL = 1;
	if(vEMsk&OpcUa_Array) {	//Array process
	    arrL = 0;
	    for(int off = 0; strParse(vl,0,"\n",&off).size(); ) arrL++;
	    oNu(buf, arrL, 4);	//ArrayLength
	}
	for(int iV = 0, off = 0; iV < arrL; iV++) {
	    string setVl = (arrL==1) ? vl : strParse(vl,0,"\n",&off);
	    switch(vEMsk&OpcUa_VarMask) {
		case OpcUa_Boolean:
		case OpcUa_SByte:	oN(buf, atoi(setVl.c_str()), 1);	break;
		case OpcUa_Byte:	oNu(buf, atoi(setVl.c_str()), 1);	break;
		case OpcUa_Int16:	oN(buf, atoi(setVl.c_str()), 2);	break;
		case OpcUa_UInt16:	oNu(buf, atoi(setVl.c_str()), 2);	break;
		case OpcUa_Int32:	oN(buf, atoi(setVl.c_str()), 4);	break;
		case OpcUa_UInt32:	oNu(buf, strtoul(setVl.c_str(),NULL,10), 4);	break;
		case OpcUa_Int64:	oN(buf, strtoll(setVl.c_str(),NULL,10), 8);	break;
		case OpcUa_UInt64:	oNu(buf, strtoull(setVl.c_str(),NULL,10), 8);	break;
		case OpcUa_Float:	oR(buf, str2real(setVl), 4);		break;
		case OpcUa_Double:	oR(buf, str2real(setVl), 8);		break;
		case OpcUa_String:
		case OpcUa_ByteString:	oS(buf, setVl);				break;
		case OpcUa_NodeId:	oNodeId(buf, NodeId::fromAddr(setVl));	break;
		case OpcUa_StatusCode:	oNu(buf, strtoll(setVl.c_str(),NULL,0), 4);	break;
		case OpcUa_QualifiedName: oSqlf(buf, setVl);			break;
		case OpcUa_LocalizedText: oSl(buf, setVl, lang2CodeSYS());	break;
		default: //oS(buf, setVl);	break;
		    throw OPCError(OpcUa_BadDecodingError, "Data type '%d' do not support.", vEMsk&OpcUa_VarMask);
	    }
	}
	//ArrayDimension
	if(vEMsk&OpcUa_ArrayDimension) throw OPCError(OpcUa_BadDecodingError, "ArrayDimensions field isn't supporteded.");
	//!!!!
    }
    if(eMsk&0x02) oN(buf, strtoul(vl.c_str(),NULL,10), 4);	//Status
    if(eMsk&0x04) oTm(buf, srcTmStmp ? srcTmStmp : curTime());	//SourceTimestamp
    if(eMsk&0x08) oTm(buf, curTime());				//ServerTimestamp
}

string UA::randBytes( int num )
{
    unsigned char buf[num];
    if(RAND_bytes(buf,num) != 1) {
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
    if(len > 0) {
	char *buf = (char*)malloc(len);
	unsigned char *p = (unsigned char *)buf;
	if(buf) {
	    i2d_X509(x, &p);
	    rez.assign(buf, len);
	    free(buf);
	}
    }
    //Free temporary data
    if(bm) BIO_free(bm);
    if(x)  X509_free(x);

    if(len <= 0) {
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
    if(bm && PEM_write_bio_X509(bm,x) > 0) {
	char buf[4000];
	for(int iR = 0; (iR=BIO_read(bm,buf,sizeof(buf))) > 0; ) rez.append(buf, iR);
    }
    //Free temporary data
    if(bm) BIO_free(bm);
    if(x)  X509_free(x);

    if(rez.empty()) {
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

    if(n <= 0) {
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
    if(bm && BIO_write(bm,certPem.data(),certPem.size()) == (int)certPem.size())
	x = PEM_read_bio_X509_AUX(bm, NULL, NULL, NULL);
    if(x)	pkey = X509_get_pubkey(x);
    if(pkey)	rsa = EVP_PKEY_get1_RSA(pkey);
    if(rsa)	keysize = RSA_size(rsa);
    if(keysize && !(mess.size()%(keysize-paddSize))) {
	unsigned char rsaOut[keysize];
	for(unsigned i_b = 0; i_b < mess.size()/(keysize-paddSize); i_b++) {
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

    if(rez.empty()) {
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
    if(bm && BIO_write(bm,keyPem.data(),keyPem.size()) == (int)keyPem.size())
	pkey = PEM_read_bio_PrivateKey(bm, NULL, 0, (char*)"keypass");
    if(pkey)	rsa = EVP_PKEY_get1_RSA(pkey);
    if(rsa)	keysize = RSA_size(rsa);
    if(keysize && !(mess.size()%keysize)) {
	unsigned char rsaOut[keysize];
	for(unsigned i_b = 0; i_b < mess.size()/keysize; i_b++) {
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

    if(rez.empty()) {
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
    if(bm && BIO_write(bm,certPem.data(),certPem.size()) == (int)certPem.size())
	x = PEM_read_bio_X509_AUX(bm, NULL, NULL, NULL);
    if(x) pkey = X509_get_pubkey(x);
    if(pkey && (int)sign.size() == EVP_PKEY_size(pkey)) mdtmp = BIO_new(BIO_f_md());
    if(mdtmp) {
	BIO_set_md(mdtmp, EVP_sha1());
	mdtmp = BIO_push(mdtmp, bm);
	if(BIO_write(mdtmp,mess.data(),mess.size()) == (int)mess.size()) {
	    BIO_get_md_ctx(mdtmp, &ctx);
	    if(ctx) rez = EVP_VerifyFinal(ctx, (unsigned char*)sign.data(), sign.size(), pkey);
	}
    }

    //Free temporary data
    if(x)	X509_free(x);
    if(mdtmp)	BIO_free(mdtmp);
    if(bm)	BIO_free(bm);
    if(pkey)	EVP_PKEY_free(pkey);

    if(rez == -1) {
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
    if(bm && BIO_write(bm,pvPem.data(),pvPem.size()) == (int)pvPem.size())
	pkey = PEM_read_bio_PrivateKey(bm, NULL, 0, (char*)"keypass");
    if(pkey) mdtmp = BIO_new(BIO_f_md());
    if(mdtmp) {
	BIO_set_md(mdtmp, EVP_sha1());
	mdtmp = BIO_push(mdtmp, bm);
	if(BIO_write(mdtmp,mess.data(),mess.size()) == (int)mess.size()) {
	    BIO_get_md_ctx(mdtmp, &ctx);
	    if(ctx) EVP_SignFinal(ctx, buf, &blen, pkey);
	}
    }

    //Free temporary data
    if(mdtmp)	BIO_free(mdtmp);
    if(bm)	BIO_free(bm);
    if(pkey)	EVP_PKEY_free(pkey);

    if(!blen) {
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
    if(bm && BIO_write(bm,keyCertPem.data(),keyCertPem.size()) == (int)keyCertPem.size())
	x = PEM_read_bio_X509_AUX(bm,NULL,NULL,NULL);
    if(x)	pkey = X509_get_pubkey(x);
    if(pkey)	rez = EVP_PKEY_size(pkey);

    //Free temporary data
    if(x)	X509_free(x);
    if(bm)	BIO_free(bm);
    if(pkey)	EVP_PKEY_free(pkey);

    if(rez == -1) {
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
    for(int iC = 0; iC < hashCnt; iC++) {
	HMAC(EVP_sha1(), secret.data(), secret.size(), hashTmp, 20+seed.size(), hashRez+20*iC, 0);
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
Client::Client( )
{
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mtxData, &attrM);
    pthread_mutexattr_destroy(&attrM);
}

Client::~Client( )
{
    pthread_mutex_lock(&mtxData);
    pthread_mutex_destroy(&mtxData);
}

string Client::poll( bool byRead )
{
    XML_N req("opc.tcp");

    if(byRead) {
	req.setAttr("id", "Read")->setAttr("timestampsToReturn", int2str(TS_NEITHER));

	//Checking for the subscriptions
	for(unsigned iSubscr = 0; iSubscr < sess.mSubScr.size(); ++iSubscr) {
	    Subscr &curS = sess.mSubScr[iSubscr];

	    req.childClear();
	    OPCAlloc res(mtxData, true);
	    for(unsigned mIt = 0; mIt < sess.mSubScr[iSubscr].mItems.size(); ++mIt) {
		Subscr::MonitItem &curIt = sess.mSubScr[iSubscr].mItems[mIt];
		if(curIt.nd.isNull())	continue;
		req.childAdd("node")->
			setAttr("mItId", uint2str(mIt))->
			setAttr("nodeId", curIt.nd.toAddr())->
			setAttr("attributeId", uint2str(curIt.aid));
	    }
	    res.unlock();

	    reqService(req);

	    res.lock();
	    for(unsigned iCh = 0; iCh < req.childSize(); ++iCh) {
		XML_N *chO = req.childGet(iCh);
		if(str2uint(chO->attr("mItId")) >= sess.mSubScr[iSubscr].mItems.size())	continue;
		Subscr::MonitItem &mO = sess.mSubScr[iSubscr].mItems[str2uint(chO->attr("mItId"))];
		mO.val = *chO;
	    }
	    res.unlock();
	}
    }
    else {
	//Checking for the input channel activity
	req.setAttr("id", "Poll");
	reqService(req);

	//Checking for the subscriptions
	for(unsigned iSubscr = 0; iSubscr < sess.mSubScr.size() && req.attr("err").empty(); ++iSubscr) {
	    Subscr &curS = sess.mSubScr[iSubscr];
	    if(!curS.subScrId) continue;

	    // Checking for loss all publish requests at missing the publish responses long time
	    if(1e-6*(curTime()-curS.lstPublTm) > OpcUa_ClntPublishResentCntr*1e-3*curS.maxKeepAliveCnt*curS.publInterval) {
		sess.mPublSeqs.clear();
		curS.lstPublTm = curTime();
	    }

	    // Sending the publish requests
	    while(sess.mPublSeqs.size() < publishReqsPool() && req.attr("err").empty()) {
		req.childClear()->setAttr("id", "Publish");
		for(unsigned iAck = 0; iAck < curS.mSeqToAcq.size(); ++iAck)
		    req.childAdd("Ack")->setAttr("subScrId", uint2str(curS.subScrId))->setText(uint2str(curS.mSeqToAcq[iAck]));
		curS.mSeqToAcq.clear();	//????
		reqService(req);
	    }
	}
    }

    return req.attr("err");
}

void Client::protIO( XML_N &io )
{
    string rez, rb, err;
    char buf[1000];
    int resp_len = 0;
    //Input chunks
    int chCnt = 0;
    string chB;
    uint32_t chClSeqN = 0, chReqId = 0;

    bool debug = (bool)atoi(io.attr("debug").c_str());

    try {
	if(io.name() == "opc.tcp") {
	    if(io.attr("id") == "HEL") {
		if(io.attr("EndPoint").size())	sess.endPoint = endPoint();

		rez = "HELF";				//> HELLO message type
		rez.reserve(50);
		oNu(rez, 0, 4);				//Message size
		oNu(rez, OpcUa_ProtocolVersion, 4);	//Protocol version
		oNu(rez, rcvBufSz(), 4);		//Recive buffer size
		oNu(rez, sndBufSz(), 4);		//Send buffer size
		oNu(rez, msgMaxSz(), 4);		//Max message size
		oNu(rez, chunkMaxCnt(), 4);		//Max chunk count
		oS(rez, sess.endPoint);			//EndpointURL
		oNu(rez, rez.size(), 4, 4);		//Real message size

		if(debug) debugMess(strMess("HELLO Req(%d): ep='%s'",rez.size(),sess.endPoint.c_str()));

		//Send request
		resp_len = messIO(rez.data(), rez.size(), buf, sizeof(buf));
		rez.assign(buf, resp_len);

		if(debug) debugMess(strMess("HELLO Resp(%d)",rez.size()));

		int off = 4;
		if(rez.size() < 8 || rez.size() > 4096 || iNu(rez,off,4) != rez.size())
		    err = strMess("0x%x:%s", OpcUa_BadTcpMessageTooLarge, "Respond size is not coincidence.");
		else if(rez.compare(0,4,"ERRF") == 0) err = iErr(rez,off);
		else if(rez.compare(0,4,"ACKF") != 0)
		    err = strMess("0x%x:%s", OpcUa_BadTcpMessageTypeInvalid, "Respond don't acknowledge.");
		else {
		    iNu(rez, off, 4);				//Protocol version
		    sess.servRcvBufSz = iNu(rez, off, 4);	//Recive buffer size
		    sess.servSndBufSz = iNu(rez, off, 4);	//Send buffer size
		    sess.servMsgMaxSz = iNu(rez, off, 4);	//Max message size
		    sess.servChunkMaxCnt = iNu(rez, off, 4);	//Max chunk count
		}
	    }
	    else if(io.attr("id") == "CLO") {
		rez = "CLOF";				//OpenSecureChannel close
		rez.reserve(200);
		oNu(rez, 0, 4);				//Message size
		oNu(rez, sess.secChnl, 4);		//Secure channel identifier
		oNu(rez, sess.secToken, 4);		//TokenId
		int begEncBlck = rez.size();
							//> Sequence header
		oNu(rez, (++sess.sqNumb), 4);		//Sequence number
		oNu(rez, (++sess.sqReqId), 4);		//RequestId
							//> Extension body object
		oNodeId(rez, OpcUa_CloseSecureChannelRequest); //TypeId request: CloseSecureChannel
							//>> Request Header
		oNodeId(rez, 0);			//Session AuthenticationToken
		oTm(rez, curTime());			//timestamp
		oN(rez, 0, 4);				//requestHandle
		oNu(rez, 0, 4);				//returnDiagnostics
		oS(rez, "");				//auditEntryId
		oNu(rez, 0, 4);				//timeoutHint
							//>>> Extensible parameter
		oNodeId(rez, 0);			//TypeId (0)
		oNu(rez, 0, 1);				//Encoding
		oNu(rez,rez.size(), 4, 4);		//> Real message size
		if(sess.secMessMode == MS_Sign || sess.secMessMode == MS_SignAndEncrypt) {
		    // Padding place
		    if(sess.secMessMode == MS_SignAndEncrypt) {
			int kSz = sess.servKey.size()/3;
			int paddingSize = ((rez.size()-begEncBlck+1+20+kSz-1)/kSz)*kSz-(rez.size()-begEncBlck+20);
			rez += string(paddingSize, (char)(paddingSize-1));
		    }
		    // Real message size calc and place
		    oNu(rez, rez.size()+20, 4, 4);
		    // Signature
		    rez += symmetricSign(rez, sess.servKey, sess.secPolicy);
		    // Encoding
		    if(sess.secMessMode == MS_SignAndEncrypt)
			rez.replace(begEncBlck, rez.size()-begEncBlck, symmetricEncrypt(rez.substr(begEncBlck),sess.servKey,sess.secPolicy));
		}

		//Parameters clear
		//io.setAttr("SecChnId", "")->setAttr("SecTokenId", "")->setAttr("SeqNumber", "")->setAttr("SeqReqId", "");
		//sess.sqNumb = 0;
		//sess.sqReqId = 0;

		if(debug) debugMess("CLO Req");

		//Send request and don't wait response
		messIO(rez.data(), rez.size(), NULL, 0);
		connect(false);	//Close the connection

		sess.clearSecCnl(str2int(io.attr("clearEP")));
	    }
	    else {
		//nextReq:
		//Open specific
		bool isSecNone = false;
		int symKeySz = 0, asymKeyPad = 0;
		string clNonce;
		//Message specific
		int iTpId = 0;
		string mReq;
		//int32_t ReqHandle = str2int(io.attr("ReqHandle"));
		bool waitResponse = true;
		if(io.attr("id") == "OPN") {
		    sess.secPolicy = io.attr("SecPolicy");
		    if(sess.secPolicy.empty()) sess.secPolicy = secPolicy();
		    if(sess.secPolicy == "None")		isSecNone = true;
		    else if(sess.secPolicy == "Basic128Rsa15")	{ symKeySz = 16; asymKeyPad = 11; }
		    else if(sess.secPolicy == "Basic256")	{ symKeySz = 32; asymKeyPad = 42; }
		    else throw OPCError(OpcUa_BadSecurityPolicyRejected, "");

		    if(io.attr("ServCert").size())	sess.servCert = io.attr("ServCert");
		    if(io.attr("SecurityMode").size())	sess.secMessMode = atoi(io.attr("SecurityMode").c_str());

		    SC_ReqTP reqTp = (SC_ReqTP)str2int(io.attr("ReqType"));
		    if(reqTp == SC_ISSUE) {
			sess.secChnl = sess.sqReqId = sess.reqHndl = 0;
			sess.sqNumb = 50;
			sess.secChnlChanged = true;
		    }

		    rez = "OPNF";			//OpenSecureChannel message type
		    rez.reserve(200);
		    oNu(rez, 0, 4);			//Message size
		    oNu(rez, sess.secChnl, 4);		//Secure channel identifier
							//> Security Header
		    oS(rez, "http://opcfoundation.org/UA/SecurityPolicy#"+sess.secPolicy);	//Security policy URI
		    oS(rez, certPEM2DER((sess.secPolicy=="None")?"":cert()));	//ClientCertificate
		    oS(rez, certThumbprint(sess.servCert));//ServerCertificateThumbprint
		    uint32_t begEncBlck = rez.size();
							//> Sequence header
		    oNu(rez, (++sess.sqNumb), 4);	//Sequence number
		    oNu(rez, (++sess.sqReqId), 4);	//RequestId
							//> Extension body object
		    oNodeId(rez, NodeId(OpcUa_OpenSecureChannelRequest));	//TypeId
							//>> Request Header
		    oNodeId(rez, 0);			//Session AuthenticationToken
		    oTm(rez, curTime());		//timestamp
		    oN(rez, (sess.reqHndl++), 4);	//requestHandle
		    oNu(rez, 0, 4);			//returnDiagnostics
		    oS(rez, "");			//auditEntryId
		    oNu(rez, 0, 4);			//timeoutHint
							//>>> Extensible parameter
		    oNodeId(rez, 0);			//TypeId (0)
		    oNu(rez, 0, 1);			//Encoding
							//>>>> Standard request
		    oNu(rez, OpcUa_ProtocolVersion, 4);	//ClienUAocolVersion
		    oNu(rez, reqTp, 4);			//RequestType
		    oN(rez, sess.secMessMode, 4);	//SecurityMode
		    clNonce = isSecNone ? string("\000",1) : randBytes(symKeySz);
		    oS(rez, clNonce);			//ClientNonce
		    oN(rez, atoi(io.attr("SecLifeTm").c_str()), 4);	//RequestedLifetime
		    oNu(rez, rez.size(), 4, 4);		//> Real message size

		    if(!isSecNone) {
			// Padding place
			int kSz = asymmetricKeyLength(sess.servCert),
			    signSz = asymmetricKeyLength(cert()),
			    encSz = (rez.size() - begEncBlck) + 1 + signSz,
			    paddingSize = (encSz/(kSz-asymKeyPad) + (encSz%(kSz-asymKeyPad)?1:0))*(kSz-asymKeyPad) - encSz;
			rez += string(paddingSize+1, char(paddingSize));
			// Real message size calc and place
			oNu(rez, begEncBlck + kSz*((rez.size()-begEncBlck+signSz)/(kSz-asymKeyPad)), 4, 4);
			// Signature
			rez += asymmetricSign(rez, pvKey());
			// Encoding
			if(debug) debugMess("OPN Req (decoded)");
			rez.replace(begEncBlck, rez.size()-begEncBlck, asymmetricEncrypt(rez.substr(begEncBlck),sess.servCert,sess.secPolicy));
		    }
		    if(debug) debugMess("OPN Req");

		    messIO(rez.data(), rez.size(), NULL, 0);
		}
		else if(io.attr("id") == "FindServers") {
		    iTpId = OpcUa_FindServersRequest;
		    oS(mReq, sess.endPoint);			//endpointUrl
		    oN(mReq, 0, 4);				//localeIds []
		    oN(mReq, 0, 4);				//serverUris []
		}
		else if(io.attr("id") == "GetEndpoints") {
		    iTpId = OpcUa_GetEndpointsRequest;
		    oS(mReq, sess.endPoint);			//endpointUrl
		    oN(mReq, 0, 4);				//localeIds []
		    oN(mReq, 0, 4);				//profileUris []
		}
		else if(io.attr("id") == "CreateSession") {
		    iTpId = OpcUa_CreateSessionRequest;
								//> clientDescription (Application Description)
		    oS(mReq, applicationUri());			//applicationUri
		    oS(mReq, productUri());			//productUri
		    oSl(mReq, applicationName());		//applicationName
		    oNu(mReq, 1, 4);				//applicationType (CLIENT_1)
		    oS(mReq, "");				//gatewayServerUri
		    oS(mReq, "");				//discoveryProfileUri
		    oN(mReq, 0, 4);				//discoveryUrls

		    oS(mReq, "");				//serverUri
		    oS(mReq, sess.endPoint);			//endpointUrl
		    oS(mReq, sessionName());			//sessionName
		    io.setAttr("Nonce", randBytes(32));		//!!!! check for policy
		    oS(mReq, io.attr("Nonce"));			//clientNonce
		    oS(mReq, certPEM2DER(cert()));		//clientCertificate
		    oR(mReq, str2real(io.attr("sesTm")), 8);	//Requested SessionTimeout, ms
		    oNu(mReq, 0x1000000, 4);			//maxResponse MessageSize
		}
		else if(io.attr("id") == "ActivateSession") {
		    iTpId = OpcUa_ActivateSessionRequest;
									//> clientSignature
		    if(sess.secPolicy == "None") {
			oS(mReq, "");					//algorithm
			oS(mReq, "");					//signature
		    }
		    else {
			oS(mReq, "http://www.w3.org/2000/09/xmldsig#rsa-sha1");	//algorithm
			oS(mReq, asymmetricSign(certPEM2DER(sess.servCert)+sess.servNonce,pvKey()));	//signature
		    }

		    oNu(mReq, 0, 4);					//clientSoftwareCertificates []
									//> localeIds []
		    oNu(mReq, 1, 4);					//List number 1
		    oS(mReq, "en");					//localeId
									//> userIdentityToken

		    string userIdTok;
		    XML_N *userIdToks;
		    for(int iIT = 0; (userIdToks=sess.endPointDscr.childGet("userIdentityToken",iIT,true)); iIT++)
			if(atoi(userIdToks->attr("tokenType").c_str()) == (authData().empty()?A_Anon:A_UserNm))
			    userIdTok = userIdToks->attr("policyId");
		    if(userIdTok.empty()) throw OPCError(OpcUa_BadIdentityTokenRejected, "");

		    if(authData().empty()) {
			oNodeId(mReq, OpcUa_AnonymousIdentityToken);	//TypeId
			oNu(mReq, 1, 1);				//Encode
			oNu(mReq, 4+userIdTok.size(), 4);		//Length
			oS(mReq, userIdTok);				//policyId
		    }
		    else {
			oNodeId(mReq, OpcUa_UserNameIdentityToken);	//TypeId
			oNu(mReq, 1, 1);				//Encode
			int tkOff = mReq.size(); oNu(mReq, 0, 4);	//Length
			oS(mReq, userIdTok);				//policyId
			oS(mReq, strLine(authData(),0));		//UserName
			oS(mReq, strLine(authData(),1));		//Password
			oS(mReq, "");					//EncryptionAlgorithm
			oNu(mReq, mReq.size()-tkOff-4, 4, tkOff);	//Real length
		    }
									//> userTokenSignature
		    oS(mReq, "");					//signature
		    oS(mReq, "");					//algorithm

		    sess.mPublSeqs.clear();
		}
		else if(io.attr("id") == "CloseSession") {
		    iTpId = OpcUa_CloseSessionRequest;
		    oN(mReq, str2uint(io.attr("deleteSubscriptions")), 1);//deleteSubscriptions
		}
		else if(io.attr("id") == "Read") {
		    iTpId = OpcUa_ReadRequest;
		    oR(mReq, str2real(io.attr("maxAge")), 8);		//maxAge 0 ms
		    oN(mReq, str2real(io.attr("timestampsToReturn")), 4);//timestampsTo Return (SERVER_1)
									//> nodesToRead []
		    oNu(mReq, io.childSize(), 4);			//nodes
		    //oNu(mReq, std::min(25u,io.childSize()-stIdx), 4);	//nodes
		    for(unsigned iN = 0; iN < io.childSize(); iN++) {
		    //for(unsigned iN = stIdx; iN < io.childSize() && (iN-stIdx) < 25; iN++) {
			oNodeId(mReq, NodeId::fromAddr(io.childGet(iN)->attr("nodeId")));	//nodeId
			oNu(mReq, strtoul(io.childGet(iN)->attr("attributeId").c_str(),NULL,0), 4);	//attributeId (Value)
			oS(mReq, "");					//indexRange
			oSqlf(mReq, "");				//dataEncoding
		    }
		}
		else if(io.attr("id") == "Write") {
		    iTpId = OpcUa_WriteRequest;
									//> nodesToWrite []
		    oNu(mReq,io.childSize(),4);				//nodes
		    for(unsigned iN = 0; iN < io.childSize(); iN++) {
			XML_N *nd = io.childGet(iN);
			oNodeId(mReq, NodeId::fromAddr(nd->attr("nodeId")));			//nodeId
			oNu(mReq, strtoul(nd->attr("attributeId").c_str(),NULL,0), 4);		//attributeId (Value)
			oS(mReq, "");								//indexRange
			oDataValue(mReq, 0x01 /*0x0D*/, nd->text(), atoi(nd->attr("VarTp").c_str()));	//value,
												//!!: TimeStamps disabled but some servers tell
												//    0x80730000 (OpcUa_BadWriteNotSupported)
		    }
		}
		else if(io.attr("id") == "Browse") {
		    iTpId = OpcUa_BrowseRequest;
									//> view
		    oNodeId(mReq, 0);					//viewId (0)
		    oTm(mReq, 0);					//timestamp
		    oNu(mReq, 0, 4);					//viewVersion

		    oNu(mReq, 0, 4);					//requestedMax ReferencesPerNode
									//> nodesToBrowse
		    oNu(mReq, io.childSize(), 4);			//Nodes 1
		    for(unsigned iN = 0; iN < io.childSize(); iN++) {
			oNodeId(mReq, NodeId::fromAddr(io.childGet(iN)->attr("nodeId")));	//nodeId (RootFolder)
			oNu(mReq, strtoul(io.childGet(iN)->attr("browseDirection").c_str(),NULL,0), 4);	//browseDirection (FORWARD_0)
			oNodeId(mReq, NodeId::fromAddr(io.childGet(iN)->attr("referenceTypeId")));	//referenceTypeId (HierarchicalReferences)
			oNu(mReq, 1, 1);				//includeSubtypes (true)
			oNu(mReq, strtoul(io.childGet(iN)->attr("nodeClassMask").c_str(),NULL,0), 4);	//nodeClassMask
			oNu(mReq, strtoul(io.childGet(iN)->attr("resultMask").c_str(),NULL,0), 4);	//resultMask
		    }
		}
		else if(io.attr("id") == "CreateSubscription") {
		    iTpId = OpcUa_CreateSubscriptionRequest;
		    oR(mReq, str2real(io.attr("publInterval")), 8);	//requestedPublishingInterval
		    oNu(mReq, str2uint(io.attr("lifetimeCnt")), 4);	//requestedLifetimeCount
		    oNu(mReq, str2uint(io.attr("maxKeepAliveCnt")), 4);	//requestedMaxKeepAliveCount
		    oNu(mReq, str2uint(io.attr("maxNtfPerPubl")), 4);	//maxNotificationsPerPublish
		    oNu(mReq, str2int(io.attr("publEn")), 1);		//publishingEnabled
		    oNu(mReq, str2uint(io.attr("pr")), 1);		//priority
		}
		else if(io.attr("id") == "DeleteSubscriptions") {
		    iTpId = OpcUa_DeleteSubscriptionsRequest;
		    oNu(mReq, io.childSize(), 4);			//subscriptionIds []
		    for(unsigned iS = 0; iS < io.childSize(); iS++)
			oNu(mReq, str2uint(io.childGet(iS)->text()), 4);//subscriptionId
		}
		else if(io.attr("id") == "CreateMonitoredItems") {
		    iTpId = OpcUa_CreateMonitoredItemsRequest;
		    oNu(mReq, str2uint(io.attr("subScrId")), 4);	//subscriptionId
		    oNu(mReq, str2uint(io.attr("tmstmpToRet")), 4);	//timestampsToReturn
		    oNu(mReq, io.childSize(), 4);			//itemsToCreate [] | itemsToModify []
		    for(unsigned iM = 0; iM < io.childSize(); iM++) {
			XML_N *chO = io.childGet(iM);
									// itemToMonitor

			oNodeId(mReq, NodeId::fromAddr(chO->text()));	//  nodeId
			oNu(mReq, str2uint(chO->attr("aId")), 4);	//  attributeId
			oS(mReq, "");					//  indexRange
			oSqlf(mReq, "");				//  dataEncoding
			oNu(mReq, str2uint(chO->attr("mMode")), 4);	// monitoringMode
									// requestedParameters
			oNu(mReq, str2uint(chO->attr("hndl")), 4);	//  clientHandle
			oR(mReq, str2real(chO->attr("smplInt")), 8);	//  samplingInterval, in ms
			oNodeId(mReq, 0);				//  filter
			oNu(mReq, 0, 1);				//   EncodingMask
			oNu(mReq, str2uint(chO->attr("qSz")), 4);	//  queueSize
			oNu(mReq, 1, 1);				//  discardOldest, !!!!:discarded by default
		    }
		}
		else if(io.attr("id") == "DeleteMonitoredItems") {
		    iTpId = OpcUa_DeleteMonitoredItemsRequest;
		    oNu(mReq, str2uint(io.attr("subScrId")), 4);	//subscriptionId
		    //oNu(mReq, (int)MM_DISABLED, 4);			//monitoringMode
		    oNu(mReq, io.childSize(), 4);			//itemsToModify []
		    for(unsigned iM = 0; iM < io.childSize(); ++iM)
			oNu(mReq, str2uint(io.childGet(iM)->text()), 4);// monitoredItemId
		}
		else if(io.attr("id") == "Publish") {
		    iTpId = OpcUa_PublishRequest;
		    //  The publish request acknowledges
		    oN(mReq, (io.childSize()?io.childSize():-1), 4);	//>subscription Acknowledgements []
		    for(unsigned iAck = 0; iAck < io.childSize(); iAck++) {
			XML_N *chO = io.childGet(iAck);
			oNu(mReq, str2uint(chO->attr("subScrId")), 4);	//> subscriptionId
			oNu(mReq, str2uint(chO->text()), 4);		//> sequenceNumber
		    }
		    waitResponse = false;

		    sess.mPublSeqs.push_back(sess.reqHndl);
		}
		else if(io.attr("id") == "Poll") waitResponse = false;	//Just read the channel for the server activity
		else throw OPCError(OpcUa_BadNotSupported, "Request '%s' isn't supported.", io.attr("id").c_str());

		if(mReq.size()) {
		    // Request extension object finalize for chunks
		    string reqBody; reqBody.reserve(50);
		    oNodeId(reqBody, iTpId);				//TypeId request
									//>> Request Header
		    oNodeId(reqBody, NodeId::fromAddr(sess.authTkId));	//Session AuthenticationToken
		    oTm(reqBody, (sess.secLstMessReqTm=curTime()));		//timestamp
		    oN(reqBody, (sess.reqHndl++), 4);			//requestHandle
		    oNu(reqBody, 0, 4);					//returnDiagnostics
		    oS(reqBody, "");					//auditEntryId
		    oNu(reqBody, 0/*10000*/, 4);			//timeoutHint
									//>>> Extensible parameter
		    oNodeId(reqBody, 0);				//TypeId (0)
		    oNu(reqBody, 0, 1);					//Encoding
		    reqBody.append(mReq);				//Same request

		    // Maximum chunk's body size calculation for client's recieve buffer size
		    unsigned chnkBodySz = sess.servRcvBufSz - 16;		// - {HeadSz}
		    if(sess.secMessMode == MS_Sign || sess.secMessMode == MS_SignAndEncrypt) {
			if(sess.secMessMode == MS_SignAndEncrypt) {
			    int kSz = pvKey().size()/3;
			    chnkBodySz = (chnkBodySz/kSz)*kSz - 1;	//Allign to the server key size and remove same padding size's byte
			}
			chnkBodySz -= 20;				//Remove the sign size
		    }
		    chnkBodySz -= 8;	//- {SeqSz}
		    if((sess.servMsgMaxSz && reqBody.size() > sess.servMsgMaxSz) ||
			    (sess.servChunkMaxCnt && (reqBody.size()/chnkBodySz + ((reqBody.size()%chnkBodySz)?1:0)) > sess.servChunkMaxCnt))
			throw OPCError(OpcUa_BadRequestTooLarge, "Request too large");
		    // Same chunks prepare.
		    sess.sqReqId++;
		    while(reqBody.size()) {
			bool isFinal = (reqBody.size() <= chnkBodySz);
			string req; req.reserve(200);
			req.append("MSG");				//OpenSecureChannel message type
			req.append(isFinal?"F":"C");
			oNu(req, 0, 4);					//Message size
			oNu(req, sess.secChnl, 4);			//Secure channel identifier
			oNu(req, sess.secToken, 4);			//TokenId
			int begEncBlck = req.size();
									//> Sequence header
			oNu(req, (++sess.sqNumb), 4);			//Sequence number
			oNu(req, sess.sqReqId, 4);			//RequestId
			req.append(reqBody,0,std::min(chnkBodySz,(unsigned)reqBody.size()));
			reqBody.erase(0,std::min(chnkBodySz,(unsigned)reqBody.size()));
			oNu(req, req.size(), 4, 4);			//Real message size

			//Security information
			if(sess.secMessMode == MS_Sign || sess.secMessMode == MS_SignAndEncrypt) {
			    // Padding place
			    if(sess.secMessMode == MS_SignAndEncrypt) {
				int kSz = sess.servKey.size()/3;
				int paddingSize = ((req.size()-begEncBlck+1+20+kSz-1)/kSz)*kSz - (req.size()-begEncBlck+20);
				req += string(paddingSize,(char)(paddingSize-1));
			    }

			    // Real message size calc and place
			    oNu(req, req.size()+20, 4, 4);

			    // Signature
			    req += symmetricSign(req, sess.servKey, sess.secPolicy);

			    // Encoding
			    if(sess.secMessMode == MS_SignAndEncrypt)
				req.replace(begEncBlck, req.size()-begEncBlck, symmetricEncrypt(req.substr(begEncBlck),sess.servKey,sess.secPolicy));
			}

			//Sending the request
			messIO(req.data(), req.size(), NULL, 0);
		    }

		    // Original one chunk request
		    /*rez = "MSGF";					//SecureChannel message
		    rez.reserve(200);
		    oNu(rez, 0, 4);					//Message size
		    oNu(rez, sess.secChnl, 4);				//Secure channel identifier
		    oNu(rez, sess.secToken, 4);				//TokenId
		    int begEncBlck = rez.size();
									//> Sequence header
		    oNu(rez, (++sess.sqNumb), 4);			//Sequence number
		    oNu(rez, (++sess.sqReqId), 4);			//RequestId
									//> Extension body object
		    oNodeId(rez, iTpId);				//TypeId request
									//>> Request Header
		    oNodeId(rez, NodeId::fromAddr(sess.authTkId));	//Session AuthenticationToken
		    oTm(rez, (sess.secLstMessReqTm=curTime()));		//timestamp
		    //io.setAttr("ReqHandle", uint2str(ReqHandle));
		    oN(rez, (sess.reqHndl++), 4);			//requestHandle
		    oNu(rez, 0, 4);					//returnDiagnostics
		    oS(rez, "");					//auditEntryId
		    oNu(rez, 0, 4);					//timeoutHint
									//>>> Extensible parameter
		    oNodeId(rez, 0);					//TypeId (0)
		    oNu(rez, 0, 1);					//Encoding
		    rez.append(mReq);					//Same request
		    oNu(rez, rez.size(), 4, 4);				//> Real message size

		    //Security information
		    if(sess.secMessMode == MS_Sign || sess.secMessMode == MS_SignAndEncrypt) {
			// Padding place
			if(sess.secMessMode == MS_SignAndEncrypt) {
			    int kSz = sess.servKey.size()/3;
			    int paddingSize = ((rez.size()-begEncBlck+1+20+kSz-1)/kSz)*kSz-(rez.size()-begEncBlck+20);
			    rez += string(paddingSize,(char)(paddingSize-1));
			}

			// Real message size calc and place
			oNu(rez, rez.size()+20, 4, 4);

			// Signature
			rez += symmetricSign(rez, sess.servKey, sess.secPolicy);

			// Encoding
			if(sess.secMessMode == MS_SignAndEncrypt)
			    rez.replace(begEncBlck, rez.size()-begEncBlck, symmetricEncrypt(rez.substr(begEncBlck),sess.servKey,sess.secPolicy));
		    }

		    if(debug) debugMess(io.attr("id")+" Out");

		    //Sending the request
		    messIO(rez.data(), rez.size(), NULL, 0);*/
		}

		rez = "";
		//Waiting for request response, or just read the channel if there is no request
		continueReadChnk:
		resp_len = messIO(NULL, 0, buf, sizeof(buf), -(waitResponse&&rez.empty()?10000:1));	//???? Get 10000 from the transport timeouts
		rez.append(buf, resp_len);

		//Reading the whole package
		continueRead:
		int off = 4;
		uint32_t msgLen = 0;
		for( ; (rez.size() || waitResponse) && (rez.size() < 8 || rez.size() < (msgLen=iNu(rez,off,4))); off = 4) {
		    resp_len = messIO(NULL, 0, buf, sizeof(buf));
		    if(!resp_len) throw OPCError(OpcUa_BadCommunicationError, "No or not full message.");
		    rez.append(buf, resp_len);
		}

		if(debug && rez.size()) debugMess("In "+uint2str(msgLen)+"("+uint2str(rez.size())+")");

		off = 4;
		//The response allowed to be big here due to the possibility of containg several messages
		if(!rez.size() && !waitResponse) ;
		else if(rez.size() < 8 || rez.size() < (msgLen=iNu(rez,off,4))) {
		    err = strMess("0x%x:%s", OpcUa_BadTcpMessageTooLarge, "Response size is not coincidence.");
		    rez = "";
		}
		else if(rez.compare(0,4,"ERRF") == 0) { err = iErr(rez, off); rez = ""; }
		else if(rez.compare(0,4,"OPNF") == 0) {
		    if(debug) debugMess("OPN Resp");

		    rb = rez.substr(0, msgLen); rez.erase(0, msgLen);

		    iNu(rb, off, 4);					//Secure channel identifier
		    iS(rb, off);					//Security policy URI
		    string servCert = iS(rb, off);			//ServerCertificate
		    string clntCertThmb = iS(rb, off);			//ClientCertificateThumbprint

		    if(!isSecNone) {
			if(clntCertThmb != certThumbprint(cert()))
			    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Client certificate thumbprint error.");

			// Decoding
			rb.replace(off, rb.size()-off, asymmetricDecrypt(rb.substr(off),pvKey(),sess.secPolicy));
		    }

		    iNu(rb, off, 4);			//Sequence number
		    iNu(rb, off, 4);			//RequestId
							//> Extension Object
		    if(iNodeId(rb,off).numbVal() != OpcUa_OpenSecureChannelResponse)	//TypeId
			throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");
							//>> Body
							//>>> RespondHeader
		    iTm(rb, off);			//timestamp
		    iN(rb, off, 4);			//requestHandle
		    iN(rb, off, 4);			//StatusCode
		    iN(rb, off, 1);			//serviceDiagnostics
		    iS(rb, off);			//stringTable
							//>>> Extensible parameter
		    iNodeId(rb, off);			//TypeId (0)
		    iNu(rb, off, 1);			//Encoding
							//>>>> Standard respond
		    iNu(rb, off, 4);			//ServerProtocolVersion
		    sess.secChnl = iNu(rb,off,4);	//Secure channel identifier
		    sess.secToken = iNu(rb,off,4);	//TokenId
		    iTm(rb, off);			//CreatedAt
		    sess.secLifeTime = iN(rb,off,4);	//RevisedLifeTime
		    string servNonce = iS(rb, off);	//nonce
		    // Signature
		    if(!isSecNone) {
			sess.clKey = deriveKey(clNonce, servNonce, symKeySz*3);
			sess.servKey = deriveKey(servNonce, clNonce, symKeySz*3);
			off += iNu(rb, off, 1);			//Pass padding
			if(!asymmetricVerify(rb.substr(0,off),rb.substr(off),sess.servCert))	//Check Signature
			    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Signature error");
		    }
		    sess.secChnlOpenTm = curTime();
		    waitResponse = false;
		}
		else if(rez.compare(0,3,"MSG") != 0) {
		    err = strMess("0x%x:%s", OpcUa_BadTcpMessageTypeInvalid, "Respond don't acknowledge.");
		    rez = "";
		}
		else {
		    if(debug) debugMess("MSG Resp");

		    rb = rez.substr(0, msgLen); rez.erase(0, msgLen);

		    uint32_t secId = iNu(rb, off, 4);			//Secure channel identifier
		    uint32_t tokId = iNu(rb, off, 4);			//Symmetric Algorithm Security Header : TokenId

		    // Decrypt message block and signature check
		    if(sess.secMessMode == MS_Sign || sess.secMessMode == MS_SignAndEncrypt) {
			if(sess.secMessMode == MS_SignAndEncrypt)
			    rb.replace(off, rb.size()-off, symmetricDecrypt(rb.substr(off),sess.clKey,sess.secPolicy));
			if(rb.substr(rb.size()-20) != symmetricSign(rb.substr(0,rb.size()-20),sess.clKey,sess.secPolicy))	//Check Signature
			    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Signature error");
			rb.erase(rb.size()-20);				//Remove the signature
			if(sess.secMessMode == MS_SignAndEncrypt)
			    rb.erase(rb.size()-(rb[rb.size()-1]+1));	//Remove the padding by the last byte value
		    }
									//> Sequence header
		    uint32_t clSeqN = iNu(rb, off, 4);			//Sequence number
		    uint32_t reqId = iNu(rb, off, 4);			//RequestId
		    //Chunks processing
		    bool passMessPrc = false;
		    // Abort
		    if(rb[3] == 'A') { chCnt = 0; chB = ""; passMessPrc = true; }
		    else if(chCnt || rb[3] == 'C') {
			//Checking for lost sequence
			// * Lost single 'C' before 'F': pass to processing but there is possible single 'F', it will be checked by correct request type.
			// * Lost intermediate 'F' ('C' both after and before): clean the chunks buffer and start it's filling from begin.
			// * Lost first 'C': clean the chunks buffer and start it's filling from next 'C' but there is possible this is first 'C' after
			//	single 'F' lost before, it will be checked by correct request type.
			// * Lost intermediate 'C': clean and mark the chunks buffer as some chunk into sequence lost and pass all next chunks up to 'F'.
			// * Lost intermediate 'C' just before 'F': clean the chunks buffer and 'F'.
			// * Lost 'F' after 'C' before 'F': clean the chunks buffer and pass current 'F' to process but there is possible that is single 'F'
			//	next, it will be checked by correct request type.
			//!!!! Maybe set a mark for errors generation pass after checking to correct request type or the message's sequence begin.
			if(chCnt < 0 || (chCnt && (clSeqN-chClSeqN) > 1)) {
			    chB = "";
			    if(rb[3] == 'C' && reqId == chReqId) { chCnt = -1; passMessPrc = true; }
			    else { chCnt = 0; passMessPrc = (reqId == chReqId); }
			}
			if(!passMessPrc) {
			    chB.append(rb, off, string::npos);
			    if(rb[3] == 'C') {
				// Checking for limits
				chCnt++;
				if((chunkMaxCnt() && chCnt > (int)chunkMaxCnt()) || (msgMaxSz() && chB.size() > msgMaxSz()))
				    throw OPCError(OpcUa_BadResponseTooLarge, "Response too large");
				passMessPrc = true;
			    }
			    else {
				rb.replace(off, string::npos, chB);
				chCnt = 0; chB = "";
			    }
			}
		    }
		    chClSeqN = clSeqN; chReqId = reqId;
		    if(passMessPrc) { waitResponse = true; goto continueReadChnk; }

									//> Extension Object
		    uint32_t oTpId = iNodeId(rb, off).numbVal();	//TypeId
									//>> Body
									//>>> RespondHeader
		    iTm(rb, off);					//timestamp
		    int32_t requestHandle = iN(rb, off, 4);		//requestHandle
		    uint32_t stCode = iNu(rb, off, 4);			//StatusCode
		    iN(rb, off, 1);					//serviceDiagnostics
		    iS(rb, off);					//stringTable
									//>>> Extensible parameter
		    iNodeId(rb, off);					//TypeId (0)
		    iNu(rb, off, 1);					//Encoding

		    switch(oTpId) {
			case OpcUa_FindServersResponse: {
			    if(iTpId != OpcUa_FindServersRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");
			    int AppDescrNub = iNu(rb, off, 4);				//List items
			    for(int iL = 0; iL < AppDescrNub; iL++) {
				XML_N *ad = io.childAdd("ApplicationDescription");
				ad->setAttr("applicationUri", iS(rb,off));		//applicationUri
				ad->setAttr("productUri", iS(rb,off));			//productUri
				ad->setAttr("applicationName", iSl(rb,off));		//applicationName
				ad->setAttr("applicationType", uint2str(iNu(rb,off,4)));//applicationType
				ad->setAttr("gatewayServerUri", iS(rb,off));		//gatewayServerUri
				ad->setAttr("discoveryProfileUri", iS(rb,off));	//discoveryProfileUri
											//>>>> discoveryUrls
				int discoveryUrlsN = iNu(rb, off, 4);			//List items
				for(int iL2 = 0; iL2 < discoveryUrlsN; iL2++)
				    ad->childAdd("discoveryUrl")->setText(iS(rb,off));	//discoveryUrl
			    }
			    waitResponse = false;
			    break;
			}
			case OpcUa_GetEndpointsResponse: {
			    if(iTpId != OpcUa_GetEndpointsRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");
			    int EndpointDescrNub = iNu(rb, off, 4);		//List items
			    for(int iL = 0; iL < EndpointDescrNub; iL++) {
				XML_N *xep = io.childAdd("EndpointDescription");
										//>>> EndpointDescription
				xep->setAttr("endpointUrl", iS(rb,off));	//endpointUrl
										//>>>> server (ApplicationDescription)
				xep->setAttr("applicationUri", iS(rb,off));	//applicationUri
				xep->setAttr("productUri", iS(rb,off));	//productUri
				xep->setAttr("applicationName", iSl(rb,off));	//applicationName
				xep->setAttr("applicationType", uint2str(iNu(rb,off,4)));	//applicationType
				xep->setAttr("gatewayServerUri", iS(rb,off));	//gatewayServerUri
				xep->setAttr("discoveryProfileUri", iS(rb,off));//discoveryProfileUri
										//>>>> discoveryUrls
				int discoveryUrlsN = iNu(rb, off, 4);		//List items
				for(int iL2 = 0; iL2 < discoveryUrlsN; iL2++)
				    xep->childAdd("discoveryUrl")->setText(iS(rb,off));	//discoveryUrl
				xep->childAdd("serverCertificate")->setText(certDER2PEM(iS(rb,off)));	//>>> serverCertificate
				xep->setAttr("securityMode", uint2str(iNu(rb,off,4)));	//securityMode
				xep->setAttr("securityPolicyUri", iS(rb,off));	//securityPolicyUri
										//>>>> userIdentityTokens
				int userIdentityTokensN = iNu(rb, off, 4);	//List items
				for(int iL2 = 0; iL2 < userIdentityTokensN; iL2++) {
				    XML_N *xit = xep->childAdd("userIdentityToken");
				    xit->setAttr("policyId", iS(rb,off));		//policyId
				    xit->setAttr("tokenType", uint2str(iNu(rb,off,4)));//tokenType
				    xit->setAttr("issuedTokenType", iS(rb,off));	//issuedTokenType
				    xit->setAttr("issuerEndpointUrl", iS(rb,off));	//issuerEndpointUrl
				    xit->setAttr("securityPolicyUri", iS(rb,off));	//securityPolicyUri
				}
				xep->setAttr("transportProfileUri", iS(rb,off));	//transportProfileUri
				xep->setAttr("securityLevel", uint2str(iNu(rb,off,1)));//securityLevel
			    }
			    waitResponse = false;
			    break;
			}
			case OpcUa_CreateSessionResponse: {
			    if(iTpId != OpcUa_CreateSessionRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");
			    sess.sesId = iNodeId(rb,off).toAddr();	//sessionId
			    sess.authTkId = iNodeId(rb,off).toAddr();	//authentication Token
			    sess.sesLifeTime = iR(rb, off, 8);		//revisedSession Timeout, ms
			    sess.servNonce = iS(rb,off);		//serverNonce
			    string servCert = certDER2PEM(iS(rb,off));	//serverCertificate
			    int EndpointDescrNub = iNu(rb, off, 4);	//List items
			    for(int iL = 0; iL < EndpointDescrNub; iL++) {
									//> EndpointDescription
				iS(rb, off);				//endpointUrl
									//>> server (ApplicationDescription)
				iS(rb, off);				//applicationUri
				iS(rb, off);				//productUri
				iSl(rb, off);				//applicationName
				iNu(rb, off, 4);			//applicationType
				iS(rb, off);				//gatewayServerUri
				iS(rb, off);				//discoveryProfileUri
									//>> discoveryUrls
				int discoveryUrlsN = iNu(rb, off, 4);	//List items
				for(int iL2 = 0; iL2 < discoveryUrlsN; iL2++)
				    iS(rb, off);			//discoveryUrl
				iS(rb, off);				//> serverCertificate
				iNu(rb, off, 4);				//securityMode
				iS(rb, off);				//securityPolicyUri
									//>> userIdentityTokens
				int userIdentityTokensN = iNu(rb, off, 4);//List items
				for(int iL2 = 0; iL2 < userIdentityTokensN; iL2++) {
				    iS(rb, off);			//policyId
				    iNu(rb, off, 4);			//tokenType
				    iS(rb, off);			//issuedTokenType
				    iS(rb, off);			//issuerEndpointUrl
				    iS(rb, off);			//securityPolicyUri
				}
				iS(rb, off);				//transportProfileUri
				iNu(rb, off, 1);			//securityLevel
			    }
			    iS(rb, off);				//serverSoftware Certificates []
									//> serverSignature
			    string alg = iS(rb, off);			//algorithm
			    string sign = iS(rb, off);			//signature
			    if(sess.secPolicy != "None" &&
				    !asymmetricVerify(certPEM2DER(cert())+io.attr("Nonce"),sign,servCert))
				throw OPCError(OpcUa_BadApplicationSignatureInvalid, "Application signature error");
			    iNu(rb, off, 4);				//maxRequest MessageSize
			    waitResponse = false;
			    break;
			}
			case OpcUa_ActivateSessionResponse: {
			    if(iTpId != OpcUa_ActivateSessionRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");
			    sess.servNonce = iS(rb, off);		//serverNonce
			    iN(rb, off, 4);				//results []
			    iN(rb, off, 4);				//diagnosticInfos []
			    waitResponse = false;
			    break;
			}
			case OpcUa_CloseSessionResponse:
			    if(iTpId != OpcUa_CloseSessionRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");
			    sess.clearSess(str2uint(io.attr("deleteSubscriptions")));
			    waitResponse = false;
			    break;
			case OpcUa_ReadResponse: {
			    if(iTpId != OpcUa_ReadRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");
									//> results []
			    uint32_t resN = iNu(rb, off, 4);		//Nodes number
			    for(uint32_t iR = 0; iR < resN && iR < io.childSize(); ++iR)
				iDataValue(rb, off, *io.childGet(iR));

			    //for(int iR = 0, stIdx_ = stIdx; iR < resN && stIdx_ < (int)io.childSize(); iR++, stIdx_++)
			    //	iDataValue(rb, off, *io.childGet(stIdx_));
			    //stIdx += std::min(25u, io.childSize()-stIdx);
									//>> diagnosticInfos []
			    iN(rb, off, 4);				//Items number
			    //if(stIdx < (int)io.childSize()) goto nextReq;
			    waitResponse = false;
			    break;
			}
			case OpcUa_WriteResponse: {
			    if(iTpId != OpcUa_WriteRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");
									//> results []
			    int resN = iNu(rb, off, 4);		//Number
			    for(int iR = 0; iR < resN && iR < (int)io.childSize(); iR++)
				io.childGet(iR)->setAttr("Status", strMess("0x%x",iNu(rb,off,4)));
									//>> diagnosticInfos []
			    iN(rb, off, 4);				//Items number
			    waitResponse = false;
			    break;
			}
			case OpcUa_BrowseResponse: {
			    if(iTpId != OpcUa_BrowseRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");
									//> results []
			    int resN = iNu(rb, off, 4);		//Numbers
			    for(int iR = 0; iR < resN && iR < (int)io.childSize(); iR++) {
				XML_N *rno = io.childGet(iR);
				//strtoul(rno->attr("resultMask").c_str(), NULL, 0);	//resultMask
				rno->setAttr("statusCode", uint2str(iNu(rb,off,4)));	//statusCode
				iS(rb, off);				//continuationPoint
									//>> References []
				int refN = iNu(rb, off, 4);		//Numbers
				for(int iRf = 0; iRf < refN; iRf++) {
				    XML_N *bno = rno->childAdd("bNode");
				    bno->setAttr("referenceTypeId", uint2str(iNodeId(rb,off).numbVal()));
				    bno->setAttr("isForward", iNu(rb,off,1)?"1":"0");
				    bno->setAttr("nodeId", iNodeId(rb,off).toAddr());
				    bno->setAttr("browseName", iSqlf(rb,off));
				    bno->setAttr("displayName", iSl(rb,off));
				    bno->setAttr("nodeClass", uint2str(iNu(rb,off,4)));
				    bno->setAttr("typeDefinition", uint2str(iNodeId(rb,off).numbVal()));
				}
			    }
			    iN(rb, off, 4);				//diagnosticInfos []
									//  !!!!: implement for parsing the non zero info
			    waitResponse = false;
			    break;
			}
			case OpcUa_CreateSubscriptionResponse: {
			    if(iTpId != OpcUa_CreateSubscriptionRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");

			    io.setAttr("subScrId", uint2str(iNu(rb,off,4)));		//subscriptionId
			    io.setAttr("publInterval", real2str(iR(rb,off,8)));	//revisedPublishingInterval
			    io.setAttr("lifetimeCnt", uint2str(iNu(rb,off,4)));	//revisedLifetimeCount
			    io.setAttr("maxKeepAliveCnt", uint2str(iNu(rb,off,4)));	//revisedMaxKeepAliveCount

			    waitResponse = false;
			    break;
			}
			case OpcUa_DeleteSubscriptionsResponse: {
			    if(iTpId != OpcUa_DeleteSubscriptionsRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");

			    uint32_t sN = iNu(rb, off, 4);		//results []
			    for(uint32_t iS = 0; iS < sN; ++iS) {
				uint32_t sC = iNu(rb, off, 4);		// statusCode
				if(iS < io.childSize())
				    io.childGet(iS)->setAttr("statusCode", uint2str(sC));
			    }
			    iN(rb, off, 4);				//diagnosticInfos []
									//  !!!!: implement for parsing the non zero info

			    waitResponse = false;
			    break;
			}
			case OpcUa_CreateMonitoredItemsResponse: {
			    if(iTpId != OpcUa_CreateMonitoredItemsRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");

			    uint32_t mItN = iNu(rb, off, 4);		//results []
			    for(uint32_t iIt = 0; iIt < mItN; ++iIt) {
				uint32_t sC = iNu(rb, off, 4);		// statusCode
				int32_t mItId = iN(rb, off, 4);	// monitoredItemId
				double smplInt = iR(rb, off, 8);	// revisedSamplingInterval
				uint32_t qSz = iNu(rb, off, 4);	// revisedQueueSize
				if(iIt < io.childSize()) {
				    XML_N *chO = io.childGet(iIt);
				    chO->setAttr("statusCode", uint2str(sC));
				    chO->setAttr("itId", int2str(mItId));
				    chO->setAttr("smplInt", real2str(smplInt));
				    chO->setAttr("qSz", uint2str(qSz));
				}
				iNodeId(rb, off);			// filterResult
				iNu(rb, off, 1);			// encodingMask
									//  !!!!: implement for non zero filter
			    }
			    iN(rb, off, 4);				//diagnosticInfos []
									//  !!!!: implement for parsing the non zero info

			    waitResponse = false;
			    break;
			}
			case OpcUa_DeleteMonitoredItemsResponse: {
			    if(iTpId != OpcUa_DeleteMonitoredItemsRequest)
				throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "NodeId of the response does not correspond the request");

			    uint32_t mItN = iNu(rb, off, 4);		//results []
			    for(uint32_t iIt = 0; iIt < mItN; ++iIt) {
				uint32_t sC = iNu(rb, off, 4);		// statusCode
				if(iIt < io.childSize())
				    io.childGet(iIt)->setAttr("statusCode", uint2str(sC));
			    }
			    iN(rb, off, 4);				//diagnosticInfos []
									//  !!!!: implement for parsing the non zero info

			    waitResponse = false;
			    break;
			}
			case OpcUa_PublishResponse: {
			    uint32_t subScrId = iNu(rb, off, 4);	//subscriptionId
			    Subscr *curSbscr = NULL;
			    for(unsigned iSbscr = 0; iSbscr < sess.mSubScr.size() && !curSbscr; ++iSbscr)
				if(sess.mSubScr[iSbscr].subScrId == subScrId) curSbscr = &sess.mSubScr[iSbscr];
			    if(!curSbscr) break;

			    curSbscr->lstPublTm = curTime();

			    for(bool isFound = false; !isFound && sess.mPublSeqs.size(); ) {
				isFound = (sess.mPublSeqs[0] == requestHandle);
				sess.mPublSeqs.erase(sess.mPublSeqs.begin());
			    }

			    // Available sequences processing
			    int32_t aSeqN = iN(rb, off, 4);		//availableSequence Numbers []
			    for(int iSeq = 0; iSeq < aSeqN; ++iSeq)
				//!!!! To implement at the republish implementing
				iNu(rb, off, 4);			// sequenceNumber

			    int moreNtfOff = iNu(rb, off, 1);		//moreNotifications
									//  !!!!: use somewhat in the future
									//notificationMessage
			    uint32_t curSeq = iNu(rb, off, 4);		// sequenceNumber, current
			    iTm(rb, off);				// publishTime
			    int32_t ntfN = iN(rb, off, 4);		// notificationData []
			    if(ntfN > 0) curSbscr->mSeqToAcq.push_back(curSeq);
			    for(int iNtf = 0; iNtf < ntfN; ++iNtf) {
				NodeId ntfType = iNodeId(rb, off);	//  TypeId (must be OpcUa_DataChangeNotification)
				int8_t eMsk = iNu(rb, off, 1);		//  encodingMask
				iNu(rb, off, 4);			//  extension object size
									//    !!!!: implement for checking to OpcUa_DataChangeNotification
									//          and reading/parsing the data block independently
				int32_t ntfItN = iN(rb, off, 4);	//  monitoredItems []
				XML_N tObj;
				OPCAlloc res(mtxData, true);
				for(int iNtfIt = 0; iNtfIt < ntfItN; ++iNtfIt) {
				    uint32_t clHdl = iNu(rb, off, 4);	//   clientHandle
				    iDataValue(rb, off, (clHdl<curSbscr->mItems.size())?curSbscr->mItems[clHdl].val:tObj);
				}
				iN(rb, off, 4);			//  diagnosticInfos []
									//  !!!!: implement for parsing the non zero info
			    }
			    int32_t resAckN = iN(rb, off, 4);		//results []
									//  !!!!: implement for the acknowledge status processing
			    for(int iResAckN = 0; iResAckN < resAckN; ++iResAckN)
				iNu(rb, off, 4);			// result
			    iN(rb, off, 4);				//diagnosticInfos []
									//  !!!!: implement for parsing the non zero info
			    break;
			}
			case OpcUa_ServiceFault: err = strMess("0x%x:%s", stCode, "Service fault");	break;
			default: err = strMess("0x%x:Not supported message response %d", OpcUa_BadServiceUnsupported, oTpId);	break;
		    }
		}
		if(rez.size() || (waitResponse && err.empty())) goto continueRead;
	    }
	}
	else err = strMess("0x%x:%s", OpcUa_BadServiceUnsupported, strMess("OPC_UA protocol '%s' isn't supported.", io.name().c_str()).c_str());
    }
    catch(OPCError &er) {
	err = strMess("0x%x:%s", er.cod, er.mess.c_str());
	if(debug) debugMess(io.attr("id")+" Error: "+err);
    }

    io.setAttr("err", err);
}

void Client::reqService( XML_N &io )
{
    //Common timings
    int64_t curTm = curTime();
    bool isScnlLive = ((curTm-sess.secChnlOpenTm)/1000ll < sess.secLifeTime);
    bool isSessLive = ((curTm-sess.secLstMessReqTm)/1000ll < sess.sesLifeTime);

    string ireq = io.attr("id");
    io.setAttr("err", "");
    XML_N req("opc.tcp");

    //Closing for previous session, secure channel and reopening new ones at:
    // the common command CloseALL, missing connection, end life time and changing the policy
    if(ireq == "CloseALL" || !sess.secChnl || !sess.secToken || !isScnlLive ||
	sess.endPoint != endPoint() || sess.secPolicy != secPolicy() || sess.secMessMode != secMessMode())
    {
	// Closing previous session or just clearing the session at error
	if(sess.authTkId.size() && ireq == "CloseALL") {
	    if(isScnlLive) {
		req.setAttr("id", "CloseSession")->setAttr("deleteSubscriptions", "1");
		protIO(req);
	    } else sess.clearSess(true);
	}

	// Closing previous secure channel
	if(sess.secChnl && sess.secToken) {
	    req.setAttr("id", "CLO")->setAttr("clearEP","1");
	    protIO(req);
	}

	if(ireq == "CloseALL") return;

	// Sending the HELLO message
	req.clear()->setAttr("id", "HEL")->setAttr("EndPoint", endPoint());
	protIO(req);
	if(!req.attr("err").empty())	{ io.setAttr("err", req.attr("err")); return; }

	// Sending the Open SecureChannel message for no secure policy
	req.setAttr("id", "OPN")->
	    setAttr("ReqType", int2str(SC_ISSUE))->
	    setAttr("SecPolicy", "None")->setAttr("SecurityMode", int2str(MS_None))->
	    setAttr("SecLifeTm", int2str(OpcUa_SecCnlDefLifeTime));
	protIO(req);
	if(!req.attr("err").empty())	{ io.setAttr("err", req.attr("err")); return; }

	if(secPolicy() != "None" || ireq == "GetEndpoints" || !sess.endPointDscr.childSize()) {
	    // Sending the GetEndpoints request for the server certificate retrieving and for the secure policy checking
	    req.setAttr("id", "GetEndpoints");
	    protIO(req);
	    if(!req.attr("err").empty()) { io.setAttr("err", req.attr("err")); return; }
	    //  Finding an endoint with needed secure policy
	    unsigned iEP;
	    for(iEP = 0; iEP < req.childSize(); iEP++)
		if(req.childGet(iEP)->name() == "EndpointDescription" &&
			strParse(req.childGet(iEP)->attr("securityPolicyUri"),1,"#") == secPolicy() &&
			atoi(req.childGet(iEP)->attr("securityMode").c_str()) == secMessMode())
		    break;
	    if(iEP >= req.childSize())
	    { io.setAttr("err",strMess("0x%x:%s",OpcUa_BadSecurityPolicyRejected,"No secure policy found")); return; }
	    sess.endPointDscr = *req.childGet(iEP);
	    if(ireq == "GetEndpoints") { io = req; return; }
	}

	// Reconnecting for the secure policy
	if(secPolicy() != "None") {
	    //  Sending the Close request for the not secure channel
	    req.clear()->setAttr("id", "CLO");
	    protIO(req);
	    if(!req.attr("err").empty()) { io.setAttr("err",req.attr("err")); return; }

	    //  Sending HELLO message
	    req.setAttr("id", "HEL");
	    protIO(req);
	    if(!req.attr("err").empty()) { io.setAttr("err",req.attr("err")); return; }

	    //  Sending the Open SecureChannel message for the secure policy
	    req.setAttr("id", "OPN")->
		setAttr("ReqType", int2str(SC_ISSUE))->
		setAttr("ServCert", sess.endPointDscr.childGet("serverCertificate")->text())->
		setAttr("SecurityMode", sess.endPointDscr.attr("securityMode"))->
		setAttr("SecLifeTm", int2str(OpcUa_SecCnlDefLifeTime));
	    protIO(req);
	    if(!req.attr("err").empty()) { io.setAttr("err",req.attr("err")); return; }
	}
    }
    //Sending the channel renew request at 75% remaining life time
    else if(1e-3*(curTm-sess.secChnlOpenTm) >= 0.75*sess.secLifeTime) {
	req.setAttr("id", "OPN")->
	    setAttr("ReqType", int2str(SC_RENEW))->
	    setAttr("SecLifeTm", int2str(OpcUa_SecCnlDefLifeTime));
	protIO(req);
	if(!req.attr("err").empty()) { io.setAttr("err",req.attr("err")); sess.clearSecCnl(); return; }
    }

    //Sending the session creation and activation messages with support of reusing of previous sessions at isSessLive
    if(ireq != "FindServers" && ireq != "GetEndpoints" && (sess.secChnlChanged || sess.authTkId.empty() || !isScnlLive)) {
	// Sending the CreateSession message
	if(sess.authTkId.empty() || !isSessLive) {
	    if(sess.authTkId.size())	sess.clearSess();
	    req.setAttr("id", "CreateSession")->
		setAttr("sesTm", "1.2e6");
	    protIO(req);
	    if(!req.attr("err").empty())	{ io.setAttr("err",req.attr("err")); sess.clearSecCnl(); return; }
	}

	// Sending the ActivateSession message
	req.setAttr("id", "ActivateSession");
	protIO(req);
	if(!req.attr("err").empty())	{ io.setAttr("err",req.attr("err")); sess.clearSess(); return; }

	sess.secChnlChanged = false;
    }

    //The same original request
    protIO(io);

    //Clearing the connection completly at the original request error OpcUa_BadInvalidArgument
    if(str2uint(io.attr("err")) == OpcUa_BadInvalidArgument || str2uint(io.attr("err")) == OpcUa_BadSecureChannelIdInvalid) {
	//sess.clearSess();
	sess.clearSecCnl();
    }
}

//***************************************************************
//* Client::Subscr - Subscription object by monitoreditems set	*
//***************************************************************
int Client::Subscr::monitoredItemAdd( const NodeId &nd, AttrIds aId, MonitoringMode mMode )
{
    XML_N req("opc.tcp");

    int hndl = -1;

    //Appending for new item
    if(!nd.isNull()) {
	int emptyIt = -1;
	OPCAlloc res(clnt->mtxData, true);
	for(unsigned iIt = 0; iIt < mItems.size() /*&& hndl < 0*/; iIt++)
	    if(mItems[iIt].nd == nd && mItems[iIt].aid == aId)
		hndl = iIt;
	    else if(mItems[iIt].nd.isNull() && emptyIt < 0)	emptyIt = iIt;

	if(hndl < 0) {
	    if(emptyIt >= 0)	hndl = emptyIt;
	    else { mItems.push_back(MonitItem(nd,aId,mMode)); hndl = mItems.size() - 1; }
	    MonitItem &itO = mItems[hndl];
	    itO.md = mMode;
	    itO.smplItv = publInterval;
	    itO.qSz = 1;
	    itO.active = false;
	    itO.st = 0;
	    itO.val.setAttr("nodata", "1");
	}
    }

    //Same registering the items
    if(subScrId && (hndl >= 0 || nd.isNull())) {
	req.setAttr("id", "CreateMonitoredItems")->
	    setAttr("subScrId", uint2str(subScrId))->
	    setAttr("tmstmpToRet", uint2str(TS_NEITHER));
	OPCAlloc res(clnt->mtxData, true);
	for(unsigned iIt = ((hndl>=0)?hndl:0); iIt < ((hndl>=0)?hndl+1:mItems.size()); ++iIt)
	    if(!mItems[iIt].active)	//Not registered still
		req.childAdd("mIt")->
		    setAttr("aId", uint2str(mItems[iIt].aid))->
		    setAttr("mMode", uint2str(mItems[iIt].md))->
		    setAttr("hndl", uint2str(iIt))->
		    setAttr("smplInt", real2str(mItems[iIt].smplItv))->
		    setAttr("qSz", uint2str(mItems[iIt].qSz))->
		    setText(mItems[iIt].nd.toAddr());
	res.unlock();

	if(req.childSize()) {
	    clnt->reqService(req);

	    // Processing the result
	    res.lock();
	    for(unsigned iCh = 0; iCh < req.childSize(); ++iCh) {
		XML_N *chO = req.childGet(iCh);
		uint32_t itId = str2uint(chO->attr("hndl"));
		if(itId >= mItems.size()) continue;
		mItems[itId].active = true;
		mItems[itId].st = str2uint(chO->attr("statusCode"));
		mItems[itId].smplItv = str2real(chO->attr("smplInt"));
		mItems[itId].qSz = str2uint(chO->attr("qSz"));
		mItems[itId].val.setAttr("nodata", "1");
	    }
	}
    }

    return hndl;
}

void Client::Subscr::monitoredItemDel( int32_t mItId, bool localDeactivation, bool onlyNoData )
{
    OPCAlloc res(clnt->mtxData, true);
    if(mItId >= (int)mItems.size())	return;

    for(unsigned iIt = ((mItId>=0)?mItId:0); iIt < ((mItId>=0)?mItId+1:mItems.size()); ++iIt)
	if(localDeactivation) {
	    if(!onlyNoData) { mItems[iIt].active = false; mItems[iIt].st = 0; }
	    mItems[iIt].val.setAttr("nodata", "1");
	}
	else {
	    XML_N req("opc.tcp");
	    req.setAttr("id", "DeleteMonitoredItems")->
		setAttr("subScrId", uint2str(subScrId));
	    req.childAdd("mIt")->setText(uint2str(mItId));
	    clnt->reqService(req);

	    //Processing the result
	    for(unsigned iCh = 0; iCh < req.childSize(); ++iCh) {
		XML_N *chO = req.childGet(iCh);
		if(str2uint(chO->attr("statusCode")) == 0 && mItId == str2uint(chO->attr("mIt"))) {
		    mItems[mItId].nd = NodeId();
		    mItems[mItId].active = false;
		    mItems[mItId].st = 0;
		    mItems[mItId].val.setAttr("nodata", "1");
		}
	    }
	}
}

void Client::Subscr::activate( bool vl, bool onlyLocally )
{
    if(vl == (bool)subScrId) return;	//Already active/deactive

    XML_N req("opc.tcp");
    if(vl) {
	req.setAttr("id", "CreateSubscription")->
	    setAttr("publInterval", real2str(publInterval))->
	    setAttr("lifetimeCnt", uint2str(lifetimeCnt))->
	    setAttr("maxKeepAliveCnt", uint2str(maxKeepAliveCnt))->
	    setAttr("maxNtfPerPubl", uint2str(maxNtfPerPubl))->
	    setAttr("publEn", publEn?"1":"0")->
	    setAttr("pr", uint2str(pr));
	clnt->reqService(req);
	if(str2uint(req.attr("subScrId"))) {
	    subScrId = str2uint(req.attr("subScrId"));
	    publInterval = str2real(req.attr("publInterval"));
	    lifetimeCnt = str2uint(req.attr("lifetimeCnt"));
	    maxKeepAliveCnt = str2uint(req.attr("maxKeepAliveCnt"));
	    lstPublTm = curTime();
	}

	//Activation off all registered monitoring items
	monitoredItemAdd(0);
    }
    else {
	if(!onlyLocally) {
	    req.setAttr("id", "DeleteSubscriptions");
	    req.childAdd("subscr")->setText(uint2str(subScrId));
	    clnt->reqService(req);
	}

	//if(req.childSize() && req.childGet(0)->attr("statusCode").size() && !str2uint(req.childGet(0)->attr("statusCode")))
	subScrId = 0;	//Independently from the status code during to possible server errors

	monitoredItemDel(-1, true);	//Deactivate-disable off all registered monitoring items
    }
}

//*************************************************
//* Protocol OPC UA server part			  *
//*************************************************
Server::Server( ) : mSecCnlIdLast(1)
{
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mtxData, &attrM);
    pthread_mutexattr_destroy(&attrM);
}

Server::~Server( )
{
    pthread_mutex_lock(&mtxData);
    pthread_mutex_destroy(&mtxData);
}

void Server::chnlList( vector<uint32_t> &chnls )
{
    OPCAlloc res(mtxData, true);

    chnls.clear();
    for(map<uint32_t,SecCnl>::iterator scIt = mSecCnl.begin(); scIt != mSecCnl.end(); ++scIt)
	chnls.push_back(scIt->first);
}

int Server::chnlSet( int cid, const string &iEp, int32_t lifeTm, const string& iClCert, const string &iSecPolicy, char iSecMessMode,
    const string &iclAddr, uint32_t iseqN )
{
    OPCAlloc res(mtxData, true);

    //Check for renew
    if(cid) {
	if(mSecCnl.find(cid) != mSecCnl.end()) {
	    mSecCnl[cid].tLife = lifeTm;
	    mSecCnl[cid].TokenIdPrev = mSecCnl[cid].TokenId;
	    if((++mSecCnl[cid].TokenId) == 0) mSecCnl[cid].TokenId = 1;
	    mSecCnl[cid].tCreate = curTime();
	    return cid;
	} else return -1;
    }

    //Checking to remove old lost channels and calculation the channels limit
    unsigned actSecCnl = 0;
    for(map<uint32_t,SecCnl>::iterator iSC = mSecCnl.begin(); iSC != mSecCnl.end(); )
	if(1e-6*(curTime()-iSC->second.tCreate) > 1e-3*iSC->second.tLife)
	    mSecCnl.erase(iSC++);
	else { ++iSC; ++actSecCnl; }
    if(actSecCnl > OpcUa_SecCnlLimit)	return -1;

    //Checking to Re-establish and to close old channells
    /*map<uint32_t, SecCnl>::iterator iSC = mSecCnl.begin();
    while(iSC != mSecCnl.end() && !((iseqN-iSC->second.clSeqN) < 10 && iseqN != iSC->second.startClSeqN &&
				iclAddr == iSC->second.clAddr && iClCert == iSC->second.clCert)) ++iSC;
    if(iSC != mSecCnl.end()) {
	if(debug()) debugMess(strMess("SecCnl: Re-establish detected for %d(%d): seqN=%d, clAddr='%s'.",
				iSC->first,iseqN,iSC->second.clSeqN,iSC->second.clAddr.c_str()));
	return iSC->first;
    }*/

    //New channel create
    do {
	if(!(++mSecCnlIdLast)) mSecCnlIdLast = 2;
    } while(mSecCnl.find(mSecCnlIdLast) != mSecCnl.end());

    mSecCnl[mSecCnlIdLast] = SecCnl(iEp, 1, lifeTm, iClCert, iSecPolicy, iSecMessMode, iclAddr, iseqN);

    return mSecCnlIdLast;
}

void Server::chnlClose( int cid )
{
    pthread_mutex_lock(&mtxData);
    mSecCnl.erase(cid);
    pthread_mutex_unlock(&mtxData);
}

Server::SecCnl Server::chnlGet( int cid )
{
    Server::SecCnl rez;

    pthread_mutex_lock(&mtxData);
    if(mSecCnl.find(cid) != mSecCnl.end()) rez = mSecCnl[cid];
    pthread_mutex_unlock(&mtxData);

    return rez;
}

void Server::chnlSecSet( int cid, const string &iServKey, const string &iClKey )
{
    OPCAlloc res(mtxData, true);
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

    //The same input request processing
nextReq:
    bool primVar = false;
    if(rba.size() <= 0) return holdConn;
    string rb, out;
    off = 4;

    try {
	if(rba.size() < 8) return holdConn;
	mSz = iNu(rba, off, 4);
	rb = rba.substr(0, std::min(mSz,(uint32_t)rba.size()));

	//Check for hello message type
	if(rb.compare(0,4,"HELF") == 0) {
	    if(rb.size() > 4096) throw OPCError(OpcUa_BadTcpMessageTooLarge, "", "");
	    if(rb.size() < mSz) return holdConn;

	    if(dbg) debugMess("HELLO Req");

	    off = 8;
	    iNu(rb, off, 4);				//Protocol version
	    clientRcvBufSzSet(inPrtId, iNu(rb,off,4));	//Recive buffer size
	    clientSndBufSzSet(inPrtId, iNu(rb,off,4));	//Send buffer size
	    clientMsgMaxSzSet(inPrtId, iNu(rb,off,4));	//Max message size
	    clientChunkMaxCntSet(inPrtId, iNu(rb,off,4)); //Max chunk count
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
	    oNu(out, std::min(rcvBufSz(),clientRcvBufSz(inPrtId)), 4);	//Recive buffer size
	    oNu(out, std::min(sndBufSz(),clientSndBufSz(inPrtId)), 4);	//Send buffer size
	    oNu(out, msgMaxSz(), 4);			//Max message size
	    oNu(out, chunkMaxCnt(), 4);			//Max chunk count

	    if(dbg) debugMess("HELLO Resp");
	}
	//Check for Open SecureChannel message type
	else if(rb.compare(0,4,"OPNF") == 0) {
	    if(rb.size() < mSz) return holdConn;
	    if(dbg) debugMess("OPN Req");

	    off = 8;
	    int chnlId = iNu(rb, off, 4);			//>SecureChannelId
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
	    int iEPOk = -1;
	    EP *wep = NULL;
	    for(int iEP = 0; iEPOk < 0 && iEP < (int)epLs.size(); iEP++) {
		if(!(wep=epEnAt(epLs[iEP])))	continue;
		for(int iS = 0; iEPOk < 0 && iS < wep->secN(); iS++)
		    if(wep->secPolicy(iS) == secPlc)
			iEPOk = iEP;
	    }
	    if(iEPOk < 0) throw OPCError(OpcUa_BadSecurityPolicyRejected, "", "");

	    string clntCert = certDER2PEM(iS(rb,off));		//>SenderCertificate
	    string serverCertThmbp = iS(rb, off);		//>ReceiverCertificateThumbrint
	    if(!isSecNone) {
		if(serverCertThmbp != certThumbprint(wep->cert()))//>ServerCertificateThumbprint
		    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Server certificate thumbprint error.");
		// Decode message block
		rb.replace(off, rb.size()-off, asymmetricDecrypt(rb.substr(off),wep->pvKey(),secPlc));
		if(dbg) debugMess("OPN Req (decrypted)");
	    }
	    uint32_t seqN = iNu(rb, off, 4);			//>Sequence number
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
	    for(int iS = 0; !secModOK && iS < wep->secN(); iS++)
		if(wep->secPolicy(iS) == secPlc && wep->secMessageMode(iS) == secMode)
		    secModOK = true;
	    if(!secModOK) throw OPCError(OpcUa_BadSecurityModeRejected, "", "");

	    if((chnlId=chnlSet((reqTp==SC_RENEW?chnlId:0),wep->id(),reqLifeTm,clntCert,secPlc,secMode,clientAddr(inPrtId),seqN)) < 0)
		throw OPCError(OpcUa_BadTcpSecureChannelUnknown, "", "");

	    // Preparing the response message
	    out.reserve(200);
	    out.append("OPNF");					//OpenSecureChannel message type
	    oNu(out, 0, 4);					//Message size
	    oNu(out, chnlId, 4);				//Secure channel identifier
	    oS(out, secPlcURI);					//Security policy URI
	    oS(out, isSecNone?string(""):certPEM2DER(wep->cert()));	//ServerCertificate
	    oS(out, isSecNone?string(""):certThumbprint(clntCert));	//ClientCertificateThumbprint
	    int begEncBlck = out.size();
	    OPCAlloc res(mtxData, true);
	    oNu(out, chnlGet_(chnlId).servSeqN++, 4);		//Sequence number
	    res.unlock();
	    oNu(out, reqId, 4);					//RequestId
								//> Extension Object
	    oNodeId(out, NodeId(OpcUa_OpenSecureChannelResponse));	//TypeId
								//>> Body
								//>>> RespondHeader
	    oTm(out, curTime());				//timestamp
	    oN(out, rqHndl, 4);				 	//requestHandle
	    oN(out, 0, 4);					//StatusCode
	    oN(out, 0, 1);					//serviceDiagnostics
	    oNu(out, 0, 4);					//stringTable
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
	    if(rb.size() < mSz) return holdConn;
	    off = 8;
	    uint32_t secId = iNu(rb, off, 4);			//Secure channel identifier
	    uint32_t tokId = iNu(rb, off, 4);			//TokenId

	    if(dbg) debugMess("CLO Req secId="+uint2str(secId)+", tokId="+uint2str(tokId));

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
	    if(scHd.secMessMode == MS_Sign || scHd.secMessMode == MS_SignAndEncrypt) {
		if(scHd.secMessMode == MS_SignAndEncrypt) off += iNu(rb, off, 1);	//Pass padding
		if(rb.substr(off) != symmetricSign(rb.substr(0,off),scHd.servKey,scHd.secPolicy))	//Check Signature
		    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Signature error");
	    }

	    chnlClose(secId);

	    // No respond. Close socket
	    return false;	//Close socket
	}
	//Check for SecureChannel message type
	else if(rb.compare(0,3,"MSG") == 0 && (rb[3] == 'F' || rb[3] == 'C' || rb[3] == 'A' || rb[3] == 'Q')) {
	    if(rb.size() < mSz) return holdConn;
	    off = 8;
	    uint32_t stCode = 0;
	    uint32_t secId = iNu(rb, off, 4);			//Secure channel identifier
	    uint32_t tokId = iNu(rb, off, 4);			//TokenId
	    SecCnl scHd = chnlGet(secId);
	    // Secure channel and token check
	    if(!scHd.TokenId) throw OPCError(OpcUa_BadSecureChannelClosed, "Secure channel closed");
	    if(!(tokId == scHd.TokenId || (tokId == scHd.TokenIdPrev && (curTime() < 1000ll*(scHd.tCreate+0.25*scHd.tLife)))))
		throw OPCError(OpcUa_BadSecureChannelTokenUnknown, "Secure channel unknown");
	    if(curTime() > (scHd.tCreate+(int64_t)scHd.tLife*1000))
		throw OPCError(OpcUa_BadSecureChannelIdInvalid, "Secure channel renew expired");
	    EP *wep = epEnAt(scHd.endPoint);
	    if(!wep) throw OPCError(OpcUa_BadTcpEndpointUrlInvalid, "No propper Endpoint present");
	    // Decrypt message block and signature check
	    if(rb[3] != 'Q' && (scHd.secMessMode == MS_Sign || scHd.secMessMode == MS_SignAndEncrypt)) {
		if(scHd.secMessMode == MS_SignAndEncrypt)
		    rb.replace(off, rb.size()-off, symmetricDecrypt(rb.substr(off),scHd.servKey,scHd.secPolicy));
		if(rb.substr(rb.size()-20) != symmetricSign(rb.substr(0,rb.size()-20),scHd.servKey,scHd.secPolicy))	//Check the signature
		    throw OPCError(OpcUa_BadTcpMessageTypeInvalid, "Signature error");
		rb.erase(rb.size()-20);				//Remove the signature
		if(scHd.secMessMode == MS_SignAndEncrypt)
		    rb.erase(rb.size()-(rb[rb.size()-1]+1));	//Remove the padding by the last byte value
	    }
	    OPCAlloc res(mtxData, true);
	    SecCnl &scHd_ = chnlGet_(secId);
								//> Sequence header
	    uint32_t clSeqN = iNu(rb, off, 4);			//Sequence number
	    uint32_t reqId = iNu(rb, off, 4);			//RequestId
	    //Chunks processing
	    if(rb[3] != 'Q') {
		bool passMessPrc = false;
		// Abort
		if(rb[3] == 'A') { scHd_.chCnt = 0; scHd_.chB = ""; passMessPrc = true; }
		else if(scHd.chCnt || rb[3] == 'C') {
		    //Checking for lost sequence
		    // * Lost single 'C' before 'F': pass to processing but there is possible single 'F', it will be checked by correct request type.
		    // * Lost intermediate 'F' ('C' both after and before): clean the chunks buffer and start it's filling from begin.
		    // * Lost first 'C': clean the chunks buffer and start it's filling from next 'C' but there is possible this is first 'C' after
		    //		single 'F' lost before, it will be checked by correct request type.
		    // * Lost intermediate 'C': clean and mark the chunks buffer as some chunk into sequence lost and pass all next chunks up to 'F'.
		    // * Lost intermediate 'C' just before 'F': clean the chunks buffer and 'F'.
		    // * Lost 'F' after 'C' before 'F': clean the chunks buffer and pass current 'F' to process but there is possible that is single 'F'
		    //		next, it will be checked by correct request type.
		    //!!!! Maybe set a mark for errors generation pass after checking to correct request type or the message's sequence begin.
		    if(scHd_.chCnt < 0 || (scHd_.chCnt && (clSeqN-scHd_.clSeqN) > 1)) {
			scHd_.chB = "";
			if(rb[3] == 'C' && reqId == scHd_.reqId) { scHd_.chCnt = -1; passMessPrc = true; }
			else { scHd_.chCnt = 0; passMessPrc = (reqId == scHd_.reqId); }
		    }
		    if(!passMessPrc) {
			scHd_.chB.append(rb, off, string::npos);
			if(rb[3] == 'C') {
			    // Checking for limits
			    scHd_.chCnt++;
			    if((chunkMaxCnt() && scHd_.chCnt > (int)chunkMaxCnt()) || (msgMaxSz() && scHd_.chB.size() > msgMaxSz()))
				throw OPCError(OpcUa_BadRequestTooLarge, "Request too large");
			    passMessPrc = true;
			}
			else {
			    rb.replace(off, string::npos, scHd_.chB);
			    scHd_.chCnt = 0; scHd_.chB = "";
			}
		    }
		}
		scHd_.clSeqN = clSeqN; scHd_.reqId = reqId;
		if(passMessPrc) { rba.erase(0, mSz); goto nextReq; }
	    }
								//> Extension body object
	    int reqTp = iNodeId(rb,off).numbVal();		//TypeId request
								//>> Request Header
	    uint32_t sesTokId = iNodeId(rb, off).numbVal();	//Session AuthenticationToken

	    if(dbg) debugMess(strMess("MSG Req: %d, seqN=%d, secId=%u, inPrtId=%s; sesTokId=%u.",reqTp,scHd_.clSeqN,secId,inPrtId.c_str(),sesTokId));

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

	    // Prepare respond message
	    string respEp;
	    switch(reqTp) {
		case OpcUa_FindServersRequest: {
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
		    for(unsigned iDU = 0; iDU < duLs.size(); iDU++)
			oS(respEp, duLs[iDU]);			//discoveryUrl
		    break;
		}
		case OpcUa_GetEndpointsRequest: {
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
		    for(unsigned iEP = 0; iEP < epLs.size(); iEP++) {
			EP *ep = epEnAt(epLs[iEP]);
			if(!ep) continue;
								//>>> EndpointDescription
			for(int iSec = 0; iSec < ep->secN(); iSec++, epCnt++) {
			    oS(respEp, ep->url()+"/OSCADA_OPC/"+ep->secPolicy(iSec)/*+"/"+ep->secMessMode(iSec)*/);    //endpointUrl
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
			    for(unsigned iDU = 0; iDU < duLs.size(); iDU++)
				oS(respEp,duLs[iDU]);		//discoveryUrl

			    oS(respEp, certPEM2DER(ep->cert()));//>>> serverCertificate
			    oNu(respEp, ep->secMessageMode(iSec), 4);	//securityMode:MessageSecurityMode
			    oS(respEp, "http://opcfoundation.org/UA/SecurityPolicy#"+ep->secPolicy(iSec));	//securityPolicyUri

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

			    oS(respEp, "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary");//transportProfileUri
			    oNu(respEp, (ep->secPolicy(iSec)!="None")?130:0, 1);	//securityLevel
			}
		    }
		    oNu(respEp, epCnt, 4, 0);			//EndpointDescrNubers list items

		    break;
		}
		case OpcUa_CreateSessionRequest: {
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
		    //int sessId = 0;
		    //if(!sesTokId && !wep->sessActivate(sesTokId,secId,true,inPrtId)) sessId = sesTokId;
		    //  Create new session
		    //if(!sessId) 
		    int sessId = wep->sessCreate(sessNm, rStm);
		    string servNonce = (scHd.secPolicy != "None") ? randBytes(32) : "";
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
		    for(unsigned iEP = 0; iEP < epLs.size(); iEP++) {
			EP *ep = epEnAt(epLs[iEP]);
			if(!ep) continue;
								//>>> EndpointDescription
			for(int iSec = 0; iSec < ep->secN(); iSec++, epCnt++) {
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
			    for(unsigned iDU = 0; iDU < duLs.size(); iDU++)
				oS(respEp, duLs[iDU]);		//discoveryUrl

			    oS(respEp, certPEM2DER(ep->cert()));//>>> serverCertificate
			    oNu(respEp, ep->secMessageMode(iSec), 4);	//securityMode:MessageSecurityMode
			    oS(respEp, "http://opcfoundation.org/UA/SecurityPolicy#"+ep->secPolicy(iSec));	//securityPolicyUri

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

			    oS(respEp, "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary");	//transportProfileUri
			    oNu(respEp, (ep->secPolicy(iSec)!="None")?10:0, 1);	//securityLevel
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
		case OpcUa_ActivateSessionRequest: {
		    //  Request
								//>clientSignature
		    string alg = iS(rb, off);			//> algorithm
		    string sign = iS(rb, off);			//> signature
		    if(scHd.secPolicy != "None") {
			if(!asymmetricVerify(certPEM2DER(wep->cert())+wep->sessGet(sesTokId).servNonce,sign,scHd.clCert))
			    throw OPCError(OpcUa_BadApplicationSignatureInvalid, "Application signature error");
		    }

		    iNu(rb, off, 4);				//>clientSoftwareCertificates []
		    uint32_t ln = iNu(rb, off, 4);		//>localeIds []
		    for(unsigned iL = 0; iL < ln; iL++)
			iS(rb, off);				//> localeId
								//>userIdentityToken
		    uint32_t userIdTk = iNodeId(rb, off).numbVal();	//> TypeId
		    iNu(rb, off, 1);				//> Encode
		    iNu(rb, off, 4);				//> Length
		    XML_N userIdent("IdentityToken");
		    switch(userIdTk) {
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
		    string servNonce = (scHd.secPolicy != "None") ? randBytes(32) : "";
		    wep->sessServNonceSet(sesTokId, servNonce);
		    oS(respEp, servNonce);			//serverNonce
		    oN(respEp, 0, 4);				//results []
		    oN(respEp, 0, 4);				//diagnosticInfos []
		    break;
		}
		case OpcUa_CloseSessionRequest: {
		    //  Request
		    bool subScrDel = iNu(rb, off, 1);		//deleteSubscriptions
		    wep->sessClose(sesTokId, subScrDel);

		    //  Respond
		    reqTp = OpcUa_CloseSessionResponse;
		    break;
		}
		case OpcUa_CreateSubscriptionRequest: {
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
		    wep->setPublish(inPrtId);
		    Subscr ss = wep->subscrGet(subScrId);

		    //  Respond
		    reqTp = OpcUa_CreateSubscriptionResponse;
		    respEp.reserve(20);
		    oNu(respEp, subScrId, 4);			//subscriptionId
		    oR(respEp, ss.publInterval, 8);		//revisedPublishingInterval
		    oNu(respEp, ss.lifetimeCnt, 4);		//revisedLifetimeCount
		    oNu(respEp, ss.maxKeepAliveCnt, 4);		//revisedMaxKeepAliveCount
		    break;
		}
		case OpcUa_ModifySubscriptionRequest: {
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
		    oR(respEp, ss.publInterval, 8);		//revisedPublishingInterval
		    oNu(respEp, ss.lifetimeCnt, 4);		//revisedLifetimeCount
		    oNu(respEp, ss.maxKeepAliveCnt, 4);		//revisedMaxKeepAliveCount
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

		    for(uint32_t iS = 0; iS < sn; iS++)
		    {
			uint32_t subScrId = iNu(rb, off, 4);	//subscriptionId
			Subscr ss = wep->subscrGet(subScrId);
			oNu(respEp, ((ss.st==SS_CLOSED)?OpcUa_BadSubscriptionIdInvalid:0), 4);	//statusCode
		    }
		    iNu(rb, off, 1);				//sendInitialValues

		    break;
		}*/
		case OpcUa_DeleteSubscriptionsRequest: {
		    //  Request
		    uint32_t sn = iNu(rb, off, 4);		//subscriptionIds []

		    //  Respond
		    reqTp = OpcUa_DeleteSubscriptionsResponse;
		    respEp.reserve(20);
		    oNu(respEp, sn, 4);				//results []

		    for(uint32_t iS = 0; iS < sn; iS++) {
			uint32_t subScrId = iNu(rb, off, 4);	//subscriptionId
			Subscr ss = wep->subscrGet(subScrId);
			oNu(respEp, ((ss.st==SS_CLOSED)?OpcUa_BadSubscriptionIdInvalid:0), 4);	//statusCode
			wep->subscrSet(subScrId, SS_CLOSED);
			if(dbg) debugMess(strMess("EP: SubScription %d closed.",subScrId));
		    }
		    oN(respEp, 0, 4);				//diagnosticInfos []
		    break;
		}
		case OpcUa_CreateMonitoredItemsRequest: primVar = true;
		case OpcUa_ModifyMonitoredItemsRequest: {
		    //  Request
		    uint32_t subScrId = iNu(rb, off, 4);	//>subscriptionId
		    if(wep->subscrGet(subScrId).st == SS_CLOSED) { reqTp = OpcUa_ServiceFault; stCode = OpcUa_BadSubscriptionIdInvalid; break; }

		    TimestampsToReturn tmStRet = (TimestampsToReturn)iNu(rb, off, 4);	//>timestampsToReturn
		    uint32_t ni = iNu(rb, off, 4);		//>itemsToCreate [] | itemsToModify []

		    //  Respond
		    reqTp = primVar ? OpcUa_CreateMonitoredItemsResponse : OpcUa_ModifyMonitoredItemsResponse;
		    oNu(respEp, ni, 4);				//<results []

		    //  Nodes list process and request form
		    for(uint32_t iM = 0; iM < ni; iM++) {
			NodeId nid;
			uint32_t aid = OpcUa_NPosID, mIt = 0;
			MonitoringMode mM = MM_CUR;
			if(primVar) {
								//> itemToMonitor
			    nid = iNodeId(rb, off);		//>  nodeId
			    aid = iNu(rb, off, 4);		//>  attributeId
			    iS(rb, off);			//>  indexRange
			    iSqlf(rb, off);			//>  dataEncoding
			    mM = (MonitoringMode)iNu(rb, off, 4);//> monitoringMode
			}
			else mIt = iNu(rb, off, 4);		//> monitoredItemId

								//> requestedParameters
			uint32_t cH = iNu(rb, off, 4);		//>  clientHandle
			double sI = iR(rb, off, 8);		//>  samplingInterval, in ms
			uint32_t fid = iNodeId(rb, off).numbVal();//>  filter
			iNu(rb, off, 1);			//>   EncodingMask
			XML_N fltr("MonitFltr");
			if(fid) {
			    fltr.setAttr("id", uint2str(fid));
			    uint32_t eSz = iNu(rb, off, 4);	//>   ExtObj
			    int offF = off; iVal(rb, off, eSz);	//>    Pass the filter part in the generic process

			    //   Filters parsing
			    if(fid == OpcUa_EventFilter && aid == AId_EventNotifier) {
				uint32_t nSelCls = iNu(rb, offF, 4);				//>    selectClauses []
				fltr.setAttr("nSelect", uint2str(nSelCls));
				for(unsigned iSel = 0; iSel < nSelCls; ++iSel) {		//>     SimpleAttribute Operand
				    XML_N *selCl = fltr.childAdd("select");
				    selCl->setAttr("nodeId", iNodeId(rb, offF).toAddr());	//>      nodeId
				    uint32_t nSelClsBrPath = iNu(rb, offF, 4);			//>      browsePath[]
				    for(unsigned iBp = 0; iBp < nSelClsBrPath; ++iBp)
					selCl->childAdd("browse")->setText(iSqlf(rb,offF));
				    selCl->setAttr("attributeId", int2str(iNu(rb,offF,4)));	//>      attributeId
				    selCl->setAttr("indexRange", int2str(iN(rb,offF,4)));	//>      indexRange
				}
				uint32_t nWCl = iNu(rb, offF, 4);	//>    whereClause, elements []
				fltr.setAttr("nWhere", uint2str(nWCl));
				for(unsigned iWCl = 0; iWCl < nWCl; ++iWCl) {
				    XML_N *wCl = fltr.childAdd("where");
				    wCl->setAttr("filterOperator", int2str(iNu(rb,offF,4)));	//>     filterOperator
				    uint32_t nWClOpr = iNu(rb, offF, 4);			//>     filterOperands []
				    for(unsigned iWClOpr = 0; iWClOpr < nWClOpr; ++iWClOpr) {
					int wClOpTp = iNodeId(rb, offF).numbVal();		//>      Extension's TypeId
					wCl->setAttr("typeId", int2str(wClOpTp));
					if(wClOpTp == OpcUa_LiteralOperand) {
					    iNu(rb, offF, 1);			//>   EncodingMask
					    eSz = iNu(rb, offF, 4);		//>   ExtObj
					    uint8_t emv;			//>   Value
					    wCl->setText(iVariant(rb,offF,&emv))->setAttr("VarTp", uint2str(emv));
					}
				    }
				}
			    }
			    else fltr.setAttr("id", "-1");	//Not supported or not processed filter mark
			}
			uint32_t qSz = iNu(rb, off, 4);		//>  queueSize
			bool dO = iNu(rb, off, 1);		//>  discardOldest

			//   Node result
			uint32_t st = 0;
			if(atoi(fltr.attr("id").c_str()) < 0) st = OpcUa_BadFilterNotAllowed;
			else if(mIt && wep->mItGet(subScrId,mIt).md == MM_DISABLED) st = OpcUa_BadMonitoredItemIdInvalid;
			else {
			    //   Create/modify new monitored item
			    mIt = wep->mItSet(subScrId, mIt, mM, nid, aid, tmStRet, sI, qSz, dO, cH, &fltr);
			    if(primVar) {
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
			    else {
				Subscr::MonitItem mItO = wep->mItGet(subScrId, mIt);
				sI = mItO.smplItv;
				qSz = mItO.qSz;
			    }
			}

			oNu(respEp, st, 4);				//< statusCode
			oN(respEp, mIt, 4);				//< monitoredItemId
			oR(respEp, sI, 8);				//< revisedSamplingInterval
			oNu(respEp, qSz, 4);				//< revisedQueueSize
			if(!st && fid) {				//<  nonzero EventFilterResult
			    oNodeId(respEp, OpcUa_EventFilterResult);	//< filterResult
			    oNu(respEp, 1, 1);				//< encodingMask
			    int szPos = respEp.size(); oNu(respEp, 0, 4);//<  eventFilterResult, size reserve
			    oNu(respEp, atoi(fltr.attr("nSelect").c_str()), 4);
			    for(unsigned iCl = 0; fltr.childGet("select",iCl,true); ++iCl)	//<  SelectClauseResults
				oNu(respEp, 0, 4);			//<   statusCode
			    oN(respEp, 0, 4);				//<  selectClauseDiagnosticInfos []
			    oNu(respEp, atoi(fltr.attr("nWhere").c_str()), 4);
			    for(unsigned iCl = 0; fltr.childGet("where",iCl,true); ++iCl) {	//<  WhereClauseResult
				oNu(respEp, 0, 4);			//<   statusCode
				oNu(respEp, 0, 4);			//<   operandStatusCodes []
				oNu(respEp, 0, 4);			//<   operandDiagnosticInfos []
			    }
			    oN(respEp, 0, 4);				//<  elementDiagnosticInfos []
			    oNu(respEp, respEp.size()-szPos-4, 4, szPos);//<  eventFilterResult, real size
			}
			else {
			    oNodeId(respEp, 0);				//< filterResult
			    oNu(respEp, 0, 1);				//< encodingMask
			}
		    }
		    oN(respEp, 0, 4);					//<diagnosticInfos []
		    break;
		}
		case OpcUa_SetMonitoringModeRequest: primVar = true;
		case OpcUa_DeleteMonitoredItemsRequest: {
		    //  Request
		    uint32_t subScrId = iNu(rb, off, 4);		//>subscriptionId
		    MonitoringMode mM = MM_DISABLED;
		    if(wep->subscrGet(subScrId).st == SS_CLOSED) { reqTp = OpcUa_ServiceFault; stCode = OpcUa_BadSubscriptionIdInvalid; break; }
		    if(primVar) {
			mM = (MonitoringMode)iNu(rb, off, 4);		//>monitoringMode
			reqTp = OpcUa_SetMonitoringModeResponse;
		    }
		    else reqTp = OpcUa_DeleteMonitoredItemsResponse;
		    uint32_t ni = iNu(rb, off, 4);			//>itemsToModify []

		    //  Respond
		    oNu(respEp, ni, 4);					//<results []
		    //  Nodes list process and request form
		    for(uint32_t iM = 0; iM < ni; iM++) {
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
		case OpcUa_SetPublishingModeRequest: {
		    //  Request
		    bool en = iNu(rb, off, 1);			//publishingEnabled
		    uint32_t sn = iNu(rb, off, 4);		//subscriptionIds []

		    //  Respond
		    reqTp = OpcUa_SetPublishingModeResponse;
		    respEp.reserve(20);
		    oNu(respEp, sn, 4);				//results []

		    for(uint32_t iS = 0; iS < sn; iS++) {
			uint32_t subScrId = iNu(rb, off, 4);	//subscriptionId
			Subscr ss = wep->subscrGet(subScrId);
			if(ss.st != SS_CLOSED) wep->subscrSet(subScrId, SS_CUR, en);
			oNu(respEp, ((ss.st==SS_CLOSED)?OpcUa_BadSubscriptionIdInvalid:0), 4);	//statusCode
		    }
		    oN(respEp, 0, 4);			//diagnosticInfos []
		    break;
		}
		case OpcUa_TranslateBrowsePathsToNodeIdsRequest: {
		    //  Request
							//> browsePaths []
		    uint32_t ip = iNu(rb, off, 4); 	//paths number

		    if(dbg) debugMess(strMess("TranslateBrowsePathsToNodeIdsRequest: paths=%d; off=%d(%d).",ip,off,rb.size()));

		    //  Respond
		    reqTp = OpcUa_TranslateBrowsePathsToNodeIdsResponse;
		    oNu(respEp, ip, 4);			//results []

		    //  Pathes list process and request form
		    for(unsigned i_p = 0; i_p < ip; i_p++) {
			if(dbg) debugMess(strMess("TranslateBrowsePathsToNodeIdsRequest: path=%d; off=%d.",i_p,off));
			NodeId sN = iNodeId(rb, off);	//startingNode
							//>> relativePath
			uint32_t irp = iNu(rb, off, 4);	//rpaths number

			bool nOK = true;
			XML_N req("data");
			for(unsigned iRp = 0; iRp < irp; iRp++) {
			    NodeId rTpId = iNodeId(rb, off);	//referenceTypeId
			    bool inv = iNu(rb, off, 1);		//isInverse
			    iNu(rb, off, 1);			//includeSubtypes
			    string tNm = iSqlf(rb, off);	//targetName

			    if(!nOK) continue;
			    //    Browse request for nodes
			    req.clear()->setAttr("node", sN.toAddr())->
					 setAttr("BrDir", uint2str(inv?BD_INVERSE:BD_FORWARD))->
					 setAttr("RefTpId", rTpId.toAddr());
			    wep->reqData(OpcUa_BrowseRequest, req);
			    unsigned iRef;
			    for(iRef = 0; iRef < req.childSize(); iRef++)
				if(req.childGet(iRef)->attr("name") == tNm) {
				    sN = NodeId::fromAddr(req.childGet(iRef)->attr("NodeId"));
				    break;
				}
			    nOK = (iRef < req.childSize());
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
		case OpcUaId_RegisterNodesRequest: {
		    //  Request
							//> nodesToRegister []
		    uint32_t nN = iNu(rb, off, 4); 	//nodes number

		    if(dbg) debugMess(strMess("RegisterNodesRequest: nodes=%d; off=%d(%d).",nN,off,rb.size()));

		    //  Respond
		    reqTp = OpcUaId_RegisterNodesResponse;
		    oNu(respEp, nN, 4);			//< registeredNodeIds []

		    //  Nodes list process
		    for(unsigned iN = 0; iN < nN; iN++) {
			if(dbg) debugMess(strMess("RegisterNodesRequest: node=%d; off=%d.",iN,off));
			NodeId rN = iNodeId(rb, off);	//>> registeredNode
			//!!!! Maybe some check for the node <rN> presence
			oNodeId(respEp, rN);		//<< registeredNode
		    }
		    break;
		}
		case OpcUaId_UnregisterNodesRequest: {
		    //  Request
							//> nodesToUnregister []
		    uint32_t nN = iNu(rb, off, 4); 	//nodes number

		    if(dbg) debugMess(strMess("UnregisterNodesRequest: nodes=%d; off=%d(%d).",nN,off,rb.size()));

		    //  Respond
		    reqTp = OpcUaId_UnregisterNodesResponse;

		    //  Nodes list process
		    for(unsigned iN = 0; iN < nN; iN++) {
			if(dbg) debugMess(strMess("UnregisterNodesRequest: node=%d; off=%d.",iN,off));
			NodeId uN = iNodeId(rb, off);	//>> unregisteredNode
			//!!!! Maybe some check for the node <uN> presence
		    }
		    break;
		}
		case OpcUa_BrowseRequest: {
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
		    for(uint32_t iC = 0; iC < nc; iC++) {
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
			for(unsigned iRef = 0; !stCode && iRef < req.childSize(); iRef++) {
			    XML_N *chN = req.childGet(iRef);
			    oRef(respEp, resMask, NodeId::fromAddr(chN->attr("NodeId")),
				NodeId::fromAddr(chN->attr("referenceTypeId")),
				atoi(chN->attr("dir").c_str()), chN->attr("name"), atoi(chN->attr("NodeClass").c_str()),
				NodeId::fromAddr(chN->attr("typeDefinition")));
			}

			if(stCode)	oNu(respEp, stCode, 4, stCodeOff);
			if(rPn && refNumb >= rPn && req.attr("LastNode").size()) { //continuationPoint prepare
			    string cP;
			    while(!wep->sessCpGet(sesTokId,(cP=randBytes(16))).empty()) ;
			    wep->sessCpSet(sesTokId, cP, Sess::ContPoint(nid.toAddr(),req.attr("LastNode"),bd,rPn,rtId.toAddr(),nClass,resMask));
			    oS(respEp, cP, cPntOff);
			    refNumbOff += cP.size();
			}
			if(refNumb)	oNu(respEp, refNumb, 4, refNumbOff);
		    }
		    oN(respEp, 0, 4);			//diagnosticInfos []

		    reqTp = OpcUa_BrowseResponse;

		    break;
		}
		case OpcUa_BrowseNextRequest: {
		    XML_N req("data");
		    iNu(rb, off, 1);			//releaseContinuationPoints
		    uint32_t nCp = iNu(rb, off, 4);	//continuationPoints []

		    //  Respond
							//> results []
		    oNu(respEp, nCp, 4);		//continuationPoints

		    //   Continuation points list processing
		    for(uint32_t iCp = 0; iCp < nCp; iCp++) {
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
			    for(unsigned iRef = 0; !stCode && iRef < req.childSize(); iRef++) {
				XML_N *chN = req.childGet(iRef);
				oRef(respEp, cPo.resMask, NodeId::fromAddr(chN->attr("NodeId")),
				    NodeId::fromAddr(chN->attr("referenceTypeId")),
				    atoi(chN->attr("dir").c_str()), chN->attr("name"), atoi(chN->attr("NodeClass").c_str()),
				    NodeId::fromAddr(chN->attr("typeDefinition")));
			    }
			    wep->sessCpSet(sesTokId, cp);	//Free previous continuationPoint. Unknown using "rCp" here!!!!
			}
			else stCode = OpcUa_BadContinuationPointInvalid;

			if(stCode)	oNu(respEp, stCode, 4, stCodeOff);
			if(refNumb >= cPo.refPerN && req.attr("LastNode").size()) { //continuationPoint prepare
			    string cP;
			    while(!wep->sessCpGet(sesTokId,(cP=randBytes(16))).empty()) ;
			    wep->sessCpSet(sesTokId, cP,
				Sess::ContPoint(cPo.brNode,req.attr("LastNode"),cPo.brDir,cPo.refPerN,cPo.refTypeId,cPo.nClassMask,cPo.resMask));
			    oS(respEp, cP, cPntOff);
			    refNumbOff += cP.size();
			}
			if(refNumb)	oNu(respEp, refNumb, 4, refNumbOff);
		    }
		    oN(respEp, 0, 4);			//diagnosticInfos []

		    reqTp = OpcUa_BrowseNextResponse;

		    break;
		}
		case OpcUa_ReadRequest: {
		    XML_N req("data");
		    //  Request
		    iR(rb, off, 8);				//maxAge
		    uint32_t tmStRet = iNu(rb, off, 4);		//timestampsTo Return
								//> nodesToRead []
		    uint32_t nc = iNu(rb, off, 4);		//nodes
		    uint8_t eMsk = 0x01;
		    switch(tmStRet) {
			case TS_SOURCE: eMsk |= 0x04;   break;
			case TS_SERVER: eMsk |= 0x08;   break;
			case TS_BOTH:   eMsk |= 0x0C;   break;
		    }

		    //  Respond
		    oNu(respEp, nc, 4);				//Numbers
		    //   Nodes list processing
		    for(uint32_t iC = 0; iC < nc; iC++) {
			NodeId nid = iNodeId(rb, off);		//nodeId
			uint32_t aid = iNu(rb, off, 4);		//attributeId
			iS(rb, off);				//indexRange
			iSqlf(rb, off);				//dataEncoding

			req.setAttr("node", nid.toAddr())->setAttr("aid",uint2str(aid));
			int rez = wep->reqData(reqTp, req);
			if(!rez) oDataValue(respEp, eMsk, req.text(), atoi(req.attr("type").c_str()));
			else oDataValue(respEp, 0x02, int2str(rez));
		    }

		    oN(respEp, 0, 4);				//diagnosticInfos []

		    reqTp = OpcUa_ReadResponse;

		    break;
		}
		case OpcUa_WriteRequest: {
		    XML_N req("data");
		    //  Request
							//> nodesToWrite []
		    uint32_t nc = iNu(rb, off, 4);	//nodes

		    //  Respond
		    oNu(respEp, nc, 4);			//Numbers
		    for(unsigned iN = 0; iN < nc; iN++) {
			NodeId nid = iNodeId(rb, off);	//nodeId
			uint32_t aid = iNu(rb, off, 4);	//attributeId (Value)
			iS(rb, off);			//indexRange
			XML_N nVal;
			iDataValue(rb, off, nVal);	//value

			req.setAttr("node", nid.toAddr())->
			    setAttr("aid", uint2str(aid))->
			    setAttr("VarTp", nVal.attr("VarTp"))->
			    setText(nVal.text());
			int rez = wep->reqData(reqTp, req);

			//   Write result status code
			oNu(respEp, rez, 4);		// StatusCode
		    }
		    oN(respEp, 0, 4);			//diagnosticInfos []

		    reqTp = OpcUa_WriteResponse;

		    break;
		}
		case OpcUa_PublishRequest: {
		    OPCAlloc mtx(wep->mtxData, true);

		    //  The publish request queue and/or process
		    Sess *s = wep->sessGet_(sesTokId);
		    if(s) {
			unsigned iP = 0;
			bool findOK = false;
			for( ; iP < s->publishReqs.size(); ++iP)
			    if((findOK=(rb==s->publishReqs[iP]))) break;
			if(!findOK) {
			    //   Early Acknowledgements processing and mark the results for this place on the entry's response
			    int off1 = off;
			    int32_t sa = iN(rb, off1, 4);			//>subscription Acknowledgements []
			    for(int iA = 0; iA < sa; iA++) {
				uint32_t prSSAck = iNu(rb, off1, 4);		//> subscriptionId
				uint32_t seqN = iNu(rb, off1, 4);		//> sequenceNumber
				uint32_t st = OpcUa_BadNoSubscription;
				if(prSSAck >= 1 && prSSAck <= wep->mSubScr.size()) {
				    prSSAck--;
				    st = OpcUa_BadSequenceNumberUnknown;
				    for(deque<string>::iterator iRQ = wep->mSubScr[prSSAck].retrQueue.begin();
						iRQ != wep->mSubScr[prSSAck].retrQueue.end(); ++iRQ)
				    {
					int rOff = 0;
					if(iNu(*iRQ,rOff,4) == seqN) { wep->mSubScr[prSSAck].retrQueue.erase(iRQ); st = 0; break; }
				    }
				}
				oNu(rb, 0, 4, off1-8); oNu(rb, st, 4, off1-4);
			    }
			    //   Queuing
			    rb[3] = 'Q';
			    oNu(rb, rb.size(), 4, 4);
			    s->publishReqs.push_back(rb);
			    //   Limiting the publish requests
			    while(s->publishReqs.size() > OpcUa_ServerMaxPublishQueue)
				s->publishReqs.pop_front();
			}
			if(findOK || s->publishReqs.size() == 1) {
			    unsigned prSS = wep->mSubScr.size();
			    for(unsigned iSs = 0; inPrtId == s->inPrtId && iSs < wep->mSubScr.size(); ++iSs)
				if(wep->mSubScr[iSs].sess == sesTokId &&
					(wep->mSubScr[iSs].toInit || wep->mSubScr[iSs].st == SS_LATE || wep->mSubScr[iSs].st == SS_KEEPALIVE) &&
					(prSS == wep->mSubScr.size() || wep->mSubScr[iSs].pr > wep->mSubScr[prSS].pr))
				    prSS = iSs;
			    if(prSS < wep->mSubScr.size()) {
				Subscr &ss = wep->mSubScr[prSS];

				//   Request
				string respAck;
				int32_t sa = iN(rb, off, 4);			//>subscription Acknowledgements []
				oN(respAck, sa, 4);				//<results []
				for(int iA = 0; iA < sa; iA++) {
				    uint32_t prSSAck = iNu(rb, off, 4);		//> subscriptionId
				    uint32_t seqN = iNu(rb, off, 4);		//> sequenceNumber
				    uint32_t st = (prSSAck == 0) ? seqN : OpcUa_BadNoSubscription;	//Place the early processed result
				    if(prSSAck >= 1 && prSSAck <= wep->mSubScr.size()) {
					prSSAck--;
					st = OpcUa_BadSequenceNumberUnknown;
					for(deque<string>::iterator iRQ = wep->mSubScr[prSSAck].retrQueue.begin();
						iRQ != wep->mSubScr[prSSAck].retrQueue.end(); ++iRQ)
					{
					    int rOff = 0;
					    if(iNu(*iRQ,rOff,4) == seqN) { wep->mSubScr[prSSAck].retrQueue.erase(iRQ); st = 0; break; }
					}
				    }
				    oNu(respAck, st, 4);			//< results
				}

				respEp.reserve(100);
				oNu(respEp, prSS+1, 4);				//<subscriptionId

				if(dbg) debugMess(strMess("Publish Resp: toInit=%d; st=%d",ss.toInit,ss.st));

				if(ss.toInit || ss.st == SS_KEEPALIVE) {
				    ss.toInit = false;
				    if(ss.st == SS_KEEPALIVE) ss.setState(SS_NORMAL);
				    oN(respEp, 0, 4);			//<availableSequence Numbers []
				    oNu(respEp, 0, 1);			//moreNotifications, FALSE
									//notificationMessage
				    oNu(respEp, ss.seqN, 4);		// sequenceNumber
				    oTm(respEp, curTime());		// publishTime
				    oN(respEp, 0, 4);			// notificationData []
				}
				else if(ss.st == SS_LATE) {
				    int aSeqOff = respEp.size(), aSeqN = 1;
				    oN(respEp, aSeqN, 4);		//<availableSequence Numbers [], reserve
				    for(deque<string>::iterator iRQ = ss.retrQueue.begin(); iRQ != ss.retrQueue.end(); ) {
					int rOff = 0;
					uint32_t rSeq = iNu(*iRQ, rOff, 4);	//>sequenceNumber
					int64_t rPblTm = iTm(*iRQ, rOff);	//>publishTime
					//Check for remove from queue by long age
					if((curTime()-rPblTm) > (wep->limRetrQueueTm()?1000000ll*wep->limRetrQueueTm():(int64_t)ss.maxKeepAliveCnt*ss.publInterval*1000))
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
				    unsigned iMIt = 0;
				    bool maxNtfPerPublLim = false, hasData = false;
				    for(unsigned iM = 0; !maxNtfPerPublLim && iM < ss.mItems.size(); iM++) {
					Subscr::MonitItem &mIt = ss.mItems[iM];
					if(!(mIt.md == MM_REPORTING && mIt.vQueue.size())) continue;
					uint8_t eMsk = 0x01;
					switch(mIt.tmToRet) {
					    case TS_SOURCE: eMsk |= 0x04;	break;
					    case TS_SERVER: eMsk |= 0x08;	break;
					    case TS_BOTH:   eMsk |= 0x0C;	break;
					    default:				break;
					}
					while(mIt.vQueue.size()) {
					    if(ss.maxNtfPerPubl && iMIt >= ss.maxNtfPerPubl) { maxNtfPerPublLim = true; break; }
					    oNu(respEp, mIt.cH, 4);		//<   clientHandle
					    if(mIt.vQueue.front().st)
						oDataValue(respEp, 0x0A, uint2str(mIt.vQueue.front().st), 0, mIt.vQueue.front().tm);	//<   status
					    else oDataValue(respEp, eMsk, mIt.vQueue.front().vl, mIt.vTp, mIt.vQueue.front().tm);	//<   value
					    mIt.vQueue.pop_front();
					    if(mIt.vQueue.size()) hasData = true;
					    iMIt++;
					}
				    }
				    if(!hasData) ss.setState(SS_NORMAL);
				    oN(respEp, 0, 4);				//<   diagnosticInfos []
				    oNu(respEp, maxNtfPerPublLim, 1, moreNtfOff);//<moreNotifications, real value write
				    oN(respEp, iMIt, 4, mItOff);		//<  monitoredItems [], real items number write
				    oNu(respEp, respEp.size()-extObjOff-4, 4, extObjOff);	//<  extension object real size write

				    ss.retrQueue.push_back(respEp.substr(ntfMsgOff));	//Queue to retranslation
				    ss.seqN++;
				    if(maxNtfPerPublLim) ss.setState(SS_LATE);	//Restore state to process into the next Publish request,
										// !!!! maybe store the previous monitored item's position
										//      and early call its into the subScrCycle()
				}
				s->publishReqs.erase(s->publishReqs.begin()+iP);//Remove the publish request from the queue
				ss.pubCntr++;

				//oN(respEp, -1, 4);			//<results [], empty
				respEp += respAck;			//<results []
				oN(respEp, 0, 4);			//<diagnosticInfos []
			    }
			}
		    }
		    mtx.unlock();

		    if(respEp.empty())	throw OPCError(0, "", "");	//> No response now

		    reqTp = OpcUa_PublishResponse;

		    break;
		}
		case OpcUa_RepublishRequest: {
		    //  Request
		    uint32_t prSS = iNu(rb, off, 4);			//> subscriptionId
		    uint32_t seqN = iNu(rb, off, 4);			//> retransmitSequenceNumber
		    if(wep->subscrGet(prSS).st == SS_CLOSED) { stCode = OpcUa_BadSubscriptionIdInvalid; reqTp = OpcUa_ServiceFault; break; }

		    OPCAlloc mtx(wep->mtxData, true);
		    Subscr &ss = wep->mSubScr[prSS-1];
		    deque<string>::iterator iRQ = ss.retrQueue.begin();
		    for( ; iRQ != ss.retrQueue.end(); ++iRQ) {
			int rOff = 0;
			if(iNu(*iRQ,rOff,4) == seqN)	break;
		    }
		    if(iRQ == ss.retrQueue.end()) {
			stCode = OpcUa_BadMessageNotAvailable; reqTp = OpcUa_ServiceFault;
			break;
		    }

		    //  Respond
		    reqTp = OpcUa_RepublishResponse;
		    respEp = *iRQ;

		    break;
		}
		case OpcUa_ServiceFault:	break;
		default:
		    throw OPCError(OpcUa_BadNotSupported, "No supported request id '%d'.", reqTp);
	    }

	    // Respond's extension object finalize
	    string respBody; respBody.reserve(50);
	    oNodeId(respBody, reqTp);			//TypeId
							//>> Body
							//>>> RespondHeader
	    oTm(respBody, curTime());			//timestamp
	    oN(respBody, reqHndl, 4);			//requestHandle
	    oN(respBody, stCode, 4);			//StatusCode
	    oN(respBody, 0, 1);				//serviceDiagnostics
	    oNu(respBody, 0, 4);			//stringTable
							//>>> Extensible parameter
	    oNodeId(respBody, 0u);			//TypeId (0)
	    oNu(respBody, 0, 1);			//Encoding
	    respBody.append(respEp); respEp.clear();

	    // Maximum chunk's body size calculation for client's recieve buffer size
	    unsigned chnkBodySz = clientRcvBufSz(inPrtId) - 16;	// - {HeadSz}
	    if(scHd.secMessMode == MS_Sign || scHd.secMessMode == MS_SignAndEncrypt) {
		if(scHd.secMessMode == MS_SignAndEncrypt) {
		    int kSz = scHd.servKey.size()/3;
		    chnkBodySz = (chnkBodySz/kSz)*kSz - 1;	//Allign to the server key size and remove same padding size's byte
		}
		chnkBodySz -= 20;			//Remove the sign size
	    }
	    chnkBodySz -= 8;	//- {SeqSz}
	    if((clientMsgMaxSz(inPrtId) && respBody.size() > clientMsgMaxSz(inPrtId)) ||
		    (clientChunkMaxCnt(inPrtId) && (respBody.size()/chnkBodySz + ((respBody.size()%chnkBodySz)?1:0)) > clientChunkMaxCnt(inPrtId)))
		throw OPCError(OpcUa_BadResponseTooLarge, "Respond too large");
	    // Same chunks prepare.
	    while(respBody.size()) {
		string resp; resp.reserve(200);
		bool isFinal = (respBody.size() <= chnkBodySz);
		resp.append("MSG");			//OpenSecureChannel message type
		resp.append(isFinal?"F":"C");
		oNu(resp, 0, 4);			//Message size
		oNu(resp, secId, 4);			//Secure channel identifier
		oNu(resp, tokId, 4);			//Symmetric Algorithm Security Header : TokenId
		int begEncBlck = resp.size();
							//> Sequence header
		oNu(resp, scHd_.servSeqN++, 4);		//Sequence number
		oNu(resp, reqId, 4);			//RequestId
		resp.append(respBody,0,std::min(chnkBodySz,(unsigned)respBody.size()));
		respBody.erase(0,std::min(chnkBodySz,(unsigned)respBody.size()));
		oNu(resp, resp.size(), 4, 4);		//Real message size

		if(scHd.secMessMode == MS_Sign || scHd.secMessMode == MS_SignAndEncrypt) {
		    //Padding place
		    if(scHd.secMessMode == MS_SignAndEncrypt) {
			int kSz = scHd.clKey.size()/3;
			int paddingSize = ((resp.size()-begEncBlck+1+20+kSz-1)/kSz)*kSz - (resp.size()-begEncBlck+20);
			resp += string(paddingSize, (char)(paddingSize-1));
		    }
		    //Real message size calc and place
		    oNu(resp, resp.size()+20, 4, 4);

		    //Signature
		    resp += symmetricSign(resp, scHd.clKey, scHd.secPolicy);

		    //Encoding
		    if(scHd.secMessMode == MS_SignAndEncrypt)
			resp.replace(begEncBlck, resp.size()-begEncBlck, symmetricEncrypt(resp.substr(begEncBlck),scHd.clKey,scHd.secPolicy));
		}
		if(answ && isFinal) out += resp;	//!!!! isFinal is to ensure separate packages for chunks
		else writeToClient(inPrtId, resp);
	    }

	    /*out.reserve(200);
	    out.append("MSGF");				//OpenSecureChannel message type
	    oNu(out, 0, 4);				//Message size
	    oNu(out, secId, 4);				//Secure channel identifier
	    oNu(out, tokId, 4);				//Symmetric Algorithm Security Header : TokenId
	    int begEncBlck = out.size();
							//> Sequence header
	    oNu(out, scHd_.servSeqN++, 4);		//Sequence number
	    oNu(out, reqId, 4);				//RequestId
							//> Extension Object
	    oNodeId(out, reqTp);			//TypeId
							//>> Body
							//>>> RespondHeader
	    oTm(out, curTime());			//timestamp
	    oN(out, reqHndl, 4);			//requestHandle
	    oN(out, stCode, 4);				//StatusCode
	    oN(out, 0, 1);				//serviceDiagnostics
	    oNu(out, 0, 4);				//stringTable
							//>>> Extensible parameter
	    oNodeId(out, 0u);				//TypeId (0)
	    oNu(out, 0, 1);				//Encoding
	    out.append(respEp);
	    oNu(out, out.size(), 4, 4);			//Real message size

	    if(scHd.secMessMode == MS_Sign || scHd.secMessMode == MS_SignAndEncrypt) {
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
	    }*/

	    if(dbg) debugMess(strMess("MSG Resp: %d",reqTp));
	}
	else throw OPCError(OpcUa_BadNotSupported, "", "");
    } catch(OPCError &er) {
	if(er.cod) {
	    if(dbg) debugMess(strMess("MSG Error: %xh:%s",er.cod,er.mess.c_str()));
	    out = mkError(er.cod, er.mess);
	    holdConn = false;
	    mSz = rba.size();	//Drop request for prevent broken requests hang, only for er.cod != 0
	}
    }

    if(answ) answ->append(out);
    else if(out.size()) writeToClient(inPrtId, out);

    rba.erase(0, mSz);
    goto nextReq;

    return true;
}

//*************************************************
//* SecCnl					  *
//*************************************************
Server::SecCnl::SecCnl( const string &iEp, uint32_t iTokenId, int32_t iLifeTm,
	const string &iClCert, const string &iSecPolicy, char iSecMessMode, const string &iclAddr, uint32_t isecN ) :
    endPoint(iEp), secPolicy(iSecPolicy), secMessMode(iSecMessMode), tCreate(curTime()),
    tLife(std::max(OpcUa_SecCnlDefLifeTime,iLifeTm)), TokenId(iTokenId), TokenIdPrev(0), clCert(iClCert), clAddr(iclAddr),
    servSeqN(isecN), clSeqN(isecN), /*startClSeqN(isecN),*/ reqId(0), chCnt(0)
{

}

Server::SecCnl::SecCnl( ) :
    secMessMode(MS_None), tCreate(0/*curTime()*/), tLife(OpcUa_SecCnlDefLifeTime), TokenId(0), TokenIdPrev(0), servSeqN(1), clSeqN(1)//, startClSeqN(1)
{

}

//*************************************************
//* Server::Sess				  *
//*************************************************
Server::Sess::Sess( const string &iName, double iTInact ) : name(iName), secCnl(0), tInact(std::max(iTInact,1.0)), tAccess(curTime())
{

}

Server::Sess::Sess( ) : secCnl(0), tInact(0), tAccess(0)	{ }

bool Server::Sess::isSecCnlActive( EP *ep )
{
    if(secCnl) {
	SecCnl &sCnlO = ep->serv->chnlGet_(secCnl);
	if(!sCnlO.tCreate || (sCnlO.tCreate && (1e-3*sCnlO.tLife-1e-6*(curTime()-sCnlO.tCreate)) <= 0))
	    secCnl = 0;
    }

    return secCnl;
}

//*************************************************
//* Server::Subscr                                *
//*************************************************
Server::Subscr Server::Subscr::copy( bool noWorkData )
{
    Subscr rez;
    if(!noWorkData) rez = *this;
    else {
	rez.st = st;
	rez.sess = sess;
	rez.publEn = publEn;
	rez.publInterval = publInterval;
	rez.lifetimeCnt = lifetimeCnt;
	rez.maxKeepAliveCnt = maxKeepAliveCnt;
	rez.maxNtfPerPubl = maxNtfPerPubl;
	rez.pr = pr;
    }

    return rez;
}

SubScrSt Server::Subscr::setState( SubScrSt ist )
{
    if(ist == st) return st;
    switch(ist) {
	case SS_CUR:	return st;
	case SS_CLOSED:	//Clear the object
	    mItems.clear();
	    retrQueue.clear();
	    publEn = false;
	    seqN = 1;
	    wLT = wKA = 0;
	    break;
	case SS_NORMAL:
	    switch(st) {
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
    pthread_mutexattr_t attrM;
    pthread_mutexattr_init(&attrM);
    pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mtxData, &attrM);
    pthread_mutexattr_destroy(&attrM);
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

    if(vl) {
     //Objects tree and nodes map init
     nodeReg(0u,OpcUa_RootFolder,"Root",NC_Object,OpcUa_Organizes,OpcUa_FolderType);
      nodeReg(OpcUa_RootFolder,OpcUa_ViewsFolder,"Views",NC_Object,OpcUa_Organizes,OpcUa_FolderType);
      nodeReg(OpcUa_RootFolder,OpcUa_ObjectsFolder,"Objects",NC_Object,OpcUa_Organizes,OpcUa_FolderType);
       nodeReg(OpcUa_ObjectsFolder,OpcUa_Server,"Server",NC_Object,OpcUa_Organizes,OpcUa_ServerType);
	nodeReg(OpcUa_Server,OpcUa_Server_Auditing,"Auditing",NC_Variable,OpcUa_HasProperty,OpcUa_PropertyType)->
	    setAttr("Value","0")->setAttr("DataType",int2str(OpcUa_Boolean));
	nodeReg(OpcUa_Server,OpcUa_Server_ServerStatus,"ServerStatus",NC_Variable,OpcUa_HasComponent,OpcUa_ServerStatusType)->
	    setAttr("Value","Running")->setAttr("DataType",int2str(OpcUa_String/*OpcUa_ServerStatusDataType*/));
	 nodeReg(OpcUa_Server_ServerStatus,OpcUa_Server_ServerStatus_State,"State",NC_Variable,OpcUa_HasComponent,OpcUa_BaseDataVariableType)->
	    setAttr("Value","0")->setAttr("DataType",int2str(OpcUa_Int32));
	nodeReg(OpcUa_Server,OpcUa_Server_NamespaceArray,"NamespaceArray",NC_Variable,OpcUa_HasProperty,OpcUa_PropertyType)->
	    setAttr("ValueRank","1")->setAttr("Value","http://opcfoundation.org/UA/\n"+serv->applicationUri()+"\nhttp://opcfoundation.org/UA/DI/\nhttp://PLCopen.org/OpcUa/IEC61131-3/")->setAttr("DataType",int2str(OpcUa_Array|OpcUa_String));
	nodeReg(OpcUa_Server,OpcUa_Server_ServerArray,"ServerArray",NC_Variable,OpcUa_HasProperty,OpcUa_PropertyType)->
	    setAttr("ValueRank","1")->setAttr("Value",serv->applicationUri())->setAttr("DataType",int2str(OpcUa_Array|OpcUa_String));
	nodeReg(OpcUa_Server,OpcUa_Server_ServerCapabilities,"ServerCapabilities",NC_Object,OpcUa_HasComponent,OpcUa_ServerCapabilitiesType);
	 //nodeReg(OpcUa_Server_ServerCapabilities,OpcUa_Server_ServerCapabilities_MinSupportedSampleRate,"MinSupportedSampleRate",NC_Variable,OpcUa_HasProperty,OpcUa_PropertyType)->
	 //    setAttr("Value","10")->setAttr("DataType",int2str(OpcUa_UInt32));
	 nodeReg(OpcUa_Server_ServerCapabilities,OpcUa_Server_ServerCapabilities_MaxArrayLength,"MaxArrayLength",NC_Variable,OpcUa_HasProperty,OpcUa_PropertyType)->
	    setAttr("Value","65536")->setAttr("DataType",int2str(OpcUa_UInt32));
	 nodeReg(OpcUa_Server_ServerCapabilities,OpcUa_Server_ServerCapabilities_MaxStringLength,"MaxStringLength",NC_Variable,OpcUa_HasProperty,OpcUa_PropertyType)->
	    setAttr("Value","16777216")->setAttr("DataType",int2str(OpcUa_UInt32));
	 nodeReg(OpcUa_Server_ServerCapabilities,OpcUa_Server_ServerCapabilities_MaxBrowseContinuationPoints,"MaxBrowseContinuationPoints",NC_Variable,OpcUa_HasProperty,OpcUa_PropertyType)->
	    setAttr("Value","5")->setAttr("DataType",int2str(OpcUa_UInt16));
	 nodeReg(OpcUa_Server_ServerCapabilities,OpcUa_Server_ServerCapabilities_MaxQueryContinuationPoints,"MaxQueryContinuationPoints",NC_Variable,OpcUa_HasProperty,OpcUa_PropertyType)->
	    setAttr("Value","5")->setAttr("DataType",int2str(OpcUa_UInt16));
	 nodeReg(OpcUa_Server_ServerCapabilities,OpcUa_Server_ServerCapabilities_MaxHistoryContinuationPoints,"MaxHistoryContinuationPoints",NC_Variable,OpcUa_HasProperty,OpcUa_PropertyType)->
	    setAttr("Value","5")->setAttr("DataType",int2str(OpcUa_UInt16));
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
	   nodeReg(OpcUa_Integer,OpcUa_IntAuto,"IntAuto",NC_DataType,OpcUa_HasSubtype);
	  nodeReg(OpcUa_Number,OpcUa_UInteger,"UInteger",NC_DataType,OpcUa_HasSubtype)->setAttr("IsAbstract","1");
	   nodeReg(OpcUa_UInteger,OpcUa_Byte,"Byte",NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_UInteger,OpcUa_UInt16,"UInt16",NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_UInteger,OpcUa_UInt32,"UInt32",NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_UInteger,OpcUa_UInt64,"UInt64",NC_DataType,OpcUa_HasSubtype);
	   nodeReg(OpcUa_UInteger,OpcUa_UIntAuto,"UIntAuto",NC_DataType,OpcUa_HasSubtype);
	  nodeReg(OpcUa_Number,OpcUa_Double,"Double",NC_DataType,OpcUa_HasSubtype);
	  nodeReg(OpcUa_Number,OpcUa_Float,"Float",NC_DataType,OpcUa_HasSubtype);
	 nodeReg(OpcUa_BaseDataType,OpcUa_String,"String",NC_DataType,OpcUa_HasSubtype);
    }
    else {
	ndMap.clear();
	objTree.clear();
    }

    mEn = vl;
}

void Server::EP::subScrCycle( unsigned cntr )
{
    OPCAlloc mtx(mtxData, true);

    //Subscriptions processing
    Sess *s = NULL;
    int64_t vTm = 0;
    for(unsigned iSc = 0; iSc < mSubScr.size(); ++iSc) {
	Subscr &scr = mSubScr[iSc];
	if(scr.st == SS_CLOSED) continue;
	if(!(s=sessGet_(scr.sess)) || !s->tAccess) { scr.setState(SS_CLOSED); continue; }
	// Checking for the security channel correspondence
	bool isSecCnlAct = s->isSecCnlActive(this);

	// Monitored items processing
	bool hasData = false;
	XML_N req("data");
	for(unsigned iM = 0; iM < scr.mItems.size(); ++iM) {
	    Subscr::MonitItem &mIt = scr.mItems[iM];

	    if(mIt.md == MM_DISABLED || (cntr%std::max(1u,(unsigned)(mIt.smplItv/subscrProcPer()))))	continue;

	    //  Read data
	    req.setAttr("node", mIt.nd.toAddr())->setAttr("aid", uint2str(mIt.aid))->setAttr("dtTmGet","1");
		//setAttr("dtTmGet",(mIt.tmToRet==TS_SOURCE||mIt.tmToRet==TS_BOTH)?"1":"0");
	    if(!reqData(OpcUa_ReadRequest,req) && (vTm=strtoll(req.attr("dtTm").c_str(),NULL,10)) > mIt.dtTm) {
		mIt.vTp = atoi(req.attr("type").c_str());
		mIt.dtTm = vTm;
		mIt.vQueue.push_back(Subscr::MonitItem::Val(req.text(),vTm));
		if(mIt.vQueue.size() > mIt.qSz) {
		    if(mIt.dO) mIt.vQueue.pop_front();
		    else mIt.vQueue.pop_back();
		}
		hasData = true;
	    }
	}
	if(hasData && isSecCnlAct) scr.setState(SS_LATE);

	if(cntr%std::max(1u,(unsigned)(scr.publInterval/subscrProcPer())))	continue;

	if(scr.pubCntr_ != scr.pubCntr || scr.st == SS_CREATING) {
	    scr.wLT = 0;
	    if(scr.toInit || scr.st == SS_LATE)	scr.wKA = 0;
	    else if((++scr.wKA) >= scr.maxKeepAliveCnt)	scr.setState(SS_KEEPALIVE);
	}
	else if((++scr.wLT) >= scr.lifetimeCnt) {
	    // Send StatusChangeNotification with Bad_Timeout
	    //!!!!
	    scr.setState(SS_CLOSED);	//Free Subscription
	}
	if(scr.st == SS_LATE || scr.st == SS_KEEPALIVE)	scr.pubCntr_ = scr.pubCntr;
    }
}

void Server::EP::publishCall( string *answ, const string &inPrtId )
{
    OPCAlloc mtx(mtxData, true);

    Sess *s = NULL;
    for(unsigned iSc = 0; iSc < mSubScr.size(); ++iSc) {
	Subscr &scr = mSubScr[iSc];
	if(!(scr.st == SS_LATE || scr.st == SS_KEEPALIVE) ||
		!(s=sessGet_(scr.sess)) || !s->tAccess || !s->isSecCnlActive(this) ||
		(inPrtId.size() && inPrtId != s->inPrtId) || s->publishReqs.empty())
	    continue;

	string req = s->publishReqs.front(), inPrt = s->inPrtId;
	mtx.unlock();
	serv->inReq(req, inPrt, answ);
	mtx.lock();
    }
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
    OPCAlloc mtx(mtxData, true);
    int iS;
    for(iS = 0; iS < (int)mSess.size(); ++iS)
	if(!mSess[iS].tAccess || 1e-3*(curTime()-mSess[iS].tAccess) > mSess[iS].tInact)
	    break;
    if(iS < (int)mSess.size()) {
	sessClose(iS+1);
	mSess[iS] = Sess(iName, iTInact);
    }
    else mSess.push_back(Sess(iName,iTInact));

    return iS+1;
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

    OPCAlloc mtx(mtxData, true);
    //Checking for the target session presence
    if(sid > 0 && sid <= (int)mSess.size() && mSess[sid-1].tAccess) {
	mSess[sid-1].tAccess = curTime();
	// Error the present session using at changing the physical channel
	if(check && inPrtId != mSess[sid-1].inPrtId)	return OpcUa_BadSecureChannelIdInvalid;
	mSess[sid-1].inPrtId = inPrtId;
	if(!check || secCnl == mSess[sid-1].secCnl) {
	    // New sequre channel binding to the actual session
	    if(secCnl != mSess[sid-1].secCnl) {
		mSess[sid-1].secCnl = secCnl;
		setPublish(inPrtId);
		mSess[sid-1].publishReqs.clear();

		for(unsigned iS = 0; iS < mSess.size(); ++iS)
		    if(iS != (sid-1) && mSess[iS].inPrtId == inPrtId)
			mSess[iS].inPrtId = "";
	    }
	    rez = 0;
	}
    }

    return rez;
}

void Server::EP::sessClose( int sid, bool delSubscr )
{
    pthread_mutex_lock(&mtxData);
    if(sid > 0 && sid <= (int)mSess.size() && mSess[sid-1].tAccess) {
	mSess[sid-1] = Sess();

	//Disable the assigned subscriptions
	for(unsigned iSs = 0; delSubscr && iSs < mSubScr.size(); ++iSs)
	    if(mSubScr[iSs].st != SS_CLOSED && mSubScr[iSs].sess == sid)
		mSubScr[iSs].setState(SS_CLOSED);
    }
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
    if(sid > 0 && sid <= (int)mSess.size()) {
	if(cp.empty() && mSess[sid-1].cntPnts.find(cpId) != mSess[sid-1].cntPnts.end())
	    mSess[sid-1].cntPnts.erase(cpId);
	else mSess[sid-1].cntPnts[cpId] = cp;
    }
    pthread_mutex_unlock(&mtxData);
}

uint32_t Server::EP::subscrSet( uint32_t ssId, SubScrSt st, bool en, int sess, double publInterval,
    uint32_t lifetimeCnt, uint32_t maxKeepAliveCnt, uint32_t maxNtfPerPubl, int pr )
{
    OPCAlloc mtx(mtxData, true);

    if(ssId == 0 || ssId > mSubScr.size()) {
	uint32_t nSubScrPerSess = 0;
	ssId = mSubScr.size();

	//Find for Subscription on CLOSED state for reusing and subscriptions per session calculation
	for(uint32_t iSs = 0; iSs < mSubScr.size(); iSs++) {
	    if(ssId >= mSubScr.size() && mSubScr[iSs].st == SS_CLOSED) ssId = iSs;
	    if(sess >= 0 && mSubScr[iSs].sess == sess) nSubScrPerSess++;
	}
	if(nSubScrPerSess >= limSubScr())	return 0;
	if(ssId >= mSubScr.size()) { ssId = mSubScr.size(); mSubScr.push_back(Subscr()); }
	mSubScr[ssId].toInit = true;
    }
    else ssId--;

    Subscr &ss = mSubScr[ssId];

    //Set parameters
    ss.publEn = en;
    if(sess >= 0) ss.sess = sess;
    if(publInterval != 0) ss.publInterval = ceil(publInterval/subscrProcPer())*subscrProcPer();
    if(maxKeepAliveCnt != 0) ss.maxKeepAliveCnt = maxKeepAliveCnt;
    if(lifetimeCnt != 0) ss.lifetimeCnt = std::max(ss.maxKeepAliveCnt*3,lifetimeCnt);
    if(maxNtfPerPubl != OpcUa_NPosID) ss.maxNtfPerPubl = maxNtfPerPubl;
    if(pr < 0) ss.pr = pr;
    ss.setState(st);

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
    TimestampsToReturn tmToRet, double smplItv, uint32_t qSz, int8_t dO, uint32_t cH, XML_N *ifltr )
{
    OPCAlloc mtx(mtxData, true);

    if((--ssId) >= mSubScr.size())	return 0;
    else {
	Subscr &ss = mSubScr[ssId];
	if((--mItId) >= ss.mItems.size()) {
	    //Find for MonitItem on DISABLED state for reusing
	    for(mItId = 0; mItId < ss.mItems.size(); mItId++)
		if(ss.mItems[mItId].md == MM_DISABLED) break;
	    if(mItId >= ss.mItems.size()) { mItId = ss.mItems.size(); ss.mItems.push_back(Subscr::MonitItem()); }
	}
	Subscr::MonitItem &mIt = ss.mItems[mItId];
	if(md != MM_CUR) {
	    if(md == MM_DISABLED && md != mIt.md) mIt = Subscr::MonitItem();
	    if(mIt.md == MM_DISABLED && aid == AId_Value) {	//Initiate the publisher by the InitialValue
		mIt.vQueue.push_back(Subscr::MonitItem::Val("",(mIt.dtTm=curTime()),OpcUa_UncertainInitialValue));
		ss.setState(SS_LATE);
	    }
	    mIt.md = md;
	}
	if(!nd.isNull())	mIt.nd = nd;
	if(aid != OpcUa_NPosID)	mIt.aid = aid;
	if(tmToRet != -1)	mIt.tmToRet = tmToRet;
	if(qSz != OpcUa_NPosID)	mIt.qSz = std::max(uint32_t(1), std::min(uint32_t(1000),qSz));	//!!!! Make the upper limit configurable
	if(dO >= 0)		mIt.dO = dO;
	if(cH != OpcUa_NPosID)	mIt.cH = cH;
	if(ifltr)		mIt.fltr = *ifltr;

	//Checkings for data
	XML_N req("data");
	req.setAttr("node", mIt.nd.toAddr())->setAttr("aid", uint2str(mIt.aid))->setAttr("dtPerGet", (smplItv==0)?"1":"0");
	uint32_t rez = reqData(OpcUa_ReadRequest, req);
	if(rez == OpcUa_BadNodeIdUnknown)		mIt.nd = NodeId();
	else if(rez == OpcUa_BadAttributeIdInvalid)	mIt.aid = Aid_Error;
	if(smplItv == 0 || isnan(smplItv)) smplItv = str2real(req.attr("dtPer"))*1000;
	if(smplItv == -1) smplItv = ss.publInterval;
	if(smplItv != -2) mIt.smplItv = ceil(std::max(smplItv,subscrProcPer())/subscrProcPer())*subscrProcPer();
    }

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
    else {
	if(parent.isNull()) { cNx = &objTree; cNx->clear(); }
	else {
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
    switch(reqTp) {
	case OpcUa_BrowseRequest: case OpcUa_BrowseNextRequest: {
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
	    if(lstNd.empty() && rtId.numbVal() == OpcUa_References && (bd == BD_FORWARD || bd == BD_BOTH)) {	//!!!! Check for other call
		map<string, XML_N*>::iterator ndTpDef = ndMap.find(ndX->second->attr("typeDefinition"));
		if(ndTpDef != ndMap.end()) {
		    unsigned cnClass = atoi(ndTpDef->second->attr("NodeClass").c_str());
		    if(!nClass || nClass&cnClass)
			req.childAdd("ref")->setAttr("NodeId", ndTpDef->second->attr("NodeId"))->
			    setAttr("referenceTypeId", ndTpDef->second->attr("referenceTypeId"))->
			    setAttr("dir", "1")->setAttr("name", ndTpDef->second->attr("name"))->
			    setAttr("NodeClass", int2str(cnClass))->setAttr("typeDefinition", ndTpDef->second->attr("typeDefinition"));
		}
	    }

	    //Inverse hierarchical references process (only parent)
	    if(lstNd.empty() && (bd == BD_INVERSE || bd == BD_BOTH) && ndX->second->parent()) {
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
	    for(unsigned iCh = 0; (bd == BD_FORWARD || bd == BD_BOTH) && iCh < ndX->second->childSize(); iCh++) {
		XML_N *chNd = ndX->second->childGet(iCh);
		if(!lstOK) { lstOK = (lstNd==chNd->attr("NodeId")); continue; }
		unsigned cnClass = atoi(chNd->attr("NodeClass").c_str());
		if(nClass && !(nClass&cnClass)) continue;
		req.childAdd("ref")->setAttr("NodeId", chNd->attr("NodeId"))->
		    setAttr("referenceTypeId", chNd->attr("referenceTypeId"))->
		    setAttr("dir", "1")->setAttr("name", chNd->attr("name"))->
		    setAttr("NodeClass", int2str(cnClass))->setAttr("typeDefinition", chNd->attr("typeDefinition"));
		if(rPn && (int)req.childSize() >= rPn && (iCh+1) < ndX->second->childSize()) {
		    req.setAttr("LastNode", chNd->attr("NodeId"));
		    break;
		}
	    }
	    return 0;
	}
	case OpcUa_ReadRequest: {
	    NodeId nid = NodeId::fromAddr(req.attr("node"));
	    map<string, XML_N*>::iterator ndX = ndMap.find(nid.toAddr());
	    if(ndX == ndMap.end()) return OpcUa_BadNodeIdUnknown;
	    uint32_t aid = atoi(req.attr("aid").c_str());
	    switch(aid) {
		// Generic attributes process
		case AId_NodeId: req.setAttr("type", int2str(OpcUa_NodeId))->setText(nid.toAddr());				return 0;
		case AId_NodeClass: req.setAttr("type", int2str(OpcUa_Int32))->setText(ndX->second->attr("NodeClass"));		return 0;
		case AId_BrowseName: req.setAttr("type", int2str(OpcUa_QualifiedName))->setText(ndX->second->attr("name"));	return 0;
		case AId_DisplayName:
		    req.setAttr("type", int2str(OpcUa_LocalizedText))->setText(ndX->second->attr(ndX->second->attr("DisplayName").empty()?"name":"DisplayName"));
		    return 0;
		case AId_Descr: req.setAttr("type", int2str(OpcUa_LocalizedText))->setText(ndX->second->attr("Descr"));		return 0;
		case AId_WriteMask: case AId_UserWriteMask: req.setAttr("type", int2str(OpcUa_UInt32))->setText("0");		return 0;
		case AId_IsAbstract: req.setAttr("type", int2str(OpcUa_Boolean))->setText(ndX->second->attr("IsAbstract"));	return 0;
		case AId_Symmetric: req.setAttr("type", int2str(OpcUa_Boolean))->setText(ndX->second->attr("Symmetric"));	return 0;
		case AId_InverseName: req.setAttr("type", int2str(OpcUa_LocalizedText))->setText(ndX->second->attr("InverseName")); return 0;
		case AId_EventNotifier: req.setAttr("type", int2str(OpcUa_Byte))->setText(ndX->second->attr("EventNotifier"));	return 0;
		default: {
		    string dtType = ndX->second->attr("DataType");
		    if(dtType.empty()) return OpcUa_BadAttributeIdInvalid;
		    // Values' attributes processing
		    switch(aid) {
			case AId_Value:
			    req.setAttr("type", dtType)->setText(req.attr("Value").empty()?ndX->second->attr("Value"):req.attr("Value"))->
				setAttr("Value","");
			    return 0;
			case AId_DataType: req.setAttr("type", int2str(OpcUa_NodeId))->setText(int2str(atoi(dtType.c_str())&(~OpcUa_Array)));	return 0;
			case AId_ValueRank: {
			    string val = ndX->second->attr("ValueRank");
			    req.setAttr("type", int2str(OpcUa_Int32))->setText(val.empty()?"-1":val);
			    return 0;
			}
			case AId_ArrayDimensions: {
			    string val = ndX->second->attr("Value");
			    int cnt = 0;
			    if(atoi(dtType.c_str())&OpcUa_Array)
				for(int off = 0; strParse(val,0,"\n",&off).size(); cnt++) ;
			    req.setAttr("type", int2str(OpcUa_Array|OpcUa_Int32))->setText(int2str(cnt));
			    return 0;
			}
			case AId_AccessLevel: {
			    string val = ndX->second->attr("AccessLevel");
			    req.setAttr("type", int2str(OpcUa_Byte))->setText(val.empty()?int2str(ACS_Read):val);
			    return 0;
			}
			case AId_UserAccessLevel: {
			    string val = ndX->second->attr("UserAccessLevel");
			    req.setAttr("type", int2str(OpcUa_Byte))->setText(val.empty()?int2str(ACS_Read|ACS_Write):val);
			    return 0;
			}
			case AId_MinimumSamplingInterval: {
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
	case OpcUa_WriteRequest: {
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
    for(unsigned iCh = 0; iCh < mChildren.size(); iCh++)
	delete mChildren[iCh];
    mChildren.clear();

    //Copy params (name,text, attributes and instructions)
    mName = prm.mName;
    mText = prm.mText;
    vector<string> ls;
    prm.attrList(ls);
    for(unsigned iA = 0; iA < ls.size(); iA++)
	setAttr(ls[iA],prm.attr(ls[iA]));

    //Recursive copy children
    for(unsigned iCh = 0; iCh < prm.childSize(); iCh++)
	*childAdd() = *prm.childGet(iCh);

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
    for(unsigned iCh = 0; iCh < mChildren.size(); iCh++)
	if(mChildren[iCh] == nd) {
	    delete mChildren[iCh];
	    mChildren.erase(mChildren.begin()+iCh);
	    break;
	}
}

XML_N* XML_N::childClear( const string &name )
{
    for(unsigned iCh = 0; iCh < mChildren.size(); )
	if(name.empty() || mChildren[iCh]->name() == name) childDel(iCh);
	else iCh++;

    return this;
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
    for(int iCh = 0, iN = 0; iCh < (int)childSize(); iCh++)
	if(strcasecmp(childGet(iCh)->name().c_str(),name.c_str()) == 0 && iN++ == numb)
	    return childGet(iCh);

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
    for(unsigned iCh = 0; !rez && iCh < childSize(); iCh++)
	rez = childGet(iCh)->getElementBy(iattr,val);

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

    int iCh = -1;
    for(int i_f = 0; i_f < (int)childSize(); i_f++)
	if(childGet(i_f)->name() == "<*>") {
	    if(iCh < 0) childGet(i_f)->mText = s;
	    else childDel(i_f--);
	    iCh = i_f;
	}
    if(iCh < 0) childAdd("<*>")->mText = s;

    return this;
}

void XML_N::attrList( vector<string> & list ) const
{
    list.clear();
    for(unsigned iA = 0; iA < mAttr.size(); iA++)
	list.push_back(mAttr[iA].first);
}

XML_N* XML_N::attrDel( const string &name )
{
    for(unsigned iA = 0; iA < mAttr.size(); iA++)
	if(mAttr[iA].first == name)
	{ mAttr.erase(mAttr.begin()+iA); break; }

    return this;
}

void XML_N::attrClear( )	{ mAttr.clear(); }

string XML_N::attr( const string &name, bool caseSens ) const
{
    if(caseSens) {
	for(unsigned iA = 0; iA < mAttr.size(); iA++)
	    if(mAttr[iA].first == name) return mAttr[iA].second;
    }
    else for(unsigned iA = 0; iA < mAttr.size(); iA++)
	if(strcasecmp(mAttr[iA].first.c_str(),name.c_str()) == 0) return mAttr[iA].second;

    return "";
}

XML_N* XML_N::setAttr( const string &name, const string &val )
{
    for(unsigned iA = 0; iA < mAttr.size(); iA++)
	if(mAttr[iA].first == name) {
	    mAttr[iA].second = val;
	    return this;
	}

    mAttr.push_back(pair<string,string>(name,val));

    return this;
}

XML_N* XML_N::clear( )
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
    char str[1024];		//!!!!
    va_list argptr;

    va_start(argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);

    cod  = 0;
    mess = str;
}

OPCError::OPCError( int icod, const char *fmt, ... )
{
    char str[1024];		//!!!!
    va_list argptr;

    va_start(argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);

    cod  = icod;
    mess = str;
}

}
