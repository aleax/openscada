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
typedef struct
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
}SFrm;

class TConfig;
class TConfigElem;
class TVirtual;

class TContrVirt: public TController
{
public:
    TContrVirt(TVirtual *tvirt, TTipController *tcntr, string name_c, string bd_c, TConfigElem *cfgelem);
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

private:
    bool      run_st;      // Stat of task
    bool      endrun;      // Command for stop task
    int       period;      // ms
    int       iterate;
    pthread_t pthr_tsk;

    TVirtual  *virt;
};

class TPrmVirt : public TParamContr
{
 public:
    TPrmVirt(TController *contr, TConfigElem *cfgelem);	    
    ~TPrmVirt( );

    void Load( string FCfg );
    float Calc();
 private:
    string        descript;
    int           form;
    vector<int>   x_id;
    vector<float> x;
    vector<float> k;
    float         y;
 private:
    int SetInput(int id ,float val){ if(id < 0) y = val; else x[id]=val; return(0); }
    
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
    void init( void *param );

    void CheckCommandLine(  );
    void UpdateOpt(  );

    TController *ContrAttach(string name, string bd);

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

