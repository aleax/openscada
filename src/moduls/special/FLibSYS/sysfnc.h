
//OpenSCADA system module Special.FLibSYS file: sysfnc.h
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef SYSFNC_H
#define SYSFNC_H

#include <time.h>
#include <stdint.h>

#include <tfunction.h>

#include "statfunc.h"

namespace FLibSYS
{

//*************************************************
//* System access functions                       *

//*************************************************
//* System's commands call                        *
//*************************************************
class sysCall : public TFunction
{
    public:
	sysCall( ) : TFunction("sysCall",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return|IO::FullText));
	    ioAdd(new IO("com",_("Command"),IO::String,IO::Default));
	}

	string name( )	{ return _("Sys: Call"); }
	string descr( )	{ return _("System call commands."); }

	void calc( TValFunc *val ) {
	    FILE *fp = popen(val->getS(1).c_str(), "r");
	    if(!fp) return;

	    char buf[STR_BUF_LEN];
	    string rez;
	    for(int r_cnt = 0; (r_cnt=fread(buf,1,sizeof(buf),fp)); )
		rez.append(buf,r_cnt);

	    pclose(fp);
	    val->setS(0, rez);
	}
};

//*************************************************
//* SQL request to DB                             *
//*************************************************
class dbReqSQL : public TFunction
{
    public:
	dbReqSQL( ) : TFunction("dbReqSQL",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::Object,IO::Return));
	    ioAdd(new IO("addr",_("DB address"),IO::String,IO::Default));
	    ioAdd(new IO("req",_("SQL request"),IO::String,IO::Default));
	    ioAdd(new IO("trans",_("Transaction"),IO::Boolean,IO::Default,i2s(EVAL_BOOL).c_str()));
	}

	string name( )	{ return _("DB: SQL request"); }
	string descr( )	{ return _("Send SQL request to DB."); }

	void calc( TValFunc *val ) {
	    string sdb = TBDS::realDBName(val->getS(1));
	    TArrayObj *rez = new TArrayObj();
	    try {
		vector< vector<string> > rtbl;
		AutoHD<TBD> db = SYS->db().at().nodeAt(sdb,0,'.');
		db.at().sqlReq(val->getS(2), &rtbl, val->getB(3));
		for(unsigned i_r = 0; i_r < rtbl.size(); i_r++) {
		    TArrayObj *row = new TArrayObj();
		    for(unsigned i_c = 0; i_c < rtbl[i_r].size(); i_c++) row->arSet(i_c, rtbl[i_r][i_c]);
		    rez->arSet(i_r, AutoHD<TVarObj>(row));
		}
	    }
	    catch(...){ }

	    val->setO(0, rez);
	}
};

//*************************************************
//* Messages function.                            *

//*************************************************
//* Get message                                   *
//*************************************************
class messGet : public TFunction
{
    public:
	messGet( ) : TFunction("messGet", SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::Object,IO::Return));
	    ioAdd(new IO("btm",_("Begin time"),IO::Integer,IO::Default));
	    ioAdd(new IO("etm",_("End time"),IO::Integer,IO::Default));
	    ioAdd(new IO("cat",_("Category"),IO::String,IO::Default));
	    ioAdd(new IO("lev",_("Level"),IO::Integer,IO::Default));
	    ioAdd(new IO("arch",_("Archivator"),IO::String,IO::Default));
	}

	string name( )	{ return _("Mess: Get"); }
	string descr( )	{ return _("Get messages from system."); }

	void calc( TValFunc *val ) {
	    vector<TMess::SRec> recs;
	    SYS->archive().at().messGet(val->getI(1), val->getI(2), recs, val->getS(3), val->getI(4), val->getS(5));
	    TArrayObj *rez = new TArrayObj();
	    for(unsigned i_m = 0; i_m < recs.size(); i_m++) {
		TVarObj *am = new TVarObj();
		am->propSet("tm", (int)recs[i_m].time);
		am->propSet("utm", recs[i_m].utime);
		am->propSet("categ", recs[i_m].categ);
		am->propSet("level", recs[i_m].level);
		am->propSet("mess", recs[i_m].mess);
		rez->arSet(i_m, AutoHD<TVarObj>(am));
	    }
	    val->setO(0,rez);
	}
};

//*************************************************
//* Put message                                   *
//*************************************************
class messPut : public TFunction
{
    public:
	messPut( ) : TFunction("messPut", SSPC_ID) {
	    ioAdd(new IO("cat",_("Category"),IO::String,IO::Default));
	    ioAdd(new IO("lev",_("Level"),IO::Integer,IO::Default));
	    ioAdd(new IO("mess",_("Message"),IO::String,IO::Default));
	}

