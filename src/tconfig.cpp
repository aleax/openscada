
#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
#include "tconfigelem.h"
#include "tconfig.h"

const char *TConfig::o_name = "TConfig";

TConfig::TConfig( TConfigElem *Elements )
{
    if( Elements == NULL)    
    {
	elem = new TConfigElem;
	single = true;
    }
    else
    {
	elem = Elements;
    	single = false;
    }
    
    elem->config.push_back(this);
    cf_InitRecord(0); 
}

TConfig::~TConfig()
{
    if(elem == NULL) return;
    while(value.size())	cf_FreeRecord(0);	

    for(unsigned i=0; i < elem->config.size() ;i++)
	if(elem->config[i] == this) 
	{
	    elem->config.erase(elem->config.begin()+i);
	    break;
	}
    if( single ) delete elem;
}

string TConfig::cf_Get_SEL( string n_val, unsigned int id)
{
    int i_val;
    
    if( id >= value.size() )                         throw TError("%s: id error!",o_name);
    int id_elem = elem->cfe_NameToId(n_val);
    if( !(elem->elem[id_elem].type&CFG_T_SELECT) )   throw TError("%s: type error!",o_name);
    if( !cf_ViewEl(id_elem,id) )                     throw TError("%s: value no view!",o_name);

    if( elem->elem[id_elem].type&CFG_T_STRING )
	for(i_val = 0; i_val < (int)elem->elem[id_elem].vals.size(); i_val++)
	    if(elem->elem[id_elem].vals[i_val] == *value[id][id_elem].s_val)
		return(elem->elem[id_elem].n_sel[i_val]);
    if( elem->elem[id_elem].type&CFG_T_INT )
	for(i_val = 0; i_val < (int)elem->elem[id_elem].vals.size(); i_val++)
	    if(atoi(elem->elem[id_elem].vals[i_val].c_str()) == value[id][id_elem].i_val)
		return(elem->elem[id_elem].n_sel[i_val]);
    if( elem->elem[id_elem].type&CFG_T_REAL )
	for(i_val = 0; i_val < (int)elem->elem[id_elem].vals.size(); i_val++)
	    if(atof(elem->elem[id_elem].vals[i_val].c_str()) == value[id][id_elem].r_val)
		return(elem->elem[id_elem].n_sel[i_val]);
    if( elem->elem[id_elem].type&CFG_T_BOOLEAN )
	for(i_val = 0; i_val < (int)elem->elem[id_elem].vals.size(); i_val++)
	    if( (elem->elem[id_elem].vals[i_val] == "true"  && value[id][id_elem].b_val == true) ||
		(elem->elem[id_elem].vals[i_val] == "false" && value[id][id_elem].b_val == false) )
		return(elem->elem[id_elem].n_sel[i_val]);
    throw TError("%s: type or select error!",o_name); 
}

string TConfig::cf_Get_S( string n_val, unsigned int id )
{
    if( id >= value.size() )                         throw TError("%s: id error!",o_name);
    int id_elem = elem->cfe_NameToId(n_val);
    if( !(elem->elem[id_elem].type&CFG_T_STRING) )   throw TError("%s: type error!",o_name);
    if( !cf_ViewEl(id_elem,id) )                        throw TError("%s: value no view!",o_name);

    return(*value[id][id_elem].s_val);
}

double TConfig::cf_Get_R( string n_val, unsigned int id )
{
    if( id >= value.size() )                      throw TError("%s: id error!",o_name);
    int id_elem = elem->cfe_NameToId(n_val);
    if( !cf_ViewEl(id_elem,id) )                     throw TError("%s: value no view!",o_name);

    if( elem->elem[id_elem].type&CFG_T_STRING )   throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_INT )     return((double)value[id][id_elem].i_val);
    else if( elem->elem[id_elem].type&CFG_T_BOOLEAN ) return((double)value[id][id_elem].b_val);
    return(value[id][id_elem].r_val);
}

