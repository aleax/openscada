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
#include "tkernel.h"
#include "tmessage.h"
#include "tconfig.h"
#include "tparamcontr.h"
#include "tcontroller.h"
#include "tvalue.h"

const char *TValue::o_name = "TValue";

TValue::TValue(  ) : l_cfg(0), m_cfg(NULL), m_hd(o_name)
{

}

TValue::TValue( TConfig *cfg ) : m_cfg(cfg), l_cfg(0), m_hd(o_name)
{
    vector<string> list;
    m_cfg->cfgList( list );
    for( unsigned i_cf = 0; i_cf < list.size(); i_cf++ )
	if( !(m_cfg->cfg(list[i_cf]).fld().type()&F_NOVAL) )
	{
	    TVal *val = new TVal(m_cfg->cfg(list[i_cf]),*this);
	    m_hd.objAdd( val, &(string &)val->name(),l_cfg++);
	}    
}    

TValue::~TValue()
{
    for(unsigned i_e = 0; i_e < elem.size(); i_e++) 
	vlDetElem(elem[i_e]);
}

void TValue::addElem( TElem &el, unsigned id_val )
{
    int i_off = l_cfg; 
    for(unsigned i_e = 0; i_e < elem.size(); i_e++) 
	if(elem[i_e]->elName() == el.elName() ) break;
	else l_cfg+=elem[i_e]->fldSize();
    TVal *val = new TVal(el.fldAt(id_val),*this);
    m_hd.objAdd( val, &(string &)val->name(), id_val+l_cfg);		
}

void TValue::delElem( TElem &el, unsigned id_val )
{    
    delete (TVal *)m_hd.objDel( (string &)el.fldAt(id_val).name() );    
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
    throw TError("Element %s no avoid!",ValEl->elName().c_str());
}

TElem &TValue::vlElem( const string &name )
{
    for(unsigned i_e = 0; i_e < elem.size(); i_e++) 
	if( elem[i_e]->elName() == name )
	    return *elem[i_e];
    throw TError("Element %s no avoid!",name.c_str());
}

//****************************************************************************
//************************* TVal *********************************************
//****************************************************************************
TVal::TVal( TFld &fld, TValue &owner ) : m_cfg(false), m_valid(false), m_owner(owner)
{
    time.s = 0; 
    
    //Chek for self field for dinamic elements
    if( fld.type()&F_SELF )
    {
	src.fld = new TFld();
	*(src.fld) = fld;
    }
    else src.fld = &fld;
    
    if( src.fld->type()&T_STRING ) 
    {
	val.val_s = new string("");
	*(val.val_s) = src.fld->def();	
    }
    else if( src.fld->type()&(T_DEC|T_OCT|T_HEX) )	
	val.val_i = atoi(src.fld->def().c_str());
    else if( src.fld->type()&T_REAL )	
	val.val_r = atof(src.fld->def().c_str());
    else if( src.fld->type()&T_BOOL )
	if( src.fld->def() == "true") val.val_b = true; else val.val_b = false;
}

TVal::TVal(TCfg &cfg, TValue &owner ) : m_cfg(true), m_valid(false), m_owner(owner)
{
    src.cfg = &cfg;
    time.s = 0;
}

TVal::~TVal( )
{
    if( !m_cfg && src.fld->type()&T_STRING) delete val.val_s;
    if( !m_cfg && src.fld->type()&F_SELF )  delete src.fld;    
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
    m_owner.vlSet( *this );
}

void TVal::vlGet(  )
{ 
    m_owner.vlGet( *this );
}

string TVal::getSEL( STime *tm )
{
    unsigned i;

    if( m_cfg ) return src.cfg->getSEL( );
    
    if(!(src.fld->type()&T_SELECT)) throw TError("(%s) No select type!",src.fld->name().c_str());
    if(src.fld->type()&T_STRING)
	return src.fld->selVl2Nm(getS( tm ));
    else if(src.fld->type()&T_REAL)
	return src.fld->selVl2Nm(getR( tm ));
    else if(src.fld->type()&(T_DEC|T_OCT|T_HEX))
	return src.fld->selVl2Nm(getI( tm ));
    else if(src.fld->type()&T_BOOL)
	return src.fld->selVl2Nm(getB( tm ));
    throw TError("(%s) Type error!",src.fld->name().c_str());
}

string &TVal::getS( STime *tm )
{
    if( m_cfg ) return src.cfg->getS( );
    
    if(!src.fld->type()&T_STRING) throw TError("(%s) No string type!",src.fld->name().c_str());
    if(src.fld->type()&F_DRD) vlGet( );
    return(*val.val_s);
}

double &TVal::getR( STime *tm )
{
    if( m_cfg ) return src.cfg->getR( );
    
    if(!src.fld->type()&T_REAL) throw TError("(%s) No real type!",src.fld->name().c_str());
    if(src.fld->type()&F_DRD) vlGet( );
    return(val.val_r);
}

