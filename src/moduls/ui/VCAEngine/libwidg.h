
//OpenSCADA system module UI.VCAEngine file: libwidg.h
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

	string id( )		{ return mId; }			//Identifier
	string name( );						//Name
	string descr( ) 	{ return cfg("DESCR").getS(); }	//Description
	string ico( )		{ return cfg("ICO").getS(); }	//Icon

	string DB( )		{ return work_lib_db; }		//Current library DB
	string tbl( )		{ return cfg("DB_TBL").getS(); }//Table of storing library data
	string fullDB( )	{ return DB()+'.'+tbl(); }	//Full address to library data storage ( DB()+"."+tbl() )

	void setName( const string &it )	{ cfg("NAME").setS(it); }
	void setDescr( const string &it )	{ cfg("DESCR").setS(it); }
	void setIco( const string &it )		{ cfg("ICO").setS(it); }
	void setTbl( const string &it )		{ cfg("DB_TBL").setS(it); }

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
	void del( const string &id, bool full = false )	{ chldDel( m_wdg, id, -1, full ); }

    protected:
	//Methods
	const char *nodeName( )		{ return mId.getSd(); }
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void load_( );
	void save_( );

	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &cfg )     { modif(); return true; }

	AutoHD<TCntrNode> chldAt( int8_t igr, const string &name, const string &user = "" );

	//Attributes
	int     m_wdg;

    private:
	//Attributes
	TCfg	&mId;
	string work_lib_db, mOldDB;
	bool	mEnable;
	bool    passAutoEn;
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

	string	path( );
	string	ico( );
	string	type( )		{ return "LibWidget"; }
	string	calcId( );
	string	calcLang( );
	string	calcProg( );
	int	calcPer( );
	string	parentNm( )	{ return cfg("PARENT").getS(); }
	string	proc( )		{ return cfg("PROC").getS(); }
	int64_t	timeStamp( )	{ return mTimeStamp; }

	void setEnable( bool val );
	void setIco( const string &iico )	{ cfg("ICO").setS(iico); }
	void setCalcLang( const string &ilng );
	void setCalcProg( const string &iprg );
	void setCalcPer( int vl );
	void setParentNm( const string &isw );
	void setEnableByNeed( );

	//> Include widgets
	void wdgAdd( const string &wid, const string &name, const string &path, bool force = false );
	AutoHD<CWidget> wdgAt( const string &wdg );

        //> Storing
	void loadIO( );
	void saveIO( );

	//> Data access
	void resourceList( vector<string> &ls );
	string resourceGet( const string &id, string *mime = NULL );

	void inheritAttr( const string &attr = "" );

	WidgetLib &ownerLib( );

	bool	enableByNeed;   //Load and enable by need

    protected:
	//Methods
	void postDisable( int flag );
	bool cfgChange( TCfg &cfg )     { modif(); return true; }
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	//> Storing
	void load_( );
	void save_( );
	void wClear( );

	unsigned int modifVal( Attr &cfg )      { modif(); return 0; }

    private:
	//Attributes
	int64_t	&m_proc_per,	//Widget period
		&mTimeStamp;
	string	mParentNmPrev;  //Previous parent name after successful enable
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
	string parentNm( )	{ return cfg("PARENT").getS(); }

	void setEnable( bool val );
	void setParentNm( const string &isw );

	//> Storing
	void loadIO( );
	void saveIO( );

	//> Data access
	void resourceList( vector<string> &ls );
	string resourceGet( const string &id, string *mime = NULL );

	void inheritAttr( const string &attr = "" );

	LWidget &ownerLWdg( );

    protected:
	//Methods
	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &cfg )     { modif(); return true; }

	//> Storing
	void load_( );
	void save_( );
	void wClear( );

	unsigned int modifVal( Attr &cfg )	{ modif(); return 0; }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process
};

}

#endif //LIBWDG_H
