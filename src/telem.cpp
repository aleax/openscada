
//OpenSCADA system file: telem.cpp
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include "tmess.h"
#include "tbds.h"
#include "telem.h"

using namespace OSCADA;

//*************************************************
//* TElem                                         *
//*************************************************
TElem::TElem( const string &name ) : m_name(name)
{

}

TElem::~TElem( )
{
    while(cont.size())	cont[0]->detElem(this);
    fldClear();
}

int TElem::fldAdd( TFld *fld, int id )
{
    ResAlloc res(mResEl, false);

    //Find dublicates
    for(unsigned i_f = 0; i_f < elem.size(); i_f++)
	if(elem[i_f]->name() == fld->name())
	{ delete fld; return i_f; }
    res.request(true);
    if(id > (int)elem.size() || id < 0) id = elem.size();
    elem.insert(elem.begin()+id,fld);

    //Add value and set them default
    res.request(false);
    for(unsigned cfg_i = 0; cfg_i < cont.size(); cfg_i++)
	cont[cfg_i]->addFld(this, id);

    return id;
}

void TElem::fldDel( unsigned int id )
{
    ResAlloc res(mResEl, true);
    if(id >= elem.size()) throw TError("Elem",_("Id error!"));
    res.request(false);
    for(unsigned cfg_i = 0; cfg_i < cont.size(); cfg_i++)
	cont[cfg_i]->delFld(this, id);
    res.request(true);
    delete elem[id];
    elem.erase(elem.begin()+id);
}

void TElem::valAtt( TValElem *cnt )
{
    ResAlloc res(mResEl, true);
    for(unsigned i = 0; i < cont.size(); i++)
	if(cont[i] == cnt) throw TError("Elem",_("The element container is already attached!"));
    cont.push_back(cnt);
}

void TElem::valDet( TValElem *cnt )
{
    ResAlloc res(mResEl, true);
    for(unsigned i = 0; i < cont.size(); i++)
	if(cont[i] == cnt) { cont.erase(cont.begin()+i); break; }
}

unsigned TElem::fldId( const string &name, bool noex )
{
    ResAlloc res(mResEl, false);
    for(unsigned i = 0; i < elem.size(); i++)
	if(elem[i]->name() == name) return i;
    if(noex) return fldSize();
    throw TError("Elem",_("Element '%s' is not present!"),name.c_str());
}

bool TElem::fldPresent( const string &name )
{
    ResAlloc res(mResEl, false);
    for(unsigned i = 0; i < elem.size(); i++)
	if(elem[i]->name() == name) return true;
    return false;
}

void TElem::fldList( vector<string> &list )
{
    list.clear();
    ResAlloc res(mResEl, false);
    for(unsigned i = 0; i < elem.size(); i++)
	list.push_back(elem[i]->name());
}

TFld &TElem::fldAt( unsigned int id )
{
    if(id >= elem.size()) throw TError("Elem",_("Id error!"));
    return *elem[id];
}

void TElem::fldClear( )
{
    while(elem.size())	fldDel(0);
}

//*************************************************
//* TFld - field of element                       *
//*************************************************
TFld::TFld( ) : m_type(TFld::Integer), m_flg(0)
{
    m_sel = NULL;
    m_val.s = NULL;
}

TFld::TFld( TFld &ifld, const char *name ) : m_len(0), m_dec(0), m_type(TFld::Integer), m_flg(0)
{
    m_sel	= NULL;
    m_val.s	= NULL;

    m_name	= name ? string(name) : ifld.name();
    m_descr	= ifld.descr();
    m_type	= ifld.type();
    m_flg	= ifld.flg();
    m_def	= ifld.def();
    m_res	= ifld.reserve();
    m_len	= ifld.len();
    m_dec	= ifld.dec();

    setValues(ifld.values());
    setSelNames(ifld.selNames());
}

TFld::TFld( const char *name, const char *descr, TFld::Type itype, unsigned iflg,
            const char *valLen, const char *valDef, const char *val_s, const char *n_Sel, const char *ires ) :
    m_type(TFld::Integer), m_flg(0)
{
    m_sel   = NULL;
    m_val.s = NULL;

    m_name  = name;
    m_descr = descr;
    m_type  = itype;
    m_flg   = iflg;
    m_def   = valDef;
    m_res   = ires;

    int ilen = 0, idec = 0;
    sscanf(valLen,"%d.%d",&ilen,&idec);
    m_len = ilen;
    m_dec = idec;

    setValues(val_s);
    setSelNames(n_Sel);
}

