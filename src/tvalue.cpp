
//OpenSCADA system file: tvalue.cpp
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
#include "tarchval.h"
#include "tparamcontr.h"
#include "tcontroller.h"
#include "tvalue.h"

TValue::TValue( ) : l_cfg(0), m_cfg(NULL)
{
    m_vl = grpAdd("a_");
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
	else l_cfg+=elem[i_e]->fldSize();
    chldAdd(m_vl,new TVal(el->fldAt(id_val),this),id_val+l_cfg);
}

void TValue::delFld( TElem *el, unsigned id_val )
{   
    if( nodeMode() == TCntrNode::Enable && chldPresent(m_vl,el->fldAt(id_val).name()) ) 
	chldDel(m_vl,el->fldAt(id_val).name());
}

void TValue::vlCfg( TConfig *cfg )
{
    vector<string> list;
    //Detach old configs
    if( m_cfg )
    {
        m_cfg->cfgList( list );
	for( unsigned i_cf = 0; i_cf < list.size(); i_cf++ )
	    if( !(m_cfg->cfg(list[i_cf]).fld().flg()&FLD_NOVAL) && vlPresent(list[i_cf]) )
            {
                chldDel(m_vl,list[i_cf]);
                l_cfg--;
            }
	m_cfg = NULL;
    }
    //Attach new config
    if( cfg )
    {
	cfg->cfgList( list );
	for( unsigned i_cf = 0; i_cf < list.size(); i_cf++ )
	    if( !(cfg->cfg(list[i_cf]).fld().flg()&FLD_NOVAL) && !vlPresent(list[i_cf]) )
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

void TValue::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{    
    if( cmd==TCntrNode::Info )
    {
	TCntrNode::cntrCmd_(a_path,opt,cmd);
    
	ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",Mess->I18N("Parameter: ")+nodeName());
	ctrMkNode("area",opt,-1,a_path.c_str(),"/val",Mess->I18N("Atributes"));
        //Add atributes list
    	vector<string> list_c;
    	vlList(list_c);
    	for( int i_el = 0; i_el < list_c.size(); i_el++ )
	    vlAt(list_c[i_el]).at().fld().cntrMake(opt,a_path.c_str(),"/val",-1);	    
	//Archiving
	ctrMkNode("area",opt,-1,a_path.c_str(),"/arch",Mess->I18N("Archiving"));
	ctrMkNode("table",opt,-1,a_path.c_str(),"/arch/arch",Mess->I18N("Archiving"),0664,0,0,1,"key","atr");
    }
    else if( cmd==TCntrNode::Get )
    {	
	if( a_path.substr(0,4) == "/val" )
	{	
	    if( a_path.substr(0,9) == "/val/sel_" )
	    {
		AutoHD<TVal> vl = vlAt(TSYS::pathLev(a_path,1).substr(4));
		for( int i_a=0; i_a < vl.at().fld().selNm().size(); i_a++ )
		    ctrSetS( opt, vl.at().fld().selNm()[i_a] );
		return;
	    }
	    AutoHD<TVal> vl = vlAt(TSYS::pathLev(a_path,1));
	    if( vl.at().fld().flg()&FLD_SELECT ) ctrSetS(opt,vl.at().getSEL());
	    else switch(vl.at().fld().type())
	    {
		case TFld::String:	ctrSetS(opt,vl.at().getS());	break;
		case TFld::Dec: case TFld::Oct: case TFld::Hex:
					ctrSetI(opt,vl.at().getI());	break;
		case TFld::Real:	ctrSetR(opt,vl.at().getR());	break;
		case TFld::Bool:	ctrSetB(opt,vl.at().getB());	break;
	    }
	}
	else if( a_path == "/arch/arch" )
	{
	    vector<string> ta_ls, a_ls, rez_a_ls;
	    //Prepare headers
	    ctrMkNode("list",opt,-1,"","atr",Mess->I18N("Atribute"),0444,0,0,1,"tp","str");
	    ctrMkNode("list",opt,-1,"","prc",Mess->I18N("Archiving"),0664,0,0,1,"tp","bool");
	    SYS->archive().at().modList(ta_ls);
	    for( int i_ta = 0; i_ta < ta_ls.size(); i_ta++ )
	    {
		SYS->archive().at().at(ta_ls[i_ta]).at().valList(a_ls);
		for( int i_a = 0; i_a < a_ls.size(); i_a++ )
		{
		    rez_a_ls.push_back(SYS->archive().at().at(ta_ls[i_ta]).at().valAt(a_ls[i_a]).at().workId());
		    ctrMkNode("list",opt,-1,"",rez_a_ls[rez_a_ls.size()-1].c_str(),rez_a_ls[rez_a_ls.size()-1].c_str(),0664,0,0,1,"tp","bool");
		}
	    }
	    //Fill table
	    vlList(ta_ls);
	    for( int i_v = 0; i_v < ta_ls.size(); i_v++ )
	    {
		ctrSetS(ctrId(opt,"atr"),ta_ls[i_v]);
		ctrSetB(ctrId(opt,"prc"),!vlAt(ta_ls[i_v]).at().arch().freeStat());
		for( int i_a = 0; i_a < rez_a_ls.size(); i_a++ )
		    ctrSetB(ctrId(opt,rez_a_ls[i_a]),(vlAt(ta_ls[i_v]).at().arch().freeStat())?false:
			(vlAt(ta_ls[i_v]).at().arch().at().archivatorPresent(rez_a_ls[i_a])));
	    }
	}
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path.substr(0,4) == "/val" )
	{ 	
	    AutoHD<TVal> vl = vlAt(TSYS::pathLev(a_path,1));
	    if( vl.at().fld().flg()&FLD_SELECT ) vl.at().setSEL(ctrGetS(opt));
	    else switch(vl.at().fld().type())
	    {
		case TFld::String:      vl.at().setS(ctrGetS(opt));	break;
		case TFld::Dec: case TFld::Oct: case TFld::Hex:
					vl.at().setI(ctrGetI(opt));	break;
		case TFld::Real:	vl.at().setR(ctrGetR(opt));	break;
		case TFld::Bool:	vl.at().setB(ctrGetB(opt));	break;
	    }    
	}
	else if( a_path == "/arch/arch" )
        {
	    bool create = false;	//Create archive
	    bool v_get = ctrGetB(opt);
	    string attr = opt->attr("key_atr");
            string col  = opt->attr("col");
			
	    //Check for create new 
	    if( v_get && vlAt(attr).at().arch().freeStat() )
	    {
		//Make archive name
		string a_nm = nodeName()+"_"+attr;
		string rez_nm = a_nm;
		int p_cnt = 0;
		while(SYS->archive().at().valPresent(rez_nm))
		    rez_nm = a_nm+TSYS::int2str(p_cnt++);
		//Create new archive		
		SYS->archive().at().valAdd(rez_nm);
		SYS->archive().at().valAt(rez_nm).at().valType(vlAt(attr).at().fld().type());
		SYS->archive().at().valAt(rez_nm).at().srcMode(TVArchive::ActiveAttr,
		    vlAt(attr).at().nodePath('.').substr(SYS->id().size()+1));
		SYS->archive().at().valAt(rez_nm).at().toStart(true);
		SYS->archive().at().valAt(rez_nm).at().start();
	    }
	    //Check for delete archive
	    if( col == "prc" && !v_get && !vlAt(attr).at().arch().freeStat() )
	    {
		string a_id = vlAt(attr).at().arch().at().id();
		SYS->archive().at().valDel(a_id,true);
	    }
	    //Change archivator status
	    if( col != "prc" && !vlAt(attr).at().arch().freeStat() )
	    {
		if( v_get )	vlAt(attr).at().arch().at().archivatorAttach(col);
		else 		vlAt(attr).at().arch().at().archivatorDetach(col);	    
	    }
	}
	else TCntrNode::cntrCmd_(a_path,opt,cmd);
    }
}

//****************************************************************************
//************************* TVal *********************************************
//****************************************************************************
TVal::TVal( TFld &fld, TValue *owner ) : 
    TCntrNode(owner), m_cfg(false)
{
    time = 0;	    
    
    //Chek for self field for dinamic elements
    if( fld.flg()&FLD_SELF )
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
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    			val.val_i = atoi(src.fld->def().c_str());	break;
	case TFld::Real: 	val.val_r = atof(src.fld->def().c_str());	break;
	case TFld::Bool:	val.val_b = (src.fld->def() == "true")?true:false;	break;
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
    if( !m_cfg && src.fld->flg()&FLD_SELF )		delete src.fld;    
}

const string &TVal::name()
{
    if( m_cfg ) return( src.cfg->name() );
    else        return( src.fld->name() );    
}

TFld &TVal::fld()
{
    if( m_cfg ) return( src.cfg->fld() );
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

void TVal::arch(const AutoHD<TVArchive> &vl)
{ 
    m_arch = vl; 
}

string TVal::getSEL( long long *tm, bool sys )
{
    if( !(fld().flg()&FLD_SELECT) )	throw TError("Val","No select type!");
    switch( fld().type() )
    {
	case TFld::String:	return fld().selVl2Nm(getS(tm,sys));
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				return fld().selVl2Nm(getI(tm,sys));
	case TFld::Real:	return fld().selVl2Nm(getR(tm,sys));
	case TFld::Bool:	return fld().selVl2Nm(getB(tm,sys));
    }
}

string TVal::getS( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	{ int vl = getI(tm,sys); return (vl==EVAL_INT)?EVAL_STR:TSYS::int2str(vl); }
	case TFld::Real:	
	{ double vl = getR(tm,sys); return (vl==EVAL_REAL)?EVAL_STR:TSYS::real2str(vl); }
	case TFld::Bool:	
	{ char vl = getB(tm,sys); return (vl==EVAL_BOOL)?EVAL_STR:TSYS::int2str((bool)vl); }
	case TFld::String:
	    //Get from archive
	    if( tm && !m_arch.freeStat() && *tm/m_arch.at().period() < time/m_arch.at().period() ) 
		return m_arch.at().getS(tm);
	    //Get value from config
	    if( m_cfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getS( );
	    }
	    //Get current value
	    if( fld().flg()&FLD_DRD && !sys )	vlGet( );
	    if( tm ) *tm = time;
	    return *val.val_s;
    }
}

double TVal::getR( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:      
	{ string vl = getS(tm,sys); return (vl==EVAL_STR)?EVAL_REAL:atof(vl.c_str()); }
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	{ int vl = getI(tm,sys); return (vl==EVAL_INT)?EVAL_REAL:vl; }
	case TFld::Bool:	
	{ char vl = getB(tm,sys); return (vl==EVAL_BOOL)?EVAL_REAL:(bool)vl; }
	case TFld::Real:		
	    //Get from archive
	    if( tm && !m_arch.freeStat() && *tm/m_arch.at().period() < time/m_arch.at().period() ) 
		return m_arch.at().getR(tm);
	    //Get value from config
	    if( m_cfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getR( );
	    }
	    //Get current value
	    if( fld().flg()&FLD_DRD && !sys )	vlGet( );
	    if( tm ) *tm = time;
	    return val.val_r;
    }
}

