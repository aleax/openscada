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

#include <deque>

#include <tmodule.h>
#include <xml.h>
#include <tarchives.h>

using std::deque;

namespace BaseArch
{
    class TMessArch;
    
    class TFileArch
    {
	public:
 	    TFileArch( TMessArch *owner );
 	    TFileArch( const string &name, time_t beg, TMessArch *owner); // string charset, int time_size );
 	    ~TFileArch();

	    void Attach( const string &name, bool full = true );
	    void put( TMess::SRec mess );
	    void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category, char level );
	    // Write changes to Archive file 
	    //  free - surely free used memory
	    void sync( bool free = false );     

	    string &name() 	{ return m_name; }
	    int	   size()	{ return m_size; }
	    time_t begin()	{ return m_beg; }
	    time_t end()  	{ return m_end; }
	    bool   err()  	{ return m_err; }

	    TMessArch &owner() { return *m_owner; }
	    
	public:
    	    bool    scan;    	// Archive scaned (for check deleted files)
	    
	private:	    	
    	    string  m_name;    	// name Archive file;
	    int	    m_size;  	// Arhive size
    	    string  m_chars;   	// Archive charset;
    	    bool    m_err;     	// Archive err
    	    bool    m_write;   	// Archive had changed but no writed to file
	    bool    m_load;    	// arhiv load to m_node
	    time_t  m_acces;   	// last of time acces to Archive file
    	    time_t  m_beg;     	// begin Archive file;
    	    time_t  m_end;     	// end Archive file;
    	    XMLNode m_node;    	// XMLNode = !NULL if opened 
    	    int     m_res;     	// resource to access;	
	    
	    TMessArch *m_owner;
    };

    class TMessArch: public TArchiveMess
    {
	public:
	    TMessArch( const string &name, TElem *cf_el );
	    ~TMessArch( );

	    void put( vector<TMess::SRec> &mess );
	    void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category = "", char level = 0 );
	    void start();
	    void stop();

	    int  maxSize()	{ return m_max_size; }
	    int	 numbFiles()	{ return m_numb_files; }
	    int  timeSize()	{ return m_time_size; }
	    int  timeoutFree()	{ return m_timeout_free; }
	    
	private:	
	    void ScanDir();

	    static void *Task(void *param);	
            //================== Controll functions ========================
	    void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	    
	private:
	    int		&m_max_size;	// maximum size kb of Archives file
	    int		&m_numb_files;	// number of Archive files
	    int		&m_time_size;	// number days to one file
	    int		&m_timeout_free;// timeout of free no used message file buffer;
	
    	    int       	m_res;     	// resource to access;	
            bool      	m_endrun;  	// pthread end run command;	    
	    pthread_t 	m_pthr;
	    
	    deque<TFileArch *>  arh_s;	    
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
	    void postEnable( );

	    TArchiveMess *AMess(const string &name);
	    
	    string optDescr( );
	    //================== Controll functions ========================
	    void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
    };

extern TMArchive *mod;
}

#endif //BASE_H