	string name( )	{ return _("Mess: Put"); }
	string descr( )	{ return _("Put message to system."); }

	void calc( TValFunc *val ) {
	    message(val->getS(0).c_str(), (TMess::Type)val->getI(1), "%s", val->getS(2).c_str());
	}
};

//*************************************************
//* String manipulation functions                 *

//*************************************************
//* String size                                   *
//*************************************************
class strSize : public TFunction
{
    public:
	strSize( ) : TFunction("strSize",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::Integer,IO::Return));
	    ioAdd(new IO("str",_("String"),IO::String,IO::Default));
	}

	string name( )	{ return _("String: Get size"); }
	string descr( )	{ return _("Use for string size getting."); }

	void calc( TValFunc *val ) { val->setI(0, val->getS(1).size()); }
};

//*************************************************
//* String substring                              *
//*************************************************
class strSubstr : public TFunction
{
    public:
	strSubstr( ) : TFunction("strSubstr",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("str",_("String"),IO::String,IO::Default));
	    ioAdd(new IO("pos",_("Position"),IO::Integer,IO::Default,"0"));
	    ioAdd(new IO("n"  ,_("Number"),IO::Integer,IO::Default,"-1"));
	}

	string name( )	{ return _("String: Get substring"); }
	string descr( )	{ return _("Use for substring getting."); }

	void calc( TValFunc *val ) {
	    string vl  = val->getS(1);
	    int    pos = val->getI(2);
	    if(pos < 0 || pos >= (int)vl.size()) return;
	    int	   n   = val->getI(3);
	    if(n < 0)	n = vl.size();
	    n = vmin((int)vl.size()-pos,n);
	    val->setS(0,vl.substr(pos,n));
	}
};

//*************************************************
//* String insertion                              *
//*************************************************
class strInsert : public TFunction
{
    public:
	strInsert( ) : TFunction("strInsert",SSPC_ID) {
	    ioAdd(new IO("str",_("String"),IO::String,IO::Output));
	    ioAdd(new IO("pos",_("Position"),IO::Integer,IO::Default,"0"));
	    ioAdd(new IO("ins",_("Insert string"),IO::String,IO::Default));
	}

	string name( )	{ return _("String: Insert string to other string"); }
	string descr( )	{ return _("Use for insertion string to other string."); }

	void calc( TValFunc *val ) {
	    string vl = val->getS(0);
	    int   pos = val->getI(1);
	    pos = vmax(0,vmin((int)vl.size(),pos));
	    val->setS(0,vl.insert(pos,val->getS(2)));
	}
};

//*************************************************
//* String replace                                *
//*************************************************
class strReplace : public TFunction
{
    public:
	strReplace( ) : TFunction("strReplace",SSPC_ID) {
	    ioAdd(new IO("str",_("String"),IO::String,IO::Output));
	    ioAdd(new IO("pos",_("Position"),IO::Integer,IO::Default,"0"));
	    ioAdd(new IO("n"  ,_("Number"),IO::Integer,IO::Default,"-1"));
	    ioAdd(new IO("repl",_("Replace string"),IO::String,IO::Default));
	}

	string name( )	{ return _("String: Replace part string on other string"); }
	string descr( )	{ return _("Use for replacing part string on other string."); }

	void calc( TValFunc *val ) {
	    string vl  = val->getS(0);
	    int    pos = val->getI(1);
	    if(pos < 0 || pos >= (int)vl.size()) return;
	    int	   n   = val->getI(2);
	    if(n < 0)	n = vl.size();
	    n = vmin((int)vl.size()-pos,n);
	    val->setS(0,vl.replace(pos,n,val->getS(3)));
	}
};

//*************************************************
//* String parsing on separator                   *
//*************************************************
class strParse : public TFunction
{
    public:
	strParse( ) : TFunction("strParse",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("str",_("String"),IO::String,IO::Default));
	    ioAdd(new IO("lev",_("Level"),IO::Integer,IO::Default));
	    ioAdd(new IO("sep",_("Separator"),IO::String,IO::Default,"."));
	    ioAdd(new IO("off",_("Offset"),IO::Integer,IO::Output));
	}

	string name( )	{ return _("String: Parse on separator"); }
	string descr( )	{ return _("Use for parse string on separator."); }

	void calc( TValFunc *val ) {
	    string sep = val->getS(3);
	    int off = val->getI(4);
	    val->setS(0,TSYS::strSepParse(val->getS(1),val->getI(2),sep.size()?sep[0]:' ',&off));
	    val->setI(4,off);
	}
};

