#ifndef TEST_TCONTR_H
#define TEST_TCONTR_H

#include "../../tmodule.h"
#include "../../tvalue.h"
#include "../../tcontroller.h"
#include "../../ttipcontroller.h"

#include <string>
using std::string;
#include <vector>
using std::vector;


class TConfig;
class TConfigElem;

class TContrVirt: public TController
{
public:
    TContrVirt(TTipController *tcntr, string name_c, string bd_c, TConfigElem *cfgelem);
    virtual ~TContrVirt();   

    virtual int Load(  );
    virtual int Save(  );
    virtual int Free(  );
    virtual int Start(  );
    virtual int Stop(  );
//    virtual int Enable(  );
//    virtual int Disable(  );
};

class TVirtual: public TModule
{
    public:
	TVirtual(char *name);
	virtual ~TVirtual();
	
	virtual	int info( const string & name, string & info );
       	virtual int init( void *param );

	void CheckCommandLine(  );

	TController *ContrAttach(string name, string bd);
/*	
	int  LoadContr(unsigned id);
	int  SaveContr(unsigned id);
	int  FreeContr(unsigned id);
	int  StartContr(unsigned id);
	int  StopContr(unsigned id);
*/
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
	static SRecStr  RStr[];
	static SRecNumb RNumb[];
	static SRecSel  RSel[];

	static SElem elem[];
	static SElem ElemAN[];
	static SElem ElemDG[];
	static SElem ElemBL[];

	static SVAL  ValAN[];
	static SVAL  ValDG[];
	static SVAL  ValPID[];
	
	static SExpFunc ExpFuncLc[];
	TTipController *TContr;
};

#endif //TEST_TCONTR_H

