#ifndef VIRTUAL_H
#define VIRTUAL_H

#include <string>
#include <vector>

#include <tmodule.h>
#include <tvalue.h>
#include <tcontroller.h>
#include <ttipcontroller.h>
#include <tparamcontr.h>

typedef unsigned char  byte;
typedef unsigned short word;
typedef short          word_s;
typedef unsigned long  dword;

using std::string;
using std::vector;

namespace Virtual1
{

//------- Discription formuls and algobloks -----------------------
struct SFrm
{
    string         name;
    char           tip;
    byte           n_inp;
    byte           n_koef;
    vector<string> name_inp;
    vector<string> name_kf;
    string         formul;
    unsigned       l_frm_e;
    char           *form_e;
};

struct SAlgb
{
    string           name;           //Name of a algoblok (parameter name)
    string           descr;          //Description of a algoblok 
    word             tp_alg;         //Type of a algoblok 
    vector< string > io;             //IO parameters
    vector< float >  kf;             //koefficients
};

class TVirtAlgb
{
    public:
    	TVirtAlgb(string cfg_file);
	~TVirtAlgb();

	void Load(string f_alg = "");
	void Save(string f_alg = "");
	void Free();
        //Formuls
	SFrm *GetFrm(unsigned id);
	unsigned SizeFrm() { return(frm_s.size()); }
	//Algobloks
	SAlgb *GetAlg(string name);	
    private:
	string          file;
	vector< SFrm* >  frm_s;
	vector< SAlgb* > algb_s;	
};

struct SIO
{
    bool  internal;    // internal locate parameter direct access
    int   hd_prm;      // hd parameter local (TParamS) or external (TController)
    float min,max;     // scale
    float x;           // value
    bool  sync;
};

class TConfig;
class TConfigElem;
class TVirtual;

class TVContr: public TController
{
    public:
	TVContr( string name_c, SBDS bd, ::TTipController *tcntr, ::TConfigElem *cfgelem);
	~TVContr();   

	void Load_(  );
	void Save_(  );
	void Start_(  );
	void Stop_(  );    
    
	TParamContr *ParamAttach( string name, int type );
	int Period()  {return(period); }
	int Iterate() {return(iterate); }
    
        int  prm_connect( string name );    
	SIO &prm( unsigned hd );
    public:
    
    private:
	static void *Task(void *);
	static void wakeup(int n_sig) {}

	void Sync();
    private:
	vector<int>  p_hd;
	vector<SIO*> p_io_hd;

	bool      endrun;      // Command for stop task
	int       &period;     // ms
	int       &per_sync;   // counter for syncing virtual controller    
	int       &iterate;    
	int       d_sync;      // counter for syncing virtual controller    
	pthread_t pthr_tsk;
};


#define R_MAN  0
#define R_AUTO 1
#define R_CAS  2

struct SPID
{
    int   hd_out;
    int   hd_sp;
    int   hd_stat;
};

class TVPrm : public TParamContr
{
 public:
    TVPrm( string name, TTipParam *tp_prm, TController *contr);
    ~TVPrm( );
    
    void UpdateVAL();
    
    void Load( );
    
    float Calc();
    void  Sync();
    
 private:
    int           form;
    int           y_id;    
    vector<int>   x_id;    
    vector<float> k;
    //int           hd_y;
    //float         y_min,y_max;    
    SPID          *pid;           //for pid
 private:
    void vl_Set( int id_elem );
    void vl_Get( int id_elem );
    
    void  Y(float val);
    float Y();
    float Y_MAX() { return( ((TVContr &)Owner()).prm(y_id).max ); } 
    float Y_MIN() { return( ((TVContr &)Owner()).prm(y_id).min ); }
    
    void  X(unsigned id ,float val);
    float X(unsigned id);
    
    float blok_dig( );
    float ymn();
    float sym();
    float free_formul();
    float calk_form(char *form, int len, int *off, float rez,byte h_prior);
    void  calk_oper(byte symb,float *rez,float parm);
    float pid_n( );
};

class TVirtual: public TTipController
{
public:
    TVirtual( string name );
    ~TVirtual();
	
    void mod_connect( );
    
    void mod_CheckCommandLine( );
    void mod_UpdateOpt(  );

    TController *ContrAttach(string name, SBDS bd);

    string NameCfg()   { return(NameCfgF); }
    TVirtAlgb *AlgbS() { return(algbs); }
public:

private:
    void pr_opt_descr( FILE * stream );
private:
    static SVAL  ValAN[];
    static SVAL  ValDG[];
    static SVAL  ValPID[];
	
    TVirtAlgb    *algbs;
    //Name of config file for virtual controllers
    string       NameCfgF;
};


} //End namespace Virtual1

#endif //VIRTUAL_H

