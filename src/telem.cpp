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

#include "tsys.h"
#include "tmess.h"
#include "tbds.h"
#include "telem.h"

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
	    throw TError("Elem","Element <%s> already present!",fld->name().c_str());    
    if( id > elem.size() || id < 0 ) id = elem.size();    
    elem.insert(elem.begin()+id,fld);
    //Add value and set them default
    for(unsigned cfg_i=0; cfg_i < cont.size(); cfg_i++) cont[cfg_i]->addElem(*this,id);

    return(id);
}

void TElem::fldDel(unsigned int id)
{
    if( id >= elem.size() ) throw TError("Elem","Id error!");
    for(unsigned cfg_i=0; cfg_i < cont.size(); cfg_i++) cont[cfg_i]->delElem(*this,id);
    delete elem[id];
    elem.erase(elem.begin()+id);
}

void TElem::valAtt( TValElem *cnt )
{
    for(unsigned i=0; i < cont.size() ;i++)
	if(cont[i] == cnt) throw TError("Elem","The element container already attached!");
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
    throw TError("Elem","Element <%s> no present!",name.c_str());
}

bool TElem::fldPresent( const string &name )
{
    for(unsigned i=0; i < elem.size(); i++)
        if(elem[i]->name() == name) return true;
    return false;	
}

void TElem::fldList( vector<string> &list )
{
    for(unsigned i = 0; i < elem.size(); i++) list.push_back(elem[i]->name());
}

TFld &TElem::fldAt( unsigned int id )
{
    if( id >= elem.size() ) throw TError("Elem","Id error!");
    return(*elem[id]);
}

//**********************************************************************
//******************** TFld - field of element *************************
//**********************************************************************
TFld::TFld( ) : m_type(TFld::Dec), m_flg(0)
{
    m_val.v_s = NULL;
}

TFld::TFld( const char *name, const char *descr, TFld::Type itype, unsigned char iflg,
            const char *valLen, const char *valDef, const char *val_s, const char *n_Sel ) : 
    m_type(TFld::Dec), m_len(0), m_dec(0), m_flg(0)
{
    int st_pos, cur_pos;    
    m_val.v_s = NULL;
    
    m_name  = name;
    m_descr = descr;    
    m_type  = itype; 
    m_flg   = iflg;
    m_def   = valDef;
    string vals = val_s;
    string nSel = n_Sel;
    
    sscanf(valLen,"%d.%d",&m_len,&m_dec);
    //set value list
    if( flg()&FLD_SELECT )
    {
	m_sel = new vector<string>;
	switch(type())
	{
	    case TFld::String:	m_val.v_s = new vector<string>; break;			
	    case TFld::Dec:
	    case TFld::Oct:
	    case TFld::Hex:	m_val.v_i = new vector<int>; 	break;
	    case TFld::Real:	m_val.v_r = new vector<double>;	break;
	    case TFld::Bool:	m_val.v_b = new vector<bool>;	break;
	}
        //Get select elements
	string 	s_el;
	int     i_lvl=0;
	while( (s_el = TSYS::strSepParse(nSel,i_lvl,';')).size() )
	{
	    m_sel->push_back(s_el);
	    s_el = TSYS::strSepParse(vals,i_lvl,';');
    	    switch(type())
	    {
		case TFld::String:	m_val.v_s->push_back( s_el ); break;
		case TFld::Dec:		m_val.v_i->push_back( strtol(s_el.c_str(),NULL,10) );	break;
		case TFld::Oct:		m_val.v_i->push_back( strtol(s_el.c_str(),NULL,8) );	break;
		case TFld::Hex:		m_val.v_i->push_back( strtol(s_el.c_str(),NULL,16) );	break;
		case TFld::Real:	m_val.v_r->push_back( atof(s_el.c_str()) );	break;
		case TFld::Bool:	m_val.v_b->push_back( (s_el == "true")?true:false );	break;
	    }
	    i_lvl++;
	}
    }
    else
	switch(type())
	{
	    case TFld::Dec:		
		m_val.v_i = new vector<int>;
		m_val.v_i->push_back( strtol(TSYS::strSepParse(vals,0,';').c_str(),NULL,10) );
		m_val.v_i->push_back( strtol(TSYS::strSepParse(vals,1,';').c_str(),NULL,10) );
		break;		
	    case TFld::Oct:
		m_val.v_i = new vector<int>;
		m_val.v_i->push_back( strtol(TSYS::strSepParse(vals,0,';').c_str(),NULL,8) );
		m_val.v_i->push_back( strtol(TSYS::strSepParse(vals,1,';').c_str(),NULL,8) );
		break;		
	    case TFld::Hex:	
		m_val.v_i = new vector<int>;
		m_val.v_i->push_back( strtol(TSYS::strSepParse(vals,0,';').c_str(),NULL,16) );
		m_val.v_i->push_back( strtol(TSYS::strSepParse(vals,1,';').c_str(),NULL,16) );
		break;		
	    case TFld::Real:	
		m_val.v_r = new vector<double>;
		m_val.v_r->push_back( atof(TSYS::strSepParse(vals,0,';').c_str()) );
		m_val.v_r->push_back( atof(TSYS::strSepParse(vals,1,';').c_str()) );
		break;
	}	
}
	    

