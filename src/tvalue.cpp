#include "tapplication.h"
#include "tmessage.h"
#include "tconfig.h"
#include "tparamcontr.h"
#include "tcontroller.h"
#include "tvalue.h"

const char *TValue::o_name = "TValue";

TValue::TValue( ) : elem(NULL)
{

}

TValue::~TValue()
{
    if(elem)
    {
	//Clean string elements
	for(unsigned i_elem = 0; i_elem < elem->elem.size(); i_elem++) FreeElem(i_elem);
	while(val.size()) val.erase(val.begin());
	while(buf.size()) buf.erase(buf.begin());
	//Clean link into Value without ValueElem
    	for(unsigned i_val = 0; i_val < elem->value.size() ;i_val++)
	    if(elem->value[i_val] == this)
	    {
		elem->value.erase(elem->value.begin()+i_val);
		break;
	    }
    }
}

void TValue::AddElem(int id_val)
{
    if(id_val == val.size()) val.push_back( ); 
    else val.insert(val.begin()+id_val);
    if(id_val == buf.size()) buf.push_back( );
    else buf.insert(buf.begin()+id_val);
    InitElem(id_val);
}

void TValue::InitElem(int id_val)
{
    val[id_val].valid = 0;
    val[id_val].time.s = 0;    
    if(elem->elem[id_val].type & VAL_T_STRING) val[id_val].val.val_s = new string("");
    //Check access    
    val[id_val].r_access = access(id_val,V_USER_OWN|V_USER_GRP|V_USER_OTH,V_ACC_READ);
    val[id_val].w_access = access(id_val,V_USER_OWN|V_USER_GRP|V_USER_OTH,V_ACC_WRITE);    
    //------------
    buf[id_val].sync = TO_FREE;
    buf[id_val].async = TO_FREE;
    if(elem->elem[id_val].type & VAL_T_REAL || elem->elem[id_val].type & VAL_T_INT)
    {
	val[id_val].scale = new SCale;
	if(elem->elem[id_val].vals.size() != 2)  val[id_val].scale->max = val[id_val].scale->min = 0.0; 
	else 
	{
	    if(elem->elem[id_val].vals[1][0] == '$') 
		val[id_val].scale->max = GetCfg()->Get_R(elem->elem[id_val].vals[1].substr(1,elem->elem[id_val].vals[1].size()-1));
	    else val[id_val].scale->max = atof(elem->elem[id_val].vals[1].c_str());
	    if(elem->elem[id_val].vals[0][0] == '$') 
		val[id_val].scale->min = GetCfg()->Get_R(elem->elem[id_val].vals[0].substr(1,elem->elem[id_val].vals[0].size()-1));
	    else val[id_val].scale->min = atof(elem->elem[id_val].vals[0].c_str());
	}
    }
    
    if(elem->elem[id_val].io & VAL_IO_B_SYNC && elem->elem[id_val].l_buf > 0 )
    {	
	buf[id_val].sync = new SBsync;
       	buf[id_val].sync->t_head.s  = buf[id_val].sync->t_per.s  = 0;
       	buf[id_val].sync->t_head.us = buf[id_val].sync->t_per.us = 0;
       	buf[id_val].sync->i_head    = buf[id_val].sync->i_tail   = 0;
   	if(elem->elem[id_val].type & VAL_T_STRING)
	{
	    buf[id_val].sync->val.val_s = new vector<string>;
	    while( buf[id_val].sync->val.val_s->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].sync->val.val_s->push_back();
	}
	else if(elem->elem[id_val].type & VAL_T_REAL)
	{
	    buf[id_val].sync->val.val_r = new vector<double>;
	    while( buf[id_val].sync->val.val_r->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].sync->val.val_r->push_back();
	}
	else if(elem->elem[id_val].type & VAL_T_INT)
	{
	    buf[id_val].sync->val.val_i = new vector<int>;
	    while( buf[id_val].sync->val.val_i->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].sync->val.val_i->push_back();
	}
	else if(elem->elem[id_val].type & VAL_T_BOOL)
	{
	    buf[id_val].sync->val.val_b = new vector<bool>;
	    while( buf[id_val].sync->val.val_b->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].sync->val.val_b->push_back(false);
	}
    } 
    else if(elem->elem[id_val].io & VAL_IO_B_ASYNC && elem->elem[id_val].l_buf > 0 )
    {
	buf[id_val].async         = new SBasync;
       	buf[id_val].async->i_head = 0;
	while( buf[id_val].async->time.size() < elem->elem[id_val].l_buf )
	{
    	    buf[id_val].async->time.push_back();  
	    buf[id_val].async->time[buf[id_val].async->time.size()-1].s = 0;
	}
   	if(elem->elem[id_val].type & VAL_T_STRING)
	{
	    buf[id_val].async->val.val_s = new vector<string>;
	    while( buf[id_val].async->val.val_s->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].async->val.val_s->push_back();
	}
	else if(elem->elem[id_val].type & VAL_T_REAL)
	{
	    buf[id_val].async->val.val_r = new vector<double>;
	    while( buf[id_val].async->val.val_r->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].async->val.val_r->push_back();
	}
	else if(elem->elem[id_val].type & VAL_T_INT)
	{
	    buf[id_val].async->val.val_i = new vector<int>;
	    while( buf[id_val].async->val.val_i->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].async->val.val_i->push_back();
	}
	else if(elem->elem[id_val].type & VAL_T_BOOL)
	{
	    buf[id_val].async->val.val_b = new vector<bool>;
	    while( buf[id_val].async->val.val_b->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].async->val.val_b->push_back(false);
	}
    } 	
}

