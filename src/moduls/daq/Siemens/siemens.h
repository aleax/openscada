
//OpenSCADA system module DAQ.Siemens file: siemens.h
/***************************************************************************
 *   Copyright (C) 2006-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef SIEMENS_H
#define SIEMENS_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttypedaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#include "nodave.h"

#undef _
#define _(mess) mod->I18N(mess)

#define MaxLenReq	240
#define ConnErrCode	10

using std::string;
using std::vector;
using namespace OSCADA;

namespace Siemens
{

//Constants
#define ISOTCP_TPKT_Version		3

enum COTP_TPS	{ COTP_CR = 0xE0, COTP_CC = 0xD0, COTP_DT = 0xF0 };
enum COTP_Class	{ COTP_NoExplFlCntr = 0x01, COTP_ExtForm = 0x02, COTP_LastDtUnit = 0x80 };
enum COTP_OPTS	{ COTP_O_SzTPDU = 0xC0, COTP_O_SrcTSAP = 0xC1, COTP_O_DstTSAP = 0xC2 };
enum ISOTCP_PDU_TAG { ISOTCP_OpenS7Connection = 0xF0, ISOTCP_Read = 0x04, ISOTCP_Write = 0x05 };

//************************************************
//* Value data structure                         *
//************************************************
class SValData
{
    public:
	SValData( int idb, int ioff, int isz ) : db(idb), off(ioff), sz(isz) { }

	int db;		//DB
	int off;	//Data offset
	int sz;		//Data size or Boolean bit
};

//************************************************
//* TMdPrm                                       *
//*----------------------------------------------*
//* Parammeters errors list:                     *
//* 1   - Parameter had disabled.                *
//* 2   - Controller is stopped.                 *
//* 10  - CIF device error.                      *
//* 11  - Value not gathered.                    *
//* 12  - Put request is timeouted.              *
//* 13  - Get request is timeouted.              *
//* 14  - Request block too big.                 *
//* 15  - Board %d no present.                   *
//* 16  - Transmit block too big.                *
//* 17  - No response of the remote station.     *
//* 18  - Master not into the logical token ring.*
//* 19  - Request to DB error %d.                *
//* 20  - Specified offset address or DB error.  *
//************************************************
class TMdContr;

class TMdPrm : public TParamContr, public TValFunc
{
    public:
	//Data
	class SLnk {
	    public:
		SLnk( int iid, const string &idbAddr = "" ) : ioId(iid), dbAddr(idbAddr), val(-1, -1, 0) { }

		int	ioId;	//Template function io index
		string	dbAddr;	//DB full address: DB1.20.1
		SValData val;	//Value address data
	};

	//Methods
	TMdPrm( string name, TTypeParam *tp_prm );
	~TMdPrm( );

	void enable( );
	void disable( );

	void calc( bool first, bool last, double frq );	//Calc template's algoritmes

	// Template link operations
	int lnkSize( );
	int lnkId( int id );
	int lnkId( const string &id );
	SLnk &lnk( int num );

	TMdContr &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void postEnable( int flag );
	void postDisable( int flag );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &val );

	void loadIO( bool force = false );
	void saveIO( );
	void initLnks( );

	//Attributes
	TElem	pEl;				//Work atribute elements

	int	idFreq, idStart, idStop, idErr, idSh, idNm, idDscr;	//Fixed system attributes identifiers
	vector<SLnk>	plnk;			//Parameter's links

	ResString	acqErr;
	time_t		acqErrTm;
};

//************************************************
//* TMdContr                                     *
//************************************************
class TTpContr;

class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Data
	enum Type { CIF_PB, ISO_TCP, ADS, ISO_TCP243, SELF_ISO_TCP };

	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string getStatus( );

	double period( )	{ return mPer; }
	string cron( )		{ return cfg("SCHEDULE").getS(); }
	string addr( )		{ return cfg("ADDR").getS(); }
	string addrTr( )	{ return cfg("ADDR_TR").getS(); }
	bool assincWrite( )	{ return mAssincWR; }
	Type type( )		{ return (Type)mType; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void connectRemotePLC( bool initOnly = false );
	void disconnectRemotePLC( );
	void getDB( unsigned n_db, long offset, string &buffer );
	void putDB( unsigned n_db, long offset, const string &buffer );

	void redntDataUpdate( );

	// Generic requests API, ISO-TCP only for now
	void reqService( XMLNode &io );
	void protIO( XMLNode &io );
	int messIO( const char *oBuf, int oLen, char *iBuf = NULL, int iLen = 0 );
	void reset( );

	TTpContr &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );
	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );

	bool cfgChange( TCfg &co, const TVariant &pc );
	void prmEn( const string &id, bool val );		//Enable parameter to process list
	void regVal( SValData ival, IO::Type itp, bool wr );	//Register value for acquisition
	// Values process
	char getValB( SValData ival, ResString &err );
	int64_t getValI( SValData ival, ResString &err );
	double getValR( SValData ival, ResString &err );
	string getValS( SValData ival, ResString &err );

	void setValB( bool ivl, SValData ival, ResString &err );
	void setValI( int64_t ivl, SValData ival, ResString &err );
	void setValR( double ivl, SValData ival, ResString &err );
	void setValS( const string &ivl, SValData ival, ResString &err );

	// Service
	void postDisable( int flag );				//Delete all DB if flag 1

	void cntrCmdProc( XMLNode *opt );			//Control interface command process

	// Decoding
	static const char *iVal( const string &buf, int &off, char vSz );
	static uint32_t iN( const string &rb, int &off, uint8_t vSz );		//Decode number from stream as BE
	// Encoding
	static void oN( string &buf, uint32_t val, uint8_t sz, int off = -1 );	//Encode number to stream as BE

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );
	void setCntrDelay( const string &err );
	int valSize( IO::Type itp, int iv_sz );			//Prepare value sizes
	string revers( const string &ibuf ) {
	    if(type() == ADS) return ibuf;
	    string obuf;
	    for(int i = ibuf.size()-1; i >= 0; i--) obuf += ibuf[i];
	    return obuf;
	}
	//Data
	class SDataRec {
	    public:
		SDataRec( int idb, int ioff, int v_rez );

		int db;			//Data block
		int off;		//Data block start offset
		string val;		//Data block values kadr
		ResString err;		//Acquisition error text
	};

	//Attributes
	int64_t	&mPerOld,		//ms
		&mPrior,		//Process task priority
		&mType,			//Connection type
		&mSlot,
		&mDev,			//CIF device number
		&restTm;		//Restore timeout in s
	char	&mAssincWR;		//Asynchronous write mode

	bool	prcSt,			//Process task active
		callSt,			//Calc now stat
		endrunReq,		//Request to stop of the Process task
		isReload,
		isInitiated;		//Only for SELF_ISO_TCP
	int8_t	alSt;			//Alarm state
	MtxString conErr;		//Connection error
	vector< AutoHD<TMdPrm> > pHd;	//Parameter's process list
	vector< SDataRec > acqBlks;	//Acquisition data blocks
	vector< SDataRec > writeBlks;	//Data block for write to a data source, for asynchronous write mode
	AutoHD<TTransportOut>	tr;	//OpenSCADA output transport, for ADS and SELF_ISO_TCP

	uint16_t mInvokeID;

	// LibnoDave specific
	daveInterface	*di;
	daveConnection	*dc;

	ResMtx	enRes,			//Access to pHd
		reqAPIRes;		//Access to local connection's API data, like for LibnoDave
	ResRW	reqDataRes,		//Access to generic request's data, mostly acqBlks
		reqDataAsWrRes;		//Access to writeBlks

	double	mPer, numR, numW, numErr;//Counters for read, wrote bytes and connection errors.
	float	tmDelay;		//Delay time for next try connect
};

//************************************************
//* TTpContr                                     *
//************************************************
class TTpContr: public TTypeDAQ
{
    friend class TMdContr;
    public:
	//Methods
	TTpContr( string name );
	~TTpContr( );

	// Devices
	bool drvCIFOK( );
	void initCIF( int dev );

	TElem	&prmIOE( )	{ return elPrmIO; }
	TElem	&CIFDevE( )	{ return elCifDev; }

	// Life list of PB stations
	void getLifeListPB( unsigned board, string &buffer );

	//Attributes
	ResRW	resAPI;

    protected:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void load_( );
	void save_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );

	//Attributes
	bool	drvCIF_OK;
	TElem	elCifDev, elPrmIO;

	struct SCifDev {
	    ResRW	res;		//Device resource
	    bool	present;	//Present flag
	    int		board;		//Board number
	    unsigned long phAddr;	//Physical address
	    short	irq;		//IRQ
	    string	fwname;		//Firmware device name
	    string	fwver;		//Firmware device version
	    int		pbaddr;		//PB device address
	    int		pbspeed;	//PB network speed
	};

	SCifDev	cif_devs[MAX_DEV_BOARDS];
};

extern TTpContr *mod;

//***********************************************************
//* Specific structures protocols'                          *
//***********************************************************
#pragma pack(push,1)

struct AMS_TCP_HEAD
{
    uint16_t reserv;
    uint32_t len;
};

struct AMS_HEAD
{
    char AMSNetId_targ[6];
    uint16_t AMSPort_targ;
    char AMSNetId_src[6];
    uint16_t AMSPort_src;
    uint16_t com, stFlgs;
    uint32_t len, errCod, InvId;
};

struct ADS_ReadWriteReq { uint32_t IdGrp, IdOff, len; };

struct ADS_ReadResp { uint32_t res, len; };

struct ADS_WriteResp { uint32_t res; };

#pragma pack(pop)

} //End namespace

#endif //SIEMENS_H
