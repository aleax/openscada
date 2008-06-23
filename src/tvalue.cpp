
//OpenSCADA system file: tvalue.cpp
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
#include "tmess.h"
#include "tarchval.h"
#include "tparamcontr.h"
#include "tcontroller.h"
#include "tvalue.h"

//*************************************************
//* TValue                                        *
//*************************************************
TValue::TValue( ) : l_cfg(0), m_cfg(NULL)
{
    m_vl = grpAdd("a_",true);
}

TValue::~TValue()
{
    while(elem.size())	vlElemDet(elem[0]);
}

void TValue::detElem( TElem *el )
{
    vlElemDet(el);
}

void TValue::addFld( TElem *el, unsigned id_val )
{
    int i_off = l_cfg; 
    for(unsigned i_e = 0; i_e < elem.size(); i_e++)
	if(elem[i_e]->elName() == el->elName() ) break;
	else i_off+=elem[i_e]->fldSize();
    chldAdd(m_vl,new TVal(el->fldAt(id_val),this),i_off+id_val);
}

void TValue::delFld( TElem *el, unsigned id_val )
{
    if( nodeMode() == TCntrNode::Enable && chldPresent(m_vl,el->fldAt(id_val).name()) ) 
	chldDel(m_vl,el->fldAt(id_val).name());
}

void TValue::setVlCfg( TConfig *cfg )
{
    vector<string> list;
    //- Detach old configs -
    if( m_cfg )
    {
	m_cfg->cfgList( list );
	for( unsigned i_cf = 0; i_cf < list.size(); i_cf++ )
	    if( !(m_cfg->cfg(list[i_cf]).fld().flg()&TCfg::NoVal) && vlPresent(list[i_cf]) )
	    {
		chldDel(m_vl,list[i_cf]);
		l_cfg--;
	    }
	m_cfg = NULL;
    }
    //- Attach new config -
    if( cfg )
    {
	cfg->cfgList( list );
	for( unsigned i_cf = 0; i_cf < list.size(); i_cf++ )
	    if( !(cfg->cfg(list[i_cf]).fld().flg()&TCfg::NoVal) && !vlPresent(list[i_cf]) )
	        chldAdd(m_vl, new TVal(cfg->cfg(list[i_cf]),this),l_cfg++);
	m_cfg = cfg;
    }
}

bool TValue::vlElemPresent( TElem *ValEl )
{
    for(int i_el = 0; i_el < elem.size(); i_el++)
	if( elem[i_el] == ValEl ) return true;
    return false;
}

void TValue::vlElemAtt( TElem *ValEl )
{
    ValEl->valAtt(this);
    for(unsigned i_elem = 0; i_elem < ValEl->fldSize(); i_elem++)
	addFld(ValEl,i_elem);
    elem.push_back(ValEl);
}

void TValue::vlElemDet( TElem *ValEl )
{
    for(unsigned i_e = 0; i_e < elem.size(); i_e++)
	if(elem[i_e] == ValEl )
	{
	    for(unsigned i_elem = 0; i_elem < elem[i_e]->fldSize(); i_elem++)
		delFld(elem[i_e],i_elem);
	    elem[i_e]->valDet(this);
	    elem.erase(elem.begin()+i_e);
	    return;
	}
    //throw TError(nodePath().c_str(),"Element <%s> no present!",ValEl->elName().c_str());
}

TElem &TValue::vlElem( const string &name )
{
    for(unsigned i_e = 0; i_e < elem.size(); i_e++)
	if( elem[i_e]->elName() == name )
	    return *elem[i_e];
    throw TError(nodePath().c_str(),"Element <%s> no present!",name.c_str());
}

