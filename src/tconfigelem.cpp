
#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "tconfig.h"
#include "tconfigelem.h"


TConfigElem::TConfigElem()
{

}

TConfigElem::~TConfigElem()
{
    while(config.size()) delete config[0];
    while(elem.size()) Del(0);
}

int TConfigElem::Add(unsigned int id, SElem *element)
{
    vector< SElem >::iterator iter;

    if(element==NULL) return (-1);
    //Find dublicates
    for(iter=elem.begin(); iter != elem.end(); iter++)
	if((*iter).name == element->name) return(-1);
    //Add element
    if( id >= elem.size() ) id = elem.size();
    elem.insert(elem.begin()+id);
    iter=elem.begin()+id;
    (*iter).name     = element->name;
    (*iter).descript = element->descript;
    (*iter).type     = element->type;
    (*iter).len      = element->len;
    (*iter).ElDep    = element->ElDep;
    (*iter).val_dep  = element->val_dep;
    if( ( (*iter).type == CFGTP_STRING && element->rstr  == NULL ) ||
	( (*iter).type == CFGTP_NUMBER && element->rnumb == NULL ) ||
	( (*iter).type == CFGTP_SELECT && element->rsel  == NULL ) )
    {
    	elem.erase(iter);
	return(-2);
    }
    if( (*iter).type == CFGTP_STRING )
    {
	(*iter).rstr = new SRecStr;
	(*iter).rstr->def = element->rstr->def;
    }
    if( (*iter).type == CFGTP_NUMBER )
    {
	(*iter).rnumb = new SRecNumb;
	(*iter).rnumb->min = element->rnumb->min;
	(*iter).rnumb->max = element->rnumb->max;
	(*iter).rnumb->def = element->rnumb->def;
	(*iter).rnumb->depth = element->rnumb->depth;
	(*iter).rnumb->view  = element->rnumb->view;
    }
    if( (*iter).type == CFGTP_SELECT )
    {
	(*iter).rsel = new SRecSel;
	(*iter).rsel->def = element->rsel->def;
	(*iter).rsel->name_varnt = element->rsel->name_varnt;   //!!!! Check true value
	(*iter).rsel->val_varnt  = element->rsel->val_varnt;  
    }
    //Add value and set them default
    for(int cfg_i=0; cfg_i < config.size(); cfg_i++)
	for(int val_id=0; val_id < config[cfg_i]->value.size(); val_id++)
	{
	    vector< _SVal >::iterator val_i;
	    
	    val_i=config[cfg_i]->value[val_id].begin()+id;
    	    config[cfg_i]->value[val_id].insert(val_i);
    	    val_i=config[cfg_i]->value[val_id].begin()+id;
    	    if( (*iter).type == CFGTP_STRING )
    	    {
    		(*val_i).sval    = new string;
    		*((*val_i).sval) = (*iter).rstr->def;
    	    }
	    if( (*iter).type == CFGTP_NUMBER )
	    {
		(*val_i).nval    = new double;
		*((*val_i).nval) = (*iter).rnumb->def;
	    }
	    if( (*iter).type == CFGTP_SELECT )
	    {
		(*val_i).sval    = new string;
		*((*val_i).sval) = (*iter).rsel->def;
	    }	
	}
    return(id);
}

int TConfigElem::Del(unsigned int id)
{
    if( id >= elem.size() ) return(-1);
    if( elem[id].type == CFGTP_STRING ) delete elem[id].rstr;
    if( elem[id].type == CFGTP_NUMBER ) delete elem[id].rnumb;
    if( elem[id].type == CFGTP_SELECT ) delete elem[id].rsel;
    elem.erase(elem.begin()+id);
    //Delete value
    for(int cfg_i=0; cfg_i < config.size(); cfg_i++)
	for(int val_i=0; val_i < config[cfg_i]->value.size(); val_i++)
	{
	    if( elem[id].type == CFGTP_STRING ) delete config[cfg_i]->value[val_i][id].sval;
	    if( elem[id].type == CFGTP_NUMBER ) delete config[cfg_i]->value[val_i][id].nval;
	    if( elem[id].type == CFGTP_SELECT ) delete config[cfg_i]->value[val_i][id].sval;
	    config[cfg_i]->value[val_i].erase(config[cfg_i]->value[val_i].begin()+id);
	}
    return(0);
}

int TConfigElem::Load( SElem *elements, int numb )
{
    int i_start = elem.size();
    for(int i = 0; i < numb; i++) Add(i_start+i,&elements[i]);
    return(0);
}

int TConfigElem::NameToId(string name)
{
    for(int i=0; i < elem.size(); i++)
	if(elem[i].name == name) return(i);
    return(-1);
}

int TConfigElem::UpdateBDAtr( string bd )
{
    int b_hd;

    b_hd = App->BD->OpenBD(bd);
    if(b_hd < 0) 
    {
	b_hd = App->BD->NewBD(bd);
	if(b_hd < 0) return(-1);
    }
    int kz = UpdateBDAtr( b_hd );    
    App->BD->SaveBD(b_hd);
    App->BD->CloseBD(b_hd);

    return(kz); 
}

int TConfigElem::UpdateBDAtr( int hd_bd )
{
    SRowAttr attr;
    int i_row, i_elem;
    //Find and delete noused fields
    for( i_row = 0; i_row < App->BD->NRows(hd_bd); i_row++ )
    {

	App->BD->GetRowAttr(hd_bd,i_row,&attr);
    	for( i_elem=0; i_elem < Size(); i_elem++)
	    if( elem[i_elem].name == attr.name ) break;
	if( i_elem == Size() )
	{ 
	    int kz = App->BD->DelRow(hd_bd,attr.name ); 
	    i_row--;
	}
    }

    
    for( i_elem=0; i_elem < Size(); i_elem++)
    {
	i_row = App->BD->RowNameToId(hd_bd,elem[i_elem].name);

    	//Add new rows  
	if(i_row < 0)
	{
	    attr.name = elem[i_elem].name;
 	    if(elem[i_elem].type == CFGTP_NUMBER)
	    {
		attr.type = 'N';
		attr.len  = elem[i_elem].len;
		attr.dec  = elem[i_elem].rnumb->depth;		
		App->BD->AddRow(hd_bd,&attr);
	    }
	    else
	    {
		attr.type = 'C';
		attr.len  = elem[i_elem].len;		
   		App->BD->AddRow(hd_bd,&attr); 
	    }
	}
    	//Check rows  
	else
	{
	    App->BD->GetRowAttr(hd_bd,i_row,&attr);
 	    if( elem[i_elem].type == CFGTP_NUMBER )
	    {
		if( attr.type != 'N' || attr.len != elem[i_elem].len || 
		    attr.dec != elem[i_elem].rnumb->depth )
		{
    		    attr.type = 'N';
    		    attr.len  = elem[i_elem].len;
    		    attr.dec  = elem[i_elem].rnumb->depth;		
       		    App->BD->SetRowAttr(hd_bd,i_row,&attr); 
		}
	    }
    	    else
	    {
 		if( attr.type != 'C' || attr.len != elem[i_elem].len )
		{
    		    attr.type = 'C';
    		    attr.len  = elem[i_elem].len;		
       		    App->BD->SetRowAttr(hd_bd,i_row,&attr); 
		}
	    }
	}
    }
    return(0);
} 


