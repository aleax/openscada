
//OpenSCADA system module DAQ.LogicLev file: logiclev.h
/***************************************************************************
 *   Copyright (C) 2006 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
 
#ifndef MOD_CIF_H
#define MOD_CIF_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

#undef _
#define _(mess) mod->I18N(mess)

#define MaxLenReq 240

using std::string;
using std::vector;

namespace CIF
{

//**** Value data structure ****
class SValData 
{ 
    public:
	SValData( int idb, int ioff, int isz ) :
	    db(idb), off(ioff), sz(isz) { }
	
	int db;	//DB
	int off;//Data offset
	int sz;	//Data size or boolean bit
};

//======================================================================
//==== TMdPrm 
//======================================================================
//Parammeters errors list
//1   - Parameter had disabled.
//2   - Controller is stoped.
//10  - CIF device error.
//11  - Value not gathered.
//12  - Put request is timeouted.
//13  - Get request is timeouted.
//14  - Request block too big.
//15  - Board %d no present.
//16  - Transmit block too big.
//17  - No response of the remote station.
//18  - Master not into the logical token ring.
//19  - Request to DB error %d.
//20  - Specified offset address or DB error.

class TMdContr;

class TMdPrm : public TParamContr, public TValFunc
{
    public:
	//Data
	class SLnk
	{
	    public:
	        SLnk(int iid, const string &db_addr = "") : 
		    io_id(iid), db_addr(db_addr), val(-1,-1,0) { }

                int     io_id;		//Template function io index
		string  db_addr;	//DB full address: DB1.20.1
		SValData val;		//Value address data
        };
    
	//Methods
    	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );	
	
	void enable();
	void disable();
	void load( );
	void save( );
	
	void calc( bool first, bool last );	//Calc template's algoritmes
	
	//- Template link operations -
        int lnkSize();
        int lnkId( int id );
        int lnkId( const string &id );
        SLnk &lnk( int num );
	
	TMdContr &owner()	{ return (TMdContr&)TParamContr::owner(); }
	
    private:    
	//Methods
        void postEnable(int flag);
        void postDisable(int flag);
	
	void cntrCmdProc( XMLNode *opt );       //Control interface command process			
						
        void vlGet( TVal &val );
        void vlSet( TVal &val );			
	
	void loadIO();
        void saveIO();
	void initLnks();
							
        //Attributes
        string  &m_tmpl;

        TElem   p_el;           		//Work atribute elements
	
	int	id_freq, id_start, id_stop, id_err;	//Fixed system attributes identifiers
	vector<SLnk>	plnk;			//Parameter's links
	
	string	acq_err;
	time_t	acq_err_tm;
};

//======================================================================
//==== TMdContr 
//======================================================================
class TTpContr;

class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
    	TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem);
	~TMdContr();   

	int period()	{ return m_per; }

	AutoHD<TMdPrm> at( const string &nm )	{ return TController::at(nm); }

	void load( );
	void save( );
	void enable_( );
	void disable_( );	
	void start_( );
	void stop_( );
	
	TTpContr &owner()	{ return *(TTpContr *)nodePrev(); }
	
    protected:
	//Methods	
	void prmEn( const string &id, bool val );	//Enable parameter to process list	
	void regVal( SValData ival, IO::Type itp );	//Register value for acquisition
	//- Values process -
	char getValB( SValData ival, string &err );
	int  getValI( SValData ival, string &err );
	double getValR( SValData ival, string &err );
	string getValS( SValData ival, string &err );
	
	void setValB( bool ivl, SValData ival, string &err );
        void setValI( int ivl, SValData ival, string &err );
        void setValR( double ivl, SValData ival, string &err );
        void setValS( const string &ivl, SValData ival, string &err );
	
	//- Service -
	void postDisable(int flag);     	//Delete all DB if flag 1
    	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
    private:
	//Methods
	TParamContr *ParamAttach( const string &name, int type );
	static void *Task( void *icntr );
	int valSize( IO::Type itp, int iv_sz );	//Prepare value sizes
	string revers(const string &ibuf)
	{
	    string obuf;
	    for(int i=ibuf.size()-1; i >= 0; i--)
		obuf+=ibuf[i];
	    return obuf;
	}
	//Data
	class SGetRec
	{
	    public:
	    	SGetRec( int idb, int ioff, int v_rez ) : db(idb), off(ioff)	
	    	{ val.assign(v_rez,0); err="11:Value not gathered."; }
	    
	    	int db;		//Data block
	    	int off;	//Data block start offset
	    	string val;	//Data block values kadr
	    	string err;	//Acquisition error text
	};
	
	//Attributes
	int	en_res;         //Resource for enable params
	int	&m_per,     	// ms
		&m_prior,	// Process task priority
		&m_dev,		// CIF device number
		&m_addr;	// Remote host address
		
	bool    prc_st,		// Process task active
		endrun_req;	// Request to stop of the Process task
        vector< AutoHD<TMdPrm> >  p_hd;	// Parameter's process list
	vector< SGetRec > acqBlks;	// Acquisition data blocks
	
	pthread_t procPthr;     // Process task thread
	
	double 	tm_calc;	// Template functions calc time
};

//======================================================================
//==== TTpContr 
//======================================================================
class TTpContr: public TTipDAQ
{
    friend class TMdContr;
    public:
	//Methods
    	TTpContr( string name );
	~TTpContr();
	
	void modLoad( );
	void modSave( );

	//- Devices -
	bool drvCIFOK( );	
	void initCIF( int dev );

	TElem   &prmIOE()	{ return el_prm_io; }
	TElem   &CIFDevE()	{ return el_cif_dev; }

	//- Remote stations requests -
	void getDBCIF(unsigned board, unsigned n_st, 
		      unsigned n_db, long offset, string &buffer);
	void putDBCIF(unsigned board, unsigned n_st, 
	              unsigned n_db, long offset, const string &buffer);

	//- Life list of bus stations -
	void getLifeListCIF(unsigned board, string &buffer);
	
    protected:
	//Methods
	void postEnable( int flag );
	void cntrCmdProc( XMLNode *opt );
	
    private:
	//Methods
	TController *ContrAttach( const string &name, const string &daq_db );
	string optDescr( );    
    
	//Attributes
	bool	drv_CIF_OK;
	TElem   el_cif_dev, el_prm_io;
	
	struct SCifDev
	{
	    int	   res;			//Device resource
	    bool   present;		//Present flag
	    int    board;		//Board number
	    unsigned long phAddr;	//Physical address
	    short  irq;			//IRQ
	    string fwname;		//Firmware device name
	    string fwver;		//Firmware device version
	    int    pbaddr;		//PB device address
	    int    pbspeed;		//PB network speed
	};
	
	SCifDev cif_devs[MAX_DEV_BOARDS];
};

extern TTpContr *mod;

} //End namespace 

#endif //CIF_H