TFld::~TFld( )
{
    if(m_sel)	delete m_sel;
    if(m_val.s != NULL)
	switch(type()) {
	    case TFld::String:	delete m_val.s;	break;
	    case TFld::Integer:	delete m_val.i;	break;
	    case TFld::Real:	delete m_val.r;	break;
	    case TFld::Boolean:	delete m_val.b;	break;
	    default: break;
	}
}

TFld::Type TFld::type( IO::Type tp )
{
    switch(tp) {
	case IO::String:  return String;
	case IO::Integer: return Integer;
	case IO::Real:    return Real;
	case IO::Boolean: return Boolean;
	case IO::Object:  return Object;
	default: break;
    }
    return String;
}

IO::Type TFld::typeIO( )
{
    switch(type()) {
	case Boolean:	return IO::Boolean;
	case Integer:	return IO::Integer;
	case Real:	return IO::Real;
	case String:	return IO::String;
	case Object:	return IO::Object;
	default: break;
    }
    return IO::String;
}

void TFld::setFlg( unsigned iflg )
{
    unsigned ch_flg = iflg^flg();
    if(ch_flg&SelfFld)	iflg = iflg^(ch_flg&SelfFld);
    if(ch_flg&Selected)	iflg = iflg^(ch_flg&Selected);
    m_flg = iflg;
}

string TFld::values( )
{
    if(m_val.s == NULL) return "";

    string rez;
    switch(type()) {
	case TFld::String:
	    for(unsigned i_el = 0; i_el < m_val.s->size(); i_el++)
		rez = rez + (*m_val.s)[i_el] + ";";
	    break;
	case TFld::Integer:
	    if(flg()&TFld::Selected || (m_val.i->size() == 2 && (*m_val.i)[0] < (*m_val.i)[1]))
		for(unsigned i_el = 0; i_el < m_val.i->size(); i_el++)
		    rez = rez + i2s((*m_val.i)[i_el]) + ";";
	    break;
	case TFld::Real:
	    if(flg()&TFld::Selected || (m_val.i->size() == 2 && (*m_val.i)[0] < (*m_val.i)[1]))
		for(unsigned i_el = 0; i_el < m_val.r->size(); i_el++)
		    rez = rez + r2s((*m_val.r)[i_el],6) + ";";
	    break;
	case TFld::Boolean:
	    for(unsigned i_el = 0; i_el < m_val.b->size(); i_el++)
		rez = rez + i2s((*m_val.b)[i_el]) + ";";
	    break;
	default: break;
    }
    return rez.size()?rez.substr(0,rez.size()-1):"";
}

string TFld::selNames( )
{
    if(m_sel == NULL) return "";

    string rez;
    for(unsigned i_el = 0; i_el < m_sel->size(); i_el++)
	rez = rez+(*m_sel)[i_el]+";";

    return rez.size()?rez.substr(0,rez.size()-1):"";
}

void TFld::setValues( const string &vls )
{
    //Set value list
    if(flg()&TFld::Selected) {
	//Count alements amount
	int i_lvl = 0, i_off = 0;
	while(TSYS::strSepParse(vls,0,';',&i_off).size()) i_lvl++;

	switch(type()) {
	    case TFld::String:
		if(!m_val.s)	m_val.s = new vector<string>;
		m_val.s->resize(i_lvl,"");
		break;
	    case TFld::Integer:
		if(!m_val.i)	m_val.i = new vector<int>;
		m_val.i->resize(i_lvl,0);
		break;
	    case TFld::Real:
		if(!m_val.r)	m_val.r = new vector<double>;
		m_val.r->resize(i_lvl,0);
		break;
	    case TFld::Boolean:
		if(!m_val.b)	m_val.b = new vector<bool>;
		m_val.b->resize(i_lvl,false);
		break;
	    default: break;
	}
	//Get elements
	for(int i = 0, i_off=0; i < i_lvl; i++) {
	    string s_el = TSYS::strSepParse(vls,0,';',&i_off);
	    switch(type()) {
		case TFld::String:  (*m_val.s)[i] = s_el;		break;
		case TFld::Integer: (*m_val.i)[i] = strtol(s_el.c_str(),NULL,(flg()&HexDec)?16:((flg()&OctDec)?8:10));	break;
		case TFld::Real:    (*m_val.r)[i] = s2r(s_el);	break;
		case TFld::Boolean: (*m_val.b)[i] = s2i(s_el);	break;
		default: break;
	    }
	}
    }
    else
	switch(type()) {
	    case TFld::Integer:
		if(!m_val.i)	m_val.i = new vector<int>;
		m_val.i->resize(2,0);
		(*m_val.i)[0] = strtol(TSYS::strSepParse(vls,0,';').c_str(),NULL,(flg()&HexDec)?16:((flg()&OctDec)?8:10));
		(*m_val.i)[1] = strtol(TSYS::strSepParse(vls,1,';').c_str(),NULL,(flg()&HexDec)?16:((flg()&OctDec)?8:10));
		break;
	    case TFld::Real:
		if(!m_val.r)	m_val.r = new vector<double>;
		m_val.r->resize(2,0);
		(*m_val.r)[0] = s2r(TSYS::strSepParse(vls,0,';'));
		(*m_val.r)[1] = s2r(TSYS::strSepParse(vls,1,';'));
		break;
	    default: break;
	}
}

