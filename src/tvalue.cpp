#include "tsys.h"
#include "tkernel.h"
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
	for(unsigned i_elem = 0; i_elem < elem->elem.size(); i_elem++) vl_FreeElem(i_elem);
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

void TValue::vl_AddElem(unsigned id_val)
{
    val.insert( val.begin()+id_val, SVal() );
    buf.insert( buf.begin()+id_val, SBUF() );
    vl_InitElem(id_val);
}

void TValue::vl_InitElem(unsigned id_val)
{
    val[id_val].valid = 0;
    val[id_val].time.s = 0;    
    if(elem->elem[id_val].type & VAL_T_STRING) val[id_val].val.val_s = new string("");
    //Check access    
    val[id_val].r_access = vl_access(id_val,V_USER_OWN|V_USER_GRP|V_USER_OTH,V_ACC_READ);
    val[id_val].w_access = vl_access(id_val,V_USER_OWN|V_USER_GRP|V_USER_OTH,V_ACC_WRITE);    
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
		val[id_val].scale->max = vl_GetCfg()->cf_Get_R(elem->elem[id_val].vals[1].substr(1,elem->elem[id_val].vals[1].size()-1));
	    else val[id_val].scale->max = atof(elem->elem[id_val].vals[1].c_str());
	    if(elem->elem[id_val].vals[0][0] == '$') 
		val[id_val].scale->min = vl_GetCfg()->cf_Get_R(elem->elem[id_val].vals[0].substr(1,elem->elem[id_val].vals[0].size()-1));
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
	       	buf[id_val].sync->val.val_s->push_back("");
	}
	else if(elem->elem[id_val].type & VAL_T_REAL)
	{
	    buf[id_val].sync->val.val_r = new vector<double>;
	    while( buf[id_val].sync->val.val_r->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].sync->val.val_r->push_back(0.0);
	}
	else if(elem->elem[id_val].type & VAL_T_INT)
	{
	    buf[id_val].sync->val.val_i = new vector<int>;
	    while( buf[id_val].sync->val.val_i->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].sync->val.val_i->push_back(0);
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
    	    buf[id_val].async->time.push_back( STime() );  
	    buf[id_val].async->time[buf[id_val].async->time.size()-1].s = 0;
	}
   	if(elem->elem[id_val].type & VAL_T_STRING)
	{
	    buf[id_val].async->val.val_s = new vector<string>;
	    while( buf[id_val].async->val.val_s->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].async->val.val_s->push_back("");
	}
	else if(elem->elem[id_val].type & VAL_T_REAL)
	{
	    buf[id_val].async->val.val_r = new vector<double>;
	    while( buf[id_val].async->val.val_r->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].async->val.val_r->push_back(0.0);
	}
	else if(elem->elem[id_val].type & VAL_T_INT)
	{
	    buf[id_val].async->val.val_i = new vector<int>;
	    while( buf[id_val].async->val.val_i->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].async->val.val_i->push_back(0);
	}
	else if(elem->elem[id_val].type & VAL_T_BOOL)
	{
	    buf[id_val].async->val.val_b = new vector<bool>;
	    while( buf[id_val].async->val.val_b->size() < elem->elem[id_val].l_buf)
	       	buf[id_val].async->val.val_b->push_back(false);
	}
    } 	
}

