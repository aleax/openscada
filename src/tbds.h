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

#define BD_ROW_STRING  0
#define BD_ROW_REAL    1
#define BD_ROW_INT     2
#define BD_ROW_BOOLEAN 3

using std::string;
using std::vector;

struct SColmAttr
{
    string   name;  // Name column
    char     tp;    // type cell (BD_FLD_STRING, BD_FLD_REAL, BD_FLD_INT, BD_FLD_BOOLEAN)
    unsigned len;   // Len in symbol for string and numberic in string 
    unsigned dec;   // Dec for real numberic
};

class TBD;

class TTable
{
    public:
	TTable( const string &name, TBD *owner );
	virtual ~TTable();

	string &name(){ return(m_name); }

	virtual void save()
	{ throw TError(_err,"save",o_name); }

	void clean() { while(nLines()) delLine(0); }
	
	virtual string getCellS( int colm, int line)
	{ throw TError(_err,"getCellS",o_name); }
	virtual double getCellR( int colm, int line)
	{ throw TError(_err,"getCellR",o_name); }
	virtual int getCellI( int colm, int line)
	{ throw TError(_err,"getCellI",o_name); }
	virtual bool getCellB( int colm, int line)
	{ throw TError(_err,"getCellB",o_name); }
	
	virtual void setCellS( int colm, int line, const string &cell)
	{ throw TError(_err,"setCellS",o_name); }
	virtual void setCellR( int colm, int line, double val)
	{ throw TError(_err,"setCellR",o_name); }
	virtual void setCellI( int colm, int line, int val)
	{ throw TError(_err,"setCellI",o_name); }
	virtual void setCellB( int colm, int line, bool val)
	{ throw TError(_err,"setCellB",o_name); }
	
	virtual int nLines( )
	{ throw TError(_err,"nLines",o_name); }
	virtual int addLine( unsigned int line)
	{ throw TError(_err,"addLines",o_name); }
	virtual void delLine( unsigned int line)
	{ throw TError(_err,"delLines",o_name); }
	
	virtual int nColums( )
	{ throw TError(_err,"nColums",o_name); }
	virtual int addColum( SColmAttr *colm )
	{ throw TError(_err,"addColum",o_name); }
	virtual void delColum( int colm)
	{ throw TError(_err,"delColum",o_name); }
	
	virtual void getColumAttr( int colm, SColmAttr *attr )
	{ throw TError(_err,"getColumAttr",o_name); }
	virtual void setColumAttr( int colm, SColmAttr *attr )
	{ throw TError(_err,"setColumAttr",o_name); }
	virtual int columNameToId( const string &colm )
	{ throw TError(_err,"columNameToId",o_name); }
	
	virtual string getCodePage( )
	{ throw TError(_err,"getCodePage",o_name); }
	virtual void setCodePage( const string &codepage )
	{ throw TError(_err,"setCodePage",o_name); }
	
	TBD &Owner(){ return *m_owner; }
    private:
    
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
	void list( vector<string> &list ) { m_hd_tb.obj_list( list ); }
	// Open stat
	bool openStat( const string &table )
	{ return m_hd_tb.obj_avoid(table); }
	// Open bd table. if create = true then try create if no avoid table
	void open( const string &table, bool create );
	// Save and Close table
	void close( const string &table )
	{ delete (TTable *)m_hd_tb.obj_del( table ); }
	// Delete table
	void del( const string &table ){ TableDel(table); }
	// Go Table
	AutoHD<TTable> at( const string &name )
        { AutoHD<TTable> obj( name, m_hd_tb ); return obj; }
	
    protected:
    
    private:
	virtual TTable *TableOpen( const string &table, bool create )
	{ throw TError("(%s) function TableOpen no support!",o_name); }
	virtual void TableDel( const string &table )
	{ throw TError("(%s) function TableDel no support!",o_name); }
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
	void list( vector<string> &list ) { m_hd_bd.obj_list( list ); }
	// Open stat
        bool openStat( const string &bd )
        { return m_hd_bd.obj_avoid(bd); }			
	// Open bd. if create = true then try create if no avoid bd
	void open( const string &name, bool create );
	// Save and Close bd
	void close( const string &name )
	{ delete (TBD *)m_hd_bd.obj_del( name ); }
	// Delete bd
	void del( const string &name ){ BDDel(name); }
	// Go BD
	AutoHD<TBD> at( const string &name )
        { AutoHD<TBD> obj( name, m_hd_bd ); return obj; }
		
/** Public atributes:: */
    public:
/** Public atributes:: */
    private:    
	virtual TBD *BDOpen( const string &name, bool create )
	{throw TError("(%s) Function \"BDOpen\" no support!",o_name); }
	virtual void BDDel( const string &name )
	{throw TError("(%s) Function \"BDDel\" no support!",o_name); }
/** Private atributes:: */
    private:
	THD          m_hd_bd;
	
	static const char     *o_name;
};
	
class SBDS
{
    public:
	SBDS( const string &m_tp, const string &m_bd, const string &m_tbl ) : tp(m_tp), bd(m_bd), tbl(m_tbl) { }
    	string tp;
	string bd;
	string tbl;
};

class TKernel;

class TBDS : public TGRPModule
{         
    /** Public methods: */
    public:
	TBDS( TKernel *app );
    
       	~TBDS(  );

	int gmdVer( ) { return(VER_BD); }
	
	// Open table. if create = true then try create if no avoid bd and table
	AutoHD<TTable> open( const SBDS &bd_t, bool create = false );
	// Save and Close table	
	void close( const SBDS &bd_t );

	void gmdUpdateOpt();
	void gmdCheckCommandLine( );

    public:

    /** Private methods: */
    private:
	string opt_descr(  );
	
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
    /** Private atributes: */
    private:	
	static const char *o_name;
};

#endif // TBDS_H
