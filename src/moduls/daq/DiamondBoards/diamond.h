
//OpenSCADA system module DAQ.DiamondBoards file: diamond.h
/***************************************************************************
 *   Copyright (C) 2005-2012 by Roman Savochenko                           *
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

#ifndef DIAMOND_H
#define DIAMOND_H

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
using namespace OSCADA;

namespace Diamond
{
//*************************************************
//* DevFeature                                    *
//*************************************************
class DevFeature
{
    public:
	//Data
	struct rng { float min, max; };
	//Functions
        DevFeature( const string &inm, unsigned iAI, unsigned iAO = 0, unsigned iDIO = 0, unsigned iDI = 0, unsigned iDO = 0 ) :
	    name(inm), AI(iAI), AO(iAO), DIO(iDIO), DI(iDI), DO(iDO) { }
        DevFeature( ) : AI(0), AO(0), DIO(0), DI(0), DO(0)	{ }

	void setAITypes( const string &vl );
	void setAOTypes( const string &vl )	{ aoTypes = vl; }

	string	 name;	//Device name

	unsigned AI;	//[res][smpl][cnls]	0x6410	smpl:interrupted samplerate into ksml/s; res:A/D resolution (bits), default 16
	unsigned AO;	//[res][cnls]		res:D/A resolution (bits), default 12
	unsigned DIO;	//[ports]	0x0002  ports*8
        unsigned DI;    //[ports]	0x0002  ports*8
        unsigned DO;    //[ports]	0x0002  ports*8

	string	aiTypes,	//Two string with modes indexes and names. Indexes code: [diff|range|polar][gain]
		aoTypes;	//Two string with modes indexes and names. Indexes code: [daPol|range|polar][gain]
	map<int, rng>	aiRngs;	//Parsed from aiTypes ranges for voltage values calculate
};

//*************************************************
//* TMdPrm                                        *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Methods
	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );

	TElem &elem( )          { return p_el; }

        void enable( );
        void disable( );

        void getVals( const string &atr = "" );
        string modPrm( const string &prm, const string &def = "" );

        void setModPrm( const string &prm, const string &val );

	TMdContr &owner( );

    private:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void vlGet( TVal &val );
	void vlSet( TVal &val, const TVariant &pvl );
	void vlArchMake( TVal &val );

	string errDSC( const string &func );

	//Attributes
	TElem	p_el;		//Work atribute elements
        int	&mTP,		//Board type
		&mADDR,		//Board address
		&mINT,		//Board interrupt
		&mAImode;	//AI values mode
	char	&asynchRd;	//Asynchronous reading

        Res	dev_res;	//Resource for access to device
	DevFeature dev;
	uint32_t dInOutRev[10];	//Up to 10 channels with 32 io each

	DSCB	dscb;		//Board descriptor
	ResString acq_err;
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
	TMdContr( string name_c, const string &daq_db, TElem *cfgelem );
	~TMdContr( );

	string	getStatus( );

        int64_t	period( )	{ return mPer; }
        string	cron( )		{ return mSched; }
        int	prior( )	{ return mPrior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void prmEn( const string &id, bool val );

    protected:
	//Methods
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	en_res;				//Resource for enable params
	int	&mPrior;			//Process task priority
	TCfg	&mSched;			//Calc schedule
	int64_t	mPer;

        bool	prcSt,				//Process task active
		call_st,			//Calc now stat
		exec_calc;			//Request for execution time measurement
        vector< AutoHD<TMdPrm> > p_hd;
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

	bool drvInitOk( )	{ return mInit; }
	void postEnable( int flag );

	//Attributes
	map<int, DevFeature> devs;

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );
	bool redntAllow( )	{ return true; }

	//Attributes
	bool	mInit;
};

extern TTpContr *mod;

} //End namespace

#endif //DIAMOND_H
