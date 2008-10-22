
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
	TMdContr &owner( )	{ return (TMdContr&)TParamContr::owner(); }

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
	void vlSet( TVal &val );
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

	double period( )	{ return vmax(m_per,0.01); }
	int    prior( )		{ return m_prior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void prmEn( const string &id, bool val );
	string DCONCRC( string str );
	string DCONReq( string &pdu, int req_len );

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
	int	&m_prior;			//Process task priority
	string	&m_addr;			//Transport device address

	bool	prc_st,				//Process task active
		endrun_req;			//Request to stop of the Process task
	vector< AutoHD<TMdPrm> >  p_hd;

	pthread_t procPthr;			//Process task thread

	double	tm_gath;			//Gathering time
};

//*************************************************
//* SSerial                                       *
//*************************************************
class TTpContr;

class SSerial : public TCntrNode, public TConfig
{
    public:
	//Data
	enum	Prot 	{ Free, ASCII, RTU };

	//Methods
	SSerial( const string &dev, TTpContr *iown );

	string	id( )		{ return m_id; }
	int	speed( )	{ return m_speed; }
	int	len( )		{ return m_len; }
	bool	twostop( )	{ return m_twostop; }
	int	parity( )	{ return m_parity; }
	int	timeoutFrame( )	{ return frTm; }
	double	timeoutChar( )	{ return charTm; }
	int	timeoutReq( )	{ return reqTm; }
	bool	hasOpen( )	{ return (fd>0); }

	void	setSpeed( int val, bool tmAdj = false );
	void	setLen( int val );
	void	setTwostop( bool val );
	void	setParity( int val );
	void	setTimeoutFrame( int val )	{ frTm = val; modif(); }
	void	setTimeoutChar( double val )	{ charTm = val; modif(); }
	void	setTimeoutReq( int val )	{ reqTm = val; modif(); }
	void	setOpen( bool vl );

	string req( const string &vl, int req_len );

	TTpContr &owner( )	{ return *(TTpContr *)nodePrev(); }

    protected:
	//Methods
	string nodeName( )	{ return id(); }
	void postDisable( int flag );

	//- DB commands -
	void load_( );
	void save_( );

    private:
	//Attributes
	string	&m_id;				//Serial port device
	int	&m_speed;			//Speed
	int	&m_len;				//Length
	bool	&m_twostop;			//Two stop bits
	int	&m_parity;			//Parity check
	int 	&frTm;				//Frame timeout in ms
	double	&charTm;			//Char timeout in ms
	int	&reqTm;				//Request timeout in ms

	Res	m_res;				//Serial port resource
	int	fd;				//Serial port
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

	TElem &serDevE( )	{ return el_ser_dev; }

	//- Serial devices -
	string serDevDB( );
	void serDevList( vector<string> &list )		{ chldList(m_sdev,list); }
	void serDevAdd( const string &dev );
	void serDevDel( const string &dev, bool full = false )		{ chldDel(m_sdev,dev,-1,full); }
	bool serDevPresent( const string &dev )		{ return chldPresent(m_sdev,dev); }
	AutoHD<SSerial> serDevAt( const string &dev )	{ return chldAt(m_sdev,dev); }

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	void load_( );
	void save_( );

    private:
	//Methods
	void postEnable( int flag );
	TController *ContrAttach( const string &name, const string &daq_db );
	string optDescr( );

	//Attributes
	TElem		el_ser_dev;
	int		m_sdev;
};

extern TTpContr *mod;

} //End namespace

#endif //DCON_CLIENT_H
