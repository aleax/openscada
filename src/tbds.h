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

#include "thd.h"
#include "terror.h"
#include "tmodule.h"
#include "tgrpmodule.h"

using std::string;
using std::vector;

class TBD;

class TTable
{
    public:
	TTable( const string &name, TBD *owner );
	virtual ~TTable();

	string &name(){ return(m_name); }

        virtual void fieldList( const string &key, vector<string> &fields )
        { throw TError(_err,"fieldList",o_name); }
	virtual void fieldGet( TConfig &cfg )
        { throw TError(_err,"fieldGet",o_name); }
	virtual void fieldSet( TConfig &cfg )
        { throw TError(_err,"fieldSet",o_name); }
	virtual void fieldDel( TConfig &cfg )
        { throw TError(_err,"fieldDel",o_name); }
	
	TBD &owner(){ return *m_owner; }
    
    private:
	string m_name;
	TBD    *m_owner;
	
	static const char *o_name;
	static char *_err;
};    

class TBD
{
    public:
	TBD( const string &name );
	virtual ~TBD();

	string &name() { return( m_name ); }
	// List opened bd tables
	void list( vector<string> &list ) { m_hd_tb.objList( list ); }
	// Open stat
	bool openStat( const string &table )
	{ return m_hd_tb.objAvoid(table); }
	// Open bd table. if create = true then try create if no avoid table
	void open( const string &table, bool create );
	// Save and Close table
	void close( const string &table )
	{ delete (TTable *)m_hd_tb.objDel( table, 5 ); }
	// Delete table
	void del( const string &table ){ delTable(table); }
	// Go Table
	AutoHD<TTable> at( const string &name )
        { AutoHD<TTable> obj( name, m_hd_tb ); return obj; }
	
    private:
	virtual TTable *openTable( const string &table, bool create )
	{ throw TError("(%s) function openTable no support!",o_name); }
	virtual void delTable( const string &table )
	{ throw TError("(%s) function delTable no support!",o_name); }
	
    private:
	THD          m_hd_tb;
	string       m_name;
    
	static const char *o_name;
};

class TBDS;

class TTipBD : public TModule
{
    /** Public methods: */
    public:
	TTipBD(  );
	virtual ~TTipBD(  );
	
	// List opened bd
	void list( vector<string> &list ) { m_hd_bd.objList( list ); }
	// Open stat
        bool openStat( const string &bd )
        { return m_hd_bd.objAvoid(bd); }			
	// Open bd. if create = true then try create if no avoid bd
	void open( const string &name, bool create );
	// Save and Close bd
	void close( const string &name )
	{ delete (TBD *)m_hd_bd.objDel( name, 5 ); }
	// Delete bd
	void del( const string &name ){ delBD(name); }
	// Go BD
	AutoHD<TBD> at( const string &name )
        { AutoHD<TBD> obj( name, m_hd_bd ); return obj; }
		
    /** Public atributes:: */
    private:        
	virtual TBD *openBD( const string &name, bool create )
	{throw TError("(%s) Function \"openBD\" no support!",o_name); }
	virtual void delBD( const string &name )
	{throw TError("(%s) Function \"delBD\" no support!",o_name); }
	
    /** Private atributes:: */
    private:
	THD          m_hd_bd;
	
	static const char     *o_name;
};

class TKernel;

class TBDS : public TGRPModule
{         
    /** Public methods: */
    public:
	class SName
	{
	    public:
		SName( const string &m_tp, const string &m_bd, const string &m_tbl ) : tp(m_tp), bd(m_bd), tbl(m_tbl) { }
		string tp;
		string bd;
		string tbl;
	};
	
	TBDS( TKernel *app );    
       	~TBDS(  );

	int gmdVer( ) { return(VER_BD); }
	
	// Open table. if create = true then try create if no avoid bd and table
	AutoHD<TTable> open( const TBDS::SName &bd_t, bool create = false );
	// Save and Close table	
	void close( const TBDS::SName &bd_t, bool only_table = true );

	void gmdCheckCommandLine( );
	void gmdUpdateOpt();
	
	string optDescr(  );

    /** Private methods: */
    private:
	//================== Controll functions ========================
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &a_path, XMLNode *opt );
	
    /** Private atributes: */
    private:	
	static const char *o_name;
};

#endif // TBDS_H