int TVal::getI( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:      
	{ string vl = getS(tm,sys); return (vl==EVAL_STR)?EVAL_INT:atoi(vl.c_str()); }
	case TFld::Real:	
	{ double vl = getR(tm,sys); return (vl==EVAL_REAL)?EVAL_INT:(int)vl; }
	case TFld::Bool:	
	{ char vl = getB(tm,sys); return (vl==EVAL_BOOL)?EVAL_INT:(bool)vl; }
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    //Get from archive
	    if( tm && !m_arch.freeStat() && *tm/m_arch.at().period() < time/m_arch.at().period() ) 
		return m_arch.at().getI(tm);
	    //Get value from config
	    if( m_cfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getI( );
	    }
	    //Get current value
	    if( fld().flg()&FLD_DRD && !sys )	vlGet( );
	    if( tm ) *tm = time;
	    return val.val_i;
    }
}

char TVal::getB( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:      
	{ string vl = getS(tm,sys); return (vl==EVAL_STR)?EVAL_BOOL:(bool)atoi(vl.c_str()); }
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	{ int vl = getI(tm,sys); return (vl==EVAL_INT)?EVAL_BOOL:(bool)vl; }
	case TFld::Real:	
	{ double vl = getR(tm,sys); return (vl==EVAL_REAL)?EVAL_BOOL:(bool)vl; }
	case TFld::Bool:
	    //Get from archive
	    if( tm && !m_arch.freeStat() && *tm/m_arch.at().period() < time/m_arch.at().period() ) 
		return m_arch.at().getB(tm);
	    //Get value from config
	    if( m_cfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getB( );
	    }
	    //Get current value
	    if( fld().flg()&FLD_DRD && !sys )	vlGet( );
	    if( tm ) *tm = time;
	    return val.val_b;
    }
}