bool TValue::vl_access(unsigned element, char user, char mode)
{
    int accs = 0777, accs_1 = 0000, i_and, i_or;
    bool p_r = false, p_w = false;
    
    if( element >= elem->vle_Size() ) throw TError("%s: element id error!",o_name);
    string str_ac = elem->elem[element].access;
    
    for(unsigned of = 0; of < str_ac.size(); of++)
    {
    	if(str_ac[of] == ' ') continue;
    	if(str_ac[of] == '$')
	{
	    of++;
	    i_and = str_ac.find("&",of,str_ac.size()-of);
	    i_or  = str_ac.find("|",of,str_ac.size()-of);
	    if( i_and < i_or ) { accs_1 = vl_GetCfg()->cf_Get_I(str_ac.substr(of,i_and-of)); of = i_and; }
	    else    	       { accs_1 = vl_GetCfg()->cf_Get_I(str_ac.substr(of,i_or-of));  of = i_or; }
	}
	if(str_ac[of] == '|') { accs|=accs_1; continue; }
	if(str_ac[of] == '&') { accs&=accs_1; continue; }
	// Direct number
	i_and = str_ac.find("&",of,str_ac.size()-of);
	i_or  = str_ac.find("|",of,str_ac.size()-of);
	if( i_and < i_or ) { accs_1 = strtol(str_ac.substr(of,i_and-of).c_str(),NULL,8); of = i_and; }
	else               { accs_1 = strtol(str_ac.substr(of,i_or-of).c_str(),NULL,8);  of = i_or; }
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

void TValue::vl_DelElem(int id_val)
{    
    vl_FreeElem(id_val);
    val.erase(val.begin()+id_val);
    buf.erase(buf.begin()+id_val);
}

void TValue::vl_FreeElem(int id_val)
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

bool TValue::vl_Valid( unsigned element )
{
    if( element >= elem->vle_Size() ) throw TError("%s: element id error!",o_name);
    return((bool)val[element].valid);
}

void TValue::vl_Valid( unsigned element, bool value )
{
    if( element >= elem->vle_Size() ) throw TError("%s: element id error!",o_name);
    val[element].valid = value;
}

void TValue::vl_SetType( TValueElem *ValEl )
{
    if(elem == ValEl) return;
    if(elem != NULL)
    {
	for(unsigned i_elem = 0; i_elem < elem->elem.size(); i_elem++) vl_FreeElem(i_elem);
	while(val.size()) val.erase(val.begin());
	while(buf.size()) buf.erase(buf.begin());
	//Clean link into Value without ValueElem	
    	for(unsigned i_val = 0; i_val < elem->value.size() ;i_val++)
	    if(elem->value[i_val] == this) { elem->value.erase(elem->value.begin()+i_val); break; }
	elem=NULL;
    }
    elem = ValEl;
    elem->value.push_back(this);
    for(unsigned i_elem = 0; i_elem < elem->elem.size(); i_elem++) vl_AddElem(i_elem);
}
    
string TValue::vl_GetSEL( unsigned id_el, STime &time, int arhiv )
{
    unsigned i;
  
    vl_CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_SELECT)) throw TError("%s: element no select type!",o_name);
    if(elem->elem[id_el].type&VAL_T_STRING)
    {
	string val = vl_GetS( id_el, time, arhiv );
	for(i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(elem->elem[id_el].vals[i]==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_REAL)
    {
	double val = vl_GetR( id_el, time, arhiv );
	for(i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(atof(elem->elem[id_el].vals[i].c_str())==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_INT)
    {
	int val = vl_GetI( id_el, time, arhiv );
	for( i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(atoi(elem->elem[id_el].vals[i].c_str())==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_BOOL)
    {
	bool val = vl_GetB( id_el, time, arhiv );
	for( i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if( (elem->elem[id_el].vals[i] == "true" && val==true) || (elem->elem[id_el].vals[i] == "false" && val==false)) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    return("");
}

string TValue::vl_GetS( unsigned  id_el, STime &time, int arhiv )
{
    vl_CheckId(id_el);  
    if(!val[id_el].r_access)                   throw TError("%s: element no reading!",o_name);
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
    	vl_CheckValid(id_el);	
	if(elem->elem[id_el].io&VAL_IO_R_DIR) vl_Get(id_el);
    }
    return(_vl_GetS(id_el,time,arhiv));
}

double TValue::vl_GetR( unsigned  id_el, STime &time, int arhiv )
{
    vl_CheckId(id_el);  
    if(!val[id_el].r_access)                 throw TError("%s: element no reading!",o_name);
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
    	vl_CheckValid(id_el);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) vl_Get(id_el);
    }
    return(_vl_GetR(id_el,time,arhiv));
}

int TValue::vl_GetI( unsigned  id_el, STime &time, int arhiv )
{
    vl_CheckId(id_el);  
    if(!val[id_el].r_access)                throw TError("%s: element no reading!",o_name);
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
    	vl_CheckValid(id_el);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) vl_Get(id_el);	
    }
    return(_vl_GetI(id_el,time,arhiv));
}

bool TValue::vl_GetB( unsigned  id_el, STime &time, int arhiv )
{
    vl_CheckId(id_el);  
    if(!val[id_el].r_access)                 throw TError("%s: element no reading!",o_name);
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
    	vl_CheckValid(id_el);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) vl_Get(id_el);
    }
    return(_vl_GetB(id_el,time,arhiv));
}


string TValue::vl_SetSEL( unsigned  id_el, string value, const STime &tm, int arhiv )
{
    unsigned i;
  
    vl_CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_SELECT)) throw TError("%s: element no select type!",o_name);
    if(elem->elem[id_el].type&VAL_T_STRING)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	vl_SetS(id_el,elem->elem[id_el].vals[i],tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_REAL)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	vl_SetR(id_el,atof(elem->elem[id_el].vals[i].c_str()),tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_INT)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	vl_SetI(id_el,atoi(elem->elem[id_el].vals[i].c_str()),tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_BOOL)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	vl_SetB(id_el,(elem->elem[id_el].vals[i]=="true")?true:false,tm,arhiv);
	return(value);
    }
    return("");
}

