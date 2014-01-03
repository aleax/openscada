
//OpenSCADA system module Special.FLibSYS file: io.cpp
/***************************************************************************
 *   Copyright (C) 2013 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "io.h"

using namespace FLibSYS;

//*************************************************
//* IOObj object (stream, file)                   *
//*************************************************
IOObj::IOObj( const string &nm, const string &perm, const string &mchFormt, const string &encIn ) : fhd(NULL), pos(0)
{
    open(nm, perm, mchFormt, encIn);
}

IOObj::~IOObj( )
{
    close();
}

bool IOObj::open( const string &nm, const string &perm, const string &mchFormt, const string &encIn )
{
    //Try for file open
    if(perm.size()) fhd = fopen(nm.c_str(), perm.c_str());
    else { str = nm; pos = 0; }
}

void IOObj::close( )
{
    if(fhd) fclose(fhd);
    str = ""; pos = 0;
}

TVariant IOObj::propGet( const string &id )
{
    if(id == "length")
    {
	if(!fhd) return (int)str.size();
	pos = ftell(fhd);
	fseek(fhd, 0, SEEK_END);
	int end = ftell(fhd);
	fseek(fhd, pos, SEEK_CUR);
	return end;
    }
    if(id == "pos") return fhd ? (int)ftell(fhd) : (int)str.size();

    throw TError("IOObj", _("Properties no supported by the object."));
}

void IOObj::propSet( const string &id, TVariant val )
{
    if(id == "pos")
    {
	if(fhd)	fseek(fhd, pos, SEEK_CUR);
	else pos = vmin(str.size(),vmax(0,val.getI()));
    }

    throw TError("IOObj", _("Properties no supported by the object."));
}

TVariant IOObj::funcCall( const string &id, vector<TVariant> &prms )
{
    // {string|int|real|Array[int|real]} read(string valType = "char", int cnt = -1, string mchFmtEnc = "n|NoEnc") -
    //		read value <valType> in <cnt> for machine format or string encodeIn <mchFmtEnc>
    //	valType - value type (char,int,float,real*4,...)
    //	cnt - values by data type counter; for no strings and multiply counter used Array as result (-1 up to end);
    //	mchFmtEnc - machine format (native(n), ieee-be(b), ieee-le(l)) or encodeIn for string
    if(id == "read" && prms.size() >= 1)
    {
	TpDescr &tpD = getTp((prms.size()>=1) ? prms[0].getS() : "char");
	long cnt = (prms.size()>=2) ? prms[1].getI() : -1;
	if(!cnt) return false;

	//Char stream
	if(tpD.szBt == 1)
	{
	    string rez, inCd = (prms.size()>=3) ? prms[2].getS() : "";
	    if(!fhd)
	    {
		rez = str.substr(pos,vmax(0,vmin(str.size()-pos,(cnt<0)?str.size():cnt)));
		pos = vmax(str.size(), pos+cnt);
	    }
	    else
	    {
		char buf[STR_BUF_LEN];
		if(cnt < 0) cnt = USER_FILE_LIMIT;
		for(int r_cnt = 0; (r_cnt=fread(buf,1,vmin(sizeof(buf),cnt-rez.size()),fhd)); )
		    rez.append(buf,r_cnt);
	    }
	    if(inCd.size()) rez = Mess->codeConvIn(inCd, rez);
	    return rez;
	}
	//Integer
	else if(!tpD.real)
	{
	    TArrayObj *ao = NULL;
	    int64_t rez = 0;
	    string mach = (prms.size()>=3) ? prms[2].getS() : "n";
	    if(mach.empty()) mach = "n";
	    // From string stream
	    if(!fhd)
	    {
		if(cnt != 1) ao = new TArrayObj();
		for(unsigned i_cnt = 0; (cnt < 0 || i_cnt < cnt) && pos <= (str.size()-tpD.szBt); pos += tpD.szBt, i_cnt++)
		{
		    switch(tpD.szBt)
		    {
			case 2:
			{
			    uint16_t v = TSYS::getUnalign16(str.data()+pos);
			    switch(mach[0])
			    {
				case 'l': v = TSYS::i16_LE(v);	break;
				case 'b': v = TSYS::i16_BE(v);	break;
			    }
			    rez = tpD.sign ? (int16_t)v : v;
			    break;
			}
			case 4:
			{
			    uint32_t v = TSYS::getUnalign32(str.data()+pos);
			    switch(mach[0])
			    {
				case 'l': v = TSYS::i32_LE(v);	break;
				case 'b': v = TSYS::i32_BE(v);	break;
			    }
			    rez = tpD.sign ? (int32_t)v : v;
			    break;
			}
			case 8:
			{
			    uint64_t v = TSYS::getUnalign64(str.data()+pos);
			    switch(mach[0])
			    {
				case 'l': v = TSYS::i64_LE(v);	break;
				case 'b': v = TSYS::i64_BE(v);	break;
			    }
			    rez = tpD.sign ? (int64_t)v : v;
			    break;
			}
		    }
		    if(cnt == 1) return (int)rez;
		    ao->arSet(i_cnt, (int)rez);
		}
		return ao;
	    }
	    // From file
	    char buf[STR_BUF_LEN];
	    if(cnt != 1) ao = new TArrayObj();
	    if(cnt < 0) cnt = USER_FILE_LIMIT/tpD.szBt;
	    cnt *= tpD.szBt;
	    for(int r_cnt = 0, r_full = 0; (r_cnt=fread(buf,1,vmin(sizeof(buf),cnt*tpD.szBt-r_full),fhd)); r_full += r_cnt)
		for(unsigned i_cnt = 0, pos = 0; pos <= (r_cnt-tpD.szBt); pos += tpD.szBt, i_cnt++)
		{
		    switch(tpD.szBt)
		    {
			case 2:
			{
			    uint16_t v = *(uint16_t*)(str.data()+pos);
			    switch(mach[0])
			    {
				case 'l': v = TSYS::i16_LE(v);	break;
				case 'b': v = TSYS::i16_BE(v);	break;
			    }
			    rez = tpD.sign ? (int16_t)v : v;
			    break;
			}
			case 4:
			{
			    uint32_t v = *(uint32_t*)(str.data()+pos);
			    switch(mach[0])
			    {
				case 'l': v = TSYS::i32_LE(v);	break;
				case 'b': v = TSYS::i32_BE(v);	break;
			    }
			    rez = tpD.sign ? (int32_t)v : v;
			    break;
			}
			case 8:
			{
			    uint64_t v = *(uint64_t*)(str.data()+pos);
			    switch(mach[0])
			    {
				case 'l': v = TSYS::i64_LE(v);	break;
				case 'b': v = TSYS::i64_BE(v);	break;
			    }
			    rez = tpD.sign ? (int64_t)v : v;
			    break;
			}
		    }
		    if(cnt == 1) return rez;
		    ao->arSet(i_cnt, rez);
		}
	    return ao;
	}
	//Real
	else
	{
	    TArrayObj *ao = NULL;
	    double rez = 0;
	    string mach = (prms.size()>=3) ? prms[2].getS() : "n";
	    // From string stream
	    if(!fhd)
	    {
		if(cnt != 1) ao = new TArrayObj();
		for(unsigned i_cnt = 0; (cnt < 0 || i_cnt < cnt) && pos <= (str.size()-tpD.szBt); pos += tpD.szBt, i_cnt++)
		{
		    switch(tpD.szBt)
		    {
			case 4:
			{
			    float v = TSYS::getUnalignFloat(str.data()+pos);
			    switch(mach[0])
			    {
				case 'l': v = TSYS::floatLErev(v);	break;
				case 'b': v = TSYS::floatBErev(v);	break;
			    }
			    rez = v;
			    break;
			}
			case 8:
			{
			    rez = TSYS::getUnalignDbl(str.data()+pos);
			    switch(mach[0])
			    {
				case 'l': rez = TSYS::doubleLErev(rez);	break;
				case 'b': rez = TSYS::doubleBErev(rez);	break;
			    }
			    break;
			}
		    }
		    if(!ao) return rez;
		    ao->arSet(i_cnt, rez);
		}
		return ao;
	    }
	    // From file
	    char buf[STR_BUF_LEN];
	    if(cnt != 1) ao = new TArrayObj();
	    if(cnt < 0) cnt = USER_FILE_LIMIT/tpD.szBt;
	    cnt *= tpD.szBt;
	    for(int r_cnt = 0, r_full = 0; (r_cnt=fread(buf,1,vmin(sizeof(buf),cnt*tpD.szBt-r_full),fhd)); r_full += r_cnt)
		for(unsigned i_cnt = 0, pos = 0; pos <= (r_cnt-tpD.szBt); pos += tpD.szBt, i_cnt++)
		{
		    switch(tpD.szBt)
		    {
			case 4:
			{
			    float v = *(float*)(str.data()+pos);
			    switch(mach[0])
			    {
				case 'l': v = TSYS::floatLErev(v);	break;
				case 'b': v = TSYS::floatBErev(v);	break;
			    }
			    rez = v;
			    break;
			}
			case 8:
			{
			    rez = *(double*)(str.data()+pos);
			    switch(mach[0])
			    {
				case 'l': rez = TSYS::doubleLErev(rez);	break;
				case 'b': rez = TSYS::doubleBErev(rez);	break;
			    }
			    break;
			}
		    }
		    if(!ao) return rez;
		    ao->arSet(i_cnt, rez);
		}
	    return ao;
	}

	return false;
    }
    // IO write({string|int|double|TArray} vals, string valType = "char", string mchFmtEnc = "n|NoEnc") -
    //		write value[s] <vals> for type <valType> for machine format or string encodeIn <mchFmtEnc>
    //	vals - single value or values array for write;
    //	valType - value type (char,int,float,real*4,...)
    //	mchFmtEnc - machine format (native(n), ieee-be(b), ieee-le(l)) or encodeIn for string
    if(id == "write" && prms.size() >= 1)
    {
	TVariant &vals = prms[0];
	TpDescr tpD;
	if(prms.size() >= 2) tpD = getTp(prms[1].getS());
	//!!!! Check for real data type

	//Char stream
	if(tpD.szBt == 1)
	{
	    string rez, inCd = (prms.size()>=3) ? prms[2].getS() : "";
	    /*if(!fhd) rez = str.substr(pos,vmax(0,vmin(str.size()-pos,(cnt<0)?str.size():cnt)));
	    else
	    {
	    }*/
	}
	//Integer
	else if(!tpD.real)
	{
	    //????

	}
	//Real
	else
	{
	    //????

	}

	return this;
    }

    throw TError("IOObj",_("Function '%s' error or not enough parameters."),id.c_str());
}

