
//OpenSCADA module DAQ.DCON file: DCON_client.h
/***************************************************************************
 *   Copyright (C) 2008-2011 by Almaz Karimov                              *
 *		   2008-2018 by Roman Savochenko, roman@oscada.org         *
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
#include <ttypedaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>
#include <map>

#undef _
#define _(mess) mod->I18N(mess).c_str()

#define MaxLenReq 200

using std::string;
using std::vector;
using std::map;
using namespace OSCADA;

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
	TMdPrm( string name, TTypeParam *tp_prm );
	~TMdPrm( );

	void enable( );
	void disable( );

	TElem &elem( )		{ return pEl; }
	TMdContr &owner( ) const;

	//Attributes
	TElem	pEl;		//Work atribute elements

	int64_t	&modAddr;	//Module address
	char	&crcCtrl;	//DCON CRC control mode
	int64_t	&hostSignal;

	int64_t	&aiMethod,
		&aiRange,
		&aoMethod,
		&aoRange,
		&diMethod,
		&doMethod,
		&ciMethod;

	ResString aiErr, aoErr, diErr, doErr, ciErr;

	//Values
	double	AI[32], AO[32], CI[32];
	char	DI[32], DO[32];

    protected:
	//Methods
	bool cfgChange( TCfg &co, const TVariant &pc );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	void postEnable( int flag );
	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
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

	double	period( )	{ return mPer; }
	string	cron( )		{ return cfg("SCHEDULE").getS(); }
	int	prior( )	{ return mPrior; }
	string	addr( ) const	{ return mAddr; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void prmEn( TMdPrm *prm, bool val );
	string DCONCRC( string str );
	string DCONReq( string &pdu, bool CRC = false, unsigned acqLen = 0, char resOK = '>' );

    protected:
	//Methods
	void load_( );
	void disable_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &co, const TVariant &pc );

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	ResMtx	enRes, reqRes;			//Resource for enable params and request values
	TCfg	&mAddr;				//Transport device address
	int64_t	&mPrior,			//Process task priority
		&connTry;			//Connections try

	bool	prcSt,				//Process task active
		callSt,				//Calc now stat
		endrunReq;			//Request to stop of the Process task
	vector< AutoHD<TMdPrm> >  pHd;

	double	mPer, tmGath;			//Gathering time
};

//*************************************************
//* TTpContr                                      *
//*************************************************
class TTpContr: public TTypeDAQ
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

#endif //DCON_CLIENT_H
