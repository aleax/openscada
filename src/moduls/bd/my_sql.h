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
	TTable_my_sql(TBD_my_sql *_bd,string _name, bool create);
	~TTable_my_sql(  );

	string _GetCellS( int colm, int line );
	double _GetCellR( int colm, int line );
	int    _GetCellI( int colm, int line );
	bool   _GetCellB( int colm, int line );

	int    _NLines( );
	string _GetCodePage( );
    private:
	TBD_my_sql  *bd;
	string name;
};

class TBD_my_sql : public TBD
{
    public:
    friend class TTable_my_sql;
    	TBD_my_sql( string _host, string _user, string _pass, string _bd, int _port, string _u_sock, bool create );
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
	
	void info( const string & name, string & info );
	//void connect( void *obj );

        TBD *BDOpen( string name, bool create );
	
	void CheckCommandLine( char **argv, int argc );
	void UpdateOpt();
    private:
	void pr_opt_descr( FILE * stream );
    private:
        int    def_port;
	string def_user;
	
	static SExpFunc ExpFuncLc[];
	//TTipBD *Tbd;
};

#endif // MY_SQL_H

