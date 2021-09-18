
//OpenSCADA file: tmess.cpp
/***************************************************************************
 *   Copyright (C) 2003-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#include <sys/types.h>
#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <errno.h>
#if defined(__ANDROID__)
#include <android/log.h>
#endif

#include <algorithm>

#include "tsys.h"
#include "resalloc.h"
#include "tmess.h"

#ifdef HAVE_LANGINFO_H
# include <langinfo.h>
#endif
#ifdef HAVE_ICONV_H
#include <iconv.h>
#endif
#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#endif

#ifdef HAVE_LIBINTL_H
extern int _nl_msg_cat_cntr;	//Detection counter of an environment of language changes of gettext
#endif

using namespace OSCADA;

//*************************************************
//* TMess                                         *
//*************************************************
TMess::TMess( ) : IOCharSet("UTF-8"), mMessLevel(Info), mLogDir(DIR_STDOUT|DIR_ARCHIVE),
    mConvCode(true), mIsUTF8(true), mTranslDyn(false), mTranslDynPlan(false), mTranslEnMan(false), mTranslSet(false),
    mLang2CodeBase(dtRes), mLang2Code(dtRes), getMessRes(true)
{
    openlog(PACKAGE, 0, LOG_USER);

    //Cleaning the LC_ALL since it overides the proper LC_NUMERIC
    char *lc = getenv("LC_ALL");
    if(lc) { setenv("LANG", lc, 0); unsetenv("LC_ALL"); }

    setenv("LC_NUMERIC", "C", 1);
    setlocale(LC_ALL, "");
#ifdef HAVE_LANGINFO_H
    IOCharSet = nl_langinfo(CODESET);
#endif

#ifdef HAVE_LIBINTL_H
    bindtextdomain(PACKAGE, localedir_full);
    textdomain(PACKAGE);
#endif

    string tLng = lang();
    mLang2Code = tLng;
    if(mLang2Code.size() < 2 || mLang2Code.getVal() == "POSIX" || mLang2Code.getVal() == "C") mLang2Code = "en";
    else mLang2Code = mLang2Code.getVal().substr(0,2);
    mIsUTF8 = (IOCharSet == "UTF-8" || IOCharSet == "UTF8" || IOCharSet == "utf8");

    if(tLng == "C" || (mLang2Code.getVal() == "en" && (IOCharSet == "ISO-8859-1" || IOCharSet == "ANSI_X3.4-1968" || IOCharSet == "ASCII" || IOCharSet == "US-ASCII")))
	mConvCode = false;
}

TMess::~TMess( )
{
    closelog();
}

void TMess::setMessLevel( int level )
{
    mMessLevel = vmax(Debug, vmin(Crit,level));
    SYS->modif();
}

void TMess::setLogDirect( int dir )
{
    mLogDir = dir;
    SYS->modif();
}

void TMess::put( const char *categ, int8_t level, const char *fmt,  ... )
{
    if(abs(vmin(Emerg,vmax(-Emerg,level))) < messLevel()) return;

    //messLevel() = TMess::Debug process for selected category and categories list combining
    if(messLevel() == TMess::Debug && level == TMess::Debug) {
	MtxAlloc res(mRes, true);
	// Check for present into debugCats and put new
	if(debugCats.find(categ) == debugCats.end()) {
	    string curCatLev, tCat;
	    for(int off = 0; (tCat=TSYS::pathLev(categ,0,true,&off)).size(); ) {
		curCatLev += "/" + tCat;
		if(debugCats.find(curCatLev) == debugCats.end()) debugCats[curCatLev] = false;
	    }
	}

	// Check for match to selectDebugCats
	bool matchOK = false;
	for(unsigned iDC = 0; !matchOK && iDC < selectDebugCats.size(); iDC++)
	    matchOK = (strncmp(categ,selectDebugCats[iDC].c_str(),selectDebugCats[iDC].size()) == 0);
	if(!matchOK) return;
    }

    //Put the message
    va_list argptr;

    va_start(argptr, fmt);
    putArg(categ, level, fmt, argptr);
    va_end(argptr);
}

void TMess::put_( const char *categ, int8_t level, const char *fmt,  ... )
{
    va_list argptr;

    va_start(argptr, fmt);
    putArg(categ, level, fmt, argptr);
    va_end(argptr);
}

void TMess::putArg( const char *categ, int8_t level, const char *fmt, va_list ap )
{
    char mess[prmStrBuf_SZ];
    vsnprintf(mess, sizeof(mess), fmt, ap);

    level = vmin(Emerg, vmax(-Emerg,level));
    int64_t ctm = TSYS::curTime();
    //string sMess = i2s(level) + "|" + categ + " | " + mess;
    string sMess = i2s(level) + "[" + categ + "] " + mess;

#if defined(__ANDROID__)
    if(mLogDir&(DIR_SYSLOG|DIR_STDOUT|DIR_STDERR)) {
	int level_sys;
	switch((int8_t)abs(level)) {
	    case Debug:		level_sys = ANDROID_LOG_DEBUG;	break;
	    case Info:		level_sys = ANDROID_LOG_INFO;	break;
	    case Notice:	level_sys = ANDROID_LOG_DEFAULT;break;
	    case Warning:	level_sys = ANDROID_LOG_WARN;	break;
	    case Error:		level_sys = ANDROID_LOG_ERROR;	break;
	    case Crit:
	    case Alert:
	    case Emerg:		level_sys = ANDROID_LOG_FATAL;	break;
	    default: 		level_sys = ANDROID_LOG_DEFAULT;
	}
	__android_log_vprint(level_sys, PACKAGE_NAME, sMess.c_str(), ap);
    }
#else
    if(mLogDir&DIR_SYSLOG) {
	int level_sys;
	switch((int8_t)abs(level)) {
	    case Debug:		level_sys = LOG_DEBUG;	break;
	    case Info:		level_sys = LOG_INFO;	break;
	    case Notice:	level_sys = LOG_NOTICE;	break;
	    case Warning:	level_sys = LOG_WARNING;break;
	    case Error:		level_sys = LOG_ERR;	break;
	    case Crit:		level_sys = LOG_CRIT;	break;
	    case Alert:		level_sys = LOG_ALERT;	break;
	    case Emerg:		level_sys = LOG_EMERG;	break;
	    default: 		level_sys = LOG_DEBUG;
	}
	syslog(level_sys, "%s", sMess.c_str());
    }
    if(mLogDir&(DIR_STDOUT|DIR_STDERR) && SYS->cmdOpt("consoleCharSet").size())
	sMess = Mess->codeConvOut(SYS->cmdOpt("consoleCharSet"), sMess);
    if(mLogDir&DIR_STDOUT)	fprintf(stdout, "%s %s\n", atm2s(SYS->sysTm(),"%Y-%m-%dT%H:%M:%S").c_str(), sMess.c_str());
    if(mLogDir&DIR_STDERR)	fprintf(stderr, "%s %s\n", atm2s(SYS->sysTm(),"%Y-%m-%dT%H:%M:%S").c_str(), sMess.c_str());
#endif

    if((mLogDir&DIR_ARCHIVE) && SYS->present("Archive"))
	SYS->archive().at().messPut(ctm/1000000, ctm%1000000, categ, level, mess, BUF_ARCH_NM);
}

void TMess::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &recs, const string &category, int8_t level )
{
    if(mLogDir & DIR_ARCHIVE)	SYS->archive().at().messGet(b_tm, e_tm, recs, category, level);
}

string TMess::translFld( const string &lng, const string &fld, bool isCfg )	{ return isCfg ? fld+"_"+lng : lng+"#"+fld; }

bool TMess::isMessTranslable( const string &mess )
{
    bool isTrSymb = false;
    for(unsigned iCh = 0; iCh < mess.size() && !isTrSymb; ++iCh)
	isTrSymb = !(isspace(mess[iCh]) || isdigit(mess[iCh]) || ispunct(mess[iCh]));

    return isTrSymb;
}

void TMess::setLang2CodeBase( const string &vl )
{
    mLang2CodeBase = vl;
    if((!mLang2CodeBase.empty() && mLang2CodeBase.size() < 2) || mLang2CodeBase.getVal() == "POSIX" || mLang2CodeBase.getVal() == "C")
	mLang2CodeBase = "en";
    else mLang2CodeBase = mLang2CodeBase.getVal().substr(0,2);

    SYS->modif();
}

void TMess::setTranslDyn( bool val, bool plan )
{
    if(plan) mTranslDynPlan = val;
    else mTranslDyn = mTranslDynPlan = val;

    SYS->modif();
}

void TMess::setTranslEnMan( bool vl, bool passive )
{
    if(vl == mTranslEnMan || mTranslSet) return;
    mTranslSet = true;
    mTranslEnMan = vl;
    if(!passive) {
	if(vl) {
	    SYS->modifG(); SYS->load();	//Built messages load
	    translReg("", "uapi:");	//User API messages load
	} else trMessIdx.clear();
    }

    mTranslSet = false;

    SYS->modif();
}

string TMess::translGet( const string &base, const string &lang, const string &src )
{
    string rez, tStr, cKey = base, trLang = lang2Code();
    if(translDyn()) {
	if(lang.size() >= 2) trLang = lang.substr(0,2);
	if(trLang == lang2CodeBase()) return base;
	cKey = trLang+"#"+cKey;
    } else if(src.compare(0,5,"uapi:") != 0) return base;

    MtxAlloc res(mRes, true);

    //Request from the cache at first
    map<string,CacheEl>::iterator itr = trMessCache.find(cKey);
    if(itr != trMessCache.end()) { itr->second.tm = SYS->sysTm(); rez = itr->second.val; }
    else {
	//Request to data source direct
	if(src.compare(0,5,"uapi:") == 0) {	// Check/Get/Place from user API translations table
	    string srcAddrs = src.substr(5), tStrVl;
	    if(srcAddrs.empty()) srcAddrs = SYS->workDB();

	    TConfig req;
	    req.setIncmplTblStrct(true);
	    int8_t isCfg = -1;

	    vector<string> addrs;
	    for(int off = 0; (tStrVl=TSYS::strParse(srcAddrs,0,";",&off)).size(); ) addrs.push_back(tStrVl);
	    for(vector<string>::reverse_iterator iA = addrs.rbegin(); iA != addrs.rend(); ++iA) {
		tStrVl = translFld(trLang, "base", (isCfg==1));
		if(isCfg != (*iA==DB_CFG)) {
		    isCfg = (*iA==DB_CFG) ? 1 : 0;
		    req.elem().fldClear();
		    req.elem().fldAdd(new TFld("base","Base",TFld::String,TCfg::Key,"1000"));
		    req.elem().fldAdd(new TFld(tStrVl.c_str(),"Tr",TFld::String,0));
		    req.cfg("base").setS(base);
		}
		if(TBDS::dataGet(*iA+"." mess_TrUApiTbl,"/" mess_TrUApiTbl,req,TBDS::NoException)) {
		    rez = req.cfg(tStrVl).getS();
		    break;
		}
		// Create new record into the translation table of the data source
		else if((iA+1) == addrs.rend() /*&& lang2CodeBase().size()*/) {
		    if(lang2CodeBase().size() && lang2CodeBase() == trLang)	req.elem().fldDel(req.elem().fldId(tStrVl.c_str()));
		    TBDS::dataSet(*iA+"." mess_TrUApiTbl, "/" mess_TrUApiTbl, req, TBDS::NoException);
		}
	    }
	    trMessCache[cKey] = CacheEl(rez, SYS->sysTm());
	}
	//Check included sources from index
	else {
	    map<string, map<string,string> >::iterator im = trMessIdx.find(base);
	    if(im != trMessIdx.end()) {
		TConfig req;
		for(map<string,string>::iterator is = im->second.begin(); rez.empty() && is != im->second.end(); ++is) {
		    string trSrc = TSYS::strParse(is->first,0,"#"), trFld = TSYS::strParse(is->first,1,"#"), reqFld;
		    bool isCfg = false;
		    //  Source is config file or included DB
		    if((isCfg=trSrc.compare(0,4,"cfg:")==0) || trSrc.compare(0,3,"db:") == 0) {
			reqFld = translFld(trLang, trFld, isCfg);
			//  Need DB structure prepare
			req.elem().fldClear();
			req.elem().fldAdd(new TFld(trFld.c_str(),"",TFld::String,0));
			req.elem().fldAdd(new TFld(reqFld.c_str(),"",TFld::String,0));
			req.cfg(trFld).setReqKey(true);
			req.cfg(trFld).setS(im->first);

			//  Get from config file or DB source
			bool seekRez = false;
			for(int inst = 0; rez.empty(); inst++) {
			    seekRez = isCfg ? TBDS::dataSeek("", trSrc.substr(4), inst, req, TBDS::UseCache)
					    : TBDS::dataSeek(trSrc.substr(3), "", inst, req, TBDS::UseCache);
			    if(!seekRez) break;
			    rez = req.cfg(reqFld).getS();
			}
		    }
		}
	    }
	    trMessCache[cKey] = CacheEl(rez, SYS->sysTm());
	}

	//Cache data and limit update
	if(trMessCache.size() > 10*(limCacheIts_N+limCacheIts_N/10)) {
	    vector< pair<time_t,string> > sortQueue;
	    for(map<string,CacheEl>::iterator itr = trMessCache.begin(); itr != trMessCache.end(); ++itr)
		sortQueue.push_back(pair<time_t,string>(itr->second.tm,itr->first));
	    sort(sortQueue.begin(), sortQueue.end());
	    for(unsigned iDel = 0; iDel < 10*(limCacheIts_N/10); ++iDel) trMessCache.erase(sortQueue[iDel].second);
	}
    }

    return rez.empty() ? base : rez;
}

