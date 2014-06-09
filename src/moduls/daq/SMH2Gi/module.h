
//OpenSCADA system module DAQ.SMH2Gi file: module.h
/***************************************************************************
 *   Copyright (C) 2012-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>

#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace SMH2Gi
{
//****************************************************
//* SMH2Gi::SHMParam - shared memory parameter type  *
//****************************************************
class SHMParam : public TTipParam
{
    public:
	//Methods
	SHMParam( );

	void enable( TParamContr *prm );
        void disable( TParamContr *prm )	{ }

	void getVals( TParamContr *prm );
	void vlSet( TParamContr *prm, TVal &vo, const TVariant &vl, const TVariant &pvl );

	bool cntrCmdProc( TParamContr *prm, XMLNode *opt );
};

//*************************************************
//* SMH2Gi::DevMRCFeature                         *
//*************************************************
class DevMRCFeature
{
    public:
	//Data
	class SVal
	{
	    public:
		SVal( ) : addr(-1), tp(TFld::Boolean), wr(false)	{ }
	    string	descr;
	    int		addr;
	    TFld::Type	tp;
	    bool	wr;
	    string	visible;
	};

	//Functions
        DevMRCFeature( const string &iniFile );
        DevMRCFeature( ) : HardID(0)	{ }

	bool load( const string &iniFile );

	//Attributes
	int HardID;	// Compared with device
	string	name, descr;
	map<string, map<string, string> > sects;
	map<string, SVal>	vars;
};

//****************************************************
//* SMH2Gi::MRCParam - MR and MC bus modules         *
//****************************************************
class MRCParam : public TTipParam
{
    public:
	//Data
        class tval
        {
            public:
                tval( ) : SoftID(-1), SN(-1), DI(0), DO(0), AO(0), CNTR(0), diRev(0), doRev(0)	{ }

		int SoftID;
		int SN;

		int DI, DO, AO, CNTR;
		uint32_t diRev, doRev;

                DevMRCFeature dev;
        };

	//Methods
	MRCParam( );

	void enable( TParamContr *prm );
        void disable( TParamContr *prm );

	void getVals( TParamContr *prm );
	void vlSet( TParamContr *prm, TVal &vo, const TVariant &vl, const TVariant &pvl );

	bool cntrCmdProc( TParamContr *prm, XMLNode *opt );

    private:
	//Methods
	void sendTune( TParamContr *prm );
};

//*************************************************
//* SMH2Gi::TMdPrm                                *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Methods
	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );

	TElem &elem( )		{ return p_el; }

	void enable( );
	void disable( );

	void vlGet( TVal &val );
	void getVals( );
	void setEval( );

	string modPrm( const string &prm, const string &def = "" );
        void setModPrm( const string &prm, const string &val );

	TMdContr &owner( );

	//Attributes
	ResString acq_err;

	TElem	p_el;				//Work atribute elements
	void    *extPrms;
	vector<string> als;

    protected:
	//Methods
	void load_( );
	void save_( );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	void postEnable( int flag );
	void vlArchMake( TVal &val );
};

//*************************************************
//* SMH2Gi::TMdContr                              *
//*************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string getStatus( );

	int64_t	period( )	{ return mPer; }
	string  cron( )         { return cfg("SCHEDULE").getS(); }
	int	prior( )	{ return m_prior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	string modBusReq( string &pdu, bool MC = false, bool broadCast = false );	//pdu included address, by specific

	//Attributes
	Shm     *smv;
	string	MRWrFrm;

    protected:
	//Methods
	void prmEn( const string &id, bool val );

	void enable_( );
	void disable_( );
	void start_( );
	void stop_( );

	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	static uint8_t CRCHi[];
        static uint8_t CRCLo[];
	uint16_t CRC16( const string &mbap );

	//Attributes
	Res	en_res;		// Resource for enable params
	int64_t	&m_prior,	// Process task priority
                &connTry;	// Connections try
	int64_t	mPer;

	bool	prcSt,		// Process task active
		callSt,		// Calc now stat
		endrunReq;	// Request to stop of the Process task

	vector< AutoHD<TMdPrm> >  p_hd;

	double	tmGath;		// Gathering time
};

//*************************************************
//* SMH2Gi::TTpContr                              *
//*************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Methods
	TTpContr( string name );
	~TTpContr( );

	string MRCDirDevs( )	{ return mMRCDirDevs; }
	void setMRCDirDevs( const string &vl );

	void perSYSCall( unsigned int cnt );

	//Attributes
	map<int, DevMRCFeature>	MRCdevs;
	Res			MRCdevsRes;

    protected:
	//Methods
	void postEnable( int flag );
	TController *ContrAttach( const string &name, const string &daq_db );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	void load_( );
	void save_( );

	bool redntAllow( )	{ return true; }

    private:
	//Attributes
	string mMRCDirDevs;
};

extern TTpContr *mod;

} //End namespace SMH2Gi

#endif //MODULE_H