bool TValue::access(unsigned element, char user, char mode)
{
    int accs = 0777, accs_c, i_and, i_or;
    bool p_r = false, p_w = false;
    
    if( element >= elem->Size() ) throw TError("%s: element id error!",o_name);
    string str_ac = elem->elem[element].access;
    
    for(int of = 0; of < str_ac.size(); of++)
    {
    	if(str_ac[of] == ' ') continue;
    	if(str_ac[of] == '$')
	{
	    of++;
	    i_and = str_ac.find("&",of,str_ac.size()-of);
	    i_or  = str_ac.find("|",of,str_ac.size()-of);
	    if( i_and < i_or ) { accs_c = GetCfg()->Get_I(str_ac.substr(of,i_and-of)); of = i_and; }
	    else    	       { accs_c = GetCfg()->Get_I(str_ac.substr(of,i_or-of));  of = i_or; }
	}
	if(str_ac[of] == '|') { accs|=accs_c; continue; }
	if(str_ac[of] == '&') { accs&=accs_c; continue; }
	// Direct number
	i_and = str_ac.find("&",of,str_ac.size()-of);
	i_or  = str_ac.find("|",of,str_ac.size()-of);
	if( i_and < i_or ) { accs_c = strtol(str_ac.substr(of,i_and-of).c_str(),NULL,8); of = i_and; }
	else               { accs_c = strtol(str_ac.substr(of,i_or-of).c_str(),NULL,8);  of = i_or; }
	of--;
    }
    
    if( mode&V_ACC_READ )
    {
	if(!p_r && user&V_USER_OWN && accs&0400 ) p_r = true;
	if(!p_r && user&V_USER_GRP && accs&0040 ) p_r = true;
	if(!p_r && user&V_USER_OTH && accs&0004 ) p_r = true;
    }
    if( mode&V_ACC_WRITE )
    {
	if(!p_w && user&V_USER_OWN && accs&0200 ) p_w = true;
	if(!p_w && user&V_USER_GRP && accs&0020 ) p_w = true;
	if(!p_w && user&V_USER_OTH && accs&0002 ) p_w = true;
    }
    if( (mode&V_ACC_READ && !(mode&V_ACC_WRITE) && p_r) || 
	(mode&V_ACC_WRITE && !(mode&V_ACC_READ) && p_w) ||
	(mode&V_ACC_WRITE && mode&V_ACC_READ && p_w && p_r) ) return(true);
    return(false);
}

