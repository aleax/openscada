
#include "tapplication.h"
#include "tmessage.h"
#include "tbd.h"
#include "tconfigelem.h"
#include "tconfig.h"

TConfig::TConfig( TConfigElem *Elements ) : elem(Elements)  
{
    elem->config.push_back(this);
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

int TConfig::GetVal( string n_val, string & val)
{
    if(Size()==0) InitRecord(0); 
    return(GetVal(0,n_val,val));   
}

int TConfig::GetVal( string n_val, double & val)
{
    if(Size()==0) InitRecord(0); 
    return(GetVal(0,n_val,val));   
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

int TConfig::SetVal( string n_val, string val)
{
    if(Size()==0) InitRecord(0); 
    return(SetVal( 0, n_val, val));
}

int TConfig::SetVal( string n_val, double val)
{
    if(Size()==0) InitRecord(0); 
    return(SetVal( 0, n_val, val));
}

int TConfig::AddRecord( unsigned int id_rec)
{
    if(id_rec > Size() ) return(-1);
    if(id_rec == Size() ) value.push_back();
    else                  value.insert(value.begin()+id_rec);
    for(int i=0; i < elem->elem.size(); i++)
    {
	value[id_rec].push_back();
	if( elem->elem[i].type == CFGTP_STRING )
	{ 
	    value[id_rec][i].sval = new string;
	    *(value[id_rec][i].sval) = elem->elem[i].rstr->def;
	}
	if( elem->elem[i].type == CFGTP_NUMBER ) 
	{
	    value[id_rec][i].nval = new double;
	    *(value[id_rec][i].nval) = elem->elem[i].rnumb->def;
	}
	if( elem->elem[i].type == CFGTP_SELECT ) 
	{ 
	    value[id_rec][i].sval = new string;
	    *(value[id_rec][i].sval) = elem->elem[i].rsel->def;
	}
    }

    return(id_rec);   
}
    
int TConfig::InitRecord( unsigned int id_rec)
{
    if(id_rec < Size())
	for(int i=0; i < elem->elem.size(); i++)
	{
    	    if( elem->elem[i].type == CFGTP_STRING ) *(value[id_rec][i].sval) = elem->elem[i].rstr->def;
    	    if( elem->elem[i].type == CFGTP_NUMBER ) *(value[id_rec][i].nval) = elem->elem[i].rnumb->def;
	    if( elem->elem[i].type == CFGTP_SELECT ) *(value[id_rec][i].sval) = elem->elem[i].rsel->def;
	}
    else
	while(id_rec >= Size()) AddRecord( Size() );

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
    int b_hd;
    
    b_hd = App->BD->OpenBD(bd);
    if(b_hd < 0) return(-3);
    int kz = LoadRecValBD(id_rec, NameFld, b_hd);
    App->BD->CloseBD(b_hd);

    return(kz);
}

int TConfig::LoadRecValBD(unsigned int id_rec, string NameFld, int hd_bd)
{
    int line, i_elem, i_fld;
    string val;
    double valn;
    
    if(id_rec >= value.size()) return(-1);
    if( (i_fld = elem->NameToId(NameFld)) < 0 || elem->elem[i_fld].type != CFGTP_STRING ) return(-2);
    if(hd_bd < 0) return(-3);
    //Find line
    for(line=0; line < App->BD->NLines(hd_bd); line++)
    {
	if(App->BD->GetCellS(hd_bd,NameFld,line,val) != 0) 
	{
	    App->BD->CloseBD(hd_bd);
	    return(-3);
	}
	if(val == *(value[id_rec][i_fld].sval) ) break;
    }
    if(line == App->BD->NLines(hd_bd)) 
    {
	App->BD->CloseBD(hd_bd);
	return(-4);
    }
    //Load config from found line
    return(LoadRecValBD(id_rec, line, hd_bd));
}

int TConfig::LoadRecValBD(unsigned int id_rec, int line_bd, int hd_bd)
{
    int line, i_elem, i_fld;
    string val;
    double valn;
    
    if(id_rec >= value.size())           return(-1);
    if(line_bd >= App->BD->NLines(hd_bd)) return(-2);
    if(hd_bd < 0)                        return(-3);
    for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
	if(elem->elem[i_elem].type == CFGTP_NUMBER)
	{
	    if(App->BD->GetCellN(hd_bd,elem->elem[i_elem].name,line_bd,valn) != 0 ) continue;
	    *(value[id_rec][i_elem].nval) = valn;
	}
	else
	{
	    if(App->BD->GetCellS(hd_bd,elem->elem[i_elem].name,line_bd,val) != 0 ) continue;
	    *(value[id_rec][i_elem].sval) = val;
	}
    return(0);
}

int TConfig::LoadRecValBD( string NameFld, string bd)
{
    if(Size()==0) InitRecord(0); 
    return(LoadRecValBD(0, NameFld, bd));
}

int TConfig::LoadRecValBD( string NameFld, int hd_bd)
{
    if(Size()==0) InitRecord(0); 
    return(LoadRecValBD(0, NameFld, hd_bd));
}

int TConfig::LoadRecValBD( int line_bd, int hd_bd)
{
    if(Size()==0) InitRecord(0); 
    return(LoadRecValBD(0, line_bd, hd_bd));
}

int TConfig::SaveRecValBD(unsigned int id_rec, string NameFld, string bd)
{
    int b_hd;

    b_hd = App->BD->OpenBD(bd);
    if(b_hd < 0) return(-1);
    int kz = SaveRecValBD(id_rec, NameFld, b_hd);    
    App->BD->SaveBD(b_hd);
    App->BD->CloseBD(b_hd);

    return(kz);
}

int TConfig::SaveRecValBD(unsigned int id_rec, string NameFld, int hd_bd)
{
    int line, i_elem, i_fld, i_row;
    string val;
    double valn;

    if(id_rec >= value.size()) return(-1);
    if( (i_fld = elem->NameToId(NameFld)) < 0 || elem->elem[i_fld].type != CFGTP_STRING ) return(-2);
    if(hd_bd < 0) return(-3);
    //Find line
    for(line=0; line < App->BD->NLines(hd_bd); line++)
    {
	if(App->BD->GetCellS(hd_bd,NameFld,line,val) != 0) 
	{

	    line = App->BD->NLines(hd_bd);
	    break;
	}
	if(val == *(value[id_rec][i_fld].sval) ) break;
    }

    if(line == App->BD->NLines(hd_bd)) App->BD->AddLine(hd_bd,line);

    return(SaveRecValBD(id_rec, line, hd_bd));
}

int TConfig::SaveRecValBD(unsigned int id_rec, int line_bd, int hd_bd)
{
    int line, i_elem, i_fld, i_row;
    string val;
    double valn;

    if(id_rec >= value.size()) return(-1);
    if(hd_bd < 0) return(-2);
    if(line_bd > App->BD->NLines(hd_bd))  return(-3);
    if(line_bd == App->BD->NLines(hd_bd)) App->BD->AddLine(hd_bd,line_bd);

    for(i_elem=0; i_elem < elem->elem.size(); i_elem++)
    	if(elem->elem[i_elem].type == CFGTP_NUMBER)
	    App->BD->SetCellN(hd_bd,elem->elem[i_elem].name,line_bd,*(value[id_rec][i_elem].nval));	    
	else
	    App->BD->SetCellS(hd_bd,elem->elem[i_elem].name,line_bd,*(value[id_rec][i_elem].sval));	    
    return(0);
}

int TConfig::SaveRecValBD(string NameFld, string bd)
{
    if(Size()==0) InitRecord(0); 
    return(SaveRecValBD(0, NameFld, bd));
}

int TConfig::SaveRecValBD(string NameFld, int hd_bd)
{
    if(Size()==0) InitRecord(0); 
    return(SaveRecValBD(0, NameFld, hd_bd));
}

int TConfig::SaveRecValBD(int line_bd, int hd_bd)
{
    if(Size()==0) InitRecord(0); 
    return(SaveRecValBD(0, line_bd, hd_bd));
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
    		App->BD->SetCellN(b_hd,elem->elem[i_elem].name,i_ln,*(value[i_ln][i_elem].nval));	    
	    else
		App->BD->SetCellS(b_hd,elem->elem[i_elem].name,i_ln,*(value[i_ln][i_elem].sval));	    
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
		if(elem->elem[i_el].type == CFGTP_STRING || elem->elem[i_el].type == CFGTP_SELECT )
		    *value[i_rc][i_el].sval = *Cfg.value[i_rc][i_el].sval;
		if(elem->elem[i_el].type == CFGTP_NUMBER )
		    *value[i_rc][i_el].nval = *Cfg.value[i_rc][i_el].nval;
	    }
	}
    }
    return(*this);
}


