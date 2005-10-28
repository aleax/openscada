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
#include "tparamcontr.h"
#include "tcontroller.h"
#include "tvalue.h"

TValue::TValue( ) : l_cfg(0), m_cfg(NULL)
{
    m_vl = grpAdd();
}

TValue::TValue( TConfig *cfg ) : m_cfg(cfg), l_cfg(0)
{    
    m_vl = grpAdd();
}

void TValue::postEnable()
{
    if( m_cfg )
    {
	vector<string> list;
	m_cfg->cfgList( list );
	for( unsigned i_cf = 0; i_cf < list.size(); i_cf++ )
    	    if( !(m_cfg->cfg(list[i_cf]).fld().flg()&FLD_NOVAL) )
        	chldAdd(m_vl, new TVal(m_cfg->cfg(list[i_cf]),this),l_cfg++);
    }
}

TValue::~TValue()
{
    //for(unsigned i_e = 0; i_e < elem.size(); i_e++) 
    //	vlDetElem(elem[i_e]);
}

void TValue::addElem( TElem &el, unsigned id_val )
{
    int i_off = l_cfg; 
    for(unsigned i_e = 0; i_e < elem.size(); i_e++) 
	if(elem[i_e]->elName() == el.elName() ) break;
	else l_cfg+=elem[i_e]->fldSize();
    chldAdd(m_vl,new TVal(el.fldAt(id_val),this),id_val+l_cfg);
}

void TValue::delElem( TElem &el, unsigned id_val )
{    
    chldDel(m_vl,(string &)el.fldAt(id_val).name());
}

void TValue::vlAttElem( TElem *ValEl )    
{    
    ValEl->valAtt(this);
    for(unsigned i_elem = 0; i_elem < ValEl->fldSize(); i_elem++) 
	addElem(*ValEl,i_elem);
    elem.push_back(ValEl);
}

void TValue::vlDetElem( TElem *ValEl )
{
    for(unsigned i_e = 0; i_e < elem.size(); i_e++) 
	if(elem[i_e] == ValEl )
	{
	    for(unsigned i_elem = 0; i_elem < elem[i_e]->fldSize(); i_elem++) 
		delElem(*elem[i_e],i_elem);
	    elem[i_e]->valDet(this);
	    elem.erase(elem.begin()+i_e);
	    return;
	}
    throw TError(nodePath().c_str(),"Element <%s> no present!",ValEl->elName().c_str());
}

TElem &TValue::vlElem( const string &name )
{
    for(unsigned i_e = 0; i_e < elem.size(); i_e++) 
	if( elem[i_e]->elName() == name )
	    return *elem[i_e];
    throw TError(nodePath().c_str(),"Element <%s> no present!",name.c_str());
}

void TValue::cntrMake( XMLNode *fld, const char *req, const char *path, int pos )
{
    vector<string> list_c;
    vlList(list_c);
	
    for( unsigned i_el = 0; i_el < list_c.size(); i_el++ )
	vlAt(list_c[i_el]).at().fld().cntrMake(fld,req,path,(pos<0)?pos:pos++);
}
		    

