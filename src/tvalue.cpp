#include "tvalue.h"


TValue::TValue( ) : mode(VAL_MA_DIRECT), elem(NULL)
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

int TValue::ResizeBuff(unsigned int len)
{
    if(mode != VAL_MA_DIRECT) return(-1);
    while(len < Size()) value.erase(value.end());
    while(len > Size())
    {
	int i_val = Size();
	value.push_back();
	while(value[i_val].size() < elem->Size())
	    value[i_val].push_back();
    }

    return(0);
}

int TValue::AddElem(int id_val)
{
    if(mode != VAL_MA_DIRECT) return(-1);    
    for(unsigned i_id = 0; i_id < Size(); i_id++)
	value[i_id].insert(value[i_id].begin()+id_val);
    return(0);
}

int TValue::DelElem(int id_val)
{
    if(mode != VAL_MA_DIRECT) return(-1);
    for(unsigned i_id = 0; i_id < Size(); i_id++)
	value[i_id].erase(value[i_id].begin()+id_val);
    return(0); 
}

int TValue::write(double val)
{
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
