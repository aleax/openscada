
#include "tapplication.h"
#include "tmessage.h"
#include "tbds.h"
#include "tconfigelem.h"
#include "tconfig.h"

const char *TConfig::o_name = "TConfig";

TConfig::TConfig( TConfigElem *Elements ) : elem(Elements)
{
    elem->config.push_back(this);
    InitRecord(0); 
}

TConfig::~TConfig()
{
    while(value.size())	FreeRecord(0);	

    for(int i=0; i < elem->config.size() ;i++)
	if(elem->config[i] == this) 
	{
	    elem->config.erase(elem->config.begin()+i);
	    break;
	}
}

string TConfig::Get_SEL( string n_val, unsigned int id)
{
    int i_val;
    
    if( id >= value.size() )                         throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( !(elem->elem[id_elem].type&CFG_T_SELECT) )   throw TError("%s: type error!",o_name);
    if( !ViewEl(id_elem,id) )                        throw TError("%s: value no view!",o_name);

    if( elem->elem[id_elem].type&CFG_T_STRING )
	for(i_val = 0; i_val < elem->elem[id_elem].vals.size(); i_val++)
	    if(elem->elem[id_elem].vals[i_val] == *value[id][id_elem].s_val)
		return(elem->elem[id_elem].n_sel[i_val]);
    if( elem->elem[id_elem].type&CFG_T_INT )
	for(i_val = 0; i_val < elem->elem[id_elem].vals.size(); i_val++)
	    if(atoi(elem->elem[id_elem].vals[i_val].c_str()) == value[id][id_elem].i_val)
		return(elem->elem[id_elem].n_sel[i_val]);
    if( elem->elem[id_elem].type&CFG_T_REAL )
	for(i_val = 0; i_val < elem->elem[id_elem].vals.size(); i_val++)
	    if(atof(elem->elem[id_elem].vals[i_val].c_str()) == value[id][id_elem].r_val)
		return(elem->elem[id_elem].n_sel[i_val]);
    if( elem->elem[id_elem].type&CFG_T_BOOLEAN )
	for(i_val = 0; i_val < elem->elem[id_elem].vals.size(); i_val++)
	    if( (elem->elem[id_elem].vals[i_val] == "true"  && value[id][id_elem].b_val == true) ||
		(elem->elem[id_elem].vals[i_val] == "false" && value[id][id_elem].b_val == false) )
		return(elem->elem[id_elem].n_sel[i_val]);
    throw TError("%s: type or select error!",o_name); 
}

string TConfig::Get_S( string n_val, unsigned int id )
{
    if( id >= value.size() )                         throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( !(elem->elem[id_elem].type&CFG_T_STRING) )   throw TError("%s: type error!",o_name);
    if( !ViewEl(id_elem,id) )                        throw TError("%s: value no view!",o_name);

    return(*value[id][id_elem].s_val);
}

double TConfig::Get_R( string n_val, unsigned int id )
{
    if( id >= value.size() )                      throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( !ViewEl(id_elem,id) )                     throw TError("%s: value no view!",o_name);

    if( elem->elem[id_elem].type&CFG_T_STRING )   throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_INT )     return((double)value[id][id_elem].i_val);
    else if( elem->elem[id_elem].type&CFG_T_BOOLEAN ) return((double)value[id][id_elem].b_val);
    return(value[id][id_elem].r_val);
}

int TConfig::Get_I( string n_val, unsigned int id)
{
    if( id >= value.size() )                      throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( !ViewEl(id_elem,id) )                     throw TError("%s: value no view!",o_name);

    if( elem->elem[id_elem].type&CFG_T_STRING )   throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_REAL )    return((int)value[id][id_elem].r_val);
    else if( elem->elem[id_elem].type&CFG_T_BOOLEAN ) return((int)value[id][id_elem].b_val);
    return(value[id][id_elem].i_val);
}

bool TConfig::Get_B( string n_val, unsigned int id)
{
    if( id >= value.size() )                      throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( !ViewEl(id_elem,id) )                     throw TError("%s: value no view!",o_name);
    
    if( elem->elem[id_elem].type&CFG_T_STRING )   throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_REAL ) return((bool)value[id][id_elem].r_val);
    else if( elem->elem[id_elem].type&CFG_T_INT )  return((bool)value[id][id_elem].i_val);
    return(value[id][id_elem].b_val);
}

