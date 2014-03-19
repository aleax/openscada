
//OpenSCADA system module DAQ.System file: os_contr.h
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#ifndef OS_CONTR_H
#define OS_CONTR_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#include "da.h"

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace SystemCntr
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

	void enable( );
	void disable( );

	void autoC( bool val )	{ mAuto = val; }
	bool autoC( )		{ return mAuto; }

	// Set perameter type
	void setType( const string &da_id );

	// Get new value
	void getVal( );
	void setEval( );

	void vlElemAtt( TElem *ValEl );
	void vlElemDet( TElem *ValEl );

	string addPrm( const string &prm, const string &def = "" );
	void setAddPrm( const string &prm, const string &val );

	TMdContr &owner( );

	//Attributes
	ResString daErr;		//DA error
	void	*daData;		//DA personal data

    protected:
	//Methods
	void load_( );
	void save_( );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &cfg );		//config change

	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &vo );

	void postEnable( int flag );

    private:
	//Attributes
	bool	mAuto;			//Autocreated
	DA	*mDA;
};

//*************************************************
//* TMdContr                                      *
//*************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, TElem *cfgelem );
	~TMdContr( );

	string getStatus( );

	double period( ){ return mPer; }
	string cron( )	{ return cfg("SCHEDULE").getS(); }
	int64_t prior( ){ return mPrior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void devUpdate( );

    protected:
	//Methods
	void load_( );
	void start_( );
	void enable_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	void prmEn( const string &id, bool val );

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	en_res;		//Resource for enable params
	int64_t	&mPerOld,	// ms
		&mPrior;	// Process task priority

	bool	prc_st,		// Process task active
		call_st,        // Calc now stat
		endrun_req;	// Request to stop of the Process task
	vector< AutoHD<TMdPrm> >  p_hd;

	double	mPer, tm_calc;	// Scheme's calc time
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

	void	daList( vector<string> &da );
	void	daReg( DA *da );
	DA	*daGet( const string &da );

	AutoHD<TMdContr> at( const string &name, const string &who = "" )
	{ return TTipDAQ::at(name,who); }

	void perSYSCall( unsigned int cnt );

    protected:
	//Methods
	void postEnable( int flag );
	void load_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );

	//Attributes
	vector<DA*>	mDA;
};

extern TTpContr *mod;

} //End namespace

#endif //OS_CONTR_H
