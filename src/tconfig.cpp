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
 
#include "tkernel.h"
#include "tmessage.h"
#include "tbds.h"
#include "tconfig.h"

const char *TConfig::o_name = "TConfig";

TConfig::TConfig( TElem *Elements )
{
    if( Elements == NULL)    
    {
	elem = new TElem("single");
	single = true;
    }
    else
    {
	elem = Elements;
    	single = false;
    }
    
    elem->cntAtt(this);
    cf_AddRecord(0); 
}

TConfig::~TConfig()
{
    if(elem == NULL) return;
    while(value.size())	cfFreeRecord(0);	

    elem->cntDet(this);
    if( single ) delete elem;
}

void TConfig::addElem( TElem &el, unsigned id )
{
    for(unsigned val_id=0; val_id < value.size(); val_id++)
	value[val_id].insert( value[val_id].begin()+id,new TCfg(elem->elAt(id),*this));
}

void TConfig::delElem( TElem &el, unsigned id )
{
    for(unsigned val_i=0; val_i < value.size(); val_i++)
    {
	delete value[val_i][id];
	value[val_i].erase(value[val_i].begin()+id);	    
    }
}

TCfg &TConfig::cfg( const string &n_val, unsigned int id )
{
    if( id >= value.size() )             throw TError("(%s) Id error!",o_name);
    int id_elem = elem->elNameId(n_val);
    if( !value[id][id_elem]->view() )     throw TError("(%s) Value no view!",o_name);
    return *value[id][id_elem];
}

int TConfig::cf_AddRecord( unsigned int id)
{
    if( id > cfSize() ) throw TError("(%s) id error!",o_name);

    vector< TCfg* > _val;
    for(unsigned i=0; i < elem->elSize(); i++)
	_val.push_back( new TCfg(elem->elAt(i),*this));
    value.insert(value.begin()+id,_val);

    return(id);   
}

void TConfig::cfFreeRecord( unsigned int id)
{
    if( id >= value.size() ) throw TError("(%s) Id error!",o_name);
    for(unsigned i=0; i < elem->elSize(); i++)	
	delete value[id][i];
    value.erase(value.begin()+id);
}

void TConfig::cfFreeDubl( const string &n_val, bool mode )
{
    int id_elem = elem->elNameId(n_val);
    if( !mode )
	for(unsigned i_cfg = 0; i_cfg < cfSize()-1; i_cfg++)
	    for(unsigned i_cfg1 = i_cfg+1; i_cfg1 < cfSize(); i_cfg1++)		
		if( value[i_cfg][id_elem] == value[i_cfg1][id_elem] )
		    cfFreeRecord( i_cfg1-- ); 
    else
	for(int i_cfg = cfSize()-1; i_cfg > 0; i_cfg--)
	    for(int i_cfg1 = i_cfg-1; i_cfg1 >= 0; i_cfg1--)
		if( value[i_cfg][id_elem] == value[i_cfg1][id_elem] )
		    cfFreeRecord( i_cfg1-- ); 
}

void TConfig::cfLoadValBD( const string &NameFld, TTable &table, unsigned int id_rec )
{
    int line;
    string val;
    
    if(id_rec >= value.size()) throw TError("(%s) Id of record error!",o_name);
    int i_fld = elem->elNameId(NameFld);
    if( !(value[id_rec][i_fld]->fld().type()&T_STRING) ) 
	throw TError("(%s) Type of individual field no string!",o_name);
    //Find line
    for(line=0; line < table.NLines(); line++)
	if( table.GetCellS(table.ColumNameToId(NameFld),line) == value[id_rec][i_fld]->getS() ) break;
    if(line == table.NLines( )) 
	throw TError("(%s) Cell %s no avoid into table!",o_name,value[id_rec][i_fld]->getS().c_str());
    //Load config from found line
    return(cfLoadValBD(line,table,id_rec));
}

