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

#ifndef TEST_BD_H
#define TEST_BD_H

#include <string>
#include <vector>

#include <tmodule.h>
#include <tbds.h>

using std::string;
using std::vector;

class TBasaDBF;

struct Shd
{
    int      use;
    string   name_bd;
    TBasaDBF *basa;
    string   codepage;
};

class TTableDir : public TTable
{
    public:
    	TTableDir(string name, bool create, TBD *owner );
    	~TTableDir(  );

        string Name(){ return(n_table); }
	
    	void   save( );
	
	string getCellS( int colm, int line );
	double getCellR( int colm, int line );
	int    getCellI( int colm, int line );
	bool   getCellB( int colm, int line );    
	void   setCellS( int colm, int line, const string &cell );
	void   setCellR( int colm, int line, double val );
	void   setCellI( int colm, int line, int val );
	void   setCellB( int colm, int line, bool val );	
    	int    nLines( );
    	int    addLine( unsigned int line );
    	void   delLine( unsigned int line );
    	int    nColums(  );
    	int    addColum( SColmAttr *colm );
    	void   delColum( int colm );
    	void   getColumAttr( int colm, SColmAttr *attr );
	void   setColumAttr( int colm, SColmAttr *attr );
    	int    columNameToId( const string &colm );
    	string getCodePage( );
    	void   setCodePage( const string &codepage );
    private:
	string n_table;
	string codepage;
	TBasaDBF *basa;
	int    m_res;
};

class TBDdir : public TBD
{
    public:
	TBDdir( string name );
	~TBDdir(  );

	TTable *TableOpen( const string &table, bool create );
	void TableDel( const string &table );
    private:    
};


class TDirectDB : public TTipBD
{
  public:
    TDirectDB( string name );
    ~TDirectDB(  );

    TBD *BDOpen( const string &name, bool create );
    
    void modCheckCommandLine( );
    void modUpdateOpt();
  private:
    void pr_opt_descr( FILE * stream );
  private:
};

#endif // TEST_BD_H
