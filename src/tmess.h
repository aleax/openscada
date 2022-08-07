
//OpenSCADA file: tmess.h
/***************************************************************************
 *   Copyright (C) 2003-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef TMESSAGE_H
#define TMESSAGE_H

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include <string>
#include <vector>
#include <map>
#include <exception>

#include "resalloc.h"

#define mess_PreSave	"<PSV>"
#define mess_TrModifMark "<!>"
#define mess_TrUApiTbl	"Trs"

//System translation
#define _(mess) Mess->I18N(mess).c_str()
//... with the BASE value presaving for next retranslation
#define trS(mess) Mess->I18N(mess, mess_PreSave)

//Data translation
#define trD(base) Mess->translGet(base)
#define trD_L(base,lng) Mess->translGet(base, lng)
#define trD_U(base,usr) Mess->translGetU(base, usr)
#define trD_LU(base,lng,usr) Mess->translGetLU(base, lng, usr)
#define trDSet(base,mess) Mess->translSet(base, mess)
#define trDSet_L(base,lng,mess) Mess->translSet(base, lng, mess)
#define trDSet_U(base,usr,mess) Mess->translSetU(base, usr, mess)
#define trDSet_LU(base,lng,usr,mess) Mess->translSetLU(base, lng, usr, mess)
#define FTM(rec) ((int64_t)rec.time*1000000 + rec.utime)
#define FTM2(tm, utm) ((int64_t)tm*1000000 + utm)

#define mess_lev( )			Mess->messLevel()
//Limited to mess_lev() messages
#define message(cat,lev,fmt,args...)	Mess->put(cat,lev,fmt,##args)
#define mess_debug(cat,fmt,args...)	Mess->put(cat,TMess::Debug,fmt,##args)
#define mess_info(cat,fmt,args...)	Mess->put(cat,TMess::Info,fmt,##args)
#define mess_note(cat,fmt,args...)	Mess->put(cat,TMess::Notice,fmt,##args)
#define mess_warning(cat,fmt,args...)	Mess->put(cat,TMess::Warning,fmt,##args)
#define mess_err(cat,fmt,args...)	Mess->put(cat,TMess::Error,fmt,##args)
#define mess_crit(cat,fmt,args...)	Mess->put(cat,TMess::Crit,fmt,##args)
#define mess_alert(cat,fmt,args...)	Mess->put(cat,TMess::Alert,fmt,##args)
#define mess_emerg(cat,fmt,args...)	Mess->put(cat,TMess::Emerg,fmt,##args)
//Unlimited to mess_lev() messages and for not periodic notifications mostly
#define message_(cat,lev,fmt,args...)	Mess->put_(cat,lev,fmt,##args)
#define mess_debug_(cat,fmt,args...)	Mess->put_(cat,TMess::Debug,fmt,##args)
#define mess_info_(cat,fmt,args...)	Mess->put_(cat,TMess::Info,fmt,##args)
#define mess_note_(cat,fmt,args...)	Mess->put_(cat,TMess::Notice,fmt,##args)
#define mess_warning_(cat,fmt,args...)	Mess->put_(cat,TMess::Warning,fmt,##args)
#define mess_err_(cat,fmt,args...)	Mess->put_(cat,TMess::Error,fmt,##args)
#define mess_crit_(cat,fmt,args...)	Mess->put_(cat,TMess::Crit,fmt,##args)
#define mess_alert_(cat,fmt,args...)	Mess->put_(cat,TMess::Alert,fmt,##args)
#define mess_emerg_(cat,fmt,args...)	Mess->put_(cat,TMess::Emerg,fmt,##args)

using std::string;
using std::vector;
using std::map;
using std::exception;

namespace OSCADA
{

//*************************************************
//* TMess                                         *
//*************************************************
class TMess
{
    friend class TSYS;

    public:
	//Data
	enum Type { Debug, Info, Notice, Warning, Error, Crit, Alert, Emerg, MaxLev = 79 };
	enum Direct { DIR_SYSLOG = 0x1, DIR_STDOUT = 0x2, DIR_STDERR = 0x4, DIR_ARCHIVE = 0x8 };

	class SRec {
	    public:
		SRec( ) : time(0), categ(""), level(TMess::Debug), mess("") { }
		SRec( time_t itm, int iutime = 0, const string &icat = "", int8_t ilev = 0, const string &imess = "" ) :
		    time(itm), utime(iutime), categ(icat), level(ilev), mess(imess) { }

		bool operator==( SRec &vl ) { return (time == vl.time && utime == vl.utime && categ == vl.categ && level == vl.level && mess == vl.mess); }

		time_t	time;
		int	utime;
		string	categ;
		int8_t	level;
		string	mess;
	};

	//Methods
	TMess( );
	~TMess( );

	void load( );
	void unload( );
	void save( );

	string codeConv( const string &fromCH, const string &toCH, const string &mess );
	string codeConvIn( const string &fromCH, const string &mess )	{ return codeConv(fromCH, IOCharSet, mess); }
	string codeConvOut( const string &toCH, const string &mess )	{ return codeConv(IOCharSet, toCH, mess); }

	string I18N( const string &mess, const char *mLang = NULL, const char *d_name = NULL );

	string lang( );
	string langCode( const string &user = "", bool onlyUser = false );
	string &charset( )	{ return IOCharSet; }
	int logDirect( )	{ return mLogDir; }
	int messLevel( )	{ return mMessLevel; }
	static bool messLevelTest( int8_t condLev, int8_t messLev );
	string selDebCats( );
	bool isUTF8( )		{ return mIsUTF8; }

	void setLang( const string &lang );
	void setLogDirect( int dir );
	void setMessLevel( int level );
	void setSelDebCats( const string &vl );

	void put( const char *categ, int8_t level, const char *fmt,  ... );
	void put_( const char *categ, int8_t level, const char *fmt,  ... );
	void putArg( const char *categ, int8_t level, const char *fmt, va_list ap );
	void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &recs, const string &category = "", int8_t level = Debug );

	// Internal messages translations
	string langBase( )			{ return mLangBase; }
	string langCodeBase( );
	string langToLocale( const string &lang );
	bool translCfg( )			{ return langCodeBase().size() && langCode() != langCodeBase(); }
	bool translDyn( bool plan = false )	{ return plan ? mTranslDynPlan : mTranslDyn; }
	bool translEnMan( )			{ return mTranslEnMan; }
	string translLangs( )			{ return mTranslLangs; }
	string translFld( const string &lng, const string &fld, bool isCfg = false );
	bool isMessTranslable( const string &vl );
	void setLangBase( const string &vl );
	void setTranslDyn( bool val, bool plan = true );
	void setTranslEnMan( bool vl, bool passive = false );
	void setTranslLangs( const string &vl )	{ mTranslLangs = vl; }

	//  Translation request for <base>, <lang> | <user> and <src> (direct source mostly for "uapi:").
	string translGet( const string &base );				//Getting user and language from the translation context
	string translGet( const string &base, const string &lang, const string &src = "" );
	string translGetU( const string &base, const string &user, const string &src = "" );
	string translGetLU( const string &base, const string &lang, const string &user, const string &src = "" );

	//  Translation set for <base>, <lang> | <user> and <mess>. Return base or the changed.
	string translSet( const string &base, const string &mess );	//Getting user and language from the translation context
	string translSet( const string &base, const string &lang, const string &mess, bool *needReload = NULL, const string &srcFltr = "" );
	string translSetU( const string &base, const string &user, const string &mess, bool *needReload = NULL );
	string translSetLU( const string &base, const string &lang, const string &user, const string &mess, bool *needReload = NULL );

	//  Register translations. Source format:
	//    for DB: "db:{MDB}.{DB}.{TBL}#{TrFld}"
	//    for <cfg>: "cfg:{ObjPath}/{TBL}#{TrFld}"
	//    for UserAPI table: "uapi:{DB}"
	void translReg( const string &mess, const string &src, const string &prms = "" );
	string translCacheGet( const string &key, bool *ok = NULL );
	void translCacheSet( const string &key, const string &val );
	void translCacheLimits( time_t tmLim = 0, const char *clrCat = NULL );
	void translIdxCacheUpd( const string &base, const string &lang, const string &mess, const string &src );
	bool translItSplit( const string &base, const string &srcFltr = "" );
	bool translItRemTrs( const string &base, const string &srcFltr = "" );

	// Getting and registering/clearing the translation context bound to the call pthread
	string trCtx( const string &user_lang = mess_TrModifMark, bool *hold = NULL );

	// Often used, generic text messages
	static string labStor( bool nogen = false );
	static string labStorRemGenStor( );
	static string labSecCRON( );
	static string labSecCRONsel( );
	static string labTaskPrior( );
	static string labMessCat( );

	int getUTF8( const string &str, int off = 0, int32_t *symb = NULL );
	static string setUTF8( int32_t symb );

    private:
	//Data
	class CacheEl {
	    public:
		CacheEl( const string &ival, time_t itm = 0 ) : tm(itm), val(ival)	{ }
		CacheEl( ) : tm(0)	{ }
		time_t	tm;
		string	val;
	};

	//Attributes
	string	IOCharSet;		//Internal charset
	unsigned mMessLevel	:3;	//Work messages level
	unsigned mLogDir	:4;	//Log direction
	unsigned mConvCode	:1;	//Enable text code conversion
	unsigned mIsUTF8	:1;
	unsigned mTranslDyn	:1;
	unsigned mTranslDynPlan	:1;
	unsigned mTranslEnMan	:1;
	unsigned mTranslSet	:1;

	ResMtx	dtRes, dbgRes, trMessIdxRes, trMessCacheRes;

	MtxString	mLangBase, mLangCode, mTranslLangs;

	map<string, bool>	debugCats;
	vector<string>		selectDebugCats;

	map<string, map<string,string> > trMessIdx;
	map<string, CacheEl>	trMessCache;

	ResMtx	getMessRes;
	string	getMessLng;

	map<pthread_t, string>	trCtxs;
};

extern TMess *Mess;

//***********************************************************
//* Automatic translation context unlock object		    *
//***********************************************************
class TrCtxAlloc
{
    public:
	//Methods
	TrCtxAlloc( ) : mHold(false) { }
	TrCtxAlloc( const string &user_lang, bool force = true ) : mHold(false) { hold(user_lang, force); }
	~TrCtxAlloc( ) { if(mHold) Mess->trCtx(""); }

	void hold( const string &user_lang, bool force = true ) { mHold = force; Mess->trCtx(user_lang, force?NULL:&mHold); }

    private:
	//Attributes
	bool mHold;
};

}

#endif // TMESSAGE_H
