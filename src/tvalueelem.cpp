#include "tapplication.h"
#include "tmessage.h"
#include "tvalue.h"
#include "tvalueelem.h"


const char *TValueElem::o_name = "TValueElem";

TValueElem::TValueElem( string name_v ) : name(name_v)  
{

}

TValueElem::~TValueElem()
{
    while(value.size()) delete value[0];
    while(elem.size()) Del(0);
}

void TValueElem::Add(unsigned int id_val, SVAL *element )
{
    if(id_val < 0 || id_val > Size()) id_val = Size();
    elem.insert(elem.begin()+id_val);
    
    elem[id_val].name  = element->name;
    elem[id_val].lname = element->lname;
    elem[id_val].descr = element->descr;
    elem[id_val].type  = element->type;
    elem[id_val].source = element->source;
    elem[id_val].io    = element->io;
    elem[id_val].view  = element->view;
    //convert string to list
    int st_pos=0, cur_pos;
    do
    {
	cur_pos = element->vals.find(";",st_pos);
	if(cur_pos == st_pos) { st_pos+=1; continue; }
	elem[id_val].vals.push_back(element->vals.substr(st_pos,cur_pos-st_pos));
	st_pos = cur_pos+1;
    }while(cur_pos != string::npos);
    do
    {
	cur_pos = element->n_sel.find(";",st_pos);
	if(cur_pos == st_pos) { st_pos+=1; continue; }
	elem[id_val].n_sel.push_back(element->n_sel.substr(st_pos,cur_pos-st_pos));
	st_pos = cur_pos+1;
    }while(cur_pos != string::npos);

    for(int i_val=0;i_val < value.size();i_val++) 
	value[i_val]->AddElem(id_val);
}

void TValueElem::Del(unsigned int id_val)
{
    if(id_val >= Size()) throw TError("%s: id error!",o_name);
    elem.erase(elem.begin()+id_val);

    for(int i_val=0;i_val < value.size();i_val++)
	value[i_val]->DelElem(id_val); 
}

void TValueElem::List(vector<string> &List)
{
    for(unsigned i_elem = 0; i_elem < Size(); i_elem++)
	List.push_back(elem[i_elem].name);    
}