void TConfig::cfLoadValBD(int line_bd, TTable &table, unsigned int id_rec )
{
    int i_elem;
    string val;

    if(id_rec >= value.size()) throw TError("(%s) Id of record error!",o_name);
    for(i_elem=0; i_elem < value[id_rec].size(); i_elem++)
    {
	try
	{
	    if(value[id_rec][i_elem]->fld().type()&T_STRING)
		value[id_rec][i_elem]->setS( table.GetCellS(table.ColumNameToId(value[id_rec][i_elem]->name()),line_bd) );
	    else if(value[id_rec][i_elem]->fld().type()&(T_DEC|T_OCT|T_HEX))
		value[id_rec][i_elem]->setI( table.GetCellI(table.ColumNameToId(value[id_rec][i_elem]->name()),line_bd) );
	    else if(value[id_rec][i_elem]->fld().type()&T_REAL)
		value[id_rec][i_elem]->setR( table.GetCellR(table.ColumNameToId(value[id_rec][i_elem]->name()),line_bd) );
	    else if(value[id_rec][i_elem]->fld().type()&T_BOOL)
		value[id_rec][i_elem]->setB( table.GetCellB(table.ColumNameToId(value[id_rec][i_elem]->name()),line_bd) );
	}
	catch(...){ }
    }
}

void TConfig::cfSaveValBD( const string &NameFld, TTable &table, unsigned int id_rec)
{
    int line;

    if(id_rec >= value.size()) throw TError("(%s) Id of record error!",o_name);
    int i_fld = elem->elNameId(NameFld);
    if( !(value[id_rec][i_fld]->fld().type()&T_STRING) ) 
	throw TError("(%s) Type of individual field no string!",o_name);
    //Find line
    for(line=0; line < table.NLines(); line++)
    {
	try
	{ 
	    if( table.GetCellS(table.ColumNameToId(NameFld),line) == value[id_rec][i_fld]->getS() )
		break; 
	}
	catch(...)
	{
	    line = table.NLines();
	    break;
	}
    }
    cfSaveValBD(line, table, id_rec);
}

void TConfig::cfSaveValBD( int line_bd, TTable &table, unsigned int id_rec)
{
    int i_elem;

    if(id_rec >= value.size()) throw TError("(%s) Id of record error!",o_name);
    if(line_bd == table.NLines() || line_bd < 0)
    {
	line_bd = table.NLines();
	table.AddLine(line_bd);
    }

    for(i_elem=0; i_elem < value[id_rec].size(); i_elem++)
    {
	try
	{
    	    if(value[id_rec][i_elem]->fld().type()&T_STRING)
    		table.SetCellS(table.ColumNameToId(value[id_rec][i_elem]->name()),line_bd,value[id_rec][i_elem]->getS());	    
	    else if(value[id_rec][i_elem]->fld().type()&(T_DEC|T_OCT|T_HEX))
	    	table.SetCellI(table.ColumNameToId(value[id_rec][i_elem]->name()),line_bd,value[id_rec][i_elem]->getI());	    
	    else if(value[id_rec][i_elem]->fld().type()&T_REAL)
		table.SetCellR(table.ColumNameToId(value[id_rec][i_elem]->name()),line_bd,value[id_rec][i_elem]->getR());	    
	    else if(value[id_rec][i_elem]->fld().type()&T_BOOL)
		table.SetCellB(table.ColumNameToId(value[id_rec][i_elem]->name()),line_bd,value[id_rec][i_elem]->getB());
	}
	catch(...){ }
    }
}

void TConfig::cfLoadAllValBD( TTable &table, bool free )
{
    int i_bd_ln, i_elem, i_rec;
    
    if( free ) 
	while(value.size()) cfFreeRecord(0);	
    
    for(i_bd_ln = 0; i_bd_ln < table.NLines( ); i_bd_ln++)
    {
	i_rec = value.size();
	cf_AddRecord(i_rec);
    	for(i_elem=0; i_elem < value[i_rec].size(); i_elem++)
	{
	    try
	    {
		if(value[i_rec][i_elem]->fld().type()&T_STRING)
		    value[i_rec][i_elem]->setS( table.GetCellS(table.ColumNameToId(value[i_rec][i_elem]->name()),i_bd_ln) );
		else if(value[i_rec][i_elem]->fld().type()&(T_DEC|T_OCT|T_HEX))
		    value[i_rec][i_elem]->setI( table.GetCellI(table.ColumNameToId(value[i_rec][i_elem]->name()),i_bd_ln) );
		else if(value[i_rec][i_elem]->fld().type()&T_REAL)
		    value[i_rec][i_elem]->setR( table.GetCellR(table.ColumNameToId(value[i_rec][i_elem]->name()),i_bd_ln) );
		else if(value[i_rec][i_elem]->fld().type()&T_BOOL)
		    value[i_rec][i_elem]->setB( table.GetCellB(table.ColumNameToId(value[i_rec][i_elem]->name()),i_bd_ln) );
	    }
	    catch(...){ }
	}
    }
}


