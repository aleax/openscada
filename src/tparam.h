#ifndef TPARAM_H
#define TPARAM_H

#include <string>
#include <vector>

#include "tcontrollers.h"

using std::string;
using std::vector;

class SCntrS;
class SHDCntr;

struct SParam
{
    SHDCntr c_hd;
    int     p_hd;
};

class TParamContr;

class TParam 
{
    /** Public methods: */
    public:
	TParam( SCntrS cntr, string name, TParamS *prms );

	~TParam(  );

	string &Name() { return(name); }
	// Registering controller's param
    	int Reg( SCntrS cntr, string name );
	// Unregistering controller's param
	int UnReg( SCntrS cntr, string name );

	TParamContr &at();    

    	TParamS &Owner() { return( *owner ); }
    private:

    private:
	string name;
	vector<SParam> PrmC;
	int    work;
	int    hd_res;
    
	TParamS  *owner;
	static const char *o_name;
};

#endif // TPARAM_H
