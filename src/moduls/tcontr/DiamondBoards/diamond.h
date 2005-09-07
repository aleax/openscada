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
 
#ifndef ATHENA_H
#define ATHENA_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttipcontroller.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#include "dscud.h"

using std::string;
using std::vector;

namespace Diamond
{

//======================================================================
//==== TMdPrm 
//======================================================================
class TMdPrm : public TParamContr
{
    public:
	enum Type { NONE, AI, AO, DI, DO };
    
    	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );
	
	Type type(){ return m_tp; }
	void type( Type val );
	
	void enable();
	void disable();
	
    protected:
	bool cfgChange( TCfg &cfg );
	void vlSet( TVal &val );
        void vlGet( TVal &val );
	
	void preDisable( int flag );    
	
    private:
	int 	&m_cnl;	
	Type	m_tp;
	
        DSCADSETTINGS	ad_set;
};

//======================================================================
//==== TMdContr 
//======================================================================
class TMdContr: public TController
{
    public:
    	TMdContr( string name_c, const TBDS::SName &bd, ::TTipController *tcntr, ::TElem *cfgelem);
	~TMdContr();   

	TParamContr *ParamAttach( const string &name, int type );

	void load_(  );
	void save_(  );
	void start_(  );
	void stop_(  );
    
	DSCB &cntrAccess(){ return dscb; }
	
    protected:
	//================== Controll functions ========================
        void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
    
    private:
	DSCB 	dscb;
};

//======================================================================
//==== TTpContr 
//======================================================================
class TTpContr: public TTipController
{
    public:
	TTpContr( string name );
	~TTpContr();	    
	
	void postEnable();
	
	bool initStat(){ return m_init; }
    
	TController *ContrAttach( const string &name, const TBDS::SName &bd);
	
	TElem &elemAI() { return elem_ai; }
	TElem &elemAO() { return elem_ao; }
	TElem &elemDI() { return elem_di; }
	TElem &elemDO() { return elem_do; }
	
    private:
	bool	m_init;	
	
	TElem 	elem_ai;
	TElem   elem_ao;
	TElem   elem_di;
	TElem   elem_do;
};


} //End namespace 

#endif //ATHENA_H