void TValue::cntrCmdProc( XMLNode *opt )
{
    vector<string> list_c;
    string a_path = opt->attr("path");
    //- Service commands process -
    if( a_path == "/serv/attr"  )		//Attributes access
    {
	vlList(list_c);
	if( ctrChkNode(opt,"list",RWRWRW,"root","root",SEQ_RD) )	//Full info attributes list
	{
	    AutoHD<TVal> attr;
	    for( int i_el = 0; i_el < list_c.size(); i_el++ )
	    {
		attr = vlAt(list_c[i_el]);
		opt->childAdd("el")->
		    setAttr("id",list_c[i_el])->
		    setAttr("nm",attr.at().fld().descr())->
		    setAttr("flg",TSYS::int2str(attr.at().fld().flg()))->
		    setAttr("tp",TSYS::int2str(attr.at().fld().type()))->
		    setAttr("vals",attr.at().fld().values())->
		    setAttr("names",attr.at().fld().selNames());
	    }
	}
	if( ctrChkNode(opt,"get",RWRWRW,"root","root",SEQ_RD) )		//All attributes values
	    for( int i_el = 0; i_el < list_c.size(); i_el++ )
		opt->childAdd("el")->setAttr("id",list_c[i_el])->setText(vlAt(list_c[i_el]).at().getS());
	if( ctrChkNode(opt,"set",RWRWRW,"root","root",SEQ_WR) )		//Multi attributes set
	    for( int i_el = 0; i_el < opt->childSize(); i_el++ )
		vlAt(opt->childGet(i_el)->attr("id")).at().setS(opt->childGet(i_el)->text());
	return;
    }

    //- Interface comands process -
    //-- Info command process --
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Parameter: ")+nodeName(),0664,"root","root");
	if(ctrMkNode("area",opt,-1,"/val",_("Atributes")))
	{
	    //--- Add atributes list ---
	    vlList(list_c);
	    for( int i_el = 0; i_el < list_c.size(); i_el++ )
		vlAt(list_c[i_el]).at().fld().cntrCmdMake(opt,"/val",-1);
	}
	//--- Archiving ---
	if(ctrMkNode("area",opt,-1,"/arch",_("Archiving")))
	{
	    if(ctrMkNode("table",opt,-1,"/arch/arch",_("Archiving"),0664,"root","root",1,"key","atr"))
	    {
		vector<string> list_c2;
		//- Prepare table headers -
		ctrMkNode("list",opt,-1,"/arch/arch/atr",_("Atribute"),0444,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/arch/arch/prc",_("Archiving"),0664,"root","root",1,"tp","bool");
		SYS->archive().at().modList(list_c);
		for( int i_ta = 0; i_ta < list_c.size(); i_ta++ )
		{
		    SYS->archive().at().at(list_c[i_ta]).at().valList(list_c2);
		    for( int i_a = 0; i_a < list_c2.size(); i_a++ )
		    {
			string a_id = SYS->archive().at().at(list_c[i_ta]).at().valAt(list_c2[i_a]).at().workId();
			ctrMkNode("list",opt,-1,("/arch/arch/"+a_id).c_str(),a_id,0664,"root","root",1,"tp","bool");
		    }
		}
	    }
	}
	return;
    }
    //-- Process command to page --
    if( a_path.substr(0,4) == "/val" )
    {
	if( a_path.size() > 9 && a_path.substr(0,9) == "/val/sel_" && ctrChkNode(opt) )
	{
	    AutoHD<TVal> vl = vlAt(TSYS::pathLev(a_path,1).substr(4));
	    for( int i_a=0; i_a < vl.at().fld().selNm().size(); i_a++ )
		opt->childAdd("el")->setText(vl.at().fld().selNm()[i_a]);
	    return;
	}
	AutoHD<TVal> vl = vlAt(TSYS::pathLev(a_path,1));
	if( ctrChkNode(opt,"get",(vl.at().fld().flg()&TFld::NoWrite)?0440:0660,"root","root",SEQ_RD) )
	{
	    if( vl.at().fld().flg()&TFld::Selected )	opt->setText(vl.at().getSEL());
	    else opt->setText( (vl.at().fld().type()==TFld::Real) ?
		    ((vl.at().getR()==EVAL_REAL) ? EVAL_STR : TSYS::real2str(vl.at().getR(),6)) : 
		    vl.at().getS() );
	}
	if( ctrChkNode(opt,"set",(vl.at().fld().flg()&TFld::NoWrite)?0440:0660,"root","root",SEQ_WR) )
	{
	    if( vl.at().fld().flg()&TFld::Selected )	vl.at().setSEL(opt->text());
	    else					vl.at().setS(opt->text());
	}
    }
    else if( a_path == "/arch/arch" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    vector<string> list_c2;
	    //--- Prepare headers ---
	    ctrMkNode("list",opt,-1,"/arch/arch/atr","",0444);
	    ctrMkNode("list",opt,-1,"/arch/arch/prc","",0664);
	    SYS->archive().at().modList(list_c);
	    for( int i_ta = 0; i_ta < list_c.size(); i_ta++ )
	    {
		SYS->archive().at().at(list_c[i_ta]).at().valList(list_c2);
		for( int i_a = 0; i_a < list_c2.size(); i_a++ )
		    ctrMkNode("list",opt,-1,("/arch/arch/"+SYS->archive().at().at(list_c[i_ta]).at().valAt(list_c2[i_a]).at().workId()).c_str(),"",0664);
	    }
	    //--- Fill table ---
	    vlList(list_c);
	    for( int i_v = 0; i_v < list_c.size(); i_v++ )
		for( int i_a = 0; i_a < opt->childSize(); i_a++ )
		{
		    XMLNode *chld = opt->childGet(i_a);
		    string c_id = chld->attr("id");
		    if(c_id=="atr")		chld->childAdd("el")->setText(list_c[i_v]);
		    else if(c_id=="prc")	chld->childAdd("el")->setText(vlAt(list_c[i_v]).at().arch().freeStat()?"0":"1");
		    else chld->childAdd("el")->
			setText(vlAt(list_c[i_v]).at().arch().freeStat()?"0":(vlAt(list_c[i_v]).at().arch().at().archivatorPresent(c_id)?"1":"0"));
		}
	}
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
	    bool create = false;	//--- Create archive ---
	    bool v_get = atoi(opt->text().c_str());
	    string attr = opt->attr("key_atr");
	    string col  = opt->attr("col");
			
	    //--- Check for create new ---
	    if( v_get && vlAt(attr).at().arch().freeStat() )
	    {
		//---- Make archive name ----
		string a_nm = nodeName()+"_"+attr;
		string rez_nm = a_nm;
		for( int p_cnt = 0; SYS->archive().at().valPresent(rez_nm); p_cnt++ )
		    rez_nm = a_nm+TSYS::int2str(p_cnt);
		//---- Create new archive ----
		SYS->archive().at().valAdd(rez_nm);
		SYS->archive().at().valAt(rez_nm).at().setValType(vlAt(attr).at().fld().type());
		SYS->archive().at().valAt(rez_nm).at().setSrcMode(TVArchive::PassiveAttr,
		    vlAt(attr).at().nodePath('.').substr(SYS->id().size()+1));
		SYS->archive().at().valAt(rez_nm).at().setToStart(true);
		SYS->archive().at().valAt(rez_nm).at().start();
		vlArchMake(vlAt(attr).at());
	    }
	    //--- Check for delete archive ---
	    if( col == "prc" && !v_get && !vlAt(attr).at().arch().freeStat() )
	    {
		string a_id = vlAt(attr).at().arch().at().id();
		SYS->archive().at().valDel(a_id,true);
		modif();
	    }
	    //--- Change archivator status ---
	    if( col != "prc" && !vlAt(attr).at().arch().freeStat() )
	    {
		if( v_get )	vlAt(attr).at().arch().at().archivatorAttach(col);
		else		vlAt(attr).at().arch().at().archivatorDetach(col,true);
		modif();
		vlAt(attr).at().arch().at().modif();
	    }
	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* TVal                                          *
//*************************************************
TVal::TVal( TFld &fld, TValue *owner ) :
    TCntrNode(owner), m_cfg(false)
{
    time = 0;
    modifClr();

    //- Chek for self field for dinamic elements -
    if( fld.flg()&TFld::SelfFld )
    {
	src.fld = new TFld();
	*(src.fld) = fld;
    }
    else src.fld = &fld;

    switch(src.fld->type())
    {
	case TFld::String:
	    val.val_s = new string("");
	    *(val.val_s) = src.fld->def();
	    break;
	case TFld::Integer:	val.val_i = atoi(src.fld->def().c_str());	break;
	case TFld::Real:	val.val_r = atof(src.fld->def().c_str());	break;
	case TFld::Boolean:	val.val_b = atoi(src.fld->def().c_str());	break;
    }
}

TVal::TVal(TCfg &cfg, TValue *owner ) :
    TCntrNode(owner), m_cfg(true)
{
    src.cfg = &cfg;
    time = 0;
}

TVal::~TVal( )
{
    if( !m_cfg && src.fld->type() == TFld::String )	delete val.val_s;
    if( !m_cfg && src.fld->flg()&TFld::SelfFld )	delete src.fld;
}

const string &TVal::name()
{
    if( m_cfg )	return( src.cfg->name() );
    else	return( src.fld->name() );
}

TFld &TVal::fld()
{
    if( m_cfg )	return( src.cfg->fld() );
    return( *src.fld );
}

void TVal::vlSet(  )
{
    ((TValue *)nodePrev())->vlSet( *this );
}

void TVal::vlGet(  )
{
    ((TValue *)nodePrev())->vlGet( *this );
}

AutoHD<TVArchive> TVal::arch()
{
    return m_arch;
}

void TVal::setArch(const AutoHD<TVArchive> &vl)
{
    m_arch = vl;
}

string TVal::getSEL( long long *tm, bool sys )
{
    if( !(fld().flg()&TFld::Selected) )	throw TError("Val","No select type!");
    switch( fld().type() )
    {
	case TFld::String:	return fld().selVl2Nm(getS(tm,sys));
	case TFld::Integer:	return fld().selVl2Nm(getI(tm,sys));
	case TFld::Real:	return fld().selVl2Nm(getR(tm,sys));
	case TFld::Boolean:	return fld().selVl2Nm(getB(tm,sys));
    }
}

string TVal::getS( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::Integer:
	{ int vl = getI(tm,sys);	return (vl!=EVAL_INT)  ? TSYS::int2str(vl) : EVAL_STR; }
	case TFld::Real:
	{ double vl = getR(tm,sys);	return (vl!=EVAL_REAL) ? TSYS::real2str(vl) : EVAL_STR; }
	case TFld::Boolean:
	{ char vl = getB(tm,sys);	return (vl!=EVAL_BOOL) ? TSYS::int2str((bool)vl) : EVAL_STR; }
	case TFld::String:
	    //- Get from archive -
	    if( tm && (*tm) && !m_arch.freeStat() && *tm/m_arch.at().period() < time/m_arch.at().period() ) 
		return m_arch.at().getS(tm);
	    //- Get value from config -
	    if( m_cfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getS( );
	    }
	    //- Get current value -
	    if( fld().flg()&TVal::DirRead && !sys )	vlGet( );
	    if( tm ) *tm = time;
	    return *val.val_s;
    }
}

