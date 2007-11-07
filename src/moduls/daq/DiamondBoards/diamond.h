
//OpenSCADA system module DAQ.DiamondBoards file: diamond.h
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
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
 
#ifndef ATHENA_H
#define ATHENA_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#include "dscud.h"

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;

namespace Diamond
{

//*************************************************
//* TMdPrm                                        *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Data
	enum Type { NONE, AI, AO, DI, DO };
    
	//Methods
    	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );
	
	Type type( )		{ return m_tp; }
	int  cnl( ) 		{ return m_cnl; }
	void setType( Type val );

	void enable( );

	TMdContr &owner( )	{ return (TMdContr &)TParamContr::owner(); }
	
    protected:
	//Methods
	bool cfgChange( TCfg &cfg );
	void vlSet( TVal &val );
        void vlGet( TVal &val );
	void vlArchMake( TVal &val );

	void postEnable( int flag );
	//void preDisable( int flag );    
	
    private:
	//Attributes
	int 	&m_cnl;
	Type    m_tp;
	union
	{
	    int	m_gain;		//AI gain
	    int m_dio_port;	//DIO port
	};
};

//*************************************************
//* TMdContr                                      *
//*************************************************
class TTpContr;

class TMdContr: public TController
{
    public:
	//Data
	//- DSC access struct -
	struct
	{
	    char comm;	// 0-free; 1-get AI; 2-set AO; 3-get DI; 4-set DO
	    int  prm1;
	    int  prm2;
	    Res  gen_res;
	    pthread_cond_t  th_cv;
	    pthread_mutex_t th_mut;
	}DSC;	
    
	//Methods
    	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem);
	~TMdContr( );   
	
	bool ADIIntMode( )	{ return ad_int_mode; }
	bool dataEmul( )	{ return data_emul; }

	TParamContr *ParamAttach( const string &name, int type );

	void load( );
	void save( );
	void start_( );
	void stop_( );

	TTpContr &owner( ) 	{ return (TTpContr&)TController::owner(); }

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	bool cfgChange( TCfg &cfg );
    
    private:
	//Methods
	static void *AD_DSCTask( void *param );
	static void *DSCTask( void *param );
	
	//Attributes
	int &m_addr;
	bool &ad_int_mode, &data_emul;
	
	double dataEmulTm;
	
	pthread_t dsc_pthr, 
		  ad_dsc_pthr;
	bool	ad_dsc_st, endrun_req_ad_dsc,
		dsc_st, endrun_req_dsc;
};

//*************************************************
//* TTpContr                                      *
//*************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Methods
	TTpContr( string name );
	~TTpContr();	    
	
	void postEnable( int flag );
	
	void drvInit( );
    
	TController *ContrAttach( const string &name, const string &daq_db );
	
	TElem &elemAI( )	{ return elem_ai; }
	TElem &elemAO( )	{ return elem_ao; }
	TElem &elemDI( ) 	{ return elem_di; }
	TElem &elemDO( ) 	{ return elem_do; }
	
    private:
	//Attributes
	bool	m_init;	
	
	TElem 	elem_ai;
	TElem   elem_ao;
	TElem   elem_di;
	TElem   elem_do;
	
	Res  	drvRes;
};

extern TTpContr *mod;

} //End namespace 

#endif //ATHENA_H
