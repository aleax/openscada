
//OpenSCADA MMS(IEC-9506) implementation library file: libMMS.cpp
/******************************************************************************
 *   Copyright (C) 2014 by Roman Savochenko, <rom_as@oscada.org>	      *
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

#include "libMMS.h"

namespace MMS
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

string int2s( int val )
{
    char buf[250];
    snprintf(buf, sizeof(buf), "%d", val);
    return buf;
}

string uint2s( unsigned val )
{
    char buf[250];
    snprintf(buf, sizeof(buf), "%u", val);
    return buf;
}

string ll2s( int64_t val )
{
    char buf[250];
    snprintf(buf, sizeof(buf), "%lld", (long long int)val);
    return buf;
}

string r2s( double val, int prec, char tp )
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

string strMess( const char *fmt, ... )
{
    char str[10000];
    va_list argptr;

    va_start(argptr, fmt);
    vsnprintf(str, sizeof(str), fmt, argptr);
    va_end(argptr);

    return str;
}

void setBS( string &vl, uint32_t n )
{
    if(n/8 >= vl.size()) vl.append((n/8+1)-vl.size(), char(0));
    vl[n/8] |= 0x80>>(n%8);
}

bool tryBS( const string &vl, uint32_t n )
{
    if(n/8 >= vl.size()) return false;
    return vl[n/8]&(0x80>>(n%8));
}

uint16_t i16_LE( uint16_t in )
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return bswap_16(in);
#endif

    return in;
}

uint32_t i32_LE( uint32_t in )
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return bswap_32(in);
#endif

    return in;
}

uint64_t i64_LE( uint64_t in )
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return bswap_64(in);
#endif

    return in;
}

//*************************************************
//* MMS object					  *
//*************************************************
Core::Core( )	{ }

Core::~Core( )	{ }

const char *Core::iVal( const string &rb, int &off, char vSz )
{
    off += vSz;
    if(off > (int)rb.size()) throw Error("Buffer size is lesser requested value.");
    return rb.data()+off-vSz;
}

uint32_t Core::iN( const string &rb, int &off, uint8_t vSz )
{
    vSz = std::max(0, std::min((int)vSz,4));
    if((off+vSz) > (int)rb.size()) throw Error("Buffer size is lesser requested value.");
    union { uint32_t v; char c[4]; } dt;
    dt.v = 0;
    while(vSz) dt.c[--vSz] = rb[off++];

    return i32_LE(dt.v);
}

int Core::ASN_i( const string &rb, int &off, int offLim )
{
    //printf("TEST 10: off=%d\n",off);
    if((off+2) > (int)rb.size())	throw Error("Buffer size is lesser to requested tag.");
    if((uint8_t)rb[off] == 0x9f || (uint8_t)rb[off] == 0xbf) off++;	//Pass big tag
    off++;	//Pass tag
    if(off > (int)rb.size())	throw Error("Buffer size is lesser to requested tag.");
    int sz = (uint8_t)rb[off++];
    if(sz&0x80)
    {
	union { uint32_t v; char c[4]; } dt;
	dt.v = 0;
	int szB = sz & (~0x80);
	if(szB > 4) throw Error("Size too big for numbers.");
	if((off+szB) > (int)rb.size()) throw Error("Buffer size is lesser to requested value.");
	while(szB) dt.c[--szB] = rb[off++];
	sz = i32_LE(dt.v);
    }

    if((off+sz) > (int)rb.size())	throw Error("Buffer size is lesser to requested value.");
    if(offLim >= 0 && off > offLim)	throw Error("Variable excel container size.");

    return sz;
}

uint16_t Core::ASN_iTAG( const string &rb, int off )
{
    uint16_t rez = 0;
    if((off+2) > (int)rb.size())	throw Error("Buffer size is lesser to requested tag.");
    if(uint8_t(rb[off]) == 0x9f || uint8_t(rb[off]) == 0xbf) rez = uint8_t(rb[off++])<<8;
    rez |= uint8_t(rb[off++]);

    return i16_LE(rez);
}

uint32_t Core::ASN_iN( const string &rb, int &off, int sz )
{
    if(sz < 0) sz = ASN_i(rb, off);
    if(sz > 4) throw Error("Size too big for integer.");
    union { uint32_t v; char c[4]; } dt;
    dt.v = 0;
    while(sz) dt.c[--sz] = rb[off++];

    return i32_LE(dt.v);
}

double Core::ASN_iR( const string &rb, int &off, int sz )
{
    if(sz < 0) sz = ASN_i(rb, off);
    if(sz < 5) throw Error("Size too little for real.");
    char exp = rb[off++]; sz--;
    if(exp == 8 && sz == 4)
    {
	union { float v; char c[4]; uint32_t vi; } dt;
	dt.v = 0;
	while(sz) dt.c[--sz] = rb[off++];
	dt.vi = i32_LE(dt.vi);
	return dt.v;
    }
    else if(exp == 11 && sz == 8)
    {
	union { double v; char c[8]; uint64_t vi; } dt;
	dt.v = 0;
	while(sz) dt.c[--sz] = rb[off++];
	dt.vi = i64_LE(dt.vi);
	return dt.v;
    }

    throw Error("Discrepancy exponent to size.");
}

string Core::ASN_iS( const string &rb, int &off, int sz )
{
    if(sz < 0) sz = ASN_i(rb, off);
    off += sz;
    return rb.substr(off-sz,sz);
}

string Core::ASN_iBS( const string &rb, int &off, int sz, char *unUsBits )
{
    if(sz < 0) sz = ASN_i(rb, off);
    if(unUsBits) *unUsBits = rb[off];
    off += sz;

    return rb.substr(off-(sz-1),sz-1);
}

void Core::ASN_iAccessResult( const string &buf, int &off, int sz, XML_N &io )
{
    const char *errS;
    for(int offC = off, i_it = 0, err, vTp; (offC+sz) != off; i_it++)
    {
	int offC1 = off, szC1 = ASN_i(buf, off, offC+sz);
	while(i_it >= (int)io.childSize()) io.childAdd("itC");
	XML_N *chN = io.childGet(i_it);
	//printf("TEST 30: AccsIt %x(%x)h: %d\n", off, offC1, i_it);
	switch((vTp=ASN_iTAG(buf,offC1)&~(0x20)))
	{
	    case VT_Error:
		switch((err=ASN_iN(buf,off,szC1)))
		{
		    case 0: errS = "object-invalidated";	break;
		    case 1: errS = "hardware-fault";		break;
		    case 2: errS = "temporarily-unavailable";	break;
		    case 3: errS = "object-access-denied";	break;
		    case 4: errS = "object-undefined";		break;
		    case 5: errS = "invalid-address";		break;
		    case 6: errS = "type-unsupported";		break;
		    case 7: errS = "type-inconsistent";		break;
		    case 8: errS = "object-attribute-inconsistent";	break;
		    case 9: errS = "object-access-unsupported";	break;
		    case 10: errS = "object-non-existent";	break;
		    default: errS = "unknown";			break;
		}
		chN->setAttr("err", int2s(err)+":"+errS);
		break;
	    case VT_Array: case VT_Struct:
		chN->setAttr("tp",int2s(vTp));
		//printf("TEST 32: EnterStruct %xh: %d(%xh): %s\n", off, i_it, vTp, chN->text().c_str());
		ASN_iAccessResult(buf, off, szC1, *chN);
		break;
	    case VT_Bool: chN->setAttr("tp",int2s(vTp))->setText(int2s(ASN_iN(buf,off,szC1)));	break;
	    case VT_BitString:	//!!!! Need for test
	    {
		char unUsBits = 0;
		string vl, value = ASN_iBS(buf, off, szC1, &unUsBits);
		for(int i_vl = 0; i_vl < std::max(0,int(value.size()*8-std::max(0,int(unUsBits)))); i_vl++)
		    vl += ((value[i_vl/8]>>(i_vl%8))&1) ? "1" : "0";
		chN->setAttr("tp",int2s(vTp))->setText(vl);
		break;
	    }
	    case VT_OctString:	//!!!! Need for test
	    {
		string vl, value = ASN_iS(buf, off, szC1);
		for(unsigned i_vl = 0; i_vl < value.size(); i_vl++)
		    vl += strMess("%0.2x",value[i_vl]);
		chN->setAttr("tp",int2s(vTp))->setText(vl);
		break;
	    }
	    case VT_VisString:
		chN->setAttr("tp",int2s(vTp))->setText(ASN_iS(buf,off,szC1));			break;
	    case VT_Int: chN->setAttr("tp",int2s(vTp))->setText(int2s((int)ASN_iN(buf,off,szC1)));	break;
	    case VT_UInt: chN->setAttr("tp",int2s(vTp))->setText(uint2s(ASN_iN(buf,off,szC1)));	break;
	    case VT_Float: chN->setAttr("tp",int2s(vTp))->setText(r2s(ASN_iR(buf,off,szC1)));	break;
	    default: off += szC1;
	}
	//printf("TEST 31: AccsIt %xh: %d(%xh): %s\n", offC1, i_it, vTp, chN->text().c_str());
    }
}

void Core::oN( string &buf, uint32_t val, uint8_t sz, int off )
{
    union { uint32_t v; char c[4]; } dt;
    dt.v = i32_LE(val);
    if(sz < 0 || sz > 4) for(sz = 4; sz > 1 && dt.c[sz-1]; ) sz--;
    off = (off >= 0) ? std::min(off,(int)buf.size()) : buf.size();
    if((off+sz) > (int)buf.size()) buf.append(off+sz-buf.size(), char(0));
    while(sz) buf[off++] = dt.c[--sz];
}

void Core::ASN_o( string &buf, uint16_t tag, uint32_t sz )
{
    //Size extend
    int extB = 4;
    if(sz < 128) extB = 0;
    else for(sz = i16_LE(sz); sz >> (extB*8); ) extB++;
    //Tag
    bool bigTag = (tag >= 256);
    tag = i16_LE(tag);
    if(bigTag) buf += char((tag>>8)&0xFF);
    buf += char(tag&0xFF);
    //Size
    if(!extB) buf += char(sz);
    else
    {
	buf += char(0x80|extB);
	for(int i_ex = 0; i_ex < extB; i_ex++) buf += char((sz>>(extB-i_ex)*8)&0xFF);
    }
}

int Core::ASN_oC( string &buf, uint16_t tag, int off )
{
    //Position
    int elOff = off = (off >= 0) ? std::min(off,(int)buf.size()) : buf.size();
    int sz = buf.size() - off;
    //Size extend
    int extB = 4;
    if(sz < 128) extB = 0;
    else
    {
	union { uint32_t v; char c[4]; } dt; dt.v = i32_LE(sz);
	while(!dt.c[extB-1]) extB--;
    }
    //Tag
    bool bigTag = (tag >= 256);
    buf.insert(elOff, ((bigTag?3:2)+extB), (char)0);
    tag = i16_LE(tag);
    if(bigTag) buf[off++] = char((tag>>8)&0xFF);
    buf[off++] = char(tag&0xFF);
    //Size
    if(!extB) buf[off++] = sz;
    else
    {
	buf[off++] = 0x80|extB;
	union { uint32_t v; char c[4]; } dt; dt.v = i32_LE(sz);
	while(extB) buf[off++] = dt.c[--extB];
    }

    return elOff;
}

void Core::ASN_oN( string &buf, uint8_t tag, uint32_t val, uint8_t szMin )
{
    union { uint32_t v; char c[4]; } dt;
    dt.v = i32_LE(val);
    int sz = std::min(4, (int)szMin);
    while(val >> (sz*8)) sz++;
    ASN_o(buf, tag, sz);
    while(sz) buf += dt.c[--sz];
}

void Core::ASN_oR( string &buf, uint8_t tag, double val, char exp )
{
    int sz = 0;
    if(exp == 8)
    {
	sz = 4;
	ASN_o(buf, tag, sz+1);
	buf += exp;
	union { float v; char c[4]; uint32_t vi; } dt;
	dt.v = val;
	dt.vi = i32_LE(dt.vi);
	while(sz) buf += dt.c[--sz];
    }
    else if(exp == 11)	//!!!!
    {
	sz = 8;
	ASN_o(buf, tag, sz+1);
	buf += exp;
	union { double v; char c[8]; uint64_t vi; } dt;
	dt.v = val;
	dt.vi = i64_LE(dt.vi);
	while(sz) buf += dt.c[--sz];
    }
    else throw Error("Unsupported exponent size.");
}

void Core::ASN_oS( string &buf, uint8_t tag, const string &vl )
{
    ASN_o(buf, tag, vl.size());
    buf += vl;
}

void Core::ASN_oBS( string &buf, uint8_t tag, const string &vl, char unUsBits )
{
    ASN_o(buf, tag, vl.size()+1);
    buf += char(unUsBits);
    buf += vl;
}

void Core::ASN_oNmObj( string &buf, uint8_t tag, const string &vl, const string &domain )
{
    int off = buf.size();
    if(!domain.size())	ASN_oS(buf, 0x80, vl);	// vmd-specific
    else					// domain-specific
    {
	ASN_oS(buf, 0x1A, domain);		//  domainId
	ASN_oS(buf, 0x1A, vl);			//  itemId
	ASN_oC(buf, 0xA1, off);			// domain-specific
    }
    //ASN_oS(rez, 0x82, vl);			// aa-specific

    ASN_oC(buf, tag, off);			//ObjectName
}

void Core::ASN_iTypeSpec( const string &buf, int &off, int sz, XML_N &io )
{
    for(int offC = off, i_it = 0, vTp; (offC+sz) != off; i_it++)
    {
	int offC1 = off, szC1 = ASN_i(buf, off, offC+sz);
	switch((vTp=ASN_iTAG(buf,offC1)))
	{
	    case 0x80: io.setAttr("typeName", ASN_iS(buf,off,szC1));	break;
	    case VT_Array:
		off += szC1;
		io.setAttr("tp", int2s(vTp));
		for(offC1 = off; (offC1+szC1) != off; )
		{
		    int offC2 = off, szC2 = ASN_i(buf, off, offC1+szC1);
		    switch(ASN_iTAG(buf,offC2))
		    {
			case 0x80: io.setAttr("packed",int2s(ASN_iN(buf,off,szC2)));	break;	//  packed
			case 0xA1:
			    for(offC2 = off; (offC2+szC2) != off; )
			    {
				int offC3 = off, szC3 = ASN_i(buf, off, offC2+szC2);
				if(ASN_iTAG(buf,offC3) == 0xA1)
				    for(offC3 = off; (offC3+szC3) != off; )
				    {
					int offC4 = off, szC4 = ASN_i(buf, off, offC3+szC3);
					if(ASN_iTAG(buf,offC4) == 0x81) io.setAttr("numberOfElements",int2s(ASN_iN(buf,off,szC4)));
					else off += szC4;
				    }
				else off += szC3;
			    }
			    break;
			case 0xA2: ASN_iTypeSpec(buf, off, szC2, io);			break;	//  elementType
			default: off += szC2;	break;
		    }
		}
		break;
	    case VT_Struct:
		io.setAttr("tp",int2s(vTp));
		for(offC1 = off; (offC1+szC1) != off; )
		{
		    int offC2 = off, szC2 = ASN_i(buf, off, offC1+szC1);
		    switch(ASN_iTAG(buf,offC2))
		    {
			case 0x80: io.setAttr("packed",int2s(ASN_iN(buf,off,szC2)));	break;	//  packed
			case 0xA1:								//  components
			    for(offC2 = off; (offC2+szC2) != off; )
			    {
				int offC3 = off, i_it = 0, szC3 = ASN_i(buf, off, offC2+szC2);
				if(ASN_iTAG(buf,offC3) == 0x30)					//   item
				    for(offC3 = off; (offC3+szC3) != off; i_it++)
				    {
					while(i_it < (int)io.childSize()) io.childAdd("stIt");
					XML_N *chN = io.childGet(i_it);

					int szC4 = ASN_i(buf, off, offC3+szC3);
					switch(ASN_iTAG(buf,offC3))
					{
					    case 0x80: chN->setAttr("id",ASN_iS(buf,off,szC4));	break;	//componentName
					    case 0xA1: ASN_iTypeSpec(buf, off, szC4, *chN);	break;	//componentType
					    default: off += szC4;				break;
					}
				    }
				else off += szC3;
			    }
			    break;
			default: off += szC2;	break;
		    }
		}
		break;
	    case VT_UInt: io.setAttr("tp", int2s(vTp))->setAttr("sz", uint2s(ASN_iN(buf,off,szC1)));	break;
	    case VT_Int: case VT_VisString: case VT_OctString: case VT_BitString:
		io.setAttr("tp", int2s(vTp))->setAttr("sz", int2s((int)ASN_iN(buf,off,szC1)));	break;
	    case VT_Bool: case VT_Float:
		io.setAttr("tp",int2s(vTp));
		off += szC1;
		break;
	    default: io.setAttr("tp",int2s(VT_VisString)); off += szC1;	break;
	}
    }
}

//*************************************************
//* Protocol MMS client part			  *
//*************************************************
Client::Client( ) : isInitiated(false), mInvokeID(1),
    m_callParameterCBB("\xFB\00",2), m_callServicesSupported("\xEE\x00\x18\x3F\x0F\xF6\x10\x03\x01\xF8\x90",11)
{
    //Reject errors
    const char *nms1[] = { "RejectConfirmedReq", "other", "unrecognized-service", "unrecognized-modifier", "invalid-invokeID",
			    "invalid-argument", "invalid-modifier", "max-serv-outstanding-exceeded", "", "max-recursion-exceeded",
			    "value-out-of-range" };
    RejectErrs[0x80] = vector<string>(nms1, nms1+11);

    const char *nms2[] = { "RejectConfirmedResp", "other", "unrecognized-service", "invalid-invokeID", "invalid-result", "", "max-recursion-exceeded",
			    "value-out-of-range" };
    RejectErrs[0x81] = vector<string>(nms2, nms2+8);

    const char *nms3[] = { "RejectConfirmedError", "other", "unrecognized-service", "invalid-invokeID", "invalid-serviceError", "value-out-of-range" };
    RejectErrs[0x82] = vector<string>(nms3, nms3+6);

    const char *nms4[] = { "RejectUnconfirmed", "other", "unrecognized-service", "invalid-argument", "max-recursion-exceeded", "value-out-of-range" };
    RejectErrs[0x83] = vector<string>(nms4, nms4+6);

    //RejectPduError, RejectCancelRequest, RejectCancelResponse, RejectCancelError, RejectConcludeRequest,RejectConcludeResponse, RejectConcludeError;

    //Confirm errors
    const char *nmsC1[] = { "vmd-state", "other", "vmd-state-conflict", "vmd-operational-problem", "domain-transfer-problem", "state-machine-id-invalid" };
    ConfirmErrs[0x80] = vector<string>(nmsC1, nmsC1+6);

    const char *nmsC2[] = { "application-reference", "other", "aplication-unreachable", "connection-lost", "application-reference-invalid",
			    "context-unsupported" };
    ConfirmErrs[0x81] = vector<string>(nmsC2, nmsC2+6);

    const char *nmsC3[] = { "definition", "other", "object-undefined", "invalid-address", "type-unsupported", "type-inconsistent", "object-exists",
			    "object-attribute-inconsistent" };
    ConfirmErrs[0x82] = vector<string>(nmsC3, nmsC3+8);

    const char *nmsC4[] = { "resource", "other", "memory-unavailable", "processor-resource-unavailable", "mass-storage-unavailable", 
			    "capability-unavailable", "capability-unknown" };
    ConfirmErrs[0x83] = vector<string>(nmsC4, nmsC4+7);

    const char *nmsC5[] = { "service", "other", "primitives-out-of-sequence", "object-sate-conflict", "pdu-size", "continuation-invalid",
			    "object-constraint-conflict" };
    ConfirmErrs[0x84] = vector<string>(nmsC5, nmsC5+7);

    const char *nmsC6[] = { "service-preempt", "other", "timeout", "deadlock", "cancel" };
    ConfirmErrs[0x85] = vector<string>(nmsC6, nmsC6+5);

    const char *nmsC7[] = { "time-resolution", "other", "unsupportable-time-resolution" };
    ConfirmErrs[0x86] = vector<string>(nmsC7, nmsC7+3);
}

Client::~Client( )	{ }

void Client::setCallParameterCBB( const string &val )
{
    m_callParameterCBB = val;
    m_callParameterCBB.resize(2, 0);
}

void Client::setCallServicesSupported( const string &val )
{
    m_callServicesSupported = val;
    m_callServicesSupported.resize(11, 0);
}

void Client::reset( )
{
    isInitiated = false;
    mInvokeID = 1;
    id_vName = id_modelName = id_rev = "";
}

void Client::protIO( XML_N &io )
{
    string tpkt, rez, rez1, err;
    int sCd = 0;
    char buf[1000];

    try
    {
	//bool debug = (bool)atoi(io.attr("debug").c_str());
	if(io.name() != "MMS")	throw Error("Unknown target protocol '%s'.", io.name().c_str());

						//>TPKT (RFC-1006)
	oN(tpkt, MMS_TPKT_Version, 1);		// Version
	oN(tpkt, 0, 1);				// Reserved
	int tpktLenOff = tpkt.size();
	oN(tpkt, 0, 2);				// Length

	//Service's requests prepare
	if(io.attr("id") == "connect")
	{
						//>COTP ConnRequest (ISO-8073)
	    oN(rez, 0, 1);			// Length
	    oN(rez, COTP_CR, 1);		// PDU Type
	    oN(rez, 0, 2);			// Dest reference
	    oN(rez, 0, 2);			// Source reference
	    oN(rez, 0, 1);			// Class
						// Options
	    if(COTP_SzTPDU() > 0)		//  TPDU size
	    {
		oN(rez, COTP_O_SzTPDU, 1);	//   Code
		oN(rez, 1, 1);			//   Length
		oN(rez, COTP_SzTPDU(), 1);	//   Value
	    }
						//  Src TSAP
	    oN(rez, COTP_O_SrcTSAP, 1);		//   Code
	    oN(rez, 2, 1);			//   Length
	    oN(rez, 0, 2);			//   Value
						//  Dest TSAP
	    oN(rez, COTP_O_DstTSAP, 1);		//   Code
	    oN(rez, 2, 1);			//   Length
	    oN(rez, COTP_DestTSAP(), 2);	//   Value

	    oN(rez, rez.size()-1, 1, 0);	//>COTP real length
	    tpkt += rez;
	    //if(debug) debugMess(strMess("ConnRequest Req(%d)",rez.size());
	}
	else
	{
	    if(io.attr("id") == "initiate")			sCd = MMS_InitiateReq;
	    else if(io.attr("id") == "identify")		sCd = Conf_Identify;
	    else if(io.attr("id") == "getNameList")		sCd = Conf_getNameList;
	    else if(io.attr("id") == "read")			sCd = Conf_Read;
	    else if(io.attr("id") == "write")			sCd = Conf_Write;
	    else if(io.attr("id") == "getVariableAccessAttributes")	sCd = Conf_getVariableAccessAttributes;
	    else if(io.attr("id") == "getCapabilityList")	sCd = Conf_getCapabilityList;

	    if(sCd)
	    {
												//>COTP ConnRequest (ISO-8073)
		oN(rez, 0, 1);									// Length
		oN(rez, COTP_DT, 1);								// PDU Type
		oN(rez, COTP_LastDtUnit, 1);							// Class
		oN(rez, rez.size()-1, 1, 0);							//>COTP real length
		tpkt += rez;
	    }
	    rez = "";										//>MMS
	    switch(sCd)
	    {
		case MMS_InitiateReq:
		{
												//InitiateReq
		    ASN_oN(rez, 0x80, 8187);							// localDetailCalling

		    ASN_oN(rez, 0x81, 3);							// proposedMaxServOutstandingCalling
		    ASN_oN(rez, 0x82, 3);							// proposedMaxServOutstandingCalled
		    ASN_oN(rez, 0x83, 127);							// proposedDataStructureNestingLevel
		    int c1off = rez.size();							// mmsInitRequestDetail
		    ASN_oN(rez, 0x80, 1);							//  proposedVersionNumber
		    ASN_oBS(rez, 0x81, callParameterCBB(), 5);					//  proposedParameterCBB
		    ASN_oBS(rez, 0x82, callServicesSupported(), 3);				//  servicesSupportedCalling
		    ASN_oC(rez, 0xA4, c1off);							// mmsInitRequestDetail
		    ASN_oC(rez, MMS_InitiateReq, 0);						//InitiateReq
		    break;
		}
												//ConfirmedRequest
		case Conf_Identify:
		case Conf_getCapabilityList:
		case Conf_getNameList:
		case Conf_Read:
		case Conf_Write:
		case Conf_getVariableAccessAttributes:
		    ASN_oN(rez, 0x02, mInvokeID++);						// invokeID
		    switch(sCd)
		    {
			case Conf_Identify: ASN_o(rez, Conf_Identify); break;			// Identify
			case Conf_getNameList:							// getNameList
			{
			    int offTmp = rez.size();						//  extendedObjectClass
			    ASN_oN(rez, 0x80, atoi(io.attr("objectClass").c_str()));		//   objectClass
			    ASN_oC(rez, 0xA0, offTmp);						//  extendedObjectClass
			    int offTmp1 = rez.size();						//  objectScope
			    if(io.attr("domainSpecific").size()) ASN_oS(rez, 0x81, io.attr("domainSpecific"));	//domainSpecific
			    else ASN_oN(rez, 0x80, 0, 0);					//   NULL
			    ASN_oC(rez, 0xA1, offTmp1);						//  objectScope
			    if(io.attr("continueAfter").size()) ASN_oS(rez, 0x82, io.attr("continueAfter"));	//  continueAfter
			    ASN_oC(rez, Conf_getNameList, offTmp);				// getNameList
			    break;
			}
			case Conf_Read:
			{
			    int offTmp = rez.size();						// ReadService
			    if(atoi(io.attr("withResult").c_str())) ASN_oN(rez, 0x80, 1);	//  specificationWithResult
												//  variableSpecification
			    int offTmp1 = rez.size();						//   listOfVariable
			    for(unsigned i_it = 0; i_it < io.childSize(); i_it++)
			    {
				XML_N *itN = io.childGet(i_it)->setAttr("err","");
				int offTmp2 = rez.size();					//    VariableSpecification
				ASN_oNmObj(rez, 0xA0, itN->attr("itemId"), itN->attr("domainId"));//     name
				//ASN_oAddr(rez, 0xA1, offTmp);					//     address
												//     variableDescription
												//     scatteredAccessDescription
												//     invalidated
				ASN_oC(rez, 0x30, offTmp2);					//    VariableSpecification
			    }
			    ASN_oC(rez, 0xA0, offTmp1);						//   listOfVariable
			    //ASN_oNmO(rez, 0xA1);						//   variableListName (ObjectName)
			    ASN_oC(rez, 0xA1, offTmp1);						//  variableSpecification
			    ASN_oC(rez, sCd, offTmp);						// ReadWriteService
			    break;
			}
			case Conf_Write:
			{
			    int offTmp = rez.size();						// WriteService
												//  listOfVariable
			    for(unsigned i_it = 0; i_it < io.childSize(); i_it++)
			    {
				XML_N *itN = io.childGet(i_it)->setAttr("err","");
				int offTmp1 = rez.size();					//   VariableSpecification
				ASN_oNmObj(rez, 0xA0, itN->attr("itemId"), itN->attr("domainId"));//     name
				//ASN_oAddr(rez, 0xA1, offTmp);					//    address
												//    variableDescription
												//    scatteredAccessDescription
												//    invalidated
				ASN_oC(rez, 0x30, offTmp1);					//   VariableSpecification
			    }
			    ASN_oC(rez, 0xA0, offTmp);						//  listOfVariable
			    int offTmp1 = rez.size();						//  listOfData
			    for(unsigned i_it = 0, dTp = 0; i_it < io.childSize(); i_it++)
			    {
				XML_N *itN = io.childGet(i_it);
				switch((dTp=atoi(itN->attr("dataType").c_str())))
				{
				    case VT_Bool: ASN_oN(rez, dTp, (bool)atoi(itN->text().c_str()));	break;
				    case VT_Int: ASN_oN(rez, dTp, atoi(itN->text().c_str()));		break;
				    case VT_UInt: ASN_oN(rez, dTp, (unsigned)atoi(itN->text().c_str()));break;
				    case VT_Float: ASN_oR(rez, dTp, atof(itN->text().c_str()));		break;
				    case VT_BitString:	//!!!! Need for test
				    {
					string srcS = itN->text(), rezBS;
					for(unsigned i_s = 0; i_s < srcS.size(); i_s++)
					    if(srcS[i_s] == '1') setBS(rezBS, i_s);
					    else if(srcS[i_s] != '0') break;
					ASN_oBS(rez, dTp, rezBS, atoi(itN->attr("unUsBits").c_str()));
					break;
				    }
				    case VT_OctString:	//!!!! Need for test
				    {
					string srcS = itN->text(), rezBS;
					for(unsigned i_s = 0; i_s < srcS.size(); i_s += 2)
					    rezBS += (char)strtol(srcS.substr(i_s,2).c_str(),NULL,16);
					ASN_oS(rez, dTp, rezBS);
					break;
				    }
				    case VT_VisString: ASN_oS(rez, dTp, itN->text());			break;
				    case VT_Array: case VT_Struct:	//!!!! Test implement
				    {
					for(unsigned i_c = 0, dTp1; i_c < itN->childSize(); i_c++)
					{
					    XML_N *icN = itN->childGet(i_c);
					    switch((dTp1=atoi(icN->attr("dataType").c_str())))
					    {
						case VT_Bool: ASN_oN(rez, dTp1, (bool)atoi(icN->text().c_str()));	break;
						case VT_Int: ASN_oN(rez, dTp1, atoi(icN->text().c_str()));		break;
						case VT_UInt: ASN_oN(rez, dTp1, (unsigned)atoi(icN->text().c_str()));break;
						case VT_Float: ASN_oR(rez, dTp1, atof(icN->text().c_str()));		break;
					    }
					}
					ASN_oC(rez, dTp, offTmp1);
					break;
				    }
				    default: ASN_oS(rez, VT_VisString, itN->text());	break;
				}
			    }
			    ASN_oC(rez, 0xA0, offTmp1);						//  listOfData
			    ASN_oC(rez, sCd, offTmp);						// WriteService
			    break;
			}
			case Conf_getVariableAccessAttributes:
			{
			    int offTmp = rez.size();
			    ASN_oNmObj(rez, 0xA0, io.attr("itemId"), io.attr("domainId"));	// name
			    //ASN_oAddr(rez, 0xA1, offTmp);					// address
			    // ASN_oN(rez, 0xA0, );						//  numericAddress
			    // ASN_oS(rez, 0xA1, );						//  symbolicAddress
			    // ASN_oS(rez, 0xA2, );						//  unconstrainedAddress
			    ASN_oC(rez, sCd, offTmp);						// ItemsService
			    break;
			}
			case Conf_getCapabilityList: ASN_o(rez, Conf_getCapabilityList); break;	// getCapabilityList
		    }
		    ASN_oC(rez, MMS_ConfirmedReq, 0);						//ConfirmedRequest
		    break;
	    }
	    tpkt += rez;
	}

	if(!rez.size())	throw Error("Unknown service '%s'.", io.attr("id").c_str());

	oN(tpkt, tpkt.size(), 2, tpktLenOff);		//>TPTK real length

	//Send request and wait respond
	int resp_len = messIO(tpkt.data(), tpkt.size(), buf, sizeof(buf));
	tpkt.assign(buf, resp_len);
	int off = 2;
	for( ; tpkt.size() < 4 || tpkt.size() < iN(tpkt,off,2); off = 2)
	{
	    resp_len = messIO(NULL, 0, buf, sizeof(buf));
	    if(!resp_len) throw Error("Not full respond.");
	    rez.append(buf, resp_len);
	}

	off = 2;
	if(tpkt.size() < 4 || iN(tpkt,off,2) != tpkt.size())	//<TPKT length
	    throw Error("Respond size is not coincidence.");

								//<COTP result
	uint8_t lenCOTP = iN(tpkt, off, 1);			// Length
	uint8_t begCOTP = off;
	if(lenCOTP < 2 || tpkt.size() < (begCOTP+lenCOTP))	throw Error("Respond's COTP short.");
	uint8_t tpPDU = iN(tpkt, off, 1);			// PDU Type

	//Service's response process
	if(io.attr("id") == "connect")
	{
	    if(tpPDU != COTP_CC)	throw Error("ConnResponse error.");
	    iN(tpkt, off, 2);					// Dest reference
	    iN(tpkt, off, 2);					// Source reference
	    iN(tpkt, off, 1);					// Class
	    while((off-begCOTP) < lenCOTP)			// Options
	    {
		iN(tpkt, off, 1);				//  Code
		uint8_t oLen = iN(tpkt, off, 1);		//  Length
		iVal(tpkt, off, oLen);
	    }
	    if((off-begCOTP) != lenCOTP)	throw Error("Malformed COTP.");
	}
	else if(sCd)
	{
	    if(tpPDU != COTP_DT)	throw Error("Response error.");
	    iN(tpkt, off, 1);					// Class

								//<MMS
	    switch(sCd)
	    {
		case MMS_InitiateReq:
		{
		    int offC = off, szC = ASN_i(tpkt, off);		// initiate_ResponsePDU
		    if(ASN_iTAG(tpkt,offC) != MMS_initiateResp)	throw Error("Initiate error.");
		    for(offC = off; (offC+szC) != off; )
		    {
			int offC1 = off, szC1 = ASN_i(tpkt, off, offC+szC);
			switch(ASN_iTAG(tpkt,offC1))
			{
			    case 0x80:	ASN_iN(tpkt, off, szC1);	break;	//  localDetailCalled
			    case 0x81:	ASN_iN(tpkt, off, szC1);	break;	//  negociatedMaxServOutstandingCalling
			    case 0x82:	ASN_iN(tpkt, off, szC1);	break;	//  negociatedMaxServOutstandingCalled
			    case 0x83:	ASN_iN(tpkt, off, szC1);	break;	//  negociatedDataStructureNestingLevel
			    case 0xA4:						//  mmsInitResponseDetail
				for(offC1 = off; (offC1+szC1) != off; )
				{
				    int offC2 = off, szC2 = ASN_i(tpkt, off, offC1+szC1);
				    switch(ASN_iTAG(tpkt,offC2))
				    {
					case 0x80: ASN_iN(tpkt, off, szC2); break;				//negociatedVersionNumber
					case 0x81: m_parameterCBB = ASN_iBS(tpkt, off, szC2); break;		//negociatedParameterCBB
					case 0x82: m_servicesSupported = ASN_iBS(tpkt, off, szC2); break;	//servicesSupportedCalled
					default: off += szC2;	break;
				    }
				}
				break;
			    default: off += szC1;	break;
			}
		    }
		    break;
		}
		case Conf_Identify:
		case Conf_getCapabilityList:
		case Conf_getNameList:
		case Conf_Read:
		case Conf_Write:
		case Conf_getVariableAccessAttributes:
		{
		    int offC = off, szC = ASN_i(tpkt, off);		// ConfirmedRespPDU
		    if(ASN_iTAG(tpkt,offC) != MMS_ConfirmedResp)
		    {
			string chErr;
			switch(ASN_iTAG(tpkt,offC))
			{
			    case MMS_Reject:
				for(offC = off; (offC+szC) != off; )
				{
				    int offC1 = off, szC1 = ASN_i(tpkt, off, offC+szC), errTp, errCode;
				    map<uint8_t, vector<string> >::iterator iErr;
				    if((errTp=ASN_iTAG(tpkt,offC1)) == 0x80) chErr += strMess("Invoke: %d. ",ASN_iN(tpkt,off,szC1));
				    else if((iErr=RejectErrs.find(errTp)) != RejectErrs.end())
					chErr += iErr->second[errCode] + " :" +
					    (((errCode=ASN_iN(tpkt,off,szC1))+1 >= (int)iErr->second.size()) ? "unknown" : iErr->second[errCode+1]) +
					    ". ";
				    else off += szC1;
				}
				break;
			    case MMS_ConfirmedErr:
				for(offC = off; (offC+szC) != off; )
				{
				    int offC1 = off, szC1 = ASN_i(tpkt, off, offC+szC);
				    switch(ASN_iTAG(tpkt,offC1))
				    {
					case 0x80: chErr += strMess("Invoke: %d. ",ASN_iN(tpkt,off,szC1));	break;
					case 0xA2:	//serviceError
					{
					    chErr += "Service error: ";
					    for(offC1 = off; (offC1+szC1) != off; )
					    {
						int offC2 = off, szC2 = ASN_i(tpkt, off, offC1+szC1);
						if(ASN_iTAG(tpkt,offC2) == 0xA0)		//errorClass
						    for(offC2 = off; (offC2+szC2) != off; )
						    {
							int offC3 = off, szC3 = ASN_i(tpkt, off, offC2+szC2);
							map<uint8_t, vector<string> >::iterator iErr;
							int errTp = ASN_iTAG(tpkt, offC3), errCode;
							if((iErr=ConfirmErrs.find(errTp)) != ConfirmErrs.end())
							    chErr += iErr->second[errCode] + " :" +
								(((errCode=ASN_iN(tpkt,off,szC3))+1 >= (int)iErr->second.size()) ? "unknown" : iErr->second[errCode+1]) +
								". ";
							else { chErr += "unknown"; off += szC3; }
						    }
						else off += szC2;	break;
					    }
					    break;
					}
					default: off += szC1;	break;
				    }
				}
				break;
			    default:
				chErr += "Unknown";
				off += szC;
				break;
			}
			throw Error("ConfirmedRequest error: %s", chErr.c_str());
		    }
		    for(offC = off; (offC+szC) != off; )
		    {
			int offC1 = off, szC1 = ASN_i(tpkt, off, offC+szC);
			switch(ASN_iTAG(tpkt,offC1))
			{
			    case 0x02:					// invokeID
			    {
				uint32_t invokeID = ASN_iN(tpkt, off, szC1);
				if(invokeID != (mInvokeID-1)) throw Error("Inconsistent invokeID.");
				break;
			    }
			    case Conf_IdentifyResp:			// Identify
				for(offC1 = off; (offC1+szC1) != off; )
				{
				    int offC2 = off, szC2 = ASN_i(tpkt, off, offC1+szC1);
				    switch(ASN_iTAG(tpkt,offC2))
				    {
					case 0x80: io.setAttr("vendorName", ASN_iS(tpkt,off,szC2)); break;	//  vendorName
					case 0x81: io.setAttr("modelName", ASN_iS(tpkt,off,szC2)); break;	//  modelName
					case 0x82: io.setAttr("revision", ASN_iS(tpkt,off,szC2)); break;	//  revision
					default: off += szC2;	break;
				    }
				}
				break;
			    case Conf_getNameList:			// getNameList
			    case Conf_getCapabilityList:		// getCapabilityList
			    {
				bool moreFollows = true;
				for(offC1 = off; (offC1+szC1) != off; )
				{
				    int offC2 = off, szC2 = ASN_i(tpkt, off, offC1+szC1);
				    switch(ASN_iTAG(tpkt,offC2))
				    {
					case 0xA0:			//  listOfCapabilities
					    for(offC2 = off; (offC2+szC2) != off; )
					    {
						int offC3 = off, szC3 = ASN_i(tpkt, off, offC2+szC2);
						if(ASN_iTAG(tpkt,offC3) == 0x1a) io.childAdd("it")->setText(ASN_iS(tpkt,off,szC3));
						else off += szC3;
					    }
					    break;
					case 0x81: moreFollows = (bool)ASN_iN(tpkt,off,szC2); break;	//  moreFollows
					default: off += szC2;	break;
				    }
				}
				if(io.childSize()) io.setAttr("moreFollows", int2s(moreFollows));
				break;
			    }
			    case Conf_Read:
			    case Conf_Write:
				for(offC1 = off; (offC1+szC1) != off; )
				{
				    int offC2 = off, szC2 = ASN_i(tpkt, off, offC1+szC1);
				    switch(ASN_iTAG(tpkt,offC2))
				    {
					case 0xA1: ASN_iAccessResult(tpkt, off, szC2, io); break;	//  listOfAccessResult
					default: off += szC2;	break;
				    }
				}
				break;
			    case Conf_getVariableAccessAttributes:
				for(offC1 = off; (offC1+szC1) != off; )
				{
				    int offC2 = off, szC2 = ASN_i(tpkt, off, offC1+szC1);
				    switch(ASN_iTAG(tpkt,offC2))
				    {
					case 0x80: io.setAttr("mmsDeletable", int2s((bool)ASN_iN(tpkt,off,szC2))); break;	//  mmsDeletable
					case 0xA2: ASN_iTypeSpec(tpkt, off, szC2, io); break;		//  typeSpecification
					default: off += szC2;	break;
				    }
				}
				break;
			    default: off += szC1;	break;
			}
		    }
		    break;
		}
	    }
	}
    }
    catch(Error er) { err = strMess("100:%s", er.mess.c_str()); }

    io.setAttr("err", err);
}

void Client::reqService( XML_N &io )
{
    io.setAttr("err", "");
    if(!isInitiated)
    {
	XML_N req("MMS"); req.setAttr("id","connect");
	protIO(req);
	if(!req.attr("err").empty())	{ io.setAttr("err", req.attr("err")); return; }

	req.clear()->setAttr("id","initiate");
	protIO(req);
	if(!req.attr("err").empty())	{ io.setAttr("err", req.attr("err")); return; }

	isInitiated = true;
    }
    if(id_vName.empty())
    {
	XML_N req("MMS"); req.setAttr("id","identify");
	protIO(req);
	if(!req.attr("err").empty())	{ io.setAttr("err", req.attr("err")); return; }
	id_vName = req.attr("vendorName");
	id_modelName = req.attr("modelName");
	id_rev = req.attr("revision");
    }

    protIO(io);
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
    if(id >= childSize()) throw Error("Child %d is not present.", id);
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
    throw Error("Child %d is not present.", index);
}

XML_N* XML_N::childGet( const string &name, const int numb, bool noex ) const
{
    for(int i_ch = 0, i_n = 0; i_ch < (int)childSize(); i_ch++)
	if(strcasecmp(childGet(i_ch)->name().c_str(),name.c_str()) == 0 && i_n++ == numb)
	    return childGet(i_ch);

    if(noex) return NULL;
    throw Error("Child %s:%d is not found!", name.c_str(), numb);
}

XML_N* XML_N::childGet( const string &attr, const string &val, bool noex ) const
{
    for(unsigned i_f = 0; i_f < childSize(); i_f++)
	if(childGet(i_f)->attr(attr) == val) return childGet(i_f);

    if(noex) return NULL;
    throw Error("Child with attribut %s=%s is not present.", attr.c_str(), val.c_str());
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
//* Error					  *
//*************************************************
Error::Error( const char *fmt, ... )
{
    char str[1024];		  //!!!!
    va_list argptr;

    va_start(argptr,fmt);
    vsnprintf(str,sizeof(str),fmt,argptr);
    va_end(argptr);

    cod  = 0;
    mess = str;
}

Error::Error( int icod, const char *fmt, ... )
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