int TConfig::cf_Get_I( string n_val, unsigned int id)
{
    if( id >= value.size() )                      throw TError("%s: id error!",o_name);
    int id_elem = elem->cfe_NameToId(n_val);
    if( !cf_ViewEl(id_elem,id) )                     throw TError("%s: value no view!",o_name);

    if( elem->elem[id_elem].type&CFG_T_STRING )   throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_REAL )    return((int)value[id][id_elem].r_val);
    else if( elem->elem[id_elem].type&CFG_T_BOOLEAN ) return((int)value[id][id_elem].b_val);
    return(value[id][id_elem].i_val);
}

bool TConfig::cf_Get_B( string n_val, unsigned int id)
{
    if( id >= value.size() )                      throw TError("%s: id error!",o_name);
    int id_elem = elem->cfe_NameToId(n_val);
    if( !cf_ViewEl(id_elem,id) )                     throw TError("%s: value no view!",o_name);
    
    if( elem->elem[id_elem].type&CFG_T_STRING )   throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_REAL ) return((bool)value[id][id_elem].r_val);
    else if( elem->elem[id_elem].type&CFG_T_INT )  return((bool)value[id][id_elem].i_val);
    return(value[id][id_elem].b_val);
}

void TConfig::cf_Set_SEL( string n_val, string val, unsigned int id)
{
    int i_val;
    
    if( id >= value.size() )                        throw TError("%s: id error!",o_name);
    int id_elem = elem->cfe_NameToId(n_val);
    if( !(elem->elem[id_elem].type&CFG_T_SELECT) )  throw TError("%s: type error!",o_name);
    if( !cf_ViewEl(id_elem,id) )                       throw TError("%s: value no view!",o_name);

    
    for(i_val = 0; i_val < (int)elem->elem[id_elem].n_sel.size(); i_val++)
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

void TConfig::cf_Set_S( string n_val, string val, unsigned int id)
{
    if( id >= value.size() )                        throw TError("%s: id error!",o_name);
    int id_elem = elem->cfe_NameToId(n_val);
    if( !(elem->elem[id_elem].type&CFG_T_STRING) )  throw TError("%s: type error!",o_name);
    if( !cf_ViewEl(id_elem,id) )                       throw TError("%s: value no view!",o_name);
    
    if( elem->elem[id_elem].type&CFG_T_SELECT )    
    {
	int i_val;
	for( i_val=0; i_val < (int)elem->elem[id_elem].vals.size(); i_val++)
	    if( elem->elem[id_elem].vals[i_val] == val ) 
	    {
		*(value[id][id_elem].s_val) = val;
		break;
	    }
	if(i_val == (int)elem->elem[id_elem].vals.size()) throw TError("%s: selectable element error!",o_name);
    }
    else *(value[id][id_elem].s_val) = val;
}

void TConfig::cf_Set_R( string n_val, double val, unsigned int id)
{
    if( id >= value.size() )                      throw TError("%s: id error!",o_name);
    int id_elem = elem->cfe_NameToId(n_val);
    if( !cf_ViewEl(id_elem,id) )                     throw TError("%s: value no view!",o_name);
    if( elem->elem[id_elem].type&CFG_T_STRING )   throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_INT )     cf_Set_I(n_val,(int)val,id); 
    else if( elem->elem[id_elem].type&CFG_T_BOOLEAN ) cf_Set_B(n_val,(bool)val,id); 
    
    if( elem->elem[id_elem].type&CFG_T_SELECT )
    {
	int i_val;
	for( i_val=0; i_val < (int)elem->elem[id_elem].vals.size(); i_val++)
	    if( atof(elem->elem[id_elem].vals[i_val].c_str()) == val ) 
	    { value[id][id_elem].r_val = val; break; }
	if(i_val == (int)elem->elem[id_elem].vals.size()) throw TError("%s: selectable element error!",o_name);
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

void TConfig::cf_Set_I( string n_val, int val, unsigned int id)
{
    if( id >= value.size() )                     throw TError("%s: id error!",o_name);
    int id_elem = elem->cfe_NameToId(n_val);
    if( !cf_ViewEl(id_elem,id) )                    throw TError("%s: value no view!",o_name);
    if( elem->elem[id_elem].type&CFG_T_STRING )  throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_REAL )    cf_Set_R(n_val,(double)val,id); 
    else if( elem->elem[id_elem].type&CFG_T_BOOLEAN ) cf_Set_B(n_val,(bool)val,id); 
    
    if( elem->elem[id_elem].type&CFG_T_SELECT )
    {
	int i_val;
	for( i_val=0; i_val < (int)elem->elem[id_elem].vals.size(); i_val++)
	    if( atoi(elem->elem[id_elem].vals[i_val].c_str()) == val ) 
	    { value[id][id_elem].i_val = val; break; }
	if(i_val == (int)elem->elem[id_elem].vals.size()) throw TError("%s: selectable element error!",o_name);
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

void TConfig::cf_Set_B( string n_val, bool val, unsigned int id)
{
    if( id >= value.size() )                         throw TError("%s: id error!",o_name);
    int id_elem = elem->cfe_NameToId(n_val);
    if( !cf_ViewEl(id_elem,id) )                        throw TError("%s: value no view!",o_name);
    if( elem->elem[id_elem].type&CFG_T_STRING )      throw TError("%s: type error!",o_name);
    else if( elem->elem[id_elem].type&CFG_T_REAL)    cf_Set_R(n_val,(double)val,id); 
    else if( elem->elem[id_elem].type&CFG_T_INT )    cf_Set_I(n_val,(int)val,id); 
    
    if( elem->elem[id_elem].type&CFG_T_SELECT )
    {
	int i_val;
	for( i_val=0; i_val < (int)elem->elem[id_elem].vals.size(); i_val++)
	    if( (elem->elem[id_elem].vals[i_val] == "true" && val == true) || 
	        (elem->elem[id_elem].vals[i_val] == "false" && val == false) ) 
	    { value[id][id_elem].b_val = val; break; }
	if(i_val == (int)elem->elem[id_elem].vals.size()) throw TError("%s: selectable element error!",o_name);
    }
    else value[id][id_elem].b_val = val;
}

int TConfig::cf_AddRecord( unsigned int id)
{
    if( id > cf_Size() ) throw TError("%s: id error!",o_name);

    vector< _EVal > _val;
    for(unsigned i=0; i < elem->elem.size(); i++)
    {
        _EVal _e_val;
	if( elem->elem[i].type&CFG_T_STRING )
	{ 
	    _e_val.s_val = new string;
	    *(_e_val.s_val) = elem->elem[i].def;
	}
	if( elem->elem[i].type&CFG_T_INT ) 
	    _e_val.i_val = atoi(elem->elem[i].def.c_str());
	if( elem->elem[i].type&CFG_T_REAL ) 
	    _e_val.r_val = atof(elem->elem[i].def.c_str());
	if( elem->elem[i].type&CFG_T_BOOLEAN ) 
	    if(elem->elem[i].def == "true") _e_val.b_val = true;
	    else _e_val.b_val = false;    
	_val.push_back(_e_val);
    }
    value.insert(value.begin()+id,_val);

    return(id);   
}
    
int TConfig::cf_InitRecord( unsigned int id )
{
    if(id < cf_Size())
	for(unsigned i=0; i < elem->elem.size(); i++)
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
	while(id >= cf_Size()) cf_AddRecord( cf_Size() );

    return(id);
}

void TConfig::cf_FreeRecord( unsigned int id)
{
    if( id >= value.size() ) throw TError("%s: id error!",o_name);
    for(unsigned i=0; i < elem->elem.size(); i++)
	if( elem->elem[i].type&CFG_T_STRING ) delete value[id][i].s_val;
    value.erase(value.begin()+id);
}

void TConfig::cf_FreeDubl( string n_val, bool mode )
{
    int id_elem = elem->cfe_NameToId(n_val);
    if( !mode )
    {
	for(unsigned i_cfg = 0; i_cfg < cf_Size()-1; i_cfg++)
	    for(unsigned i_cfg1 = i_cfg+1; i_cfg1 < cf_Size(); i_cfg1++)
	    {   
		if( elem->elem[id_elem].type&CFG_T_STRING && 
			*(value[i_cfg][id_elem].s_val) != *(value[i_cfg1][id_elem].s_val) ) continue;
		else if( elem->elem[id_elem].type&CFG_T_INT && 
			value[i_cfg][id_elem].i_val != value[i_cfg1][id_elem].i_val) continue;
		else if( elem->elem[id_elem].type&CFG_T_REAL && 
			value[i_cfg][id_elem].r_val != value[i_cfg1][id_elem].r_val) continue;
		else if( elem->elem[id_elem].type&CFG_T_BOOLEAN && 
			value[i_cfg][id_elem].b_val != value[i_cfg1][id_elem].b_val) continue;
		cf_FreeRecord( i_cfg1 ); 
		i_cfg1--; 
	    }
    }
    else
    {
	for(int i_cfg = cf_Size()-1; i_cfg > 0; i_cfg--)
	    for(int i_cfg1 = i_cfg-1; i_cfg1 >= 0; i_cfg1--)
	    {   
		if( elem->elem[id_elem].type&CFG_T_STRING && 
			*(value[i_cfg][id_elem].s_val) != *(value[i_cfg1][id_elem].s_val) ) continue;
		else if( elem->elem[id_elem].type&CFG_T_INT && 
			value[i_cfg][id_elem].i_val != value[i_cfg1][id_elem].i_val) continue;
		else if( elem->elem[id_elem].type&CFG_T_REAL && 
			value[i_cfg][id_elem].r_val != value[i_cfg1][id_elem].r_val) continue;
		else if( elem->elem[id_elem].type&CFG_T_BOOLEAN && 
			value[i_cfg][id_elem].b_val != value[i_cfg1][id_elem].b_val) continue;
		cf_FreeRecord( i_cfg1 ); 
		i_cfg--; 
	    }	
    }
}

void TConfig::cf_LoadValBD( string NameFld, TTable &table, unsigned int id_rec )
{
    int line;
    string val;
    
    if(id_rec >= value.size())                   throw TError("%s: id of record error!",o_name);
    int i_fld = elem->cfe_NameToId(NameFld);
    if( !(elem->elem[i_fld].type&CFG_T_STRING) ) throw TError("%s: type of individual field no string!",o_name);
    //Find line
    for(line=0; line < table.NLines(); line++)
	if( table.GetCellS(table.ColumNameToId(NameFld),line) == *(value[id_rec][i_fld].s_val) ) break;
    if(line == table.NLines( )) 
	throw TError("%s: cell %s no avoid into table!",o_name,value[id_rec][i_fld].s_val->c_str());
    //Load config from found line
    return(cf_LoadValBD(line,table,id_rec));
}

void TConfig::cf_LoadValBD(int line_bd, TTable &table, unsigned int id_rec )
{
    int i_elem;
    string val;

    if(id_rec >= value.size()) throw TError("%s: id of record error!",o_name);
    for(i_elem=0; i_elem < (int)elem->elem.size(); i_elem++)
    {
	try
	{
	    if(elem->elem[i_elem].type&CFG_T_STRING)
		*(value[id_rec][i_elem].s_val) = table.GetCellS(table.ColumNameToId(elem->elem[i_elem].name),line_bd);
	    else if(elem->elem[i_elem].type&CFG_T_INT)
		value[id_rec][i_elem].i_val = table.GetCellI(table.ColumNameToId(elem->elem[i_elem].name),line_bd);	   
	    else if(elem->elem[i_elem].type&CFG_T_REAL)
		value[id_rec][i_elem].r_val = table.GetCellR(table.ColumNameToId(elem->elem[i_elem].name),line_bd);	   
	    else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		value[id_rec][i_elem].b_val = table.GetCellB(table.ColumNameToId(elem->elem[i_elem].name),line_bd);    
	}
	catch(...){ }
    }
}

void TConfig::cf_SaveValBD( string NameFld, TTable &table, unsigned int id_rec)
{
    int line;

    if(id_rec >= value.size())                   throw TError("%s: id of record error!",o_name);
    int i_fld = elem->cfe_NameToId(NameFld);
    if( !(elem->elem[i_fld].type&CFG_T_STRING) ) throw TError("%s: type of individual field no string!",o_name);
    //Find line
    for(line=0; line < table.NLines(); line++)
    {
	try
	{ 
	    if(table.GetCellS(table.ColumNameToId(NameFld),line) == *(value[id_rec][i_fld].s_val) )	
		break; 
	}
	catch(...)
	{
	    line = table.NLines();
	    break;
	}
    }
    cf_SaveValBD(line, table, id_rec);
}

void TConfig::cf_SaveValBD( int line_bd, TTable &table, unsigned int id_rec)
{
    int i_elem;

    if(id_rec >= value.size())     throw TError("%s: id of record error!",o_name);
    if(line_bd == table.NLines()) table.AddLine(line_bd);

    for(i_elem=0; i_elem < (int)elem->elem.size(); i_elem++)
    {
	try
	{
    	    if(elem->elem[i_elem].type&CFG_T_STRING)
    		table.SetCellS(table.ColumNameToId(elem->elem[i_elem].name),line_bd,*(value[id_rec][i_elem].s_val));	    
	    else if(elem->elem[i_elem].type&CFG_T_INT)
	    	table.SetCellI(table.ColumNameToId(elem->elem[i_elem].name),line_bd,value[id_rec][i_elem].i_val);	    
	    else if(elem->elem[i_elem].type&CFG_T_REAL)
		table.SetCellR(table.ColumNameToId(elem->elem[i_elem].name),line_bd,value[id_rec][i_elem].r_val);	    
	    else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		table.SetCellB(table.ColumNameToId(elem->elem[i_elem].name),line_bd,value[id_rec][i_elem].b_val);
	}
	catch(...){ }
    }
}

void TConfig::cf_LoadAllValBD( TTable &table )
{
    int i_bd_ln, i_elem, i_rec;
    
    for(i_bd_ln = 0; i_bd_ln < table.NLines( ); i_bd_ln++)
    {
	i_rec = value.size();
	cf_InitRecord(i_rec);
    	for(i_elem=0; i_elem < (int)elem->elem.size(); i_elem++)
	{
	    try
	    {
		if(elem->elem[i_elem].type&CFG_T_STRING)
		    *(value[i_rec][i_elem].s_val) = table.GetCellS(table.ColumNameToId(elem->elem[i_elem].name),i_bd_ln);
		else if(elem->elem[i_elem].type&CFG_T_INT)
		    value[i_rec][i_elem].i_val = table.GetCellI(table.ColumNameToId(elem->elem[i_elem].name),i_bd_ln);
		else if(elem->elem[i_elem].type&CFG_T_REAL)
		    value[i_rec][i_elem].r_val = table.GetCellR(table.ColumNameToId(elem->elem[i_elem].name),i_bd_ln);
		else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		    value[i_rec][i_elem].b_val = table.GetCellB(table.ColumNameToId(elem->elem[i_elem].name),i_bd_ln);
	    }
	    catch(...){ }
	}
    }
}


int TConfig::cf_SaveAllValBD( TTable &table )
{
    int i_ln, i_elem;
    
    while(table.NLines()) table.DelLine(0);
    for( i_ln=0 ;i_ln < (int)value.size(); i_ln++)
    {
	table.AddLine(i_ln);	    
	for(i_elem=0; i_elem < (int)elem->elem.size(); i_elem++)
	{
	    try
	    {
		if(elem->elem[i_elem].type&CFG_T_STRING)
		    table.SetCellS(table.ColumNameToId(elem->elem[i_elem].name),i_ln,*(value[i_ln][i_elem].s_val));
		else if(elem->elem[i_elem].type&CFG_T_INT)
		    table.SetCellI(table.ColumNameToId(elem->elem[i_elem].name),i_ln,value[i_ln][i_elem].i_val);	    
		else if(elem->elem[i_elem].type&CFG_T_REAL)
		    table.SetCellR(table.ColumNameToId(elem->elem[i_elem].name),i_ln,value[i_ln][i_elem].r_val);	    
		else if(elem->elem[i_elem].type&CFG_T_BOOLEAN)
		    table.SetCellB(table.ColumNameToId(elem->elem[i_elem].name),i_ln,value[i_ln][i_elem].b_val);
	    }
	    catch(...) {  }
	}
    }
    table.Save( );

    return(0);
}

TConfig & TConfig::operator=(TConfig & Cfg)
{
    if(elem == Cfg.elem)
    {
	while(cf_Size()) cf_FreeRecord(0);
	for(unsigned i_rc=0; i_rc < Cfg.cf_Size(); i_rc++)
	{
	    cf_AddRecord(i_rc);
	    for(unsigned i_el=0; i_el < elem->cfe_Size(); i_el++)
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

int TConfig::cf_AddElem(int id)
{
    for(unsigned val_id=0; val_id < value.size(); val_id++)
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

int TConfig::cf_DelElem(int id)
{
    for(unsigned val_i=0; val_i < value.size(); val_i++)
    {
	if( elem->elem[id].type&CFG_T_STRING ) delete value[val_i][id].s_val;
	value[val_i].erase(value[val_i].begin()+id);
    }
    return(0);
}

void TConfig::cf_ListEl( vector<string> &list, unsigned int id )
{
    list.clear();
    for(unsigned i = 0; i < elem->cfe_Size(); i++)
    	if(cf_ViewEl(i,id)) list.push_back(elem->elem[i].name);
}

bool TConfig::cf_ViewEl( unsigned id_el, unsigned id )
{
    unsigned i_n;
    if( id_el >= elem->cfe_Size() ) throw TError("%s: element id error!");
    if( id >= cf_Size() )          throw TError("%s: config id error!");
    if( !elem->elem[id_el].ElDep.size() ) return(true);
    try{ i_n = elem->cfe_NameToId(elem->elem[id_el].ElDep); } catch(...) { return(false); }
    if( !(elem->elem[i_n].type&CFG_T_SELECT) ) return(false);
    if( !cf_ViewEl(i_n,id) ) return(false);
    if( cf_Get_SEL(elem->elem[id_el].ElDep,id) != elem->elem[id_el].val_dep ) return(false);
    return(true);
}

void TConfig::cf_ConfElem(TConfigElem *Elements)
{
    if(elem == Elements) return;
    if(elem != NULL)
    {
    	while(value.size())	cf_FreeRecord(0);	
	for(unsigned i=0; i < elem->config.size() ;i++)
	    if(elem->config[i] == this) 
	    {
		elem->config.erase(elem->config.begin()+i);
		break;
	    }
	if(single) delete elem;
    }
    
    if( Elements == NULL)    
    {
	elem = new TConfigElem;
	single = true;
    }
    else
    {
	elem = Elements;
    	single = false;
    }
    
    elem->config.push_back(this);
    cf_InitRecord(0); 
}

TConfigElem *TConfig::cf_ConfElem()
{
    if(elem == NULL) throw TError("%s: config element no attach!");
    return(elem);
}

