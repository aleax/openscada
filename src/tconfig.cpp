
//OpenSCADA system file: tconfig.cpp
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

//*************************************************
//* TConfig                                       *
//*************************************************
TConfig::TConfig( TElem *Elements ) : m_elem(NULL)
{
    setElem(Elements,true);
}

TConfig::~TConfig()
{
    //- Deinit value -
    TCfgMap::iterator p;
    while( (p=value.begin())!=value.end() )
    {
        delete p->second;
        value.erase(p);
    }

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
	    case TFld::Integer:	d_cfg.setI(s_cfg.getI());break;
	    case TFld::Boolean:	d_cfg.setB(s_cfg.getB());break;
	}
    }
    return *this;
}

void TConfig::detElem( TElem *el )
{
    if( el == m_elem )	setElem(NULL);
}

void TConfig::addFld( TElem *el, unsigned id )
{
    value.insert( std::pair<string,TCfg*>(m_elem->fldAt(id).name(),new TCfg(m_elem->fldAt(id),*this)) );
}
	    
void TConfig::delFld( TElem *el, unsigned id )
{
    TCfgMap::iterator p=value.find(m_elem->fldAt(id).name());
    if(p==value.end())	return;
    delete p->second;
    value.erase(p);
}					

TCfg &TConfig::cfg( const string &n_val )
{
    TCfgMap::iterator p=value.find(n_val);
    if(p==value.end())	throw TError("TConfig",_("Attribute '%s' no present!"),n_val.c_str());
    return *p->second;
}

void TConfig::cfgList( vector<string> &list )
{
    list.clear();
    if(m_elem)	m_elem->fldList(list);
}

bool TConfig::cfgPresent( const string &n_val )
{
    TCfgMap::iterator p=value.find(n_val);
    if(p==value.end()) return false;
    return true;
}

void TConfig::cfgViewAll( bool val )
{
    for( TCfgMap::iterator p = value.begin(); p != value.end(); p++ )
	p->second->setView(val);
}

void TConfig::setElem(TElem *Elements, bool first)
{
    if(m_elem == Elements && !first ) return;
    
    //- Clear previos setting -
    if(m_elem)
    {
	TCfgMap::iterator p;
	while( (p=value.begin())!=value.end() )
	{
    	    delete p->second;
    	    value.erase(p);
	}    
	m_elem->valDet(this);
	if(single) delete m_elem;
    }
    
    //- Set new setting -
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
	value.insert( std::pair<string,TCfg*>(m_elem->fldAt(i).name(),new TCfg(m_elem->fldAt(i),*this)) );    
}

TElem &TConfig::elem()
{
    return *m_elem;
}

void TConfig::cntrCmdMake( XMLNode *opt, const string &path, int pos, const string &user, const string &grp, int perm )
{
    vector<string> list_c;
    cfgList(list_c);    
    for( unsigned i_el = 0; i_el < list_c.size(); i_el++ )
	if( cfg(list_c[i_el]).view() )
	    cfg(list_c[i_el]).fld().cntrCmdMake(opt,path,(pos<0)?pos:pos++,user,grp,perm);
}

void TConfig::cntrCmdProc( XMLNode *opt, const string &elem, const string &user, const string &grp, int perm )
{
    if( elem.size() > 4 && elem.substr(0,4) == "sel_" && TCntrNode::ctrChkNode(opt) )
    { 
	TFld &n_e_fld = cfg(elem.substr(4)).fld();
	for( unsigned i_a=0; i_a < n_e_fld.selNm().size(); i_a++ )
	    opt->childAdd("el")->setText(n_e_fld.selNm()[i_a]);
	return;
    }
    TCfg &cel = cfg(elem);
    if( TCntrNode::ctrChkNode(opt,"get",(cel.fld().flg()&TFld::NoWrite)?(perm&~0222):perm,user.c_str(),grp.c_str(),SEQ_RD) )
    {
	if( cel.fld().flg()&TFld::Selected )	opt->setText(cel.getSEL());       	
	else 					opt->setText(cel.getS());
    }	
    if( TCntrNode::ctrChkNode(opt,"set",(cel.fld().flg()&TFld::NoWrite)?(perm&~0222):perm,user.c_str(),grp.c_str(),SEQ_WR) )
    {
	if( cel.fld().flg()&TFld::Selected )	cel.setSEL(opt->text());
	else 					cel.setS(opt->text());
    }
}

