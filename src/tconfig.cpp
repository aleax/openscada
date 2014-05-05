
//OpenSCADA system file: tconfig.cpp
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko                           *
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
    pthread_mutex_init(&mRes, NULL);

    setElem(Elements, true);
}

TConfig::~TConfig( )
{
    //> Deinit value
    TCfgMap::iterator p;
    while((p=value.begin()) != value.end())
    {
	delete p->second;
	value.erase(p);
    }

    m_elem->valDet(this);
    if(single) delete m_elem;

    pthread_mutex_lock(&mRes);
    pthread_mutex_destroy(&mRes);
}

TConfig &TConfig::operator=(TConfig &config)	{ return exclCopy(config); }

TConfig &TConfig::exclCopy( TConfig &config, const string &passCpLs )
{
    vector<string> list_el;

    cfgList(list_el);
    for(unsigned i_el = 0; i_el < list_el.size(); i_el++)
    {
	if(!config.cfgPresent(list_el[i_el]) || passCpLs.find(list_el[i_el]+";") != string::npos) continue;
	TCfg &s_cfg = config.cfg(list_el[i_el]);
	TCfg &d_cfg = cfg(list_el[i_el]);
	switch(d_cfg.fld().type())
	{
	    case TFld::String:	d_cfg.setS(s_cfg.getS());break;
	    case TFld::Real:	d_cfg.setR(s_cfg.getR());break;
	    case TFld::Integer:	d_cfg.setI(s_cfg.getI());break;
	    case TFld::Boolean:	d_cfg.setB(s_cfg.getB());break;
	    default: break;
	}
    }
    return *this;
}

void TConfig::detElem( TElem *el )
{
    if(el == m_elem)	setElem(NULL);
}

void TConfig::addFld( TElem *el, unsigned id )
{
    value.insert(std::pair<string,TCfg*>(m_elem->fldAt(id).name(),new TCfg(m_elem->fldAt(id),*this)));
}

void TConfig::delFld( TElem *el, unsigned id )
{
    TCfgMap::iterator p = value.find(m_elem->fldAt(id).name());
    if(p == value.end()) return;
    delete p->second;
    value.erase(p);
}

TCfg &TConfig::cfg( const string &n_val )
{
    TCfgMap::iterator p = value.find(n_val);
    if(p == value.end()) throw TError("TConfig",_("Attribute '%s' is not present!"),n_val.c_str());
    return *p->second;
}

TCfg *TConfig::at( const string &n_val, bool noExpt )
{
    TCfgMap::iterator p = value.find(n_val);
    if(p != value.end()) return p->second;
    if(noExpt) return NULL;
    throw TError("TConfig",_("Attribute '%s' is not present!"),n_val.c_str());
}

void TConfig::cfgList( vector<string> &list )
{
    list.clear();
    if(m_elem)	m_elem->fldList(list);
}

bool TConfig::cfgPresent( const string &n_val )
{
    TCfgMap::iterator p = value.find(n_val);
    if(p == value.end()) return false;
    return true;
}

void TConfig::cfgViewAll( bool val )
{
    for(TCfgMap::iterator p = value.begin(); p != value.end(); ++p)
	p->second->setView(val);
}

void TConfig::cfgKeyUseAll( bool val )
{
    for(TCfgMap::iterator p = value.begin(); p != value.end(); ++p)
	if(p->second->fld().flg()&TCfg::Key)
	    p->second->setKeyUse(val);
}

void TConfig::setElem( TElem *Elements, bool first )
{
    if(m_elem == Elements && !first) return;

    //> Clear previos setting
    if(m_elem)
    {
	TCfgMap::iterator p;
	while((p=value.begin()) != value.end())
	{
	    delete p->second;
	    value.erase(p);
	}
	m_elem->valDet(this);
	if(single) delete m_elem;
    }

    //> Set new setting
    if(!Elements)
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
	value.insert(std::pair<string,TCfg*>(m_elem->fldAt(i).name(),new TCfg(m_elem->fldAt(i),*this)));
}

TElem &TConfig::elem( )	{ return *m_elem; }

void TConfig::cntrCmdMake( XMLNode *opt, const string &path, int pos, const string &user, const string &grp, int perm )
{
    vector<string> list_c;
    cfgList(list_c);
    for(unsigned i_el = 0; i_el < list_c.size(); i_el++)
	if(cfg(list_c[i_el]).view())
	    cfg(list_c[i_el]).fld().cntrCmdMake(opt, path, (pos<0)?pos:pos++, user, grp, perm);
}