void TConfig::Set_SEL( string n_val, string val, unsigned int id)
{
    int i_val;
    
    if( id >= value.size() )                        throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( !(elem->elem[id_elem].type&CFG_T_SELECT) )  throw TError("%s: type error!",o_name);
    if( !ViewEl(id_elem,id) )                       throw TError("%s: value no view!",o_name);
    
    for(i_val = 0; i_val < elem->elem[id_elem].n_sel.size(); i_val++)
	if( elem->elem[id_elem].n_sel[i_val] == val )
	{
	    if( elem->elem[id_elem].type&CFG_T_STRING )
	    { *(value[id][id_elem].s_val) = elem->elem[id_elem].vals[i_val]; return; }
	    else if( elem->elem[id_elem].type&CFG_T_REAL )
	    { value[id][id_elem].r_val = atof(elem->elem[id_elem].vals[i_val].c_str()); return; }
	    else if( elem->elem[id_elem].type&CFG_T_INT )
	    { value[id][id_elem].i_val = atoi(elem->elem[id_elem].vals[i_val].c_str()); return; }
	    else if( elem->elem[id_elem].type&CFG_T_BOOLEAN )
	    { 
		if(elem->elem[id_elem].vals[i_val] == "true")       value[id][id_elem].b_val = true; 
		else if(elem->elem[id_elem].vals[i_val] == "false") value[id][id_elem].b_val = false; 
		return; 
	    }
	}
    throw TError("%s: type or select error!",o_name); 
}

void TConfig::Set_S( string n_val, string val, unsigned int id)
{
    if( id >= value.size() )                        throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( !(elem->elem[id_elem].type&CFG_T_STRING) )  throw TError("%s: type error!",o_name);
    if( !ViewEl(id_elem,id) )                       throw TError("%s: value no view!",o_name);
    
    if( elem->elem[id_elem].type&CFG_T_SELECT )    
    {
	int i_val;
	for( i_val=0; i_val < elem->elem[id_elem].vals.size(); i_val++)
	    if( elem->elem[id_elem].vals[i_val] == val ) 
	    {
		*(value[id][id_elem].s_val) = val;
		break;
	    }
	if(i_val == elem->elem[id_elem].vals.size()) throw TError("%s: selectable element error!",o_name);
    }
    else *(value[id][id_elem].s_val) = val;
}

void TConfig::Set_R( string n_val, double val, unsigned int id)
{
    if( id >= value.size() )                      throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( !ViewEl(id_elem,id) )                     throw TError("%s: value no view!",o_name);
    if( elem->elem[id_elem].type&CFG_T_STRING )   throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_INT )     Set_I(n_val,(int)val,id); 
    else if( elem->elem[id_elem].type&CFG_T_BOOLEAN ) Set_B(n_val,(bool)val,id); 
    
    if( elem->elem[id_elem].type&CFG_T_SELECT )
    {
	int i_val;
	for( i_val=0; i_val < elem->elem[id_elem].vals.size(); i_val++)
	    if( atof(elem->elem[id_elem].vals[i_val].c_str()) == val ) 
	    { value[id][id_elem].r_val = val; break; }
	if(i_val == elem->elem[id_elem].vals.size()) throw TError("%s: selectable element error!",o_name);
    }
    else
    {
	if( elem->elem[id_elem].vals.size() == 2 )
	{
	    if( val < atof(elem->elem[id_elem].vals[0].c_str()) ) val = atof(elem->elem[id_elem].vals[0].c_str());
	    else if( val > atof(elem->elem[id_elem].vals[1].c_str()) ) val = atof(elem->elem[id_elem].vals[1].c_str());
	}
       	value[id][id_elem].r_val = val;
    }
}

