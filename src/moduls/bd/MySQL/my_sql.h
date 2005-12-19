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

#ifndef MY_SQL_H
#define MY_SQL_H

#include <string>

#include <tmodule.h>
#include <tbds.h>

using std::string;

namespace BDMySQL
{
    class MBD;
    class MTable : public TTable
    {
	public:
	    MTable(string name, MBD *iown, bool create);
	    ~MTable(  );
	    
	    //Fields
	    bool fieldSeek( int row, TConfig &cfg );
            void fieldGet( TConfig &cfg );
            void fieldSet( TConfig &cfg );
            void fieldDel( TConfig &cfg );

	    MBD &owner()	{ return (MBD&)TTable::owner(); }
	    
	private:
	    void fieldFix( TConfig &cfg );
	    void fieldPrmSet( TCfg &cfg, const string &last, string &req );
    };

    class BDMod;
    class MBD : public TBD
    {
	public:
	friend class MTable;
	    MBD( const string &name, BDMod *iown, const string &_host, const string &_user, 
		const string &_pass, const string &_bd, int _port, const string &_u_sock, const string &_cd_pg, bool create );
	    ~MBD(  );	    

	    TTable *openTable( const string &name, bool create );
	    void delTable( const string &name );
	    
	    void sqlReq( const string &req, vector< vector<string> > *tbl = NULL );	    
	    
	protected:
	    string host, user, pass, bd, u_sock, cd_pg;
	    int    port;
	
	    MYSQL connect;
    };

    class BDMod: public TTipBD
    {
	public:
	    BDMod( string name );
	    ~BDMod();
	    
	    TBD *openBD( const string &name, bool create );
	    void delBD( const string &name );
	    
	    void modLoad( );
	    
	private:
	    string optDescr( );
    };
}

#endif // MY_SQL_H

