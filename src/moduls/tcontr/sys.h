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
    	TMdContr( ::TTipController *tcntr, string name_c,string _t_bd, string _n_bd, string _n_tb, ::TConfigElem *cfgelem);
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

    TController *ContrAttach(string name, string t_bd, string n_bd, string n_tb);
public:

private:
    void pr_opt_descr( FILE * stream );
private:
    //Name of config file for virtual controllers
    string       NameCfgF;
};


} //End namespace 

#endif //SYS_H