int TVal::getI( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	{ string vl = getS(tm,sys);	return (vl!=EVAL_STR) ? atoi(vl.c_str()) : EVAL_INT; }
	case TFld::Real:
	{ double vl = getR(tm,sys);	return (vl!=EVAL_REAL) ? (int)vl : EVAL_INT; }
	case TFld::Boolean:
	{ char vl = getB(tm,sys);	return (vl!=EVAL_BOOL) ? (bool)vl : EVAL_INT; }
	case TFld::Integer:
	    //- Get from archive -
	    if( tm && (*tm) && !m_arch.freeStat() && *tm/m_arch.at().period() < time/m_arch.at().period() ) 
		return m_arch.at().getI(tm);
	    //- Get value from config -
	    if( m_cfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getI( );
	    }
	    //- Get current value -
	    if( fld().flg()&TVal::DirRead && !sys )	vlGet( );
	    if( tm ) *tm = time;
	    return val.val_i;
    }
}

double TVal::getR( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	{ string vl = getS(tm,sys);	return (vl!=EVAL_STR) ? atof(vl.c_str()) : EVAL_REAL; }
	case TFld::Integer:
	{ int vl = getI(tm,sys);	return (vl!=EVAL_INT) ? vl : EVAL_REAL; }
	case TFld::Boolean:
	{ char vl = getB(tm,sys);	return (vl!=EVAL_BOOL) ? (bool)vl : EVAL_REAL; }
	case TFld::Real:
	    //- Get from archive -
	    if( tm && (*tm) && !m_arch.freeStat() && *tm/m_arch.at().period() < time/m_arch.at().period() ) 
		return m_arch.at().getR(tm);
	    //- Get value from config -
	    if( m_cfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getR( );
	    }
	    //- Get current value -
	    if( fld().flg()&TVal::DirRead && !sys )	vlGet( );
	    if( tm ) *tm = time;
	    return val.val_r;
    }
}

