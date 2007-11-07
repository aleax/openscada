
//OpenSCADA system module DAQ.SNMP file: snmp_client.h
/***************************************************************************
 *   Copyright (C) 2006-2007 by Roman Savochenko                           *
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
	
	string &OIDList( )	{ return m_oid; }
	vector<string> &lsOID( ){ return ls_oid; }
	TElem &elem( )		{ return p_el; }

	void enable( );
	void disable( );
	void load( );
		
	TMdContr &owner( )	{ return (TMdContr&)TParamContr::owner(); }
	
    private:
	//Methods
        void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	void vlArchMake( TVal &val );
	
	void parseOIDList( const string &ioid );
	
        //Attributes
	string	&m_oid;	//Gaher OID list. Single text
	vector<string>	ls_oid;	//Parsed IOD list.
	
        TElem   p_el;           //Work atribute elements
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

	double period( )	{ return m_per; }
        int    prior( )		{ return m_prior; }
	int    pAttrLimit( )	{ return m_pattr_lim; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void load( );
	void save( );
	void start_( );
	void stop_( );
	
    protected:
	//Methods
	void prmEn( const string &id, bool val );
    	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );
	
	string oid2str( oid *ioid, size_t isz );
	
	//Attributes
	Res	en_res;         //Resource for enable params
	int	&m_per,     	// s
		&m_prior,	// Process task priority
		&m_pattr_lim;	// Parameter's attributes limit
	string	&m_addr,	// Host address
		&m_comm;	// Server community		
		
	bool    prc_st,		// Process task active
		endrun_req;	// Request to stop of the Process task
        vector< AutoHD<TMdPrm> >  p_hd;
	
	pthread_t procPthr;     // Process task thread
	
	double 	tm_gath;	// Gathering time
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
	
	void postEnable( int flag );
	void modLoad( );

    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );
	string optDescr( );
};

extern TTpContr *mod;

} //End namespace 

#endif //SNMP_DAQ_H
