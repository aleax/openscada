/* Test Modul
** ==============================================================
*/

#ifndef MY_SQL_H
#define MY_SQL_H

#include <string>
using std::string;

#include "../../tmodule.h"

class TMY_SQL: public TModule
{
    public:
	TMY_SQL(char *name);
	~TMY_SQL();
	
	void info( const string & name, string & info );
       	void init( void *param );

	void CheckCommandLine(  );
	int OpenBD( string name );
	int CloseBD( int hd );
    public:
    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];
};

#endif // MY_SQL_H

