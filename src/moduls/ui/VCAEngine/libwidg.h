
//OpenSCADA module UI.VCAEngine file: libwidg.h
/***************************************************************************
 *   Copyright (C) 2006-2022 by Roman Savochenko, <roman@oscada.org>       *
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

	TCntrNode &operator=( const TCntrNode &node );

	string id( ) const	{ return mId; }			//Identifier
	string name( ) const;					//Name
	string descr( ) const	{ return cfg("DESCR").getS(); }	//Description
	string ico( ) const	{ string sval = cfg("ICO").getS(); return sval.size() ? sval : SYS->ico(); }	//Icon
	string getStatus( );

	bool isStdStorAddr( ) const		{ return (tbl() == ("wlb_"+id())); }	//????[v1.0] Remove
	string DB( bool qTop = false ) const	{ return storage(mDB, qTop); }
	string tbl( ) const	{ return cfg("DB_TBL").getS().empty() ? ("wlb_"+id()) : cfg("DB_TBL").getS(); }
	string fullDB( bool qTop = false ) const{ return DB(qTop)+'.'+tbl(); }

	void setName( const string &it )	{ cfg("NAME").setS(it); }
	void setDescr( const string &it )	{ cfg("DESCR").setS(it); }
	void setIco( const string &it )		{ cfg("ICO").setS(it); }

	void setDB( const string &vl, bool qTop = false ) { setStorage(mDB, vl, qTop); if(!qTop) modifG(); }
	void setTbl( const string &it )		{ cfg("DB_TBL").setS(it); }
	void setFullDB( const string &it );

	// Enable stat
	bool enable( ) const	{ return mEnable; }
	void setEnable( bool val, bool force = false );

        // Mime data access
	void resourceDataList( vector<string> &list, const string &idb = "" ) const;
	bool resourceDataGet( const string &id, string &mimeType, string *mimeData = NULL, const string &idb = "", int off = -1, int *size = NULL ) const;
	void resourceDataSet( const string &id, const string &mimeType, const string &mimeData, const string &idb = "" );
	void resourceDataDel( const string &id, const string &idb = "" );

	// Widgets
	void list( vector<string> &ls ) const		{ chldList(mWdg, ls); }
	bool present( const string &id ) const		{ return chldPresent(mWdg, id); }
	AutoHD<LWidget> at( const string &id ) const;
	string add( const string &id, const string &name, const string &orig = "" );
	void add( LWidget *iwdg );
	void del( const string &id, bool full = false )	{ chldDel(mWdg, id, -1, full?NodeRemove:NodeNoFlg); }

    protected:
	//Methods
	const char *nodeName( ) const	{ return mId.getSd(); }
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void load_( TConfig *cfg );
	void save_( );

	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &co, const TVariant &pc )	{ modif(); return true; }

	AutoHD<TCntrNode> chldAt( int8_t igr, const string &name, const string &user = "" ) const;

	//Attributes
	int	mWdg;

    private:
	//Attributes
	TCfg	&mId;
	string	mDB,
		mDB_MimeSrc;	//After the copy mostly
	bool	mEnable;
	bool	passAutoEn;
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

	TCntrNode &operator=( const TCntrNode &node );

	string	addr( ) const;
	string	ico( ) const;
	string	type( )		{ return "LibWidget"; }
	string	getStatus( );
	string	calcId( );
	string	calcLang( ) const;
	bool	calcProgTr( )		{ return cfg("PR_TR").getB(); }
	string	calcProg( ) const;
	string	calcProgStors( const string &attr = "" );
	int	calcPer( ) const;
	string	parentAddr( ) const	{ return cfg("PARENT").getS(); }
	string	proc( ) const		{ return cfg("PROC").getS(); }
	int64_t	timeStamp( )		{ return mTimeStamp; }

	void setEnable( bool val, bool force = false );
	void setIco( const string &ico )	{ cfg("ICO").setS(ico); }
	void setCalcLang( const string &lng );
	void setCalcProgTr( bool vl )		{ cfg("PR_TR") = vl; }
	void setCalcProg( const string &prg );
	void setCalcPer( int vl )		{ mProcPer = vmax(PerVal_UserMin, vl); modif(); }
	void setParentAddr( const string &nm );
	void setEnableByNeed( )			{ enableByNeed = true; modifClr(); }

	// Include widgets
	void wdgAdd( const string &wid, const string &name, const string &path, bool force = false );
	AutoHD<CWidget> wdgAt( const string &wdg ) const;

	// Storing
	void loadIO( );
	void saveIO( );

	// Data access
	void resourceList( vector<string> &ls );
	string resourceGet( const string &id, string *mime = NULL, int off = -1, int *size = NULL, bool noParent = false ) const;
	void resourceSet( const string &id, const string &data, const string &mime = "" );

	void procChange( bool src = true );

	void inheritAttr( const string &attr = "" );

	WidgetLib &ownerLib( ) const;

	bool	enableByNeed;	//Load and enable by need
	ResMtx &funcM( )	{ return mFuncM; }

	TVariant stlReq( Attr &a, const TVariant &vl, bool wr );

    protected:
	//Methods
	void postDisable( int flag );
	bool cfgChange( TCfg &co, const TVariant &pc );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	// Storing
	void load_( TConfig *cfg );
	void save_( );
	void wClear( );

	void setWModif( Attr *a = NULL, bool force = false )
	{ if(!a || !(a->flgGlob()&Attr::NotStored) || force) { modif(); Widget::setWModif(a); } }

    private:
	//Attributes
	int64_t	&mProcPer,	//Widget period
		&mTimeStamp;
	string	mParentAddrPrev;//Previous parent name after successful enable
	ResMtx	mFuncM;
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

	TCntrNode &operator=( const TCntrNode &node );

	// Main parameters
	string	addr( ) const;
	string	ico( ) const;
	string	type( )		{ return "LibLink"; }
	string	calcId( );
	string	calcLang( ) const;
	string	calcProg( ) const;
	string	calcProgStors( const string &attr = "" );
	int	calcPer( ) const;
	string	parentAddr( ) const	{ return cfg("PARENT").getS(); }

	void setEnable( bool val, bool force = false );
	void setCalcPer( int vl )		{ mProcPer = vmax(PerVal_UserMin, vl); modif(); }
	void setParentAddr( const string &isw );

	// Storing
	void loadIO( );
	void saveIO( );

	// Data access
	void resourceList( vector<string> &ls );
	string resourceGet( const string &id, string *mime = NULL, int off = -1, int *size = NULL, bool noParent = false ) const;
	void resourceSet( const string &id, const string &data, const string &mime = "" );

	void procChange( bool src = true );

	void inheritAttr( const string &attr = "" );

	LWidget &ownerLWdg( ) const;

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

    private:
	//Attributes
	int64_t	&mProcPer;	//Widget period

};

}

#endif //LIBWDG_H