TFld::~TFld( )
{
    if( flg()&FLD_SELECT ) 	delete m_sel;    
    if( m_val.v_s != NULL )
	switch(type())
	{
	    case TFld::String:	delete m_val.v_s;	break;			
	    case TFld::Dec:	
	    case TFld::Oct:
	    case TFld::Hex:	delete m_val.v_i; 	break;
	    case TFld::Real:	delete m_val.v_r;	break;
	    case TFld::Bool:	delete m_val.v_b;	break;
	}
}

vector<string> &TFld::selValS()
{ 
    if( flg()&FLD_SELECT && type() == TFld::String ) 
	return *m_val.v_s;
    throw TError(name().c_str(),"Error string values!");
}

vector<int> &TFld::selValI()
{ 
    if( type() == TFld::Dec || type() == TFld::Oct || type() == TFld::Hex ) 
	return *m_val.v_i;
    throw TError(name().c_str(),"Error int values!");
}

vector<double> &TFld::selValR()
{ 
    if( type() == TFld::Real ) 
	return *m_val.v_r;
    throw TError(name().c_str(),"Error real values!");
}

vector<bool> &TFld::selValB()
{ 
    if( flg()&FLD_SELECT && type() == TFld::Bool ) 
	return *m_val.v_b;
    throw TError(name().c_str(),"Error bool values!");
}

vector<string> &TFld::selNm()
{ 
    if( flg()&FLD_SELECT ) 	return *m_sel; 
    throw TError("%s: Field no select!",m_name.c_str());
}
	
