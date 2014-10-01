
//OpenSCADA system module DAQ.LogicLev file: logiclev.h
/***************************************************************************
 *   Copyright (C) 2006-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#undef _
#define _(mess) mod->I18N(mess)

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
	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );

	TCntrNode &operator=( TCntrNode &node );

	bool isStd( );
        bool isPRefl( );

	void enable( );
	void disable( );

	void calc( bool first, bool last, double frq );	//Calc template's algoritmes

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

	TMdContr &owner( );

    private:
	//Data
	class SLnk {
	    public:
		SLnk( int iid, const string &iprm_attr = "" ) : io_id(iid), prm_attr(iprm_attr) { }
		int	io_id;
		string	prm_attr;
		AutoHD<TVal> aprm;
	};

	struct STmpl {
	    TValFunc	val;
	    vector<SLnk> lnk;
	};

	union {
	    AutoHD<TValue> *prm_refl;		//Direct reflection
	    STmpl *tmpl;			//Template
	};

	//Methods
	// Template link operations
	int lnkSize( );
	int lnkId( int id );
	int lnkId( const string &id );
	SLnk &lnk( int num );

	void loadIO( bool force = false );
	void saveIO( );
	void initTmplLnks( bool checkNoLink = false );

	//Attributes
	TElem	p_el;				//Work atribute elements

	bool	chk_lnk_need;			//Check lnk need flag
	Res	calcRes;			//Resource
	int	id_freq, id_start, id_stop, id_err, id_sh, id_nm, id_dscr;	//Fixed system attributes identifiers
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

	string getStatus( );
	double period( )			{ return mPer; }
        string cron( )				{ return cfg("SCHEDULE").getS(); }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void redntDataUpdate( );

    protected:
	//Methods
	void prmEn( TMdPrm *p, bool val );
	void postDisable( int flag );		//Delete all DB if flag 1

	void load_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	pthread_mutex_t	enRes;			//Resource for enable params
	int64_t	&mPerOld,			// ms
		&mPrior;			// Process task priority

	bool	prc_st,				// Process task active
		call_st,        		// Calc now stat
		endrun_req;			// Request to stop of the Process task
	vector< AutoHD<TMdPrm> >  p_hd;

	double	mPer;
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
	void postEnable( int flag );
	void load_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );

	//Attributes
	TElem	el_prm_io;
};

extern TTpContr *mod;

} //End namespace

#endif //LOGICLEV_H
