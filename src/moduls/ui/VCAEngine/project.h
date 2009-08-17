
//OpenSCADA system module UI.VCAEngine file: project.h
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
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

#ifndef PROJECT_H
#define PROJECT_H

#include <tcntrnode.h>
#include <tconfig.h>

#include "widget.h"

namespace VCA
{

//************************************************
//* Project: VCA project                     	 *
//************************************************
class Page;

class Project : public TCntrNode, public TConfig
{
    public:
	//Data
	enum Flag
	{
	    Maximize	= 0x01,	//Maximize master window and resize content
	    FullScreen	= 0x02,	//Full screen project run
	};
	//Methods
	Project( const string &id, const string &name, const string &lib_db = "*.*" );
	~Project( );

	TCntrNode &operator=( TCntrNode &node );

	const string &id( )	{ return mId; }			//Identifier
	string	name( );					//Name
	string	descr( )	{ return m_descr; }		//Description
	string	ico( )		{ return m_ico; }		//Icon
	string	owner( );					//Library owner
	string	grp( );						//Library group
	short	permit( )	{ return mPermit; }		//Permition for access to library
	int	period( )	{ return mPer; }		//Project's session calc period
	int	prjFlags( )	{ return mFlgs; }		//Project's flags

	string DB( )		{ return work_prj_db; }		//Current library DB
	string tbl( )		{ return m_dbt; }		//Table of storing library data
	string fullDB( )	{ return DB()+'.'+tbl(); }	//Full address to library data storage ( DB()+"."+tbl() )

	void setName( const string &it )	{ m_name = it; modif(); }
	void setDescr( const string &it )	{ m_descr = it; modif(); }
	void setIco( const string &it )		{ m_ico = it; modif(); }
	void setOwner( const string &it );
	void setGrp( const string &it )		{ m_grp = it; modif(); }
	void setPermit( short it )		{ mPermit = it; modif(); }
	void setPeriod( int it )		{ mPer = it; modif(); }
	void setPrjFlags( int val )		{ mFlgs = val; modif(); }

	void setTbl( const string &it )		{ m_dbt = it; }
	void setFullDB( const string &it );

	//> Enable stat
	bool enable( )			{ return mEnable; }
	void setEnable( bool val );

	//> Pages
	void list( vector<string> &ls ) 	{ chldList(mPage,ls); }
	bool present( const string &id )	{ return chldPresent(mPage,id); }
	AutoHD<Page> at( const string &id );
	void add( const string &id, const string &name, const string &orig = "" );
	void add( Page *iwdg );
	void del( const string &id, bool full = false )
	{ chldDel( mPage, id, -1, full ); }

	//> Mime data access
	void mimeDataList( vector<string> &list, const string &idb = "" );
	bool mimeDataGet( const string &id, string &mimeType, string *mimeData = NULL, const string &idb = "" );
	void mimeDataSet( const string &id, const string &mimeType, const string &mimeData, const string &idb = "" );
	void mimeDataDel( const string &id, const string &idb = "" );

    protected:
	//Methods
	string nodeName( )	{ return mId; }
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void load_( );
	void save_( );

	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );

	//Attributes
	int	mPage;

    private:
	//Attributes
	string  &mId,		//Identifier
		&m_name,	//Name
		&m_descr,	//Description
		&m_dbt,		//DB table
		&m_owner,	//Access owner
		&m_grp,		//Access group
		&m_ico,		//Individual icon
		work_prj_db,	//Work DB
		mOldDB;
	int	&mPermit,	//Access permition
		&mPer,		//Calc period
		&mFlgs;		//Project's flags
	bool	mEnable;	//Enable state

	//> Styles
	int	mCurStyle;	//Current style index
	map< string, vector<string> >	mStProp;	//Styles' properties
};

//************************************************
//* Page: Project's page			 *
//************************************************
class PageWdg;

class Page : public Widget, public TConfig
{
    public:
	//Data
	enum Flag
	{
	    Container	= 0x01,	//Page is container included pages
	    Template	= 0x02,	//Page is template for included pages
	    Empty	= 0x04	//No page, use for logical containers
	};

