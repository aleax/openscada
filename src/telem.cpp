/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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

#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
#include "telem.h"

const char *TElem::o_name = "TElem";

TElem::TElem( const string &name ) : m_name(name) 
{

}

TElem::~TElem()
{
    while(cont.size()) delete cont[0];
    while(elem.size()) fldDel(0);
}

int TElem::fldAdd( TFld *fld, int id )
{
    vector<TFld*>::iterator iter;
    
    //Find dublicates
    for(iter = elem.begin(); iter != elem.end(); iter++)
	if((*iter)->name() == fld->name()) 
	    throw TError("(%s) Element <%s> already present!",o_name,fld->name().c_str());    
    if( id > elem.size() || id < 0 ) id = elem.size();    
    elem.insert(elem.begin()+id,fld);
    //Add value and set them default
    for(unsigned cfg_i=0; cfg_i < cont.size(); cfg_i++) cont[cfg_i]->addElem(*this,id);

    return(id);
}

void TElem::fldDel(unsigned int id)
{
    if( id >= elem.size() ) throw TError("(%s) id error!",o_name);
    for(unsigned cfg_i=0; cfg_i < cont.size(); cfg_i++) cont[cfg_i]->delElem(*this,id);
    delete elem[id];
    elem.erase(elem.begin()+id);
}

void TElem::valAtt( TValElem *cnt )
{
    for(unsigned i=0; i < cont.size() ;i++)
	if(cont[i] == cnt) throw TError("(%s) The element container already attached!",o_name);
    cont.push_back(cnt);
}
 
void TElem::valDet( TValElem *cnt )
{
    for(unsigned i=0; i < cont.size() ;i++)
	if(cont[i] == cnt) 
	{
	    cont.erase(cont.begin()+i);
	    break;
	}
}

unsigned TElem::fldId( const string &name )
{
    for(unsigned i=0; i < elem.size(); i++)
	if(elem[i]->name() == name) return(i);
    throw TError("(%s) Element <%s> no available!",o_name,name.c_str());
}

void TElem::fldList( vector<string> &list )
{
    for(unsigned i = 0; i < elem.size(); i++) list.push_back(elem[i]->name());
}

TFld &TElem::fldAt( unsigned int id )
{
    if( id >= elem.size() ) throw TError("(%s) id error!",o_name);
    return(*elem[id]);
}

//**********************************************************************
//******************** TFld - field of element *************************
//**********************************************************************
TFld::TFld( ) : m_type(T_DEC) 
{
    m_val.v_s = NULL;
}

TFld::TFld( const string &name, const string &descr, unsigned type,
            const string &valLen, const string &valDef, const string &vals, const string &nSel ) : 
    m_type(T_DEC), m_len(0), m_dec(0)
{
    int st_pos, cur_pos;    
    m_val.v_s = NULL;
    
    m_name  = name;
    m_descr = descr;
    m_type  = type; 
    m_def   = valDef;
    sscanf(valLen.c_str(),"%d.%d",&m_len,&m_dec);
    //set value list
    if( m_type&(T_SELECT|T_DEC|T_OCT|T_HEX|T_REAL) )
    {
	if( m_type&T_STRING ) 			m_val.v_s = new vector<string>;
	else if( m_type&(T_DEC|T_OCT|T_HEX) ) 	m_val.v_i = new vector<int>;
	else if( m_type&(T_REAL) ) 		m_val.v_r = new vector<double>;
	else if( m_type&(T_BOOL) ) 		m_val.v_b = new vector<bool>;	
	if( vals.size() )
	{
	    st_pos = 0;
	    string t_vl;
	    do
	    {
		cur_pos = vals.find(";",st_pos);
		if(cur_pos == st_pos) { st_pos+=1; continue; }
		t_vl = vals.substr(st_pos,cur_pos-st_pos);
		
		if( m_type&T_STRING ) 		m_val.v_s->push_back( t_vl );
		else if( m_type&(T_DEC) ) 	m_val.v_i->push_back( strtol(t_vl.c_str(),NULL,10) );
		else if( m_type&(T_OCT) ) 	m_val.v_i->push_back( strtol(t_vl.c_str(),NULL,8) );
		else if( m_type&(T_HEX) ) 	m_val.v_i->push_back( strtol(t_vl.c_str(),NULL,16) );
		else if( m_type&(T_REAL) )	m_val.v_r->push_back( atof(t_vl.c_str()) );
		else if( m_type&(T_BOOL) )	m_val.v_b->push_back( (t_vl== "true")?true:false );
		
		st_pos = cur_pos+1;
	    }while(cur_pos != string::npos);
	}
    }
    if( !(m_type&T_SELECT) && m_type&(T_DEC|T_OCT|T_HEX|T_REAL) )
    {
	if( m_type&(T_DEC|T_OCT|T_HEX) )
	    while(m_val.v_i->size() < 2) m_val.v_i->push_back(0);
	else if( m_type&T_REAL )
	    while(m_val.v_r->size() < 2) m_val.v_r->push_back(0.0);
    }
	
    //set select
    if( m_type&T_SELECT )
    {
	m_sel = new vector<string>;
    	if( nSel.size() )
	{
	    st_pos = 0;
	    do
	    {
		cur_pos = nSel.find(";",st_pos);
		if(cur_pos == st_pos) { st_pos+=1; continue; }
		m_sel->push_back(nSel.substr(st_pos,cur_pos-st_pos));
		st_pos = cur_pos+1;
	    }while(cur_pos != string::npos);
	}
    }    
}
	    

