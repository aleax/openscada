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
	void test(int id);
	void test1(int id);
    private:
	static SRecStr  RStr[];
	static SRecNumb RNumb[];
	static SRecSel  RSel[];

	static SElem elem[];
	static SElem ElemAN[];
	static SElem ElemDG[];
	static SElem ElemBL[];
	
	static SExpFunc ExpFuncLc[];
	TTipController *TContr;
};

#endif //TEST_TCONTR_H

