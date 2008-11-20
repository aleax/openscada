
//OpenSCADA system module DAQ.Transporter file: transporter.h
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
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
 
#ifndef TRANSPORTER_H
#define TRANSPORTER_H

#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;

namespace DAQTrasport
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

	bool isDel( )		{ return m_pdel; }
	string cntrAdr( )	{ return m_cntr_adr; }

	void setCntrAdr( const string &vl );

	void enable( );
	void disable( );

	void update( );				//Update parameter

	TElem &elem( )		{ return p_el; }
	TMdContr &owner( )	{ return (TMdContr&)TParamContr::owner(); }

    protected:
	//Methods
	void load_( );				//Synchronize parameter
	void save_( );				//Save blocking
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	void postEnable( int flag );
	void vlSet( TVal &val );
	void vlArchMake( TVal &val );

	//Attributes
	TElem	p_el;				//Work atribute elements
	bool	m_pdel;				//Remote parameter deleted flag
	string	m_cntr_adr;			//Parameter's remote controller address'
};

//******************************************************
//* TMdContr                                           *
//******************************************************
class TMdContr: public TController
{
    public:
	//Methods
	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem );
	~TMdContr( );

	double	period( )	{ return vmax(m_per,0.1); }
	int	prior( )	{ return m_prior; }
	double	syncPer( )	{ return m_sync; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void prmEn( const string &id, bool val );

    protected:
	//Methods
	void enable_( );
	void start_( );
	void stop_( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	//Attributes
	Res	en_res, req_res;		//Resource for enable params and request to remote OpenSCADA station
	double	&m_per,				//Acquisition task (seconds)
		&m_sync;			//Synchronization inter remote OpenSCADA station:
						//configuration update, attributes list update, local and remote archives sync.
	int	&m_prior;			//Process task priority
	string	&m_stations,			//Remote stations list
		&m_contr_prm;			//Transported remote cotrollers and parameters list

	bool	prc_st,				//Process task active
		endrun_req;			//Request to stop of the Process task
	vector< AutoHD<TMdPrm> > p_hd;

	pthread_t procPthr;			//Process task thread

	double	tm_gath;			//Gathering time
};

//******************************************************
//* TTpContr                                           *
//******************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Data
	class RemHost
	{
	    public:
		//Methods
		RemHost( const string &ist, const string &itransp, const string &iaddr,
			const string &iuser, const string &ipass ) :
		    stat(ist), transp(itransp), addr(iaddr), user(iuser), pass(ipass),
		    ses_id(-1), link_ok(false) { }

		//Attributes
		string	stat;		//External station
		string	transp;		//Connect transport
		string	addr;		//External host address
		string	user;		//External host user
		string	pass;		//External host password
		int	ses_id;		//Session ID
		bool	link_ok;	//Link OK
	};

	//Methods
	TTpContr( string name );
	~TTpContr( );

	//- Request to OpenSCADA control interface -
	int cntrIfCmd( XMLNode &node );

    protected:
	//Methods
	void postEnable( int flag );
	void load_( );

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );
	string optDescr( );

	//Attributes
	vector<RemHost>	openHosts;
};

extern TTpContr *mod;

} //End namespace

#endif //TRANSPORTER_H
