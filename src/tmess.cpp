
//OpenSCADA file: tmess.cpp
/***************************************************************************
 *   Copyright (C) 2003-2023 by Roman Savochenko, <roman@oscada.org>       *
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
    mLangBase(dtRes), mLangCode(dtRes), mTranslLangs(dtRes), getMessRes(true)
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
    mLangCode = tLng;
    if(mLangCode.size() < 2 || mLangCode.getVal() == "POSIX" || mLangCode.getVal() == "C") mLangCode = "en";
    else mLangCode = mLangCode.getVal().substr(0,2);
    mIsUTF8 = (IOCharSet == "UTF-8" || IOCharSet == "UTF8" || IOCharSet == "utf8");

    if(tLng == "C" || (mLangCode.getVal() == "en" && (IOCharSet == "ISO-8859-1" || IOCharSet == "ANSI_X3.4-1968" || IOCharSet == "ASCII" || IOCharSet == "US-ASCII")))
	mConvCode = false;
}

TMess::~TMess( )
{
    closelog();
}

void TMess::setMessLevel( int level )
{
    mMessLevel = vmax(Debug, vmin(Crit,level));

    SYS->sysModifFlgs |= TSYS::MDF_MESS;
    SYS->modif();
}

void TMess::setLogDirect( int dir )
{
    mLogDir = dir;

    SYS->sysModifFlgs |= TSYS::MDF_MESS;
    SYS->modif();
}

void TMess::put( const char *categ, int8_t level, const char *fmt,  ... )
{
    if(!messLevelTest(messLevel(),level)) return;

    //messLevel() = TMess::Debug process for selected category and categories list combining
    if(messLevel() == TMess::Debug && level == TMess::Debug) {
	MtxAlloc res(dbgRes, true);
	// Checking for presence in debugCats and put new
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

    level = vmin(TMess::MaxLev, vmax(-TMess::MaxLev,level));
    int64_t ctm = TSYS::curTime();
    //string sMess = i2s(level) + "|" + categ + " | " + mess;
    string sMess = i2s(level) + "[" + categ + "] " + mess;

    int level_in = (int8_t)abs(level), level_sys;
    if(level_in > Emerg) level_in = level_in/10;

#if defined(__ANDROID__)
    if(mLogDir&(DIR_SYSLOG|DIR_STDOUT|DIR_STDERR)) {
	switch(level_in) {
	    case Debug:		level_sys = ANDROID_LOG_DEBUG;	break;
	    case Info:		level_sys = ANDROID_LOG_INFO;	break;
	    case Notice:	level_sys = ANDROID_LOG_DEFAULT;break;
	    case Warning:	level_sys = ANDROID_LOG_WARN;	break;
	    case Error:		level_sys = ANDROID_LOG_ERROR;	break;
	    case Crit:
	    case Alert:
	    case Emerg:		level_sys = ANDROID_LOG_FATAL;	break;
	    default: 		level_sys = ANDROID_LOG_FATAL;
	}
	__android_log_vprint(level_sys, PACKAGE_NAME, sMess.c_str(), ap);
    }
#else
    if(mLogDir&DIR_SYSLOG) {
	switch(level_in) {
	    case Debug:		level_sys = LOG_DEBUG;	break;
	    case Info:		level_sys = LOG_INFO;	break;
	    case Notice:	level_sys = LOG_NOTICE;	break;
	    case Warning:	level_sys = LOG_WARNING;break;
	    case Error:		level_sys = LOG_ERR;	break;
	    case Crit:		level_sys = LOG_CRIT;	break;
	    case Alert:		level_sys = LOG_ALERT;	break;
	    case Emerg:		level_sys = LOG_EMERG;	break;
	    default: 		level_sys = LOG_EMERG;
	}
	syslog(level_sys, "%s", sMess.c_str());
    }
    if(mLogDir&(DIR_STDOUT|DIR_STDERR) && SYS->cmdOpt("consoleCharSet").size())
	sMess = Mess->codeConvOut(SYS->cmdOpt("consoleCharSet"), sMess);
    if(mLogDir&DIR_STDOUT)	fprintf(stdout, "%s %s\n", atm2s(SYS->sysTm(),"%Y-%m-%dT%H:%M:%S").c_str(), sMess.c_str());
    if(mLogDir&DIR_STDERR)	fprintf(stderr, "%s %s\n", atm2s(SYS->sysTm(),"%Y-%m-%dT%H:%M:%S").c_str(), sMess.c_str());
#endif

    if((mLogDir&DIR_ARCHIVE) && SYS->present("Archive"))
	SYS->archive().at().messPut(ctm/1000000, ctm%1000000, categ, level, mess, ARCH_BUF);
}

void TMess::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &recs, const string &category, int8_t level )
{
    if(mLogDir & DIR_ARCHIVE)	SYS->archive().at().messGet(b_tm, e_tm, recs, category, level);
}

string TMess::translFld( const string &lng, const string &fld, bool isCfg )	{ return isCfg ? fld+"_"+lng : lng+"#"+fld; }

bool TMess::isMessTranslable( const string &mess )
{
    if(mess == EVAL_STR) return false;

    for(unsigned iCh = 0; iCh < mess.size(); ++iCh)
	switch(mess[iCh]) {
	    case '0' ... '9':					//Digits
	    case ' ': case '\t': case '\x0D': case '\x0A':	//Space
	    case '!': case '"': case '#': case '$': case '%':	//Punctuation
	    case '&': case '\'': case '(': case ')': case '*':
	    case '+': case ',': case '-': case '.': case '/':
	    case ':': case ';': case '<': case '=': case '>':
	    case '?': case '@': case '[': case '\\': case ']':
	    case '^': case '_': case '`': case '{': case '|':
	    case '}': case '~':
		continue;
	    default: return true;
	}
    return false;
}

string TMess::langCodeBase( )	{ return TSYS::strParse(TSYS::strParse(langBase(),0,";"), 0, "_"); }

void TMess::setLangBase( const string &vl )
{
    int off = 0;
    string tvl = TSYS::strParse(vl, 0, ";", &off);
    if((!tvl.empty() && tvl.size() < 2) || tvl == "POSIX" || tvl == "C") tvl = "en";
    //else tvl = tvl.substr(0, 2);

    mLangBase = tvl + ((off<vl.size())?";" + vl.substr(off):"");

    SYS->sysModifFlgs |= TSYS::MDF_TR;
    SYS->modif();

    //Setting to the translation languages by default
    string lLs, lIt;
    for(int off = 0, pos = 0; (lIt=TSYS::strParse(langBase(),0,";",&off)).size(); ++pos)
	if(pos) lLs += (lLs.size()?";":"") + lIt.substr(0,2);
    setTranslLangs(lLs);
}

string TMess::langToLocale( const string &lang )
{
    string lIt;

    for(int off = 0; (lIt=TSYS::strParse(langBase(),0,";",&off)).size() && lIt.find(lang) != 0; ) ;

    return lIt.size() ? lIt : lang;
}

void TMess::setTranslDyn( bool val, bool plan )
{
    if(plan) mTranslDynPlan = val;
    else mTranslDyn = mTranslDynPlan = val;

    SYS->sysModifFlgs |= TSYS::MDF_TR;
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
	} else { trMessIdxRes.lock(); trMessIdx.clear(); trMessIdxRes.unlock(); }
    }

    mTranslSet = false;

    SYS->sysModifFlgs |= TSYS::MDF_TR;
    SYS->modif();
}

string TMess::translGet( const string &ibase )
{
    string base = ibase;
    //Getting the real base for the system prestored messages
    if(TSYS::strParse(base,0,string(1,0)) != base && TSYS::strParse(base,2,string(1,0)).empty()) base = Mess->I18N(base);

    if(!translDyn()) return base;

    string ctx = trCtx();
    return translGetLU(base, TSYS::strLine(ctx,1), TSYS::strLine(ctx,0));
}

string TMess::translGet( const string &ibase, const string &lang, const string &src )
{
    string base = TSYS::strParse(ibase, 0, string(1,0)), tVl;

    //Implementing the combained storing in "{base}\000{lang}\000{mess}"
    if(translDyn() && (tVl=TSYS::strParse(ibase,1,string(1,0))).size() &&
	    (lang.find(tVl) == 0 || (lang.empty() && tVl == langCode())))
	return TSYS::strParse(ibase,2,string(1,0));

    if(base.empty()) return base;

    string rez, tStr, cKey = base, trLang = langCode();
    bool isUAPI = (src.find("uapi:") != string::npos),
	isDUAPI = (src.find("duapi:") == 0);

    if(translDyn()) {
	if(lang.size() >= 2) trLang = lang.substr(0,2);
	if(trLang == langCodeBase() && !isUAPI) return base;
	cKey = trLang+"#"+cKey;
    } else if(!isUAPI) return base;

    //Requesting the cache at the first
    if((rez=translCacheGet(cKey)).size()) ;
    else if(Mess->isMessTranslable(base)) {
	bool isReqUAPI = false;

	//Checking the included sources from the index
	if(!isUAPI || isDUAPI) {
	    MtxAlloc res(trMessIdxRes, true);
	    map<string, map<string,string> >::iterator im = trMessIdx.find(base);
	    if(im != trMessIdx.end()) {
		map<string,string> strgs = im->second;
		res.unlock();

		TConfig req;
		for(map<string,string>::iterator is = strgs.begin(); rez.empty() && is != strgs.end(); ++is) {
		    string trSrc = TSYS::strParse(is->first,0,"#"), trFld = TSYS::strParse(is->first,1,"#"), reqFld;
		    bool isCfg = false;
		    //  Source is config file or included DB
		    if((isCfg=(trSrc.find("cfg:")==0)) || trSrc.find("db:") == 0) {
			reqFld = translFld(trLang, trFld, isCfg);
			//  Need DB structure prepare
			req.elem().fldClear();
			req.elem().fldAdd(new TFld(trFld.c_str(),"",TFld::String,0));
			req.elem().fldAdd(new TFld(reqFld.c_str(),"",TFld::String,0));
			req.cfg(trFld).setReqKey(true);
			req.cfg(trFld).setS(base);

			//  Get from config file or DB source
			for(int inst = 0; rez.empty(); inst++) {
			    bool seekRez = isCfg ? TBDS::dataSeek("", trSrc.substr(4), inst, req, TBDS::UseCache)
					    : TBDS::dataSeek(trSrc.substr(3), "", inst, req, TBDS::UseCache);
			    if(!seekRez) break;
			    rez = req.cfg(reqFld).getS();
			    isReqUAPI = true;
			}
		    }
		}
	    }
	    res.unlock();

	    translCacheSet(cKey, rez);
	}
	//Requesting the data source directly
	if(isUAPI && !isReqUAPI) {	//Checking/Getting/Placing from user API translations table
	    string srcAddrs = src.substr(isDUAPI?6:5), tStrVl;
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
		    req.elem().fldAdd(new TFld("base","",TFld::String,TCfg::Key,"1000"));
		    req.elem().fldAdd(new TFld(tStrVl.c_str(),"",TFld::String,0));
		    req.cfg("base").setS(base);
		}
		if(TBDS::dataGet(*iA+"." mess_TrUApiTbl,"/" mess_TrUApiTbl,req,TBDS::NoException)) {
		    rez = req.cfg(tStrVl).getS();
		    break;
		}
		// Create new record in the translation table of the data source
		else if((iA+1) == addrs.rend() /*&& langCodeBase().size()*/) {
		    //  Removeing the translation column for the SINGLE translation mode and the base language
		    if(!langCodeBase().size() || trLang == langCodeBase())
			req.elem().fldDel(req.elem().fldId(tStrVl.c_str()));
		    //  Trying the translation record presence in whole and creating when it missing
		    if(!TBDS::dataGet(*iA+"." mess_TrUApiTbl,"/" mess_TrUApiTbl,req,TBDS::NoException)) {
			TBDS::dataSet(*iA+"." mess_TrUApiTbl, "/" mess_TrUApiTbl, req, TBDS::NoException);
			//  Early register the new message in the translation index after the real appending
			if(translDyn()) Mess->translReg(base, (isCfg?"cfg:/":"db:"+*iA+".")+mess_TrUApiTbl+"#base");
		    }
		}
	    }
	    translCacheSet(cKey, rez);
	}
    }

    return rez.empty() ? base : rez;
}

