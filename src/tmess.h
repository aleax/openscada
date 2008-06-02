
//OpenSCADA system file: tmess.h
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#include <stdio.h>

#include <string>
#include <vector>
#include <exception>

#define _(mess) Mess->I18N(mess)
#define message(cat,lev,fmt,args...) Mess->put(cat,lev,fmt,##args)
#define mess_debug(cat,fmt,args...) Mess->put(cat,TMess::Debug,fmt,##args)
#define mess_info(cat,fmt,args...) Mess->put(cat,TMess::Info,fmt,##args)
#define mess_note(cat,fmt,args...) Mess->put(cat,TMess::Notice,fmt,##args)
#define mess_warning(cat,fmt,args...) Mess->put(cat,TMess::Warning,fmt,##args)
#define mess_err(cat,fmt,args...) Mess->put(cat,TMess::Error,fmt,##args)
#define mess_crit(cat,fmt,args...) Mess->put(cat,TMess::Crit,fmt,##args)
#define mess_alert(cat,fmt,args...) Mess->put(cat,TMess::Alert,fmt,##args)
#define mess_emerg(cat,fmt,args...) Mess->put(cat,TMess::Emerg,fmt,##args)

using std::string;
using std::vector;
using std::exception;

//*************************************************
//* TMess                                         *
//*************************************************
class TMess
{
    public:
	//Data
	enum Type { Debug, Info, Notice, Warning, Error, Crit, Alert, Emerg };

	class SRec
	{
	    public:
		SRec() : time(0), categ(""), level(TMess::Debug), mess("") { }
		SRec(time_t itm, const string &icat, Type ilev, const string &imess) :
		    time(itm), categ(icat), level(ilev), mess(imess) { }
		
		time_t time;
		string categ;
		Type   level;
		string mess;
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
	
	static bool chkPattern( const string &val, const string &patern );
	
	string lang( );
	string &charset( )    	{ return IOCharSet; }
	int logDirect( )     	{ return log_dir; }
	int messLevel( )	{ return m_mess_level; }
	
	void setLang( const string &lang );
	void setLogDirect( int dir );
	void setMessLevel( int level );
	
	void put( const char *categ, Type level, const char *fmt,  ... );
        void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> & recs, const string &category = "", Type level = Debug );
	
    private:
	//Attributes
	string IOCharSet;      	//Internal charset
	int    m_mess_level;	//Work messages level
	int    log_dir;        	//Log direction	
};

extern TMess *Mess;

#endif // TMESSAGE_H