TFld &TFld::operator=( TFld &fld )
{
    //free old
    if( flg()&FLD_SELECT ) 	delete m_sel;    
    if( m_val.v_s != NULL )
	switch(type())
	{
	    case TFld::String:	delete m_val.v_s;	break;			
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:	
				delete m_val.v_i; 	break;
	    case TFld::Real:	delete m_val.v_r;	break;
	    case TFld::Bool:	delete m_val.v_b;	break;
	}
    //create new
    m_name  = fld.name();
    m_descr = fld.descr();
    m_len   = fld.len();
    m_flg   = fld.flg();
    m_type  = fld.type();
    m_descr = fld.def();
    //m_vals  = fld.vals();
   
    //Copy select and values border
    if( flg()&FLD_SELECT )
    {
	m_sel  = new vector<string>;
	*m_sel = fld.selNm();
	switch(type())
	{
	    case TFld::String:	
		m_val.v_s = new vector<string>;
	        *(m_val.v_s) = fld.selValS();
		break;			
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:	
		m_val.v_i = new vector<int>;
		*(m_val.v_i) = fld.selValI();		
		break;
	    case TFld::Real:	
		m_val.v_r = new vector<double>;
		*(m_val.v_r) = fld.selValR();		
		break;
	    case TFld::Bool:	
		m_val.v_b = new vector<bool>;
		*(m_val.v_b) = fld.selValB();		
		break;
	}
    }
    else
	switch(type())
	{
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:	
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
    if( flg()&FLD_SELECT && type() == TFld::String )
    {
	int sz = vmin(m_sel->size(), m_val.v_s->size());
	if( !sz ) return("Empty");
	int i_val = 0;
	for(i_val = 0; i_val < sz; i_val++) 
	    if((*m_val.v_s)[i_val] == val) break;
        if( i_val >= sz ) i_val = 0;
    	return((*m_sel)[i_val]);
    }
    throw TError((string("Elem:")+m_name).c_str(),"Select error! Val: <%s>.",val.c_str());
}

string TFld::selVl2Nm( int val )
{
    if( flg()&FLD_SELECT && (type()==TFld::Dec || type()==TFld::Oct || type()==TFld::Hex) )
    {
	int sz = vmin(m_sel->size(), m_val.v_i->size());
	if( !sz ) return("Empty");
	int i_val = 0;
       	for(i_val = 0; i_val < sz; i_val++)
    	    if((*m_val.v_i)[i_val] == val) break;
	if( i_val >= sz ) i_val = 0;
	return((*m_sel)[i_val]);
    }
    throw TError((string("Elem:")+m_name).c_str(),"Select error! Val: <%d>.",val);
}

string TFld::selVl2Nm( double val )
{
    if( flg()&FLD_SELECT && type()==TFld::Real )
    {
	int sz = vmin(m_sel->size(), m_val.v_r->size());
	if( !sz ) return("Empty");
	int i_val = 0;
	for(i_val = 0; i_val < sz; i_val++)
	    if((*m_val.v_r)[i_val] == val) break;
        if( i_val >= sz ) i_val = 0;
	return((*m_sel)[i_val]);
    }
    throw TError((string("Elem:")+m_name).c_str(),"Select error! Val: <%f>.",val);
}

string TFld::selVl2Nm( bool val )
{
    if( flg()&FLD_SELECT && type()==TFld::Bool )
    {
	int sz = vmin(m_sel->size(), m_val.v_b->size());
	if( !sz ) return("Empty");
	int i_val;
	for(i_val = 0; i_val < sz; i_val++)
	    if( (*m_val.v_b)[i_val] == val ) break;
	if( i_val >= sz ) i_val = 0;
    	return((*m_sel)[i_val]);
    }
    throw TError((string("Elem:")+m_name).c_str(),"Select error! Val: <%d>.",val);
}

string &TFld::selNm2VlS( const string &name )
{
    if( flg()&FLD_SELECT && type()==TFld::String )
	for(int i_val = 0; i_val < vmin(m_sel->size(), m_val.v_s->size()); i_val++)
	    if( name == (*m_sel)[i_val] ) 
		return (*m_val.v_s)[i_val];
    throw TError((string("Elem:")+m_name).c_str(),"Select error! Name: <%s>.",name.c_str());    
}

int TFld::selNm2VlI( const string &name )
{
    if( flg()&FLD_SELECT && (type()==TFld::Dec || type()==TFld::Oct || type()==TFld::Hex) )
	for(int i_val = 0; i_val < vmin(m_sel->size(), m_val.v_i->size()); i_val++)
	    if( name == (*m_sel)[i_val] ) 
		return (*m_val.v_i)[i_val];
    throw TError((string("Elem:")+m_name).c_str(),"Select error! Name: <%s>.",name.c_str());    
}

double TFld::selNm2VlR( const string &name )
{
    if( flg()&FLD_SELECT && type()==TFld::Real )
	for(int i_val = 0; i_val < vmin(m_sel->size(), m_val.v_r->size()); i_val++)
	    if( name == (*m_sel)[i_val] ) 
		return (*m_val.v_r)[i_val];
    throw TError((string("Elem:")+m_name).c_str(),"Select error! Name: <%s>.",name.c_str());    
}

bool TFld::selNm2VlB( const string &name )
{
    if( flg()&FLD_SELECT && type()==TFld::Bool )
	for(int i_val = 0; i_val < vmin(m_sel->size(), m_val.v_b->size()); i_val++)
	    if( name == (*m_sel)[i_val] ) 
		return (*m_val.v_b)[i_val];
    throw TError((string("Elem:")+m_name).c_str(),"Select error! Name: <%s>.",name.c_str());    
}

void TFld::cntrMake( XMLNode *fld, const char *req, const char *path, int pos )
{
    XMLNode *n_e = TSYS::ctrInsNode("fld",pos,fld,req,(string(path)+"/"+name()).c_str(),descr(),
	    (flg()&FLD_NWR)?0440:0660)->attr("len",TSYS::int2str(len()));
    if(flg()&FLD_SELECT) 
	n_e->attr_("tp","str")->attr_("len","")->attr_("dest","select")->attr("select",string(path)+"/sel_"+name());
    else switch(type())
    {	    
        case TFld::String:	
	    n_e->attr_("tp","str");	
	    if( len() > 100 )	n_e->attr_("cols","50")->attr_("rows","4");
	    break;
        case TFld::Dec:		n_e->attr_("tp","dec");	break;
        case TFld::Oct:		n_e->attr_("tp","oct");	break;
        case TFld::Hex:		n_e->attr_("tp","hex");	break;
        case TFld::Real:	n_e->attr_("tp","real");break;
        case TFld::Bool:	n_e->attr_("tp","bool");break;
    }
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