string TMess::translGetU( const string &base, const string &user, const string &src )
{
    if(!translDyn() && src.empty()) return base;

    return translGet(base, langCode(user), src);
}

string TMess::translGetLU( const string &base, const string &lang, const string &user, const string &src )
{
    if(lang.size()) return translGet(base, lang, src);
    return translGetU(base, user, src);
}

string TMess::translSet( const string &ibase, const string &mess )
{
    //Getting the real base for the system prestored messages
    string base = ibase;
    if(TSYS::strParse(base,0,string(1,0)) != base && TSYS::strParse(base,2,string(1,0)).empty()) base = Mess->I18N(base);

    if(!translDyn()) return mess;

    string ctx = trCtx();
    return translSetLU(base, TSYS::strLine(ctx,1), TSYS::strLine(ctx,0), mess);
}

string TMess::translSet( const string &base, const string &lang, const string &mess, bool *needReload, const string &srcFltr )
{
    string trLang = langCode();
    if(lang.size() >= 2)	trLang = lang.substr(0, 2);
    else if(trLang.empty())	trLang = langCode();

    if(!needReload) {
	if(!translDyn() || lang == langCodeBase() /*|| base.empty()*/) return mess;	//!!!! base.empty() can be for new values

	//Implementing the combained storing in "{base}\000{lang}\000{mess}"
	return TSYS::strParse(base,0,string(1,0)) + string(1,0) + trLang + string(1,0) + mess;
    }

    if(base.empty() && mess.size()) trLang = langCodeBase();
    bool chBase = (trLang == langCodeBase());

    MtxAlloc res(trMessIdxRes, true);
    map<string,string> mI = trMessIdx[base];
    res.unlock();

    for(map<string,string>::iterator is = mI.begin(); is != mI.end(); ++is) {
	if(srcFltr.size() && (is->first+"#"+is->second).find(srcFltr) == string::npos)	continue;

	string trSrc = TSYS::strParse(is->first,0,"#"), setFld = TSYS::strParse(is->first,1,"#");

	TConfig req;
	req.setNoTransl(true);
	bool setRes = false, isCfg = false;
	//  Source is config file or included DB
	if((isCfg=(trSrc.find("cfg:")==0)) || trSrc.find("db:") == 0) {
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
	    res.lock();
	    if(needReload) *needReload = trMessIdx[mess].size();
	    trMessIdx[mess][is->first] = is->second;
	    res.unlock();
	}
    }
    if(chBase) { res.lock(); trMessIdx.erase(base); res.unlock(); }
    translCacheLimits(0, "");

    return chBase ? mess : base;
}