string TMess::translGetU( const string &base, const string &user, const string &src )
{
    if(!translDyn() && src.empty()) return base;
    return translGet(base, (SYS->security().at().usrPresent(user)?SYS->security().at().usrAt(user).at().lang():lang2Code()), src);
}

string TMess::translGetLU( const string &base, const string &lang, const string &user, const string &src )
{
    if(lang.size()) return translGet(base, lang, src);
    return translGetU(base, user, src);
}

string TMess::translSet( const string &base, const string &lang, const string &mess, bool *needReload, const string &srcFltr )
{
    if(!translDyn() || !needReload) return mess;

    string trLang = lang2Code();
    if(lang.size() >= 2)	trLang = lang.substr(0,2);
    else if(trLang.empty())	trLang = lang2Code();
    if(base.empty() && mess.size())	trLang = lang2CodeBase();
    bool chBase = (trLang == lang2CodeBase());

    MtxAlloc res(mRes, true);
    map<string,string> mI = trMessIdx[base];
    for(map<string,string>::iterator is = mI.begin(); is != mI.end(); ++is) {
	if(srcFltr.size() && (is->first+"#"+is->second).find(srcFltr) == string::npos)	continue;

	string trSrc = TSYS::strParse(is->first,0,"#"), setFld = TSYS::strParse(is->first,1,"#");

	TConfig req;
	bool setRes = false, isCfg = false;
	//  Source is config file or included DB
	if((isCfg=trSrc.compare(0,4,"cfg:") == 0) || trSrc.compare(0,3,"db:") == 0) {	//Source is config file
	    req.elem().fldAdd(new TFld(setFld.c_str(),setFld.c_str(),TFld::String,0));
	    req.cfg(setFld).setReqKey(true);
	    req.cfg(setFld).setS(base, chBase?TCfg::ExtValTwo|TCfg::ForceUse:0);
	    if(!chBase) {
		setFld = translFld(trLang, setFld, isCfg);
		req.elem().fldAdd(new TFld(setFld.c_str(),setFld.c_str(),TFld::String,0));
	    }
	    req.cfg(setFld).setS(mess, TCfg::ForceUse);
	    setRes = isCfg ? TBDS::dataSet("", trSrc.substr(4), req, TBDS::NoException)
			   : TBDS::dataSet(trSrc.substr(3), "", req, TBDS::NoException);
	}
	//  Move the source to new base
	if(setRes && chBase) {
	    if(needReload) *needReload = trMessIdx[mess].size();
	    trMessIdx[mess][is->first] = is->second;
	}
    }
    if(chBase) trMessIdx.erase(base);
    trMessCache.clear();

    return chBase ? mess : base;
}

