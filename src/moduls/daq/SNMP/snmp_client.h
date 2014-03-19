
//OpenSCADA system module DAQ.SNMP file: snmp_client.h
/***************************************************************************
 *   Copyright (C) 2006-2014 by Roman Savochenko                           *
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

#ifndef SNMP_DAQ_H
#define SNMP_DAQ_H

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

namespace SNMP_DAQ
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

	string OIDList( )	{ return cfg("OID_LS").getS(); }
	vector<string> &lsOID( ){ return ls_oid; }
	TElem &elem( )		{ return p_el; }

	void enable( );
	void disable( );

	void upVal( void *ss, bool onlyInit = false );

	TMdContr &owner( );

    protected:
	//Methods
	void load_( );

    private:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &val );

	void parseOIDList( const string &ioid );

	//Attributes
	vector<string>	ls_oid;		//Parsed IOD list.

	TElem	p_el;			//Work atribute elements
};

//*************************************************
//* TMdContr                                      *
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
	string	cron( )		{ return cfg("SCHEDULE").getS(); }
	int	prior( )	{ return m_prior; }
	string	ver( )		{ return cfg("VER").getS(); }
	int	pAttrLimit( )	{ return m_pattr_lim; }
	struct snmp_session *getSess( );

	string	secLev( );
	string	secAuthProto( );
	string	secAuthPass( );
	string	secPrivProto( );
	string	secPrivPass( );

	void	setSecLev(const string &vl);
	void	setSecAuthProto(const string &vl);
	void	setSecAuthPass(const string &vl);
	void	setSecPrivProto(const string &vl);
	void	setSecPrivPass(const string &vl);

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

    protected:
	//Methods
	void prmEn( const string &id, bool val );
	void enable_( );
	void start_( );
	void stop_( );

	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	string oid2str( oid *ioid, size_t isz );
	void str2oid( const string &str, oid *ioid, size_t &isz );

	//Attributes
	Res	en_res;		// Resource for enable params
	int64_t	&m_prior,	// Process task priority
		&m_pattr_lim,	// Parameter's attributes limit
		&m_retr,	// Request retries
		&m_tm;		// Request timeout
	string	w_addr, w_comm;
	int64_t	mPer;

	bool	prc_st,		// Process task active
		call_st,	// Calc now stat
		endrun_req,	// Request to stop of the Process task
		prmEnErr;	// Error connection pass for enable parameters
	vector< AutoHD<TMdPrm> >  p_hd;

	double	tm_gath;	// Gathering time
	ResString acq_err;

	struct snmp_session session;
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
	void postEnable( int flag );
	void load_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );
};

extern TTpContr *mod;

} //End namespace

#endif //SNMP_DAQ_H