TFld::~TFld( )
{
    if( m_val.v_s != NULL )
    {
	if( m_type&T_STRING ) 			delete m_val.v_s;
	else if( m_type&(T_DEC|T_OCT|T_HEX) ) 	delete m_val.v_i;
	else if( m_type&(T_REAL) ) 		delete m_val.v_r;
	else if( m_type&(T_BOOL) ) 		delete m_val.v_b;
    }
    if( m_type&T_SELECT ) delete m_sel;    
}

vector<string> &TFld::selValS()
{ 
    if( m_type&T_SELECT && m_type&T_STRING ) return *m_val.v_s;
    throw TError("Error string values!");
}

vector<int> &TFld::selValI()
{ 
    if( m_type&(T_DEC|T_OCT|T_HEX) ) return *m_val.v_i;
    throw TError("Error int values!");
}

vector<double> &TFld::selValR()
{ 
    if( m_type&(T_REAL) ) return *m_val.v_r;
    throw TError("Error real values!");
}

vector<bool> &TFld::selValB()
{ 
    if( m_type&T_SELECT && m_type&(T_BOOL) ) return *m_val.v_b;
    throw TError("Error bool values!");
}

vector<string> &TFld::selNm()
{ 
    if( m_type&T_SELECT ) return *m_sel; 
    throw TError("%s: Field no select!",m_name.c_str());
}
	
TFld &TFld::operator=( TFld &fld )
{
    //free old
    if( m_val.v_s != NULL )
    {
	if( m_type&T_STRING ) 			delete m_val.v_s;
	else if( m_type&(T_DEC|T_OCT|T_HEX) ) 	delete m_val.v_i;
	else if( m_type&(T_REAL) ) 		delete m_val.v_r;
	else if( m_type&(T_BOOL) ) 		delete m_val.v_b;
    }    
    if( m_type&T_SELECT ) delete m_sel;
    //create new
    m_name  = fld.name();
    m_descr = fld.descr();
    m_len   = fld.len();
    m_type  = fld.type();
    m_descr = fld.def();
    //m_vals  = fld.vals();
    
    if( m_type&T_SELECT && m_type&T_STRING )
    {
	m_val.v_s = new vector<string>;
	*(m_val.v_s) = fld.selValS();
    }
    else if( m_type&(T_DEC|T_OCT|T_HEX) )
    {
	m_val.v_i = new vector<int>;
	*(m_val.v_i) = fld.selValI();
    }
    else if( m_type&T_REAL ) 		
    {
	m_val.v_r = new vector<double>;
	*(m_val.v_r) = fld.selValR();
    }
    else if( m_type&T_SELECT && m_type&T_BOOL )
    {
	m_val.v_b = new vector<bool>;
	*(m_val.v_b) = fld.selValB();
    }
    
    if( m_type&T_SELECT ) 
    {
	m_sel  = new vector<string>;
	*m_sel = fld.selNm();
    }
    return *this;
}