void TValue::DelElem(int id_val)
{    
    FreeElem(id_val);
    val.erase(val.begin()+id_val);
    buf.erase(buf.begin()+id_val);
}

void TValue::FreeElem(int id_val)
{
    if(elem->elem[id_val].type & VAL_T_STRING) delete val[id_val].val.val_s;
    if(elem->elem[id_val].io & VAL_IO_B_SYNC && elem->elem[id_val].l_buf > 0 )
    {
   	if(elem->elem[id_val].type & VAL_T_STRING)    delete buf[id_val].sync->val.val_s;
	else if(elem->elem[id_val].type & VAL_T_REAL) delete buf[id_val].sync->val.val_r;
	else if(elem->elem[id_val].type & VAL_T_INT)  delete buf[id_val].sync->val.val_i;
	else if(elem->elem[id_val].type & VAL_T_BOOL) delete buf[id_val].sync->val.val_b;
	delete buf[id_val].sync;
    }
    else if(elem->elem[id_val].io & VAL_IO_B_ASYNC && elem->elem[id_val].l_buf > 0 )
    {
   	if(elem->elem[id_val].type & VAL_T_STRING)    delete buf[id_val].async->val.val_s;
	else if(elem->elem[id_val].type & VAL_T_REAL) delete buf[id_val].async->val.val_r;
	else if(elem->elem[id_val].type & VAL_T_INT)  delete buf[id_val].async->val.val_i;
	else if(elem->elem[id_val].type & VAL_T_BOOL) delete buf[id_val].async->val.val_b;
	delete buf[id_val].async;
    }
}

bool TValue::Valid( unsigned element )
{
    if( element >= elem->Size() ) throw TError("%s: element id error!",o_name);
    return((bool)val[element].valid);
}

void TValue::Valid( unsigned element, bool value )
{
    if( element >= elem->Size() ) throw TError("%s: element id error!",o_name);
    val[element].valid = value;
}

void TValue::SetValType( TValueElem *ValEl )
{
    if(elem == ValEl) return;
    if(elem != NULL)
    {
	for(unsigned i_elem = 0; i_elem < elem->elem.size(); i_elem++) FreeElem(i_elem);
	while(val.size()) val.erase(val.begin());
	while(buf.size()) buf.erase(buf.begin());
	//Clean link into Value without ValueElem	
    	for(unsigned i_val = 0; i_val < elem->value.size() ;i_val++)
	    if(elem->value[i_val] == this) { elem->value.erase(elem->value.begin()+i_val); break; }
	elem=NULL;
    }
    elem = ValEl;
    elem->value.push_back(this);
    for(unsigned i_elem = 0; i_elem < elem->elem.size(); i_elem++) AddElem(i_elem);
}
    
