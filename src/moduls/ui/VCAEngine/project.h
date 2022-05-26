
//OpenSCADA module UI.VCAEngine file: project.h
/***************************************************************************
 *   Copyright (C) 2007-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#define STL_PRM_NM	"<Styles>"

namespace VCA
{

//************************************************
//* Project: VCA project                     	 *
//************************************************
class Page;
class Session;

class Project : public TCntrNode, public TConfig
{
    public:
	//Data
	enum Flag {
	    Maximize		= 0x01,	//Maximize master window and resize content
	    FullScreen		= 0x02,	//Full screen project run
	    KeepAspectRatio	= 0x04,	//Keep master page aspect ratio on scale
	};
	enum Styles {
	    StlCreate		= -2,
	    StlDisabled		= -1,
	    StlMaximum		= 10,
	};

	//Methods
	Project( const string &id, const string &name, const string &lib_db = "*.*" );
	~Project( );

	TCntrNode &operator=( const TCntrNode &node );

	string	id( ) const	{ return mId; }			//Identifier
	string	name( ) const;					//Name
	string	descr( ) const	{ return cfg("DESCR").getS(); }	//Description
	string	ico( ) const	{ return cfg("ICO").getS(); }	//Icon
	string	owner( ) const;					//Library owner
	string	grp( ) const;					//Library group
	short	permit( ) const	{ return mPermit; }		//Permission for access to library
	int	period( )	{ return mPer; }		//Project's session calculate period
	bool	toEnByNeed( )	{ return cfg("EN_BY_NEED").getB(); } //To enable the project by need
	string	getStatus( );

	bool isStdStorAddr( ) const		{ return (tbl() == ("prj_"+id())); }	//????[v1.0] Remove
	string DB( bool qTop = false ) const	{ return storage(mDB, qTop); }
	string tbl( ) const	{ return cfg("DB_TBL").getS().empty() ? ("prj_"+id()) : cfg("DB_TBL").getS(); }
	string fullDB( bool qTop = false ) const{ return DB(qTop)+'.'+tbl(); }

	void setName( const string &it )	{ cfg("NAME").setS(it); }
	void setDescr( const string &it )	{ cfg("DESCR").setS(it); }
	void setIco( const string &it )		{ cfg("ICO").setS(it); }
	void setOwner( const string &it );
	void setGrp( const string &it )		{ cfg("GRP").setS(it); }
	void setPermit( short it )		{ mPermit = it; modif(); }
	void setPeriod( int it )		{ mPer = it; modif(); }
	void setToEnByNeed( bool vl )		{ cfg("EN_BY_NEED").setB(vl); }

	void setDB( const string &vl, bool qTop = false ) { setStorage(mDB, vl, qTop); if(!qTop) modifG(); }
	void setTbl( const string &it )		{ cfg("DB_TBL").setS(it); }
	void setFullDB( const string &it );

	// Enable stat
	bool enable( ) const			{ return mEnable; }
	void setEnable( bool val );
	void setEnableByNeed( )			{ enableByNeed = true; modifClr(); }

	// Pages
	void list( vector<string> &ls ) const		{ chldList(mPage,ls); }
	bool present( const string &id ) const		{ return chldPresent(mPage,id); }
	AutoHD<Page> at( const string &id ) const;
	string add( const string &id, const string &name, const string &orig = "" );
	void add( Page *iwdg );
	void del( const string &id, bool full = false )	{ chldDel( mPage, id, -1, full?NodeRemove:NodeNoFlg ); }

	// Resource data access
	void resourceDataList( vector<string> &list, const string &idb = "" ) const;
	bool resourceDataGet( const string &id, string &mimeType, string *mimeData = NULL, const string &idb = "", int off = -1, int *size = NULL ) const;
	void resourceDataSet( const string &id, const string &mimeType, const string &mimeData, const string &idb = "" );
	void resourceDataDel( const string &id, const string &idb = "" );

	// Styles
	void stlList( vector<string> &ls );
	int stlSize( );
	int64_t stlCurent( )			{ return mStyleIdW; }
	void stlCurentSet( int sid );
	string stlGet( int sid );
	void stlSet( int sid, const string &stl );
	void stlPropList( vector<string> &ls );
	string stlPropGet( const string &pid, const string &def = "", int sid = -1 );
	bool stlPropSet( const string &pid, const string &vl, int sid = -1 );

	string catsPat( );	//Individual the page's sessions' messages' categories pattern

	// Sessions-heritors
	void heritReg( Session *s );	//Register the heritator
	void heritUnreg( Session *s );	//Unregister the heritator
	void pageEnable( const string &pg, bool vl );	//Process for the page <pg> enabling for herit sessions

	//Attributes
	bool	enableByNeed;	//Load and enable by need
	ResMtx &funcM( )		{ return mFuncM; }

    protected:
	//Methods
	const char *nodeName( ) const	{ return mId.getSd(); }
	string nodeNameSYSM( ) const	{ return mId.getSd(); }
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void load_( TConfig *cfg );
	void save_( );

	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &co, const TVariant &pc )	{ modif(); return true; }

	//Attributes
	int	mPage;

    private:
	//Attributes
	TCfg	&mId;		//Identifier
	string	mDB,		//Work DB
		mDB_MimeSrc;	//After the copy mostly
	int64_t	&mPermit,	//Access permission
		&mPer,		//Calculate period
		&mStyleIdW;	//Work style
	bool	mEnable;	//Enable state

	// Styles
	ResRW	mStRes;
	map< string, vector<string> >	mStProp;	//Styles' properties

	ResMtx	mFuncM, mHeritRes;

	vector< AutoHD<Session> > mHerit;	//Heritators
};

//************************************************
//* Page: Project's page			 *
//************************************************
class PageWdg;

class Page : public Widget, public TConfig
{
    public:
	//Data
	enum Flag {
	    Container	= 0x01,	//Page is container included pages
	    Template	= 0x02,	//Page is template for included pages
	    Empty	= 0x04,	//No page, use for logical containers
	    Link	= 0x08	//Links of the projects executing side
	};

	//Methods
	Page( const string &id, const string &isrcwdg = "" );
	~Page( );

	TCntrNode &operator=( const TCntrNode &node );

	string	addr( ) const;
	string	ico( ) const;
	string	type( )			{ return "ProjPage"; }
	string	getStatus( );
	string	calcId( );
	string	calcLang( ) const;
	bool	calcProgTr( )		{ return cfg("PR_TR").getB(); }
	string	calcProg( ) const;
	string	calcProgStors( const string &attr = "" );
	int	calcPer( ) const;
	string	ownerFullId( bool contr = false ) const;
	int	prjFlags( ) const	{ return mFlgs; }
	string	parentAddr( ) const	{ return cfg("PARENT").getS(); }
	string	proc( ) const		{ return cfg("PROC").getS(); }
	int	timeStamp( );

	void setIco( const string &ico )	{ cfg("ICO").setS(ico); }
	void setCalcLang( const string &lng );
	void setCalcProgTr( bool vl )		{ cfg("PR_TR") = vl; }
	void setCalcProg( const string &prg );
	void setCalcPer( int vl )		{ mProcPer = vl; modif(); }
	void setParentAddr( const string &nm );
	void setPrjFlags( int val );

	// Storing
	void loadIO( );
	void saveIO( );

	void setEnable( bool val, bool force = false );

	// Include widgets
	void wdgAdd( const string &wid, const string &name, const string &path, bool force = false );
	AutoHD<Widget> wdgAt( const string &wdg, int lev = -1, int off = 0 ) const;

	// Pages
	void pageList( vector<string> &ls ) const;
	bool pagePresent( const string &id ) const		{ return chldPresent(mPage,id); }
	AutoHD<Page> pageAt( const string &id ) const;
	string pageAdd( const string &id, const string &name, const string &orig = "" );
	void pageAdd( Page *iwdg );
	void pageDel( const string &id, bool full = false )	{ chldDel(mPage, id, -1, full?NodeRemove:NodeNoFlg); }

	// Data access
	void resourceList( vector<string> &ls );
	string resourceGet( const string &id, string *mime = NULL, int off = -1, int *size = NULL, bool noParent = false ) const;
	void resourceSet( const string &id, const string &data, const string &mime = "" );

	void procChange( bool src = true );

	void inheritAttr( const string &attr = "" );

	Page	*ownerPage( ) const;
	Project	*ownerProj( ) const;

	TVariant stlReq( Attr &a, const TVariant &vl, bool wr );

    public:
	//Attributes
	bool	manCrt;		//Manual created, mostly for child widget's modification clear after it's inheritance

    protected:
	//Methods
	void postEnable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &co, const TVariant &pc );

	// Storing
	void load_( TConfig *cfg );
	void save_( );
	void wClear( );

	void setWModif( Attr *a = NULL, bool force = false )
	{ if(!a || !(a->flgGlob()&Attr::NotStored) || force) { modif(); Widget::setWModif(a); } }
	TVariant vlGet( Attr &a );

	bool cntrCmdGeneric( XMLNode *opt );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cntrCmdLinks( XMLNode *opt, bool lnk_ro = false );

    private:
	//Attributes
	int	mPage;		//Page container identifier
	int64_t	&mFlgs,		//Project's flags
		&mProcPer,	//Process period
		&mTimeStamp;
	string	mParentAddrPrev;//Previous parent name after successful enable
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

	TCntrNode &operator=( const TCntrNode &node );

	// Main parameters
	string	addr( ) const;
	string	ico( ) const;
	string	type( )		{ return "ProjLink"; }
	string	calcId( );
	string	calcLang( ) const;
	string	calcProg( ) const;
	string	calcProgStors( const string &attr = "" );
	int	calcPer( ) const;
	string	parentAddr( ) const	{ return cfg("PARENT").getS(); }

	void setEnable( bool val, bool force = false );
	void setParentAddr( const string &isw );

	// Storing
	void loadIO( );
	void saveIO( );

	// Data access
	void resourceList( vector<string> &ls );
	string resourceGet( const string &id, string *mime = NULL, int off = -1, int *size = NULL, bool noParent = false ) const;
	void resourceSet( const string &id, const string &data, const string &mime = "" );

	void procChange( bool src = true );

	AutoHD<Widget> wdgAt( const string &wdg, int lev = -1, int off = 0 ) const;

	void inheritAttr( const string &attr = "" );

	Page &ownerPage( ) const;

    protected:
	//Methods
	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &co, const TVariant &pc )	{ modif(); return true; }

	// Storing
	void load_( TConfig *cfg );
	void save_( );
	void wClear( );

	void setWModif( Attr *a = NULL, bool force = false )
	{ if(!a || !(a->flgGlob()&Attr::NotStored) || force) { modif(); Widget::setWModif(a); } }

	void cntrCmdProc( XMLNode *opt );	//Control interface command process
};

}

#endif //PROJECT_H
