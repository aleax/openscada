
//OpenSCADA system module DAQ.ModBus file: modbus_daq.h
/***************************************************************************
 *   Copyright (C) 2007-2009 by Roman Savochenko                           *
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
 
#ifndef MODBUS_DAQ_H
#define MODBUS_DAQ_H

#include <string>
#include <vector>
#include <map>
#include <deque>

#include <tsys.h>

#undef _
#define _(mess) mod->I18N(mess)

#define MaxLenReq 200

using std::string;
using std::vector;
using std::map;
using std::deque;

//*************************************************
//* DAQ modul info!                               *
#define DAQ_ID		"ModBus"
#define DAQ_NAME	"ModBus"
#define DAQ_TYPE	"DAQ"
#define DAQ_SUBVER	VER_CNTR
#define DAQ_MVER	"0.9.1"
#define DAQ_AUTORS	"Roman Savochenko"
#define DAQ_DESCR	"Allow realisation of ModBus client service. Supported Modbus/TCP, Modbus/RTU and Modbus/ASCII protocols."
#define DAQ_LICENSE	"GPL"
//*************************************************

namespace ModBus
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

    protected:
	void	cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	void postEnable( int flag );
	void vlGet( TVal &val );
	void vlSet( TVal &val );
	void vlArchMake( TVal &val );

        //Attributes
	string	&m_attrLs;
	TElem	p_el;		//Work atribute elements
	string	acq_err;
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

	double	period( )	{ return vmax(mPer,0.01); }
	int	prior( )	{ return mPrior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void regVal( int reg, const string &dt = "R" );			//Register value for acquisition
	int  getValR( int addr, string &err, bool in = false );		//Get register value
	char getValC( int addr, string &err, bool in = false );		//Get coins value
	void setValR( int val, int addr, string &err );			//Set register value
	void setValC( char val, int addr, string &err );		//Set coins value
	string modBusReq( string &pdu );

    protected:
	//Methods
	void disable_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &cfg );

    private:
	//Data
	class SDataRec
	{
	    public:
		SDataRec( int ioff, int v_rez ) : off(ioff)
		{ val.assign(v_rez,0); err="11:Value not gathered."; }

		int	off;			//Data block start offset
		string	val;			//Data block values kadr
		string	err;			//Acquisition error text
	};

	//Methods
	TParamContr *ParamAttach( const string &name, int type );

	static void *Task( void *icntr );
	void setCntrDelay( const string &err );

	//Attributes
	Res     req_res;
	double	&mPer;				//Acquisition task (seconds)
	int	&mPrior,			//Process task priority
		&mNode;				//Node
	string	&mPrt,				//Protocol
		&mAddr;				//Transport device address
	bool	&mMerge;			//Fragments of register merge
	int	&reqTm,				//Request timeout in ms
		&restTm,			//Restore timeout in s
		&connTry;			//Connections try

	bool	prc_st,				//Process task active
		endrun_req;			//Request to stop of the Process task
	vector<SDataRec>	acqBlks;	//Acquisition data blocks for registers
	vector<SDataRec>	acqBlksIn;	//Acquisition data blocks for input registers
	vector<SDataRec>	acqBlksCoil;	//Acquisition data blocks for coils
	vector<SDataRec>	acqBlksCoilIn;	//Acquisition data blocks for input coils

	pthread_t	procPthr;		//Process task thread

	double	tmGath;			//Gathering time
	float	tmDelay;			//Delay time for next try connect

	float numRReg, numRRegIn, numRCoil, numRCoilIn, numWReg, numWCoil, numErrCon, numErrResp;
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
	void	load_( );
	void	save_( );

    private:
	//Methods
	void	postEnable( int flag );
	TController *ContrAttach( const string &name, const string &daq_db );
	string	optDescr( );
};

extern TTpContr *mod;

} //End namespace

#endif //MODBUS_DAQ_H
