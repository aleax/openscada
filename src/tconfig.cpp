
#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "tconfig.h"

TConfig::TConfig()
{

}

TConfig::~TConfig()
{
    while(Elem.size()) DelElem(0);
}


int TConfig::AddElem(unsigned int id, SElem *element)
{
    vector< SElem >::iterator iter;

    if(element==NULL) return (-1);
    //Find dublicates
    for(iter=Elem.begin(); iter != Elem.end(); iter++)
	if((*iter).name == element->name) return(-1);
    //Add element
    if( id > Elem.size() ) id = Elem.size();
    iter=Elem.begin()+id;
    Elem.insert(iter);
    iter=Elem.begin()+id;
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
    	Elem.erase(iter);
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
    for(int i=0; i < value.size(); i++)
    {
    	vector< SVal >::iterator val_i;
	val_i=value[i].begin()+id;
	value[i].insert(val_i);
	val_i=value[i].begin()+id;
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

int TConfig::DelElem(unsigned int id)
{
    if( id >= Elem.size() ) return(-1);
    if( Elem[id].type == CFGTP_STRING ) delete Elem[id].rstr;
    if( Elem[id].type == CFGTP_NUMBER ) delete Elem[id].rnumb;
    if( Elem[id].type == CFGTP_SELECT ) delete Elem[id].rsel;
    Elem.erase(Elem.begin()+id);
    //Delete value
    for(int i=0; i < value.size(); i++)
    {
	if( Elem[id].type == CFGTP_STRING ) delete value[i][id].sval;
    	if( Elem[id].type == CFGTP_NUMBER ) delete value[i][id].nval;
    	if( Elem[id].type == CFGTP_SELECT ) delete value[i][id].sval;
        value[i].erase(value[i].begin()+id);
    }
    return(0);
}

int TConfig::NElem()
{
    return(Elem.size());
}

int TConfig::NameToId(string name)
{
    for(int i=0; i < Elem.size(); i++)
	if(Elem[i].name == name) return(i);
    return(-1);
}


int TConfig::GetVal( unsigned int id_ctr, string n_val, string & val)
{
    int id_elem;
    
    if( id_ctr >= value.size() )              return(-1);
    if( (id_elem = NameToId(n_val)) < 0  )    return(-2);
    if( Elem[id_elem].type == CFGTP_NUMBER )  return(-3);
    val = *(value[id_ctr][id_elem].sval);

    return(0);
}

int TConfig::GetVal( unsigned int id_ctr, string n_val, double & val)
{
    int id_elem;
    
    if( id_ctr >= value.size() )              return(-1);
    if( (id_elem = NameToId(n_val)) < 0  )    return(-2);
    if( Elem[id_elem].type != CFGTP_NUMBER )  return(-3);
    val = *(value[id_ctr][id_elem].nval);

    return(0);
}

int TConfig::SetVal( unsigned int id_ctr, string n_val, string val)
{
    int id_elem;
    
    if( id_ctr >= value.size() )              return(-1);
    if( (id_elem = NameToId(n_val)) < 0  )    return(-2);
    if( Elem[id_elem].type == CFGTP_NUMBER )  return(-3);
    if( Elem[id_elem].type == CFGTP_STRING )
    	*(value[id_ctr][id_elem].sval) = val;
    if( Elem[id_elem].type == CFGTP_SELECT )
    {
	int i_beg=0, i_end=0;
	do
	{
	   i_end = Elem[id_elem].rsel->val_varnt.find(";",i_beg);
	   if( Elem[id_elem].rsel->val_varnt.substr(i_beg,i_end) == val) 
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
    if( (id_elem = NameToId(n_val)) < 0  )    return(-2);
    if( Elem[id_elem].type != CFGTP_NUMBER )  return(-3);
    if( val < Elem[id_elem].rnumb->min) val = Elem[id_elem].rnumb->min;
    if( val > Elem[id_elem].rnumb->max) val = Elem[id_elem].rnumb->max;
    *(value[id_ctr][id_elem].nval) = val;

    return(0);
}

int TConfig::InitRecord( unsigned int id_rec)
{
    if(id_rec < value.size())
	for(int i=0; i < Elem.size(); i++)
	{
    	    if( Elem[i].type == CFGTP_STRING ) *(value[id_rec][i].sval) = Elem[i].rstr->def;
    	    if( Elem[i].type == CFGTP_NUMBER ) *(value[id_rec][i].nval) = Elem[i].rnumb->def;
	    if( Elem[i].type == CFGTP_SELECT ) *(value[id_rec][i].sval) = Elem[i].rsel->def;
	}
    else
	while(id_rec >= value.size())
	{
	    int id_create = value.size();
	    value.push_back(  );
	    for(int i=0; i < Elem.size(); i++)
    	    {
		value[id_create].push_back();
		if( Elem[i].type == CFGTP_STRING )
		{ 
		    value[id_create][i].sval = new string;
		    *(value[id_create][i].sval) = Elem[i].rstr->def;
		}
	    	if( Elem[i].type == CFGTP_NUMBER ) 
		{
		    value[id_create][i].nval = new double;
		    *(value[id_create][i].nval) = Elem[i].rnumb->def;
		}
		if( Elem[i].type == CFGTP_SELECT ) 
		{ 
		    value[id_create][i].sval = new string;
		    *(value[id_create][i].sval) = Elem[i].rsel->def;
		}
	    }	    
	}

    return(id_rec);
}

int TConfig::FreeRecord( unsigned int id_rec)
{
    if(id_rec >= value.size()) return(-1);
    for(int i=0; i < Elem.size(); i++)
    {
	if( Elem[i].type == CFGTP_STRING ) delete value[id_rec][i].sval;
	if( Elem[i].type == CFGTP_NUMBER ) delete value[id_rec][i].nval;
	if( Elem[i].type == CFGTP_SELECT ) delete value[id_rec][i].sval;
    }
    value.erase(value.begin()+id_rec);
}


int TConfig::LoadRecValBD(unsigned int id_rec, string NameFld, string bd)
{
    int b_hd, line, i_elem, i_fld;
    string val;
    double valn;
    
    if(id_rec >= value.size()) return(-1);
    if( (i_fld = NameToId(NameFld)) < 0 || Elem[i_fld].type != CFGTP_STRING ) return(-2);
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
    for(i_elem=0; i_elem < Elem.size(); i_elem++)
	if(Elem[i_elem].type == CFGTP_NUMBER)
	{
	    if(App->BD->GetCellN(b_hd,Elem[i_elem].name,line,valn) != 0 ) continue;
	    *(value[id_rec][i_elem].nval) = valn;
	}
	else
	{
	    if(App->BD->GetCellS(b_hd,Elem[i_elem].name,line,val) != 0 ) continue;
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
    if( (i_fld = NameToId(NameFld)) < 0 || Elem[i_fld].type != CFGTP_STRING ) return(-2);
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
    for(i_elem=0; i_elem < Elem.size(); i_elem++)
    	if(Elem[i_elem].type == CFGTP_NUMBER)
	{
 	    if(App->BD->RowNameToId(b_hd,Elem[i_elem].name) < 0)
		App->BD->AddRow(b_hd,Elem[i_elem].name,'N',Elem[i_elem].len,Elem[i_elem].rnumb->depth);
	    //My be check type of cell !!!
	    App->BD->SetCellN(b_hd,Elem[i_elem].name,line,*(value[id_rec][i_elem].nval));	    
	}
	else
	{
 	    if(App->BD->RowNameToId(b_hd,Elem[i_elem].name) < 0)		
		App->BD->AddRow(b_hd,Elem[i_elem].name,'C',Elem[i_elem].len);
	    //My be check type of cell !!!
	    App->BD->SetCellS(b_hd,Elem[i_elem].name,line,*(value[id_rec][i_elem].sval));	    
	}
    App->BD->SaveBD(b_hd);
    App->BD->CloseBD(b_hd);

    return(0);
}

