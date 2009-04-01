
//OpenSCADA system module DAQ.ModBus file: modbus_client.h
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
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
 
#ifndef MODBUS_CLIENT_H
#define MODBUS_CLIENT_H

#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>
#include <map>
#include <deque>

#undef _
#define _(mess) mod->I18N(mess)

#define MaxLenReq 200

using std::string;
using std::vector;
using std::map;
using std::deque;

namespace ModBusDAQ
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

	double	period( )	{ return vmax(m_per,0.01); }
	int	prior( )	{ return m_prior; }

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
	Res	en_res, req_res;		//Resource for enable params and request values
	double	&m_per;				//Acquisition task (seconds)
	int	&m_prior,			//Process task priority
		&m_prt,				//Protocol
		&m_node;			//Node
	string	&m_addr;			//Transport device address
	bool	&m_merge;			//Fragments of register merge
	int	&reqTm;				//Request timeout in s

	bool	prc_st,				//Process task active
		endrun_req;			//Request to stop of the Process task
	vector< SDataRec >	acqBlks;	//Acquisition data blocks for registers
	vector< SDataRec >	acqBlksIn;	//Acquisition data blocks for input registers
	vector< SDataRec >	acqBlksCoil;	//Acquisition data blocks for coils
	vector< SDataRec >	acqBlksCoilIn;	//Acquisition data blocks for input coils

	pthread_t	procPthr;		//Process task thread

	double	tm_gath;			//Gathering time
	float	tm_delay;			//Delay time for next try connect

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

	int connResume( )	{ return mConnResume; }
	void setConnResume( int vl )	{ mConnResume = vl; modif(); }

	//> Special modbus protocol's
	ui16	CRC16( const string &mbap );
	ui8	LRC( const string &mbap );
	string	DataToASCII( const string &in );
	string	ASCIIToData( const string &in );

	//> Protocol
	int prtLen( )	{ return mPrtLen; }
	void setPrtLen( int vl );
	void pushPrtMess( const string &vl );

    protected:
	//Methods
	void	cntrCmdProc( XMLNode *opt );	//Control interface command process
	void	load_( );
	void	save_( );

    private:
	//Methods
	void	postEnable( int flag );
	TController *ContrAttach( const string &name, const string &daq_db );
	string	optDescr( );

	int	mConnResume;

	//Attributes
	//> Protocol
	Res	mPrtRes;
	int	mPrtLen;
	deque<string>	mPrt;

	//> Special modbus protocol's
	static ui8 CRCHi[];
	static ui8 CRCLo[];
};

extern TTpContr *mod;

} //End namespace

#endif //MODBUS_CLIENT_H