string TFld::selVl2Nm( const string &val )
{
    if( m_type&T_SELECT && m_type&T_STRING )
    {
	int i_val = 0;
	if( !m_sel->size() ) return("Empty");
	for(i_val = 0; i_val < m_val.v_s->size(); i_val++) 
	    if((*m_val.v_s)[i_val] == val) break;
        if( i_val >= m_val.v_s->size() ) i_val = 0;
	    return((*m_sel)[i_val]);
    }
    throw TError("%s: Select error!",m_name.c_str());     
}

string TFld::selVl2Nm( int val )
{
    if( m_type&T_SELECT && m_type&(T_DEC|T_OCT|T_HEX) )
    {
	int i_val = 0;
	if( !m_sel->size() ) return("Empty");
       	for(i_val = 0; i_val < m_val.v_i->size(); i_val++)
    	    if((*m_val.v_i)[i_val] == val) break;
	if( i_val >= m_val.v_i->size() ) i_val = 0;
	return((*m_sel)[i_val]);
    }
    throw TError("%s: Select error!",m_name.c_str());     
}

string TFld::selVl2Nm( double val )
{
    if( m_type&T_SELECT && m_type&T_REAL )
    {
	int i_val = 0;
	if( !m_sel->size() ) return("Empty");
	for(i_val = 0; i_val < m_val.v_r->size(); i_val++)
	    if((*m_val.v_r)[i_val] == val) break;
        if( i_val >= m_val.v_r->size() ) i_val = 0;
	return((*m_sel)[i_val]);
    }
    throw TError("%s: Select error!",m_name.c_str());
}

string TFld::selVl2Nm( bool val )
{
    if( m_type&T_SELECT && m_type&T_BOOL )
    {
	int i_val;
	if( !m_sel->size() ) return("Empty");
	for(i_val = 0; i_val < m_val.v_b->size(); i_val++)
	    if( (*m_val.v_b)[i_val] == val ) break;
	if( i_val >= m_val.v_b->size() ) i_val = 0;
	    return((*m_sel)[i_val]);
    }
    throw TError("%s: Select error!",m_name.c_str());
}

string &TFld::selNm2VlS( const string &name )
{
    if( m_type&T_SELECT && m_type&T_STRING )
	for(int i_val = 0; i_val < m_sel->size(); i_val++)
	    if( name == (*m_sel)[i_val] ) return (*m_val.v_s)[i_val];
    throw TError("%s: Select error!",m_name.c_str());    
}

int TFld::selNm2VlI( const string &name )
{
    if( m_type&T_SELECT && m_type&(T_DEC|T_OCT|T_HEX) )
	for(int i_val = 0; i_val < m_sel->size(); i_val++)
	    if( name == (*m_sel)[i_val] ) return (*m_val.v_i)[i_val];
    throw TError("%s: Select error!",m_name.c_str());    
}

double TFld::selNm2VlR( const string &name )
{
    if( m_type&T_SELECT && m_type&T_REAL )
	for(int i_val = 0; i_val < m_sel->size(); i_val++)
	    if( name == (*m_sel)[i_val] ) return (*m_val.v_r)[i_val];
    throw TError("%s: Select error!",m_name.c_str());    
}

bool TFld::selNm2VlB( const string &name )
{
    if( m_type&T_SELECT && m_type&T_BOOL )
	for(int i_val = 0; i_val < m_sel->size(); i_val++)
	    if( name == (*m_sel)[i_val] ) return (*m_val.v_b)[i_val];
    throw TError("%s: Select error!",m_name.c_str());    
}

//**********************************************************************
//************** TValElem - container of elements **********************
//**********************************************************************

TValElem::TValElem()
{

}

TValElem::~TValElem()
{

}

