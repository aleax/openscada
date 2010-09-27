
//OpenSCADA system module UI.VCAEngine file: libwidg.h
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

	const string &id( )	{ return mId; }			//Identifier
	string name( );						//Name
	string descr( ) 	{ return m_descr; }		//Description
	string ico( )		{ return m_ico; }		//Icon

	string DB( )		{ return work_lib_db; }		//Current library DB
	string tbl( )		{ return m_dbt; }		//Table of storing library data
	string fullDB( )	{ return DB()+'.'+tbl(); }	//Full address to library data storage ( DB()+"."+tbl() )

	void setName( const string &it )	{ m_name = it; modif(); }
	void setDescr( const string &it )	{ m_descr = it; modif(); }
	void setIco( const string &it )		{ m_ico = it; modif(); }
	void setTbl( const string &it )		{ m_dbt = it; }

	void setFullDB( const string &it );

	//> Enable stat
	bool enable( )		{ return mEnable; }
	void setEnable( bool val );

        //> Mime data access
	void mimeDataList( vector<string> &list, const string &idb = "" );
	bool mimeDataGet( const string &id, string &mimeType, string *mimeData = NULL, const string &idb = "" );
	void mimeDataSet( const string &id, const string &mimeType, const string &mimeData, const string &idb = "" );
	void mimeDataDel( const string &id, const string &idb = "" );

	//> Widgets
	void list( vector<string> &ls ) 	{ chldList(m_wdg,ls); }
	bool present( const string &id )	{ return chldPresent(m_wdg,id); }
	AutoHD<LWidget> at( const string &id );
	void add( const string &id, const string &name, const string &orig = "" );
	void add( LWidget *iwdg );
	void del( const string &id, bool full = false )
	{ chldDel( m_wdg, id, -1, full ); }

    protected:
	//Methods
	const string &nodeName( )		{ return mId; }
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void load_( );
	void save_( );

	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );

	//Attributes
	int     m_wdg;

    private:
	//Attributes
	string  &mId, &m_name, &m_descr, &m_dbt, &m_ico, work_lib_db, mOldDB;
	bool	mEnable;
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
	string calcId( );
	string calcLang( );
	string calcProg( );
	int    calcPer( );
	string parentNm( )	{ return mParent; }

	void setIco( const string &iico )	{ m_ico = iico; modif(); }
	void setCalcLang( const string &ilng );
	void setCalcProg( const string &iprg );
	void setCalcPer( int vl );
	void setParentNm( const string &isw );

	//> Include widgets
	void wdgAdd( const string &wid, const string &name, const string &path, bool force = false );
	AutoHD<CWidget> wdgAt( const string &wdg );

        //> Storing
	void loadIO( );
	void saveIO( );

	//> Data access
	string resourceGet( const string &id, string *mime = NULL );

	void inheritAttr( const string &attr = "" );

	WidgetLib &ownerLib( );

    protected:
	//Methods
	void postDisable( int flag );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	//> Storing
	void load_( );
	void save_( );
	void wClear( );

	unsigned int modifVal( Attr &cfg )      { modif(); return 0; }

    private:
	//Attributes
	string	&m_ico,		//Widget icon
		&m_proc,	//Widget procedure
		&mParent,	//Widget parent
		&m_attrs;	//Changed attributes list
	int	&m_proc_per;	//Widget period
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

	//> Main parameters
	string path( );
	string ico( );
	string type( )		{ return "LibLink"; }
	string calcId( );
	string calcLang( );
	string calcProg( );
	int    calcPer( );
	string parentNm( )	{ return mParent; }

	void setEnable( bool val );
	void setParentNm( const string &isw );

	//> Storing
	void loadIO( );
	void saveIO( );

	//> Data access
	string resourceGet( const string &id, string *mime = NULL );

	void inheritAttr( const string &attr = "" );

	LWidget &ownerLWdg( );

    protected:
	//Methods
	void postEnable( int flag );
	void preDisable( int flag );

	//> Storing
	void load_( );
	void save_( );
	void wClear( );

	unsigned int modifVal( Attr &cfg )	{ modif(); return 0; }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	bool	delMark;
	string	&mParent, &m_attrs;
};

}

#endif //LIBWDG_H
