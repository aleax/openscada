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

#ifndef TGRPMODULE_H
#define TGRPMODULE_H

#include <string>
#include <vector>

#include "terror.h"
#include "thd.h"
#include "xml.h"
#include "tcontr.h"
#include "tmodule.h"

using std::string;
using std::vector;

class TModule;
class TModSchedul;
class TKernel;

class TGRPModule : public TContr
{
    /** Public methods: */
    public:
	TGRPModule( TKernel *app, char * NameT );
    
	virtual ~TGRPModule(  );

	string name();

	// Type/grp module version
	virtual int gmdVer( ) = 0;
	// Init group modules.
	virtual void gmdInit( ) { }
	// Start group modules    
	virtual void gmdStart( ) { }
	virtual void gmdStop( ) { }
    
	// Avoid modules list
	void gmdList( vector<string> &list )
	{ m_hd.objList( list ); }
	// Add modul
	virtual void gmdAdd( TModule *modul );
	// Del modul
	virtual void gmdDel( const string &name );
	// Modul
        AutoHD<TModule> gmdAt( const string &name )
        { AutoHD<TModule> obj( name, m_hd ); return obj; }			
	
	virtual void gmdCheckCommandLine( );
	virtual void gmdUpdateOpt();

	void gmdCheckCommandLineMods();
	void gmdUpdateOptMods();
	// Get XML section node
	XMLNode *gmdCfgNode();
    
	string &gmdName()    { return(nameType); }
	string &gmdModPath() { return(DirPath); }
	
	//================== Message functions ========================
        void mPut( const string &categ, int level, char *fmt,  ... );
        void mPutS( const string &categ, int level, const string &mess );			
    
	TKernel &owner() { return(*m_owner); }

    /** Protected methods: */
    protected:
        //================== Controll functions ========================
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &path, XMLNode *opt );
	void ctrDinSet_( const string &a_path, XMLNode *opt );
	AutoHD<TContr> ctrAt1( const string &br );
	
    /** Protected Attributes: */
    protected:    
	string            DirPath;
	string            s_name;
	
    /** Private Attributes: */
    private:
	TKernel           *m_owner;    
	THD               m_hd;
	
	string            nameType;
	static const char *o_name;
};

#endif // TGRPMODULE_H
