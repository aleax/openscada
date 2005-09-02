/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

using std::string;
using std::vector;
using std::exception;

class TMess
{
    /** Public methods: */
    public:
	enum Type { Debug, Info, Notice, Warning, Error, Crit, Allert, Emerg };
    
	struct SRec
	{
	    time_t time;
	    string categ;
	    Type   level;
	    string mess;
	};    
    
	TMess(  );
	~TMess(  );
	
	void load();

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
	int messBufLen( )   	{ return m_buf.size(); }
	int messLevel( )	{ return m_mess_level; }
	
	void lang( const string &lang );
	void logDirect(int dir)       	{ log_dir   = dir; }
	void messBufLen(int len);
	void messLevel(int level)	{ m_mess_level = level; }
	
	void put( const char *categ, Type level, const char *fmt,  ... );
	//void put_s( const string &categ, Type level, const string &mess );
        void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> & recs, const string &category = "", Type level = Debug );
	
    /**Attributes: */
    private:
	string IOCharSet;      	//Internal charset
	int    m_mess_level;	//Debug level
	int    log_dir;        	//Log direction
	
	int    m_res;          	//Mess resource
	int    head_buf;       	//Head buffer
	vector<TMess::SRec> m_buf; //Messages buffer
};

extern TMess *Mess;

#endif // TMESSAGE_H

