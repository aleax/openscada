#include "tvalue.h"


TValue::TValue( TConfigElem *Elements ) : TConfig(Elements)
{
    value.push_back();
}

TValue::~TValue()
{

}

int TValue::AddVal(int id_val, SBlock *block)
{
    if(AddRecord(id_val) < 0) return(-1);
    SetVal(id_val, "NAME", block->name);
    SetVal(id_val,"LNAME", block->lname);
    SetVal(id_val,"DESCR", block->descr);
    SetVal(id_val, "TYPE", (double)block->type);
    SetVal(id_val, "STAT", (double)block->stat);
    SetVal(id_val, "MODE", (double)block->mode);
    SetVal(id_val, "DATA", (double)block->data);
    SetVal(id_val,"ACCESS",(double)block->access);
    SetVal(id_val,  "MIN", (double)block->min);
    SetVal(id_val,  "MAX", (double)block->max);

    for(int i_val=0;i_val < value.size();i_val++)
	value[i_val].insert(value[i_val].begin()+id_val);
    
    return(0);
}

int TValue::DelVal(int id_val)
{
    if(FreeRecord(id_val) < 0) return(-1);

    for(int i_val=0;i_val < value.size();i_val++)
	value[i_val].erase(value[i_val].begin()+id_val);

    return(0);  
}

int TValue::ResizeBuff(unsigned int len)
{
    while(len < SizeBuff()) value.erase(value.end());
    while(len > SizeBuff())
    {
	int i_val = SizeBuff();
	value.push_back();
	while(value[i_val].size() < TConfig::Size())
	    value[i_val].push_back();
    }

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