char TVal::getB( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	{ string vl = getS(tm,sys);	return (vl!=EVAL_STR) ? (bool)atoi(vl.c_str()) : EVAL_BOOL; }
	case TFld::Integer:
	{ int vl = getI(tm,sys);	return (vl!=EVAL_INT) ? (bool)vl : EVAL_BOOL; }
	case TFld::Real:
	{ double vl = getR(tm,sys);	return (vl!=EVAL_REAL) ? (bool)vl : EVAL_BOOL; }
	case TFld::Boolean:
	    //- Get from archive -
	    if( tm && (*tm) && !m_arch.freeStat() && *tm/m_arch.at().period() < time/m_arch.at().period() ) 
		return m_arch.at().getB(tm);
	    //- Get value from config -
	    if( m_cfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getB( );
	    }
	    //- Get current value -
	    if( fld().flg()&TVal::DirRead && !sys )	vlGet( );
	    if( tm ) *tm = time;
	    return val.val_b;
    }
}

void TVal::setSEL( const string &value, long long tm, bool sys )
{
    if( !(fld().flg()&TFld::Selected) )	throw TError("Val","No select type!");
    switch( fld().type() )
    {
	case TFld::String:	setS(fld().selNm2VlS(value),tm,sys);	break;
	case TFld::Integer:	setI(fld().selNm2VlI(value),tm,sys);	break;
	case TFld::Real:	setR(fld().selNm2VlR(value),tm,sys);	break;
	case TFld::Boolean:	setB(fld().selNm2VlB(value),tm,sys);	break;
    }
}

