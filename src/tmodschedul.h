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

#ifndef TMODSCHEDUL_H
#define TMODSCHEDUL_H

#include <pthread.h>

#include "tmodule.h"

class TModSchedul : public TSubSYS  
{
    /** Public methods: */
    public:
	   
	struct SUse
	{
    	    string mod_sub;
	    string n_mod;
	};
    
	struct SHD
	{
    	    void                *hd;         // NULL - share lib avoid but no attached
	    vector<SUse>        use;         // if share lib attached to show how modules used
	    time_t              m_tm;        // data modify of share lib for automatic update
	    string              name;        // share lib path
	};
	    
	TModSchedul( TSYS *app );    
	~TModSchedul(  );
	
	void preDisable(int flag);

	// Load/Init/Start all share libs and registry moduls into TSubSYS	
    	void loadLibS(  );
	void load( );
	void subLoad(  );
	void subStart(  );	
	void subStop(  );
	
	//Start/stop the sheduler task
	void schedStart( );
	void schedStop( );
	
	// Get stat share lib <name>
        SHD &lib( const string &name );		
	// List avoid share libs
    	void libList( vector<string> &list );
	// Load share libs for <dest> from <path> whith call gmdInit if set <full>
        void libLoad( const string &path, bool full );
	// Attach share libs
	void libAtt( const string &name, bool full = false);
	// Detach share libs
    	void libDet( const string &name );
	
        // Description of config help
	string optDescr( );								
	
    /** Private methods: */
    private:
	// Scan directory for OpenSCADA share libs
    	void ScanDir( const string &Paths, vector<string> &files, bool new_f );
	// Check file to OpenSCADA share libs
    	bool CheckFile( const string &name, bool new_f = false );
	// Registre avoid share lib
    	int  libReg( const string &name );
	// Unreg deleted share lib
	void libUnreg( const string &name );
	// Check file to auto attaching
	bool CheckAuto( const string &name) const;
	
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );

    	static void *SchedTask(void *param);    
	
    private:
	string         	m_mod_path;
	vector<string>	m_am_list;
   
	unsigned     	hd_res;   
	//vector<TSubSYS *>	grpmod; 
	vector<SHD *> 	SchHD;
	pthread_t    	pthr_tsk;
	bool         	m_stat;
	bool         	m_endrun;
};

#endif // TMODSCHEDUL_H