IOObj::TpDescr &IOObj::getTp( const string &dtT )
{
    //Init data types
    if(dTPs.empty())
    {
	dTPs["uchar"] = dTPs["uint8"] = dTPs["unsigned char"] = TpDescr(1);
	dTPs["uint16"] = dTPs["ushort"] = TpDescr(2);
	dTPs["uint32"] = dTPs["uint"] = dTPs["ulong"] = TpDescr(4);
	dTPs["uint64"] = TpDescr(8);
	dTPs["char"] = dTPs["int8"] = dTPs["schar"] = dTPs["signed char"] = dTPs["integer*1"] = TpDescr(1, false, true);
	dTPs["int16"] = dTPs["short"] = dTPs["integer*2"] = TpDescr(2, false, true);
	dTPs["int32"] = dTPs["int"] = dTPs["long"] = dTPs["integer*4"] = dTPs["integer"] = TpDescr(4, false, true);
	dTPs["int64"] = dTPs["integer*8"] = TpDescr(8, false, true);
	dTPs["float"] = dTPs["float32"] = dTPs["single"] = dTPs["real*4"] = TpDescr(4, true);
	dTPs["double"] = dTPs["float64"] = dTPs["real*8"] = dTPs["real"] = TpDescr(8, true);
    }

    return dTPs[dtT];
}