int TConfig::cfSaveAllValBD( TTable &table )
{
    int i_ln, i_elem;
    
    while(table.NLines()) table.DelLine(0);
    for( i_ln=0 ;i_ln < (int)value.size(); i_ln++)
    {
	table.AddLine(i_ln);	    
	for(i_elem=0; i_elem < value[i_ln].size(); i_elem++)
	{
	    try
	    {
		if(value[i_ln][i_elem]->fld().type()&T_STRING)
		    table.SetCellS(table.ColumNameToId(value[i_ln][i_elem]->name()),i_ln,value[i_ln][i_elem]->getS());
		else if(value[i_ln][i_elem]->fld().type()&(T_DEC|T_OCT|T_HEX))
		    table.SetCellI(table.ColumNameToId(value[i_ln][i_elem]->name()),i_ln,value[i_ln][i_elem]->getI());	    
		else if(value[i_ln][i_elem]->fld().type()&T_REAL)
		    table.SetCellR(table.ColumNameToId(value[i_ln][i_elem]->name()),i_ln,value[i_ln][i_elem]->getR());	    
		else if(value[i_ln][i_elem]->fld().type()&T_BOOL)
		    table.SetCellB(table.ColumNameToId(value[i_ln][i_elem]->name()),i_ln,value[i_ln][i_elem]->getB());
	    }
	    catch(...) {  }
	}
    }
    table.Save( );

    return(0);
}

TConfig & TConfig::operator=(TConfig & cfg)
{
    if(elem == cfg.elem)
    {
	while(cfSize()) cfFreeRecord(0);
	for(unsigned i_rc=0; i_rc < cfg.cfSize(); i_rc++)
	{
	    cf_AddRecord(i_rc);
	    for(unsigned i_el=0; i_el < elem->elSize(); i_el++)
	    {
		try{ *value[i_rc][i_el] = *cfg.value[i_rc][i_el]; }
		catch(...){  }
    	    }
	}
    }
    return(*this);
}

void TConfig::cfListEl( vector<string> &list, unsigned int id )
{
    list.clear();
    if(id >= value.size()) throw TError("(%s) Id of record error!",o_name);
    for(unsigned i = 0; i < value[id].size(); i++)
    	if(value[id][i]->view()) list.push_back(value[id][i]->name());
}

void TConfig::cfConfElem(TElem *Elements)
{
    if(elem == Elements) return;
    if(elem != NULL)
    {
    	while(value.size()) cfFreeRecord(0);
	elem->cntDet(this);
	if(single) delete elem;
    }
    
    if( Elements == NULL)    
    {
	elem = new TElem("single");
	single = true;
    }
    else
    {
	elem = Elements;
    	single = false;
    }
    
    elem->cntAtt(this);
    cf_AddRecord(0); 
}

TElem &TConfig::cfConfElem()
{
    if(elem == NULL) throw TError("(%s) config element no attach!");
    return(*elem);
}


//*************************************************
//**************** TCfg ***************************
//*************************************************
const char *TCfg::o_name = "TCfg";

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
	throw TError("(%s) Type no select: %s!",o_name,name().c_str());

    if( m_fld->type()&T_STRING ) 	return m_fld->selName(*m_val.s_val);
    else if( m_fld->type()&(T_DEC|T_OCT|T_HEX) )	return m_fld->selName(m_val.i_val);
    else if( m_fld->type()&T_REAL )	return m_fld->selName(m_val.r_val);
    else if( m_fld->type()&T_BOOL )	return m_fld->selName(m_val.b_val);
    else throw TError("(%s) Select error!",o_name); 
}

string &TCfg::getS( )
{
    if( !(m_fld->type()&T_STRING) )   
	throw TError("(%s) Type no string: %s!",o_name,name().c_str());

    return(*m_val.s_val);
}

double &TCfg::getR( )
{
    if( !(m_fld->type()&T_REAL) ) 
	throw TError("(%s) Type no real: %s!",o_name,name().c_str());
    
    return(m_val.r_val);
}

