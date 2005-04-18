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

#include "tcntrnode.h" 
#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
#include "tconfig.h"

TConfig::TConfig( TElem *Elements )
{
    if( Elements == NULL)    
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
    //Init value
    for(unsigned i=0; i < m_elem->fldSize(); i++) value.push_back( new TCfg(m_elem->fldAt(i),*this));
}

TConfig::~TConfig()
{
    if(m_elem == NULL) return;
    //Init value
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
	try
	{
	    TCfg &s_cfg = config.cfg( list_el[i_el] );
	    TCfg &d_cfg = cfg( list_el[i_el] );
	    if( d_cfg.fld().type()&T_STRING ) 			d_cfg.setS(s_cfg.getS());
	    else if( d_cfg.fld().type()&T_REAL ) 		d_cfg.setR(s_cfg.getR());
	    else if( d_cfg.fld().type()&(T_DEC|T_OCT|T_HEX) ) 	d_cfg.setI(s_cfg.getI());
	    else if( d_cfg.fld().type()&T_BOOL )		d_cfg.setB(s_cfg.getB());
	}catch(...){ }
    }
    return *this;
}

void TConfig::addElem( TElem &el, unsigned id )
{
    value.insert( value.begin()+id,new TCfg(m_elem->fldAt(id),*this));
}
	    
void TConfig::delElem( TElem &el, unsigned id )
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

void TConfig::elem(TElem *Elements)
{
    if(m_elem == Elements) return;
    if(m_elem != NULL)
    {
	for(unsigned i=0; i < value.size(); i++) delete value[i];
	m_elem->valDet(this);
	if(single) delete m_elem;
    }
    
    if( Elements == NULL)    
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
    for(unsigned i=0; i < m_elem->fldSize(); i++) value.push_back( new TCfg(m_elem->fldAt(i),*this));
}

TElem &TConfig::elem()
{
    if(m_elem == NULL) throw TError("Structure no attached!");
    return(*m_elem);
}

void TConfig::cntrMake( XMLNode *fld, const char *req, const char *path, int pos )
{    	
    vector<string> list_c;
    cfgList(list_c);
    
    for( unsigned i_el = 0; i_el < list_c.size(); i_el++ )
	if( cfg(list_c[i_el]).view() )
	    cfg(list_c[i_el]).fld().cntrMake(fld,req,path,(pos<0)?pos:pos++);
}

void TConfig::cntrCmd( const string &elem, XMLNode *fld, int cmd )
{   
    if( cmd==TCntrNode::Get && elem.substr(0,4) == "sel_" )
	{
	    TFld &n_e_fld = cfg(elem.substr(4)).fld();
	    for( unsigned i_a=0; i_a < n_e_fld.selNm().size(); i_a++ )
		TCntrNode::ctrSetS( fld, n_e_fld.selNm()[i_a] );
	    return;
	}
    TFld &n_e_fld = cfg(elem).fld();
    if(n_e_fld.type()&T_SELECT)
    {
	if( cmd==TCntrNode::Get )	TCntrNode::ctrSetS(fld,cfg(elem).getSEL());	
    	else if( cmd==TCntrNode::Set )	cfg(elem).setSEL(TCntrNode::ctrGetS(fld));
    }
    else if(n_e_fld.type()&T_STRING)
    {
	if( cmd==TCntrNode::Get )	TCntrNode::ctrSetS(fld,cfg(elem).getS());
    	else if( cmd==TCntrNode::Set )	cfg(elem).setS(TCntrNode::ctrGetS(fld));
    }		
    else if(n_e_fld.type()&(T_DEC|T_OCT|T_HEX))
    {
	if( cmd==TCntrNode::Get )	TCntrNode::ctrSetI(fld,cfg(elem).getI());
 	else if( cmd==TCntrNode::Set )	cfg(elem).setI(TCntrNode::ctrGetI(fld));
    }
    else if(n_e_fld.type()&T_REAL) 	
    {    
	if( cmd==TCntrNode::Get )	TCntrNode::ctrSetR(fld,cfg(elem).getR());
       	else if( cmd==TCntrNode::Set )	cfg(elem).setR(TCntrNode::ctrGetR(fld));
    }
    else if(n_e_fld.type()&T_BOOL)
    {
	if( cmd==TCntrNode::Get )	TCntrNode::ctrSetB(fld,cfg(elem).getB());
	else if( cmd==TCntrNode::Set )	cfg(elem).setB(TCntrNode::ctrGetB(fld));
    }
}