string TMess::translSetU( const string &base, const string &user, const string &mess, bool *needReload )
{
    if(!translDyn() && !needReload) return mess;

    return translSet(base, langCode(user), mess, needReload);
}

string TMess::translSetLU( const string &base, const string &lang, const string &user, const string &mess, bool *needReload )
{
    if(lang.size()) return translSet(base, lang, mess, needReload);
    return translSetU(base, user, mess, needReload);
}

void TMess::translReg( const string &mess, const string &src, const string &prms )
{
    if(!translEnMan()) return;

    MtxAlloc res(trMessIdxRes);

    //Service request of loading the translation index
    if(src.find("uapi:") == 0) {
	vector<string> ls;
	if(src.size() > 5) ls.push_back(src.substr(5));
	else { TBDS::dbList(ls); ls.push_back("<cfg>"); }

	TConfig req;
	req.elem().fldAdd(new TFld("base","Base",TFld::String,TCfg::Key,"1000"));
	for(unsigned iL = 0; iL < ls.size(); iL++)
	    if(ls[iL] == DB_CFG)
		for(int io_cnt = 0; TBDS::dataSeek("","/" mess_TrUApiTbl,io_cnt++,req,TBDS::UseCache); ) {
		    res.lock();
		    trMessIdx[req.cfg("base").getS()]["cfg:/" mess_TrUApiTbl] = prms;
		    res.unlock();
		}
	    else
		for(int io_cnt = 0; TBDS::dataSeek(ls[iL]+"." mess_TrUApiTbl,"",io_cnt++,req,TBDS::UseCache); ) {
		    res.lock();
		    trMessIdx[req.cfg("base").getS()]["db:"+ls[iL]+"." mess_TrUApiTbl "#base"] = prms;
		    res.unlock();
		}
    }
    //Same translations registration
    else {
	if(sTrm(mess).empty() || !isMessTranslable(mess)) return;
	res.lock(); trMessIdx[mess][src] = prms; res.unlock();
    }
}