string TMess::translSetU( const string &base, const string &user, const string &mess, bool *needReload )
{
    if(!translDyn() && !needReload) return mess;
    return translSet(base, (SYS->security().at().usrPresent(user)?SYS->security().at().usrAt(user).at().lang():lang2Code()), mess, needReload);
}

string TMess::translSetLU( const string &base, const string &lang, const string &user, const string &mess, bool *needReload )
{
    if(lang.size()) return translSet(base, lang, mess, needReload);
    return translSetU(base, user, mess, needReload);
}

void TMess::translReg( const string &mess, const string &src, const string &prms )
{
    if(!translEnMan()) return;

    //Service request of loading the translation index
    if(src.compare(0,5,"uapi:") == 0) {
	vector<string> ls;
	if(src.size() > 5) ls.push_back(src.substr(5));
	else { TBDS::dbList(ls); ls.push_back("<cfg>"); }

	TConfig req;
	req.elem().fldAdd(new TFld("base","Base",TFld::String,TCfg::Key,"1000"));

	MtxAlloc res(mRes, true);
	for(unsigned iL = 0; iL < ls.size(); iL++)
	    if(ls[iL] == DB_CFG)
		for(int io_cnt = 0; TBDS::dataSeek("","/" mess_TrUApiTbl,io_cnt++,req,TBDS::UseCache); )
		    trMessIdx[req.cfg("base").getS()]["cfg:/" mess_TrUApiTbl] = prms;
	    else
		for(int io_cnt = 0; TBDS::dataSeek(ls[iL]+"." mess_TrUApiTbl,"",io_cnt++,req,TBDS::UseCache); )
		    trMessIdx[req.cfg("base").getS()]["db:"+ls[iL]+"." mess_TrUApiTbl "#base"] = prms;
    }
    //Same translations registration
    else {
	if(sTrm(mess).empty() || !isMessTranslable(mess)) return;
	MtxAlloc res(mRes, true);
	trMessIdx[mess][src] = prms;
    }
}

