
//OpenSCADA system file: tbds.h
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
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
 
#ifndef TBDS_H
#define TBDS_H

#define  VER_BD 1    //BDS type modules version

#include <stdio.h>

#include <string>
#include <vector>
#include <deque>

#include "resalloc.h"
#include "tsubsys.h"
#include "tconfig.h"

using std::string;
using std::vector;
using std::deque;

//*************************************************
//* TTable                                        *
//*************************************************
class TBD;

class TTable : public TCntrNode
{
    public:
	//Public methods
	TTable( const string &name );
	virtual ~TTable();

	string &name()		{ return m_name; }

	virtual void fieldStruct( TConfig &cfg )
        { throw TError(nodePath().c_str(),_("Function <%s> no support!"),"fieldStruct"); }
	virtual bool fieldSeek( int row, TConfig &cfg )
        { throw TError(nodePath().c_str(),_("Function <%s> no support!"),"fieldSeek"); }
	virtual void fieldGet( TConfig &cfg )
        { throw TError(nodePath().c_str(),_("Function <%s> no support!"),"fieldGet"); }
	virtual void fieldSet( TConfig &cfg )
        { throw TError(nodePath().c_str(),_("Function <%s> no support!"),"fieldSet"); }
	virtual void fieldDel( TConfig &cfg )
        { throw TError(nodePath().c_str(),_("Function <%s> no support!"),"fieldDel"); }
	
	TBD &owner( )		{ return *(TBD *)nodePrev(); }

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
    
    private:
	//Private methods
	string nodeName()	{ return m_name; }

	//Private attributes
	string m_name;
};    

//************************************************
//* TBD                                          *
//************************************************ 
class TTipBD;

class TBD : public TCntrNode, public TConfig
{
    public:
	//Public methods
	TBD( const string &iid, TElem *cf_el );
	virtual ~TBD( );
	
	const string &id( )		{ return m_id; }
	string       name( );
	const string &dscr( )   	{ return m_dscr; }
	const string &addr( )   	{ return m_addr; }
	const string &codePage( )	{ return m_codepage; } 
	
	bool enableStat( )       	{ return m_en; }
	bool toEnable( )        	{ return m_toen; }
	
	void setName( const string &inm )  	{ m_name = inm; }
	void setDscr( const string &idscr )	{ m_dscr = idscr; }
	void setAddr( const string &iaddr )	{ m_addr = iaddr; }
	void setCodePage( const string &icp )	{ m_codepage = icp; }
	void setToEnable( bool ivl )		{ m_toen = ivl; }
	
	virtual void enable( );
        virtual void disable( );
        virtual void load( );
        virtual void save( );
	
	//- Opened DB tables -
	virtual void allowList( vector<string> &list )
        { throw TError(nodePath().c_str(),_("Function <%s> no support!"),"allowList"); }
	void list( vector<string> &list )	{ chldList(m_tbl,list); }
	bool openStat( const string &table )	{ return chldPresent(m_tbl,table); }
	void open( const string &table, bool create );
	void close( const string &table, bool del = false )	{ chldDel(m_tbl,table,-1,del); }
	AutoHD<TTable> at( const string &name )	{ return chldAt(m_tbl,name); }
	
	//- SQL request interface -
	virtual void sqlReq( const string &req, vector< vector<string> > *tbl = NULL )
	{ throw TError(nodePath().c_str(),_("Function <%s> no support!"),"sqlReq"); }

	TTipBD &owner( )		{ return *(TTipBD *)nodePrev(); }	
	
    protected:
	//Protected methods
	virtual TTable *openTable( const string &table, bool create )
	{ throw TError(nodePath().c_str(),_("Function <%s> no support!"),"openTable"); }    
    
	void preDisable( int flag );
        void postDisable( int flag );
	
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
    private:
	//Private methods
	void postEnable( int flag );
	string nodeName( )	{ return m_id; }
	
	//Private attributes
	//- Base options -
	string  &m_id,          //ID
    		&m_name,        //Name
    		&m_dscr,        //Description
    		&m_addr,        //Individual address
		&m_codepage;	//DB codepage
	bool	&m_toen;
	
	bool    m_en;
	//- Special options -
	int	m_tbl;
};

//************************************************
//* TTipBD                                       *
//************************************************ 
class TBDS;

class TTipBD : public TModule
{
    public:
	//Public methods
	TTipBD( );
	virtual ~TTipBD( );
	
	bool fullDeleteDB( )	{ return full_db_del; }
	
	//- Opened DB -
	void list( vector<string> &list )	{ chldList(m_db,list); }
        bool openStat( const string &idb )	{ return chldPresent(m_db,idb); }
	void open( const string &iid );
	void close( const string &iid, bool erase = false )	{ chldDel(m_db,iid,-1,erase); }
	AutoHD<TBD> at( const string &name )	{ return chldAt(m_db,name); }
	
	TBDS &owner( )		{ return (TBDS&)TModule::owner(); }    	
	
    private:
	//Private methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
            
	virtual TBD *openBD( const string &id )
	{throw TError(nodePath().c_str(),_("Function <%s> no support!"),"openBD"); }

	//Private attributes
	bool	full_db_del;	
	int	m_db;
};

//************************************************
//* TBDS                                         *
//************************************************ 
class TSYS;

class TBDS : public TSubSYS, public TElem
{         
    public:
	//Public methods
	TBDS( );    
       	~TBDS( );

	int subVer( ) 		{ return VER_BD; }
	void subLoad( );
	void subSave( );

	static string realDBName( const string &bdn );
	
	//- Open/close table. -
	AutoHD<TTable> open( const string &bdn, bool create = false );
	void close( const string &bdn, bool del = false );

	//- Get Data from DB or config file. If <tbl> cleaned then load from config file -
	bool dataSeek( const string &bdn, const string &path, int lev, TConfig &cfg );
	bool dataGet( const string &bdn, const string &path, TConfig &cfg );
	bool dataSet( const string &bdn, const string &path, TConfig &cfg );
	bool dataDel( const string &bdn, const string &path, TConfig &cfg );
	
	//- Generic DB table -
	static string genDBGet( const string &path, const string &oval = "", 
		const string &user = "root", bool onlyCfg = false );
	static void genDBSet( const string &path, const string &val, const string &user = "root" );
	
	string fullDBSYS( );
	string fullDB( );
	
	TElem &openDB_E( )	{ return el_db; }
	
	AutoHD<TTipBD> at(const string &iid)	{ return modAt(iid); }

	string optDescr( );

    private:
	//Private methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	//Private attributes
	TElem	el_db;
	Res	genDBCacheRes;
	deque<TConfig*> genDBCache;
};

#endif // TBDS_H