void TVal::setS( const string &value, long long tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::Integer:
	    setI( (value!=EVAL_STR) ? atoi(value.c_str()) : EVAL_INT, tm, sys );	break;
	case TFld::Real:
	    setR( (value!=EVAL_STR) ? atof(value.c_str()) : EVAL_REAL, tm, sys );	break;
	case TFld::Boolean:
	    setB( (value!=EVAL_STR) ? (bool)atoi(value.c_str()) : EVAL_BOOL, tm, sys );	break;
	case TFld::String:
	    //- Set value to config -
	    if( m_cfg )	{ src.cfg->setS( value ); return; }
	    //- Check to write -
	    if( !sys && fld().flg()&TFld::NoWrite )	throw TError("Val","No write access!");
	    //- Set current value and time -
	    *val.val_s = value;
	    time = tm;
	    if(!time) time = TSYS::curTime();
	    if(fld().flg()&TVal::DirWrite && !sys)	vlSet( );
	    //- Set to archive -
	    if( !m_arch.freeStat() && m_arch.at().srcMode() == TVArchive::PassiveAttr )
		m_arch.at().setS(value,time);
    }
}

void TVal::setI( int value, long long tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	    setS( (value!=EVAL_INT) ? TSYS::int2str(value) : EVAL_STR, tm, sys );	break;
	case TFld::Real:
	    setR( (value!=EVAL_INT) ? value : EVAL_REAL, tm, sys );		break;
	case TFld::Boolean:
	    setB( (value!=EVAL_INT) ? (bool)value : EVAL_BOOL, tm, sys );	break;
	case TFld::Integer:
	    //- Set value to config -
	    if( m_cfg )	{ src.cfg->setI( value ); return; }
	    //- Check to write -
	    if( !sys && fld().flg()&TFld::NoWrite )	throw TError("Val","No write access!");
	    //- Set current value and time -
	    if( !(fld().flg()&TFld::Selected) && fld().selValI()[1] > fld().selValI()[0] && value != EVAL_INT )
	    {
		if( value > fld().selValI()[1] )value = fld().selValI()[1];
		if( value < fld().selValI()[0] )value = fld().selValI()[0];
	    }
	    val.val_i = value;
	    time = tm;
	    if(!time) time = TSYS::curTime();
	    if(fld().flg()&TVal::DirWrite && !sys) vlSet( );
	    //- Set to archive -
	    if( !m_arch.freeStat() && m_arch.at().srcMode() == TVArchive::PassiveAttr )
		m_arch.at().setI(value,time);
    }
}

