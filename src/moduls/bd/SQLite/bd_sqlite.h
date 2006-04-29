
//OpenSCADA system module BD.SQLite file: bd_sqlite.h
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

#ifndef BD_SQLITE
#define BD_SQLITE

#include <string>
#include <vector>
#include <tmodule.h>
#include <tbds.h>

#define COM_MAX_CNT 100

using std::string;
using std::vector;

namespace BDSQLite
{
    class MBD;
    class MTable : public TTable
    {
	public:
	    MTable(string name, MBD *bd, bool create);
	    ~MTable(  );

	    //Fields
	    bool fieldSeek( int row, TConfig &cfg );
	    void fieldGet( TConfig &cfg );
	    void fieldSet( TConfig &cfg );
	    void fieldDel( TConfig &cfg );
	    
	    MBD &owner()	{ return (MBD&)TTable::owner(); }
	    
	private:
	    void postDisable(int flag);
	    void fieldFix( TConfig &cfg );
    
	private:
	    bool my_trans;
    };

    class MBD : public ::TBD
    {
	friend class MTable;
	public:
	    MBD( const string &iid, TElem *cf_el );
	    ~MBD(  );
	    
	    void enable( );
            void disable( );			

	    string dbFile();
            string codepage();

	    void sqlReq( const string &req, vector< vector<string> > *tbl = NULL );
	
	protected:
    	    void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
			
	private:
	    //Methods
            void postDisable(int flag);    
	    TTable *openTable( const string &name, bool create );
	    //Attributes
	    string      cd_pg;
            sqlite3     *m_db;
            int        	commCnt;
    };

    class BDMod: public TTipBD
    {
	public:
	    BDMod( string name );
	    ~BDMod();
	
	    void modLoad( );
	    
	private:
	    TBD *openBD( const string &iid );
	    string optDescr( );
    };
    extern BDMod *mod;
}

#endif // BD_SQLITE