string TMess::translCacheGet( const string &key, bool *ok )
{
    trMessCacheRes.lock();
    string rez;
    map<string,CacheEl>::iterator itr = trMessCache.find(key);
    if(itr != trMessCache.end()) {
	itr->second.tm = SYS->sysTm();
	rez = itr->second.val;
	if(ok) *ok = true;
    } else if(ok) *ok = false;
    trMessCacheRes.unlock();

    return rez;
}

void TMess::translCacheSet( const string &key, const string &val )
{
    trMessCacheRes.lock();
    trMessCache[key] = CacheEl(val, SYS->sysTm());
    trMessCacheRes.unlock();

    translCacheLimits();
}

void TMess::translCacheLimits( time_t tmLim, const char *clrCat )
{
    if(trMessCache.empty()) return;

    trMessCacheRes.lock();

    //Time limit and clearings
    if(tmLim || clrCat) {
	time_t curTm = SYS->sysTm();
	for(map<string,CacheEl>::iterator cEl = trMessCache.begin(); cEl != trMessCache.end(); )
	    if((tmLim && (curTm-cEl->second.tm) > tmLim) || (clrCat && TSYS::strParse(cEl->first,1,string(1,0)) == clrCat))
		trMessCache.erase(cEl++);
	    else ++cEl;
    }

    //Size limit
    if(trMessCache.size() > 10*(limCacheIts_N+limCacheIts_N/10)) {
	vector< pair<time_t,string> > sortQueue;
	for(map<string,CacheEl>::iterator itr = trMessCache.begin(); itr != trMessCache.end(); ++itr)
	    sortQueue.push_back(pair<time_t,string>(itr->second.tm,itr->first));
	sort(sortQueue.begin(), sortQueue.end());
	for(unsigned iDel = 0; iDel < 10*(limCacheIts_N/10); ++iDel)
	    trMessCache.erase(sortQueue[iDel].second);
    }

    trMessCacheRes.unlock();
}

