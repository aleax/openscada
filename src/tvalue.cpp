#include "tapplication.h"
#include "tmessage.h"
#include "tconfig.h"
#include "tparamcontr.h"
#include "tcontroller.h"
#include "tvalue.h"

const char *TValue::o_name = "TValue";

TValue::TValue( ) : elem(NULL)
{

}

TValue::~TValue()
{
    if(elem)
    	for(unsigned i_val = 0; i_val < elem->value.size() ;i_val++)
	    if(elem->value[i_val] == this)
	    {
		elem->value.erase(elem->value.begin()+i_val);
		break;
	    }
}

void TValue::ResizeBuff(unsigned int len)
{
/*
    while(len < Size()) buffer.erase(value.end());
    while(len > Size())
    {
	int i_val = Size();
	value.push_back();
	while(value[i_val].size() < elem->Size())
	    value[i_val].push_back();
    }

    vector<string> parm;
    ((TParamContr *)this)->Controller()->List("ANALOG",parm);
*/    
}

void TValue::AddElem(int id_val)
{
    value.insert(value.begin()+id_val);	
    for(unsigned i_id = 0; i_id < Size(); i_id++)
	buffer[i_id].insert(buffer[i_id].begin()+id_val);	
}

void TValue::DelElem(int id_val)
{
    value.erase(value.begin()+id_val);
    for(unsigned i_id = 0; i_id < Size(); i_id++)
	buffer[i_id].erase(buffer[i_id].begin()+id_val);
}

void TValue::SetValType( TValueElem *ValEl )
{
    if(elem == ValEl) return;
    if(elem != NULL)
    {
    	for(unsigned i_val = 0; i_val < elem->value.size() ;i_val++)
	    if(elem->value[i_val] == this)
	    {
		elem->value.erase(elem->value.begin()+i_val);
		break;
	    }
	elem=NULL;
    }
    elem = ValEl;
    elem->value.push_back(this);
}

/*
int TValue::write(double val)
{
//    int (TModule::*Contr)( unsigned id );
//    char *n_f = "LoadContr";

    //in progect!!
    return(0);
}

int TValue::write(int val)
{
//in progect!!
    return(0);
}

int TValue::write(bool val)
{
//in progect!!
    return(0);
}
*/