string TMess::lang( )
{
    char *lng = NULL;
    if(((lng=getenv("LANGUAGE")) && strlen(lng)) || ((lng=getenv("LC_MESSAGES")) && strlen(lng)) || ((lng=getenv("LANG")) && strlen(lng)))
	return lng;
    else return "C";
}

string TMess::selDebCats( )
{
    string rez;

    MtxAlloc res(mRes, true);
    for(unsigned i_sdc = 0; i_sdc < selectDebugCats.size(); i_sdc++)
	rez += selectDebugCats[i_sdc]+";";

    return rez;
}

void TMess::setSelDebCats( const string &vl )
{
    MtxAlloc res(mRes, true);
    debugCats.clear();
    selectDebugCats.clear();

    string curCat;
    for(int off = 0; (curCat=TSYS::strParse(vl,0,";",&off)).size(); ) {
	debugCats[curCat] = true;
	selectDebugCats.push_back(curCat);
    }
    SYS->modif();
}

void TMess::setLang( const string &lng, bool init )
{
    char *prvLng = NULL;
    if((prvLng=getenv("LANGUAGE")) && strlen(prvLng)) setenv("LANGUAGE", lng.c_str(), 1);
    //else setenv("LC_MESSAGES", lng.c_str(), 1);
    else setenv("LANG", lng.c_str(), 1);	//!!!! May be use further for the miss environment force set
    setlocale(LC_ALL, "");
#if defined(__ANDROID__)
    setlocale(LC_CTYPE, "C.UTF-8");	//For original NDK r13 and Android >= 5
#endif

#ifdef HAVE_LANGINFO_H
    IOCharSet = nl_langinfo(CODESET);
#endif

    string tLng = lang();
    mLang2Code = tLng;
    if(mLang2Code.size() < 2 || mLang2Code.getVal() == "POSIX" || mLang2Code.getVal() == "C") mLang2Code = "en";
    else mLang2Code = mLang2Code.getVal().substr(0, 2);
    mIsUTF8 = (IOCharSet == "UTF-8" || IOCharSet == "UTF8" || IOCharSet == "utf8");

    mConvCode = !(tLng == "C" || (mLang2Code.getVal() == "en" && (IOCharSet == "ISO-8859-1" || IOCharSet == "ANSI_X3.4-1968" || IOCharSet == "ASCII" || IOCharSet == "US-ASCII")));

    if(init) SYS->sysModifFlgs &= ~TSYS::MDF_LANG;
    else { SYS->sysModifFlgs |= TSYS::MDF_LANG; SYS->modif(); }
}

