
//OpenSCADA system file: tbds.h
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#include "tsubsys.h"
#include "tconfig.h"

using std::string;
using std::vector;
using std::deque;

class TBD;

class TTable : public TCntrNode
{
    public:
	TTable( const string &name );
	virtual ~TTable();

	string &name()	{ return(m_name); }

	virtual bool fieldSeek( int row, TConfig &cfg )
        { throw TError(nodePath().c_str(),Mess->I18N("Function <%s> no support!"),"fieldSeek"); }		
	virtual void fieldGet( TConfig &cfg )
        { throw TError(nodePath().c_str(),Mess->I18N("Function <%s> no support!"),"fieldGet"); }
	virtual void fieldSet( TConfig &cfg )
        { throw TError(nodePath().c_str(),Mess->I18N("Function <%s> no support!"),"fieldSet"); }
	virtual void fieldDel( TConfig &cfg )
        { throw TError(nodePath().c_str(),Mess->I18N("Function <%s> no support!"),"fieldDel"); }
	
	TBD &owner()	{ return *(TBD *)nodePrev(); }	
    
    private:
	//Methods
	string nodeName()	{ return m_name; }

	//Attributes
	string m_name;
};    

class TTipBD;

class TBD : public TCntrNode, public TConfig
{
    public:
	TBD( const string &iid, TElem *cf_el );
	virtual ~TBD();
	
	const string &id()	{ return m_id; }
	string name();
	string dscr()   	{ return m_dscr; }
	string addr()   	{ return m_addr; }
	bool   create()		{ return m_creat; }
	
	bool enableStat()       { return m_en; }
	bool toEnable()        	{ return m_toen; }
	
	void name( const string &inm )  { m_name = inm; }
	void dscr( const string &idscr ){ m_dscr = idscr; }
	void addr( const string &iaddr ){ m_addr = iaddr; }
	void create( bool ivl )		{ m_creat= ivl; }
	void toEnable( bool ivl )	{ m_toen = ivl; }
	
	virtual void enable( );
        virtual void disable( );
        virtual void load( );
        virtual void save( );
	
	//- Opened DB tables -
	void list( vector<string> &list )	{ chldList(m_tbl,list); }
	bool openStat( const string &table )	{ return chldPresent(m_tbl,table); }
	void open( const string &table, bool create );
	void close( const string &table, bool del = false )	{ chldDel(m_tbl,table,-1,del); }
	AutoHD<TTable> at( const string &name )	{ return chldAt(m_tbl,name); }
	
	//- SQL request interface -
	virtual void sqlReq( const string &req, vector< vector<string> > *tbl = NULL )
	{ throw TError(nodePath().c_str(),Mess->I18N("Function <%s> no support!"),"sqlReq"); }

	TTipBD &owner()		{ return *(TTipBD *)nodePrev(); }	
	
    protected:
	virtual TTable *openTable( const string &table, bool create )
	{ throw TError(nodePath().c_str(),Mess->I18N("Function <%s> no support!"),"openTable"); }    
    
	void preDisable(int flag);
        void postDisable(int flag);
	
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
    private:
	//Methods
	void postEnable( );
	string nodeName( )	{ return m_id; }
	
	//Attributes
	//- Base options -
	string  &m_id,          //ID
    		&m_name,        //Name
    		&m_dscr,        //Description
    		&m_addr;        //Individual address
	bool	&m_toen;
	
	bool    m_en, m_creat;
	//- Special options -
	int	m_tbl;
};

class TBDS;

class TTipBD : public TModule
{
    public:
	TTipBD( );
	virtual ~TTipBD( );
	
	bool fullDeleteDB()	{ return full_db_del; }
	
	//- Opened DB -
	void list( vector<string> &list )	{ chldList(m_db,list); }
        bool openStat( const string &idb )	{ return chldPresent(m_db,idb); }
	void open( const string &iid );
	void close( const string &iid, bool erase = false )	{ chldDel(m_db,iid,-1,erase); }
	AutoHD<TBD> at( const string &name )	{ return chldAt(m_db,name); }
	
	TBDS &owner()	{ return (TBDS&)TModule::owner(); }    	
	
    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
            
	virtual TBD *openBD( const string &iid )
	{throw TError(nodePath().c_str(),Mess->I18N("Function <%s> no support!"),"openBD"); }

	//Attributes
	bool	full_db_del;	
	int	m_db;
};

class TSYS;

class TBDS : public TSubSYS, public TElem
{         
    public:
	TBDS( );    
       	~TBDS( );

	int subVer( ) { return(VER_BD); }
	void subLoad( );
	void subSave( );
	
	//- Open/close table. -
	AutoHD<TTable> open( const string &bdn, bool create = false );
	void close( const string &bdn, bool del = false );

	//- Get Data from DB or config file. If <tbl> cleaned then load from config file -
	bool dataSeek( const string &bdn, const string &path, int lev, TConfig &cfg );
	bool dataGet( const string &bdn, const string &path, TConfig &cfg );
	void dataSet( const string &bdn, const string &path, TConfig &cfg );
	void dataDel( const string &bdn, const string &path, TConfig &cfg );
	
	//- Generic DB table -
	static string genDBGet(const string &path, const string &oval = "", const string &user = "root", bool onlyCfg = false );
	static void genDBSet(const string &path, const string &val, const string &user = "root");
	
	string SysBD();
	string openBD();
	
	TElem &openDB_E()	{ return el_db; }
	
	AutoHD<TTipBD> at(const string &iid)	{ return modAt(iid); }

	string optDescr(  );

    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	//Attributes
	TElem	el_db;
	int	genDBCacheRes;
	deque<TConfig*> genDBCache;
};

#endif // TBDS_H
