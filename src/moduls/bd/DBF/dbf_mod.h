
//OpenSCADA system module BD.DBF file: dbf_mod.h
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

#ifndef TEST_BD_H
#define TEST_BD_H

#include <string>
#include <vector>

#include <tmodule.h>
#include <tbds.h>

using std::string;
using std::vector;

class TBasaDBF;

namespace BDDBF
{
    struct Shd
    {
	int      use;
	string   name_bd;
	TBasaDBF *basa;
	string   codepage;
    };

    class MBD;    
    class MTable : public TTable
    {
	public:
	    MTable(const string &name, MBD *iown, bool create );
	    ~MTable(  );
	    
	    bool fieldSeek( int row, TConfig &cfg );
	    void fieldGet( TConfig &cfg );
	    void fieldSet( TConfig &cfg );
	    void fieldDel( TConfig &cfg );
	    
	    MBD &owner() { return (MBD&)TTable::owner(); }
	    
	private:
	    void postDisable(int flag);
	    void   save( );	    
	    int    findKeyLine( TConfig &cfg, int cnt = 0 );
	    void   fieldPrmSet( TCfg &e_cfg, db_str_rec &n_rec );
	    
	private:
	    string n_table;
	    string codepage;
	    TBasaDBF *basa;
	    
	    int    m_res;
	    bool   m_modify;
    };

    class BDMod;    
    class MBD : public TBD
    {
	public:
	    MBD( string name, TElem *cf_el );
	    ~MBD(  );
	    
	    void enable( );
	    
	    string dbDir();
            string codepage();
			
	private:
	    void postDisable(int flag);
	    TTable *openTable( const string &table, bool create );
	    //void delTable( const string &table );
    };

    class BDMod : public TTipBD
    {
    	public:
    	    BDMod( string name );
    	    ~BDMod(  );
    
	    void modLoad( );
	    
	private:
	    TBD *openBD( const string &iid );
	    string optDescr( );
    };
    extern BDMod *mod;
}

#endif // TEST_BD_H
