/* Test Modul
** ==============================================================
*/

#ifndef TEST_BD_H
#define TEST_BD_H

#include <string>

#include "../gener/tmodule.h"

class TBDtest2: public TModule
{
    public:
	TBDtest2(char *name);
	virtual ~TBDtest2();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( );

	void CheckCommandLine(  );
	int OpenBD( string name );
	int CloseBD( int hd );
    public:
    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SExpFunc ExpFuncLc[];
};

#endif // TEST_BD_H