//*************************************************
//* Path parsing                                  *
//*************************************************
class strParsePath : public TFunction
{
    public:
	strParsePath( ) : TFunction("strParsePath",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("path",_("Path"),IO::String,IO::Default));
	    ioAdd(new IO("lev",_("Level"),IO::Integer,IO::Default));
	    ioAdd(new IO("off",_("Offset"),IO::Integer,IO::Output));
	}

	string name( )	{ return _("String: Path parse"); }
	string descr( )	{ return _("Use for parse path on elements."); }

	void calc( TValFunc *val ) {
	    int off = val->getI(3);
	    val->setS(0,TSYS::pathLev(val->getS(1),val->getI(2),true,&off));
	    val->setI(3,off);
	}
};

//*************************************************
//* Path string convert to separated string       *
//*************************************************
class strPath2Sep : public TFunction
{
    public:
	strPath2Sep( ) : TFunction("strPath2Sep",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("src",_("Source"),IO::String,IO::Default));
	    ioAdd(new IO("sep",_("Separator"),IO::String,IO::Default,"."));
	}

	string name( )	{ return _("String: Path to separated string"); }
	string descr( )	{ return _("Use for convert path to separated string."); }

	void calc( TValFunc *val ) {
	    val->setS(0,TSYS::path2sepstr(val->getS(1),val->getS(2).size()?val->getS(2)[0]:'.'));
	}
};

//*************************************************
//* String encode to HTML                         *
//*************************************************
class strEnc2HTML : public TFunction
{
    public:
	strEnc2HTML( ) : TFunction("strEnc2HTML",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("src",_("Source"),IO::String,IO::Default));
	}

	string name( )	{ return _("String: Encode string to HTML"); }
	string descr( )	{ return _("Use for encode string for use into HTML source."); }

	void calc( TValFunc *val ) { val->setS(0,TSYS::strEncode(val->getS(1),TSYS::Html)); }
};

//*************************************************
//* String encode to Bin                          *
//*************************************************
class strEnc2Bin : public TFunction
{
    public:
	strEnc2Bin( ) : TFunction("strEnc2Bin",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("src",_("Source"),IO::String,IO::Default));
	}

	string name( )	{ return _("String: Encode text to bin"); }
	string descr( )	{ return _("Use for encode text to bin, from format <00 A0 FA DE>."); }

	void calc( TValFunc *val ) { val->setS(0, TSYS::strEncode(val->getS(1),TSYS::Bin)); }
};

//*************************************************
//* String decode from Bin                        *
//*************************************************
class strDec4Bin : public TFunction
{
    public:
	strDec4Bin( ) : TFunction("strDec4Bin",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("src",_("Source"),IO::String,IO::Default));
	    ioAdd(new IO("sep",_("Separator"),IO::String,IO::Default," "));
	}

	string name( )	{ return _("String: Decode text from bin"); }
	string descr( )	{ return _("Use for decode text from bin to format <00 A0 FA DE>."); }

	void calc( TValFunc *val ) { val->setS(0, TSYS::strDecode(val->getS(1),TSYS::Bin,val->getS(2))); }
};

//*************************************************
//* Convert real to string                        *
//*************************************************
class real2str : public TFunction
{
    public:
	real2str( ) : TFunction("real2str",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("val",_("Value"),IO::Real,IO::Default));
	    ioAdd(new IO("prc",_("Precision"),IO::Integer,IO::Default,"4"));
	    ioAdd(new IO("tp",_("Type"),IO::String,IO::Default,"f"));
	}

	string name( )	{ return _("String: Real to string"); }
	string descr( )	{ return _("Convert real to string."); }

	void calc( TValFunc *val ) {
	    val->setS(0, r2s(val->getR(1),val->getI(2),val->getS(3).size()?val->getS(3)[0]:'f'));
	}
};

//*************************************************
//* Convert integer to string                     *
//*************************************************
class int2str : public TFunction
{
    public:
	int2str( ) : TFunction("int2str",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("val",_("Value"),IO::Integer,IO::Default));
	    ioAdd(new IO("base",_("Base"),IO::Integer,IO::Default,"10"));
	}

	string name( )	{ return _("String: Integer to string"); }
	string descr( )	{ return _("Convert integer to string."); }

	void calc( TValFunc *val ) {
	    switch(val->getI(2))
	    {
		case 8:	val->setS(0, ll2s(val->getI(1),TSYS::Oct));	break;
		case 10:val->setS(0, ll2s(val->getI(1),TSYS::Dec));	break;
		case 16:val->setS(0, ll2s(val->getI(1),TSYS::Hex));	break;
		default: val->setS(0,"");
	    }
	}
};

