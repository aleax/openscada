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

TValue::TValue(  ) : elem(NULL), l_cfg(0), m_cfg(NULL)
{

}

TValue::TValue( TConfig *cfg ) : elem(NULL), m_cfg(cfg), l_cfg(0)
{
    vector<string> list;
    m_cfg->cfListEl( list );
    for( unsigned i_cf = 0; i_cf < list.size(); i_cf++ )
	if( !(m_cfg->cfg(list[i_cf]).fld().type()&V_NOVAL) )
	    val.insert( val.begin()+(l_cfg++), new TVal(m_cfg->cfg(list[i_cf]),*this) );
}    

TValue::~TValue()
{
    while(val.size())
    {
	delete val[0];
	val.erase(val.begin());
    }
    if(elem) elem->cntDet(this);
}


void TValue::vlList( vector<string> &list )
{ 
    for( unsigned i_vl = 0; i_vl < val.size(); i_vl++ )
	list.push_back(val[i_vl]->name());
    for( unsigned i_br = 0; i_br < m_br.size(); i_br++ )
	m_br[i_br]->vlList(list);
}

TVal &TValue::vlVal( string name )
{
    string t_el;
    
    int i_pos = name.find("/");
    if( i_pos != string::npos )
    {	
	t_el = name.substr(0,i_pos);
	name = name.substr(i_pos+1);
    }
    if( !t_el.size() ) 
        for( int i_v = 0; i_v < val.size(); i_v++ )
	    if( val[i_v]->name() == name )
		return(*val[i_v]);	
    else
	for( unsigned i_el = 0; i_el < m_br.size(); i_el++ )
	    if( t_el == m_br[i_el]->vlElem().elName() ) 
		return( m_br[i_el]->vlVal(name));
    throw TError("(%s) Attribut no avoid!",name.c_str());
}

void TValue::addElem( unsigned id_val )
{
    val.insert( val.begin()+id_val+l_cfg, new TVal(elem->elAt(id_val),*this) );
}

void TValue::delElem( unsigned id_val )
{
    delete val[id_val+l_cfg];
    val.erase(val.begin()+id_val+l_cfg);
}

void TValue::vlElem( TElem *ValEl )
{
    if(elem == ValEl) return;
    if(elem != NULL)
    {
	while(val.size() > l_cfg)
	{
	    delete val[l_cfg];
	    val.erase(val.begin()+l_cfg);
	}
	elem->cntDet(this);
	elem = NULL;
    }
    elem = ValEl;
    if( elem != NULL )
    {
	elem->cntAtt(this);
	for(unsigned i_elem = 0; i_elem < elem->elSize(); i_elem++) 
	    addElem(i_elem);
    }
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
    
    /*
    if( src.fld->type()&(T_REAL|T_DEC|T_OCT|T_HEX) )
    {
	scale = new SCale;
	if( src.fld->val() && src.fld->vals().size() != 2)  scale->max = scale->min = 0.0; 
	else 
	{
	    scale->max = atof(src.fld->vals()[1].c_str());
	    scale->min = atof(src.fld->vals()[0].c_str());
	}
    }
    */
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
	return src.fld->selName(getS( tm ));
    else if(src.fld->type()&T_REAL)
	return src.fld->selName(getR( tm ));
    else if(src.fld->type()&(T_DEC|T_OCT|T_HEX))
	return src.fld->selName(getI( tm ));
    else if(src.fld->type()&T_BOOL)
	return src.fld->selName(getB( tm ));
    throw TError("(%s) Type error!",src.fld->name().c_str());
}

string &TVal::getS( STime *tm )
{
    if( m_cfg ) return src.cfg->getS( );
    
    if(!src.fld->type()&T_STRING) throw TError("(%s) No string type!",src.fld->name().c_str());
    if(src.fld->type()&V_RD_D) vlGet( );
    return(*val.val_s);
}

double &TVal::getR( STime *tm )
{
    if( m_cfg ) return src.cfg->getR( );
    
    if(!src.fld->type()&T_REAL) throw TError("(%s) No real type!",src.fld->name().c_str());
    if(src.fld->type()&V_RD_D) vlGet( );
    return(val.val_r);
}

int &TVal::getI( STime *tm )
{
    if( m_cfg ) return src.cfg->getI( );
    
    if( !(src.fld->type()&(T_DEC|T_OCT|T_HEX)) ) 
	throw TError("(%s) No int type!",src.fld->name().c_str());
    if(src.fld->type()&V_RD_D) vlGet( );
    return(val.val_i);
}

bool &TVal::getB( STime *tm )
{
    if( m_cfg ) return src.cfg->getB( );
    
    if(!(src.fld->type()&T_BOOL)) throw TError("(%s) No bool type!",src.fld->name().c_str());
    if(src.fld->type()&V_RD_D) vlGet( );
    return(val.val_b);
}

string TVal::setSEL( string value, STime *tm, bool sys )
{
    unsigned i;
    if( m_cfg )
    { 
        src.cfg->setSEL( value );
	return src.cfg->getSEL( );
    }
  
    if(!(src.fld->type()&T_SELECT)) throw TError("(%s) No select type!",src.fld->name().c_str());
    if(src.fld->type()&T_STRING)		setS(src.fld->selVals(value),tm,sys);
    else if(src.fld->type()&T_REAL)		setR(src.fld->selValr(value),tm,sys);
    else if(src.fld->type()&(T_DEC|T_OCT|T_HEX))setI(src.fld->selVali(value),tm,sys);
    else if(src.fld->type()&T_BOOL)		setB(src.fld->selValb(value),tm,sys);
    else throw TError("(%s) Type error!",src.fld->name().c_str());
    
    return(value);
}

string &TVal::setS( string value, STime *tm, bool sys )
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
    if(src.fld->type()&V_WR_D) vlSet( );
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
    if( !(src.fld->type()&T_SELECT) && src.fld->val_r()[1] > src.fld->val_r()[0] )
        value = (value > src.fld->val_r()[1])?src.fld->val_r()[1]:
                (value < src.fld->val_r()[0])?src.fld->val_r()[0]:value;
    if(src.fld->type()&V_WR_D) vlSet( );
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
    if( !(src.fld->type()&T_SELECT) && src.fld->val_i()[1] > src.fld->val_i()[0] )
        value = (value > src.fld->val_i()[1])?src.fld->val_i()[1]:
                (value < src.fld->val_i()[0])?src.fld->val_i()[0]:value;
    //Chek for get curent value
    val.val_i = value;
    if(src.fld->type()&V_WR_D) vlSet( );
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
    if(src.fld->type()&V_WR_D) vlSet( );
    return(val.val_b);
}


