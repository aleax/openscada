
//OpenSCADA system file: tconfig.cpp
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

TConfig::TConfig( TElem *Elements ) : m_elem(NULL)
{
    elem(Elements,true);
}

TConfig::~TConfig()
{
    //Deinit value
    for(unsigned i=0; i < value.size(); i++) delete value[i];

    m_elem->valDet(this);
    if( single ) delete m_elem;
}

TConfig &TConfig::operator=(TConfig &config)
{
    vector<string> list_el;
    
    cfgList( list_el );
    for( int i_el = 0; i_el < list_el.size(); i_el++)
    {
	TCfg &s_cfg = config.cfg( list_el[i_el] );
	TCfg &d_cfg = cfg( list_el[i_el] );
	switch(d_cfg.fld().type())
	{
	    case TFld::String:	d_cfg.setS(s_cfg.getS());break;
	    case TFld::Real:	d_cfg.setR(s_cfg.getR());break;
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:
				d_cfg.setI(s_cfg.getI());break;
	    case TFld::Bool:	d_cfg.setB(s_cfg.getB());break;
	}
    }
    return *this;
}

void TConfig::detElem( TElem *el )
{
    if( el == m_elem )	elem(NULL);
}

void TConfig::addFld( TElem *el, unsigned id )
{
    value.insert( value.begin()+id,new TCfg(m_elem->fldAt(id),*this));
}
	    
void TConfig::delFld( TElem *el, unsigned id )
{
    delete value[id];
    value.erase(value.begin()+id);
}					

TCfg &TConfig::cfg( const string &n_val )
{
    int id_elem = m_elem->fldId(n_val);
    return *value[id_elem];
}

void TConfig::cfgList( vector<string> &list )
{
    list.clear();
    for(unsigned i = 0; i < value.size(); i++)
	list.push_back(value[i]->name());
}

void TConfig::elem(TElem *Elements, bool first)
{
    if(m_elem == Elements && !first ) return;
    //Clear previos setting
    if(m_elem)
    {
	for(unsigned i=0; i < value.size(); i++) delete value[i];
	m_elem->valDet(this);
	if(single) delete m_elem;
    }
    
    //Set new setting
    if( !Elements )
    {
	m_elem = new TElem("single");
	single = true;
    }
    else
    {
	m_elem = Elements;
    	single = false;
    }
    
    m_elem->valAtt(this);
    for(unsigned i=0; i < m_elem->fldSize(); i++) 
	value.push_back( new TCfg(m_elem->fldAt(i),*this));
}

TElem &TConfig::elem()
{
    return *m_elem;
}

void TConfig::cntrCmdMake( XMLNode *opt, const char *path, int pos )
{
    vector<string> list_c;
    cfgList(list_c);    
    for( unsigned i_el = 0; i_el < list_c.size(); i_el++ )
	if( cfg(list_c[i_el]).view() )
	    cfg(list_c[i_el]).fld().cntrCmdMake(opt,path,(pos<0)?pos:pos++);
}

void TConfig::cntrCmdProc( XMLNode *opt, const string &elem )
{
    if( elem.size() > 4 && elem.substr(0,4) == "sel_" && TCntrNode::ctrChkNode(opt) )
    { 
	TFld &n_e_fld = cfg(elem.substr(4)).fld();
	for( unsigned i_a=0; i_a < n_e_fld.selNm().size(); i_a++ )
	    opt->childAdd("el")->text(n_e_fld.selNm()[i_a]);
	return;
    }
    TCfg &cel = cfg(elem);
    if( TCntrNode::ctrChkNode(opt,"get",(cel.fld().flg()&FLD_NWR)?0440:0660,"root","root",SEQ_RD) )
    {
	if( cel.fld().flg()&FLD_SELECT )	opt->text(cel.getSEL());       	
	else 					opt->text(cel.getS());
    }	
    if( TCntrNode::ctrChkNode(opt,"set",(cel.fld().flg()&FLD_NWR)?0440:0660,"root","root",SEQ_WR) )
    {
	if( cel.fld().flg()&FLD_SELECT )	cel.setSEL(opt->text());
	else 					cel.setS(opt->text());
    }
}