string TMess::codeConv( const string &fromCH, const string &toCH, const string &mess )
{
    if(!mConvCode || fromCH == toCH) return mess;

#ifdef HAVE_ICONV_H
    //Make convert to blocks 1000 bytes
    string buf;
    buf.reserve(mess.size());
# if defined(__UCLIBC__)
    const char	*ibuf;
# else
    char	*ibuf;
# endif
    char	outbuf[1000], *obuf;
    size_t ilen, olen, chwrcnt = 0;
    iconv_t hd;

    hd = iconv_open(toCH.c_str(), fromCH.c_str());
    if(hd == (iconv_t)(-1)) {
	//mess_crit("IConv", _("Error opening 'iconv': %s"), strerror(errno));	//But there can be a recursion for a wrong charset.
	return mess;
    }

    ibuf = (char *)mess.data();
    ilen = mess.size();

    while(ilen) {
	obuf = outbuf;
	olen = sizeof(outbuf)-1;
	size_t rez = iconv(hd, &ibuf, &ilen, &obuf, &olen);
	if(rez == (size_t)(-1) && (errno == EINVAL || errno == EBADF)) {
	    //mess_crit("IConv", _("Error converting input sequence: %s"), strerror(errno));	//But there can be a recursion for a wrong charset.
	    buf = mess;
	    break;
	}
	if(obuf > outbuf) buf.append(outbuf, obuf-outbuf);
	if(rez == (size_t)(-1) && errno == EILSEQ) { buf += '?'; ilen--; ibuf++; chwrcnt++; }
    }
    iconv_close(hd);

    //> Deadlock possible on the error message print
    //if(chwrcnt)	mess_err("IConv", _("Error converting %d symbols from '%s' to '%s' for the message part: '%s'(%d)"),
    //		    chwrcnt, fromCH.c_str(), toCH.c_str(), mess.substr(0,20).c_str(), mess.size());

    return buf;
#else
    return mess;
#endif
}