void TMess::translIdxCacheUpd( const string &base, const string &lang, const string &mess, const string &src )
{
    //printf("Updating the field '%s' translation '%s':'%s' for the base '%s'\n", src.c_str(), lang.c_str(), mess.c_str(), base.c_str());

    //Base message modification
    if(lang.empty() || lang == Mess->langCodeBase()) {
	//  1. Registering new one, the previous is empty
	if(base.empty() && mess.size())	Mess->translReg(mess, src);
	//  2. Changing available one
	//  3. Removing of available message
	else {
	    //!!!! Do not remove the previous message's source due to the source can be one for several messages-records,
	    //     or after appendig they counting
	    /*MtxAlloc res(trMessIdxRes, true);
	    map<string, map<string,string> >::iterator im = trMessIdx.find(base);
	    if(im != trMessIdx.end()) {
		im->second.erase(src);			//Remove the message source from the base
		if(im->second.empty()) {
		    trMessIdx.erase(im);		//Remove the base for empty sources
		    //   Clear the cache
		    for(map<string,CacheEl>::iterator iCach = trMessCache.begin(); iCach != trMessCache.end(); )
			if(TSYS::strParse(iCach->first,1,"#") == base) trMessCache.erase(iCach++);
			else ++iCach;
		}
	    }
	    res.unlock();*/
	    if(mess.size()) Mess->translReg(mess, src);	//Register new
	}
    }
    // Modification the translation
    else {
	MtxAlloc res(trMessCacheRes, true);
	map<string,CacheEl>::iterator iCach = trMessCache.find(lang+"#"+base);
	if(iCach != trMessCache.end()) {
	    //  1. Changing available one
	    if(!mess.empty()) iCach->second = mess;
	    //  2. Removing of available message
	    else trMessCache.erase(iCach);
	}
    }
}

bool TMess::translItSplit( const string &base, const string &srcFltr )
{
    bool isFirst = true, isRenFirst = false;
    vector<string> toRename;

    trMessIdxRes.lock();
    map<string, map<string,string> >::iterator im = trMessIdx.find(base);
    if(im != trMessIdx.end())
	for(map<string,string>::iterator imSrc = im->second.begin(); imSrc != im->second.end(); ) {
	    if(imSrc->first.find("db:") == 0 && imSrc->first.find(mess_TrUApiTbl "#base") == string::npos &&
		(srcFltr.empty() || imSrc->first.find(srcFltr) != string::npos))
	    {
		if(isFirst) isRenFirst = true;
		toRename.push_back(imSrc->first);
		if(!isFirst) { im->second.erase(imSrc++); continue; }
	    }
	    isFirst = false;
	    ++imSrc;
	}
    trMessIdxRes.unlock();

    bool needReload = false;
    for(unsigned iRen = 0, iRenCnt = 0; iRen < toRename.size(); ++iRen) {
	string trSrc = TSYS::strParse(toRename[iRen],0,"#").substr(3), trFld = TSYS::strParse(toRename[iRen],1,"#");

	AutoHD<TTable> tbl = SYS->db().at().tblOpen(trSrc);

	TConfig req;
	req.setNoTransl(true);
	tbl.at().fieldStruct(req);
	req.cfgViewAll(false);
	req.cfg(trFld).setReqKey(true);
	req.cfg(trFld).setS(base, true);

	// Get from config file or DB source
	for(int iPos = 0; tbl.at().fieldSeek(iPos,req,__FUNCTION__); ++iPos) {
	    if(isRenFirst && iRenCnt == 0) { isRenFirst = false; continue; }
	    req.cfg(trFld).setReqKey(false);
	    req.cfg(trFld).setS(base+i2s(iRenCnt+2), TCfg::ForceUse);
	    tbl.at().fieldSet(req);

	    translReg(base+i2s(iRenCnt+2), toRename[iRen]);
	    iRenCnt++;
	    needReload = true;

	    //  For continue the checking
	    req.cfg(trFld).setReqKey(true);
	    req.cfg(trFld).setS(base, true);
	}
    }

    return needReload;
}

bool TMess::translItRemTrs( const string &base, const string &srcFltr )
{
    vector<string> toRemove;

    trMessIdxRes.lock();
    map<string, map<string,string> >::iterator im = trMessIdx.find(base);
    if(im != trMessIdx.end()) {
	for(map<string,string>::iterator imSrc = im->second.begin(); imSrc != im->second.end(); ) {
	    if(imSrc->first.find(mess_TrUApiTbl "#base") != string::npos && (srcFltr.empty() || imSrc->first.find(srcFltr) != string::npos)) {
		toRemove.push_back(imSrc->first);
		im->second.erase(imSrc++);
		continue;
	    }
	    ++imSrc;
	}
	if(im->second.empty()) trMessIdx.erase(im);
    }
    trMessIdxRes.unlock();

    for(unsigned iRem = 0; iRem < toRemove.size(); ++iRem) {
	string trSrc = TSYS::strParse(toRemove[iRem],0,"#"), trFld = TSYS::strParse(toRemove[iRem],1,"#");

	TConfig req;
	req.setNoTransl(true);
	req.elem().fldAdd(new TFld(trFld.c_str(),"",TFld::String,0));
	req.cfg(trFld).setReqKey(true);
	req.cfg(trFld).setS(base, true);

	if(trSrc.find("cfg:") == 0) TBDS::dataDel("", trSrc.substr(4), req);
	else TBDS::dataDel(trSrc.substr(3), "", req);
    }

    return (bool)toRemove.size();
}

