
//OpenSCADA system module DAQ.DCON file: DCON_client.h
/***************************************************************************
 *   Copyright (C) 2008 by Almaz Karimov                                   *
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

#ifndef DCON_CLIENT_H
#define DCON_CLIENT_H

#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>
#include <map>

#undef _
#define _(mess) mod->I18N(mess)

#define MaxLenReq 200

using std::string;
using std::vector;
using std::map;

namespace DCONDAQ
{

//******************************************************
//* TMdPrm                                             *
//******************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Methods
	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );

	void enable( );
	void disable( );

	TElem &elem( )		{ return p_el; }
	TMdContr &owner( );

	//Attributes
	TElem	p_el;		//Work atribute elements
	int	&mod_tp;	//I-7000 module type
	int	&mod_addr;	//I-7000 module address
	bool	&crc_ctrl;	//DCON CRC control mode
	string	acq_err;

	//Values
	bool	DI[16];
	bool	DO[16];
	double	AI[16];
	double	AO[16];
	bool	module_err;

    private:
	//Methods
	void postEnable( int flag );
	void vlGet( TVal &val );
	void vlSet( TVal &val, const TVariant &pvl );
	void vlArchMake( TVal &val );
};

//******************************************************
//* TMdContr                                           *
//******************************************************
class TMdContr: public TController
{
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, TElem *cfgelem);
	~TMdContr( );

	string getStatus( );

	double period( )	{ return vmax(m_per,0.01); }
	int    prior( )		{ return m_prior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void prmEn( const string &id, bool val );
	string DCONCRC( string str );
	string DCONReq( string &pdu );

    protected:
	//Methods
	void disable_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	bool cfgChange( TCfg &cfg );

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	en_res, req_res;		//Resource for enable params and request values
	double	&m_per;				//Acquisition task (seconds)
	int	&m_prior,			//Process task priority
		&connTry;			//Connections try
	string	&m_addr;			//Transport device address

	bool	prc_st,				//Process task active
		endrun_req;			//Request to stop of the Process task
	vector< AutoHD<TMdPrm> >  p_hd;

	double	tm_gath;			//Gathering time
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

    protected:
	//Methods
	void load_( );
	void save_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	void postEnable( int flag );
	TController *ContrAttach( const string &name, const string &daq_db );
	string optDescr( );
};

extern TTpContr *mod;

} //End namespace

#endif //DCON_CLIENT_H