string TValue::vl_SetS( unsigned  id_el, string value, const STime &tm, int arhiv )
{
    vl_CheckId(id_el);  
    if(!val[id_el].w_access)                   throw TError("%s: element no writing!",o_name);
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
    	vl_CheckValid(id_el);
	string val = _vl_SetS(id_el,value,tm,arhiv);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) vl_Set(id_el);
	return(val);
    }
    return(_vl_SetS(id_el,value,tm,arhiv));
}

double TValue::vl_SetR( unsigned  id_el, double value, const STime &tm, int arhiv )
{
    vl_CheckId(id_el);  
    if(!val[id_el].w_access)                 throw TError("%s: element no writing!",o_name);
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
    	vl_CheckValid(id_el);
	float val = _vl_SetR(id_el,value,tm,arhiv);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) vl_Set(id_el);
	return(val);
    }
    return(_vl_SetR(id_el,value,tm,arhiv));
}

int TValue::vl_SetI( unsigned  id_el, int value, const STime &tm, int arhiv )
{
    vl_CheckId(id_el);  
    if(!val[id_el].w_access)                throw TError("%s: element no writing!",o_name);
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
    	vl_CheckValid(id_el);
	int val = _vl_SetI(id_el,value,tm,arhiv);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) vl_Set(id_el);
	return(val);	
    }
    return(_vl_SetI(id_el,value,tm,arhiv));
}

bool TValue::vl_SetB( unsigned  id_el, bool value, const STime &tm, int arhiv )
{
    vl_CheckId(id_el);  
    if(!val[id_el].w_access)                 throw TError("%s: element no writing!",o_name);
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
    	vl_CheckValid(id_el);
	bool val = _vl_SetB(id_el,value,tm,arhiv);
	if(elem->elem[id_el].io&VAL_IO_R_DIR) vl_Set(id_el);
	return(val);	
    }
    return(_vl_SetB(id_el,value,tm,arhiv));
}

/*
 * Read curent value for owner
 */