string TMess::trCtx( const string &user_lang, bool *hold )
{
    string rez;

    dtRes.lock();
    pthread_t pthr = pthread_self();
    if(user_lang == mess_TrModifMark) rez = trCtxs[pthr];
    else if(user_lang.empty()) trCtxs.erase(pthr);
    else if(!hold || trCtxs[pthr].empty()) {
	trCtxs[pthr] = rez = user_lang;
	if(hold) *hold = true;
    } else if(hold) *hold = false;
    dtRes.unlock();

    return rez;
}

string TMess::lang( )
{
    char *lng = NULL;
    if(((lng=getenv("LANGUAGE")) && strlen(lng)) ||
	    ((lng=getenv("LC_MESSAGES")) && strlen(lng)) ||
	    ((lng=getenv("LANG")) && strlen(lng)))
	return lng;
    else return "C";
}

string TMess::langCode( const string &user, bool onlyUser )
{
    if(user.size()) {
	bool ok = false;
	string toLang = translCacheGet(user+string(1,0)+"user", &ok);
	if(!ok && !SYS->stopSignal() && SYS->security().at().usrPresent(user) &&
		    (toLang=SYS->security().at().usrAt(user).at().lang()).size())
	    translCacheSet(user+string(1,0)+"user", toLang);
	if(onlyUser || toLang.size())
	    return (toLang.size() > 2) ? toLang.substr(0,2) : toLang;
    }

    return mLangCode;
}

bool TMess::messLevelTest( int8_t condLev, int8_t messLev )
{
    condLev = vmax(Debug, vmin(Emerg,abs(condLev)));
    messLev = abs(messLev);

    return (((messLev <= Emerg)?messLev:messLev/10) >= condLev);
}

string TMess::selDebCats( )
{
    string rez;

    MtxAlloc res(dbgRes, true);
    for(unsigned iSdc = 0; iSdc < selectDebugCats.size(); iSdc++)
	rez += selectDebugCats[iSdc]+";";
    res.unlock();

    return rez;
}

void TMess::setSelDebCats( const string &vl )
{
    MtxAlloc res(dbgRes, true);
    debugCats.clear();
    selectDebugCats.clear();

    string curCat;
    for(int off = 0; (curCat=TSYS::strParse(vl,0,";",&off)).size(); ) {
	debugCats[curCat] = true;
	selectDebugCats.push_back(curCat);
    }

    SYS->sysModifFlgs |= TSYS::MDF_DBG;
    SYS->modif();
}

void TMess::setLang( const string &lng )
{
    char *prvLng = NULL;
    if((prvLng=getenv("LANGUAGE")) && strlen(prvLng)) setenv("LANGUAGE", lng.c_str(), 1);
    //else setenv("LC_MESSAGES", lng.c_str(), 1);
    else setenv("LANG", lng.c_str(), 1);	//!!!! Can be used further for the miss environments force set
    setlocale(LC_ALL, "");
#if defined(__ANDROID__)
    setlocale(LC_CTYPE, "C.UTF-8");	//For original NDK r13 and Android >= 5
#endif

#ifdef HAVE_LANGINFO_H
    IOCharSet = nl_langinfo(CODESET);
#endif

    string tLng = lang();
    mLangCode = tLng;
    if(mLangCode.size() < 2 || mLangCode.getVal() == "POSIX" || mLangCode.getVal() == "C") mLangCode = "en";
    else mLangCode = mLangCode.getVal().substr(0, 2);
    mIsUTF8 = (IOCharSet == "UTF-8" || IOCharSet == "UTF8" || IOCharSet == "utf8");

    mConvCode = !(tLng == "C" || (mLangCode.getVal() == "en" && (IOCharSet == "ISO-8859-1" || IOCharSet == "ANSI_X3.4-1968" || IOCharSet == "ASCII" || IOCharSet == "US-ASCII")));

    SYS->sysModifFlgs |= TSYS::MDF_LANG;
    SYS->modif();
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

    iconv_t hd = iconv_open(toCH.c_str(), fromCH.c_str());
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
    if(iconv_close(hd) != 0)
	mess_warning("IConv", _("Closing the iconv handler error '%s (%d)'!"), strerror(errno), errno);

    //Deadlock possible on the error message print
    //if(chwrcnt)	mess_err("IConv", _("Error converting %d symbols from '%s' to '%s' for the message part: '%s'(%d)"),
    //		    chwrcnt, fromCH.c_str(), toCH.c_str(), mess.substr(0,20).c_str(), mess.size());

    return buf;
#else
    return mess;
#endif
}

