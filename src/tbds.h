/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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

#include "tgrpmodule.h"
#include "tconfig.h"

using std::string;
using std::vector;

class TBD;

class TTable : public TCntrNode
{
    public:
	TTable( const string &name, TBD *owner );
	virtual ~TTable();

	string &name(){ return(m_name); }

	virtual bool fieldSeek( int row, TConfig &cfg )
        { throw TError(_err,"fieldSeek",__func__); }		
	virtual void fieldGet( TConfig &cfg )
        { throw TError(_err,"fieldGet",__func__); }
	virtual void fieldSet( TConfig &cfg )
        { throw TError(_err,"fieldSet",__func__); }
	virtual void fieldDel( TConfig &cfg )
        { throw TError(_err,"fieldDel",__func__); }
	
	TBD &owner(){ return *m_owner; }
    
    private:
	string nodeName(){ return m_name; }

    private:
	string m_name;
	TBD    *m_owner;
	
	static char *_err;
};    

class TBD : public TCntrNode
{
    public:
	TBD( const string &name );
	virtual ~TBD();

	string &name() { return( m_name ); }
	
	//Opened DB tables
	void list( vector<string> &list )	{ chldList(m_tbl,list); }
	bool openStat( const string &table )	{ return chldAvoid(m_tbl,table); }
	void open( const string &table, bool create );
	void close( const string &table )	{ return chldDel(m_tbl,table); }
	void del( const string &table )		{ delTable(table); }
	AutoHD<TTable> at( const string &name )	{ return chldAt(m_tbl,name); }
	
    private:
	string nodeName(){ return m_name; }
	
	virtual TTable *openTable( const string &table, bool create )
	{ throw TError("(%s) function openTable no support!",__func__); }
	virtual void delTable( const string &table )
	{ throw TError("(%s) function delTable no support!",__func__); }
	
    private:
	int	m_tbl;
	string	m_name;
};

class TBDS;

class TTipBD : public TModule
{
    /** Public methods: */
    public:
	TTipBD(  );
	virtual ~TTipBD(  );
	
	// Opened DB
	void list( vector<string> &list )	{ chldList(m_db,list); }
        bool openStat( const string &db )	{ return chldAvoid(m_db,db); }
	void open( const string &name, bool create );
	void close( const string &name )	{ return chldDel(m_db,name); }
	void del( const string &name )		{ delBD(name); }
	AutoHD<TBD> at( const string &name )	{ return chldAt(m_db,name); }
	
    /** Public atributes:: */
    private:
	//================== Controll functions ========================
        void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
            
	virtual TBD *openBD( const string &name, bool create )
	{throw TError("(%s) Function \"openBD\" no support!",__func__); }
	virtual void delBD( const string &name )
	{throw TError("(%s) Function \"delBD\" no support!",__func__); }
	
    /** Private atributes:: */
    private:
	int	m_db;
};

class TKernel;

class TBDS : public TGRPModule
{         
    /** Public methods: */
    public:
	class SName
	{
	    public:
		SName( const char *m_tp, const char *m_bd, const char *m_tbl ) : tp(m_tp), bd(m_bd), tbl(m_tbl) { }
		string tp;
		string bd;
		string tbl;
	};
	
	TBDS( TKernel *app );    
       	~TBDS(  );

	int gmdVer( ) { return(VER_BD); }
	void gmdLoad( );
	
	// Open table. if create = true then try create if no avoid bd and table
	AutoHD<TTable> open( const TBDS::SName &bd_t, bool create = false );
	// Save and Close table	
	void close( const TBDS::SName &bd_t );

	//Get Data from DB or config file. If <tbl> cleaned then load from config file
	bool dataSeek( AutoHD<TTable> &tbl, const string &path, int lev, TConfig &cfg );

	string optDescr(  );

    /** Private methods: */
    private:
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
};

#endif // TBDS_H
