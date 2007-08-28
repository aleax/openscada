
//OpenSCADA system file: telem.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "tsys.h"
#include "tmess.h"
#include "tbds.h"
#include "telem.h"

TElem::TElem( const string &name ) : m_name(name) 
{

}

TElem::~TElem()
{
    while(cont.size())	cont[0]->detElem(this);
    while(elem.size()) 	fldDel(0);
}

int TElem::fldAdd( TFld *fld, int id )
{
    //Find dublicates
    for( int i_f = 0; i_f < elem.size(); i_f++)
	if(elem[i_f]->name() == fld->name()) return i_f;
    if( id > elem.size() || id < 0 ) id = elem.size();
    elem.insert(elem.begin()+id,fld);
    //Add value and set them default
    for(unsigned cfg_i=0; cfg_i < cont.size(); cfg_i++) 
	cont[cfg_i]->addFld(this,id);

    return id;
}

void TElem::fldDel( unsigned int id )
{
    if( id >= elem.size() ) throw TError("Elem",_("Id error!"));
    for(unsigned cfg_i=0; cfg_i < cont.size(); cfg_i++) 
	cont[cfg_i]->delFld(this,id);
    delete elem[id];
    elem.erase(elem.begin()+id);
}

void TElem::valAtt( TValElem *cnt )
{
    for(unsigned i=0; i < cont.size() ;i++)
	if(cont[i] == cnt) throw TError("Elem",_("The element container already attached!"));
    cont.push_back(cnt);
}
 
void TElem::valDet( TValElem *cnt )
{
    for(unsigned i=0; i < cont.size() ;i++)
	if(cont[i] == cnt) { cont.erase(cont.begin()+i); break; }
}

unsigned TElem::fldId( const string &name )
{
    for(unsigned i=0; i < elem.size(); i++)
	if(elem[i]->name() == name) return(i);
    throw TError("Elem",_("Element <%s> no present!"),name.c_str());
}

bool TElem::fldPresent( const string &name )
{
    for(unsigned i=0; i < elem.size(); i++)
        if(elem[i]->name() == name) return true;
    return false;	
}

void TElem::fldList( vector<string> &list )
{
    list.clear();
    for(unsigned i = 0; i < elem.size(); i++) 
	list.push_back(elem[i]->name());
}

TFld &TElem::fldAt( unsigned int id )
{
    if( id >= elem.size() ) throw TError("Elem",_("Id error!"));
    return(*elem[id]);
}

//**********************************************************************
//******************** TFld - field of element *************************
//**********************************************************************
TFld::TFld( ) : m_type(TFld::Integer), m_flg(0)
{
    m_sel = NULL;
    m_val.v_s = NULL;
}

TFld::TFld( TFld &ifld ) : m_type(TFld::Integer), m_len(0), m_dec(0), m_flg(0)
{
    int st_pos, cur_pos;    
    m_sel   = NULL;
    m_val.v_s = NULL;
    
    m_name  = ifld.name();
    m_descr = ifld.descr();
    m_type  = ifld.type();
    m_flg   = ifld.flg();
    m_def   = ifld.def();
    m_res   = ifld.reserve();
    m_len   = ifld.len();
    m_dec   = ifld.dec();

    setValues(ifld.values());
    setSelNames(ifld.selNames());
}

TFld::TFld( const char *name, const char *descr, TFld::Type itype, unsigned iflg,
            const char *valLen, const char *valDef, const char *val_s, const char *n_Sel, int ires ) : 
    m_type(TFld::Integer), m_len(0), m_dec(0), m_flg(0)
{
    int st_pos, cur_pos;    
    m_sel   = NULL;
    m_val.v_s = NULL;
    
    m_name  = name;
    m_descr = descr;    
    m_type  = itype; 
    m_flg   = iflg;
    m_def   = valDef;
    m_res   = ires;
    
    sscanf(valLen,"%d.%d",&m_len,&m_dec);
    setValues(val_s);
    setSelNames(n_Sel);
}
	    

TFld::~TFld( )
{
    if( m_sel )	delete m_sel;    
    if( m_val.v_s != NULL )
	switch(type())
	{
	    case TFld::String:	delete m_val.v_s;	break;			
	    case TFld::Integer:	delete m_val.v_i; 	break;
	    case TFld::Real:	delete m_val.v_r;	break;
	    case TFld::Boolean:	delete m_val.v_b;	break;
	}
}