void TVal::setR( double value, long long tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	    setS( (value!=EVAL_REAL) ? TSYS::real2str(value) : EVAL_STR, tm, sys );	break;
	case TFld::Integer:
	    setI( (value!=EVAL_REAL) ? (int)value : EVAL_INT, tm, sys );		break;
	case TFld::Boolean:
	    setB( (value!=EVAL_REAL) ? (bool)value : EVAL_BOOL, tm, sys );		break;
	case TFld::Real:
	    //- Set value to config -
	    if( m_cfg )	{ src.cfg->setR( value ); return; }
	    //- Check to write -
	    if( !sys && fld().flg()&TFld::NoWrite )	throw TError("Val","No write access!");
	    //- Set current value and time -
	    if( !(fld().flg()&TFld::Selected) && fld().selValR()[1] > fld().selValR()[0] && value != EVAL_REAL )
	    {
		if( value > fld().selValR()[1] )value = fld().selValR()[1];
		if( value < fld().selValR()[0] )value = fld().selValR()[0];
	    }
	    val.val_r = value;
	    time = tm;
	    if(!time) time = TSYS::curTime();
	    if(fld().flg()&TVal::DirWrite && !sys) vlSet( );
	    //- Set to archive -
	    if( !m_arch.freeStat() && m_arch.at().srcMode() == TVArchive::PassiveAttr )
		m_arch.at().setR(value,time);
    }
}

void TVal::setB( char value, long long tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	    setS( (value!=EVAL_BOOL) ? TSYS::int2str((bool)value) : EVAL_STR, tm, sys );	break;
	case TFld::Integer:
	    setI( (value!=EVAL_BOOL) ? (bool)value : EVAL_INT, tm, sys );	break;
	case TFld::Real:
	    setR( (value!=EVAL_BOOL) ? (bool)value : EVAL_REAL, tm, sys);	break;
	case TFld::Boolean:
	    //- Set value to config -
	    if( m_cfg )	{ src.cfg->setB( value ); return; }
	    //- Check to write -
	    if( !sys && fld().flg()&TFld::NoWrite )	throw TError("Val","No write access!");
	    //- Set current value and time -
	    val.val_b = value;
	    time = tm;
	    if(!time) time = TSYS::curTime();
	    if(fld().flg()&TVal::DirWrite && !sys)	vlSet( );
	    //- Set to archive -
	    if( !m_arch.freeStat() && m_arch.at().srcMode() == TVArchive::PassiveAttr )
		m_arch.at().setB(value,time);
    }
}

void TVal::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //- Service commands process -
    if( a_path == "/serv/val"  )		//Values access
    {
	if( ctrChkNode(opt,"info",RWRWRW,"root","root",SEQ_RD) )	//Value's data information
	{
	    if( !arch().freeStat() ) arch().at().cntrCmdProc(opt);
	    else
	    {
		opt->setAttr("end","0")->setAttr("beg","0")->setAttr("per","0");
		opt->setAttr("vtp",TSYS::int2str(fld().type()));
	    }
	}
	else if( ctrChkNode(opt,"get",RWRWRW,"root","root",SEQ_RD) )    //Value's data request
	{
	    if( !atoll(opt->attr("tm_grnd").c_str()) )
	    {
		long long tm = atoll(opt->attr("tm").c_str());
		opt->setText(getS(&tm));
		opt->setAttr("tm",TSYS::ll2str(tm));
	    }
	    else if( !arch().freeStat() ) arch().at().cntrCmdProc(opt);
	    else throw TError(nodePath().c_str(),"Attribute not have archive");
	}
	return;
    }

    //- Interface comands process -
    //-- Info command process --
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
	return;
    }
    //-- Process command to page --
    TCntrNode::cntrCmdProc(opt);
}
