
#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
#include "tconfig.h"
#include "tconfigelem.h"

const char *TConfigElem::o_name = "TConfigElem";

TConfigElem::TConfigElem() 
{

}

TConfigElem::~TConfigElem()
{
    while(config.size()) delete config[0];
    while(elem.size()) cfe_Del(0);
}

int TConfigElem::cfe_Add(unsigned int id, SCfgFld *element)
{
    vector< _SCfgFld >::iterator iter;
    
    if(element==NULL)                     throw TError("%s: parameter error!",o_name);
    //Find dublicates
    for(iter=elem.begin(); iter != elem.end(); iter++)
	if((*iter).name == element->name) throw TError("%s: element already present!",o_name);
    //Add element
    if( id >= elem.size() ) id = elem.size();
    elem.insert(elem.begin()+id);
    iter = elem.begin()+id;
    (*iter).name     = element->name;
    (*iter).descript = element->descript;
    (*iter).type     = element->type;
    (*iter).ElDep    = element->ElDep;
    (*iter).val_dep  = element->val_dep;
    (*iter).def      = element->def;    
    (*iter).len      = element->len;
    (*iter).view     = element->view;
    //convert string to list
    int st_pos=0, cur_pos;
    do
    {
	cur_pos = element->vals.find(";",st_pos);
	if(cur_pos == st_pos) { st_pos+=1; continue; }
       	(*iter).vals.push_back(element->vals.substr(st_pos,cur_pos-st_pos));
	st_pos = cur_pos+1;
    }while(cur_pos != (int)string::npos);
    do
    {
	cur_pos = element->n_sel.find(";",st_pos);
	if(cur_pos == st_pos) { st_pos+=1; continue; }
       	(*iter).n_sel.push_back(element->n_sel.substr(st_pos,cur_pos-st_pos));
	st_pos = cur_pos+1;
    }while(cur_pos != (int)string::npos);
    //Add value and set them default
    for(unsigned cfg_i=0; cfg_i < config.size(); cfg_i++) config[cfg_i]->cf_AddElem(id);

    return(id);
}

void TConfigElem::cfe_Del(unsigned int id)
{
    if( id >= elem.size() ) throw TError("%s: id error!",o_name);
    elem.erase(elem.begin()+id);
    //Delete value
    for(unsigned cfg_i=0; cfg_i < config.size(); cfg_i++) config[cfg_i]->cf_DelElem(id);
}

void TConfigElem::cfe_Load( SCfgFld *elements, int numb )
{
    int i_start = elem.size();
    for(int i = 0; i < numb; i++) cfe_Add(i_start+i,&elements[i]);
}

unsigned int TConfigElem::cfe_NameToId(string name)
{
    for(unsigned i=0; i < elem.size(); i++)
	if(elem[i].name == name) return(i);
    throw TError("%s: no avoid config element: %s!",o_name,name.c_str());
}

void TConfigElem::cfe_UpdateBDAttr( TTable *tbl )
{
    SColmAttr attr;
    int i_row, i_elem;
    //Find and delete noused fields
    for( i_row = 0; i_row < tbl->NColums( ); i_row++ )
    {
	tbl->GetColumAttr(i_row,&attr);
    	for( i_elem=0; i_elem < (int)cfe_Size(); i_elem++)
	    if( elem[i_elem].name == attr.name ) break;
	if( i_elem == (int)cfe_Size() )
	{ 
	    tbl->DelColum(tbl->ColumNameToId(attr.name)); 
	    i_row--;
	}
    }

    //Add new columns  
    for( i_elem=0; i_elem < (int)cfe_Size(); i_elem++)
    {	
	try{ i_row = tbl->ColumNameToId(elem[i_elem].name); }
	catch(TError err)
	{
	    attr.name = elem[i_elem].name;
 	    if(elem[i_elem].type & CFG_T_STRING)
	    {
		attr.tp   = BD_ROW_STRING;
		attr.len  = atoi(elem[i_elem].len.c_str());		
	    }
	    else if(elem[i_elem].type & CFG_T_INT)
	    {
		attr.tp   = BD_ROW_INT;
		attr.len  = atoi(elem[i_elem].len.c_str());		
	    }
	    else if(elem[i_elem].type&CFG_T_REAL)
	    {
		attr.tp   = BD_ROW_REAL;
		sscanf(elem[i_elem].len.c_str(),"%d.%d",&attr.len,&attr.dec);
	    }
	    else if(elem[i_elem].type&CFG_T_BOOLEAN)
	    {
		attr.tp   = BD_ROW_BOOLEAN;
	    }else continue;
	    tbl->AddColum(&attr);
	    continue;
	}
	//Check columns  
	tbl->GetColumAttr(i_row,&attr);
	if(elem[i_elem].type & CFG_T_STRING && 
		( attr.tp != BD_ROW_STRING || attr.len != (unsigned)atoi(elem[i_elem].len.c_str()) ) )
	{
	    attr.tp   = BD_ROW_STRING;
	    attr.len  = atoi(elem[i_elem].len.c_str());		
	    tbl->SetColumAttr(i_row,&attr); 
	}
	else if( elem[i_elem].type&CFG_T_INT && attr.tp != BD_ROW_INT )		
	{
	    attr.tp   = BD_ROW_INT;
	    attr.len  = atoi(elem[i_elem].len.c_str());		
	    tbl->SetColumAttr(i_row,&attr); 
	}
	else if(elem[i_elem].type&CFG_T_REAL && attr.tp != BD_ROW_REAL )
	{
	    attr.tp   = BD_ROW_REAL;
	    sscanf(elem[i_elem].len.c_str(),"%d.%d",&attr.len,&attr.dec);
	    tbl->SetColumAttr(i_row,&attr); 
	}
	else if(elem[i_elem].type&CFG_T_BOOLEAN && attr.tp != BD_ROW_BOOLEAN )
	{
	    attr.tp   = BD_ROW_BOOLEAN;
	    tbl->SetColumAttr(i_row,&attr); 
	}
    }
} 

void TConfigElem::cfe_List( vector<string> &list )
{
    list.clear();
    for(unsigned i = 0; i < elem.size(); i++) list.push_back(elem[i].name);
}