void TFld::setFlg( unsigned iflg )
{
    unsigned ch_flg = iflg^flg();
    if( ch_flg&SelfFld ) iflg=iflg^(ch_flg&SelfFld);
    if( ch_flg&Selected )iflg=iflg^(ch_flg&Selected);
    m_flg = iflg;
}

string TFld::values()
{
    if( m_val.v_s == NULL )	return "";

    string rez;
    switch(type())
    {
        case TFld::String:  
	    for(int i_el = 0; i_el < m_val.v_s->size(); i_el++)
		rez=rez+(*m_val.v_s)[i_el]+";";    
	    break;
        case TFld::Integer: 
	    for(int i_el = 0; i_el < m_val.v_i->size(); i_el++)
		rez=rez+TSYS::int2str((*m_val.v_i)[i_el])+";";    
	    break;	
        case TFld::Real:    
	    for(int i_el = 0; i_el < m_val.v_r->size(); i_el++)
		rez=rez+TSYS::real2str((*m_val.v_r)[i_el],6)+";";
	    break;	
        case TFld::Boolean: 
	    for(int i_el = 0; i_el < m_val.v_b->size(); i_el++)
		rez=rez+TSYS::int2str((*m_val.v_b)[i_el])+";";    
	    break;
    }
    return rez.size()?rez.substr(0,rez.size()-1):"";
}

string TFld::selNames()
{
    if( m_sel == NULL )	return "";
    
    string rez;
    for(int i_el = 0; i_el < m_sel->size(); i_el++)
        rez=rez+(*m_sel)[i_el]+";";
    
    return rez.size()?rez.substr(0,rez.size()-1):"";
}

void TFld::setValues( const string &vls )
{
    //set value list
    if( flg()&TFld::Selected )
    {
	//Count alements amount
	int i_lvl = 0, i_off = 0;
        while( TSYS::strSepParse(vls,0,';',&i_off).size() ) i_lvl++;
	
	switch(type())
	{
	    case TFld::String:	
		if(!m_val.v_s)	m_val.v_s = new vector<string>; 
		m_val.v_s->resize(i_lvl,"");
		break;			
	    case TFld::Integer:	
		if(!m_val.v_i)	m_val.v_i = new vector<int>;
		m_val.v_i->resize(i_lvl,0);
		break;
	    case TFld::Real:
		if(!m_val.v_r)	m_val.v_r = new vector<double>;
		m_val.v_r->resize(i_lvl,0);
		break;
	    case TFld::Boolean:	
		if(!m_val.v_b)	m_val.v_b = new vector<bool>;
		m_val.v_b->resize(i_lvl,false);
		break;
	}
        //Get elements
	for( int i = 0, i_off=0; i < i_lvl; i++ )
	{
	    string s_el = TSYS::strSepParse(vls,0,';',&i_off);
    	    switch(type())
	    {
		case TFld::String:	(*m_val.v_s)[i] = s_el; break;
		case TFld::Integer:	(*m_val.v_i)[i] = strtol(s_el.c_str(),NULL,(flg()&HexDec)?16:((flg()&OctDec)?8:10));	break;
		case TFld::Real:	(*m_val.v_r)[i] = atof(s_el.c_str());	break;
		case TFld::Boolean:	(*m_val.v_b)[i] = atoi(s_el.c_str());	break;
	    }
	}
    }
    else
	switch(type())
	{
	    case TFld::Integer:
		if(!m_val.v_i)	m_val.v_i = new vector<int>;
		m_val.v_i->resize(2,0);
		(*m_val.v_i)[0] = strtol(TSYS::strSepParse(vls,0,';').c_str(),NULL,(flg()&HexDec)?16:((flg()&OctDec)?8:10));
		(*m_val.v_i)[1] = strtol(TSYS::strSepParse(vls,1,';').c_str(),NULL,(flg()&HexDec)?16:((flg()&OctDec)?8:10));
		break;		
	    case TFld::Real:	
		if(!m_val.v_r)	m_val.v_r = new vector<double>;
		m_val.v_r->resize(2,0);
		(*m_val.v_r)[0] = atof(TSYS::strSepParse(vls,0,';').c_str());
		(*m_val.v_r)[1] = atof(TSYS::strSepParse(vls,1,';').c_str());
		break;
	}
}