//*************************************************
//* TCfg                                          *
//*************************************************
TCfg::TCfg( TFld &fld, TConfig &owner ) : m_view(true), m_owner(owner)
{
    //- Chek for self field for dinamic elements -
    if( fld.flg()&TFld::SelfFld )
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
	case TFld::Integer:	m_val.i_val = atoi(m_fld->def().c_str());	break;
	case TFld::Real: 	m_val.r_val = atof(m_fld->def().c_str());	break;
	case TFld::Boolean:	m_val.b_val = atoi(m_fld->def().c_str());	break;
    }
}

TCfg::~TCfg(  )
{
    if( m_fld->type() == TFld::String )	delete m_val.s_val;
    if( m_fld->flg()&TFld::SelfFld )   	delete m_fld;
}

const string &TCfg::name()
{
    return m_fld->name();
}

string TCfg::getSEL( )
{
    if( !(m_fld->flg()&TFld::Selected) )   
	throw TError("Cfg",_("Element type no select!"));
    switch( m_fld->type() )
    {
	case TFld::String:	return m_fld->selVl2Nm(*m_val.s_val);
	case TFld::Integer:	return m_fld->selVl2Nm(m_val.i_val);
	case TFld::Real:	return m_fld->selVl2Nm(m_val.r_val);
	case TFld::Boolean:	return m_fld->selVl2Nm(m_val.b_val);
    }
}

string &TCfg::getSd( )
{
    if( m_fld->type()!=TFld::String )
        throw TError("Cfg",_("Element type no string!"));
	    
    return *m_val.s_val;
}

double &TCfg::getRd( )
{
    if( m_fld->type()!=TFld::Real )
	throw TError("Cfg",_("Element type no real!"));
	     
    return m_val.r_val;
}

int &TCfg::getId( )
{
    if( m_fld->type()!=TFld::Integer )
        throw TError("Cfg",_("Element type no int!"));
	    
    return m_val.i_val;
}

bool &TCfg::getBd( )
{
    if( m_fld->type()!=TFld::Boolean )
        throw TError("Cfg",_("Element type no boolean!"));
	    
    return m_val.b_val;
}

string TCfg::getS( )
{
    switch(m_fld->type())
    {
	case TFld::String:	return *m_val.s_val;
	case TFld::Integer:	return TSYS::int2str(m_val.i_val);
	case TFld::Real:	return TSYS::real2str(m_val.r_val);
	case TFld::Boolean:	return TSYS::int2str(m_val.b_val);
    }
}

double TCfg::getR( )
{
    switch(m_fld->type())
    {
	case TFld::String:	return atof(m_val.s_val->c_str());
	case TFld::Integer:	return m_val.i_val;
	case TFld::Real:	return m_val.r_val;
	case TFld::Boolean:	return m_val.b_val;
    }
}

int TCfg::getI( )
{
    switch(m_fld->type())
    {
	case TFld::String:	return atoi(m_val.s_val->c_str());
	case TFld::Integer:	return m_val.i_val;
	case TFld::Real:	return (int)m_val.r_val;
	case TFld::Boolean:	return m_val.b_val;
    }
}

bool TCfg::getB( )
{
    switch(m_fld->type())
    {
	case TFld::String:	return atoi(m_val.s_val->c_str());
	case TFld::Integer:	return m_val.i_val;
	case TFld::Real:	return (int)m_val.r_val;
	case TFld::Boolean:	return m_val.b_val;
    }
}

void TCfg::setSEL( const string &val, bool forcView )
{
    if( !(m_fld->flg()&TFld::Selected) ) 
	throw TError("Cfg",_("Element type no select!"));
    switch( m_fld->type() )
    {
	case TFld::String:      setS( m_fld->selNm2VlS(val),forcView );	break;
	case TFld::Integer:	setI( m_fld->selNm2VlI(val),forcView );	break;
	case TFld::Real:	setR( m_fld->selNm2VlR(val),forcView );	break;
	case TFld::Boolean:	setB( m_fld->selNm2VlB(val),forcView );	break;
    }
}