string TValue::_vl_GetSEL( unsigned id_el, STime &time, int arhiv )
{
    unsigned i;
  
    vl_CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_SELECT)) throw TError("%s: element no select type!",o_name);
    if(elem->elem[id_el].type&VAL_T_STRING)
    {
	string val = _vl_GetS( id_el, time, arhiv );
	for(i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(elem->elem[id_el].vals[i]==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_REAL)
    {
	double val = _vl_GetR( id_el, time, arhiv );
	for(i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(atof(elem->elem[id_el].vals[i].c_str())==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_INT)
    {
	int val = _vl_GetI( id_el, time, arhiv );
	for( i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if(atoi(elem->elem[id_el].vals[i].c_str())==val) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    else if(elem->elem[id_el].type&VAL_T_BOOL)
    {
	bool val = _vl_GetB( id_el, time, arhiv );
	for( i = 0; i < elem->elem[id_el].vals.size(); i++)
	    if( (elem->elem[id_el].vals[i] == "true" && val==true) || (elem->elem[id_el].vals[i] == "false" && val==false)) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	return(elem->elem[id_el].n_sel[i]);
    }
    return("");
}

string TValue::_vl_GetS( unsigned  id_el, STime &time, int arhiv )
{
    vl_CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_STRING)) throw TError("%s: element no string type!",o_name);    
    //Chek get of curent value
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
        time = val[id_el].time;
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    *val[id_el].val.val_s = vl_GetCfg()->cf_Get_S(elem->elem[id_el].vals[0]); 
	return(*val[id_el].val.val_s);
    }
    //value geting from arhiv  //?!?!
    return("");
}

double TValue::_vl_GetR( unsigned  id_el, STime &time, int arhiv )
{
    vl_CheckId(id_el);  
    if(elem->elem[id_el].type&VAL_T_STRING) throw TError("%s: element no real type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_INT)  return((double)_vl_GetI(id_el,time,arhiv));
    else if(elem->elem[id_el].type&VAL_T_BOOL) return((double)_vl_GetB(id_el,time,arhiv));
    if( arhiv == V_MAX ) return(val[id_el].scale->max);
    if( arhiv == V_MIN ) return(val[id_el].scale->min);
    
    //Chek get of curent value
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
        time = val[id_el].time;
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    val[id_el].val.val_r = vl_GetCfg()->cf_Get_R(elem->elem[id_el].vals[0]); 	
	return(val[id_el].val.val_r);
    }	
    //value geting from arhiv  //?!?!
    return(0.0);
}

int TValue::_vl_GetI( unsigned  id_el, STime &time, int arhiv )
{
    vl_CheckId(id_el);  
    if(elem->elem[id_el].type&VAL_T_STRING) throw TError("%s: element no int type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_REAL) return((int)_vl_GetR(id_el,time,arhiv));
    else if(elem->elem[id_el].type&VAL_T_BOOL) return((int)_vl_GetB(id_el,time,arhiv));
    if( arhiv == V_MAX ) return((int)val[id_el].scale->max);
    if( arhiv == V_MIN ) return((int)val[id_el].scale->min);
    //Chek get of curent value
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
        time = val[id_el].time;
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    val[id_el].val.val_i = vl_GetCfg()->cf_Get_I(elem->elem[id_el].vals[0]); 	
	return(val[id_el].val.val_i);
    }	
    //value geting from arhiv  //?!?!
    return(0);
}

bool TValue::_vl_GetB( unsigned  id_el, STime &time, int arhiv )
{
    vl_CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_BOOL)) throw TError("%s: element no bool type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_REAL) return((bool)_vl_GetR(id_el,time,arhiv));
    else if(elem->elem[id_el].type&VAL_T_INT)  return((bool)_vl_GetI(id_el,time,arhiv));
    //Chek get of curent value
    if(!time.s || time.s > val[id_el].time.s || (time.s == val[id_el].time.s && time.us > val[id_el].time.us) )
    {
        time = val[id_el].time;
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    val[id_el].val.val_b = vl_GetCfg()->cf_Get_B(elem->elem[id_el].vals[0]); 	
	return(val[id_el].val.val_b);
    }	
    //value geting from arhiv  //?!?!
    return(false);
}



/*
 * Set new value(s) for owner
 */
string TValue::_vl_SetSEL( unsigned  id_el, string value, const STime &tm, int arhiv )
{
    unsigned i;
  
    vl_CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_SELECT)) throw TError("%s: element no select type!",o_name);
    if(elem->elem[id_el].type&VAL_T_STRING)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	_vl_SetS(id_el,elem->elem[id_el].vals[i],tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_REAL)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	_vl_SetR(id_el,atof(elem->elem[id_el].vals[i].c_str()),tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_INT)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	_vl_SetI(id_el,atoi(elem->elem[id_el].vals[i].c_str()),tm,arhiv);
	return(value);
    }
    else if(elem->elem[id_el].type&VAL_T_BOOL)
    {
	for(i = 0; i < elem->elem[id_el].n_sel.size(); i++)
	    if(elem->elem[id_el].n_sel[i]==value) break;
       	if(i == elem->elem[id_el].vals.size()) throw TError("%s: select type error!",o_name);
	_vl_SetB(id_el,(elem->elem[id_el].vals[i]=="true")?true:false,tm,arhiv);
	return(value);
    }
    return("");
}

