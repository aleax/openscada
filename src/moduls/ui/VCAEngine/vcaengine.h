
//OpenSCADA system module UI.VCSEngine file: vcaengine.h
/***************************************************************************
 *   Copyright (C) 2006 by Roman Savochenko
 *   rom_as@diyaorg.dp.ua                                                     
 *                                                                         
 *   This program is free software; you can redistribute it and/or modify  
 *   it under the terms of the GNU General Public License as published by  
 *   the Free Software Foundation; either version 2 of the License, or     
 *   (at your option) any later version.                                   
 *                                                                         
 *   This program is distributed in the hope that it will be useful,       
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *   GNU General Public License for more details.                          
 *                                                                         
 *   You should have received a copy of the GNU General Public License     
 *   along with this program; if not, write to the                         
 *   Free Software Foundation, Inc.,                                       
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             
 ***************************************************************************/

#ifndef VCAENGINE_H
#define VCAENGINE_H

#include <telem.h>
#include <tuis.h>
#include "libwidg.h"

#undef _
#define _(mess) mod->I18N(mess)

namespace VCA
{

class Engine : public TUI
{
    public:
	//Methods
	Engine( string name );
	~Engine( );	

	void modStart();
	void modStop();

	void modLoad( );
	void modSave( );
	
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );
	
	string wlbTable()	{ return "VCALibs"; }
	
	//Original widgets
	void origList( vector<string> &ls );
	bool origPresent( const string &orig );
	string origIcon( const string &orig );
	Widget *origGet( const string &orig, const string &id, bool lib_loc = true );
	
	//Widget's libraries
        void wlbList( vector<string> &ls )		{ chldList(id_wlb,ls); }
        bool wlbPresent( const string &id )		{ return chldPresent(id_wlb,id); }
	void wlbAdd( const string &iid, const string &inm = "", const string &idb = "*.*" );
        void wlbDel( const string &id, bool full = false )	{ chldDel(id_wlb,id,-1,full); }	
        AutoHD<LibWdg> wlbAt( const string &id );
	
	TElem &elWdgLib()	{ return lbwdg_el; }
	TElem &elWdg()		{ return wdg_el; }
	TElem &elWdgIO()	{ return wdgio_el; }
	TElem &elInclWdg()	{ return inclwdg_el; }
	
    protected:
	void postEnable( );
	void preDisable( int flag );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process	
	
	string optDescr( );
	
    private:
	int	id_wlb;
	TElem	lbwdg_el, 	//The generic table structure of libraries
		wdg_el, 	//The table structure of library widgets
		wdgio_el, 	//The table structure of library widget's atributes
		inclwdg_el;	//The table structure of container including widgets
};
    
extern  Engine *mod;
}

#endif //VCAENGINE_H

