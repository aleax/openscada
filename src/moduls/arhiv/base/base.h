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

#ifndef BASE_H
#define BASE_H

#include <tmodule.h>
#include <xml.h>
#include <tarchives.h>

namespace BaseArch
{
    class TMessArch;
    
    class TFileArch
    {
	public:
 	    TFileArch( TMessArch *owner );
 	    TFileArch( const string &name, time_t beg, time_t end, TMessArch *owner); // string charset, int time_size );
 	    ~TFileArch();

	    void Attach( const string &name );
	    void put( TMess::SRec mess );
	    void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category, char level );
	    // Write changes to Archive file 
	    //  free - surely free used memory
	    void Sync( bool free = false );     

	    string &Name() { return(m_name); }
	    time_t &Begin(){ return(m_beg); }
	    time_t &End()  { return(m_end); }
	    bool   &Err()  { return(m_err); }

	    TMessArch &owner() { return *m_owner; }
	public:
    	    bool    scan;    // Archive scaned (for check deleted files)
	private:	    	
    	    string  m_name;    // name Archive file;
    	    string  m_chars;   // Archive charset;
    	    bool    m_err;     // Archive err
    	    bool    m_write;   // Archive had changed but no writed to file
	    bool    m_load;    // arhiv load to m_node
	    time_t  m_acces;   // last of time acces to Archive file
    	    time_t  m_beg;     // begin Archive file;
    	    time_t  m_end;     // end Archive file;
    	    XMLNode m_node;    // XMLNode = !NULL if opened 
    	    int     m_res;     // resource to access;	
	    
	    TMessArch *m_owner;
    };

    class TMessArch: public TArchiveMess
    {
	public:
	    TMessArch( const string &name, TTipArchive *owner );
	    ~TMessArch( );

	    void put( vector<TMess::SRec> &mess );
	    void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category = "", char level = 0 );
	    void start();
	    void stop();
	private:	
	    void ScanDir();

	    static void *Task(void *param);	
	private:	
    	    int       m_res;     // resource to access;	
            bool      m_endrun;  // pthread end run command;	    
	    pthread_t m_pthr;
	    
	    vector<TFileArch *>  arh_s;	    
    };
    
    class TMArchive: public TTipArchive
    {
	friend class TMessArch;
	friend class TFileArch;
	public:
	    TMArchive( const string &name );
	    ~TMArchive();
	    
	    void modLoad();
    
	private:
	    void modConnect(  );

	    TArchiveMess *AMess(const string &name);
	    
	    string optDescr( );
	    //================== Controll functions ========================
	    void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	    
	private:
	    string m_mess_charset;   // default message charset
	    int    m_mess_max_size;  // maximum size kb of Archives file
	    int    m_mess_numb_file; // number of Archive files
	    int    m_mess_time_size; // number days to one file
	    int    m_mess_timeout_free; // timeout of free no used message file buffer;
    };
}

#endif //BASE_H