//*************************************************
//* Convert string to real                        *
//*************************************************
class str2real : public TFunction
{
    public:
	str2real( ) : TFunction("str2real",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::Real,IO::Return));
	    ioAdd(new IO("val",_("Value"),IO::String,IO::Default));
	}

	string name( )	{ return _("String: String to real"); }
	string descr( )	{ return _("Convert string to real."); }

	void calc( TValFunc *val )	{ val->setR(0,atof(val->getS(1).c_str())); }
};

//*************************************************
//* Convert string to integer                     *
//*************************************************
class str2int : public TFunction
{
    public:
	str2int( ) : TFunction("str2int",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::Integer,IO::Return));
	    ioAdd(new IO("val",_("Value"),IO::String,IO::Default));
	    ioAdd(new IO("base",_("Base"),IO::Integer,IO::Default,"0"));
	}

	string name( )	{ return _("String: String to integer"); }
	string descr( )	{ return _("Convert string to integer."); }

	void calc( TValFunc *val )	{ val->setI(0,strtol(val->getS(1).c_str(),NULL,val->getI(2))); }
};

//*************************************************
//* Split float to words                          *
//*************************************************
class floatSplitWord : public TFunction
{
    public:
	floatSplitWord( ) : TFunction("floatSplitWord",SSPC_ID) {
	    ioAdd(new IO("val",_("Value"),IO::Real,IO::Default));
	    ioAdd(new IO("w1",_("Word 1"),IO::Integer,IO::Output));
	    ioAdd(new IO("w2",_("Word 2"),IO::Integer,IO::Output));
	}

	string name( )	{ return _("Float: Split to words"); }
	string descr( )	{ return _("Split float (4 byte) to words (2 byte)."); }

	void calc( TValFunc *val ) {
	    union { uint32_t i; float f; } wl;
	    wl.f = val->getR(0);
	    val->setI(1,wl.i&0xFFFF);
	    val->setI(2,(wl.i>>16)&0xFFFF);
	}
};

//*************************************************
//* Merge float from words                        *
//*************************************************
class floatMergeWord : public TFunction
{
    public:
	floatMergeWord( ) : TFunction("floatMergeWord",SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::Real,IO::Return));
	    ioAdd(new IO("w1",_("Word 1"),IO::Integer,IO::Default));
	    ioAdd(new IO("w2",_("Word 2"),IO::Integer,IO::Default));
	}

	string name( )	{ return _("Float: Merge from words"); }
	string descr( )	{ return _("Merge float (4 byte) from words (2 byte)."); }

	void calc( TValFunc *val ) {
	    union { uint32_t i; float f; } wl;
	    wl.i = ((val->getI(2)&0xffff)<<16) | (val->getI(1)&0xffff);
	    val->setR(0,wl.f);
	}
};

//*************************************************
//* Merge float from words                        *
//*************************************************
class CRC : public TFunction
{
    public:
	CRC( ) : TFunction("CRC", SSPC_ID) {
	    ioAdd(new IO("rez",_("Result"),IO::Integer,IO::Return));
	    ioAdd(new IO("data",_("Data"),IO::String,IO::Default));
	    ioAdd(new IO("poly",_("Polynomial (reversion)"),IO::Integer,IO::Default,"40961"));	//0xA001
	    ioAdd(new IO("width",_("Width"),IO::Integer,IO::Default,"16"));
	    ioAdd(new IO("init",_("Initial"),IO::Integer,IO::Default,"-1"));	//0xFFFFFFFFFFFFFFFF
	}

	string name( )	{ return _("Cyclic Redundancy Code (CRC)"); }
	string descr( )	{ return _("Unified Cyclic Redundancy Code implement for 8-64 bits width."); }

	void calc( TValFunc *val ) {
	    int wdth = vmin(64, vmax(1,val->getI(3)));
	    uint64_t mask = 0xFFFFFFFFFFFFFFFFll >> (64-wdth);
	    uint64_t CRC = val->getI(4) & mask;
	    uint64_t pat = val->getI(2) & mask;
	    string data = val->getS(1);
	    for(unsigned i = 0; i < data.size(); i++) {
		CRC ^= (uint8_t)data[i];
		for(char j = 0; j < 8; j++) CRC = (CRC&1) ? (CRC>>1)^pat : (CRC>>1);
	    }
	    val->setI(0, (int64_t)CRC);
	}
};

}

#endif //SYSFNC_H
