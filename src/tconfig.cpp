
//OpenSCADA system file: tconfig.cpp
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

using namespace OSCADA;

//*************************************************
//* TConfig                                       *
//*************************************************
TConfig::TConfig( TElem *Elements ) : m_elem(NULL), mNoTransl(false)
{
    setElem(Elements,true);
}

TConfig::~TConfig()
{
    //> Deinit value
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
    if( p==value.end() ) return;
    delete p->second;
    value.erase(p);
}

TCfg &TConfig::cfg( const string &n_val )
{
    TCfgMap::iterator p=value.find(n_val);
    if( p==value.end() ) throw TError("TConfig",_("Attribute '%s' is not present!"),n_val.c_str());
    return *p->second;
}

TCfg *TConfig::at( const string &n_val, bool noExpt )
{
    TCfgMap::iterator p=value.find(n_val);
    if( p != value.end() ) return p->second;
    if( noExpt ) return NULL;
    throw TError("TConfig",_("Attribute '%s' is not present!"),n_val.c_str());
}

void TConfig::cfgList( vector<string> &list )
{
    list.clear();
    if(m_elem)	m_elem->fldList(list);
}

bool TConfig::cfgPresent( const string &n_val )
{
    TCfgMap::iterator p=value.find(n_val);
    if( p==value.end() ) return false;
    return true;
}

void TConfig::cfgViewAll( bool val )
{
    for( TCfgMap::iterator p = value.begin(); p != value.end(); ++p )
	p->second->setView(val);
}

void TConfig::cfgKeyUseAll( bool val )
{
    for( TCfgMap::iterator p = value.begin(); p != value.end(); ++p )
	if( p->second->fld().flg()&TCfg::Key )
	    p->second->setKeyUse(val);
}

void TConfig::setElem(TElem *Elements, bool first)
{
    if( m_elem == Elements && !first ) return;

    //> Clear previos setting
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

    //> Set new setting
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
    for( unsigned i=0; i < m_elem->fldSize(); i++ )
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
    if(elem.size() > 4 && elem.substr(0,4) == "sel_" && TCntrNode::ctrChkNode(opt))
    {
	TFld &n_e_fld = cfg(elem.substr(4)).fld();
	for( unsigned i_a=0; i_a < n_e_fld.selNm().size(); i_a++ )
	    opt->childAdd("el")->setText(n_e_fld.selNm()[i_a]);
	return;
    }
    TCfg &cel = cfg(elem);
    if(TCntrNode::ctrChkNode(opt,"get",(cel.fld().flg()&TFld::NoWrite)?(perm&~0222):perm,user.c_str(),grp.c_str(),SEC_RD))
    {
	if(cel.fld().flg()&TFld::Selected)	opt->setText(cel.getSEL());
	else					opt->setText(cel.getS());
    }
    if(TCntrNode::ctrChkNode(opt,"set",(cel.fld().flg()&TFld::NoWrite)?(perm&~0222):perm,user.c_str(),grp.c_str(),SEC_WR))
    {
	if(cel.fld().flg()&TFld::Selected)	cel.setSEL(opt->text());
	else					cel.setS(opt->text());
    }
}

//*************************************************
//* TCfg                                          *
//*************************************************
TCfg::TCfg( TFld &fld, TConfig &owner ) : mView(true), mKeyUse(false), mOwner(owner), mNoTransl(false)
{
    //> Chek for self field for dinamic elements
    if( fld.flg()&TFld::SelfFld )
    {
	mFld = new TFld();
	*mFld = fld;
    }
    else mFld = &fld;

    switch(mFld->type())
    {
	case TFld::String:
	    m_val.s_val    = new ResString();
	    m_val.s_val->setVal(mFld->def());
	    break;
	case TFld::Integer:	m_val.i_val = atoi(mFld->def().c_str());	break;
	case TFld::Real:	m_val.r_val = atof(mFld->def().c_str());	break;
	case TFld::Boolean:	m_val.b_val = atoi(mFld->def().c_str());	break;
    }
    if( fld.flg()&TCfg::Hide )	mView = false;
}

TCfg::~TCfg(  )
{
    if( mFld->type() == TFld::String )	delete m_val.s_val;
    if( mFld->flg()&TFld::SelfFld )	delete mFld;
}

const string &TCfg::name()	{ return mFld->name(); }

string &TCfg::getSd( )
{
    if( mFld->type()!=TFld::String )	throw TError("Cfg",_("Element type is not string!"));
    return m_val.s_val->str;
}

double &TCfg::getRd( )
{
    if( mFld->type()!=TFld::Real )	throw TError("Cfg",_("Element type is not real!"));
    return m_val.r_val;
}

int &TCfg::getId( )
{
    if( mFld->type()!=TFld::Integer )	throw TError("Cfg",_("Element type is not int!"));
    return m_val.i_val;
}

bool &TCfg::getBd( )
{
    if( mFld->type()!=TFld::Boolean )	throw TError("Cfg",_("Element type is not boolean!"));
    return m_val.b_val;
}

string TCfg::getSEL( char RqFlg )
{
    if(!(mFld->flg()&TFld::Selected))	throw TError("Cfg",_("Element type is not selected!"));
    switch(mFld->type())
    {
	case TFld::String:	return mFld->selVl2Nm(getS(RqFlg));
	case TFld::Integer:	return mFld->selVl2Nm(getI(RqFlg));
	case TFld::Real:	return mFld->selVl2Nm(getR(RqFlg));
	case TFld::Boolean:	return mFld->selVl2Nm(getB(RqFlg));
    }
    return "";
}

