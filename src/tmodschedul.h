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

#include "tcontr.h"

//For a multi moduls declaration into once a shared lib
struct SUse
{
    int id_tmod;
    int id_mod;
};

struct SHD
{
    void         *hd;         // NULL - share lib avoid but no attached
    vector<SUse> use;         // if share lib attached to show how modules used 
    time_t       m_tm;        // data modify of share lib for automatic update    
    string       name;        // share lib path
};

class TGRPModule;

class TModSchedul : public TContr   
{
    /** Public methods: */
    public:
	TModSchedul( TKernel *app );
    
	~TModSchedul(  );

	string Name();
	// Check command line all TGRPModules 
	void CheckCommandLine(  );
	void CheckCommandLineMod(  );
	// Update options from generic config file
    	void UpdateOpt();
	void UpdateOptMod();
	// Description of config help
	string opt_descr( );
	// Load all share libs and registry moduls into TGRPModule	
    	void LoadAll(  );
	// Load share libs for <dest> from <path> whith call gmd_Init if set <full>
    	void Load( const string &path, int dest, bool full );

	void InitAll(  );
    
	void DeinitAll(  );

	void StartAll(  );
	
	void StartSched(  );
	// Register group moduls
    	int RegGroupM( TGRPModule *gmod );
	// Unregister group moduls   	 
    	int UnRegGroupM( TGRPModule *gmod );
	// List avoid share libs
    	void ListSO( vector<string> &list );
	// Get stat share lib <name>
    	SHD SO( const string &name );    
	/*
	 * Attach share libs
	 *   name = SO name;
	 *   dest = direct loading <dest> type modules
	 *   full = after loading will be call gmd_Init;
	 */
	void AttSO( const string &name, bool full = false, int dest = -1);
	// Detach share libs
    	void DetSO( const string &name );

	TKernel &Owner() const { return(*owner); }
    public:
    /** Private methods: */
    private:
	// Get XML section node
	XMLNode *XMLCfgNode();
	// Scan directory for OpenScada share libs
    	void ScanDir( const string &Paths, vector<string> &files, bool new_f ) const;
	// Check file to OpenScada share libs
    	bool CheckFile( const string &name, bool new_f = false ) const;
	// Registre avoid share lib
    	int  RegSO( const string &name );
	// Check file to auto attaching
    	bool CheckAuto( const string &name) const;    
	// Unreg deleted share lib
	void UnregSO( const string &name );
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );

    	static void *SchedTask(void *param);    
    private:
	TKernel              *owner;
	
	string               m_mod_path;
	vector<string>       m_am_list;
   
	unsigned             hd_res;   
	vector<TGRPModule *> grpmod; 
	vector<SHD *>        SchHD;
	pthread_t            pthr_tsk;
	bool                 m_stat;
	bool                 m_endrun;

	static const char   *o_name;
	static const char   *s_name;
};

#endif // TMODSCHEDUL_H