string TMess::I18N( const string &imess, const char *mLang, const char *d_name )
{
#ifdef HAVE_LIBINTL_H
    if(imess.empty() || (TSYS::strParse(imess,0,string(1,0)).empty() && TSYS::strParse(imess,1,string(1,0)).empty()))
	return "";
    if(TSYS::strParse(imess,2,string(1,0)).size()) return imess;	//That is a dynamic data changing message - pass

    int doff = 0;
    string  ctx,
	    mess = TSYS::strParse(imess, 0, string(1,0), &doff),
	    dir = d_name ? d_name : TSYS::strParse(imess,0,string(1,0),&doff).c_str();

    //Presaving the base message with the directory name "{base}\000{cat}" for real translation at reading the saved one
    if(mLang && strcmp(mLang,mess_PreSave) == 0 &&
	    ((!translDyn() && mess.size() == imess.size()) || (translDyn() && (ctx=trCtx()).empty())))
	return mess + string(1,0) + dir;

    if(translDyn()) {
	//Obtaining the message language whether directly or from the user, and from the cache before
	string rez, toLang;
	if(mLang && strlen(mLang) && strcmp(mLang,mess_PreSave) != 0) toLang = mLang;
	else {
	    string toUser;
	    if(ctx.empty()) ctx = trCtx();
	    if((toLang=TSYS::strLine(ctx,1)).empty() && (toUser=TSYS::strLine(ctx,0)).size())
		toLang = langCode(toUser, true);
	}

	//Obtaining the message translation from the cache
	if((rez=translCacheGet(toLang+"#"+mess+string(1,0)+"sys")).size()) return rez;

	//Obtaining the message translation from the translation DB
	getMessRes.lock();
	if(toLang.empty()) {
	    setenv("LANGUAGE", "", 1);
	    //setenv("LC_MESSAGES", "", 1);
	    ++_nl_msg_cat_cntr;	//Make change known.
	    getMessLng = "";
	}
	else if(getMessLng != toLang) {
	    setenv("LANGUAGE", toLang.c_str(), 1);
	    //setenv("LC_MESSAGES", toLang.c_str(), 1);
	    ++_nl_msg_cat_cntr;	//Make change known.
	    getMessLng = toLang;
	}

	rez = dgettext((dir.size()?dir.c_str():NULL), mess.c_str());
	getMessRes.unlock();

	translCacheSet(toLang+"#"+mess+string(1,0)+"sys", rez);

	return rez;
    }
    else return dgettext(dir.size()?dir.c_str():NULL, mess.c_str());
#else
    return imess;
#endif
}

void TMess::load( )
{
    //Load params from command line
    string argVl;
    if((argVl=SYS->cmdOpt("lang")).size()) setLang(argVl);
    if((argVl=SYS->cmdOpt("messLev")).size()) {
	int i = s2i(argVl);
	if(i >= Debug && i <= Emerg) setMessLevel(i);
    }
    if((argVl=SYS->cmdOpt("log")).size()) setLogDirect(s2i(argVl));

    //Load params config-file
    setMessLevel(s2i(TBDS::genPrmGet(SYS->nodePath()+"MessLev",i2s(messLevel()))));
    setSelDebCats(TBDS::genPrmGet(SYS->nodePath()+"SelDebCats",selDebCats()));
    setLogDirect(s2i(TBDS::genPrmGet(SYS->nodePath()+"LogTarget",i2s(logDirect()))));
    setLang(TBDS::genPrmGet(SYS->nodePath()+"Lang",lang(),"root",TBDS::OnlyCfg));
    if((argVl=TBDS::genPrmGet(SYS->nodePath()+"LangBase")).size() ||
	    (argVl=TBDS::genPrmGet(SYS->nodePath()+"Lang2CodeBase")).size())	//????[v1.0] Remove, loading the deprecated parameter
	setLangBase(argVl);
    setTranslDyn(s2i(TBDS::genPrmGet(SYS->nodePath()+"TranslDyn",i2s(translDyn()))), false);
    setTranslEnMan(translDyn() || s2i(TBDS::genPrmGet(SYS->nodePath()+"TranslEnMan",i2s(translEnMan()))), true);

    SYS->sysModifFlgs = TSYS::MDF_NONE;
}