void TConfig::cntrCmdProc( XMLNode *opt, const string &elem, const string &user, const string &grp, int perm )
{
    if(elem.compare(0,4,"sel_") == 0 && TCntrNode::ctrChkNode(opt))
    {
	TFld &n_e_fld = cfg(elem.substr(4)).fld();
	for(unsigned i_a = 0; i_a < n_e_fld.selNm().size(); i_a++)
	    opt->childAdd("el")->setText(n_e_fld.selNm()[i_a]);
	return;
    }
    TCfg &cel = cfg(elem);
    if(TCntrNode::ctrChkNode(opt,"get",(cel.fld().flg()&TFld::NoWrite)?(perm&~0222):perm,user.c_str(),grp.c_str(),SEC_RD))
	opt->setText(cel.getS());
    if(TCntrNode::ctrChkNode(opt,"set",(cel.fld().flg()&TFld::NoWrite)?(perm&~0222):perm,user.c_str(),grp.c_str(),SEC_WR))
	cel.setS(opt->text());
}

TVariant TConfig::objFunc( const string &iid, vector<TVariant> &prms, const string &user )
{
    // ElTp cfg(string nm) - config variable 'nm' get.
    //  nm - config variable name.
    if(iid == "cfg" && prms.size() >= 1)
    {
	TCfg *cf = at(prms[0].getS(), true);
	if(!cf) return EVAL_REAL;
	return *cf;
    }
    // ElTp cfgSet(string nm, ElTp val) - set config variable 'nm' to 'val'.
    //  nm - config variable name;
    //  val - variable value.
    if(iid == "cfgSet" && prms.size() >= 2)
    {
	TCfg *cf = at(prms[0].getS(), true);
	if(!cf || (cf->fld().flg()&TFld::NoWrite)) return false;
	*(TVariant*)cf = prms[1];
	return true;
    }

    return TVariant();
}

//*************************************************
//* TCfg                                          *
//*************************************************
TCfg::TCfg( TFld &fld, TConfig &owner ) : mView(true), mKeyUse(false), mNoTransl(false), mOwner(owner)
{
    //> Chek for self field for dinamic elements
    if(fld.flg()&TFld::SelfFld)
    {
	mFld = new TFld();
	*mFld = fld;
    }
    else mFld = &fld;

    switch(mFld->type())
    {
	case TFld::String:	setType(TVariant::String, true);  TVariant::setS(mFld->def());	break;
	case TFld::Integer:	setType(TVariant::Integer, true); TVariant::setI(atoll(mFld->def().c_str()));	break;
	case TFld::Real:	setType(TVariant::Real, true);	  TVariant::setR(atof(mFld->def().c_str()));	break;
	case TFld::Boolean:	setType(TVariant::Boolean, true); TVariant::setB((bool)atoi(mFld->def().c_str()));break;
	default: break;
    }
    if(fld.flg()&TCfg::Hide)	mView = false;
}

TCfg::~TCfg( )
{
    if(mFld->flg()&TFld::SelfFld)	delete mFld;
}

const string &TCfg::name( )	{ return mFld->name(); }

const char *TCfg::getSd( )
{
    if(type() != TVariant::String)	throw TError("Cfg",_("Element type is not string!"));
    return (mSize < sizeof(val.sMini)) ? val.sMini : val.sPtr;
}

double &TCfg::getRd( )
{
    if(type() != TVariant::Real)	throw TError("Cfg",_("Element type is not real!"));
    return val.r;
}

int64_t &TCfg::getId( )
{
    if(type() != TVariant::Integer)	throw TError("Cfg",_("Element type is not int!"));
    return val.i;
}

char &TCfg::getBd( )
{
    if(type() != TVariant::Boolean)	throw TError("Cfg",_("Element type is not Boolean!"));
    return val.b;
}

string TCfg::getSEL( )
{
    if(!(mFld->flg()&TFld::Selected))	throw TError("Cfg",_("Element type is not selected!"));
    switch(type())
    {
	case TVariant::String:	return mFld->selVl2Nm(getS());
	case TVariant::Integer:	return mFld->selVl2Nm(getI());
	case TVariant::Real:	return mFld->selVl2Nm(getR());
	case TVariant::Boolean:	return mFld->selVl2Nm(getB());
	default: break;
    }
    return "";
}

string TCfg::getS( )
{
    pthread_mutex_lock(&mOwner.mRes);
    string rez = TVariant::getS();
    pthread_mutex_unlock(&mOwner.mRes);
    return rez;
}

