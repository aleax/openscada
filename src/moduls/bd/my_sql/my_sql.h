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
using std::string;

#include <tmodule.h>
#include <tbds.h>

class TBD_my_sql;

class TTable_my_sql : public TTable
{
    public:
	TTable_my_sql(TBD_my_sql *bd,string name, bool create);
	~TTable_my_sql(  );

	string getCellS( int colm, int line );
	double getCellR( int colm, int line );
	int    getCellI( int colm, int line );
	bool   getCellB( int colm, int line );

	int    nLines( );
	string getCodePage( );
    private:
	TBD_my_sql  *m_bd;
};

class TBD_my_sql : public TBD
{
    public:
    friend class TTable_my_sql;
    	TBD_my_sql( string name, string _host, string _user, string _pass, string _bd, int _port, string _u_sock, bool create );
	~TBD_my_sql(  );

	TTable *TableOpen( const string &name, bool create );
	void TableDel( const string &name );
    protected:
	string host;
	string user;
	string pass;
	string bd;
	int    port;
	string u_sock;
	
	MYSQL connect;
};

class TMY_SQL: public TTipBD
{
    public:
	TMY_SQL( string name );
	~TMY_SQL();
	
        TBD *BDOpen( const string &name, bool create );
	
	void modCheckCommandLine( );
	void modUpdateOpt();
    private:
	void pr_opt_descr( FILE * stream );
    private:
        int    def_port;
	string def_user;
};

#endif // MY_SQL_H

