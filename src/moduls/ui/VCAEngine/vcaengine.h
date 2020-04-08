
//OpenSCADA module UI.VCAEngine file: vcaengine.h
/***************************************************************************
 *   Copyright (C) 2006-2015 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include "types.h"
#include "libwidg.h"
#include "project.h"
#include "session.h"

#undef _
#define _(mess) mod->I18N(mess)

using namespace OSCADA;

namespace VCA
{

//*************************************************
//* Engine                                        *
//*************************************************
class Engine : public TUI
{
    public:
	//Methods
	Engine( string name );
	~Engine( );

	string modInfo( const string &name );
	void   modInfo( vector<string> &list );

	void modStart( );
	void modStop( );

	string wlbTable( )	{ return "VCALibs"; }
	string prjTable( )	{ return "VCAPrjs"; }

	// Widgets libraries
	void wlbList( vector<string> &ls ) const		{ chldList(idWlb, ls); }
	bool wlbPresent( const string &id ) const		{ return chldPresent(idWlb, id); }
	string wlbAdd( const string &id, const string &inm = "", const string &idb = "*.*" );
	void wlbDel( const string &id, bool full = false )	{ chldDel(idWlb, id, -1, full); }
	AutoHD<WidgetLib> wlbAt( const string &id ) const;

	// Projects
	void prjList( vector<string> &ls ) const		{ chldList(idPrj, ls); }
	bool prjPresent( const string &id ) const		{ return chldPresent(idPrj, id); }
	string prjAdd( const string &id, const string &inm = "", const string &idb = "*.*" );
	void prjDel( const string &id, bool full = false )	{ chldDel(idPrj, id, -1, full); }
	AutoHD<Project> prjAt( const string &id ) const;

	// Sessions
	void sesList( vector<string> &ls ) const		{ chldList(idSes, ls); }
	bool sesPresent( const string &id ) const		{ return chldPresent(idSes, id); }
	void sesAdd( const string &id, const string &proj = "" );
	void sesDel( const string &id, bool full = false )	{ chldDel(idSes, id, -1, full); }
	AutoHD<Session> sesAt( const string &id ) const;

	// User functions
	void fList( vector<string> &ls ) const			{ chldList(idFnc, ls); }
	bool fPresent( const string &id ) const			{ return chldPresent(idFnc, id); }
	AutoHD<TFunction> fAt( const string &id ) const;

	// Attributes process functions
	void attrsLoad( Widget &w, const string &fullDB, const string &idw, const string &idc, const string &attrs, bool ldGen = false );
	string attrsSave( Widget &w, const string &fullDB, const string &idw, const string &idc, bool ldGen = false );

	// DB structures
	TElem &elWdgLib( )	{ return lbwdg_el; }
	TElem &elWdgData( )	{ return wdgdata_el; }
	TElem &elWdg( )		{ return wdg_el; }
	TElem &elWdgIO( )	{ return wdgio_el; }
	TElem &elWdgUIO( )	{ return wdguio_el; }
	TElem &elInclWdg( )	{ return inclwdg_el; }
	TElem &elProject( )	{ return prj_el; }
	TElem &elPage( )	{ return page_el; }
	TElem &elPrjSes( )	{ return prj_ses_el; }
	TElem &elPrjStl( )	{ return prjStl_el; }

	void perSYSCall( unsigned int cnt );

    protected:
	//Methods
	void load_( );
	void save_( );
	void postEnable( int flag );
	void preDisable( int flag );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	AutoHD<TCntrNode> chldAt( int8_t igr, const string &name, const string &user = "" ) const;

    private:
	//Attributes
	bool	passAutoEn;
	int	idWlb, idPrj, idSes, idFnc;
	TElem	lbwdg_el,	//The generic table structure of libraries
		wdgdata_el,	//Media and other data what use by widgets and stored into DB
		wdg_el,		//The table structure of library widgets
		wdgio_el,	//The table structure of library widget's atributes
		wdguio_el,	//The table structure of library widget's user atributes
		inclwdg_el,	//The table structure of container including widgets
		prj_el,		//The generic table structure of project
		page_el,	//The table structure of project's pages
		prj_ses_el,	//The table structure of session's IO of project
		prjStl_el;	//The table structure of styles of project

	ResRW	mSesRes;	//Sessions resource

	map<string,string>	mSessAuto;
};

extern  Engine *mod;
}

#endif //VCAENGINE_H