int &TVal::getI( STime *tm )
{
    if( m_cfg ) return src.cfg->getI( );
    
    if( !(src.fld->type()&(T_DEC|T_OCT|T_HEX)) ) 
	throw TError("(%s) No int type!",src.fld->name().c_str());
    if(src.fld->type()&F_DRD) vlGet( );
    return(val.val_i);
}

bool &TVal::getB( STime *tm )
{
    if( m_cfg ) return src.cfg->getB( );
    
    if(!(src.fld->type()&T_BOOL)) throw TError("(%s) No bool type!",src.fld->name().c_str());
    if(src.fld->type()&F_DRD) vlGet( );
    return(val.val_b);
}

string TVal::setSEL( const string &value, STime *tm, bool sys )
{
    unsigned i;
    if( m_cfg )
    { 
        src.cfg->setSEL( value );
	return src.cfg->getSEL( );
    }
  
    if(!(src.fld->type()&T_SELECT)) throw TError("(%s) No select type!",src.fld->name().c_str());
    if(src.fld->type()&T_STRING)		setS(src.fld->selNm2VlS(value),tm,sys);
    else if(src.fld->type()&T_REAL)		setR(src.fld->selNm2VlR(value),tm,sys);
    else if(src.fld->type()&(T_DEC|T_OCT|T_HEX))setI(src.fld->selNm2VlI(value),tm,sys);
    else if(src.fld->type()&T_BOOL)		setB(src.fld->selNm2VlB(value),tm,sys);
    else throw TError("(%s) Type error!",src.fld->name().c_str());
    
    return(value);
}

string &TVal::setS( const string &value, STime *tm, bool sys )
{
    if( m_cfg )
    { 
	if( !sys && src.cfg->fld().type()&F_NWR ) 
	    throw TError("(%s) No write access!",src.fld->name().c_str());
        src.cfg->setS( value );
	return src.cfg->getS( );
    }
    
    if( !sys && src.fld->type()&F_NWR ) 
	throw TError("(%s) No write access!",src.fld->name().c_str());
    if( !(src.fld->type()&T_STRING) ) 
	throw TError("(%s) No string type!",src.fld->name().c_str());
    
    *val.val_s = value;
    if(src.fld->type()&F_DWR) vlSet( );
    return(*val.val_s);
}

double &TVal::setR( double value, STime *tm, bool sys )
{    
    if( m_cfg )
    { 
	if( !sys && src.cfg->fld().type()&F_NWR ) 
	    throw TError("(%s) No write access!",src.fld->name().c_str());
        src.cfg->setR( value );
	return src.cfg->getR( );
    }
    
    if( !sys && src.fld->type()&F_NWR ) 
	throw TError("(%s) No write access!",src.fld->name().c_str());
    if( !src.fld->type()&T_REAL ) 
	throw TError("(%s) No real type!",src.fld->name().c_str());
    //Check range          
    if( !(src.fld->type()&T_SELECT) && src.fld->selValR()[1] > src.fld->selValR()[0] )
        value = (value > src.fld->selValR()[1])?src.fld->selValR()[1]:
                (value < src.fld->selValR()[0])?src.fld->selValR()[0]:value;
    if(src.fld->type()&F_DWR) vlSet( );
    val.val_r = value;
    return(val.val_r);
}

int &TVal::setI( int value, STime *tm, bool sys )
{        
    if( m_cfg )
    {
	if( !sys && src.cfg->fld().type()&F_NWR ) 
	    throw TError("(%s) No write access!",src.fld->name().c_str());
        src.cfg->setI( value );
	return src.cfg->getI( );
    }
    
    if( !sys && src.fld->type()&F_NWR ) 
	throw TError("(%s) No write access!",src.fld->name().c_str());    
    if( !(src.fld->type()&(T_DEC|T_OCT|T_HEX)) ) 
	throw TError("(%s) No int type!",src.fld->name().c_str());
    //Want check range          
    if( !(src.fld->type()&T_SELECT) && src.fld->selValI()[1] > src.fld->selValI()[0] )
        value = (value > src.fld->selValI()[1])?src.fld->selValI()[1]:
                (value < src.fld->selValI()[0])?src.fld->selValI()[0]:value;
    //Chek for get curent value
    val.val_i = value;
    if(src.fld->type()&F_DWR) vlSet( );
    return(val.val_i);
}

bool &TVal::setB( bool value, STime *tm, bool sys )
{
    if( m_cfg )
    { 
	if( !sys && src.cfg->fld().type()&F_NWR ) 
	    throw TError("(%s) No write access!",src.fld->name().c_str());
        src.cfg->setB( value );
	return src.cfg->getB( );
    }
    
    if( !sys && src.fld->type()&F_NWR ) 
	throw TError("(%s) No write access!",src.fld->name().c_str());        
    if( !src.fld->type()&T_BOOL ) 
	throw TError("(%s) No bool type!",src.fld->name().c_str());
    //Chek for get curent value
    val.val_b = value;
    if(src.fld->type()&F_DWR) vlSet( );
    return(val.val_b);
}