void TCfg::setS( const string &val )
{
    switch(type())
    {
	case TVariant::Integer:	setI(atoll(val.c_str()));	break;
	case TVariant::Real:	setR(atof(val.c_str()));	break;
	case TVariant::Boolean:	setB((bool)atoi(val.c_str()));	break;
	case TVariant::String:
	{
	    pthread_mutex_lock(&mOwner.mRes);
	    string t_str = TVariant::getS();
	    TVariant::setS(val);
	    pthread_mutex_unlock(&mOwner.mRes);
	    try
	    {
		if(!mOwner.cfgChange(*this))
		{
		    pthread_mutex_lock(&mOwner.mRes);
		    TVariant::setS(t_str);
		    pthread_mutex_unlock(&mOwner.mRes);
		}
	    }
	    catch(TError err)
	    {
		pthread_mutex_lock(&mOwner.mRes);
		TVariant::setS(t_str);
		pthread_mutex_unlock(&mOwner.mRes);
		throw;
	    }
	    break;
	}
	default: break;
    }
}

void TCfg::setR( double val )
{
    switch(type())
    {
	case TVariant::String:	setS(r2s(val));	break;
	case TVariant::Integer:	setI((int)val);	break;
	case TVariant::Boolean:	setB((bool)val);break;
	case TVariant::Real:
	{
	    if(!(mFld->flg()&TFld::Selected) && mFld->selValR()[0] < mFld->selValR()[1])
		val = vmin(mFld->selValR()[1],vmax(mFld->selValR()[0],val));
	    double t_val = TVariant::getR();
	    TVariant::setR(val);
	    try{ if(!mOwner.cfgChange(*this)) TVariant::setR(t_val); }
	    catch(TError err) { TVariant::setR(t_val); throw; }
	    break;
	}
	default: break;
    }
}

void TCfg::setI( int64_t val )
{
    switch(type())
    {
	case TVariant::String:	setS(i2s(val));	break;
	case TVariant::Real:	setR(val);	break;
	case TVariant::Boolean:	setB((bool)val);break;
	case TVariant::Integer:
	{
	    if(!(mFld->flg()&TFld::Selected) && mFld->selValI()[0] < mFld->selValI()[1])
		val = vmin(mFld->selValI()[1],vmax(mFld->selValI()[0],val));
	    int t_val = TVariant::getI();
	    TVariant::setI(val);
	    try{ if(!mOwner.cfgChange(*this)) TVariant::setI(t_val); }
	    catch(TError err) { TVariant::setI(t_val); throw; }
	    break;
	}
	default: break;
    }
}

void TCfg::setB( char val )
{
    switch(type())
    {
	case TVariant::String:	setS(i2s(val));	break;
	case TVariant::Integer:	setI(val);	break;
	case TVariant::Real:	setR(val);	break;
	case TVariant::Boolean:
	{
	    bool t_val = TVariant::getB();
	    TVariant::setB(val);
	    try{ if(!mOwner.cfgChange(*this)) TVariant::setB(t_val); }
	    catch(TError err) { TVariant::setB(t_val); throw; }
	    break;
	}
	default: break;
    }
}

void TCfg::setSEL( const string &val, char RqFlg )
{
    if(!(mFld->flg()&TFld::Selected)) throw TError("Cfg",_("Element type is not selected!"));
    switch(type())
    {
	case TVariant::String:	setS(mFld->selNm2VlS(val), RqFlg);	break;
	case TVariant::Integer:	setI(mFld->selNm2VlI(val), RqFlg);	break;
	case TVariant::Real:	setR(mFld->selNm2VlR(val), RqFlg);	break;
	case TVariant::Boolean:	setB(mFld->selNm2VlB(val), RqFlg);	break;
	default: break;
    }
}

void TCfg::setS( const string &val, char RqFlg )
{
    setS(val);
    if(RqFlg&TCfg::ForceUse)	{ setView(true); setKeyUse(true); }
}

void TCfg::setR( double val, char RqFlg )
{
    setR(val);
    if(RqFlg&TCfg::ForceUse)	{ setView(true); setKeyUse(true); }
}

void TCfg::setI( int64_t val, char RqFlg )
{
    setI(val);
    if(RqFlg&TCfg::ForceUse)	{ setView(true); setKeyUse(true); }
}

void TCfg::setB( char val, char RqFlg )
{
    setB(val);
    if(RqFlg&TCfg::ForceUse)	{ setView(true); setKeyUse(true); }
}

bool TCfg::operator==( TCfg &cfg )
{
    if(fld().type() == cfg.fld().type())
	switch(fld().type())
	{
	    case TFld::String:	return (getS() == cfg.getS());
	    case TFld::Integer:	return (getI() == cfg.getI());
	    case TFld::Real:	return (getR() == cfg.getR());
	    case TFld::Boolean:	return (getB() == cfg.getB());
	    default: break;
	}
    return false;
}

TCfg &TCfg::operator=(TCfg & cfg)
{
    switch(type())
    {
	case TVariant::String:	setS(cfg.getS());	break;
	case TVariant::Integer:	setI(cfg.getI());	break;
	case TVariant::Real:	setR(cfg.getR());	break;
	case TVariant::Boolean:	setB(cfg.getB());	break;
	default: break;
    }
    return *this;
}
