
#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
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
    return(*value[id][id_elem].s_val);
}

double TConfig::Get_R( string n_val, unsigned int id )
{
    if( id >= value.size() )                      throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( elem->elem[id_elem].type&CFG_T_STRING )   throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_INT )     return((double)value[id][id_elem].i_val);
    else if( elem->elem[id_elem].type&CFG_T_BOOLEAN ) return((double)value[id][id_elem].b_val);
    return(value[id][id_elem].r_val);
}

int TConfig::Get_I( string n_val, unsigned int id)
{
    if( id >= value.size() )                      throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
    if( elem->elem[id_elem].type&CFG_T_STRING )   throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_REAL )    return((int)value[id][id_elem].r_val);
    else if( elem->elem[id_elem].type&CFG_T_BOOLEAN ) return((int)value[id][id_elem].b_val);
    return(value[id][id_elem].i_val);
}

bool TConfig::Get_B( string n_val, unsigned int id)
{
    if( id >= value.size() )                      throw TError("%s: id error!",o_name);
    int id_elem = elem->NameToId(n_val);
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
    if( !(elem->elem[id_elem].type&CFG_T_REAL) )  throw TError("%s: type error!",o_name);
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
    if( !(elem->elem[id_elem].type&CFG_T_INT) )  throw TError("%s: type error!",o_name);
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
    if( !(elem->elem[id_elem].type&CFG_T_BOOLEAN) )  throw TError("%s: type error!",o_name);
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

void TConfig::LoadRecValBD( string NameFld, string bd, unsigned int id_rec )
{
    int b_hd = App->BD->OpenBD(bd);
    try { LoadRecValBD(NameFld, b_hd, id_rec); }
    catch(...)
    {
    	App->BD->CloseBD(b_hd);
	throw;
    }
    App->BD->CloseBD(b_hd);
}

void TConfig::LoadRecValBD( string NameFld, unsigned int hd_bd, unsigned int id_rec )
{
    int line, i_elem;
    string val;
    double valn;
    
    if(id_rec >= value.size())                   throw TError("%s: id of record error!",o_name);
    int i_fld = elem->NameToId(NameFld);
    if( !(elem->elem[i_fld].type&CFG_T_STRING) ) throw TError("%s: type of individual field no string!",o_name);
    //Find line
    for(line=0; line < App->BD->NLines(hd_bd); line++)
	if( App->BD->GetCellS(hd_bd,App->BD->ColumNameToId(hd_bd,NameFld),line) == *(value[id_rec][i_fld].s_val) ) break;
    if(line == App->BD->NLines(hd_bd)) 
	throw TError("%s: cell %s no avoid into table!",o_name,value[id_rec][i_fld].s_val->c_str());
    //Load config from found line
    return(LoadRecValBD(line, hd_bd,id_rec));
}

void TConfig::LoadRecValBD(int line_bd, unsigned int hd_bd, unsigned int id_rec )
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
		*(value[id_rec][i_elem].s_val) = App->BD->GetCellS(hd_bd,App->BD->ColumNameToId(hd_bd,elem->elem[i_elem].name),line_bd);	   
	    else if(elem->elem[i_elem].type&CFG_T_INT)
		value[id_rec][i_elem].i_val = App->BD->GetCellI(hd_bd,App->BD->ColumNameToId(hd_bd,elem->elem[i_elem].name),line_bd);	   
	    else if(elem->elem[i_elem].type&CFG_T_REAL)
		value[id_rec][i_elem].r_val = App->BD->GetCellR(hd_bd,App->BD->ColumNameToId(hd_bd,elem->elem[i_elem].name),line_bd);	   
	    else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		value[id_rec][i_elem].b_val = App->BD->GetCellB(hd_bd,App->BD->ColumNameToId(hd_bd,elem->elem[i_elem].name),line_bd);    
	}
	catch(...){ }
    }
}

void TConfig::SaveRecValBD( string NameFld, string bd, unsigned int id_rec)
{
    int b_hd = App->BD->OpenBD(bd);
    try{ SaveRecValBD(NameFld, b_hd, id_rec); }
    catch(...)
    {
    	App->BD->CloseBD(b_hd);
	throw;
    }
    App->BD->SaveBD(b_hd);
    App->BD->CloseBD(b_hd);
}

void TConfig::SaveRecValBD( string NameFld, unsigned int hd_bd, unsigned int id_rec)
{
    int line, i_elem, i_row;

    if(id_rec >= value.size())                   throw TError("%s: id of record error!",o_name);
    int i_fld = elem->NameToId(NameFld);
    if( !(elem->elem[i_fld].type&CFG_T_STRING) ) throw TError("%s: type of individual field no string!",o_name);
    //Find line
    for(line=0; line < App->BD->NLines(hd_bd); line++)
    {
	try
	{ 
	    if(App->BD->GetCellS(hd_bd,App->BD->ColumNameToId(hd_bd,NameFld),line) == *(value[id_rec][i_fld].s_val) )	break; 
	}
	catch(...)
	{
	    line = App->BD->NLines(hd_bd);
	    break;
	}
    }
    SaveRecValBD(line, hd_bd, id_rec);
}