//*************************************************
//**************** TCfg ***************************
//*************************************************
TCfg::TCfg( TFld &fld, TConfig &owner ) : m_view(true), m_owner(owner)
{
    //Chek for self field for dinamic elements
    if( fld.flg()&FLD_SELF )
    {
	m_fld = new TFld();
	*m_fld = fld;
    }
    else m_fld = &fld;
    
    switch(m_fld->type())
    {
	case TFld::String:
	    m_val.s_val    = new string;
	    *(m_val.s_val) = m_fld->def();
	    break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    			m_val.i_val = atoi(m_fld->def().c_str());	break;
	case TFld::Real: 	m_val.r_val = atof(m_fld->def().c_str());	break;
	case TFld::Bool:	m_val.b_val = (m_fld->def()=="true")?true:false;break;
    }
}

TCfg::~TCfg(  )
{
    if( m_fld->type() == TFld::String )	delete m_val.s_val;
    if( m_fld->flg()&FLD_SELF )   	delete m_fld;
}

const string &TCfg::name()
{
    return m_fld->name();
}

string TCfg::getSEL( )
{
    if( !(m_fld->flg()&FLD_SELECT) )   
	throw TError("Cfg",Mess->I18N("Element type no select!"));
    switch( m_fld->type() )
    {
	case TFld::String:	return m_fld->selVl2Nm(*m_val.s_val);
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				return m_fld->selVl2Nm(m_val.i_val);
	case TFld::Real:	return m_fld->selVl2Nm(m_val.r_val);
	case TFld::Bool:	return m_fld->selVl2Nm(m_val.b_val);
    }
}

string &TCfg::getSd( )
{
    if( m_fld->type()!=TFld::String )
        throw TError("Cfg",Mess->I18N("Element type no string!"));
	    
    return *m_val.s_val;
}

double &TCfg::getRd( )
{
    if( m_fld->type()!=TFld::Real )
	throw TError("Cfg",Mess->I18N("Element type no real!"));
	     
    return m_val.r_val;
}

int &TCfg::getId( )
{
    if( m_fld->type()!=TFld::Dec && m_fld->type()!=TFld::Hex && m_fld->type()!=TFld::Oct )
        throw TError("Cfg",Mess->I18N("Element type no int!"));
	    
    return m_val.i_val;
}

bool &TCfg::getBd( )
{
    if( m_fld->type()!=TFld::Bool )
        throw TError("Cfg",Mess->I18N("Element type no boolean!"));
	    
    return m_val.b_val;
}

string TCfg::getS( )
{
    switch(m_fld->type())
    {
	case TFld::String:	return *m_val.s_val;
	case TFld::Dec: case TFld::Hex: case TFld::Oct:
				return TSYS::int2str(m_val.i_val);
	case TFld::Real:	return TSYS::real2str(m_val.r_val);
	case TFld::Bool:	return TSYS::int2str(m_val.b_val);
    }
}

double TCfg::getR( )
{
    switch(m_fld->type())
    {
	case TFld::String:	return atof(m_val.s_val->c_str());
	case TFld::Dec: case TFld::Hex: case TFld::Oct:
				return m_val.i_val;
	case TFld::Real:	return m_val.r_val;
	case TFld::Bool:	return m_val.b_val;
    }
}

int TCfg::getI( )
{
    switch(m_fld->type())
    {
	case TFld::String:	return atoi(m_val.s_val->c_str());
	case TFld::Dec: case TFld::Hex: case TFld::Oct:
				return m_val.i_val;
	case TFld::Real:	return (int)m_val.r_val;
	case TFld::Bool:	return m_val.b_val;
    }
}

bool TCfg::getB( )
{
    switch(m_fld->type())
    {
	case TFld::String:	return atoi(m_val.s_val->c_str());
	case TFld::Dec: case TFld::Hex: case TFld::Oct:
				return m_val.i_val;
	case TFld::Real:	return (int)m_val.r_val;
	case TFld::Bool:	return m_val.b_val;
    }
}

void TCfg::setSEL( const string &val )
{
    if( !(m_fld->flg()&FLD_SELECT) ) 
	throw TError("Cfg",Mess->I18N("Element type no select!"));
    switch( m_fld->type() )
    {
	case TFld::String:      setS( m_fld->selNm2VlS(val) );	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				setI( m_fld->selNm2VlI(val) );	break;
	case TFld::Real:	setR( m_fld->selNm2VlR(val) );	break;
	case TFld::Bool:	setB( m_fld->selNm2VlB(val) );	break;
    }
}

