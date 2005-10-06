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
    	    void                *hd;         // NULL - share lib present but no attached
	    vector<SUse>        use;         // if share lib attached to show who modules used
	    time_t              m_tm;        // data modify of share lib for automatic update
	    string              name;        // share lib path
	};
	    
	TModSchedul( );    
	~TModSchedul( );
	
	void preDisable(int flag);

	void subLoad( );
	void subSave( );
	void subStart( );	
	void subStop( );
	
	void loadLibS(  );      // Load/Init/Start all share libs and registry moduls into TSubSYS	
	
        SHD &lib( const string &name );			// Get stat share lib <name>
    	void libList( vector<string> &list );		// List present share libs
        void libLoad( const string &path, bool full );	// Load share libs for <dest> from <path> whith call gmdInit if set <full>
	void libAtt( const string &name, bool full = false);	// Attach share libs
    	void libDet( const string &name );		// Detach share libs
	
	string optDescr( );				// Description of config help
	
    /** Private methods: */
    private:
	// Scan directory for OpenSCADA share libs
    	void ScanDir( const string &Paths, vector<string> &files, bool new_f );
	// Check file to OpenSCADA share libs
    	bool CheckFile( const string &name, bool new_f = false );
	// Registre present share lib
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

