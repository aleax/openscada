
//OpenSCADA system module UI.VCAEngine file: vcaengine.h
/***************************************************************************
 *   Copyright (C) 2006-2008 by Roman Savochenko                           *
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

#ifndef VCAENGINE_H
#define VCAENGINE_H

#include <telem.h>
#include <tuis.h>
#include "libwidg.h"
#include "project.h"
#include "session.h"

#undef _
#define _(mess) mod->I18N(mess)

namespace VCA
{

//*************************************************
//* Engine                                        *
//*************************************************
class Engine : public TUI
{
    public:
	//Data
	enum Alarm	{ Light = 0x01, Alarm = 0x02, Sound = 0x04 };

	//Methods
	Engine( string name );
	~Engine( );

	string modInfo( const string &name );
	void   modInfo( vector<string> &list );

	void modStart( );
	void modStop( );

	string synthCom( )	{ return mSynthCom; }
	string synthCode( )	{ return mSynthCode; }

	string wlbTable( )	{ return "VCALibs"; }
	string prjTable( )	{ return "VCAPrjs"; }

	void setSynthCom( const string &vl )	{ mSynthCom = vl; modif(); }
	void setSynthCode( const string &vl )	{ mSynthCode = vl; modif(); }

	string callSynth( const string &txt );

	//- Widget's libraries -
	void wlbList( vector<string> &ls )			{ chldList( id_wlb, ls ); }
	bool wlbPresent( const string &id )			{ return chldPresent( id_wlb, id ); }
	void wlbAdd( const string &iid, const string &inm = "", const string &idb = "*.*" );
	void wlbDel( const string &iid, bool full = false )	{ chldDel( id_wlb, iid, -1, full ); }
	AutoHD<WidgetLib> wlbAt( const string &id );

	//- Projects -
	void prjList( vector<string> &ls )			{ chldList( id_prj, ls ); }
	bool prjPresent( const string &id )			{ return chldPresent( id_prj, id ); }
	void prjAdd( const string &iid, const string &inm = "", const string &idb = "*.*" );
	void prjDel( const string &iid, bool full = false )	{ chldDel( id_prj, iid, -1, full ); }
	AutoHD<Project> prjAt( const string &id );

	//- Sessions -
	void sesList( vector<string> &ls )			{ chldList( id_ses, ls ); }
	bool sesPresent( const string &id )			{ return chldPresent( id_ses, id ); }
	void sesAdd( const string &id, const string &proj = "" );
	void sesDel( const string &iid, bool full = false )	{ chldDel( id_ses, iid, -1, full ); }
	AutoHD<Session> sesAt( const string &id );

	//- DB structures -
	TElem &elWdgLib( )	{ return lbwdg_el; }
	TElem &elWdgData( )	{ return wdgdata_el; }
	TElem &elWdg( )		{ return wdg_el; }
	TElem &elWdgIO( )	{ return wdgio_el; }
	TElem &elWdgUIO( )	{ return wdguio_el; }
	TElem &elInclWdg( )	{ return inclwdg_el; }
	TElem &elProject( )	{ return prj_el; }
	TElem &elPage( )	{ return page_el; }
	TElem &elPrjSes( )	{ return prj_ses_el; }

    protected:
	//Methods
	void load_( );
	void save_( );
	void postEnable( int flag );
	void preDisable( int flag );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	string optDescr( );

    private:
	//Attributes
	int	id_wlb, id_prj, id_ses;
	TElem	lbwdg_el,	//The generic table structure of libraries
		wdgdata_el,	//Media and other data what use by widgets and stored into DB
		wdg_el,		//The table structure of library widgets
		wdgio_el,	//The table structure of library widget's atributes
		wdguio_el,	//The table structure of library widget's user atributes
		inclwdg_el,	//The table structure of container including widgets
		prj_el,		//The generic table structure of project
		page_el,	//The table structure of project's pages
		prj_ses_el;	//The table structure of session's IO of project
	string	mSynthCom, mSynthCode;	//Synth parameters

	Res	mSynthRes;	//Synth resource
};

extern  Engine *mod;
}

#endif //VCAENGINE_H
