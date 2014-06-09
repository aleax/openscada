
//OpenSCADA system module UI.VCAEngine file: vcaengine.h
/***************************************************************************
 *   Copyright (C) 2006-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

using namespace OSCADA;

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

	//> Widgets libraries
	void wlbList( vector<string> &ls )			{ chldList( idWlb, ls ); }
	bool wlbPresent( const string &id )			{ return chldPresent( idWlb, id ); }
	void wlbAdd( const string &iid, const string &inm = "", const string &idb = "*.*" );
	void wlbDel( const string &iid, bool full = false )	{ chldDel( idWlb, iid, -1, full ); }
	AutoHD<WidgetLib> wlbAt( const string &id );

	//> Projects
	void prjList( vector<string> &ls )			{ chldList( idPrj, ls ); }
	bool prjPresent( const string &id )			{ return chldPresent( idPrj, id ); }
	void prjAdd( const string &iid, const string &inm = "", const string &idb = "*.*" );
	void prjDel( const string &iid, bool full = false )	{ chldDel( idPrj, iid, -1, full ); }
	AutoHD<Project> prjAt( const string &id );

	//> Sessions
	void sesList( vector<string> &ls )			{ chldList( idSes, ls ); }
	bool sesPresent( const string &id )			{ return chldPresent( idSes, id ); }
	void sesAdd( const string &id, const string &proj = "" );
	void sesDel( const string &iid, bool full = false )	{ chldDel( idSes, iid, -1, full ); }
	AutoHD<Session> sesAt( const string &id );

	//> User functions
	void fList( vector<string> &ls )			{ chldList( idFnc, ls ); }
	bool fPresent( const string &id )			{ return chldPresent( idFnc, id ); }
	AutoHD<TFunction> fAt( const string &id );

	//> Attributes process functions
	void attrsLoad( Widget &w, const string &fullDB, const string &idw, const string &idc, const string &attrs, bool ldGen = false );
	string attrsSave( Widget &w, const string &fullDB, const string &idw, const string &idc, bool ldGen = false );

	//> DB structures
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
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	AutoHD<TCntrNode> chldAt( int8_t igr, const string &name, const string &user = "" );

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
	string	mSynthCom, mSynthCode;	//Synth parameters

	Res	mSynthRes;	//Synth resource

	map<string,string>	mSessAuto;
};

extern  Engine *mod;
}

#endif //VCAENGINE_H
