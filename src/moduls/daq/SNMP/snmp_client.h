
//OpenSCADA module DAQ.SNMP file: snmp_client.h
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

#ifndef SNMP_DAQ_H
#define SNMP_DAQ_H

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
	TMdPrm( string name, TTypeParam *tp_prm );
	~TMdPrm( );

	string OIDList( )	{ return cfg("OID_LS").getS(); }
	void setOIDList( const string &vl )	{ cfg("OID_LS").setS(vl); }
	vector<string> &lsOID( ){ return ls_oid; }
	TElem &elem( )		{ return p_el; }

	void enable( );
	void disable( );

	void upVal( void *ss, bool onlyInit = false );

	TMdContr &owner( ) const;

    private:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &val );

	void parseOIDList( const string &ioid );

	//Attributes
	vector<string>	ls_oid;		//Parsed IOD list.

	TElem	p_el;			//Work atribute elements
	MtxString acqErr;
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
	int	prior( )	{ return mPrior; }
	string	ver( )		{ return cfg("VER").getS(); }
	int	pAttrLimit( )	{ return mPattrLim; }
	struct snmp_session *getSess( );

	string	secLev( );
	string	secAuthProto( );
	string	secAuthPass( );
	string	secPrivProto( );
	string	secPrivPass( );

	void	setSecLev( const string &vl );
	void	setSecAuthProto( const string &vl );
	void	setSecAuthPass( const string &vl );
	void	setSecPrivProto( const string &vl );
	void	setSecPrivPass( const string &vl );

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

    protected:
	//Methods
	void prmEn( TMdPrm *prm, bool val );
	void enable_( );
	void start_( );
	void stop_( );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	bool cfgChange( TCfg &co, const TVariant &pc );

    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );

	string oid2str( oid *ioid, size_t isz, const string &sep = "_" );
	void str2oid( const string &str, oid *ioid, size_t &isz, const string &sep = "_" );

	//Attributes
	ResMtx	enRes;		// Resource for enable params and controller DAQ API
	int64_t	&mPrior,	// Process task priority
		&mPattrLim,	// Parameter's attributes limit
		&mRetr,		// Request retries
		&mTm;		// Request timeout
	string	wAddr, wComm;
	int64_t	mPer;

	bool	prcSt,		// Process task active
		callSt,		// Calc now stat
		endrunReq,	// Request to stop of the Process task
		prmEnErr;	// Error connection pass for enable parameters
	vector< AutoHD<TMdPrm> >  pHd;

	double	tmGath;		// Gathering time
	MtxString acqErr;

	struct snmp_session session;
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
