
//OpenSCADA module DAQ.LogicLev file: logiclev.h
/***************************************************************************
 *   Copyright (C) 2006-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef LOGICLEV_H
#define LOGICLEV_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttypedaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#undef _
#define _(mess) mod->I18N(mess).c_str()
#undef trS
#define trS(mess) mod->I18N(mess,mess_PreSave)

using std::string;
using std::vector;
using namespace OSCADA;

namespace LogicLev
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

	TElem *dynElCntr( )	{ return &pEl; }

	TCntrNode &operator=( const TCntrNode &node );

	bool isStd( ) const;
        bool isPRefl( ) const;

	void enable( );
	void disable( );

	void calc( bool first, bool last, double frq );	//Calc template's algoritmes

	//Attributes
	float tmCalc, tmCalcMax;

    protected:
	//Methods
	void postEnable( int flag );
	void postDisable( int flag );
	void load_( );
	void save_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	void setType( const string &tpId );

	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &vo );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang );

	TMdContr &owner( ) const;

    private:
	//Data
	union {
	    AutoHD<TValue> *prmRefl;		//Direct reflection
	    TPrmTempl::Impl *tmpl;		//Template
	};

	//Methods
	void loadIO( bool force = false );
	void saveIO( );

	//Attributes
	TElem	pEl;				//Work atribute elements

	bool	chkLnkNeed;			//Check lnk need flag
	//ResRW	calcRes;			//Resource
	int	idFreq, idStart, idStop,
		idErr, idSh, idNm, idDscr;	//Fixed system attributes identifiers
};

//*************************************************
//* TMdContr                                      *
//*************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem);
	~TMdContr( );

	string	getStatus( );
	double	period( )			{ return mPer; }
	string	cron( )				{ return cfg("SCHEDULE").getS(); }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void redntDataUpdate( );

    protected:
	//Methods
	void prmEn( TMdPrm *p, bool val );
	void postDisable( int flag );		//Delete all DB if flag 1

	void load_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &co, const TVariant &pc );

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	ResMtx	enRes;				//Resource for enable params
	int64_t	&mPrior;			//Process task priority

	bool	prcSt,				//Process task active
		callSt,				//Calc now stat
		endrunReq;			//Request to stop of the Process task
	vector< AutoHD<TMdPrm> > pHd;

	double	mPer;
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

	TElem	&prmIOE( )	{ return elPrmIO; }

    protected:
	//Methods
	void postEnable( int flag );
	void load_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );

	//Attributes
	TElem	elPrmIO;
};

extern TTpContr *mod;

} //End namespace

#endif //LOGICLEV_H