//*************************************************
//**************** TCfg ***************************
//*************************************************
TCfg::TCfg( TFld &fld, TConfig &owner ) : m_view(true), m_owner(owner)
{
    //Chek for self field for dinamic elements
    if( fld.type()&F_SELF )
    {
	m_fld = new TFld();
	*m_fld = fld;
    }
    else m_fld = &fld;
    
    if( m_fld->type()&T_STRING )
    {
	m_val.s_val    = new string;
	*(m_val.s_val) = m_fld->def();
    }
    else if( m_fld->type()&(T_DEC|T_OCT|T_HEX) )	
	m_val.i_val    = atoi(m_fld->def().c_str());
    else if( m_fld->type()&T_REAL )	
	m_val.r_val    = atof(m_fld->def().c_str());
    else if( m_fld->type()&T_BOOL )
	if( m_fld->def() == "true") m_val.b_val = true; else m_val.b_val = false;
}

TCfg::~TCfg(  )
{
    if( m_fld->type()&T_STRING ) delete m_val.s_val;
    if( m_fld->type()&F_SELF )   delete m_fld;    
}

const string &TCfg::name()
{
    return( m_fld->name() );
}

string TCfg::getSEL( )
{
    if( !(m_fld->type()&T_SELECT) )   
	throw TError("Element type no select: %s!",name().c_str());

    if( m_fld->type()&T_STRING ) 	return m_fld->selVl2Nm(*m_val.s_val);
    else if( m_fld->type()&(T_DEC|T_OCT|T_HEX) )	return m_fld->selVl2Nm(m_val.i_val);
    else if( m_fld->type()&T_REAL )	return m_fld->selVl2Nm(m_val.r_val);
    else if( m_fld->type()&T_BOOL )	return m_fld->selVl2Nm(m_val.b_val);
    else throw TError("(%s) Select error!",__func__); 
}

string &TCfg::getS( )
{
    if( !(m_fld->type()&T_STRING) )   
	throw TError("Element type no string: %s!",name().c_str());

    return(*m_val.s_val);
}

double &TCfg::getR( )
{
    if( !(m_fld->type()&T_REAL) ) 
	throw TError("Element type no real: %s!",name().c_str());
    
    return(m_val.r_val);
}

int &TCfg::getI( )
{
    if( !(m_fld->type()&(T_DEC|T_OCT|T_HEX)) )   
       	throw TError("Element type no int: %s!",name().c_str());       
    
    return(m_val.i_val);
}

bool &TCfg::getB( )
{
    if( !(m_fld->type()&T_BOOL) ) 
	throw TError("Element type no boolean: %s!",name().c_str());
    
    return(m_val.b_val);
}

void TCfg::setSEL( const string &val )
{
    if( !(m_fld->type()&T_SELECT) ) 
	throw TError("Element type no select: %s!",name().c_str());

    if( m_fld->type()&T_STRING ) 	setS( m_fld->selNm2VlS(val) );
    else if( m_fld->type()&(T_DEC|T_OCT|T_HEX) )	setI( m_fld->selNm2VlI(val) );
    else if( m_fld->type()&T_REAL )	setR( m_fld->selNm2VlR(val) );
    else if( m_fld->type()&T_BOOL )	setB( m_fld->selNm2VlB(val) ); 
    else 				throw TError("(%s) Select error!",__func__); 
}

void TCfg::setS( const string &val )
{
    //if( m_fld->type()&F_NWR ) throw TError("(%s) No write access to <%s>!",o_name,name().c_str());
    if( !(m_fld->type()&T_STRING) )  
	throw TError("Element type no string: %s!",name().c_str());
    
    if( m_fld->type()&T_SELECT ) m_fld->selVl2Nm(val);       //check selectable
    if( m_fld->type()&F_PREV )
    {
	string t_str = *(m_val.s_val);
	*(m_val.s_val) = val;    
	if( !m_owner.cfgChange(*this) ) 
	    *(m_val.s_val) = t_str;
    }
    else *(m_val.s_val) = val;    
}