void TFld::setSelNames( const string &slnms )
{
    //set value list
    if( !(flg()&TFld::Selected) ) return;
    
    int i_lvl = 0;
    for( int i_off = 0; TSYS::strSepParse(slnms,0,';',&i_off).size(); i_lvl++ );
    
    if(!m_sel)  m_sel = new vector<string>;
    m_sel->resize(i_lvl,"");
    
    for( int i = 0, i_off = 0; i < i_lvl; i++ )
	(*m_sel)[i] = TSYS::strSepParse(slnms,0,';',&i_off);
}

const vector<string> &TFld::selValS()
{ 
    if( flg()&TFld::Selected && type() == TFld::String ) 
	return *m_val.v_s;
    throw TError("Field",_("Error string values!"));
}

const vector<int> &TFld::selValI()
{ 
    if( type() == TFld::Integer ) 
	return *m_val.v_i;
    throw TError("Field",_("Error int values!"));
}

const vector<double> &TFld::selValR()
{ 
    if( type() == TFld::Real ) 
	return *m_val.v_r;
    throw TError("Field",_("Error real values!"));
}

const vector<bool> &TFld::selValB()
{ 
    if( flg()&TFld::Selected && type() == TFld::Boolean ) 
	return *m_val.v_b;
    throw TError("Field",_("Error bool values!"));
}

const vector<string> &TFld::selNm()
{ 
    if( m_sel && flg()&TFld::Selected )	return *m_sel; 
    throw TError("Field",_("No select type!"));
}
	
TFld &TFld::operator=( TFld &fld )
{
    //free old
    if( m_sel )	delete m_sel;    
    if( m_val.v_s != NULL )
	switch(type())
	{
	    case TFld::String:	delete m_val.v_s;	break;			
	    case TFld::Integer:	delete m_val.v_i; 	break;
	    case TFld::Real:	delete m_val.v_r;	break;
	    case TFld::Boolean:	delete m_val.v_b;	break;
	}
    //create new
    m_name  = fld.name();
    m_descr = fld.descr();
    m_len   = fld.len();
    m_flg   = fld.flg();
    m_type  = fld.type();
    m_def   = fld.def();
    //m_vals  = fld.vals();
   
    //Copy select and values border
    if( flg()&TFld::Selected )
    {
	m_sel  = new vector<string>;
	*m_sel = fld.selNm();
	switch(type())
	{
	    case TFld::String:	
		m_val.v_s = new vector<string>;
	        *(m_val.v_s) = fld.selValS();
		break;			
	    case TFld::Integer:	
		m_val.v_i = new vector<int>;
		*(m_val.v_i) = fld.selValI();		
		break;
	    case TFld::Real:	
		m_val.v_r = new vector<double>;
		*(m_val.v_r) = fld.selValR();		
		break;
	    case TFld::Boolean:	
		m_val.v_b = new vector<bool>;
		*(m_val.v_b) = fld.selValB();		
		break;
	}
    }
    else
	switch(type())
	{
	    case TFld::Integer:
		m_val.v_i = new vector<int>;
		*(m_val.v_i) = fld.selValI();		
		break;
	    case TFld::Real:	
		m_val.v_r = new vector<double>;
		*(m_val.v_r) = fld.selValR();		
		break;
	}
    return *this;
}

string TFld::selVl2Nm( const string &val )
{
    if( flg()&TFld::Selected && type() == TFld::String )
    {
	int sz = vmin(m_sel->size(), m_val.v_s->size());
	if( !sz ) return("Empty");
	int i_val = 0;
	for(i_val = 0; i_val < sz; i_val++) 
	    if((*m_val.v_s)[i_val] == val) break;
        if( i_val >= sz ) return val;
    	return (*m_sel)[i_val];
    }
    throw TError("Field",_("Select error! Val: <%s>."),val.c_str());
}

string TFld::selVl2Nm( int val )
{
    if( flg()&TFld::Selected && type()==TFld::Integer )
    {
	int sz = vmin(m_sel->size(), m_val.v_i->size());
	if( !sz ) return("Empty");
	int i_val = 0;
       	for(i_val = 0; i_val < sz; i_val++)
    	    if((*m_val.v_i)[i_val] == val) break;
	if( i_val >= sz ) return TSYS::int2str(val);
	return (*m_sel)[i_val];
    }
    throw TError("Field",_("Select error! Val: <%d>."),val);
}

