#include "tapplication.h"
#include "tmessage.h"
#include "tvalue.h"
#include "tvalueelem.h"


TValueElem::TValueElem( string name_v ) : name(name_v)  
{

}

TValueElem::~TValueElem()
{
    while(value.size()) delete value[0];
    while(elem.size()) Del(0);
}

int TValueElem::Add(int id_val, SVAL *block )
{
    if(id_val < 0 || id_val > Size()) id_val = Size();
    elem.insert(elem.begin()+id_val);
    elem[id_val] = *block;

    for(int i_val=0;i_val < value.size();i_val++)
	value[i_val]->AddElem(id_val);
    
    return(0);
}

int TValueElem::Del(int id_val)
{
    if(id_val < 0 || id_val >= Size()) return(-1);
    elem.erase(elem.begin()+id_val);

    for(int i_val=0;i_val < value.size();i_val++)
	value[i_val]->DelElem(id_val); 

    return(0);  
}

void TValueElem::List(vector<string> &List)
{
    for(unsigned i_elem = 0; i_elem < Size(); i_elem++)
	List.push_back(elem[i_elem].name);    
}