string TValue::GetSEL( unsigned id_el, STime &time, int arhiv )
{
    unsigned i;
  
    CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_SELECT)) throw TError("%s: element no select type!",o_name);
    if(elem->elem[id_el].type&VAL_T_STRING)
    {
	string val = GetS( id_el, time, arhiv );
	for(i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(elem->elem[id_el].vals[i]==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_REAL)
    {
	double val = GetR( id_el, time, arhiv );
	for(i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(atof(elem->elem[id_el].vals[i].c_str())==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_INT)
    {
	int val = GetI( id_el, time, arhiv );
	for( i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(atoi(elem->elem[id_el].vals[i].c_str())==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_BOOL)
    {
	bool val = GetB( id_el, time, arhiv );
	for( i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if( (elem->elem[id_el].vals[i] == "true" && val==true) || (elem->elem[id_el].vals[i] == "false" && val==false)) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
}

string TValue::GetS( unsigned  id_el, STime &time, int arhiv )
{
    CheckId(id_el);  
    if(!val[id_el].r_access)                   throw TError("%s: element no reading!",o_name);
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
    	CheckValid(id_el);	
	if(elem->elem[id_el].io&VAL_IO_R_DIR) GetVal(id_el);
    }
    return(_GetS(id_el,time,arhiv));
}

double TValue::GetR( unsigned  id_el, STime &time, int arhiv )
{
    CheckId(id_el);  
    if(!val[id_el].r_access)                 throw TError("%s: element no reading!",o_name);
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
    	CheckValid(id_el);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) GetVal(id_el);
    }
    return(_GetR(id_el,time,arhiv));
}

int TValue::GetI( unsigned  id_el, STime &time, int arhiv )
{
    CheckId(id_el);  
    if(!val[id_el].r_access)                throw TError("%s: element no reading!",o_name);
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
    	CheckValid(id_el);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) GetVal(id_el);	
    }
    return(_GetI(id_el,time,arhiv));
}

bool TValue::GetB( unsigned  id_el, STime &time, int arhiv )
{
    CheckId(id_el);  
    if(!val[id_el].r_access)                 throw TError("%s: element no reading!",o_name);
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
    	CheckValid(id_el);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) GetVal(id_el);
    }
    return(_GetB(id_el,time,arhiv));
}


string TValue::SetSEL( unsigned  id_el, string value, const STime &tm, int arhiv )
{
    unsigned i;
  
    CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_SELECT)) throw TError("%s: element no select type!",o_name);
    if(elem->elem[id_el].type&VAL_T_STRING)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	SetS(id_el,elem->elem[id_el].vals[i],tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_REAL)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	SetR(id_el,atof(elem->elem[id_el].vals[i].c_str()),tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_INT)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	SetI(id_el,atoi(elem->elem[id_el].vals[i].c_str()),tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_BOOL)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	SetB(id_el,(elem->elem[id_el].vals[i]=="true")?true:false,tm,arhiv);
	return(value);
    }
}

string TValue::SetS( unsigned  id_el, string value, const STime &tm, int arhiv )
{
    CheckId(id_el);  
    if(!val[id_el].w_access)                   throw TError("%s: element no writing!",o_name);
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
    	CheckValid(id_el);
	string val = _SetS(id_el,value,tm,arhiv);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) SetVal(id_el);
	return(val);
    }
    return(_SetS(id_el,value,tm,arhiv));
}

double TValue::SetR( unsigned  id_el, double value, const STime &tm, int arhiv )
{
    CheckId(id_el);  
    if(!val[id_el].w_access)                 throw TError("%s: element no writing!",o_name);
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
    	CheckValid(id_el);
	float val = _SetR(id_el,value,tm,arhiv);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) SetVal(id_el);
	return(val);
    }
    return(_SetR(id_el,value,tm,arhiv));
}

int TValue::SetI( unsigned  id_el, int value, const STime &tm, int arhiv )
{
    CheckId(id_el);  
    if(!val[id_el].w_access)                throw TError("%s: element no writing!",o_name);
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
    	CheckValid(id_el);
	int val = _SetI(id_el,value,tm,arhiv);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) SetVal(id_el);
	return(val);	
    }
    return(_SetI(id_el,value,tm,arhiv));
}

bool TValue::SetB( unsigned  id_el, bool value, const STime &tm, int arhiv )
{
    CheckId(id_el);  
    if(!val[id_el].w_access)                 throw TError("%s: element no writing!",o_name);
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
    	CheckValid(id_el);
	bool val = _SetB(id_el,value,tm,arhiv);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) SetVal(id_el);
	return(val);	
    }
    return(_SetB(id_el,value,tm,arhiv));
}

/*
 * Read curent value for owner
 */