	//Methods
	Page( const string &id, const string &isrcwdg = "" );
	~Page( );

	TCntrNode &operator=( TCntrNode &node );

	string path( );
	string ico( );
	string type( )		{ return "ProjPage"; }
	string owner( );
	string grp( );
	short  permit( )	{ return mPermit; }
	string calcId( );
	string calcLang( );
	string calcProg( );
	int    calcPer( );
	string ownerFullId( bool contr = false );
	int    prjFlags( )	{ return m_flgs; }
	string parentNm( )	{ return mParent; }

	void setIco( const string &iico )	{ m_ico = iico; modif(); }
	void setOwner( const string &iown );
	void setGrp( const string &igrp )	{ m_grp = igrp; modif(); }
	void setPermit( short iperm )		{ mPermit = iperm; modif(); }
	void setCalcLang( const string &ilng );
	void setCalcProg( const string &iprg );
	void setCalcPer( int vl );
	void setParentNm( const string &isw );
	void setPrjFlags( int val );

	//> Storing
	void loadIO( );
	void saveIO( );

	void setEnable( bool val );

	//> Include widgets
	void wdgAdd( const string &wid, const string &name, const string &path, bool force = false );
	AutoHD<PageWdg> wdgAt( const string &wdg );

	//> Pages
	void pageList( vector<string> &ls )	{ chldList(mPage,ls); }
	bool pagePresent( const string &id )	{ return chldPresent(mPage,id); }
	AutoHD<Page> pageAt( const string &id );
	void pageAdd( const string &id, const string &name, const string &orig = "" );
	void pageAdd( Page *iwdg );
	void pageDel( const string &id, bool full = false )
	{ chldDel( mPage, id, -1, full ); }

	//> Data access
	string resourceGet( const string &id, string *mime = NULL );

	void inheritAttr( const string &attr = "" );

	Page	*ownerPage( );
	Project	*ownerProj( );

    protected:
	//Methods
	void postEnable( int flag );
	void postDisable( int flag );

	//> Storing
	void load_( );
	void save_( );
	void wClear( );

	unsigned int modifVal( Attr &cfg )	{ modif(); return 0; }

	bool cntrCmdGeneric( XMLNode *opt );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Attributes
	int	mPage;		//Page container identifier
	string	&m_ico,		//Widget icon
		&m_owner,	//Widget owner
		&m_grp,		//Widget group
		&m_proc,	//Widget procedure
		&mParent,	//Widget parent
		&m_attrs;	//Changed attributes list
	int	&mPermit,	//Widget permission
		&m_flgs,	//Project's flags
		&m_proc_per;	//Process period
};

//************************************************
//* PageWdg: Page included widget                *
//************************************************
class PageWdg : public Widget, public TConfig
{
    public:
	//Methods
	PageWdg( const string &id, const string &isrcwdg = "" );
	~PageWdg( );

	TCntrNode &operator=( TCntrNode &node );

	//> Main parameters
	string path( );
	string ico( );
	string type( )		{ return "ProjLink"; }
	string owner( );
	string grp( );
	short  permit( )	{ return mPermit; }
	string calcId( );
	string calcLang( );
	string calcProg( );
	int    calcPer( );
	string parentNm( )	{ return mParent; }

	void setEnable( bool val );
	void setOwner( const string &iown );
	void setGrp( const string &igrp )	{ m_grp = igrp; modif(); }
	void setPermit( short iperm )		{ mPermit = iperm; modif(); }
	void setParentNm( const string &isw )	{ mParent = isw; modif(); }

	//> Storing
	void loadIO( );
	void saveIO( );

	//> Data access
	string resourceGet( const string &id, string *mime = NULL );

	void inheritAttr( const string &attr = "" );

	Page &ownerPage( );

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
	string	&mParent, &m_attrs, &m_owner, &m_grp;
	int	&mPermit;
};

}

#endif //PROJECT_H