const char *TMess::I18N( const char *mess, const char *d_name, const char *mLang )
{
#ifdef HAVE_LIBINTL_H
    getMessRes.lock();
    if(translDyn()) {
	if(!mLang || !strlen(mLang)) {
	    setenv("LANGUAGE", "", 1);
	    //setenv("LC_MESSAGES", "", 1);
	    ++_nl_msg_cat_cntr;	//Make change known.
	    getMessLng = "";
	}
	else if(getMessLng != mLang) {
	    setenv("LANGUAGE", mLang, 1);
	    //setenv("LC_MESSAGES", mLang, 1);
	    ++_nl_msg_cat_cntr;	//Make change known.
	    getMessLng = mLang;
	}
    }
    const char *rez = dgettext(d_name, mess);

    /*bool chLng = (mLang && strlen(mLang) && translDyn());
    if(chLng) {
	setenv("LANGUAGE", mLang, 1);
	//setenv("LC_MESSAGES", mLang, 1);
	++_nl_msg_cat_cntr;	//Make change known.
    }
    const char *rez = dgettext(d_name, mess);
    if(chLng) {
	setenv("LANGUAGE", "", 1);
	//setenv("LC_MESSAGES", "", 1);
	++_nl_msg_cat_cntr;	//Make change known.
    }*/
    getMessRes.unlock();
    return rez;
#else
    return mess;
#endif
}