void TVal::setSEL( const string &value, long long tm, bool sys )
{
    if( !(fld().flg()&FLD_SELECT) )	throw TError("Val","No select type!");
    switch( fld().type() )
    {
	case TFld::String:      setS(fld().selNm2VlS(value),tm,sys);	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				setI(fld().selNm2VlI(value),tm,sys);	break;
	case TFld::Real:	setR(fld().selNm2VlR(value),tm,sys);	break;
	case TFld::Bool:	setB(fld().selNm2VlB(value),tm,sys);	break;
    }    
}

void TVal::setS( const string &value, long long tm, bool sys )
{    
    switch( fld().type() )
    {
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    setI((value==EVAL_STR)?EVAL_INT:atoi(value.c_str()),tm,sys);	break;
	case TFld::Real:	
	    setR((value==EVAL_STR)?EVAL_REAL:atof(value.c_str()),tm,sys);	break;
	case TFld::Bool:	
	    setB((value==EVAL_STR)?EVAL_BOOL:(bool)atoi(value.c_str()),tm,sys);	break;
	case TFld::String:
	    //Set value to config
	    if( m_cfg )	{ src.cfg->setS( value ); return; }
	    //Check to write
	    if( !sys && fld().flg()&FLD_NWR )	throw TError("Val","No write access!");
	    //Set current value and time
	    *val.val_s = value;
	    time = tm;
	    if(!time) time = TSYS::curTime();
	    if(fld().flg()&FLD_DWR && !sys)	vlSet( );
	    //Set to archive
	    if( !m_arch.freeStat() && m_arch.at().srcMode() == TVArchive::PassiveAttr )
		m_arch.at().setS(value,time);
    }        
}