string TValue::_GetSEL( unsigned id_el, STime &time, int arhiv )
{
    unsigned i;
  
    CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_SELECT)) throw TError("%s: element no select type!",o_name);
    if(elem->elem[id_el].type&VAL_T_STRING)
    {
	string val = _GetS( id_el, time, arhiv );
	for(i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(elem->elem[id_el].vals[i]==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_REAL)
    {
	double val = _GetR( id_el, time, arhiv );
	for(i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(atof(elem->elem[id_el].vals[i].c_str())==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_INT)
    {
	int val = _GetI( id_el, time, arhiv );
	for( i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(atoi(elem->elem[id_el].vals[i].c_str())==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_BOOL)
    {
	bool val = _GetB( id_el, time, arhiv );
	for( i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if( (elem->elem[id_el].vals[i] == "true" && val==true) || (elem->elem[id_el].vals[i] == "false" && val==false)) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
}

string TValue::_GetS( unsigned  id_el, STime &time, int arhiv )
{
    CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_STRING)) throw TError("%s: element no string type!",o_name);    
    //Chek get of curent value
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
        time = val[id_el].time;
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    *val[id_el].val.val_s = GetCfg()->Get_S(elem->elem[id_el].vals[0]); 
	return(*val[id_el].val.val_s);
    }
    //value geting from arhiv  //?!?!
}

double TValue::_GetR( unsigned  id_el, STime &time, int arhiv )
{
    CheckId(id_el);  
    if(elem->elem[id_el].type&VAL_T_STRING) throw TError("%s: element no real type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_INT)  return((double)_GetI(id_el,time,arhiv));
    else if(elem->elem[id_el].type&VAL_T_BOOL) return((double)_GetB(id_el,time,arhiv));
    if( arhiv == V_MAX ) return(val[id_el].scale->max);
    if( arhiv == V_MIN ) return(val[id_el].scale->min);
    
    //Chek get of curent value
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
        time = val[id_el].time;
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    val[id_el].val.val_r = GetCfg()->Get_R(elem->elem[id_el].vals[0]); 	
	return(val[id_el].val.val_r);
    }	
    //value geting from arhiv  //?!?!
}

int TValue::_GetI( unsigned  id_el, STime &time, int arhiv )
{
    CheckId(id_el);  
    if(elem->elem[id_el].type&VAL_T_STRING) throw TError("%s: element no int type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_REAL) return((int)_GetR(id_el,time,arhiv));
    else if(elem->elem[id_el].type&VAL_T_BOOL) return((int)_GetB(id_el,time,arhiv));
    if( arhiv == V_MAX ) return((int)val[id_el].scale->max);
    if( arhiv == V_MIN ) return((int)val[id_el].scale->min);
    //Chek get of curent value
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
        time = val[id_el].time;
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    val[id_el].val.val_i = GetCfg()->Get_I(elem->elem[id_el].vals[0]); 	
	return(val[id_el].val.val_i);
    }	
    //value geting from arhiv  //?!?!
}

bool TValue::_GetB( unsigned  id_el, STime &time, int arhiv )
{
    CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_BOOL)) throw TError("%s: element no bool type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_REAL) return((bool)_GetR(id_el,time,arhiv));
    else if(elem->elem[id_el].type&VAL_T_INT)  return((bool)_GetI(id_el,time,arhiv));
    //Chek get of curent value
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
        time = val[id_el].time;
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    val[id_el].val.val_b = GetCfg()->Get_B(elem->elem[id_el].vals[0]); 	
	return(val[id_el].val.val_b);
    }	
    //value geting from arhiv  //?!?!
}



/*
 * Set new value(s) for owner
 */
string TValue::_SetSEL( unsigned  id_el, string value, const STime &tm, int arhiv )
{
    unsigned i;
  
    CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_SELECT)) throw TError("%s: element no select type!",o_name);
    if(elem->elem[id_el].type&VAL_T_STRING)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	_SetS(id_el,elem->elem[id_el].vals[i],tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_REAL)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	_SetR(id_el,atof(elem->elem[id_el].vals[i].c_str()),tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_INT)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	_SetI(id_el,atoi(elem->elem[id_el].vals[i].c_str()),tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_BOOL)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	_SetB(id_el,(elem->elem[id_el].vals[i]=="true")?true:false,tm,arhiv);
	return(value);
    }
}