void TMess::load( )
{
    //Load params from command line
    string argVl;
    if((argVl=SYS->cmdOpt("lang")).size()) setLang(argVl, true);
    if((argVl=SYS->cmdOpt("messLev")).size()) {
	int i = s2i(argVl);
	if(i >= Debug && i <= Emerg) setMessLevel(i);
    }
    if((argVl=SYS->cmdOpt("log")).size()) setLogDirect(s2i(argVl));

    //Load params config-file
    setMessLevel(s2i(TBDS::genPrmGet(SYS->nodePath()+"MessLev",i2s(messLevel()),"root",TBDS::OnlyCfg)));
    setSelDebCats(TBDS::genPrmGet(SYS->nodePath()+"SelDebCats",selDebCats(),"root",TBDS::OnlyCfg));
    setLogDirect(s2i(TBDS::genPrmGet(SYS->nodePath()+"LogTarget",i2s(logDirect()),"root",TBDS::OnlyCfg)));
    setLang(TBDS::genPrmGet(SYS->nodePath()+"Lang",lang(),"root",TBDS::OnlyCfg), true);
    mLang2CodeBase = TBDS::genPrmGet(SYS->nodePath()+"Lang2CodeBase",mLang2CodeBase,"root",TBDS::OnlyCfg);
    setTranslDyn(s2i(TBDS::genPrmGet(SYS->nodePath()+"TranslDyn",i2s(translDyn()),"root",TBDS::OnlyCfg)), false);
    setTranslEnMan(translDyn() || s2i(TBDS::genPrmGet(SYS->nodePath()+"TranslEnMan",i2s(translEnMan()),"root",TBDS::OnlyCfg)), true);
}

void TMess::unload( )
{
    mRes.lock();
    debugCats.clear();
    selectDebugCats.clear();

    mTranslLangs = "";
    trMessIdx.clear();
    trMessCache.clear();
    mRes.unlock();

    IOCharSet = "UTF-8", mMessLevel = Info, mLogDir = DIR_STDOUT|DIR_ARCHIVE;
    mConvCode = mIsUTF8 = true, mTranslDyn = mTranslDynPlan = mTranslEnMan = mTranslSet = false;
}

void TMess::save( )
{
    TBDS::genPrmSet(SYS->nodePath()+"MessLev",i2s(messLevel()),"root",TBDS::OnlyCfg);
    TBDS::genPrmSet(SYS->nodePath()+"SelDebCats",selDebCats(),"root",TBDS::OnlyCfg);
    TBDS::genPrmSet(SYS->nodePath()+"LogTarget",i2s(logDirect()),"root",TBDS::OnlyCfg);
    if(SYS->sysModifFlgs&TSYS::MDF_LANG) TBDS::genPrmSet(SYS->nodePath()+"Lang",lang(),"root",TBDS::OnlyCfg);
    TBDS::genPrmSet(SYS->nodePath()+"Lang2CodeBase",mLang2CodeBase,"root",TBDS::OnlyCfg);
    TBDS::genPrmSet(SYS->nodePath()+"TranslDyn",i2s(translDyn(true)),"root",TBDS::OnlyCfg);
    TBDS::genPrmSet(SYS->nodePath()+"TranslEnMan",i2s(translEnMan()),"root",TBDS::OnlyCfg);
}