void TConfig::Set_I( string n_val, int val, unsigned int id)
{
    if( id >= value.size() )                     throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( !ViewEl(id_elem,id) )                    throw TError("%s: value no view!",o_name);
    if( elem->elem[id_elem].type&CFG_T_STRING )  throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_REAL )    Set_R(n_val,(double)val,id); 
    else if( elem->elem[id_elem].type&CFG_T_BOOLEAN ) Set_B(n_val,(bool)val,id); 
    
    if( elem->elem[id_elem].type&CFG_T_SELECT )
    {
	int i_val;
	for( i_val=0; i_val < elem->elem[id_elem].vals.size(); i_val++)
	    if( atoi(elem->elem[id_elem].vals[i_val].c_str()) == val ) 
	    { value[id][id_elem].i_val = val; break; }
	if(i_val == elem->elem[id_elem].vals.size()) throw TError("%s: selectable element error!",o_name);
    }
    else
    {
	if( elem->elem[id_elem].vals.size() == 2 )
	{
	    if( val < atoi(elem->elem[id_elem].vals[0].c_str()) )      val = atoi(elem->elem[id_elem].vals[0].c_str());
	    else if( val > atoi(elem->elem[id_elem].vals[1].c_str()) ) val = atoi(elem->elem[id_elem].vals[1].c_str());
	}
       	value[id][id_elem].i_val = val;
    }
}

void TConfig::Set_B( string n_val, bool val, unsigned int id)
{
    if( id >= value.size() )                         throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( !ViewEl(id_elem,id) )                        throw TError("%s: value no view!",o_name);
    if( elem->elem[id_elem].type&CFG_T_STRING )      throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_REAL)    Set_R(n_val,(double)val,id); 
    else if( elem->elem[id_elem].type&CFG_T_INT )    Set_I(n_val,(int)val,id); 
    
    if( elem->elem[id_elem].type&CFG_T_SELECT )
    {
	int i_val;
	for( i_val=0; i_val < elem->elem[id_elem].vals.size(); i_val++)
	    if( (elem->elem[id_elem].vals[i_val] == "true" && val == true) || 
	        (elem->elem[id_elem].vals[i_val] == "false" && val == false) ) 
	    { value[id][id_elem].b_val = val; break; }
	if(i_val == elem->elem[id_elem].vals.size()) throw TError("%s: selectable element error!",o_name);
    }
    else value[id][id_elem].b_val = val;
}

int TConfig::AddRecord( unsigned int id)
{
    if( id > Size() ) throw TError("%s: id error!",o_name);
    if( id == Size() ) value.push_back();
    else               value.insert(value.begin()+id);
    for(int i=0; i < elem->elem.size(); i++)
    {
	value[id].push_back();
	if( elem->elem[i].type&CFG_T_STRING )
	{ 
	    value[id][i].s_val = new string;
	    *(value[id][i].s_val) = elem->elem[i].def;
	}
	if( elem->elem[i].type&CFG_T_INT ) 
	    value[id][i].i_val = atoi(elem->elem[i].def.c_str());
	if( elem->elem[i].type&CFG_T_REAL ) 
	    value[id][i].r_val = atof(elem->elem[i].def.c_str());
	if( elem->elem[i].type&CFG_T_BOOLEAN ) 
	    if(elem->elem[i].def == "true") value[id][i].b_val = true;
	    else value[id][i].b_val = false;    
    }
    return(id);   
}
    
int TConfig::InitRecord( unsigned int id )
{
    if(id < Size())
	for(int i=0; i < elem->elem.size(); i++)
	{
    	    if( elem->elem[i].type&CFG_T_STRING )
    	    { 
    		value[id][i].s_val = new string;
    		*(value[id][i].s_val) = elem->elem[i].def;
    	    }
    	    if( elem->elem[i].type&CFG_T_INT ) 
    		value[id][i].i_val = atoi(elem->elem[i].def.c_str());
    	    if( elem->elem[i].type&CFG_T_REAL ) 
    		value[id][i].r_val = atof(elem->elem[i].def.c_str());
    	    if( elem->elem[i].type&CFG_T_BOOLEAN ) 
    		if(elem->elem[i].def == "true") value[id][i].b_val = true;
    		else value[id][i].b_val = false;	    
	}
    else
	while(id >= Size()) AddRecord( Size() );

    return(id);
}

void TConfig::FreeRecord( unsigned int id)
{
    if( id >= value.size() ) throw TError("%s: id error!",o_name);
    for(int i=0; i < elem->elem.size(); i++)
	if( elem->elem[i].type&CFG_T_STRING ) delete value[id][i].s_val;
    value.erase(value.begin()+id);
}


void TConfig::LoadValBD( string NameFld, string t_bd, string n_bd, string n_tb, unsigned int id_rec )
{
    int t_hd = App->BD->OpenTable(t_bd,n_bd,n_tb);
    try { LoadValBD(NameFld, t_hd, id_rec); }
    catch(...)
    {
    	App->BD->CloseTable(t_hd);
	throw;
    }
    App->BD->CloseTable(t_hd);
}

