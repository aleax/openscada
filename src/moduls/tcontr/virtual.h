#ifndef TEST_TCONTR_H
#define TEST_TCONTR_H

#include "../gener/tmodule.h"
#include "../../ttipcontroller.h"

#include <string>
using std::string;
#include <vector>
using std::vector;

class TConfig;

class TVirtual: public TModule
{
    public:
	TVirtual(char *name);
	virtual ~TVirtual();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( void *param );
	/**
   	 * Put command to external modul (plugin)
      	 *  (Start, Stop ....))
	*/
	virtual int PutCommand( string command, int id_cntr );
	int  InitContr(int id);
	void CheckCommandLine(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SElem elem[];
	static SRecStr RStr[];
	static SRecNumb RNumb[];
	
	static SExpFunc ExpFuncLc[];
	STContr *data;
};

#endif //TEST_TCONTR_H