const char *TMess::labDB( )
{
    return _("Storage address in the format \"{DB module}.{DB name}\".\n"
	     "Set '*.*' to use the Generic Storage.");
}

const char *TMess::labSecCRON( )
{
    return _("Schedule wrotes in seconds periodic form or in the standard CRON form.\n"
	     "The seconds periodic form is one real number (1.5, 1e-3).\n"
	     "Cron is the standard form \"* * * * *\".\n"
	     "  Where items by order:\n"
	     "    - minutes (0-59);\n"
	     "    - hours (0-23);\n"
	     "    - days (1-31);\n"
	     "    - month (1-12);\n"
	     "    - week day (0[Sunday]-6).\n"
	     "  Where an item variants:\n"
	     "    - \"*\" - any value;\n"
	     "    - \"1,2,3\" - allowed values list;\n"
	     "    - \"1-5\" - raw range of allowed values;\n"
	     "    - \"*/2\" - range divider for allowed values.\n"
	     "Examples:\n"
	     "  \"1e-3\" - call with a period of one millisecond;\n"
	     "  \"* * * * *\" - each minute;\n"
	     "  \"10 23 * * *\" - only at 23 hour and 10 minute for any day and month;\n"
	     "  \"*/2 * * * *\" - for minutes: 0,2,4,...,56,58;\n"
	     "  \"* 2-4 * * *\" - for any minutes in hours from 2 to 4(include).");
}

const char *TMess::labSecCRONsel( )	{ return "1;1e-3;* * * * *;10 * * * *;10-20 2 */2 * *"; }

const char *TMess::labTaskPrior( )
{
    return _("Task priority level (-1...199), where:\n"
	     "  -1        - lowest priority batch policy;\n"
	     "  0         - standard userspace priority;\n"
	     "  1...99    - realtime priority level (round-robin), often allowed only for \"root\";\n"
	     "  100...199 - realtime priority level (FIFO), often allowed only for \"root\".");
}

const char *TMess::labMessCat( )
{
    return _("Specifies the category of the requested messages.\n"
	     "Use template's symbols for group selection:\n  '*' - any substring;\n  '?' - any character.\n"
	     "Regular expression enclosed between the symbols '/' (/mod_(System|LogicLev)/).");
}

int TMess::getUTF8( const string &str, int off, int32_t *symb )
{
    if(off < 0 || off >= (int)str.size())	return 0;
    if(!isUTF8() || !(str[off]&0x80)) {
	if(symb) *symb = (uint8_t)str[off];
	return 1;
    }
    int len = 0;
    int32_t rez = 0;
    if((str[off]&0xE0) == 0xC0)		{ len = 2; rez = str[off]&0x1F; }
    else if((str[off]&0xF0) == 0xE0)	{ len = 3; rez = str[off]&0x0F; }
    else if((str[off]&0xF8) == 0xF0)	{ len = 4; rez = str[off]&0x07; }
    if((off+len) > (int)str.size())	return 0;
    for(int iSmb = 1; iSmb < len; iSmb++)
	if((str[off+iSmb]&0xC0) != 0x80) return 0;
	else rez = (rez<<6) | (str[off+iSmb]&0x3F);
    if(symb) *symb = rez;
    return len;
}

string TMess::setUTF8( int32_t symb )
{
    string rez;
    if(symb < 0x80) rez += (char)symb;
    else for(int iCh = 5, iSt = -1; iCh >= 0; iCh--) {
	if(iSt < iCh && (symb>>(iCh*6))) iSt = iCh;
	if(iCh == iSt) rez += (char)((0xFF<<(7-iCh))|(symb>>(iCh*6)));
	else if(iCh < iSt) rez += (char)(0x80|(0x3F&(symb>>(iCh*6))));
    }

    return rez;
}
