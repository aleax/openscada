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

#ifndef VIRTUAL_H
#define VIRTUAL_H

#include <string>
#include <vector>

//#include "thd.h"
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

	void load(string f_alg = "");
	void save(string f_alg = "");
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
    bool  		internal;    // internal locate parameter direct access
    AutoHD<TParam> 	hd_g;        // Global header
    AutoHD<TParamContr>	hd_i;        // Internal header
    float x;           // value
    bool  sync;
};

class TConfig;
class TElem;
class TVirtual;

class TVContr: public TController
{
    public:
	TVContr( string name_c, const TBDS::SName &bd, ::TTipController *tcntr, ::TElem *cfgelem);
	~TVContr();   

	void load_(  );
	void save_(  );
	void start_(  );
	void stop_(  );    
    
	TParamContr *ParamAttach( const string &name, int type );
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
	vector< AutoHD<TParamContr> > p_hd;
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
    TVal *out;
    TVal *sp;
    TVal *stat;
};

class TVPrm : public TParamContr
{
    public:
     	TVPrm( string name, TTipParam *tp_prm, TController *contr);
	~TVPrm( );
    
	//void UpdateVAL();
    
	void load( );
    
	float Calc();
	void  Sync();
    
    private:
	int           form;
	int           y_id;    
	vector<int>   x_id;    
	vector<float> k;
	SPID          *pid;           //for pid
    private:
	void vlSet( int id_elem );
	void vlGet( int id_elem );
    
	void  Y(float val);
	float Y();
	float Y_MAX() { return( 100. ); } //????
	float Y_MIN() { return( 0. ); }   //????
    
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


//New formules and algobloks
class TFrm : public TCntrNode
{
    public:
    	TFrm( const string &name, TVirtual &owner, XMLNode *dt = NULL );
	~TFrm();

	string &name() { return(m_name); }
	string &lname(){ return(m_lname); }

	unsigned ioSize(){ return(name_io.size()); }
	unsigned kfSize(){ return(name_kf.size()); }

    protected:
	string nodeName(){ return m_name; }
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	
    private:
	string         m_name;
	string         m_lname;
	
	vector<string> name_io;
	vector<string> name_kf;
	
	string         form;
	string         form_b;

	TVirtual       &m_owner;
};

class TAlg : public TCntrNode
{
    public:
	TAlg( const string &name, TVirtual &owner, XMLNode *dt = NULL );
	~TAlg();

	string &name() { return(m_name); }
	string &lname(){ return(m_lname); }
	
    protected:
	string nodeName(){ return m_name; }
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );	
	
    private:
    	string           m_name;         //Name of a algoblok (parameter name)
       	string           m_lname;        //Description of a algoblok 
	
	TFrm 		 *formule;       //Connect formule
	
	vector< string > io;             //IO parameters
	vector< float >  kf;             //koefficients
	
	TVirtual         &m_owner;
};

class TVirtual: public TTipController
{
    public:
	TVirtual( string name );
	~TVirtual();
	
	void modConnect( );
	void modLoad( );
	
	TController *ContrAttach( const string &name, const TBDS::SName &bd);

	string NameCfg()   { return(NameCfgF); }
	TVirtAlgb *AlgbS() { return(algbs); }

	// Formuls list
	void frm_list( vector<string> &list )	{ chldList(m_frm,list); }
    	void frm_add( const string &name, XMLNode *dt = NULL );
	void frm_del( const string &name )	{ chldDel(m_frm,name); }
	AutoHD<TFrm> frm_at( const string &name )
	{ return chldAt(m_frm,name); }    
	
	// Algobloks list
	void alg_list( vector<string> &list )	{ chldList(m_alg,list); }
    	void alg_add( const string &name, XMLNode *dt = NULL );
	void alg_del( const string &name )	{ chldDel(m_alg,name); }
	AutoHD<TAlg> alg_at( const string &name )
	{ return chldAt(m_alg,name); }    
	
    protected:
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	AutoHD<TCntrNode> ctrAt1( const string &br );
    
    private:
        string optDescr( );
	void loadBD();
	void saveBD();
	
    private:
	int 	m_frm, m_alg;
	TVirtAlgb    *algbs;
	//Name of config file for virtual controllers
	string       NameCfgF;
	string       formCfg;
	string       algbCfg;

	vector<TElem *> val_el;
};


} //End namespace Virtual1

#endif //VIRTUAL_H

