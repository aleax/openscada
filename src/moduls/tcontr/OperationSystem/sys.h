/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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
 
#ifndef SYS_H
#define SYS_H

#include <tmodule.h>
#include <tcontroller.h>
#include <ttipcontroller.h>
#include <tparamcontr.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace SystemCntr
{

class TConfig;
class TElem;

#define PRM_NONE   -1
#define PRM_CPU     0
#define PRM_MEM     1
#define PRM_UPTIME  2
#define PRM_HDDT    3
#define PRM_SENSOR  4


class TMdPrm;

//======================================================================
//==== UpTime
//======================================================================
struct S_CPU
{
    long user;	
    long nice;
    long sys;
    long idle;
};

class CPU: private ::TElem
{
    public:
	CPU( TMdPrm &prm );
	~CPU();

	void init();
	void getVal( );
    private:
	S_CPU         gen;
	vector<S_CPU> cpu;
	
	TMdPrm      &prm;
};

//======================================================================
//==== Memory
//======================================================================
class Mem: private ::TElem
{
    public:
	Mem( TMdPrm &prm );
	~Mem();

	void init();
	void getVal( );
	void chSub( );
    private:
	TMdPrm      &prm;
};

//======================================================================
//==== UpTime
//======================================================================
class UpTime: private ::TElem
{
    public:
	UpTime( TMdPrm &prm );
	~UpTime();

	void init();
	void getVal( );
    private:
        time_t      st_tm;
	
	TMdPrm      &prm;
};

//======================================================================
//==== HddTemp
//======================================================================
class Hddtemp: private ::TElem
{
    public:
	Hddtemp( TMdPrm &prm );
	~Hddtemp();

	void init();
	void getVal( );
	void chSub( );
    private:
	void dList( vector<string> &list );
    private:
	bool        err_st;  
	AutoHD<TTransportS>	tr;
	TMdPrm      &prm;
	string      t_tr;
	string      n_tr;

	TCfg        &c_subt;
	vector< AutoHD<TVal> > atrb;

	AutoHD<TTransportOut> *otr;
};

//======================================================================
//==== Lmsensors
//======================================================================
class Lmsensors: private ::TElem
{
    public:
	Lmsensors( TMdPrm &prm );
	~Lmsensors();
	
	void init();
	void getVal( );	
	void chSub( );
    private:	
	void dList( vector<string> &list );
    private:
	string      s_path;

	TMdPrm      &prm;	
};

//======================================================================
//==== TMdPrm 
//======================================================================
class TMdPrm : public TParamContr
{
    friend class CPU;
    friend class Mem;
    friend class UpTime;
    friend class Hddtemp;
    friend class Lmsensors;
    
    public:
    	TMdPrm( string name, TTipParam *tp_prm );
	~TMdPrm( );
	
	void enable();
	void disable();

	//set perameter type
	void setType( char tp );
	//get new value
	void getVal();
	
    protected:
        bool cfgChange( TCfg &cfg );	//config change
		       
	void vlGet( TVal &val );

	void postEnable();
	void preDisable( int flag );
	
    private:
	char m_type; //Type parameter: PRM_HDDT, PRM_SENSOR
	union
	{
	    CPU       *p_cpu;
	    Mem       *p_mem;
	    UpTime    *p_upt;
	    Hddtemp   *p_hdd;
	    Lmsensors *p_sens;
	}prm;	
};

//======================================================================
//==== TMdContr 
//======================================================================
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
    	TMdContr( string name_c, const TBDS::SName &bd, ::TElem *cfgelem);
	~TMdContr();   

	TParamContr *ParamAttach( const string &name, int type );

	void load_(  );
	void save_(  );
	void start_(  );
	void stop_(  );    
	
    protected:
	void prmEn( const string &id, bool val );
    	
    private:
	static void *Task(void *);
	
    private:
	int	en_res;         //Resource for enable params
	int	&period;     	// ms
	
	bool	endrun;      	// Command for stop task
	vector< AutoHD<TMdPrm> >  p_hd;    
	
	pthread_t	pthr_tsk;
};

//======================================================================
//==== TTpContr 
//======================================================================
class TTpContr: public TTipController
{
    public:
    	TTpContr( string name );
	~TTpContr();
	
	void postEnable();
	void modLoad( );

	TController *ContrAttach( const string &name, const TBDS::SName &bd);
    
    private:
	string optDescr( );    
};

extern TTpContr *mod;

} //End namespace 

#endif //SYS_H

