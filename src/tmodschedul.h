
//OpenSCADA system file: tmodschedul.h
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

#ifndef TMODSCHEDUL_H
#define TMODSCHEDUL_H

#include <pthread.h>

#include "tsubsys.h"

//*************************************************
//* TModSchedul                                   *
//*************************************************
class TModSchedul : public TSubSYS  
{
    public:
	//Data
	class SHD
	{
	    public:
		//Methods
		SHD( ) : hd(NULL), m_tm(0)	{ }
		SHD( void *ihd, time_t itm, const string &iname ) : hd(ihd), m_tm(itm), name(iname)	{ }
		
		//Attributes
    		void		*hd;         // NULL - share lib present but no attached
		vector<string>	use;         // if share lib attached to show who modules used    
		time_t		m_tm;        // data modify of share lib for automatic update
		string		name;        // share lib path
	};
	    
	//Methods
	TModSchedul( );    
	~TModSchedul( );
	
	void chkPer( int per );

	void subLoad( );
	void subSave( );
	void subStart( );	
	void subStop( );
	
	void loadLibS(  );      			// Load/Init/Start all share libs and registry moduls into TSubSYS	
	
        SHD &lib( const string &name );			// Get stat share lib <name>
    	void libList( vector<string> &list );		// List present share libs
        void libLoad( const string &path, bool full );	// Load share libs for <dest> from <path> whith call gmdInit if set <full>
	void libAtt( const string &name, bool full = false);	// Attach share libs
    	void libDet( const string &name );		// Detach share libs

    protected:
	//Methods
	void preDisable( int flag );
	
    private:
	//Methods
	string optDescr( );					// Description of config help	
    	void ScanDir( const string &Paths, vector<string> &files );//Scan directory for OpenSCADA share libs
    	bool CheckFile( const string &name );			//Check file to OpenSCADA share libs
    	int  libReg( const string &name );			//Registre present share lib
	void libUnreg( const string &name );			//Unreg deleted share lib
	bool CheckAuto( const string &name) const;		//Check file to auto attaching
	
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    	static void SchedTask(union sigval obj);    
	
	//Attributes
	string         	m_mod_path;
	vector<string>	m_am_list;
   
	Res     	hd_res;   
	int		m_per;	//Check to new modules period
	vector<SHD> 	SchHD;
	timer_t 	tmId;   //Thread timer
	bool         	prc_st;
};

#endif // TMODSCHEDUL_H

