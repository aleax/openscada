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

//Message levels
#define MESS_DEBUG   0
#define MESS_INFO    1
#define MESS_NOTICE  2
#define MESS_WARNING 3
#define MESS_ERR     4
#define MESS_CRIT    5
#define MESS_ALLERT  6
#define MESS_EMERG   7

using std::string;
using std::vector;
using std::exception;

class TMessage
{
    /** Public methods: */
    public:
	struct SRec
	{
	    time_t time;
	    string categ;
	    char   level;
	    string mess;
	};    
    
	TMessage(  );
	~TMessage(  );

	string Sconv( const string &fromCH, const string &toCH, const string &mess);
	string SconvIn( const string &fromCH, const string &mess)
	{ return( Sconv( fromCH, IOCharSet, mess ) ); }
	string SconvOut( const string &toCH, const string &mess)
	{ return( Sconv( IOCharSet, toCH , mess ) ); }

	static char *I18N( char *mess, char *d_name = NULL );
	static string I18Ns( const string &mess, char *d_name = NULL )
	{ return( I18N((char *)mess.c_str(), d_name) ); }
	
	string lang( );
	string &charset( )    { return( IOCharSet ); }
	int d_level( )        { return( m_d_level ); }
	int log_direct( )     { return( log_dir ); }
	int mess_buf_len( )   { return( m_buf.size() ); }
	
	void lang( const string &lang );
	void d_level(int level)        { m_d_level = level; }
	void log_direct(int dir)       { log_dir   = dir; }
	void mess_buf_len(int len);
	
	void put( const string &categ, int level, char *fmt,  ... );
	void put_s( const string &categ, int level, const string &mess );
        void get( time_t b_tm, time_t e_tm, vector<TMessage::SRec> & recs, const string &category = "", char level = 0 );
    
	void updateOpt();
	// Update comand line option
	void checkCommandLine( );

    /**Attributes: */
    private:
	string IOCharSet;      //Internal charset
	int    m_d_level;      //Debug level
	int    log_dir;        //Log direction
	
	int    m_res;          //Mess resource
	int    head_buf;       //Head buffer
	vector<TMessage::SRec> m_buf; //Messages buffer

	static const char *o_name; 
};

extern TMessage *Mess;

#endif // TMESSAGE_H
