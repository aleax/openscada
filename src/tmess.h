
//OpenSCADA system file: tmess.h
/***************************************************************************
 *   Copyright (C) 2003-2013 by Roman Savochenko                           *
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

#ifndef TMESSAGE_H
#define TMESSAGE_H

#include <stdint.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <map>
#include <exception>

#include "resalloc.h"

#define _(mess) Mess->I18N(mess)
#define FTM(rec) ((int64_t)rec.time*1000000 + rec.utime)

#define mess_lev( )			Mess->messLevel()
//> Limited to mess_lev() messages
#define message(cat,lev,fmt,args...)	Mess->put(cat,lev,fmt,##args)
#define mess_debug(cat,fmt,args...)	Mess->put(cat,TMess::Debug,fmt,##args)
#define mess_info(cat,fmt,args...)	Mess->put(cat,TMess::Info,fmt,##args)
#define mess_note(cat,fmt,args...)	Mess->put(cat,TMess::Notice,fmt,##args)
#define mess_warning(cat,fmt,args...)	Mess->put(cat,TMess::Warning,fmt,##args)
#define mess_err(cat,fmt,args...)	Mess->put(cat,TMess::Error,fmt,##args)
#define mess_crit(cat,fmt,args...)	Mess->put(cat,TMess::Crit,fmt,##args)
#define mess_alert(cat,fmt,args...)	Mess->put(cat,TMess::Alert,fmt,##args)
#define mess_emerg(cat,fmt,args...)	Mess->put(cat,TMess::Emerg,fmt,##args)
//> Unlimited to mess_lev() messages
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
	enum Type { Debug, Info, Notice, Warning, Error, Crit, Alert, Emerg };
	enum Direct { DIR_SYSLOG = 0x1, DIR_STDOUT = 0x2, DIR_STDERR = 0x4, DIR_ARCHIVE = 0x8 };

	class SRec
	{
	    public:
		SRec( ) : time(0), categ(""), level(TMess::Debug), mess("") { }
		SRec( time_t itm, int iutime, const string &icat, int8_t ilev, const string &imess ) :
		    time(itm), utime(iutime), categ(icat), level(ilev), mess(imess) { }

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
	void save( );

	string codeConv( const string &fromCH, const string &toCH, const string &mess);
	string codeConvIn( const string &fromCH, const string &mess)
	{ return( codeConv( fromCH, IOCharSet, mess ) ); }
	string codeConvOut( const string &toCH, const string &mess)
	{ return( codeConv( IOCharSet, toCH , mess ) ); }

	static const char *I18N( const char *mess, const char *d_name = NULL );
	static string I18Ns( const string &mess, const char *d_name = NULL )
	{ return I18N((char *)mess.c_str(), d_name); }

	string lang( );
	string lang2Code( )	{ return mLang2Code; }
	string lang2CodeBase( )	{ return mLang2CodeBase; }
	string &charset( )	{ return IOCharSet; }
	int logDirect( )	{ return mLogDir; }
	int messLevel( )	{ return mMessLevel; }
	string selDebCats( );
	bool isUTF8( )		{ return mIsUTF8; }

	void setLang( const string &lang, bool init = false );
	void setLang2CodeBase( const string &vl );
	void setLogDirect( int dir );
	void setMessLevel( int level );
	void setSelDebCats( const string &vl );

	void put( const char *categ, int8_t level, const char *fmt,  ... );
	void put_( const char *categ, int8_t level, const char *fmt,  ... );
	void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &recs, const string &category = "", int8_t level = Debug );

	//> Often used, generic text messages
	static const char *labDB( );
	static const char *labSecCRON( );
	static const char *labSecCRONsel( );
	static const char *labTaskPrior( );

    private:
	//Methods
	void putArg( const char *categ, int8_t level, const char *fmt, va_list ap );

	//Attributes
	string	IOCharSet;		//Internal charset
	unsigned mMessLevel	:3;	//Work messages level
	unsigned mLogDir	:4;	//Log direction
	unsigned mConvCode	:1;	//Enable text code conversion
	unsigned mIsUTF8	:1;

	string	mLang2CodeBase, mLang2Code;

	map<string, bool>	debugCats;
	vector<string>		selectDebugCats;

	Res	mRes;
};

extern TMess *Mess;

}

#endif // TMESSAGE_H
