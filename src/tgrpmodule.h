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

	string Name();

	// Type/grp module version
	virtual int gmd_Ver( ) = 0;
	// Init group modules.
	virtual void gmd_Init( ) { }
	// Start group modules    
	virtual void gmd_Start( ) { }
	virtual void gmd_Stop( ) { }
    
	// Avoid modules list
	void gmd_list( vector<string> &list )
	{ m_hd.obj_list( list ); }
	// Add modul
	virtual void gmd_add( TModule *modul );
	// Del modul
	virtual void gmd_del( const string &name );
	// Modul
        AutoHD<TModule> gmd_at( const string &name )
        { AutoHD<TModule> obj( name, m_hd ); return obj; }			
	
	// Must delete! Attach to modul.
	unsigned gmd_att( const string &name, const string &how = "" )
	{ return( m_hd.hd_att( name, how ) ); }
	// Detach from modul
	void gmd_det( unsigned hd )
	{ m_hd.hd_det( hd ); }
	// Get attached modul
	TModule &gmd_at( unsigned hd ) 
	{ return( *(TModule *)m_hd.hd_at( hd ) ); }

	virtual void gmd_CheckCommandLine( );
	virtual void gmd_UpdateOpt();

	void gmd_CheckCommandLineMods();
	void gmd_UpdateOptMods();
	// Get XML section node
	XMLNode *gmd_XMLCfgNode();
    
	string &gmd_Name()    { return(NameType); }
	string &gmd_ModPath() { return(DirPath); }
    
	TKernel &Owner() { return(*owner); }
	//================== Message functions ========================
	void m_put( const string &categ, int level, char *fmt,  ... );
	void m_put_s( const string &categ, int level, const string &mess ); 
    /**Public Attributes: */
    public:

    /** Protected methods: */
    protected:
        //================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
	void ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez );
	AutoHD<TContr> ctr_at1( const string &br );
    /** Protected Attributes: */
    protected:    
	string            DirPath;
	string            s_name;
    /** Private methods: */
    private:
    /** Private Attributes: */
    private:
	TKernel           *owner;    
	THD               m_hd;
	
	string            NameType;
	static const char *o_name;
};

#endif // TGRPMODULE_H