string TFld::selVl2Nm( double val )
{
    if( flg()&TFld::Selected && type()==TFld::Real )
    {
	int sz = vmin(m_sel->size(), m_val.v_r->size());
	if( !sz ) return("Empty");
	int i_val = 0;
	for(i_val = 0; i_val < sz; i_val++)
	    if((*m_val.v_r)[i_val] == val) break;
        if( i_val >= sz ) return TSYS::real2str(val);
	return (*m_sel)[i_val];
    }
    throw TError("Field",_("Select error! Val: <%f>."),val);
}

string TFld::selVl2Nm( bool val )
{
    if( flg()&TFld::Selected && type()==TFld::Boolean )
    {
	int sz = vmin(m_sel->size(), m_val.v_b->size());
	if( !sz ) return("Empty");
	int i_val;
	for(i_val = 0; i_val < sz; i_val++)
	    if( (*m_val.v_b)[i_val] == val ) break;
	if( i_val >= sz ) return TSYS::int2str(val);
    	return (*m_sel)[i_val];
    }
    throw TError("Field",_("Select error! Val: <%d>."),val);
}

string TFld::selNm2VlS( const string &name )
{
    if( flg()&TFld::Selected && type()==TFld::String )
	for(int i_val = 0; i_val < vmin(m_sel->size(), m_val.v_s->size()); i_val++)
	    if( name == (*m_sel)[i_val] ) 
		return (*m_val.v_s)[i_val];
    return name;
    //throw TError("Field",_("Select error! Name: <%s>."),name.c_str());    
}

int TFld::selNm2VlI( const string &name )
{
    if( flg()&TFld::Selected && type()==TFld::Integer )
	for(int i_val = 0; i_val < vmin(m_sel->size(), m_val.v_i->size()); i_val++)
	    if( name == (*m_sel)[i_val] ) 
		return (*m_val.v_i)[i_val];
    return atoi(name.c_str());
    //throw TError("Field",_("Select error! Name: <%s>."),name.c_str());    
}

double TFld::selNm2VlR( const string &name )
{
    if( flg()&TFld::Selected && type()==TFld::Real )
	for(int i_val = 0; i_val < vmin(m_sel->size(), m_val.v_r->size()); i_val++)
	    if( name == (*m_sel)[i_val] ) 
		return (*m_val.v_r)[i_val];
    return atof(name.c_str());
    //throw TError("Field",_("Select error! Name: <%s>."),name.c_str());    
}

bool TFld::selNm2VlB( const string &name )
{
    if( flg()&TFld::Selected && type()==TFld::Boolean )
	for(int i_val = 0; i_val < vmin(m_sel->size(), m_val.v_b->size()); i_val++)
	    if( name == (*m_sel)[i_val] ) 
		return (*m_val.v_b)[i_val];
    return atoi(name.c_str());
    //throw TError("Field",_("Select error! Name: <%s>."),name.c_str());    
}

XMLNode *TFld::cntrCmdMake( XMLNode *opt, const string &path, int pos, const string &user, const string &grp, int perm )
{
    XMLNode *n_e = TCntrNode::ctrMkNode("fld",opt,pos,(path+"/"+name()).c_str(),descr(),
	    (flg()&TFld::NoWrite)?(perm&~0222):perm,user.c_str(),grp.c_str(),1,"len",TSYS::int2str(len()).c_str());
    if( n_e )
    {	    
	if(flg()&TFld::Selected) 
	    n_e->setAttr_("tp","str")->setAttr_("len","")->setAttr_("dest","select")->
		setAttr("select",path+"/sel_"+name());
	else switch(type())
	{	    
    	    case TFld::String:	
		n_e->setAttr_("tp","str");	
		if( flg()&FullText )	n_e->setAttr_("cols","50")->setAttr_("rows","4");
		break;
    	    case TFld::Integer:	n_e->setAttr_("tp",(flg()&HexDec)?"hex":((flg()&OctDec)?"oct":"dec")); break;
    	    case TFld::Real:	n_e->setAttr_("tp","real");break;
    	    case TFld::Boolean:	n_e->setAttr_("tp","bool");break;
	}
    }
    return n_e;
}