int &TCfg::getI( )
{
    if( !(m_fld->type()&(T_DEC|T_OCT|T_HEX)) )   
       	throw TError("(%s) Type no int: %s!",o_name,name().c_str());       
    
    return(m_val.i_val);
}

bool &TCfg::getB( )
{
    if( !(m_fld->type()&T_BOOL) ) 
	throw TError("(%s) Type no boolean: %s!",o_name,name().c_str());
    
    return(m_val.b_val);
}

void TCfg::setSEL( const string &val )
{
    if( !(m_fld->type()&T_SELECT) ) 
	throw TError("(%s) Type no select: %s!",o_name,name().c_str());

    if( m_fld->type()&T_STRING ) 	setS( m_fld->selVals(val) );
    else if( m_fld->type()&(T_DEC|T_OCT|T_HEX) )	setI( m_fld->selVali(val) );
    else if( m_fld->type()&T_REAL )	setR( m_fld->selValr(val) );
    else if( m_fld->type()&T_BOOL )	setB( m_fld->selValb(val) ); 
    else 				throw TError("(%s) Select error!",o_name); 
}

void TCfg::setS( const string &val )
{
    if( m_fld->type()&F_NWR )  
	throw TError("(%s) No write access to <%s>!",o_name,name().c_str());
    if( !(m_fld->type()&T_STRING) )  
	throw TError("(%s) Type no string: %s!",o_name,name().c_str());
    
    if( m_fld->type()&T_SELECT ) m_fld->selName(val);       //check selectable
    if( m_fld->type()&F_PREV )
    {
	string t_str = *(m_val.s_val);
	*(m_val.s_val) = val;    
	if( !m_owner.cfChange(*this) ) 
	    *(m_val.s_val) = t_str;
    }
    else *(m_val.s_val) = val;    
}

void TCfg::setR( double val )
{
    if( m_fld->type()&F_NWR )  
	throw TError("(%s) No write access to <%s>!",o_name,name().c_str());
    if( !(m_fld->type()&T_REAL) ) 
	throw TError("(%s) Type no real: %s!",o_name,name().c_str());
    
    if( m_fld->type()&T_SELECT )	m_fld->selName(val);       //check selectable
    else if( m_fld->val_r()[0] < m_fld->val_r()[1] )
    {
	if( val < m_fld->val_r()[0] )		val = m_fld->val_r()[0];
	else if( val > m_fld->val_r()[1] )	val = m_fld->val_r()[1];
    }
    if( m_fld->type()&F_PREV )
    {
	double t_val = m_val.r_val;
       	m_val.r_val = val;
	if( !m_owner.cfChange(*this) ) 
	    m_val.r_val = t_val;
    }
    else m_val.r_val = val;
}

void TCfg::setI( int val )
{
    if( m_fld->type()&F_NWR )  
	throw TError("(%s) No write access to <%s>!",o_name,name().c_str());
    if( !(m_fld->type()&(T_DEC|T_OCT|T_HEX)) )  
	throw TError("(%s) Type no int: %s!",o_name,name().c_str());
    
    if( m_fld->type()&T_SELECT ) m_fld->selName(val);       //check selectable
    else if( m_fld->val_i()[0] < m_fld->val_i()[1] )
    {        
	if( val < m_fld->val_i()[0] ) 		val = m_fld->val_i()[0];
	else if( val > m_fld->val_i()[1] ) 	val = m_fld->val_i()[1];
    }
    if( m_fld->type()&F_PREV )
    {
	int t_val = m_val.i_val;
       	m_val.i_val = val;
	if( !m_owner.cfChange(*this) ) 
	    m_val.i_val = t_val;
    }
    else m_val.i_val = val;
}

void TCfg::setB( bool val )
{
    if( m_fld->type()&F_NWR )  
	throw TError("(%s) No write access to <%s>!",o_name,name().c_str());
    if( !(m_fld->type()&T_BOOL) ) 
	throw TError("(%s) Type no boolean: %s!",o_name,name().c_str());
    
    if( m_fld->type()&T_SELECT )	m_fld->selName(val);       //check selectable
    if( m_fld->type()&F_PREV )
    {
	bool t_val = m_val.b_val;
       	m_val.b_val = val;
	if( !m_owner.cfChange(*this) ) 
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

    return( *this );
}