void TCfg::setS( const string &val, bool forcView )
{
    switch( m_fld->type() )
    {
	case TFld::String:      
	    if( m_fld->flg()&TCfg::Prevent )
	    {
		string t_str = *(m_val.s_val);
		*(m_val.s_val) = val;    
		if( !m_owner.cfgChange(*this) ) 
		    *(m_val.s_val) = t_str;
	    }	    
	    else *(m_val.s_val) = val;
	    if( forcView ) setView(true);
	    break;
	case TFld::Integer:	setI( atoi(val.c_str()),forcView );	break;
	case TFld::Real:	setR( atof(val.c_str()),forcView );	break;
	case TFld::Boolean:	setB( atoi(val.c_str()),forcView );	break;
    }
}

void TCfg::setR( double val, bool forcView )
{
    switch( m_fld->type() )
    {
	case TFld::String:	setS( TSYS::real2str(val),forcView );	break;
	case TFld::Integer:	setI( (int)val, forcView );	break;
	case TFld::Real:
	    if( !(m_fld->flg()&TFld::Selected) && m_fld->selValR()[0] < m_fld->selValR()[1] )
	    {
		if( val < m_fld->selValR()[0] )	val = m_fld->selValR()[0];
		if( val > m_fld->selValR()[1] )	val = m_fld->selValR()[1];
	    }
	    if( m_fld->flg()&TCfg::Prevent )
	    {
		double t_val = m_val.r_val;
		m_val.r_val = val;
		if( !m_owner.cfgChange(*this) ) 
		    m_val.r_val = t_val;
	    }
	    else m_val.r_val = val;
	    if( forcView ) setView(true);
	    break;
	case TFld::Boolean:	setB( val, forcView );	break;
    }
}

void TCfg::setI( int val, bool forcView )
{
    switch( m_fld->type() )
    {
	case TFld::String:	setS( TSYS::int2str(val),forcView );	break;
	case TFld::Integer:
	    if( !(m_fld->flg()&TFld::Selected) && m_fld->selValI()[0] < m_fld->selValI()[1] )
	    {        
		if( val < m_fld->selValI()[0] )	val = m_fld->selValI()[0];
		if( val > m_fld->selValI()[1] )	val = m_fld->selValI()[1];
	    }
	    if( m_fld->flg()&TCfg::Prevent )
	    {
		int t_val = m_val.i_val;
		m_val.i_val = val;
		if( !m_owner.cfgChange(*this) ) 
		    m_val.i_val = t_val;
	    }
	    else m_val.i_val = val;
	    if( forcView ) setView(true);
	    break;
	case TFld::Real:	setR( val,forcView );	break;
	case TFld::Boolean:	setB( val,forcView );	break;
    }
}

void TCfg::setB( bool val, bool forcView )
{
    switch( m_fld->type() )
    {
	case TFld::String:	setS( TSYS::int2str(val),forcView );	break;
	case TFld::Integer:	setI( val,forcView );	break;
	case TFld::Real:	setR( val,forcView );	break;
	case TFld::Boolean:
	    if( m_fld->flg()&TCfg::Prevent )
	    {
		bool t_val = m_val.b_val;
		m_val.b_val = val;
		if( !m_owner.cfgChange(*this) ) 
		    m_val.b_val = t_val;
	    }
	    else m_val.b_val = val;
	    if( forcView ) setView(true);
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
	case TFld::Integer:
	    if( cfg.fld().type()==TFld::Integer && getI() == cfg.getI() )	return true;
	    break;
	case TFld::Real:
	    if( cfg.fld().type()==TFld::Real && getR() == cfg.getR() )	return true;
	    break;
	case TFld::Boolean:
	    if( cfg.fld().type()==TFld::Boolean && getB() == cfg.getB())return true;
	    break;
    }
    return(false);
}

TCfg &TCfg::operator=(TCfg & cfg)
{
    switch(fld().type())
    {
	case TFld::String:	setS( cfg.getS() );	break;
	case TFld::Integer:	setI( cfg.getI() );	break;
	case TFld::Real:	setR( cfg.getR() );	break;
	case TFld::Boolean:	setB( cfg.getB() );	break;
    }
    return *this;
}