void TCfg::setS( const string &val )
{
    switch( m_fld->type() )
    {
	case TFld::String:      
	    if( m_fld->flg()&FLD_PREV )
	    {
		string t_str = *(m_val.s_val);
		*(m_val.s_val) = val;    
		if( !m_owner.cfgChange(*this) ) 
		    *(m_val.s_val) = t_str;
	    }	    
	    else *(m_val.s_val) = val;
	    break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				setI( atoi(val.c_str()) );	break;
	case TFld::Real:	setR( atof(val.c_str()) );	break;
	case TFld::Bool:	setB( atoi(val.c_str()) );	break;
    }
}

void TCfg::setR( double val )
{
    switch( m_fld->type() )
    {
	case TFld::String:	setS(TSYS::real2str(val));	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				setI( (int)val );	break;
	case TFld::Real:
	    if( !(m_fld->flg()&FLD_SELECT) && m_fld->selValR()[0] < m_fld->selValR()[1] )
	    {
		if( val < m_fld->selValR()[0] )	val = m_fld->selValR()[0];
		if( val > m_fld->selValR()[1] )	val = m_fld->selValR()[1];
	    }
	    if( m_fld->flg()&FLD_PREV )
	    {
		double t_val = m_val.r_val;
		m_val.r_val = val;
		if( !m_owner.cfgChange(*this) ) 
		    m_val.r_val = t_val;
	    }
	    else m_val.r_val = val;				
	    break;
	case TFld::Bool:	setB( val );	break;
    }
}

void TCfg::setI( int val )
{
    switch( m_fld->type() )
    {
	case TFld::String:	setS(TSYS::int2str(val));	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    if( !(m_fld->flg()&FLD_SELECT) && m_fld->selValI()[0] < m_fld->selValI()[1] )
	    {        
		if( val < m_fld->selValI()[0] )	val = m_fld->selValI()[0];
		if( val > m_fld->selValI()[1] )	val = m_fld->selValI()[1];
	    }
	    if( m_fld->flg()&FLD_PREV )
	    {
		int t_val = m_val.i_val;
		m_val.i_val = val;
		if( !m_owner.cfgChange(*this) ) 
		    m_val.i_val = t_val;
	    }
	    else m_val.i_val = val;
	    break;
	case TFld::Real:	setR( val );    break;
	case TFld::Bool:	setB( val );	break;
    }
}

void TCfg::setB( bool val )
{
    switch( m_fld->type() )
    {
	case TFld::String:	setS(TSYS::int2str(val));	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				setI( val );	break;
	case TFld::Real:	setR( val );    break;
	case TFld::Bool:	
	    if( m_fld->flg()&FLD_PREV )
	    {
		bool t_val = m_val.b_val;
		m_val.b_val = val;
		if( !m_owner.cfgChange(*this) ) 
		    m_val.b_val = t_val;
	    }
	    else m_val.b_val = val;				
	    break;
    }
}

bool TCfg::operator==(TCfg & cfg)
{
    switch(fld().type())
    {
	case TFld::String:
	    if( cfg.fld().type()==TFld::String && getS() == cfg.getS() )	return true;
	    break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    if( (cfg.fld().type()==TFld::Dec || cfg.fld().type()==TFld::Hex || cfg.fld().type()==TFld::Oct) 
		    && getI() == cfg.getI())	return true;
	    break;
	case TFld::Real:
	    if( cfg.fld().type()==TFld::Real && getR() == cfg.getR() )	return true;
	    break;
	case TFld::Bool:
	    if( cfg.fld().type()==TFld::Bool && getB() == cfg.getB()) 	return true;
	    break;
    }
    return(false);
}

TCfg &TCfg::operator=(TCfg & cfg)
{
    switch(fld().type())
    {
	case TFld::String:	setS( cfg.getS() );	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				setI( cfg.getI() );	break;
	case TFld::Real:	setR( cfg.getR() );	break;
	case TFld::Bool:	setB( cfg.getB() );	break;
    }
    return *this;
}