void TConfig::LoadValBD( string NameFld, unsigned int hd_bd, unsigned int id_rec )
{
    int line, i_elem;
    string val;
    double valn;
    
    if(id_rec >= value.size())                   throw TError("%s: id of record error!",o_name);
    int i_fld = elem->NameToId(NameFld);
    if( !(elem->elem[i_fld].type&CFG_T_STRING) ) throw TError("%s: type of individual field no string!",o_name);
    //Find line
    for(line=0; line < App->BD->at_tbl(hd_bd)->NLines(); line++)
	if( App->BD->at_tbl(hd_bd)->GetCellS(App->BD->at_tbl(hd_bd)->ColumNameToId(NameFld),line) == *(value[id_rec][i_fld].s_val) ) break;
    if(line == App->BD->at_tbl(hd_bd)->NLines( )) 
	throw TError("%s: cell %s no avoid into table!",o_name,value[id_rec][i_fld].s_val->c_str());
    //Load config from found line
    return(LoadValBD(line, hd_bd,id_rec));
}

void TConfig::LoadValBD(int line_bd, unsigned int hd_bd, unsigned int id_rec )
{
    int line, i_elem, i_fld;
    string val;
    double valn;

    if(id_rec >= value.size()) throw TError("%s: id of record error!",o_name);
    for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
    {
	try
	{
	    if(elem->elem[i_elem].type&CFG_T_STRING)
		*(value[id_rec][i_elem].s_val) = App->BD->at_tbl(hd_bd)->GetCellS(App->BD->at_tbl(hd_bd)->ColumNameToId(elem->elem[i_elem].name),line_bd);
	    else if(elem->elem[i_elem].type&CFG_T_INT)
		value[id_rec][i_elem].i_val = App->BD->at_tbl(hd_bd)->GetCellI(App->BD->at_tbl(hd_bd)->ColumNameToId(elem->elem[i_elem].name),line_bd);	   
	    else if(elem->elem[i_elem].type&CFG_T_REAL)
		value[id_rec][i_elem].r_val = App->BD->at_tbl(hd_bd)->GetCellR(App->BD->at_tbl(hd_bd)->ColumNameToId(elem->elem[i_elem].name),line_bd);	   
	    else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		value[id_rec][i_elem].b_val = App->BD->at_tbl(hd_bd)->GetCellB(App->BD->at_tbl(hd_bd)->ColumNameToId(elem->elem[i_elem].name),line_bd);    
	}
	catch(...){ }
    }
}


void TConfig::SaveValBD( string NameFld, string t_bd, string n_bd, string n_tb, unsigned int id_rec)
{
    int t_hd = App->BD->OpenTable(t_bd,n_bd,n_tb);
    try{ SaveValBD(NameFld, t_hd, id_rec); }
    catch(...)
    {
    	App->BD->CloseTable(t_hd);
	throw;
    }
    App->BD->at_tbl(t_hd)->Save();
    App->BD->CloseTable(t_hd);
}

void TConfig::SaveValBD( string NameFld, unsigned int hd_bd, unsigned int id_rec)
{
    int line, i_elem, i_row;

    if(id_rec >= value.size())                   throw TError("%s: id of record error!",o_name);
    int i_fld = elem->NameToId(NameFld);
    if( !(elem->elem[i_fld].type&CFG_T_STRING) ) throw TError("%s: type of individual field no string!",o_name);
    //Find line
    for(line=0; line < App->BD->at_tbl(hd_bd)->NLines(); line++)
    {
	try
	{ 
	    if(App->BD->at_tbl(hd_bd)->GetCellS(App->BD->at_tbl(hd_bd)->ColumNameToId(NameFld),line) == *(value[id_rec][i_fld].s_val) )	break; 
	}
	catch(...)
	{
	    line = App->BD->at_tbl(hd_bd)->NLines();
	    break;
	}
    }
    SaveValBD(line, hd_bd, id_rec);
}

