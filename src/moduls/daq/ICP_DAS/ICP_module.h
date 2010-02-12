
//OpenSCADA system module DAQ.ICP_DAS file: ICP_module.h
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
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

using std::string;
using std::vector;
using std::map;

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

	void loadExtPrms( );
	void saveExtPrms( );

	TElem &elem( )		{ return p_el; }
	TMdContr &owner( );

	//Attributes
	TElem	p_el;		//Work atribute elements
	int	&modTp;		//I-7000,I-8000 module type
	int	&modAddr;	//I-7000,I-8000 module address
	int	&modSlot;	//I-8000 module slot
	string	&modPrms;	//Individual module extended parameters

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &cfg );

    private:
	//Data
	class PrmsI8017
	{
	    public:
		PrmsI8017( ) : init(false), prmNum(8), fastPer(0)
		{ for( int ip = 0; ip < 8; ip++ ) cnlMode[ip] = 0; }

		bool	init;
		char	prmNum;
		float	fastPer;
		char	cnlMode[8];
	};

	//Methods
	void postEnable( int flag );
	void vlGet( TVal &val );
	void vlSet( TVal &val, const TVariant &pvl );
	void vlArchMake( TVal &val );

	static void *fastTask( void *iprm );

	//Attributes
	bool		endRunReq, prcSt;
	int		clcCnt;
	ResString	acq_err;

	void	*extPrms;
	float	wTm;
	int	dOutRev, dInRev;
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

	double period( )	{ return vmax(mPer,0.01); }
	int    prior( )		{ return mPrior; }
	string prmLP( const string &prm );

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void prmEn( const string &id, bool val );
	void setPrmLP( const string &prm, const string &vl );

	string serReq( string req, char mSlot = 0 );

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
	Res	en_res, reqRes, pBusRes;	//Resource for enable params, request values and parallel bus devices
	double	&mPer;				//Acquisition task (seconds)
	int	&mPrior,			//Process task priority
		&mBus,				//Serial port address: 0-COM1(LP), 1-COM1, 2-COM2, ...
		&mBaud,				//Baudrate
		&connTry;			//Connections try
	string	&mLPprms;			//LinPAC parameters

	bool	prcSt,				//Process task active
		endRunReq;			//Request to stop of the Process task
	vector< AutoHD<TMdPrm> >  p_hd;

	double	tm_gath;			//Gathering time
	int	mCurSlot;
	float	numReq, numErr, numErrResp;

	//AutoHD<TTransportOut> tr;
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
};

extern TTpContr *mod;

} //End namespace

#endif //ICP_MODULE