void TFld::setSelNames( const string &slnms )
{
    //Set value list
    if(!(flg()&TFld::Selected)) return;

    int i_lvl = 0;
    for(int i_off = 0; TSYS::strSepParse(slnms,0,';',&i_off).size(); i_lvl++);

    if(!m_sel)  m_sel = new vector<string>;
    m_sel->resize(i_lvl,"");

    for(int i = 0, i_off = 0; i < i_lvl; i++)
	(*m_sel)[i] = TSYS::strSepParse(slnms,0,';',&i_off);
}

const vector<string> &TFld::selValS( )
{
    if(flg()&TFld::Selected && type() == TFld::String) return *m_val.s;
    throw TError("Field",_("Field is not String."));
}

const vector<int> &TFld::selValI( )
{
    if(type() == TFld::Integer) return *m_val.i;
    throw TError("Field",_("Field is not Integer."));
}

const vector<double> &TFld::selValR( )
{
    if(type() == TFld::Real) return *m_val.r;
    throw TError("Field",_("Field is not Real."));
}

const vector<bool> &TFld::selValB( )
{
    if(flg()&TFld::Selected && type() == TFld::Boolean) return *m_val.b;
    throw TError("Field",_("Field is not Boolean."));
}

const vector<string> &TFld::selNm( )
{
    if(m_sel && flg()&TFld::Selected) return *m_sel;
    throw TError("Field",_("Field is not select type!"));
}

TFld &TFld::operator=( TFld &fld )
{
    //Free old
    if(m_sel)	delete m_sel;
    if(m_val.s != NULL)
	switch(type()) {
	    case TFld::String:	delete m_val.s;	break;
	    case TFld::Integer:	delete m_val.i;	break;
	    case TFld::Real:	delete m_val.r;	break;
	    case TFld::Boolean:	delete m_val.b;	break;
	    default: break;
	}
    //Create new
    m_name	= fld.name();
    m_descr	= fld.descr();
    m_len	= fld.len();
    m_flg	= fld.flg();
    m_type	= fld.type();
    m_def	= fld.def();
    //m_vals  = fld.vals();

    //Copy select and values border
    if(flg()&TFld::Selected) {
	m_sel  = new vector<string>;
	*m_sel = fld.selNm();
	switch(type()) {
	    case TFld::String:	m_val.s = new vector<string>;	*(m_val.s) = fld.selValS();	break;
	    case TFld::Integer:	m_val.i = new vector<int>;	*(m_val.i) = fld.selValI();	break;
	    case TFld::Real:	m_val.r = new vector<double>;	*(m_val.r) = fld.selValR();	break;
	    case TFld::Boolean:	m_val.b = new vector<bool>;	*(m_val.b) = fld.selValB();	break;
	    default: break;
	}
    }
    else
	switch(type()) {
	    case TFld::Integer:	m_val.i = new vector<int>;	*(m_val.i) = fld.selValI();	break;
	    case TFld::Real:	m_val.r = new vector<double>;	*(m_val.r) = fld.selValR();	break;
	    default: break;
	}
    return *this;
}

string TFld::selVl2Nm( const string &val )
{
    if(flg()&TFld::Selected && type() == TFld::String) {
	int sz = vmin(m_sel->size(), m_val.s->size());
	if(!sz) return _("Empty");
	int i_val = 0;
	for(i_val = 0; i_val < sz; i_val++)
	    if((*m_val.s)[i_val] == val) break;
	if(i_val >= sz) return val;
	return (*m_sel)[i_val];
    }
    throw TError("Field",_("Select error! Val: '%s'."), val.c_str());
}