void TConfig::SaveValBD( int line_bd, unsigned int hd_bd, unsigned int id_rec)
{
    int line, i_elem, i_fld, i_row;

    if(id_rec >= value.size())            throw TError("%s: id of record error!",o_name);
    if(line_bd == App->BD->at_tbl(hd_bd)->NLines()) App->BD->at_tbl(hd_bd)->AddLine(line_bd);

    for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
    {
	try
	{
    	    if(elem->elem[i_elem].type&CFG_T_STRING)
    		App->BD->at_tbl(hd_bd)->SetCellS(App->BD->at_tbl(hd_bd)->ColumNameToId(elem->elem[i_elem].name),line_bd,*(value[id_rec][i_elem].s_val));	    
	    else if(elem->elem[i_elem].type&CFG_T_INT)
	    	App->BD->at_tbl(hd_bd)->SetCellI(App->BD->at_tbl(hd_bd)->ColumNameToId(elem->elem[i_elem].name),line_bd,value[id_rec][i_elem].i_val);	    
	    else if(elem->elem[i_elem].type&CFG_T_REAL)
		App->BD->at_tbl(hd_bd)->SetCellR(App->BD->at_tbl(hd_bd)->ColumNameToId(elem->elem[i_elem].name),line_bd,value[id_rec][i_elem].r_val);	    
	    else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		App->BD->at_tbl(hd_bd)->SetCellB(App->BD->at_tbl(hd_bd)->ColumNameToId(elem->elem[i_elem].name),line_bd,value[id_rec][i_elem].b_val);
	}
	catch(...){ }
    }
}

void TConfig::LoadAllValBD( string t_bd, string n_bd, string n_tb )
{
    int i_bd_ln, i_bd_rw, i_elem, i_rec;
   
#if OSC_DEBUG
    App->Mess->put(0, "LoadValBD: %s !",n_tb.c_str());
#endif    
    int t_hd = App->BD->OpenTable(t_bd,n_bd,n_tb);
    for(i_bd_ln = 0; i_bd_ln < App->BD->at_tbl(t_hd)->NLines( ); i_bd_ln++)
    {
	i_rec = value.size();
	InitRecord(i_rec);
    	for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
	{
	    try
	    {
		if(elem->elem[i_elem].type&CFG_T_STRING)
		    *(value[i_rec][i_elem].s_val) = App->BD->at_tbl(t_hd)->GetCellS(App->BD->at_tbl(t_hd)->ColumNameToId(elem->elem[i_elem].name),i_bd_ln);
		else if(elem->elem[i_elem].type&CFG_T_INT)
		    value[i_rec][i_elem].i_val = App->BD->at_tbl(t_hd)->GetCellI(App->BD->at_tbl(t_hd)->ColumNameToId(elem->elem[i_elem].name),i_bd_ln);
		else if(elem->elem[i_elem].type&CFG_T_REAL)
		    value[i_rec][i_elem].r_val = App->BD->at_tbl(t_hd)->GetCellR(App->BD->at_tbl(t_hd)->ColumNameToId(elem->elem[i_elem].name),i_bd_ln);
		else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		    value[i_rec][i_elem].b_val = App->BD->at_tbl(t_hd)->GetCellB(App->BD->at_tbl(t_hd)->ColumNameToId(elem->elem[i_elem].name),i_bd_ln);
	    }
	    catch(...){ }
	}
    }
    App->BD->CloseTable(t_hd);
#if OSC_DEBUG
    App->Mess->put(0, "LoadValBD: %s is OK !",n_tb.c_str());
#endif    
}


