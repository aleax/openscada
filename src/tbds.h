
//OpenSCADA file: tbds.h
/***************************************************************************
 *   Copyright (C) 2003-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef TBDS_H
#define TBDS_H

#define SDB_VER		24		//BDS type modules version
#define SDB_ID		"BD"

#define SEEK_PRELOAD_LIM	100

#include <stdio.h>

#include <string>
#include <vector>
#include <deque>

#include "resalloc.h"
#include "tsubsys.h"
#include "tvariant.h"
#include "tconfig.h"

using std::string;
using std::vector;
using std::deque;

namespace OSCADA
{

//*************************************************
//* TTable                                        *
//*************************************************
class TBD;

class TTable : public TCntrNode
{
    public:
	//Data
	// Flags of the SQL requests
	enum SQLFeqFlag {
	    SQLNoFlg 		= 0,
	    SQLOrderForSeek	= 0x01,
	    SQLFirstSkipForSeek	= 0x02
	};

	// Item of the table structure
	class TStrIt {
	    public:
	    TStrIt( ) : key(0)	{ }
	    TStrIt( const string &inm, const string itp, int ikey, const string &idef = "" ) :
		nm(inm), tp(itp), key(ikey), def(idef) { }

	    string nm, tp, def;
	    int key;
	};

	//Public methods
	TTable( const string &name );
	virtual ~TTable( );

	TCntrNode &operator=( const TCntrNode &node );

	string	name( )		{ return mName.c_str(); }
	string	fullDBName( );
	time_t	lstUse( )	{ return mLstUse; }

	// Common requests
	virtual void fieldStruct( TConfig &cfg )
	{ throw TError(nodePath().c_str(),_("Function '%s' is not supported!"),"fieldStruct"); }
	virtual bool fieldSeek( int row, TConfig &cfg, const string &cacheKey = "" )
	{ throw TError(nodePath().c_str(),_("Function '%s' is not supported!"),"fieldSeek"); }
	virtual void fieldGet( TConfig &cfg )
	{ throw TError(nodePath().c_str(),_("Function '%s' is not supported!"),"fieldGet"); }
	virtual void fieldSet( TConfig &cfg )
	{ throw TError(nodePath().c_str(),_("Function '%s' is not supported!"),"fieldSet"); }
	virtual void fieldDel( TConfig &cfg )
	{ throw TError(nodePath().c_str(),_("Function '%s' is not supported!"),"fieldDel"); }

	// Internal requests
	virtual void fieldFix( TConfig &cfg, const string &langLs = "" )	{ }

	TBD &owner( ) const;

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang );

	// SQL specific common operations
	virtual string getSQLVal( TCfg &cf, uint8_t RqFlg = 0 ) { return ""; }
	virtual void setSQLVal( TCfg &cf, const string &vl, bool tr = false ) { }

	bool fieldSQLSeek( int row, TConfig &cfg, const string &cacheKey, int flags = SQLNoFlg );
	void fieldSQLGet( TConfig &cfg );
	void fieldSQLSet( TConfig &cfg );
	void fieldSQLDel( TConfig &cfg );

	//Protected attributes
	time_t	mLstUse;

	vector<TStrIt> tblStrct;
	map<string, vector< vector<string> > >	seekSess;

    private:
	//Private methods
	const char *nodeName( ) const	{ return mName.c_str(); }

	//Private attributes
	const string	mName;
	bool		notFullShow;
	int		tblOff, tblSz;
};

//************************************************
//* TBD                                          *
//************************************************
class TTypeBD;

class TBD : public TCntrNode, public TConfig
{
    public:
	//Public methods
	TBD( const string &iid, TElem *cf_el );
	virtual ~TBD( );

	TCntrNode &operator=( const TCntrNode &node );

	string	id( )			{ return mId; }
	string	fullDBName( );
	string	name( );
	string	dscr( )			{ return cfg("DESCR").getS(); }
	string	addr( ) const		{ return cfg("ADDR").getS(); }
	string	codePage( )		{ return cfg("CODEPAGE").getS(); }
	int	lsPr( )			{ return cfg("LS_PR").getI(); }
	double	trTm_ClsOnOpen( )	{ return mTrTm_ClsOnOpen; }
	double	trTm_ClsOnReq( )	{ return mTrTm_ClsOnReq; }
	int	trPr_ClsTask( )		{ return mTrPr_ClsTask; }

	bool enableStat( ) const	{ return mEn; }
	bool toEnable( )		{ return mToEn; }
	bool disabledByUser( )		{ return mDisByUser; }

	void setName( const string &nm )	{ cfg("NAME").setS(nm); }
	void setDscr( const string &dscr )	{ cfg("DESCR").setS(dscr); }
	void setAddr( const string &addr )	{ cfg("ADDR").setS(addr); }
	void setCodePage( const string &cp )	{ cfg("CODEPAGE").setS(cp); }
	void setLsPr( int vl )			{ cfg("LS_PR").setI(vl); }
	void setToEnable( bool vl )		{ mToEn = vl; }

	virtual void enable( );
	virtual void disable( );

	// Opened DB tables
	virtual void allowList( vector<string> &list ) const
	{ throw TError(nodePath().c_str(),_("Function '%s' is not supported!"),"allowList"); }
	void list( vector<string> &list ) const		{ chldList(mTbl, list); }
	bool openStat( const string &table ) const	{ return chldPresent(mTbl, table); }
	virtual void open( const string &table, bool create );
	virtual void close( const string &table, bool del = false, long tm = -1 )	{ chldDel(mTbl, table, tm, del?NodeRemove:NodeNoFlg); }
	AutoHD<TTable> at( const string &name ) const	{ return chldAt(mTbl, name); }

	// SQL request interface
	virtual void sqlReq( const string &req, vector< vector<string> > *tbl = NULL, char intoTrans = EVAL_BOOL )
	{ throw TError(nodePath().c_str(),_("Function '%s' is not supported!"),"sqlReq"); }

	virtual void transCloseCheck( )		{ }

	TTypeBD &owner( ) const;

	//Public attributes
	ResMtx	resTbls, connRes;

    protected:
	//Protected methods
	virtual TTable *openTable( const string &table, bool create )
	{ throw TError(nodePath().c_str(),_("Function '%s' is not supported!"),"openTable"); }

	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &co, const TVariant &pc );

	void load_( TConfig *cfg );
	void save_( );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang );

	AutoHD<TCntrNode> chldAt( int8_t igr, const string &name, const string &user = "" ) const;

	//Protected attributes
	bool	mEn;

    private:
	//Private methods
	const char *nodeName( ) const	{ return mId.getSd(); }

	static void *Task( void * );

	//Private attributes
	// Base options
	TCfg	&mId;	//ID
	char	&mToEn;
	double	&mTrTm_ClsOnOpen, &mTrTm_ClsOnReq;
	int64_t	&mTrPr_ClsTask;

	// Special options
	int	mTbl;

	string	userSQLReq;
	vector< vector<string> > userSQLResTbl;
	char	userSQLTrans;

	bool	mDisByUser;	//Disabled by user to prevent of restoring the enabling
};

//************************************************
//* TTypeBD                                      *
//************************************************
class TBDS;

class TTypeBD : public TModule
{
    public:
	//Public methods
	TTypeBD( const string &id );
	virtual ~TTypeBD( );

	void   modInfo( vector<string> &list );
	string modInfo( const string &name );

	virtual string features( )	{ return ""; }
	virtual int lsPr( )		{ return 0; }

	bool fullDeleteDB( )		{ return fullDBDel; }

	// Opened DB
	void list( vector<string> &list ) const		{ chldList(mDB, list); }
	bool openStat( const string &idb ) const	{ return chldPresent(mDB, idb); }
	string open( const string &id );
	void close( const string &id, bool erase = false ) { chldDel(mDB, id, -1, erase?NodeRemove:NodeNoFlg); }
	AutoHD<TBD> at( const string &id ) const	{ return chldAt(mDB, id); }

	TBDS &owner( ) const;

    private:
	//Private methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	virtual TBD *openBD( const string &id )	{ throw TError(nodePath().c_str(),_("Function '%s' is not supported!"),"openBD"); }

	//Private attributes
	bool	fullDBDel;
	int	mDB;
};

//************************************************
//* TBDS                                         *
//************************************************
class TSYS;

class TBDS : public TSubSYS
{
    public:
	//Data
	enum DBLsFlg {
	    LsNoFlg	= 0,
	    LsCheckSel	= 0x01,
	    LsInclGenFirst = 0x02
	};
	enum ReqGenFlg {
	    NoFlg	= 0,
	    NoException	= 0x01,		//Do not generate the exceptions
	    OnlyCfg	= 0x02,		//Force request to the configuration file, in genPrmSet() and dataSet() only
	    // Specific ones
	    UseTranslation = 0x04,	//Use translation for request, in genPrmGet() and genPrmSet() only
	    UseCache	= 0x04,		//Use the cache, in dataSeek() only
	    UseAllKeys	= 0x04		//Use all keys, in dataDel() only
	};

	//Generic static block
	static string fullDBSYS( )	{ return "*.*.SYS"; }
	static string fullDB( )		{ return "*.*.DB"; }

	static string realDBName( const string &bdn, bool back = false );
	static string dbPart( const string &bdn, bool tbl = false );

	static void dbList( vector<string> &ls, char flags = LsNoFlg );

	// Open/close table
	static AutoHD<TTable> tblOpen( const string &bdn, bool create = false );
	static void tblClose( const string &bdn, bool del = false );

	// Get Data from DB or config file. If <tbl> cleaned then load from config-file
	static bool dataSeek( const string &bdn, const string &path, int lev, TConfig &cfg, char flags = NoFlg, XMLNode *localCfgCtx = NULL );
	static bool dataGet( const string &bdn, const string &path, TConfig &cfg, char flags = NoFlg, XMLNode *localCfgCtx = NULL );
	static bool dataSet( const string &bdn, const string &path, TConfig &cfg, char flags = NoFlg, XMLNode *localCfgCtx = NULL );
	static bool dataDel( const string &bdn, const string &path, TConfig &cfg, char flags = NoFlg );
	static bool dataDelTbl( const string &bdn, const string &path = "", char flags = NoFlg );

	// Generic Parameters
	static string genPrmGet( const string &path, const string &oval = "", const string &user = "root", char flags = NoFlg );
	static void genPrmSet( const string &path, const string &val, const string &user = "root", char flags = NoFlg );

	//Public methods
	TBDS( );
	~TBDS( );

	int subVer( )		{ return SDB_VER; }

	int tblLifeTime( )	{ return mTblLifeTime; }
	void setTblLifeTime( int vl )	{ mTblLifeTime = vmax(10, vmin(1000,vl)); modif(); }

	void perSYSCall( unsigned int cnt );

	TElem &openDB_E( )	{ return elDB; }

	AutoHD<TTypeBD> at( const string &iid ) const	{ return modAt(iid); }

	string optDescr( );

    protected:
	void load_( );
	void save_( );

    private:
	//Private methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Private attributes
	static TElem elSYS;
	TElem	elDB;
	int	mTblLifeTime;			//Tables lifetime
};

}

#endif // TBDS_H
