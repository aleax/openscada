
#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "tconfig.h"


// *****************
// TConfigElem
// *****************

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
	(*iter).rsel->name_varnt = element->rsel->name_varnt;
	(*iter).rsel->val_varnt = element->rsel->val_varnt;
    }
    //Add value and set them default
    for(int cfg_i=0; cfg_i < config.size(); cfg_i++)
	for(int val_id=0; val_id < config[cfg_i]->value.size(); val_id++)
	{
	    vector< SVal >::iterator val_i;
	    
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

int TConfigElem::Size()
{
    return(elem.size());
}

int TConfigElem::NameToId(string name)
{
    for(int i=0; i < elem.size(); i++)
	if(elem[i].name == name) return(i);
    return(-1);
}

// *****************
// TConfig
// *****************

TConfig::TConfig( TConfigElem *Elements ) : elem(Elements)  
{
    elem->config.push_back(this);
}

TConfig::~TConfig()
{
    while(value.size())	FreeRecord(0);	

    for(int i=0; i<elem -> config.size() ;i++)
	if(elem->config[i] == this) 
	{
	    elem->config.erase(elem->config.begin()+i);
	    break;
	}
}

int TConfig::GetVal( unsigned int id_ctr, string n_val, string & val)
{
    int id_elem;
    
    if( id_ctr >= value.size() )              return(-1);
    if( (id_elem = elem->NameToId(n_val)) < 0  )    return(-2);
    if( elem->elem[id_elem].type == CFGTP_NUMBER )  return(-3);
    val = *(value[id_ctr][id_elem].sval);

    return(0);
}

int TConfig::GetVal( unsigned int id_ctr, string n_val, double & val)
{
    int id_elem;
    
    if( id_ctr >= value.size() )              return(-1);
    if( (id_elem = elem->NameToId(n_val)) < 0  )    return(-2);
    if( elem->elem[id_elem].type != CFGTP_NUMBER )  return(-3);
    val = *(value[id_ctr][id_elem].nval);

    return(0);
}

int TConfig::SetVal( unsigned int id_ctr, string n_val, string val)
{
    int id_elem;
    
    if( id_ctr >= value.size() )              return(-1);
    if( (id_elem = elem->NameToId(n_val)) < 0  )    return(-2);
    if( elem->elem[id_elem].type == CFGTP_NUMBER )  return(-3);
    if( elem->elem[id_elem].type == CFGTP_STRING )
    	*(value[id_ctr][id_elem].sval) = val;
    if( elem->elem[id_elem].type == CFGTP_SELECT )
    {
	int i_beg=0, i_end=0;
	do
	{
	   i_end = elem->elem[id_elem].rsel->val_varnt.find(";",i_beg);
	   if( elem->elem[id_elem].rsel->val_varnt.substr(i_beg,i_end) == val) 
		*(value[id_ctr][id_elem].sval) = val;
	   i_beg=i_end+1;	   
	}while(i_end != string::npos);
    } 

    return(0);    
}

int TConfig::SetVal( unsigned int id_ctr, string n_val, double val)
{
    int id_elem;
    
    if( id_ctr >= value.size() )              return(-1);
    if( (id_elem = elem->NameToId(n_val)) < 0  )    return(-2);
    if( elem->elem[id_elem].type != CFGTP_NUMBER )  return(-3);
    if( val < elem->elem[id_elem].rnumb->min) val = elem->elem[id_elem].rnumb->min;
    if( val > elem->elem[id_elem].rnumb->max) val = elem->elem[id_elem].rnumb->max;
    *(value[id_ctr][id_elem].nval) = val;

    return(0);
}

int TConfig::InitRecord( unsigned int id_rec)
{
    if(id_rec < value.size())
	for(int i=0; i < elem->elem.size(); i++)
	{
    	    if( elem->elem[i].type == CFGTP_STRING ) *(value[id_rec][i].sval) = elem->elem[i].rstr->def;
    	    if( elem->elem[i].type == CFGTP_NUMBER ) *(value[id_rec][i].nval) = elem->elem[i].rnumb->def;
	    if( elem->elem[i].type == CFGTP_SELECT ) *(value[id_rec][i].sval) = elem->elem[i].rsel->def;
	}
    else
	while(id_rec >= value.size())
	{
	    int id_create = value.size();
	    value.push_back(  );
	    for(int i=0; i < elem->elem.size(); i++)
    	    {
		value[id_create].push_back();
		if( elem->elem[i].type == CFGTP_STRING )
		{ 
		    value[id_create][i].sval = new string;
		    *(value[id_create][i].sval) = elem->elem[i].rstr->def;
		}
	    	if( elem->elem[i].type == CFGTP_NUMBER ) 
		{
		    value[id_create][i].nval = new double;
		    *(value[id_create][i].nval) = elem->elem[i].rnumb->def;
		}
		if( elem->elem[i].type == CFGTP_SELECT ) 
		{ 
		    value[id_create][i].sval = new string;
		    *(value[id_create][i].sval) = elem->elem[i].rsel->def;
		}
	    }	    
	}

    return(id_rec);
}

int TConfig::FreeRecord( unsigned int id_rec)
{
    if(id_rec >= value.size()) return(-1);
    for(int i=0; i < elem->elem.size(); i++)
    {
	if( elem->elem[i].type == CFGTP_STRING ) delete value[id_rec][i].sval;
	if( elem->elem[i].type == CFGTP_NUMBER ) delete value[id_rec][i].nval;
	if( elem->elem[i].type == CFGTP_SELECT ) delete value[id_rec][i].sval;
    }
    value.erase(value.begin()+id_rec);
}


int TConfig::LoadRecValBD(unsigned int id_rec, string NameFld, string bd)
{
    int b_hd, line, i_elem, i_fld;
    string val;
    double valn;
    
    if(id_rec >= value.size()) return(-1);
    if( (i_fld = elem->NameToId(NameFld)) < 0 || elem->elem[i_fld].type != CFGTP_STRING ) return(-2);
    b_hd = App->BD->OpenBD(bd);
    if(b_hd < 0) return(-3);
    //Find line
    for(line=0; line < App->BD->NLines(b_hd); line++)
    {
	if(App->BD->GetCellS(b_hd,NameFld,line,val) != 0) 
	{
	    App->BD->CloseBD(b_hd);
	    return(-3);
	}
	if(val == *(value[id_rec][i_fld].sval) ) break;
    }
    if(line == App->BD->NLines(b_hd)) 
    {
	App->BD->CloseBD(b_hd);
	return(-4);
    }
    //Load config from found line
    for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
	if(elem->elem[i_elem].type == CFGTP_NUMBER)
	{
	    if(App->BD->GetCellN(b_hd,elem->elem[i_elem].name,line,valn) != 0 ) continue;
	    *(value[id_rec][i_elem].nval) = valn;
	}
	else
	{
	    if(App->BD->GetCellS(b_hd,elem->elem[i_elem].name,line,val) != 0 ) continue;
	    *(value[id_rec][i_elem].sval) = val;
	}
    App->BD->CloseBD(b_hd);

    return(0);
}

int TConfig::SaveRecValBD(unsigned int id_rec, string NameFld, string bd)
{
    int b_hd, line, i_elem, i_fld, i_row;
    string val;
    double valn;

    if(id_rec >= value.size()) return(-1);
    if( (i_fld = elem->NameToId(NameFld)) < 0 || elem->elem[i_fld].type != CFGTP_STRING ) return(-2);
    b_hd = App->BD->OpenBD(bd);
    if(b_hd < 0) 
    {
	b_hd = App->BD->NewBD(bd);
	if(b_hd < 0) return(-3);
    }
    //Find line
    for(line=0; line < App->BD->NLines(b_hd); line++)
    {
	if(App->BD->GetCellS(b_hd,NameFld,line,val) != 0) 
	{

	    line = App->BD->NLines(b_hd);
	    break;
	}
	if(val == *(value[id_rec][i_fld].sval) ) break;
    }

    if(line == App->BD->NLines(b_hd)) App->BD->AddLine(b_hd,line);

    App->Mess->put(0, "Line: %d !",line);

    //Save value
    for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
    	if(elem->elem[i_elem].type == CFGTP_NUMBER)
	{
 	    if(App->BD->RowNameToId(b_hd,elem->elem[i_elem].name) < 0)
		App->BD->AddRow(b_hd,elem->elem[i_elem].name,'N',elem->elem[i_elem].len,elem->elem[i_elem].rnumb->depth);
	    //My be check type of cell !!!
	    App->BD->SetCellN(b_hd,elem->elem[i_elem].name,line,*(value[id_rec][i_elem].nval));	    
	}
	else
	{
 	    if(App->BD->RowNameToId(b_hd,elem->elem[i_elem].name) < 0)		
		App->BD->AddRow(b_hd,elem->elem[i_elem].name,'C',elem->elem[i_elem].len);
	    //My be check type of cell !!!
	    App->BD->SetCellS(b_hd,elem->elem[i_elem].name,line,*(value[id_rec][i_elem].sval));	    
	}
    App->BD->SaveBD(b_hd);
    App->BD->CloseBD(b_hd);

    return(0);
}

int TConfig::LoadValBD( string bd)
{
    int b_hd, i_bd_ln, i_bd_rw, i_elem, i_rec;
    string val;
    double valn;
   
#if debug
    App->Mess->put(0, "LoadValBD: %s !",bd.c_str());
#endif    
    b_hd = App->BD->OpenBD(bd);
    if(b_hd < 0) return(-3);
    for(i_bd_ln = 0; i_bd_ln < App->BD->NLines(b_hd); i_bd_ln++)
    {
	i_rec = value.size();
	InitRecord(i_rec);
    	for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
    	    if(elem->elem[i_elem].type == CFGTP_NUMBER)
    	    {
    		if(App->BD->GetCellN(b_hd,elem->elem[i_elem].name,i_bd_ln,valn) != 0 ) continue;
    		*(value[i_rec][i_elem].nval) = valn;
    	    }
    	    else
    	    {
    		if(App->BD->GetCellS(b_hd,elem->elem[i_elem].name,i_bd_ln,val) != 0 ) continue;
    		*(value[i_rec][i_elem].sval) = val;
    	    }
    }
    App->BD->CloseBD(b_hd);
#if debug
    App->Mess->put(0, "LoadValBD: %s is OK !",bd.c_str());
#endif    
    return(0);
}

int TConfig::SaveValBD( string bd)
{
    int b_hd, i_ln, i_elem;
    
    b_hd = App->BD->OpenBD(bd);
    if(b_hd < 0) 
    {
	b_hd = App->BD->NewBD(bd);
	if(b_hd < 0) return(-3);
    }
    //Clear BD
    while(App->BD->NLines(b_hd)) App->BD->DelLine(b_hd,0);
    for( i_ln=0 ;i_ln < value.size(); i_ln++)
    {
	App->BD->AddLine(b_hd,i_ln);	    
	for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
	    if(elem->elem[i_elem].type == CFGTP_NUMBER)
	    {
		if(App->BD->RowNameToId(b_hd,elem->elem[i_elem].name) < 0)
    		    App->BD->AddRow(b_hd,elem->elem[i_elem].name,'N',elem->elem[i_elem].len,elem->elem[i_elem].rnumb->depth);
    		//My be check type of cell !!!
    		App->BD->SetCellN(b_hd,elem->elem[i_elem].name,i_ln,*(value[i_ln][i_elem].nval));	    
	    }
	    else
	    {
		if(App->BD->RowNameToId(b_hd,elem->elem[i_elem].name) < 0)		
		    App->BD->AddRow(b_hd,elem->elem[i_elem].name,'C',elem->elem[i_elem].len);
		//My be check type of cell !!!
		App->BD->SetCellS(b_hd,elem->elem[i_elem].name,i_ln,*(value[i_ln][i_elem].sval));	    
	    }
    }
    App->BD->SaveBD(b_hd);
    App->BD->CloseBD(b_hd);

    return(0);
}

int TConfig::Size()
{
    return(value.size());
}
