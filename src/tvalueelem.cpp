#include "tkernel.h"
#include "tmessage.h"
#include "tvalue.h"
#include "tvalueelem.h"


const char *TValueElem::o_name = "TValueElem";

TValueElem::TValueElem( string name_v ) : name(name_v)  
{

}

TValueElem::~TValueElem()
{
    while(elem.size())  Del(0);
    while(value.size()) delete value[0];
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
    }while(cur_pos != (int)string::npos);
    do
    {
	cur_pos = element->n_sel.find(";",st_pos);
	if(cur_pos == st_pos) { st_pos+=1; continue; }
	elem[id_val].n_sel.push_back(element->n_sel.substr(st_pos,cur_pos-st_pos));
	st_pos = cur_pos+1;
    }while(cur_pos != (int)string::npos);
    elem[id_val].l_buf = element->l_buf;

    for(unsigned i_val=0;i_val < value.size();i_val++) 
	value[i_val]->AddElem(id_val);
}

void TValueElem::Del(unsigned int id_val)
{
    if(id_val >= Size()) throw TError("%s: id error!",o_name);
    
    for(unsigned i_val=0;i_val < value.size();i_val++)
	value[i_val]->DelElem(id_val);    
    elem.erase(elem.begin()+id_val);
}

void TValueElem::List(vector<string> &List) const
{
    for(unsigned i_elem = 0; i_elem < Size(); i_elem++)
	List.push_back(elem[i_elem].name);    
}

int TValueElem::NameToId( string name ) const
{
    for(unsigned i_elem = 0; i_elem < Size(); i_elem++)
	if( elem[i_elem].name == name ) return(i_elem);    
    throw TError("%s: Name element %s error!",o_name,name.c_str());
}

void TValueElem::Get( unsigned int id, SVAL &element ) const
{
    if( id >= Size() ) throw TError("%s: id element error!",o_name);
    
    element.name   = elem[id].name;
    element.lname  = elem[id].lname;
    element.descr  = elem[id].descr;
    element.type   = elem[id].type;
    element.source = elem[id].source;
    element.io     = elem[id].io;
    element.view   = elem[id].view;
    //convert string to list
    element.vals.clear();
    for(unsigned int i_vl = 0; i_vl < elem[id].vals.size(); i_vl++)
    {
	if(i_vl != 0) element.vals+=";";
       	element.vals+=elem[id].vals[i_vl];
    }
    element.n_sel.clear();
    for(unsigned int i_sl = 0; i_sl < elem[id].n_sel.size(); i_sl++)
    {
	if(i_sl != 0) element.n_sel+=";";
       	element.n_sel+=elem[id].n_sel[i_sl];
    }
    element.l_buf = elem[id].l_buf;
}

int TValueElem::Type( unsigned int id ) const
{
    if( id >= Size() ) throw TError("%s: id element error!",o_name);
    return(elem[id].type);
}

int TValueElem::TypeIO( unsigned int id ) const
{
    if( id >= Size() ) throw TError("%s: id element error!",o_name);
    return(elem[id].io);
}

