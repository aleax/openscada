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

#ifndef TSYS_H
#define TSYS_H

#define TO_FREE         NULL // Object free
#define STR_BUF_LEN     2000 // Len of string buffers (no string class)
#define STD_WAIT_DELAY  100 // Standart wait dalay (ms)

#define C_INT_DEC 0
#define C_INT_OCT 1
#define C_INT_HEX 2

#include <semaphore.h>
#include <stdio.h>

#include <string>
#include <vector>

#include "xml.h"
#include "thd.h"
#include "tcontr.h"

using std::string;
using std::vector;

class TKernel;

//======================================================================================
//====================== TSYS ==========================================================
//======================================================================================


class TSYS : public TContr 
{
    // Public methods:
    public:
	TSYS( int argi, char ** argb, char **env );
	~TSYS(  );

	int start(  );	
        //========= System function ====================
    	// Convert path to absolut name
	string FixFName( const string &fname ) const;    
	// Convert value to string
        static string int2str( int val, char view = C_INT_DEC );
        static string real2str( double val );	
	
	// Config file's functions
        XMLNode *cfgNode();
	
        // Programms options
	string UserName() { return(User); }               //Run user name 
	unsigned cr_file_perm( ) { return(m_cr_f_perm); } //Permision for created files ( default 0644 )
	unsigned cr_dir_perm( ) { return(m_cr_d_perm); }  //Permision for created files ( default 0755 )
	bool event_wait( bool &m_mess_r_stat, bool exempl, const string &loc, time_t time = 0 );
	
	// Get option from generic config file and update data from XML config.
	void updateOpt();
	// Update comand line option
	void checkCommandLine( );
	// Print comand line options!
	string optDescr( );
	// Set task title
	void SetTaskTitle(const char *fmt, ...);
	string CfgFile() { return(Conf_File); }
	string Station() { return(m_station); }
	
        //================== Kernel functions ========================
        void kern_list( vector<string> &list )
	{ m_kern.objList( list ); }
	void kern_add( const string &name );
	void kern_del( const string &name );
	AutoHD<TKernel> kern_at( const string &name )
	{ AutoHD<TKernel> obj( name, m_kern ); return obj; }

	static void sighandler( int signal );
	
    public:
	// A comand line seting counter.
	const int argc;
	// A comand line seting buffer.
	const char **argv;
	// A system environment.
	const char **envp;							     

    private:
	void ScanCfgFile( bool first = false );
        //================== Controll functions ========================
	void     ctrStat_( XMLNode *inf );
	void     ctrDinGet_( const string &a_path, XMLNode *opt );
	void     ctrDinSet_( const string &a_path, XMLNode *opt );
	AutoHD<TContr> ctrAt1( const string &br ){ return( kern_at( pathLev(br,2) ) ); }
    /** Private atributes: */
    
    private:    
    	// A owner user name!
	string User;
	string Conf_File;
	string m_station;
	unsigned m_cr_f_perm;
	unsigned m_cr_d_perm;
	//OpenScada and station XML config node
	XMLNode root_n;
	XMLNode *stat_n;
	
	int    	stop_signal;

	THD	m_kern;  // List kernels		

	//Request mess params
	time_t	m_beg, m_end;
	string	m_cat;
	int	m_lvl;

	static const char *o_name;    
};

struct SSem
{
    bool  use;          // using flag
    bool  del;          // deleting flag    
    sem_t sem;          // semafor id 
    int   rd_c;         // readers counter
};

class ResAlloc 
{
    public: 
	ResAlloc( unsigned id );
	ResAlloc( unsigned id, bool write );
	~ResAlloc( );

	void request( bool write = false, long tm = 0 );
	void release();
	
	// Static metods
	static unsigned resCreate( unsigned val = 1 );
	static void resDelete( unsigned res );
    
	static void resRequestW( unsigned res, long tm = 0 ); // Write request
        static void resReleaseW( unsigned res );              // Write release
	static void resRequestR( unsigned res, long tm = 0 ); // Read request
	static void resReleaseR( unsigned res );              // Read release
	
    private:
	int   m_id;     //
	char  m_wr;     //0x01 - alloc; 0x02 - write
	
	static vector<SSem>  sems;
	
	static const char *o_name;    
};


extern TSYS *SYS;

#endif // TSYS_H