string TValue::_SetS( unsigned  id_el, string value, const STime &tm, int arhiv )
{
    CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_STRING)) throw TError("%s: element no string type!",o_name);
    //Chek for get curent value
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
	*val[id_el].val.val_s = value;
        if(tm.s) val[id_el].time = tm;
	else { val[id_el].time.s = time(NULL); val[id_el].time.us = 0; }
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    GetCfg()->Set_S(elem->elem[id_el].vals[0],*val[id_el].val.val_s);       
    }
    //value geting from arhiv
    return(*val[id_el].val.val_s);   //?!?!
}

double TValue::_SetR( unsigned  id_el, double value, const STime &tm, int arhiv )
{
    CheckId(id_el);  
    if(elem->elem[id_el].type&VAL_T_STRING) throw TError("%s: element no real type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_INT)  return( (float)_SetI(id_el,(int)value, tm, arhiv ) );
    else if(elem->elem[id_el].type&VAL_T_BOOL) return( (float)_SetB(id_el,(bool)value, tm, arhiv ) );
    //Want check range          
    if(val[id_el].scale->max > val[id_el].scale->min)
	value = (value > val[id_el].scale->max)?val[id_el].scale->max:
	        (value < val[id_el].scale->min)?val[id_el].scale->min:value;
    //Chek for get curent value
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
	val[id_el].val.val_r = value;
        if(tm.s) val[id_el].time = tm;
	else { val[id_el].time.s = time(NULL); val[id_el].time.us = 0; }
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    GetCfg()->Set_R(elem->elem[id_el].vals[0],val[id_el].val.val_r);       
    }
    //value geting from arhiv
    return(val[id_el].val.val_r);   //?!?!
}

int TValue::_SetI( unsigned  id_el, int value, const STime &tm, int arhiv )
{
    CheckId(id_el);  
    if(elem->elem[id_el].type&VAL_T_STRING)    throw TError("%s: element no int type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_REAL) return((int)_SetR(id_el,(double)value,tm,arhiv) );
    else if(elem->elem[id_el].type&VAL_T_BOOL) return((int)_SetB(id_el,(bool)value,tm,arhiv) );
    //Want check range          
    if(val[id_el].scale->max > val[id_el].scale->min)
	value = (value > (int)val[id_el].scale->max)?(int)val[id_el].scale->max:
	        (value < (int)val[id_el].scale->min)?(int)val[id_el].scale->min:value;
    //Chek for get curent value
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
	val[id_el].val.val_i = value;
        if(tm.s) val[id_el].time = tm;
	else { val[id_el].time.s = time(NULL); val[id_el].time.us = 0; }
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    GetCfg()->Set_I(elem->elem[id_el].vals[0],val[id_el].val.val_i);       
    }
    //value geting from arhiv
    return(val[id_el].val.val_i);   //?!?!
}

bool TValue::_SetB( unsigned  id_el, bool value, const STime &tm, int arhiv )
{
    CheckId(id_el);  
    if(elem->elem[id_el].type&VAL_T_STRING) throw TError("%s: element no bool type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_REAL) return((bool)_SetR(id_el,(double)value,tm,arhiv));
    else if(elem->elem[id_el].type&VAL_T_INT)  return((bool)_SetI(id_el,(int)value,tm,arhiv));
    //Chek for get curent value
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
	val[id_el].val.val_b = value;
        if(tm.s) val[id_el].time = tm;
	else { val[id_el].time.s = time(NULL); val[id_el].time.us = 0; }
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    GetCfg()->Set_B(elem->elem[id_el].vals[0],val[id_el].val.val_b);       
    }
    //value geting from arhiv
    return(val[id_el].val.val_b);   //?!?!
}