void TCfg::setR( double val )
{
    //if( m_fld->type()&F_NWR )  throw TError("(%s) No write access to <%s>!",o_name,name().c_str());
    if( !(m_fld->type()&T_REAL) ) 
	throw TError("Element type no real: %s!",name().c_str());
    
    if( m_fld->type()&T_SELECT )	m_fld->selVl2Nm(val);       //check selectable
    else if( m_fld->selValR()[0] < m_fld->selValR()[1] )
    {
	if( val < m_fld->selValR()[0] )		val = m_fld->selValR()[0];
	else if( val > m_fld->selValR()[1] )	val = m_fld->selValR()[1];
    }
    if( m_fld->type()&F_PREV )
    {
	double t_val = m_val.r_val;
       	m_val.r_val = val;
	if( !m_owner.cfgChange(*this) ) 
	    m_val.r_val = t_val;
    }
    else m_val.r_val = val;
}

void TCfg::setI( int val )
{
    //if( m_fld->type()&F_NWR )  throw TError("(%s) No write access to <%s>!",o_name,name().c_str());
    if( !(m_fld->type()&(T_DEC|T_OCT|T_HEX)) )  
	throw TError("Element type no int: %s!",name().c_str());
    
    if( m_fld->type()&T_SELECT ) m_fld->selVl2Nm(val);       //check selectable
    else if( m_fld->selValI()[0] < m_fld->selValI()[1] )
    {        
	if( val < m_fld->selValI()[0] ) 	val = m_fld->selValI()[0];
	else if( val > m_fld->selValI()[1] ) 	val = m_fld->selValI()[1];
    }
    if( m_fld->type()&F_PREV )
    {
	int t_val = m_val.i_val;
       	m_val.i_val = val;
	if( !m_owner.cfgChange(*this) ) 
	    m_val.i_val = t_val;
    }
    else m_val.i_val = val;
}

void TCfg::setB( bool val )
{
    //if( m_fld->type()&F_NWR )  throw TError("(%s) No write access to <%s>!",o_name,name().c_str());
    if( !(m_fld->type()&T_BOOL) ) 
	throw TError("Element type no boolean: %s!",name().c_str());
    
    if( m_fld->type()&T_SELECT )	m_fld->selVl2Nm(val);       //check selectable
    if( m_fld->type()&F_PREV )
    {
	bool t_val = m_val.b_val;
       	m_val.b_val = val;
	if( !m_owner.cfgChange(*this) ) 
	    m_val.b_val = t_val;
    }
    else m_val.b_val = val;
}

bool TCfg::operator==(TCfg & cfg)
{
    if( fld().type()&T_STRING && cfg.fld().type()&T_STRING && getS() == cfg.getS() )	return(true);
    else if( (fld().type()&(T_DEC|T_OCT|T_HEX)) && 
	    (cfg.fld().type()&(T_DEC|T_OCT|T_HEX)) && getI() == cfg.getI()) 		return(true);
    else if( fld().type()&T_REAL && cfg.fld().type()&T_REAL && getR() == cfg.getR()) 	return(true);
    else if( fld().type()&T_BOOL && cfg.fld().type()&T_BOOL && getB() == cfg.getB()) 	return(true);

    return(false);
}

TCfg &TCfg::operator=(TCfg & cfg)
{
    if( fld().type()&T_STRING && cfg.fld().type()&T_STRING )
	setS( cfg.getS() );
    else if( (fld().type()&(T_DEC|T_OCT|T_HEX)) && 
	    (cfg.fld().type()&(T_DEC|T_OCT|T_HEX)) )
	setI( cfg.getI() );
    else if( fld().type()&T_REAL )
	setR( cfg.getR() );
    else if( fld().type()&T_BOOL )
	setB( cfg.getB() );

    return *this;
}

