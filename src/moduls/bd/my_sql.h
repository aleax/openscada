/* Test Modul
** ==============================================================
*/

#ifndef MY_SQL_H
#define MY_SQL_H

#include <string>
using std::string;

#include "../../tmodule.h"
#include "../../tbds.h"

class TBD_my_sql;

class TTable_my_sql : public TTable
{
    public:
	TTable_my_sql(TBD_my_sql *bd,string name, bool create);
	~TTable_my_sql(  );

	string GetCellS( int colm, int line );
	double GetCellR( int colm, int line );
	int    GetCellI( int colm, int line );
	bool   GetCellB( int colm, int line );

	int    NLines( );
	string GetCodePage( );
    private:
	TBD_my_sql  *m_bd;
};

class TBD_my_sql : public TBD
{
    public:
    friend class TTable_my_sql;
    	TBD_my_sql( string name, string _host, string _user, string _pass, string _bd, int _port, string _u_sock, bool create );
	~TBD_my_sql(  );

	TTable *TableOpen( string name, bool create );
	void TableDel( string name );
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
	TMY_SQL(char *name);
	~TMY_SQL();
	
        TBD *BDOpen( string name, bool create );
	
	void mod_CheckCommandLine( );
	void mod_UpdateOpt();
    private:
	void pr_opt_descr( FILE * stream );
    private:
        int    def_port;
	string def_user;
};

#endif // MY_SQL_H

