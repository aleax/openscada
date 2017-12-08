
//OpenSCADA system module DAQ.SoundCard file: sound.h
/***************************************************************************
 *   Copyright (C) 2008-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef SOUND_H
#define SOUND_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttypedaq.h>

#include <string>
#include <vector>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace SoundCard
{

//*************************************************
//* TMdPrm                                        *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    public:
	//Methods
	TMdPrm( string name, TTypeParam *tp_prm );
	~TMdPrm( );

	int64_t iCnl( )			{ return mCnl; }
	void setICnl( int64_t val )	{ mCnl = val; modif(); }

	void enable( );
	void disable( );

	TMdContr &owner( ) const;

    protected:
	//Methods
	void vlArchMake( TVal &val );

	void cntrCmdProc( XMLNode *opt );

	void postEnable( int flag );

    private:
	//Attributes
	int64_t	&mCnl;
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
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string	card( )		{ return cfg("CARD").getS(); }
	int	prior( )	{ return mPrior; }

	string getStatus( );
	int channelAllow( );
	string sampleRates( );

	TParamContr *ParamAttach( const string &name, int type );

	TElem &prmEL( )		{ return pEl; }

	void start_( );
	void stop_( );

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	TTpContr &owner( ) const;

	ResRW &nodeRes( )	{ return nRes; }

    protected:
	//Methods
	void prmEn( const string &id, bool val );	//Enable parameter to process list
	static int recordCallback( const void *iBuf, void *oBuf, unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData );

	bool cfgChange( TCfg &co, const TVariant &pc );
	void cntrCmdProc( XMLNode *opt );		//Control interface command process

    private:
	//Attributes
	TElem	pEl;					//Work atribute elements
	int64_t	&mSmplRate, &mSmplType, &mPrior;
	bool	prcSt, endrunReq, firstCall;
	int	numChan, smplSize;
	vector< AutoHD<TMdPrm> > pHd;			//Parameter's process list

	PaStream *stream;
	time_t	curTm[10], corTm;
	int64_t	wTm;
	int	sRt;
	float	acqSize, cntCor;

	ResRW	nRes;
};

//*************************************************
//* TTpContr                                      *
//*************************************************
class TTpContr: public TTypeDAQ
{
    public:
	//Methods
	TTpContr( string name );
	~TTpContr();

	void postEnable( int flag );

	TController *ContrAttach( const string &name, const string &daq_db );

    protected:
	//Attributes
	bool redntAllow( )	{ return true; }

    private:
	//Attributes
};

extern TTpContr *mod;

} //End namespace

#endif //SOUND_H
