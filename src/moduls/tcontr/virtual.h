#ifndef TEST_TCONTR_H
#define TEST_TCONTR_H

#include "../../tmodule.h"
#include "../../tvalue.h"
#include "../../tcontroller.h"
#include "../../ttipcontroller.h"
#include "../../tparamcontr.h"

#include <string>
using std::string;
#include <vector>
using std::vector;

typedef unsigned char  byte;
typedef unsigned short word;
typedef short          word_s;
typedef unsigned long  dword;

//------- Discription formuls and algobloks -----------------------
struct SFrm
{
    string name;
    char   tip;
    byte   n_inp;
    byte   n_koef;
    char   **name_inp;
    char   **name_kf;
    word   l_frm;
    char   *formul;
    word   l_frm1;
    char   *form_e;
};

class TConfig;
class TConfigElem;
class TVirtual;

class TContrVirt: public TController
{
public:
    TContrVirt(TVirtual *tvirt, TTipController *tcntr, string name_c,string _t_bd, string _n_bd, string _n_tb, TConfigElem *cfgelem);
    virtual ~TContrVirt();   

    virtual int Load(  );
    virtual int Save(  );
    virtual int Free(  );
    virtual int Start(  );
    virtual int Stop(  );    
//    virtual int Enable(  );
//    virtual int Disable(  );
//
    virtual TParamContr *ParamAttach(int type);
    int Period()  {return(period); }
    int Iterate() {return(iterate); }

    TVirtual *Virt(){ return(virt); }
public:
    
private:
    static void *Task(void *);
    static void wakeup(int n_sig) {}

    void Sync();
private:
    bool      run_st;      // Stat of task
    bool      endrun;      // Command for stop task
    int       period;      // ms
    int       d_sync;      // counter for syncing virtual controller    
    int       iterate;
    pthread_t pthr_tsk;

    TVirtual  *virt;
};

struct SIO
{
    bool  internal;    // internal locate parameter direct access
    //int  hd_cntr;     // maybe make direct access to virtualV1 type controller
    int   hd_prm;      // hd parameter local (TParamS) or external (TController)
    float min,max;    // scale
    bool  sync;
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

class TPrmVirt : public TParamContr
{
 public:
    TPrmVirt(TController *contr, TConfigElem *cfgelem);	    
    ~TPrmVirt( );
    
    void UpdateVAL();
    
    void Load( string FCfg );
    
    float Calc();
    void  Sync();
    
 private:
    string        descript;
    int           form;
    vector<SIO>   x_id;    
    vector<float> x;
    vector<float> k;
    int           hd_y;
    float         y_min,y_max;    
    SPID          *pid;           //for pid
 private:
    void SetVal( int id_elem );
    void GetVal( int id_elem );
    
    void  Y(float val);
    float Y();
    
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

class TVirtual: public TModule
{
public:
    TVirtual(char *name);
    ~TVirtual();
	
    void info( const string & name, string & info );
    void connect( void *obj );
    void init( void *param );
    
    void CheckCommandLine( char **argv, int argc );
    void UpdateOpt(  );

    TController *ContrAttach(string name, string t_bd, string n_bd, string n_tb);

    string NameCfg() { return(NameCfgF); }
    
    vector<SFrm> formuls; 
public:

private:
    void pr_opt_descr( FILE * stream );
	
    void LoadAlg( string NameCfgF );
private:
    static SCfgFld elem[];
    static SCfgFld ElemAN[];
    static SCfgFld ElemDG[];
    static SCfgFld ElemBL[];

    static SVAL  ValAN[];
    static SVAL  ValDG[];
    static SVAL  ValPID[];
	
    static SExpFunc ExpFuncLc[];
    TTipController *TContr;
    //Name of config file for virtual controllers
    string       NameCfgF;
};




#endif //TEST_TCONTR_H