string TFld::selVl2Nm( int64_t val )
{
    if(flg()&TFld::Selected && type() == TFld::Integer) {
	int sz = vmin(m_sel->size(), m_val.i->size());
	if(!sz) return _("Empty");
	int i_val = 0;
	for(i_val = 0; i_val < sz; i_val++)
	    if((*m_val.i)[i_val] == val) break;
	if(i_val >= sz) return i2s(val);
	return (*m_sel)[i_val];
    }
    throw TError("Field",_("Select error! Val: '%d'."), val);
}

string TFld::selVl2Nm( double val )
{
    if(flg()&TFld::Selected && type() == TFld::Real) {
	int sz = vmin(m_sel->size(), m_val.r->size());
	if(!sz) return _("Empty");
	int i_val = 0;
	for(i_val = 0; i_val < sz; i_val++)
	    if((*m_val.r)[i_val] == val) break;
	if(i_val >= sz) return r2s(val);
	return (*m_sel)[i_val];
    }
    throw TError("Field",_("Select error! Val: '%f'."), val);
}

string TFld::selVl2Nm( bool val )
{
    if(flg()&TFld::Selected && type() == TFld::Boolean) {
	int sz = vmin(m_sel->size(), m_val.b->size());
	if(!sz) return _("Empty");
	int i_val;
	for(i_val = 0; i_val < sz; i_val++)
	    if((*m_val.b)[i_val] == val) break;
	if(i_val >= sz) return i2s(val);
	return (*m_sel)[i_val];
    }
    throw TError("Field",_("Select error! Val: '%d'."), val);
}

string TFld::selNm2VlS( const string &name )
{
    if(flg()&TFld::Selected && type() == TFld::String)
	for(unsigned i_val = 0; i_val < vmin(m_sel->size(), m_val.s->size()); i_val++)
	    if(name == (*m_sel)[i_val])
		return (*m_val.s)[i_val];
    return name;
    //throw TError("Field",_("Select error! Name: '%s'."),name.c_str());
}

int64_t TFld::selNm2VlI( const string &name )
{
    if(flg()&TFld::Selected && type() == TFld::Integer)
	for(unsigned i_val = 0; i_val < vmin(m_sel->size(), m_val.i->size()); i_val++)
	    if(name == (*m_sel)[i_val])
		return (*m_val.i)[i_val];
    return s2i(name);
    //throw TError("Field",_("Select error! Name: '%s'."),name.c_str());
}

double TFld::selNm2VlR( const string &name )
{
    if(flg()&TFld::Selected && type() == TFld::Real)
	for(unsigned i_val = 0; i_val < vmin(m_sel->size(), m_val.r->size()); i_val++)
	    if(name == (*m_sel)[i_val])
		return (*m_val.r)[i_val];
    return s2r(name);
    //throw TError("Field",_("Select error! Name: '%s'."),name.c_str());
}

bool TFld::selNm2VlB( const string &name )
{
    if(flg()&TFld::Selected && type() == TFld::Boolean)
	for(unsigned i_val = 0; i_val < vmin(m_sel->size(), m_val.b->size()); i_val++)
	    if(name == (*m_sel)[i_val])
		return (*m_val.b)[i_val];
    return s2i(name);
    //throw TError("Field",_("Select error! Name: '%s'."),name.c_str());
}

XMLNode *TFld::cntrCmdMake( XMLNode *opt, const string &path, int pos, const string &user, const string &grp, int perm )
{
    XMLNode *n_e = TCntrNode::ctrMkNode("fld",opt,pos,(path+"/"+name()).c_str(),descr(),
	    (flg()&TFld::NoWrite)?(perm&~0222):perm,user.c_str(),grp.c_str(),1,"len",i2s(len()).c_str());
    if(n_e) {
	if(flg()&TFld::Selected)
	    n_e->setAttr("tp","str")->setAttr("len","")->setAttr("dest",(flg()&TFld::SelEdit)?"sel_ed":"select")->
		setAttr("sel_id",values())->setAttr("sel_list",selNames());
	else switch(type()) {
	    case TFld::String:
		n_e->setAttr("tp","str");
		if(flg()&FullText)	n_e->setAttr("cols","100")->setAttr("rows","4");
		break;
	    case TFld::Integer:	n_e->setAttr("tp",(flg()&HexDec)?"hex":((flg()&OctDec)?"oct":"dec"));	break;
	    case TFld::Real:	n_e->setAttr("tp","real");	break;
	    case TFld::Boolean:	n_e->setAttr("tp","bool");	break;
	    case TFld::Object:	n_e->setAttr("tp","str")->setAttr("cols","100")->setAttr("rows","4");	break;
	    default: break;
	}
    }
    return n_e;
}
