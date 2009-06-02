
//OpenSCADA system module DAQ.DiamondBoards file: diamond.h
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
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

	Type	type( )		{ return m_tp; }
	int	cnl( )		{ return m_cnl; }
	void	setType( Type val );

	TMdContr &owner( );

    protected:
	//Methods
	void load_( );
	bool cfgChange( TCfg &cfg );
	void vlSet( TVal &val, const TVariant &pvl );
	void vlGet( TVal &val );
	void vlArchMake( TVal &val );

	void postEnable( int flag );

    private:
	//Attributes
	int	&m_cnl;
	Type	m_tp;
	union
	{
	    int	m_gain;		//AI gain
	    int	m_dio_port;	//DIO port
	};
};

//*************************************************
//* TMdContr                                      *
//*************************************************
class TTpContr;

class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem);
	~TMdContr( );

	bool ADIIntMode( )	{ return ad_int_mode; }
	bool dataEmul( )	{ return data_emul; }

	TParamContr *ParamAttach( const string &name, int type );

	TTpContr &owner( );

    protected:
	//Methods
	void load_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	bool cfgChange( TCfg &cfg );

    private:
	//Methods
	static void *AD_DSCTask( void *param );

	//Attributes
	int	&m_addr;
	bool	&ad_int_mode, &data_emul;

	double	dataEmulTm;

	DSCB	dscb;
	DSCADSETTINGS dscadsettings;

	pthread_t ad_dsc_pthr;
	bool	ad_dsc_st, endrun_req_ad_dsc;

	Res	ai_res, ao_res, dio_res;
};

//*************************************************
//* TTpContr                                      *
//*************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Methods
	TTpContr( string name );
	~TTpContr( );

	bool drvInitOk( )	{ return m_init; }
	void postEnable( int flag );

	TController *ContrAttach( const string &name, const string &daq_db );

	TElem &elemAI( )	{ return elem_ai; }
	TElem &elemAO( )	{ return elem_ao; }
	TElem &elemDI( )	{ return elem_di; }
	TElem &elemDO( )	{ return elem_do; }

    protected:
	//Attribute
	bool redntAllow( )	{ return true; }

    private:
	//Attributes
	bool	m_init;

	TElem	elem_ai;
	TElem	elem_ao;
	TElem	elem_di;
	TElem	elem_do;

	Res	drvRes;
};

extern TTpContr *mod;

} //End namespace

#endif //ATHENA_H
