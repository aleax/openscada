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
	TModule     &mod;
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
	TModule     &mod;
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
	TTransportS &tr;
	TMdPrm      &prm;
	STrS        n_tr;
	SHDTr       hd;
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
     public:
	TMdPrm( string name, TTipParam *tp_prm, TController *contr);
	~TMdPrm( );

	//set perameter type
	void setType( char tp );
	//config change
	bool cfChange( TCfg &cfg );
	//get new value
	void getVal();
     protected:
	void vlGet( TVal &val );
     private:
	void free();
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


class TMdContr: public TController
{
    public:
    	TMdContr( string name_c, SBDS bd, ::TTipController *tcntr, ::TElem *cfgelem);
	~TMdContr();   

	TParamContr *ParamAttach( string name, int type );

	void Load_(  );
	void Save_(  );
	void Start_(  );
	void Stop_(  );    
    public:
	
    private:
	static void *Task(void *);
	static void wakeup(int n_sig) {}
    private:
	int          &period;     // ms
	
	bool         endrun;      // Command for stop task
	vector<int>  p_hd;

	pthread_t    pthr_tsk;
};

class TTpContr: public TTipController
{
public:
    TTpContr( string name );
    ~TTpContr();
	
    void mod_connect( );
    
    void mod_CheckCommandLine( );
    void mod_UpdateOpt(  );

    TController *ContrAttach(string name, SBDS bd);
public:

private:
    void pr_opt_descr( FILE * stream );
private:
    //Name of config file for virtual controllers
    string       NameCfgF;
};


} //End namespace 

#endif //SYS_H

