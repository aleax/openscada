
//OpenSCADA system module UI.VCAEngine file: libwidg.h
/***************************************************************************
 *   Copyright (C) 2006-2007 by Roman Savochenko                           *
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

#ifndef LIBWDG_H
#define LIBWDG_H

#include <tcntrnode.h>
#include <tconfig.h>

#include "widget.h"

namespace VCA
{

//************************************************
//* WidgetLib: Widgets library                   *
//************************************************
class LWidget;

class WidgetLib : public TCntrNode, public TConfig
{
    public:
	//Methods
	WidgetLib( const string &id, const string &name, const string &lib_db = "*.*" );
	~WidgetLib( );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return m_id; }		//Identifier
	string name( );						//Name
	string descr( ) 	{ return m_descr; }		//Description
	string ico( )		{ return m_ico; }		//Icon
	string user( );						//Library user
	string grp( );						//Library group
	short  permit( )	{ return m_permit; }		//Permition for access to library

	string DB( )		{ return work_lib_db; }		//Current library DB
	string tbl( )		{ return m_dbt; }		//Table of storing library data
	string fullDB( )	{ return DB()+'.'+tbl(); }	//Full address to library data storage ( DB()+"."+tbl() )

	void setName( const string &it )	{ m_name = it; modif(); }
	void setDescr( const string &it )	{ m_descr = it; modif(); }
	void setIco( const string &it )		{ m_ico = it; modif(); }
	void setUser( const string &it );
	void setGrp( const string &it )		{ m_grp = it; modif(); }
	void setPermit( short it )		{ m_permit = it; modif(); }
	void setTbl( const string &it )		{ m_dbt = it; }

	void setFullDB( const string &it );

	//- Enable stat -
	bool enable( )		{ return m_enable; }
	void setEnable( bool val );

        //- Mime data access -
	void mimeDataList( vector<string> &list, const string &idb = "" );
	bool mimeDataGet( const string &id, string &mimeType, string *mimeData = NULL, const string &idb = "" );
	void mimeDataSet( const string &id, const string &mimeType, const string &mimeData, const string &idb = "" );
	void mimeDataDel( const string &id, const string &idb = "" );

	//- Widgets -
	void list( vector<string> &ls ) 	{ chldList(m_wdg,ls); }
	bool present( const string &id )	{ return chldPresent(m_wdg,id); }
	AutoHD<LWidget> at( const string &id );
	void add( const string &id, const string &name, const string &orig = "" );
	void add( LWidget *iwdg );
	void del( const string &id, bool full = false )
	{ chldDel( m_wdg, id, -1, full, true ); }

    protected:
	//Methods
	string nodeName()       { return m_id; }
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	void load_( );
	void save_( );

	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );

	//Attributes
	int     m_wdg;

    private:
	//Attributes
	string  &m_id, &m_name, &m_descr, &m_dbt, &m_user, &m_grp, &m_ico, work_lib_db, mOldDB;
	int	&m_permit;
	bool	m_enable;
};

//************************************************
//* LWidget: Library stored widget               *
//************************************************
class CWidget;

class LWidget : public Widget, public TConfig
{
    public:
    //Methods
	LWidget( const string &id, const string &isrcwdg = "" );
	~LWidget( );

	string path( );
	string ico( );
	string type( )		{ return "LibWidget"; }
	string user( );
	string grp( );
	short  permit( )	{ return m_permit; }
	string calcId( );
	string calcLang( );
	string calcProg( );
	int    calcPer( );
	string parentNm( )	{ return m_parent; }

	void setIco( const string &iico )	{ m_ico = iico; modif(); }
	void setUser( const string &iuser );
	void setGrp( const string &igrp )	{ m_grp = igrp; modif(); }
	void setPermit( short iperm )		{ m_permit = iperm; modif(); }
	void setCalcLang( const string &ilng );
	void setCalcProg( const string &iprg );
	void setCalcPer( int vl );
	void setParentNm( const string &isw )	{ m_parent = isw; modif(); }

	//- Include widgets -
	void wdgAdd( const string &wid, const string &name, const string &path );
	AutoHD<CWidget> wdgAt( const string &wdg );

        //- Storing -
	void loadIO( );
	void saveIO( );

	//- Data access -
	string resourceGet( const string &id, string *mime = NULL );

	void inheritAttr( const string &attr = "" );

	WidgetLib &owner( );

    protected:
	//Methods
	void postDisable( int flag );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	//- Storing -
	void load_( );
	void save_( );

	unsigned int modifVal( Attr &cfg )      { modif(); return 0; }

    private:
	//Attributes
	string	&m_ico,		//Widget icon
		&m_user,	//Widget user
		&m_grp,		//Widget group
		&m_proc,	//Widget procedure
		&m_parent,	//Widget parent
		&m_attrs;	//Changed attributes list
	int	&m_permit,	//Widget permission
		&m_proc_per;	//Widget period
};

//************************************************
//* CWidget: Container stored widget             *
//************************************************
class CWidget : public Widget, public TConfig
{
    public:
	//Methods
	CWidget( const string &id, const string &isrcwdg = "" );
	~CWidget( );

	//- Main parameters -
	string path( );
	string ico( );
	string type( )		{ return "LibLink"; }
	string user( );
	string grp( );
	short  permit( );
	string calcId( );
	string calcLang( );
	string calcProg( );
	int    calcPer( );
	string parentNm( )	{ return m_parent; }

	void setEnable( bool val );

	void setParentNm( const string &isw )	{ m_parent = isw; modif(); }

	//- Storing -
	void loadIO( );
	void saveIO( );

	//- Data access -
	string resourceGet( const string &id, string *mime = NULL );

	void inheritAttr( const string &attr = "" );

	LWidget &owner( );

    protected:
	//Methods
	void postEnable( int flag );
	void preDisable( int flag );

	//- Storing -
	void load_( );
	void save_( );

	unsigned int modifVal( Attr &cfg )	{ modif(); return 0; }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	bool    delMark;
	string  &m_parent, &m_attrs;
};

}

#endif //LIBWDG_H
