
#include "tapplication.h"
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
    while(elem.size()) Del(0);
}

int TConfigElem::Add(unsigned int id, SCfgFld *element)
{
    vector< _SCfgFld >::iterator iter;

//    App->Mess->put(1,"TEST!");
    
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
    }while(cur_pos != string::npos);
    do
    {
	cur_pos = element->n_sel.find(";",st_pos);
	if(cur_pos == st_pos) { st_pos+=1; continue; }
       	(*iter).n_sel.push_back(element->n_sel.substr(st_pos,cur_pos-st_pos));
	st_pos = cur_pos+1;
    }while(cur_pos != string::npos);
    //Add value and set them default
    for(int cfg_i=0; cfg_i < config.size(); cfg_i++) config[cfg_i]->AddElem(id);

    return(id);
}

void TConfigElem::Del(unsigned int id)
{
    if( id >= elem.size() ) throw TError("%s: id error!",o_name);
    elem.erase(elem.begin()+id);
    //Delete value
    for(int cfg_i=0; cfg_i < config.size(); cfg_i++) config[cfg_i]->DelElem(id);
}

void TConfigElem::Load( SCfgFld *elements, int numb )
{
    int i_start = elem.size();
    for(int i = 0; i < numb; i++) Add(i_start+i,&elements[i]);
}

unsigned int TConfigElem::NameToId(string name)
{
    for(int i=0; i < elem.size(); i++)
	if(elem[i].name == name) return(i);
    throw TError("%s: no avoid config element: %s!",o_name,name.c_str());
}

void TConfigElem::UpdateBDAttr( string t_bd, string n_bd, string n_tb )
{
    int t_hd;

    try { t_hd = App->BD->OpenTable(t_bd,n_bd,n_tb); }
    catch(...){ t_hd = App->BD->OpenTable(t_bd,n_bd,n_tb,true); }
    
    try{ UpdateBDAttr( t_hd ); }
    catch(...)
    {
    	App->BD->CloseTable(t_hd);
	throw;
    }
    App->BD->at_tbl(t_hd)->Save( );
    App->BD->CloseTable(t_hd);
}

void TConfigElem::UpdateBDAttr( int hd_bd )
{
    SColmAttr attr;
    int i_row, i_elem;
    //Find and delete noused fields
    for( i_row = 0; i_row < App->BD->at_tbl(hd_bd)->NColums( ); i_row++ )
    {
	App->BD->at_tbl(hd_bd)->GetColumAttr(i_row,&attr);
    	for( i_elem=0; i_elem < Size(); i_elem++)
	    if( elem[i_elem].name == attr.name ) break;
	if( i_elem == Size() )
	{ 
	    App->BD->at_tbl(hd_bd)->DelColum(App->BD->at_tbl(hd_bd)->ColumNameToId(attr.name)); 
	    i_row--;
	}
    }
    
    for( i_elem=0; i_elem < Size(); i_elem++)
    {
	try{ i_row = App->BD->at_tbl(hd_bd)->ColumNameToId(elem[i_elem].name); }
	catch(...)
	{
    	    //Add new columns  
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
		scanf("%d.%d",attr.len,attr.dec);
	    }
	    else if(elem[i_elem].type&CFG_T_BOOLEAN)
	    {
		attr.tp   = BD_ROW_BOOLEAN;
	    }else continue;
	    App->BD->at_tbl(hd_bd)->AddColum(&attr);
	}
    	//Check columns  
	App->BD->at_tbl(hd_bd)->GetColumAttr(i_row,&attr);
	if(elem[i_elem].type & CFG_T_STRING && 
		( attr.tp != BD_ROW_STRING || attr.len != atoi(elem[i_elem].len.c_str()) ) )
	{
	    attr.tp   = BD_ROW_STRING;
	    attr.len  = atoi(elem[i_elem].len.c_str());		
	    App->BD->at_tbl(hd_bd)->SetColumAttr(i_row,&attr); 
	}
	else if( elem[i_elem].type&CFG_T_INT && attr.tp != BD_ROW_INT )		
	{
	    attr.tp   = BD_ROW_INT;
	    attr.len  = atoi(elem[i_elem].len.c_str());		
	    App->BD->at_tbl(hd_bd)->SetColumAttr(i_row,&attr); 
	}
	else if(elem[i_elem].type&CFG_T_REAL && attr.tp != BD_ROW_REAL )
	{
	    attr.tp   = BD_ROW_REAL;
	    scanf("%d.%d",attr.len,attr.dec);
	    App->BD->at_tbl(hd_bd)->SetColumAttr(i_row,&attr); 
	}
	else if(elem[i_elem].type&CFG_T_BOOLEAN && attr.tp != BD_ROW_BOOLEAN )
	{
	    attr.tp   = BD_ROW_BOOLEAN;
	    App->BD->at_tbl(hd_bd)->SetColumAttr(i_row,&attr); 
	}
    }
} 

void TConfigElem::List( vector<string> &list )
{
    list.clear();
    for(unsigned i = 0; i < elem.size(); i++) list.push_back(elem[i].name);
}