void TValue::cntrCmd( const string &elem, XMLNode *fld, TCntrNode::Command cmd )
{ 
    AutoHD<TVal> vl = vlAt(elem);
    switch(cmd)
    {
	case TCntrNode::Get:	    
	    if( elem.substr(0,4) == "sel_" )
	    {
		AutoHD<TVal> vl = vlAt(elem.substr(4));
		for( unsigned i_a=0; i_a < vl.at().fld().selNm().size(); i_a++ )
		    TCntrNode::ctrSetS( fld, vl.at().fld().selNm()[i_a] );
		return;
	    }
	    if( vl.at().fld().flg()&FLD_SELECT )	TCntrNode::ctrSetS(fld,vl.at().getSEL());
	    else switch(vl.at().fld().type())
	    {
		case TFld::String:	TCntrNode::ctrSetS(fld,vl.at().getS());	break;
		case TFld::Dec: case TFld::Oct: case TFld::Hex:
					TCntrNode::ctrSetI(fld,vl.at().getI());	break;
		case TFld::Real:	TCntrNode::ctrSetR(fld,vl.at().getR());	break;
		case TFld::Bool:	TCntrNode::ctrSetB(fld,vl.at().getB());	break;
	    }
	    break;	    
	case TCntrNode::Set:
	    if( vl.at().fld().flg()&FLD_SELECT )	vl.at().setSEL(TCntrNode::ctrGetS(fld));
	    else switch(vl.at().fld().type())
	    {
		case TFld::String:      vl.at().setS(TCntrNode::ctrGetS(fld));	break;
		case TFld::Dec: case TFld::Oct: case TFld::Hex:
					vl.at().setI(TCntrNode::ctrGetI(fld));	break;
		case TFld::Real:	vl.at().setR(TCntrNode::ctrGetR(fld));	break;
		case TFld::Bool:	vl.at().setB(TCntrNode::ctrGetB(fld));	break;
	    }    
	    break;
    }
}

