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

class TTable
{
    public:
	TTable( const string &name );
	virtual ~TTable();

	string &Name(){ return(m_name); }

	virtual void Save()
	{ throw TError(_err,"Save",o_name); }

	void Clean() { while(NLines()) DelLine(0); }
	
	virtual string GetCellS( int colm, int line)
	{ throw TError(_err,"GetCellS",o_name); }
	virtual double GetCellR( int colm, int line)
	{ throw TError(_err,"GetCellR",o_name); }
	virtual int GetCellI( int colm, int line)
	{ throw TError(_err,"GetCellI",o_name); }
	virtual bool GetCellB( int colm, int line)
	{ throw TError(_err,"GetCellB",o_name); }
	
	virtual void SetCellS( int colm, int line, const string &cell)
	{ throw TError(_err,"SetCellS",o_name); }
	virtual void SetCellR( int colm, int line, double val)
	{ throw TError(_err,"SetCellR",o_name); }
	virtual void SetCellI( int colm, int line, int val)
	{ throw TError(_err,"SetCellI",o_name); }
	virtual void SetCellB( int colm, int line, bool val)
	{ throw TError(_err,"SetCellB",o_name); }
	
	virtual int NLines( )
	{ throw TError(_err,"NLines",o_name); }
	virtual int AddLine( unsigned int line)
	{ throw TError(_err,"AddLines",o_name); }
	virtual void DelLine( unsigned int line)
	{ throw TError(_err,"DelLines",o_name); }
	
	virtual int NColums( )
	{ throw TError(_err,"NColums",o_name); }
	virtual int AddColum( SColmAttr *colm )
	{ throw TError(_err,"AddColum",o_name); }
	virtual void DelColum( int colm)
	{ throw TError(_err,"DelColum",o_name); }
	
	virtual void GetColumAttr( int colm, SColmAttr *attr )
	{ throw TError(_err,"GetColumAttr",o_name); }
	virtual void SetColumAttr( int colm, SColmAttr *attr )
	{ throw TError(_err,"SetColumAttr",o_name); }
	virtual int ColumNameToId( const string &colm )
	{ throw TError(_err,"ColumNameToId",o_name); }
	
	virtual string GetCodePage( )
	{ throw TError(_err,"GetCodePage",o_name); }
	virtual void SetCodePage( const string &codepage )
	{ throw TError(_err,"SetCodePage",o_name); }
    private:
    
    private:
	string m_name;
	
	static const char *o_name;
	static char *_err;
};    

class TBD
{
    public:
	TBD( const string &name );
	virtual ~TBD();

	string &Name() { return( m_name ); }
	// List opened bd tables
	void list( vector<string> &list ) { m_hd_tb.obj_list( list ); }
	// Open bd table. if create = true then try create if no avoid table
	unsigned open( const string &table, bool create );
	// Save and Close table
	void close( unsigned hd );	
	// Delete table
	void del( const string &table ){ TableDel(table); }
	TTable &at(unsigned hd) { return( *(TTable *)m_hd_tb.hd_at( hd ) ); }
	TTable &operator[](unsigned hd ) { return(at(hd)); }	
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
	// Open bd. if create = true then try create if no avoid bd
	unsigned open( const string &name, bool create );
	// Save and Close bd
	void close( unsigned hd );	
	// Delete bd
	void del( const string &name ){ BDDel(name); }
	TBD &at(unsigned hd) { return( *(TBD *)m_hd_bd.hd_at( hd ) ); }
	TBD &operator[](unsigned hd ) { return(at(hd)); }	
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

struct SHDBD
{
    unsigned h_tp;
    unsigned h_bd;
    unsigned h_tbl;
};

class TKernel;

class TBDS : public TGRPModule
{         
    /** Public methods: */
    public:
	TBDS( TKernel *app );
    
       	~TBDS(  );

	int gmd_Ver( ) { return(VER_BD); }

	// List opened tables
	void list( vector<SBDS> &list );
	// Open table. if create = true then try create if no avoid bd and table
	SHDBD open( const SBDS &bd_t, bool create = false );
	// Save and Close table
	void close( const SHDBD &hd );
	// Delete table
	void del( const SBDS &bd_t );
	TTable &at( SHDBD &hd )	{ return ((TTipBD &)gmd_at(hd.h_tp)).at(hd.h_bd).at(hd.h_tbl); }

	void gmd_UpdateOpt();
	void gmd_CheckCommandLine( );

    public:

    /** Private methods: */
    private:
	string opt_descr(  );
	
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
    /** Private atributes: */
    private:
	
	static const char *i_cntr;
	static const char *o_name;
};

#endif // TBDS_H
