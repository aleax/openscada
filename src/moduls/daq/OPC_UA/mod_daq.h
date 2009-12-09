
//OpenSCADA system module DAQ.OPC_UA file: mod_daq.h
/***************************************************************************
 *   Copyright (C) 2009 by Roman Savochenko                                *
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

#ifndef MOD_OPC_UA_H
#define MOD_OPC_UA_H

#include <string>
#include <vector>

#include <tsys.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;

//*************************************************
//* DAQ modul info!                               *
#define DAQ_ID		"OPC_UA"
#define DAQ_NAME	"OPC UA"
#define DAQ_TYPE	"DAQ"
#define DAQ_SUBVER	VER_CNTR
#define DAQ_MVER	"0.0.1"
#define DAQ_AUTOR	"Roman Savochenko"
#define DAQ_DESCR	"Allow realisation of OPC UA client service."
#define DAQ_LICENSE	"GPL"
//*************************************************

namespace OPC_UA
{
//*************************************************
//* OPC_UA::TMdPrm                                *
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

	TMdContr &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void vlArchMake( TVal &val );

	//Attributes
	TElem	p_el;			//Work atribute elements
};

//*************************************************
//* OPC_UA::TMdContr                              *
//*************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	string getStatus( );

	double	period( )	{ return vmax(m_per,0.1); }
	int	prior( )	{ return m_prior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

    protected:
	//Methods
	void prmEn( const string &id, bool val );

	void start_( );
	void stop_( );

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	en_res;		//Resource for enable params
	int	&m_per,		// s
		&m_prior;	// Process task priority

	bool	prc_st,		// Process task active
		endrun_req;	// Request to stop of the Process task

	vector< AutoHD<TMdPrm> >  p_hd;

	double	tm_gath;	// Gathering time
};

//*************************************************
//* OPC_UA::TTpContr                              *
//*************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Methods
	TTpContr( string name );
	~TTpContr( );

    protected:
	//Methods
	void postEnable( int flag );

	void load_( );
	void save_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );

	string optDescr( );
};

extern TTpContr *mod;

} //End namespace OPC_UA

#endif //MOD_OPC_UA_H