//****************************************************************************
//************************* TVal *********************************************
//****************************************************************************
TVal::TVal( TFld &fld, TValue *owner ) : 
    TCntrNode(owner), m_cfg(false), m_valid(false)
{
    time.s = 0; 
    
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
    TCntrNode(owner), m_cfg(true), m_valid(false)
{
    src.cfg = &cfg;
    time.s = 0;
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
    else        return( *src.fld );
}

void TVal::vlSet(  )
{ 
    ((TValue *)nodePrev())->vlSet( *this );
}

void TVal::vlGet(  )
{ 
    ((TValue *)nodePrev())->vlGet( *this );
}

string TVal::getSEL( STime *tm )
{
    unsigned i;

    if( m_cfg ) return src.cfg->getSEL( );    
    
    if( !(src.fld->flg()&FLD_SELECT) )	
	throw TError("Val","No select type!");
    switch( src.fld->type() )
    {
	case TFld::String:	return src.fld->selVl2Nm(getS( tm ));
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				return src.fld->selVl2Nm(getI( tm ));
	case TFld::Real:	return src.fld->selVl2Nm(getR( tm ));
	case TFld::Bool:	return src.fld->selVl2Nm(getB( tm ));
    }
}

string TVal::getS( STime *tm )
{
    if( m_cfg ) return src.cfg->getS( );
    if( src.fld->flg()&FLD_DRD )vlGet( );
    switch( src.fld->type() )
    {
	case TFld::String:      return *val.val_s;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				return TSYS::int2str(val.val_i);
	case TFld::Real:	return TSYS::real2str(val.val_r);
	case TFld::Bool:	return TSYS::int2str(val.val_b);
    }
}

double TVal::getR( STime *tm )
{
    if( m_cfg ) return src.cfg->getR( );
    if( src.fld->flg()&FLD_DRD )vlGet( );
    switch( src.fld->type() )
    {
	case TFld::String:      return atof(val.val_s->c_str());
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				return val.val_i;
	case TFld::Real:	return val.val_r;
	case TFld::Bool:	return val.val_b;
    }
}

int TVal::getI( STime *tm )
{
    if( m_cfg ) return src.cfg->getI( );
    if( src.fld->flg()&FLD_DRD )vlGet( );
    switch( src.fld->type() )
    {
	case TFld::String:      return atoi(val.val_s->c_str());
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				return val.val_i;
	case TFld::Real:	return (int)val.val_r;
	case TFld::Bool:	return val.val_b;
    }
}

bool TVal::getB( STime *tm )
{
    if( m_cfg ) return src.cfg->getB( );
    if( src.fld->flg()&FLD_DRD )vlGet( );
    switch( src.fld->type() )
    {
	case TFld::String:      return atoi(val.val_s->c_str());
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				return val.val_i;
	case TFld::Real:	return val.val_r;
	case TFld::Bool:	return val.val_b;
    }
}

void TVal::setSEL( const string &value, STime *tm, bool sys )
{
    if( m_cfg )	src.cfg->setSEL( value );
    if( !(src.fld->flg()&FLD_SELECT) )	
	throw TError("Val","No select type!");
    switch( src.fld->type() )
    {
	case TFld::String:      setS(src.fld->selNm2VlS(value),tm,sys);	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
				setI(src.fld->selNm2VlI(value),tm,sys);	break;
	case TFld::Real:	setR(src.fld->selNm2VlR(value),tm,sys);	break;
	case TFld::Bool:	setB(src.fld->selNm2VlB(value),tm,sys);	break;
    }    
}

void TVal::setS( const string &value, STime *tm, bool sys )
{    
    if( !sys && src.cfg->fld().flg()&FLD_NWR ) 
	throw TError("Val","No write access!");
    if( m_cfg )	src.cfg->setS( value );
    switch( src.fld->type() )
    {
	case TFld::String:      
	    *val.val_s = value;
	    if(src.fld->flg()&FLD_DWR)	vlSet( );
	    break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    			setI(atoi(value.c_str()),tm,sys);	break;
	case TFld::Real:	setR(atof(value.c_str()),tm,sys);	break;
	case TFld::Bool:	setB(atoi(value.c_str()),tm,sys);	break;
    }        
}

void TVal::setR( double value, STime *tm, bool sys )
{    
    if( !sys && src.cfg->fld().flg()&FLD_NWR ) 
	throw TError("Val","No write access!");
    if( m_cfg )	src.cfg->setR( value );
    switch( src.fld->type() )
    {
	case TFld::String:	setS(TSYS::real2str(value),tm,sys);	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    			setI((int)value,tm,sys);	break;
	case TFld::Real:
	    if( !(src.fld->flg()&FLD_SELECT) && src.fld->selValR()[1] > src.fld->selValR()[0] )
	    {
        	if( value > src.fld->selValR()[1] )	value = src.fld->selValR()[1];
		if( value < src.fld->selValR()[0] )	value = src.fld->selValR()[0];
	    }
	    val.val_r = value;
	    if(src.fld->flg()&FLD_DWR) vlSet( );
	    break;
	case TFld::Bool:	setB(value,tm,sys);	break;
    }        
}

void TVal::setI( int value, STime *tm, bool sys )
{        
    if( !sys && src.cfg->fld().flg()&FLD_NWR ) 
	throw TError("Val","No write access!");
    if( m_cfg )	src.cfg->setI( value );
    switch( src.fld->type() )
    {
	case TFld::String:	setS(TSYS::int2str(value),tm,sys);	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    if( !(src.fld->flg()&FLD_SELECT) && src.fld->selValI()[1] > src.fld->selValI()[0] )
	    {
		if( value > src.fld->selValI()[1] )	value = src.fld->selValI()[1];
		if( value < src.fld->selValI()[0] )	value = src.fld->selValI()[0];
	    }
	    val.val_i = value;
	    if(src.fld->flg()&FLD_DWR) vlSet( );
	    break;
	case TFld::Real:	setR(value,tm,sys);	break;
	case TFld::Bool:	setB(value,tm,sys);	break;
    }        
}

void TVal::setB( bool value, STime *tm, bool sys )
{
    if( !sys && src.cfg->fld().flg()&FLD_NWR ) 
	throw TError("Val","No write access!");
    if( m_cfg )	src.cfg->setB( value );
    switch( src.fld->type() )
    {
	case TFld::String:	setS(TSYS::int2str(value),tm,sys);	break;
	case TFld::Dec: case TFld::Oct: case TFld::Hex:
	    			setI(value,tm,sys);	break;
	case TFld::Real:	setR(value,tm,sys);	break;
	case TFld::Bool:
	    val.val_b = value;
	    if(src.fld->flg()&FLD_DWR)	vlSet( );
	    break;
    }        
}