void TMess::unload( )
{
    dbgRes.lock(); debugCats.clear(); selectDebugCats.clear(); dbgRes.unlock();

    mTranslLangs = "";
    trMessIdxRes.lock(); trMessIdx.clear(); trMessIdxRes.unlock();
    trMessCacheRes.lock(); trMessCache.clear(); trMessCacheRes.unlock();

    IOCharSet = "UTF-8", mMessLevel = Info, mLogDir = DIR_STDOUT|DIR_ARCHIVE;
    mConvCode = mIsUTF8 = true, mTranslDyn = mTranslDynPlan = mTranslEnMan = mTranslSet = false;
}

void TMess::save( )
{
    if(SYS->sysModifFlgs&TSYS::MDF_MESS) {
	TBDS::genPrmSet(SYS->nodePath()+"MessLev",i2s(messLevel()),"root",TBDS::OnlyCfg);
	TBDS::genPrmSet(SYS->nodePath()+"LogTarget",i2s(logDirect()),"root",TBDS::OnlyCfg);
    }
    if(SYS->sysModifFlgs&TSYS::MDF_DBG) TBDS::genPrmSet(SYS->nodePath()+"SelDebCats",selDebCats(),"root",TBDS::OnlyCfg);
    if(SYS->sysModifFlgs&TSYS::MDF_LANG) TBDS::genPrmSet(SYS->nodePath()+"Lang",lang(),"root",TBDS::OnlyCfg);
    if(SYS->sysModifFlgs&TSYS::MDF_TR) {
	TBDS::genPrmSet(SYS->nodePath()+"LangBase",langBase(),"root",TBDS::OnlyCfg);
	TBDS::genPrmSet(SYS->nodePath()+"Lang2CodeBase","","root",TBDS::OnlyCfg);	//????[v1.0] Remove, cleaning up the deprecated parameter
	TBDS::genPrmSet(SYS->nodePath()+"TranslDyn",i2s(translDyn(true)),"root",TBDS::OnlyCfg);
	TBDS::genPrmSet(SYS->nodePath()+"TranslEnMan",i2s(translEnMan()),"root",TBDS::OnlyCfg);
    }
}

string TMess::labStor( bool nogen )
{
    return string(_("Storage address in the format \"{DB module}.{DB name}\".")) +
	(nogen?string(""):string("\n")+_("Set '*.*' to use the Generic Storage."));
}

string TMess::labStorRemGenStor( )
{
    return TSYS::strMess(_("Please note that removing from the Generic Storage (*.*) "
	"will cause the data removing both from the Configuration File (<cfg>) and the Work DB (%s)!"),SYS->workDB().c_str());
}

string TMess::labSecCRON( )
{
    return _("Schedule wrotes in seconds periodic form or in the standard CRON form.\n"
	     "The seconds periodic form is one real number (1.5, 1e-3).\n"
	     "CRON is the standard form \"* * * * *\".\n"
	     "  Where items by the order:\n"
	     "    - minutes (0-59);\n"
	     "    - hours (0-23);\n"
	     "    - days (1-31);\n"
	     "    - month (1-12);\n"
	     "    - week day (0[Sunday]-6).\n"
	     "  Where the item variants:\n"
	     "    - \"*\" - any value;\n"
	     "    - \"1,2,3\" - list of the allowed values;\n"
	     "    - \"1-5\" - raw range of the allowed values;\n"
	     "    - \"*/2\" - divider to the all allowed values range.\n"
	     "Examples:\n"
	     "  \"1e-3\" - call with a period of one millisecond;\n"
	     "  \"* * * * *\" - each minute;\n"
	     "  \"10 23 * * *\" - only at 23 hour and 10 minute for any day and month;\n"
	     "  \"*/2 * * * *\" - for minutes: 0,2,4,...,56,58;\n"
	     "  \"* 2-4 * * *\" - for any minutes in hours from 2 to 4(include).");
}

string TMess::labSecCRONsel( )	{ return "1;1e-3;* * * * *;10 * * * *;10-20 2 */2 * *"; }

string TMess::labTaskPrior( )
{
    return _("Task priority level (-1...199), where:\n"
	     "  -1        - lowest priority batch policy;\n"
	     "  0         - standard userspace priority;\n"
	     "  1...99    - realtime priority level (round-robin), often allowed only for \"root\";\n"
	     "  100...199 - realtime priority level (FIFO), often allowed only for \"root\".");
}

string TMess::labStdOutTrs( )
{
    return _("Address format of the output transport using the unified connection:\n"
	     "  \"{TrModule}.[out_]{TrID}[:{TrAddr}]\" - typical output with automatic creation <TrID> at it missing and providing <TrAddr>;\n"
	     "  \"{TrModule}.in_{TrID}:{RemConId}\" - initiative input with the connection identifier in <RemConId>.");
}

string TMess::labMessCat( )
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
