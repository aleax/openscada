#ifndef SYS_H
#define SYS_H

#include "../../tmodule.h"
#include "../../tcontroller.h"
#include "../../ttipcontroller.h"
#include "../../tparamcontr.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace SystemCntr
{

class TConfig;
class TConfigElem;

class TMdContr: public TController
{
    public:
    	TMdContr( string name_c, SBDS bd, ::TTipController *tcntr, ::TConfigElem *cfgelem);
	~TMdContr();   

	void Load(  );
	void Save(  );
	void Free(  );
	void Start(  );
	void Stop(  );    
	//void Enable(  );
	//void Disable(  );
    public:
	
    private:
};

class TTpContr: public TTipController
{
public:
    TTpContr(char *name);
    ~TTpContr();
	
    void mod_connect( );
    
    void mod_CheckCommandLine( );
    void mod_UpdateOpt(  );

    TController *ContrAttach(string name, SBDS bd);
public:

private:
    void pr_opt_descr( FILE * stream );
private:
    //Name of config file for virtual controllers
    string       NameCfgF;
};


} //End namespace 

#endif //SYS_H