void TConfig::SaveRecValBD( int line_bd, unsigned int hd_bd, unsigned int id_rec)
{
    int line, i_elem, i_fld, i_row;

    if(id_rec >= value.size())            throw TError("%s: id of record error!",o_name);
    if(line_bd == App->BD->NLines(hd_bd)) App->BD->AddLine(hd_bd,line_bd);

    for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
    {
	try
	{
    	    if(elem->elem[i_elem].type&CFG_T_STRING)
    		App->BD->SetCellS(hd_bd,App->BD->ColumNameToId(hd_bd,elem->elem[i_elem].name),line_bd,*(value[id_rec][i_elem].s_val));	    
	    else if(elem->elem[i_elem].type&CFG_T_INT)
	    	App->BD->SetCellI(hd_bd,App->BD->ColumNameToId(hd_bd,elem->elem[i_elem].name),line_bd,value[id_rec][i_elem].i_val);	    
	    else if(elem->elem[i_elem].type&CFG_T_REAL)
		App->BD->SetCellR(hd_bd,App->BD->ColumNameToId(hd_bd,elem->elem[i_elem].name),line_bd,value[id_rec][i_elem].r_val);	    
	    else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		App->BD->SetCellB(hd_bd,App->BD->ColumNameToId(hd_bd,elem->elem[i_elem].name),line_bd,value[id_rec][i_elem].b_val);
	}
	catch(...){ }
    }
}

void TConfig::LoadValBD( string bd)
{
    int i_bd_ln, i_bd_rw, i_elem, i_rec;
   
#if debug
    App->Mess->put(0, "LoadValBD: %s !",bd.c_str());
#endif    
    int b_hd = App->BD->OpenBD(bd);
    for(i_bd_ln = 0; i_bd_ln < App->BD->NLines(b_hd); i_bd_ln++)
    {
	i_rec = value.size();
	InitRecord(i_rec);
    	for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
	{
	    try
	    {
		if(elem->elem[i_elem].type&CFG_T_STRING)
		    *(value[i_rec][i_elem].s_val) = App->BD->GetCellS(b_hd,App->BD->ColumNameToId(b_hd,elem->elem[i_elem].name),i_bd_ln);
		else if(elem->elem[i_elem].type&CFG_T_INT)
		    value[i_rec][i_elem].i_val = App->BD->GetCellI(b_hd,App->BD->ColumNameToId(b_hd,elem->elem[i_elem].name),i_bd_ln);
		else if(elem->elem[i_elem].type&CFG_T_REAL)
		    value[i_rec][i_elem].r_val = App->BD->GetCellR(b_hd,App->BD->ColumNameToId(b_hd,elem->elem[i_elem].name),i_bd_ln);
		else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		    value[i_rec][i_elem].b_val = App->BD->GetCellB(b_hd,App->BD->ColumNameToId(b_hd,elem->elem[i_elem].name),i_bd_ln);
	    }
	    catch(...){ }
	}
    }
    App->BD->CloseBD(b_hd);
#if debug
    App->Mess->put(0, "LoadValBD: %s is OK !",bd.c_str());
#endif    
}

int TConfig::SaveValBD( string bd)
{
    int b_hd, i_ln, i_elem;
    
    try {b_hd = App->BD->OpenBD(bd);}
    catch(...){ b_hd = App->BD->NewBD(bd); }
    //Clear BD
    while(App->BD->NLines(b_hd)) App->BD->DelLine(b_hd,0);
    for( i_ln=0 ;i_ln < value.size(); i_ln++)
    {
	App->BD->AddLine(b_hd,i_ln);	    
	for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
	{
	    try
	    {
		if(elem->elem[i_elem].type&CFG_T_STRING)
		    App->BD->SetCellS(b_hd,App->BD->ColumNameToId(b_hd,elem->elem[i_elem].name),i_ln,*(value[i_ln][i_elem].s_val));
		else if(elem->elem[i_elem].type&CFG_T_INT)
		    App->BD->SetCellI(b_hd,App->BD->ColumNameToId(b_hd,elem->elem[i_elem].name),i_ln,value[i_ln][i_elem].i_val);	    
		else if(elem->elem[i_elem].type&CFG_T_REAL)
		    App->BD->SetCellR(b_hd,App->BD->ColumNameToId(b_hd,elem->elem[i_elem].name),i_ln,value[i_ln][i_elem].r_val);	    
		else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		    App->BD->SetCellB(b_hd,App->BD->ColumNameToId(b_hd,elem->elem[i_elem].name),i_ln,value[i_ln][i_elem].b_val);
	    }
	    catch(...) {  }
	}
    }
    App->BD->SaveBD(b_hd);
    App->BD->CloseBD(b_hd);

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