string TCfg::getS( char RqFlg )
{
    switch(mFld->type())
    {
	case TFld::Integer:	return TSYS::int2str(getI(RqFlg));
	case TFld::Real:	return TSYS::real2str(getR(RqFlg));
	case TFld::Boolean:	return TSYS::int2str(getB(RqFlg));
	case TFld::String:	return m_val.s_val->getVal();
    }
    return "";
}

double TCfg::getR( char RqFlg )
{
    switch(mFld->type())
    {
	case TFld::String:	return atof(getS(RqFlg).c_str());
	case TFld::Integer:	return getI(RqFlg);
	case TFld::Boolean:	return getB(RqFlg);
	case TFld::Real:	return m_val.r_val;
    }
    return 0;
}

int TCfg::getI( char RqFlg )
{
    switch(mFld->type())
    {
	case TFld::String:	return atoi(getS(RqFlg).c_str());
	case TFld::Real:	return (int)getR(RqFlg);
	case TFld::Boolean:	return getB(RqFlg);
	case TFld::Integer:	return m_val.i_val;
    }
    return 0;
}

bool TCfg::getB( char RqFlg )
{
    switch(mFld->type())
    {
	case TFld::String:	return atoi(getS(RqFlg).c_str());
	case TFld::Integer:	return getI(RqFlg);
	case TFld::Real:	return (int)getR(RqFlg);
	case TFld::Boolean:	return m_val.b_val;
    }
    return false;
}

void TCfg::setSEL( const string &val, char RqFlg )
{
    if( !(mFld->flg()&TFld::Selected) ) throw TError("Cfg",_("Element type is not selected!"));
    switch( mFld->type() )
    {
	case TFld::String:	setS( mFld->selNm2VlS(val), RqFlg );	break;
	case TFld::Integer:	setI( mFld->selNm2VlI(val), RqFlg );	break;
	case TFld::Real:	setR( mFld->selNm2VlR(val), RqFlg );	break;
	case TFld::Boolean:	setB( mFld->selNm2VlB(val), RqFlg );	break;
    }
}

void TCfg::setS( const string &val, char RqFlg )
{
    switch( mFld->type() )
    {
	case TFld::Integer:	setI( atoi(val.c_str()), RqFlg );	break;
	case TFld::Real:	setR( atof(val.c_str()), RqFlg );	break;
	case TFld::Boolean:	setB( atoi(val.c_str()), RqFlg );	break;
	case TFld::String:
	{
	    string t_str = m_val.s_val->getVal();
	    m_val.s_val->setVal(val);
	    if( !mOwner.cfgChange(*this) ) m_val.s_val->setVal(t_str);
	    if( RqFlg&TCfg::ForceUse )	{ setView(true); setKeyUse(true); }
	    break;
	}
    }
}

void TCfg::setR( double val, char RqFlg )
{
    switch( mFld->type() )
    {
	case TFld::String:	setS( TSYS::real2str(val), RqFlg );	break;
	case TFld::Integer:	setI( (int)val, RqFlg );	break;
	case TFld::Boolean:	setB( val, RqFlg );	break;
	case TFld::Real:
	{
	    if( !(mFld->flg()&TFld::Selected) && mFld->selValR()[0] < mFld->selValR()[1] )
		val = vmin(mFld->selValR()[1],vmax(mFld->selValR()[0],val));
	    double t_val = m_val.r_val;
	    m_val.r_val = val;
	    if( !mOwner.cfgChange(*this) )	m_val.r_val = t_val;
	    if( RqFlg&TCfg::ForceUse )	{ setView(true); setKeyUse(true); }
	    break;
	}
    }
}

void TCfg::setI( int val, char RqFlg )
{
    switch( mFld->type() )
    {
	case TFld::String:	setS( TSYS::int2str(val), RqFlg );	break;
	case TFld::Real:	setR( val, RqFlg );	break;
	case TFld::Boolean:	setB( val, RqFlg );	break;
	case TFld::Integer:
	{
	    if( !(mFld->flg()&TFld::Selected) && mFld->selValI()[0] < mFld->selValI()[1] )
		val = vmin(mFld->selValI()[1],vmax(mFld->selValI()[0],val));
	    int t_val = m_val.i_val;
	    m_val.i_val = val;
	    if( !mOwner.cfgChange(*this) )	m_val.i_val = t_val;
	    if( RqFlg&TCfg::ForceUse )	{ setView(true); setKeyUse(true); }
	    break;
	}
    }
}

void TCfg::setB( bool val, char RqFlg )
{
    switch( mFld->type() )
    {
	case TFld::String:	setS( TSYS::int2str(val), RqFlg );	break;
	case TFld::Integer:	setI( val, RqFlg );	break;
	case TFld::Real:	setR( val, RqFlg );	break;
	case TFld::Boolean:
	{
	    bool t_val = m_val.b_val;
	    m_val.b_val = val;
	    if( !mOwner.cfgChange(*this) )	m_val.b_val = t_val;
	    if( RqFlg&TCfg::ForceUse )	{ setView(true); setKeyUse(true); }
	    break;
	}
    }
}

bool TCfg::operator==( TCfg &cfg )
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
