#ifndef TEST_TCONTR_H
#define TEST_TCONTR_H

#include "../gener/tmodule.h"
#include "../../tvalue.h"
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

	void CheckCommandLine(  );

	int  LoadContr(unsigned id);
	int  SaveContr(unsigned id);
	int  FreeContr(unsigned id);
	int  StartContr(unsigned id);
	int  StopContr(unsigned id);
    public:

    private:
	void pr_opt_descr( FILE * stream );
	void SetCfgValue(int id);
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

	static SBlock ValAN[];
	static SBlock ValDG[];
	
	static SExpFunc ExpFuncLc[];
	TTipController *TContr;
};

#endif //TEST_TCONTR_H

