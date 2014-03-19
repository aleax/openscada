
//OpenSCADA system module DAQ.ICP_DAS file: ICP_module.h
/***************************************************************************
 *   Copyright (C) 2010-2014 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#ifndef ICP_MODULE_H
#define ICP_MODULE_H

#include <ttipdaq.h>

#include <string>
#include <vector>
#include <map>

#undef _
#define _(mess) mod->I18N(mess)

#define MaxLenReq 200
#define TrIcpDasNm "<ICP DAS>"

using std::string;
using std::vector;
using std::map;
using namespace OSCADA;

namespace ICP_DAS_DAQ
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

	void getVals( );
	string modPrm( const string &prm, const string &def = "" );

	void setModPrm( const string &prm, const string &val );

	TElem &elem( )		{ return p_el; }
	TMdContr &owner( );

	//Attributes
	TElem	p_el;		//Work atribute elements
	void	*extPrms;
	TCfg	&modTp;		//I-7000,I-8000 module type
	int64_t	&modAddr;	//I-7000,I-8000 module address
	int64_t	&modSlot;	//I-8000 module slot

	ResString acq_err;

	bool	endRunReq, prcSt;
	uint32_t dInOutRev[10];	//Up to 10 channels with 32 io each
	float	wTm;

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &cfg );

    private:
	//Methods
	void postEnable( int flag );
	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &val );

	//Attributes
	int	clcCnt;

	DA	*da;
};

//******************************************************
//* TMdContr                                           *
//******************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, TElem *cfgelem);
	~TMdContr( );

	string getStatus( );

	int64_t	period( )	{ return mPer; }
	string	cron( )		{ return mSched; }
	int	prior( )	{ return mPrior; }
	int	bus( ) 		{ return mBus; }
	string	trOscd( )	{ return mTrOscd; }
	string prmLP( const string &prm );

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void prmEn( const string &id, bool val );
	void setPrmLP( const string &prm, const string &vl );

	string serReq( string req, char mSlot = 0, bool CRC = false );

	Res	reqRes, pBusRes;	//Resource for request values and parallel bus devices

    protected:
	//Methods
	void disable_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	bool cfgChange( TCfg &cfg );

	string DCONCRC( string str );

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	en_res;				//Resource for enable params
	int64_t	&mPrior,			//Process task priority
		&mBus,				//Serial port address: 0-COM1(LP), 1-COM1, 2-COM2, ...
		&mBaud,				//Baudrate
		&connTry;			//Connections try
	TCfg	&mSched,			//Calc schedule
		&mTrOscd;			//OpenSCADA transport
	int64_t mPer;

	bool	prcSt,				//Process task active
		call_st,			//Calc now stat
		endRunReq;			//Request to stop of the Process task
	vector< AutoHD<TMdPrm> > p_hd;
	AutoHD<TTransportOut>	tr;

	double	tm_gath;			//Gathering time
	int	mCurSlot;
	float	numReq, numErr, numErrResp;
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

	void	daReg( DA *da );
	void	daTpList( TMdPrm *prm, vector<string> &tpl, vector<string> *ntpl = NULL );
	DA	*daGet( TMdPrm *prm );

    protected:
	//Methods
	void load_( );
	void save_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	void postEnable( int flag );
	TController *ContrAttach( const string &name, const string &daq_db );

	//Attributes
	vector<DA *> m_da;
};

extern TTpContr *mod;

} //End namespace

#endif //ICP_MODULE
