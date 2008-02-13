
//OpenSCADA system module DAQ.ModBus file: modbus_client.h
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
 
#ifndef MODBUS_CLIENT_H
#define MODBUS_CLIENT_H

#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>
#include <map>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using std::map;

namespace ModBusDAQ
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

	void enable( );
	void disable( );

	TElem &elem( )		{ return p_el; }
	TMdContr &owner( )	{ return (TMdContr&)TParamContr::owner(); }
	
        //Attributes
	bool    isErr;		//Error present	flag

    private:
	//Methods
        void postEnable( int flag );
	void vlGet( TVal &val );
	void vlSet( TVal &val );
	void vlArchMake( TVal &val );
	
        //Attributes
	string  &m_attrLs;
        TElem   p_el;           //Work atribute elements
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

	double period( )	{ return m_per; }
	int    prior( )		{ return m_prior; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void load( );
	void save( );
	void start_( );
	void stop_( );
	
	void prmEn( const string &id, bool val );
	string modBusReq( string &pdu );

    protected:
	//Methods
    	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	bool cfgChange( TCfg &cfg );
	
    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );
	
	//Attributes
	Res	en_res, req_res;		//Resource for enable params and request values
	double	&m_per;     			//Acquisition task (seconds)
	int	&m_prior,			//Process task priority
		&m_prt,				//Protocol
		&m_speed,			//Serial speed
		&m_node;			//Node
	string	&m_dev,				//Serial device
		m_wdev,				//Work serial device
		&m_addr;			//Remote host address
		
	bool    prc_st,				//Process task active
		endrun_req;			//Request to stop of the Process task
        vector< AutoHD<TMdPrm> >  p_hd;
	
	pthread_t procPthr;     		//Process task thread
	
	double 	tm_gath;			//Gathering time
};

//*************************************************
//* SSerial                                       *
//*************************************************
class SSerial
{
    public:
	//Data
	enum	Prot { Free, ASCII, RTU };

	//Methods
	SSerial( )	{ }
	SSerial( const string &dev );
	
	void connect( int speed, Prot iPrt = RTU, ui32 iFrTm = 3000, ui32 iCharTm = 100, ui32 iTm = 3 );
	void disconnect( );
	
	string req( const string &vl );
		
    private:
	//Attributes
	string	dev;				//Serial port device
	Prot   	prot;   			//ASCII/RTU protocol
	ui32 	frTm;				//Frame timeout in ms
	ui32	charTm;				//Char timeout in ms
	ui32   	tm;				//Request timeout in s
		
	Res	m_res;				//Serial port resource
	int	fd;				//Serial port
	int	m_connect;			//Serial port connection's counter, from acquisition controllers
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

	void modLoad( );

	SSerial &sPortAt( const string &dev );
	
	ui16 CRC16( const string &mbap );
	ui8  LRC( const string &mbap );
	string DataToASCII( const string &in );
	string ASCIIToData( const string &in );

    private:
	//Methods
	void postEnable( int flag );
	TController *ContrAttach( const string &name, const string &daq_db );
	string optDescr( );
	
	//Attributes
	map<string,SSerial>	m_sdevs;
	
	static ui8 CRCHi[];
	static ui8 CRCLo[];
};

extern TTpContr *mod;

} //End namespace 

#endif //MODBUS_CLIENT_H