int TConfig::SaveAllValBD( string t_bd, string n_bd, string n_tb)
{
    int t_hd, i_ln, i_elem;
    
    try {t_hd = App->BD->OpenTable(t_bd,n_bd,n_tb);}
    catch(...){ t_hd = App->BD->OpenTable(t_bd,n_bd,n_tb,true); }
    //Clear BD
    while(App->BD->at_tbl(t_hd)->NLines()) App->BD->at_tbl(t_hd)->DelLine(0);
    for( i_ln=0 ;i_ln < value.size(); i_ln++)
    {
	App->BD->at_tbl(t_hd)->AddLine(i_ln);	    
	for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
	{
	    try
	    {
		if(elem->elem[i_elem].type&CFG_T_STRING)
		    App->BD->at_tbl(t_hd)->SetCellS(App->BD->at_tbl(t_hd)->ColumNameToId(elem->elem[i_elem].name),i_ln,*(value[i_ln][i_elem].s_val));
		else if(elem->elem[i_elem].type&CFG_T_INT)
		    App->BD->at_tbl(t_hd)->SetCellI(App->BD->at_tbl(t_hd)->ColumNameToId(elem->elem[i_elem].name),i_ln,value[i_ln][i_elem].i_val);	    
		else if(elem->elem[i_elem].type&CFG_T_REAL)
		    App->BD->at_tbl(t_hd)->SetCellR(App->BD->at_tbl(t_hd)->ColumNameToId(elem->elem[i_elem].name),i_ln,value[i_ln][i_elem].r_val);	    
		else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		    App->BD->at_tbl(t_hd)->SetCellB(App->BD->at_tbl(t_hd)->ColumNameToId(elem->elem[i_elem].name),i_ln,value[i_ln][i_elem].b_val);
	    }
	    catch(...) {  }
	}
    }
    App->BD->at_tbl(t_hd)->Save( );
    App->BD->CloseTable(t_hd);

    return(0);
}

TConfig & TConfig::operator=(TConfig & Cfg)
{
    if(elem == Cfg.elem)
    {
	while(Size()) FreeRecord(0);
	for(unsigned i_rc=0; i_rc < Cfg.Size(); i_rc++)
	{
	    AddRecord(i_rc);
	    for(unsigned i_el=0; i_el < elem->Size(); i_el++)
	    {
		try
		{
		    if(elem->elem[i_el].type&CFG_T_STRING)
			*value[i_rc][i_el].s_val = *Cfg.value[i_rc][i_el].s_val;
		    else if(elem->elem[i_el].type&CFG_T_INT)
			value[i_rc][i_el].i_val = Cfg.value[i_rc][i_el].i_val;
		    else if(elem->elem[i_el].type&CFG_T_REAL)
			value[i_rc][i_el].r_val = Cfg.value[i_rc][i_el].r_val;
		    else if(elem->elem[i_el].type&CFG_T_BOOLEAN)
			value[i_rc][i_el].b_val = Cfg.value[i_rc][i_el].b_val;
		}
		catch(...){  }
    	    }
	}
    }
    return(*this);
}

int TConfig::AddElem(int id)
{
    for(int val_id=0; val_id < value.size(); val_id++)
    {
	vector< _EVal >::iterator val_i;
	    
	val_i = value[val_id].begin()+id;
	value[val_id].insert(val_i);
	val_i = value[val_id].begin()+id;

	if( elem->elem[id].type&CFG_T_STRING )
	{
	    (*val_i).s_val    = new string;
	    *((*val_i).s_val) = elem->elem[id].def;
	}
	if( elem->elem[id].type&CFG_T_INT )
	    (*val_i).i_val    = atoi(elem->elem[id].def.c_str());
	if( elem->elem[id].type&CFG_T_REAL )
	    (*val_i).r_val    = atof(elem->elem[id].def.c_str());
	if( elem->elem[id].type&CFG_T_BOOLEAN )
	    if( elem->elem[id].def == "true") (*val_i).b_val = true; else (*val_i).b_val = false;
    }
    return(0);
}

int TConfig::DelElem(int id)
{
    for(int val_i=0; val_i < value.size(); val_i++)
    {
	if( elem->elem[id].type&CFG_T_STRING ) delete value[val_i][id].s_val;
	value[val_i].erase(value[val_i].begin()+id);
    }
    return(0);
}

void TConfig::ListEl( vector<string> &list, unsigned int id )
{
    list.clear();
    for(unsigned i = 0; i < elem->Size(); i++)
    	if(ViewEl(i,id)) list.push_back(elem->elem[i].name);
}

bool TConfig::ViewEl( unsigned id_el, unsigned id )
{
    unsigned i_n;
    if( id_el >= elem->Size() ) throw TError("%s: element id error!");
    if( id >= Size() )          throw TError("%s: config id error!");
    if( !elem->elem[id_el].ElDep.size() ) return(true);
    try{ i_n = elem->NameToId(elem->elem[id_el].ElDep); } catch(...) { return(false); }
    if( !(elem->elem[i_n].type&CFG_T_SELECT) ) return(false);
    if( !ViewEl(i_n,id) ) return(false);
    if( Get_SEL(elem->elem[id_el].ElDep,id) != elem->elem[id_el].val_dep ) return(false);
    return(true);
}

