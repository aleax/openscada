/* Test Modul
** ==============================================================
*/

#ifndef TEST_BD1_H
#define TEST_BD1_H

#include <string>

#include "../gener/tmodule.h"

class TBDtest1: public TModule
{
    public:
	TBDtest1(char *name);
	virtual ~TBDtest1();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( );

	virtual int GetFunc( string NameFunc, void (TModule::**offptr)() );

	void CheckCommandLine(  );
	int OpenBD( string name );
	int CloseBD( int hd );
    public:
	static SExpFunc ExpFunc[];
    private:
	void pr_opt_descr( FILE * stream );
    private:
//	char *FileName;
};

#endif // TEST_BD1_H