void TVal::setR( double value, long long tm, bool sys )
{    
    switch( fld().type() )
    {
	case TFld::String:	
	    setS((value==EVAL_REAL)?EVAL_STR:TSYS::real2str(value),tm,sys);	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    setI((value==EVAL_REAL)?EVAL_INT:(int)value,tm,sys);	break;
	case TFld::Bool:	
	    setB((value==EVAL_REAL)?EVAL_BOOL:(bool)value,tm,sys);	break;
	case TFld::Real:
	    //Set value to config
	    if( m_cfg )	{ src.cfg->setR( value ); return; }
	    //Check to write
	    if( !sys && fld().flg()&FLD_NWR )	throw TError("Val","No write access!");
	    //Set current value and time
	    if( !(fld().flg()&FLD_SELECT) && fld().selValR()[1] > fld().selValR()[0] && value != EVAL_REAL )
	    {
        	if( value > fld().selValR()[1] )value = fld().selValR()[1];
		if( value < fld().selValR()[0] )value = fld().selValR()[0];
	    }
	    val.val_r = value;
	    time = tm;
	    if(!time) time = TSYS::curTime();
	    if(fld().flg()&FLD_DWR && !sys) vlSet( );
	    //Set to archive
	    if( !m_arch.freeStat() && m_arch.at().srcMode() == TVArchive::PassiveAttr )
		m_arch.at().setR(value,time);
    }        
}

void TVal::setI( int value, long long tm, bool sys )
{        
    switch( fld().type() )
    {
	case TFld::String:	
	    setS((value==EVAL_INT)?EVAL_STR:TSYS::int2str(value),tm,sys);	break;
	case TFld::Real:	
	    setR((value==EVAL_INT)?EVAL_REAL:value,tm,sys);	break;
	case TFld::Bool:	
	    setB((value==EVAL_INT)?EVAL_BOOL:(bool)value,tm,sys);	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:				
	    //Set value to config
	    if( m_cfg )	{ src.cfg->setI( value ); return; }
	    //Check to write
	    if( !sys && fld().flg()&FLD_NWR )	throw TError("Val","No write access!");
	    //Set current value and time
	    if( !(fld().flg()&FLD_SELECT) && fld().selValI()[1] > fld().selValI()[0] && value != EVAL_INT )
	    {
		if( value > fld().selValI()[1] )value = fld().selValI()[1];
		if( value < fld().selValI()[0] )value = fld().selValI()[0];
	    }
	    val.val_i = value;
	    time = tm;
	    if(!time) time = TSYS::curTime();
	    if(fld().flg()&FLD_DWR && !sys) vlSet( );
	    //Set to archive
	    if( !m_arch.freeStat() && m_arch.at().srcMode() == TVArchive::PassiveAttr )
		m_arch.at().setI(value,time);
    }        
}

void TVal::setB( char value, long long tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:	
	    setS((value==EVAL_BOOL)?EVAL_STR:TSYS::int2str((bool)value),tm,sys);	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    setI((value==EVAL_BOOL)?EVAL_INT:(bool)value,tm,sys);	break;
	case TFld::Real:	
	    setR((value==EVAL_BOOL)?EVAL_REAL:(bool)value,tm,sys);	break;
	case TFld::Bool:
	    //Set value to config
	    if( m_cfg )	{ src.cfg->setB( value ); return; }
	    //Check to write
	    if( !sys && fld().flg()&FLD_NWR )	throw TError("Val","No write access!");
	    //Set current value and time
	    val.val_b = value;
	    time = tm;
	    if(!time) time = TSYS::curTime();
	    if(fld().flg()&FLD_DWR && !sys)	vlSet( );
	    //Set to archive
	    if( !m_arch.freeStat() && m_arch.at().srcMode() == TVArchive::PassiveAttr )
		m_arch.at().setB(value,time);
    }        
}

