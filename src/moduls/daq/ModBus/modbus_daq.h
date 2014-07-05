
//OpenSCADA system module DAQ.ModBus file: modbus_daq.h
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

using std::string;
using std::vector;
using std::map;
using std::deque;
using namespace OSCADA;

//*************************************************
//* DAQ modul info!                               *
#define DAQ_ID		"ModBus"
#define DAQ_NAME	_("ModBUS")
#define DAQ_TYPE	SDAQ_ID
#define DAQ_SUBVER	SDAQ_VER
#define DAQ_MVER	"1.3.0"
#define DAQ_AUTHORS	_("Roman Savochenko")
#define DAQ_DESCR	_("Allow realization of ModBus client service. Supported Modbus/TCP, Modbus/RTU and Modbus/ASCII protocols.")
#define DAQ_LICENSE	"GPL2"
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

	TCntrNode &operator=( TCntrNode &node );

	bool isStd( );
	bool isLogic( );

	void enable( );
	void disable( );

	void upVal( bool first, bool last, double frq );

	TElem &elem( )		{ return p_el; }
	TMdContr &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	void setType( const string &tpId );

    private:
	//Methods
	void postEnable( int flag );
	void postDisable( int flag );
	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &vo );

	//Attributes
	TElem		p_el;		//Work atribute elements
	ResString	acq_err;

	//> Logical type by template
	//Data
	//***************************************************
	//* Logical type parameter's context                *
	class TLogCtx : public TValFunc
	{
	    public:
	    //Data
	    //>> Link structure
	    class SLnk
	    {
		public:
		SLnk(int iid, const string &iaddr = "") : io_id(iid), addr(iaddr) { }

		int	io_id;		//Template function io index
		string	addr, real;	//Full item address: R:23
	    };

	    //Methods
	    TLogCtx( const string &name );

	    //>> Link operations
	    int lnkSize( )		{ return plnk.size(); }
	    int lnkId( int id );
	    int lnkId( const string &id );
	    SLnk &lnk( int num );

	    //Attributes
	    int	id_freq, id_start, id_stop, id_err, id_sh, id_nm, id_dscr;	//Fixed system attributes identifiers
	    vector<SLnk>	plnk;		//Parameter's links
	};

	//Methods
	void loadIO( bool force = false );
	void saveIO( );
	void initLnks( );

	//Attributes
	TLogCtx	*lCtx;
};

//******************************************************
//* TMdContr                                           *
//******************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr(string name_c, const string &daq_db, TElem *cfgelem);
	~TMdContr( );

	string	getStatus( );

	int64_t	period( )	{ return mPer; }
	string	cron( )		{ return mSched; }
	string	addr( )		{ return mAddr; }
	int	prior( )	{ return mPrior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void regVal( int reg, const string &dt = "R" );			//Register value for acquisition
	TVariant getVal( const string &addr, ResString &err );		//Unified value request from string address
	int64_t getValR( int addr, ResString &err, bool in = false );	//Get register value
	char getValC( int addr, ResString &err, bool in = false );	//Get coins value
	bool setVal( const TVariant &val, const string &addr, ResString &err, bool chkAssync = false );	//Unified value set by string address
	bool setValR( int val, int addr, ResString &err );		//Set register value
	bool setValRs( const map<int,int> &regs, ResString &err );	//Set multiply registers
	bool setValC( char val, int addr, ResString &err );		//Set coins value
	string modBusReq( string &pdu );

    protected:
	//Methods
	void postDisable( int flag );		//Delete all DB if flag 1
	void disable_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &co, const TVariant &pc );
	void prmEn( TMdPrm *prm, bool val );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Data
	class SDataRec
	{
	    public:
		SDataRec( int ioff, int v_rez );

		int		off;		//Data block start offset
		string		val;		//Data block values kadr
		ResString	err;		//Acquisition error text
	};

	//Methods
	TParamContr *ParamAttach( const string &name, int type );

	static void *Task( void *icntr );
	void setCntrDelay( const string &err );

	//Attributes
	pthread_mutex_t	enRes, dataRes;
	Res	reqRes;
	int64_t	&mPrior,			//Process task priority
		&mNode,				//Node
		&blkMaxSz;			//Maximum request block size
	TCfg	&mSched,			//Calc schedule
		&mPrt,                          //Protocol
		&mAddr;                         //Transport device address
	char	&mMerge,			//Fragments of register merge
		&mMltWr,			//Use multi-write functions(15,16)
		&mAsynchWr;			//Asynchronous write
	int64_t	&reqTm,				//Request timeout in ms
		&restTm,			//Restore timeout in s
		&connTry;			//Connections try
	int64_t	mPer;

	bool	prc_st,				//Process task active
		call_st,			//Calc now stat
		endrun_req,			//Request to stop of the Process task
		isReload;
	int8_t	alSt;				//Alarm state
	vector<SDataRec>	acqBlks;	//Acquisition data blocks for registers
	vector<SDataRec>	acqBlksIn;	//Acquisition data blocks for input registers
	vector<SDataRec>	acqBlksCoil;	//Acquisition data blocks for coils
	vector<SDataRec>	acqBlksCoilIn;	//Acquisition data blocks for input coils
	map<string,string>	asynchWrs;	//Asynchronous writers list

	float	tmDelay;			//Delay time for next try connect

	vector< AutoHD<TMdPrm> > pHd;

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

	TElem	&prmIOE( )	{ return el_prm_io; }

    protected:
	//Methods
	void	load_( );
	void	save_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	void	postEnable( int flag );
	TController *ContrAttach( const string &name, const string &daq_db );

	//Attributes
	TElem	el_prm_io;
};

extern TTpContr *mod;

} //End namespace

#endif //MODBUS_DAQ_H