string TValue::_vl_SetS( unsigned  id_el, string value, const STime &tm, int arhiv )
{
    vl_CheckId(id_el);  
    if(!(elem->elem[id_el].type&VAL_T_STRING)) throw TError("%s: element no string type!",o_name);
    //Chek for get curent value
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
	*val[id_el].val.val_s = value;
        if(tm.s) val[id_el].time = tm;
	else { val[id_el].time.s = time(NULL); val[id_el].time.us = 0; }
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    vl_GetCfg()->cf_Set_S(elem->elem[id_el].vals[0],*val[id_el].val.val_s);       
    }
    //value geting from arhiv
    return(*val[id_el].val.val_s);   //?!?!
}

double TValue::_vl_SetR( unsigned  id_el, double value, const STime &tm, int arhiv )
{
    vl_CheckId(id_el);  
    if(elem->elem[id_el].type&VAL_T_STRING) throw TError("%s: element no real type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_INT)  return( (float)_vl_SetI(id_el,(int)value, tm, arhiv ) );
    else if(elem->elem[id_el].type&VAL_T_BOOL) return( (float)_vl_SetB(id_el,(bool)value, tm, arhiv ) );
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
	    vl_GetCfg()->cf_Set_R(elem->elem[id_el].vals[0],val[id_el].val.val_r);       
    }
    //value geting from arhiv
    return(val[id_el].val.val_r);   //?!?!
}

int TValue::_vl_SetI( unsigned  id_el, int value, const STime &tm, int arhiv )
{
    vl_CheckId(id_el);  
    if(elem->elem[id_el].type&VAL_T_STRING)    throw TError("%s: element no int type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_REAL) return((int)_vl_SetR(id_el,(double)value,tm,arhiv) );
    else if(elem->elem[id_el].type&VAL_T_BOOL) return((int)_vl_SetB(id_el,(bool)value,tm,arhiv) );
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
	    vl_GetCfg()->cf_Set_I(elem->elem[id_el].vals[0],val[id_el].val.val_i);       
    }
    //value geting from arhiv
    return(val[id_el].val.val_i);   //?!?!
}

bool TValue::_vl_SetB( unsigned  id_el, bool value, const STime &tm, int arhiv )
{
    vl_CheckId(id_el);  
    if(elem->elem[id_el].type&VAL_T_STRING) throw TError("%s: element no bool type!",o_name);
    else if(elem->elem[id_el].type&VAL_T_REAL) return((bool)_vl_SetR(id_el,(double)value,tm,arhiv));
    else if(elem->elem[id_el].type&VAL_T_INT)  return((bool)_vl_SetI(id_el,(int)value,tm,arhiv));
    //Chek for get curent value
    if(!tm.s || tm.s > val[id_el].time.s || (tm.s == val[id_el].time.s && tm.us > val[id_el].time.us) )
    {
	val[id_el].val.val_b = value;
        if(tm.s) val[id_el].time = tm;
	else { val[id_el].time.s = time(NULL); val[id_el].time.us = 0; }
       	if(elem->elem[id_el].source&VAL_S_BD) 
	    vl_GetCfg()->cf_Set_B(elem->elem[id_el].vals[0],val[id_el].val.val_b);       
    }
    //value geting from arhiv
    return(val[id_el].val.val_b);   //?!?!
}


