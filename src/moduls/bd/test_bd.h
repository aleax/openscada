/* Test Modul
** ==============================================================
*/

#ifndef TEST_BD_H
#define TEST_BD_H

#include <string>

#include "../gener/tmodule.h"

class TBDtest: public TModule
{
    public:
	TBDtest(char *name);
	virtual ~TBDtest();
	
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

