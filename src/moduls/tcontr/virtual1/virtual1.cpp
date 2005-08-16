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

#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>

#include <terror.h>
#include <tsys.h>
#include <tmessage.h>
#include <tconfig.h>
#include <tvalue.h>
#include <tparam.h>
#include <tparams.h>
#include <ttiparam.h>
#include "virtual1.h"

//============ Modul info! =====================================================
#define MOD_ID      "virtual_v1"
#define MOD_NAME    "Virtual controller"
#define MOD_TYPE    "Controller"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.0.15"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Virtual controller V1.x (from Complex2) - may be used how internal controller or instrument for GUI"
#define LICENSE     "GPL"
//==============================================================================

//Present controller parameter
#define PRM_ANALOG "ANALOG"
#define PRM_B_AN   "PRM_BD1"

#define PRM_DIGIT  "DIGIT"
#define PRM_B_DG   "PRM_BD2"

#define PRM_BLOCK  "BLOCK"
#define PRM_B_BLCK "PRM_BD3"

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;

	if(n_mod==0)
    	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
	else
	    AtMod.id	= "";

	return( AtMod );
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	Virtual1::TVirtual *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new Virtual1::TVirtual( source );

	return ( self_addr );
    }
}

using namespace Virtual1;

//======================================================================
//==== TVirtual ======================================================== 
//======================================================================

TVirtual::TVirtual( string name ) : 
    algbCfg("./virt_alg.xml"), formCfg("./virt_frm.xml"), NameCfgF("./alg.cfg"), algbs(NULL)
{
    m_frm = grpAdd();
    m_alg = grpAdd();	
    
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= name;    
}

TVirtual::~TVirtual()
{    
    if(algbs) delete algbs;
    for( int i_el = 0; i_el < val_el.size(); i_el++)
	delete val_el[i_el];
}

string TVirtual::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
        "======================= The module <%s:%s> options =======================\n"
	"    --Vrt1CFG=<path>   config file name (default ./alg.cfg)\n"
	"    --Vrt1Alg=<path>   Algoblock's config file (default ./virt_alg.xml)\n"
	"    --Vrt1Form=<path>  Formul's config file (default ./virt_frm.xml)\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"config <path>          config file name (default ./alg.cfg)\n"
	"alg_cfg <path>         Algoblock's config file (default ./virt_alg.xml)\n"
	"form_cfg <path>        Formul's config file (default ./virt_frm.xml)\n\n"),
	MOD_TYPE,MOD_ID,MOD_ID);
    return(buf);
}

void TVirtual::modLoad( )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"      ,0,NULL,'h'},
	{"Vrt1CFG"   ,1,NULL,'c'},
	{"Vrt1Alg"   ,1,NULL,'a'},
	{"Vrt1Form"  ,1,NULL,'f'},
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case 'c': NameCfgF = optarg;   break;
	    case 'a': algbCfg = optarg;    break;
	    case 'f': formCfg = optarg;    break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    
    try{ NameCfgF = ctrId(&SYS->cfgRoot(),nodePath())->childGet("id","config")->text(); } catch(...) {  }
    try{ algbCfg = ctrId(&SYS->cfgRoot(),nodePath())->childGet("id","alg_cfg")->text(); } catch(...) {  }
    try{ formCfg = ctrId(&SYS->cfgRoot(),nodePath())->childGet("id","form_cfg")->text(); } catch(...) {  }    
    
    //Load algobloks
    algbs = new TVirtAlgb(NameCfgF);  //Old
    loadBD();  //NEW
}

void TVirtual::modConnect( )
{    
    TModule::modConnect( );
    
    //Controllers BD structure
    fldAdd( new TFld(PRM_B_AN,I18N("ANALOG parameteres table"),TFld::String,0,"30","VRT_AN") );
    fldAdd( new TFld(PRM_B_DG,I18N("DIGIT parameteres table"),TFld::String,0,"30","VRT_DG") );
    fldAdd( new TFld(PRM_B_BLCK,I18N("BLOCK parameteres table"),TFld::String,0,"30","VRT_BL") );
    fldAdd( new TFld("PERIOD",I18N("The calc period (ms)"),TFld::Dec,0,"5","1000","0;10000") );
    fldAdd( new TFld("ITER",I18N("The iteration number into calc period"),TFld::Dec,0,"2","1","0;99") );
    fldAdd( new TFld("PER_S",I18N("The sync period (ms)"),TFld::Dec,0,"5","1000","0;10000") );
    
    //loadCfg(elem,sizeof(elem)/sizeof(SFld));
    
    //Add parameter types
    //Analog parameters
    int t_prm = tpParmAdd(PRM_ANALOG,PRM_B_AN  ,I18N("Analog parameter"));
    tpPrmAt(t_prm).fldAdd( new TFld("ED",I18N("Value of measurement"),TFld::String,0,"10") );
    tpPrmAt(t_prm).fldAdd( new TFld("SCALE",I18N("Scale"),TFld::Dec,FLD_SELECT,"1","0","0;1",I18N("Linear;Square")) );
    tpPrmAt(t_prm).fldAdd( new TFld("TIPO",I18N("Type of processing"),TFld::Dec,FLD_SELECT,"1","0","0;1;2",I18N("Average;Integrate;Counter")) );
    tpPrmAt(t_prm).fldAdd( new TFld("MIN",I18N("Lower scale border"),TFld::Real,0,"10.2","0") );
    tpPrmAt(t_prm).fldAdd( new TFld("MAX",I18N("Upper scale border"),TFld::Real,0,"10.2","100") );
    tpPrmAt(t_prm).fldAdd( new TFld("NTG",I18N("Lower technically scale border"),TFld::Real,0,"10.2","0") );
    tpPrmAt(t_prm).fldAdd( new TFld("VTG",I18N("Upper technically scale border"),TFld::Real,0,"10.2","0") );
    tpPrmAt(t_prm).fldAdd( new TFld("NAG",I18N("Lower alarm scale border"),TFld::Real,0,"10.2","0") );
    tpPrmAt(t_prm).fldAdd( new TFld("VAG",I18N("Upper alarm scale border"),TFld::Real,0,"10.2","0") );
    tpPrmAt(t_prm).fldAdd( new TFld("Z_GR",I18N("Non-sensitive zone (%)"),TFld::Real,0,"4.1","0.5","0;50") );
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",I18N("The parameter value type"),TFld::String,FLD_SELECT,"5","A_IN","A_IN;A_OUT;PID",I18N("Input;Output;PID")) );
    //Digital parameters
    t_prm = tpParmAdd(PRM_DIGIT,PRM_B_DG,I18N("Digital parameter"));
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",I18N("Parameter type"),TFld::String,FLD_SELECT,"5","D_IN","D_IN;D_OUT",I18N("Input;Output")) );
    //Digital parameters
    t_prm = tpParmAdd(PRM_BLOCK,PRM_B_BLCK,I18N("Block parameter (algoblock)"));
    
    //Add types of value
    //Analog input
    TElem *elem = new TElem("A_IN");
    elem->fldAdd( new TFld("VAL","Value analog parameter",TFld::Real,0,"10.2") );
    val_el.push_back(elem);
    //Digital input
    elem = new TElem("D_IN");
    elem->fldAdd( new TFld("VAL","Value digital parameter",TFld::Bool,0,"10.2") );
    val_el.push_back(elem);
    //PID regulator
    elem = new TElem("PID");
    elem->fldAdd( new TFld("VAL","Value analog parameter",TFld::Real,0,"10.2") );
    elem->fldAdd( new TFld("OUT","Output of regulator",TFld::Real,0,"10.2","","0;100") );
    elem->fldAdd( new TFld("SP","Setpoint of regulator",TFld::Real,0,"10.2") );
    elem->fldAdd( new TFld("STAT","Stat regulator (Manual,Auto,Casc)",TFld::Dec,FLD_SELECT,"1","","0;1;2","Manual;Auto;Cascad") );
    elem->fldAdd( new TFld("Kp","Koefficient of proportion",TFld::Real,0,"6.2","","-20;20") );
    elem->fldAdd( new TFld("Ti","Time of integrated (sek)",TFld::Real,0,"7.2","","0;1000") );
    elem->fldAdd( new TFld("Td","Time of diff (sek)",TFld::Real,0,"7.2","","0;1000") );
    elem->fldAdd( new TFld("Tf","Time of lag (sek)",TFld::Real,0,"7.2","","0;1000") );
    elem->fldAdd( new TFld("H_UP","Up out border (%)",TFld::Real,0,"6.2","","0;100") );
    elem->fldAdd( new TFld("H_DOWN","Down out border (%)",TFld::Real,0,"6.2","","0;100") );
    elem->fldAdd( new TFld("ZN","Non-sensitive pid error (%)",TFld::Real,0,"5.2","","0;20") );
    elem->fldAdd( new TFld("K1","Koefficient scale of addon input 1",TFld::Real,0,"6.2","","-20;20") );
    elem->fldAdd( new TFld("K2","Koefficient scale of addon input 2",TFld::Real,0,"6.2","","-20;20") );
    elem->fldAdd( new TFld("K3","Koefficient scale of addon input 3",TFld::Real,0,"6.2","","-20;20") );
    elem->fldAdd( new TFld("K4","Koefficient scale of addon input 4",TFld::Real,0,"6.2","","-20;20") );
    val_el.push_back(elem);    
}

TController *TVirtual::ContrAttach( const string &name, const TBDS::SName &bd)
{
    return( new TVContr(name,bd,this,this));
}

void TVirtual::loadBD()
{
    int        buf_len = 100000;
    dword      ofs_alg;
    word       form_am, k_alg, len_2;
    byte       len_1;
    char       *buf;
    
    int fh = open(NameCfgF.c_str(),O_RDONLY);
    if(fh == -1) throw TError(nodePath().c_str(),"Open file %s for read, error!",NameCfgF.c_str());    

    buf = (char *)malloc(buf_len);
    
    read(fh,&ofs_alg,4); 
    read(fh,&form_am,2);
    for(unsigned i_frm = 0; i_frm < form_am; i_frm++)
    {
	string     s_buf;
        byte       tp,n_inp,n_koef;
	
	XMLNode frm("frm");
       	frm.attr("id","std_"+SYS->int2str(i_frm));
       	read(fh,&len_1,1);
	if(len_1)
	{
	    read(fh,buf,len_1); buf[len_1] = '\0';
	    frm.text(Mess->SconvIn("CP866",buf));
	}       
	read(fh,&tp,1);
	read(fh,&n_inp,1); 
	read(fh,&n_koef,1);
	
	if(n_inp)
   	    for(unsigned i_inp=0;i_inp < n_inp;i_inp++)
   	    {
		XMLNode *io = frm.childAdd("io");
       		read(fh,&len_1,1);
		read(fh,buf,len_1);
		s_buf.assign(buf,len_1);
	       	io->attr("id","io_"+SYS->int2str(i_inp));
	       	io->text(s_buf);
	    }
	if(n_koef)
	    for(unsigned i_kf=0;i_kf < n_koef;i_kf++)
	    {
		XMLNode *io = frm.childAdd("kf");
		read(fh,&len_1,1);
		read(fh,buf,len_1);
		s_buf.assign(buf,len_1);
	       	io->attr("id","io_"+SYS->int2str(i_kf));
	       	io->text(s_buf);		
	    }
	read(fh,&len_2,2);
    	read(fh,buf,len_2); buf[len_2] = '\0';
	if(tp != 5 ) s_buf = buf;
	else         s_buf = "In progress!!!";
	frm.childAdd("formula")->text(Mess->SconvIn("CP866",s_buf));
	
        frm_add(frm.attr("id"), &frm );
    }
    /*
    lseek(fh,ofs_alg,SEEK_SET);
    read(fh,&k_alg,2);
    for(unsigned i_alg=0; i_alg < k_alg; i_alg++)
    {
        byte    n_inp,n_koef;
	string  s_buf;
	XMLNode alg("alg");
	
    	read(fh,buf,9); buf[9] = 0;
	for(int i=8; i >= 0; i--) 
	    if(buf[i]==' ' || buf[i]== 0) buf[i]=0; 
	    else break; 
	s_buf = buf;
	Mess->SconvIn("CP866",s_buf);
       	alg.attr("id",s_buf);

	read(fh,&len_1,1); 
	read(fh,buf,len_1);
	s_buf.assign(buf,len_1);	
	Mess->SconvIn("CP866",s_buf);
       	alg.text(s_buf);
	
	read(fh,&len_2,2);
	string form_n = string("std_")+SYS->int2str(len_2);
	alg->childAdd("formula")->attr("id",form_n);
	for( int i_frm = 0; i_frm < m_frm.size(); i_frm++ )
	    if( m_frm[i_frm].name() == form_n )
		//????????????
	
	unsigned i_n = frm_s[tp_alg]->n_inp;
	if(i_n)
	    for(unsigned i_x=0;i_x < i_n;i_x++)
	    { 
		if( i_x == algb->io.size() ) algb->io.push_back("");
		
		read(fh,buf,9); buf[9] = 0;
		for(int i=8; i >= 0; i--) 
		    if(buf[i]==' ' || buf[i]== 0) buf[i]=0; 
		    else break;
		algb->io[i_x] = buf;
		Mess->SconvIn("CP866",algb->io[i_x]);
	    }
	i_n = frm_s[tp_alg]->n_koef;
	if(i_n)
	    for(unsigned i_k = 0;i_k < i_n; i_k++)
	    {
		if( i_k == algb->kf.size() ) algb->kf.push_back(0.0);
		read(fh,&(algb->kf[i_k]),4);
	    }
	algb_s.push_back(algb);
    }
    */
    free(buf);
    close(fh); 
}

void TVirtual::saveBD()
{

}
    	
void TVirtual::frm_add( const string &name, XMLNode *dt )
{
    if( chldAvoid(m_frm,name) ) return;
    chldAdd(m_frm,new TFrm(name,*this,dt));
}
    	
void TVirtual::alg_add( const string &name, XMLNode *dt )
{
    if( chldAvoid(m_alg,name) ) return;
    chldAdd(m_alg,new TAlg(name,*this,dt));
}

//================== Controll functions ========================
void TVirtual::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> list;
    
    if( cmd==TCntrNode::Info )
    {
	TTipController::cntrCmd_( a_path, opt, cmd );

	ctrInsNode("area",1,opt,a_path.c_str(),"/virt",I18N(MOD_NAME));
	ctrMkNode("area",opt,a_path.c_str(),"/virt/opt",I18N("Config"),0440);
	ctrMkNode("fld",opt,a_path.c_str(),"/virt/opt/a_cfg",I18N("Algoblok's config file"),0660,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/virt/opt/f_cfg",I18N("Formul's config file"),0660,0,0,"str");
	ctrMkNode("area",opt,a_path.c_str(),"/virt/alg",I18N("Algobloks"),0440);
	ctrMkNode("list",opt,a_path.c_str(),"/virt/alg/alg",I18N("Algobloks"),0660,0,0,"br")->
	    attr_("s_com","add,del")->attr_("mode","att");
	ctrMkNode("area",opt,a_path.c_str(),"/virt/frm",I18N("Formuls"),0440);
	ctrMkNode("list",opt,a_path.c_str(),"/virt/frm/frm",I18N("Formuls"),0660,0,0,"br")->
	    attr_("s_com","add,del")->attr_("mode","att");
	ctrMkNode("fld",opt,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,"str")->
            attr_("cols","90")->attr_("rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {    
	if( a_path == "/virt/opt/a_cfg" )	ctrSetS( opt, algbCfg );
	else if( a_path == "/virt/opt/f_cfg" )	ctrSetS( opt, formCfg );
	else if( a_path == "/virt/frm/frm" )
	{
	    frm_list(list);
	    opt->childClean();
	    for( unsigned i_f=0; i_f < list.size(); i_f++ )
		ctrSetS( opt, list[i_f] );
	}
	else if( a_path == "/virt/alg/alg" )
	{
	    alg_list(list);
	    opt->childClean();
	    for( unsigned i_f=0; i_f < list.size(); i_f++ )
		ctrSetS( opt, list[i_f] );
	}
	else if( a_path == "/help/g_help" ) 	ctrSetS( opt, optDescr() );
	else TTipController::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/virt/opt/a_cfg" )		algbCfg = ctrGetS( opt );
	else if( a_path == "/virt/opt/f_cfg" )		formCfg = ctrGetS( opt );
	else if( a_path == "/virt/frm/frm" )
	{
	    if( opt->name() == "add" )		frm_add(opt->text());
	    else if( opt->name() == "del" )	frm_del(opt->text());
	}
	else if( a_path == "/virt/alg/alg" )
	{
	    if( opt->name() == "add" )		alg_add(opt->text());
	    else if( opt->name() == "del" )	alg_del(opt->text());
	}
	else TTipController::cntrCmd_( a_path, opt, cmd );
    }
}

AutoHD<TCntrNode> TVirtual::ctrAt1( const string &br )
{
    if( br.substr(0,13) == "/virt/frm/frm" )   		return frm_at(TSYS::pathLev(br,3));
    else if( br.substr(0,13) == "/virt/alg/alg" )	return alg_at(TSYS::pathLev(br,3));
    else return TTipController::ctrAt1(br);
}

//======================================================================
//==== TAlg 
//======================================================================
TAlg::TAlg( const string &name, TVirtual &owner, XMLNode *dt ) : 
    m_name(name), m_owner(owner)
{ 
    if( dt != NULL && dt->name() == "frm" )
    {
	m_lname = dt->text( );
    }
}

TAlg::~TAlg()
{ 

}

void TAlg::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",m_owner.I18Ns("Algoblok: ")+name());
    }
    else if( cmd==TCntrNode::Get )
	throw TError((m_owner.nodePath()+"alg_"+name()).c_str(),"Branch <%s> error!",a_path.c_str());
    else if( cmd==TCntrNode::Set )
    	throw TError((m_owner.nodePath()+"alg_"+name()).c_str(),"Branch <%s> error!",a_path.c_str());
}

//======================================================================
//==== TFrm 
//======================================================================
TFrm::TFrm( const string &name, TVirtual &owner, XMLNode *dt ) : 
    m_name(name), m_owner(owner)
{ 
    if( dt != NULL && dt->name() == "frm" )
    {
	m_lname = dt->text( );	
	form    = dt->childGet("formula")->text( );
    }
}

TFrm::~TFrm()
{ 

}

void TFrm::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",m_owner.I18Ns("Formula: ")+name());
	ctrMkNode("area",opt,a_path.c_str(),"/gen",m_owner.I18N("Formula control"));
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/nm",Mess->I18N("Name"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/lnm",Mess->I18N("Description"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/gen/frm",Mess->I18N("Formula"),0664,0,0,"str")->
	    attr_("cols","100")->attr_("rows","20");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/gen/nm" )	ctrSetS( opt, m_name );
	else if( a_path == "/gen/lnm" )	ctrSetS( opt, m_lname );
	else if( a_path == "/gen/frm" )	ctrSetS( opt, form );
	else throw TError((m_owner.nodePath()+"frm_"+name()).c_str(),"Branch <%s> error",a_path.c_str());
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/gen/lnm" )	m_lname = ctrGetS( opt );
	else if( a_path == "/gen/frm" )	form    = ctrGetS( opt );
	else throw TError((m_owner.nodePath()+"frm_"+name()).c_str(),"Branch <%s> error",a_path.c_str());
    }
}


//======================================================================
//==== TVContr 
//======================================================================

TVContr::TVContr(  string name_c, const TBDS::SName &bd, ::TTipController *tcntr, ::TElem *cfgelem) :
    ::TController(name_c, bd, tcntr, cfgelem), endrun(false),
    period(cfg("PERIOD").getId()), per_sync(cfg("PER_S").getId()), iterate(cfg("ITER").getId())    
{

}

TVContr::~TVContr()
{
    if( run_st ) stop();
}

void TVContr::load_( )
{

}

void TVContr::save_( )
{

}

void TVContr::start_( )
{   
    pthread_attr_t      pthr_attr;
    struct sched_param  prior;
    //---- Attach parameter algoblock ----
    vector<string> list_p;

    if( !run_st ) 
    {    
    	list(list_p);
	for(unsigned i_prm=0; i_prm < list_p.size(); i_prm++)
	{
	    AutoHD<TVPrm> prm_c = at(list_p[i_prm],name()+"_start");
	    prm_c.at().load();
	    prm_c.at().enable();
	    p_hd.push_back(prm_c);	    
	}
	//------------------------------------    
	pthread_attr_init(&pthr_attr);
	if(SYS->user() == "root")
	{
	    prior.__sched_priority=10;
	    pthread_attr_setschedpolicy(&pthr_attr,SCHED_FIFO);
	    pthread_attr_setschedparam(&pthr_attr,&prior);
	    
	    Mess->put(nodePath().c_str(),TMess::Info,"Start into realtime mode!");
	}
	else pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
	pthread_create(&pthr_tsk,&pthr_attr,Task,this);
	pthread_attr_destroy(&pthr_attr);
	if( TSYS::eventWait( run_st, true, nodePath()+"start",5) )
	    throw TError(nodePath().c_str(),"Controller no started!");    	    
    }	
}

void TVContr::stop_( )
{  
    if( run_st )
    {
	endrun = true;
	pthread_kill(pthr_tsk, SIGALRM);
    	if( TSYS::eventWait( run_st, false, nodePath()+"stop",5) )
    	    throw TError(nodePath().c_str(),"Controller no stoped!");
	pthread_join(pthr_tsk, NULL);
	
	for(unsigned i_prm=0; i_prm < p_hd.size(); i_prm++)
	    ((TVPrm &)p_hd[i_prm].at()).disable();
	p_hd.clear();
	for(unsigned i_prm=0; i_prm < p_io_hd.size(); i_prm++)
	    delete p_io_hd[i_prm];
	p_io_hd.clear();    
    }
} 

void *TVContr::Task(void *contr)
{
    int    i_sync=0;
    
    struct itimerval mytim;             //Interval timer
    long   time_t1,time_t2,cnt_lost=0;
    int    frq = sysconf(_SC_CLK_TCK);  //Count of system timer n/sek
    TVContr *cntr = (TVContr *)contr;

#if OSC_DEBUG
    Mess->put(cntr->nodePath().c_str(),TMess::Debug,"Thread <%d> started!",getpid() );
#endif	

    try
    {
	if(cntr->period == 0) return(NULL);
	cntr->d_sync = cntr->per_sync/cntr->period;
	if(cntr->d_sync == 0)
	{ 
	    cntr->d_sync = 1;
	    cntr->per_sync = cntr->d_sync*cntr->period;
	}
	if(cntr->iterate <= 0) cntr->iterate = 1; 

	mytim.it_interval.tv_sec = 0; mytim.it_interval.tv_usec = cntr->period*1000;
	mytim.it_value.tv_sec    = 0; mytim.it_value.tv_usec    = cntr->period*1000;
    
	signal(SIGALRM,wakeup);
	setitimer(ITIMER_REAL,&mytim,NULL);
    
	cntr->run_st = true;  cntr->endrun = false;
	time_t1=times(NULL);    
    
	while( !cntr->endrun )
	{
	    pause();
	    /*
#ifdef OSC_DEBUG
	    //check hard cycle
	    time_t2 = times(0);
	    if( time_t2 != (time_t1+cntr->period*frq/1000) )
	    {
		cnt_lost+=time_t2-(time_t1+cntr->period*frq/1000);
		cntr->owner().mPut("DEBUG",TMess::Debug,"%s:Lost ticks %d - %d (%d)",cntr->name().c_str(),time_t2,time_t1+cntr->period*frq/1000,cnt_lost);
	    }
	    time_t1 = time_t2;	
	    //----------------
#endif
            */
	    if((++i_sync) >= cntr->d_sync) { i_sync=0; cntr->Sync(); }
	    for(int i_c=0; i_c < cntr->iterate; i_c++)
		for(unsigned i_p=0; i_p < cntr->p_hd.size(); i_p++)
		    ((TVPrm &)cntr->p_hd[i_p].at()).Calc();
	}
    } catch(TError err) 
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str() ); }    
    
    cntr->run_st = false;
    
    return(NULL);
}

void TVContr::Sync()
{
    for(unsigned i_x = 0; i_x < p_io_hd.size(); i_x++)
	if( !p_io_hd[i_x]->internal )
	{
	    try
	    {
		string vl_nm = "VAL";
		AutoHD<TVal> val = p_io_hd[i_x]->hd_g.at().at().vlAt(vl_nm);
		if( !val.at().valid() ) continue;
		if( p_io_hd[i_x]->sync )
		{
		    val.at().setR(p_io_hd[i_x]->x,NULL,true);
		    p_io_hd[i_x]->sync = false;
		}
		else p_io_hd[i_x]->x = val.at().getR( );
	    }catch(TError) {  }
	}    
    //Sync individual parameters
    for(unsigned i_p=0; i_p < p_hd.size(); i_p++)
	((TVPrm &)p_hd[i_p].at()).Sync();
}

TParamContr *TVContr::ParamAttach( const string &name, int type )
{
    return(new TVPrm(name,&owner().tpPrmAt(type)));
}

int TVContr::prm_connect( string nm )
{
    STime tm = {0,0}; 
    //Find already connected
    for(unsigned i_hd = 0; i_hd < p_io_hd.size(); i_hd++)
    {
	if( p_io_hd[i_hd]->internal && p_io_hd[i_hd]->hd_i.at().name() == nm )	
	    return(i_hd);
	if( !p_io_hd[i_hd]->internal && p_io_hd[i_hd]->hd_g.at().name() == nm )	
	    return(i_hd);
    }
    //Create new
    SIO *io = new SIO;
    try
    {
	io->hd_i	= at(nm,name());
	io->internal	= true;
    }
    catch(...)
    {   
	try
	{
	    io->hd_g 	= owner().owner().owner().param().at().at(nm,name());
    	    io->internal= false;
	}
	catch(...) { return(-1); }
    }
    io->x = 0.0;
    p_io_hd.push_back( io );
    
    return( p_io_hd.size()-1 );
}

SIO &TVContr::prm( unsigned hd )
{
    if(hd >= p_io_hd.size()) throw TError(nodePath().c_str(),"Hd %d no avoid!",hd);
    return( *p_io_hd[hd] );
}

//======================================================================
//==== TVPrm 
//====================================================================== 

TVPrm::TVPrm( string name, TTipParam *tp_prm) : 
    TParamContr(name,tp_prm), pid(NULL)
{

}

/*
void TVPrm::UpdateVAL( )
{
    TParamContr::UpdateVAL();
    
    
    hd_y  = vl_Elem().elNameId("VAL");
    if( vl_Elem().vle_Type(hd_y)&VAL_T_REAL )
    {
	STime tm = {0,0}; 
    	y_min = _vl_GetR(hd_y,tm,V_MIN);
	y_max = _vl_GetR(hd_y,tm,V_MAX);
    }
    else y_max = y_min = 0.0;
    
    if(vlElem().elName() == "PID")
    {	
    	pid = new SPID;
    	pid->hd_out  = vlElem().elNameId("OUT");
    	pid->hd_sp   = vlElem().elNameId("SP");
    	pid->hd_stat = vlElem().elNameId("STAT");	
    }
} 
*/

TVPrm::~TVPrm( )
{    
    if(pid) delete pid;
}

void TVPrm::vlSet( int id_elem )
{
    Mess->put(nodePath().c_str(),TMess::Warning,"Comand to direct set value of element!");
}


void TVPrm::vlGet( int id_elem )
{
    Mess->put(nodePath().c_str(),TMess::Warning,"Comand to direct get value of element!");
}

void TVPrm::load( )
{
    SAlgb *algb = NULL;
    try
    {
	algb = ( (TVirtual &)( (TVContr &)owner() ).owner() ).AlgbS()->GetAlg(name());
    }
    catch(TError err) 
    {
    	form = -1;
	return;
    }
    form = algb->tp_alg;
    
    y_id = ( (TVContr &)owner() ).prm_connect(name());
    for(unsigned i_x=0; i_x < algb->io.size(); i_x++)
    {
	if( i_x >= x_id.size() ) x_id.insert(x_id.begin()+i_x,-1);
	x_id[i_x] =  ( (TVContr &)owner() ).prm_connect(algb->io[i_x]);
    }
    for(unsigned i_k = 0;i_k < algb->kf.size(); i_k++)
    {
	if( i_k >= k.size() ) k.insert(k.begin()+i_k,0.0);
	k[i_k] = algb->kf[i_k];
    }
}

inline void TVPrm::Y(float val)
{
    float val_t;
    
    SIO &io = ( (TVContr &)owner() ).prm(y_id);   
    val_t = val;
    if(io.x != val_t) { io.sync = true; io.x = val_t; }
    /*
    STime tm = {0,0};
    _vl_SetR(hd_y,val,tm);
    */
}

inline float TVPrm::Y()
{ 
    return( ( (TVContr &)owner() ).prm(y_id).x );
    /*
    STime tm = {0,0};
    return(_vl_GetR(hd_y,tm));
    */
}

inline void TVPrm::X(unsigned id ,float val)
{ 
    float val_t;

    if(x_id[id] < 0) return;
    SIO &io = ( (TVContr &)owner() ).prm(x_id[id]);   
    val_t = val;
    if(io.x != val_t) { io.sync = true; io.x = val_t; }
}

inline float TVPrm::X(unsigned id)
{
    if(x_id[id] < 0) return(0.0);
    return( ( (TVContr &)owner() ).prm(x_id[id]).x );
}

void TVPrm::Sync()
{    

}

float TVPrm::Calc()
{
    if(form < 0) return(1E+10);

    
    switch( ((TVirtual &)((TVContr &)owner()).owner()).AlgbS()->GetFrm(form)->tip)
    {	
	case  0:return(0.0);
	case  1:return blok_dig();
        //case  2:return komyt(GB);
    	case  3:return ymn();
     	case  4:return sym();
	case  5:return free_formul();
//      case  8:return ymndl(GB);
//      case  9:return esx(GB);
//      case 10:return xsy(GB);
//      case 11:
//      case 12:return es1(GB);
//      case 13:return es3(GB);
//      case 14:return es4(GB);
//      case 15:return es5(GB);
//      case 16:return del(GB);
	case 17:return pid_n(); //pid(GB);
//      case 18:return nar(GB);
//      case 19:return zad_n(GB); //zad(GB);
//      case 20:return imzad(GB);
//      case 21:return petl(GB);
//      case 22:return im1(GB);
//      case 23:return symn(GB);
//      case 24:return sdel(GB);
//      case 25:return zap1(GB);
//      case 26:return zap2(GB);
//      case 27:return vard(GB);
//      case 28:return systep(GB);
//      case 29:return exp1(GB);
//      case 30:return perekl(GB);
//      case 31:return volna(GB);
//      case 32:return baj(GB);
//      case 33:return alarmk(GB);
//      case 34:return srob(GB);
    }
    Mess->put(nodePath().c_str(),TMess::Warning,"%d Furmule id= %d no present!",
		form, ((TVirtual &)((TVContr &)owner()).owner()).AlgbS()->GetFrm(form)->tip);

    return(1E+10);
}

//************************************************************
//* float blok_dig( );                                       *
//*   Сборка дискретных параметров.                          *
//************************************************************
float TVPrm::blok_dig( )
{
    bool set = false;

    if(X(0) && k[2] != 1.) { k[2]=1.; set = true; }
    if(X(2) && k[2] != 2.) { k[2]=2.; set = true; }
    if(X(4) && k[2] != 3.) { k[2]=3.; set = true; }
    if( set && k[0] > 0. ) { k[1]=k[0]; set = false; }
    if(k[1] > 0.) k[1] -= ((TVContr &)owner()).Period()/(1000*sysconf(_SC_CLK_TCK)*((TVContr &)owner()).Iterate());
    else
    {
    	k[1] = 0.;
	if(k[0] > 0.)
	{
	    if(k[2] == 1.) {k[2]=0.; X(0,0.); }
	    if(k[2] == 2.) {k[2]=0.; X(2,0.); }
	    if(k[2] == 3.) {k[2]=0.; X(4,0.); }
	}
    }
    return 0.;
}
       
//************************************************************
//* float ymn();                                             *
//*   Function simple multiply and divi.                     *
//************************************************************
float TVPrm::ymn()
{
    if(X(2)==0. || X(3)==0. || X(6)==0. || X(7)==0.) return(1E+10);
    if( !k[0] ) k[0] = 1.;
    return(k[0]*k[5]*X(0)*X(1)*X(4)*X(5))/(X(2)*X(3)*X(6)*X(7));
}
         
//************************************************************
//* float sym(a_fli *GB);                                    *
//*   Function addition.                                     *
//************************************************************
float TVPrm::sym()
{
    return(k[0]*X(0)+k[1]*X(1)+k[2]*X(2)+k[5]*X(3)+k[6]*X(4)+k[7]*X(5)+k[10]*X(6)+k[11]*X(7));
}

//************************************************************
//* float free_formul(a_fli *GB)                             *
//*   Flow formuls that free edit.                           *
//************************************************************
float TVPrm::free_formul( )
{
    int offset = 0;
    SFrm *formul = ((TVirtual &)((TVContr &)owner()).owner()).AlgbS()->GetFrm(form);
    return(calk_form(formul->form_e,formul->l_frm_e,&offset,0,0));
}
               
float TVPrm::calk_form(char *form, int len, int *off, float rez,byte h_prior)
{
    int     i,ii;
    dword   oper;
    float   parm=0,temp[10];
    byte    b_form;
    byte    symb=0;
    byte    flow_prior=0;
    byte    tmb_b;

    hom_f:
    if(*off > len-1) goto exit_f;
    b_form = form[*off];
    if(b_form=='(')
    {
	(*off)++;
	if(symb) parm = calk_form(form,len,off,0,0);  
	else     rez  = calk_form(form,len,off,0,0);
	goto hom_f;
    }    
    if(b_form==')' || b_form==',' || b_form==';')
    {
	exit_f:
	calk_oper(symb,&rez,parm);
	if( !h_prior && b_form!=';') (*off)++;
	return rez;
    }
    if(b_form=='Y')                              //Self param
    {
	(*off)++;
	if(form[*off]=='=')
	{ (*off)++; Y(calk_form(form,len,off,0,0)); (*off)++; goto hom_f; }
    	if(symb) parm = Y(); else rez = Y();
	goto hom_f;
    }
    if(b_form=='X')                              //External param
    {
	oper = form[++(*off)]; ++(*off);
	if(form[*off]=='='){(*off)++; X(oper,calk_form(form,len,off,0,0)); (*off)++; goto hom_f; }
	if(form[*off]=='('){(*off)++; X(oper,calk_form(form,len,off,0,0)); }
	if(symb) parm = X(oper); else rez = X(oper);
	goto hom_f;
    }
    if(b_form=='K')                              //Koefficients
    {
	oper = form[++(*off)]; ++(*off);
	if(form[*off]=='='){ (*off)++; k[oper]=calk_form(form,len,off,0,0); (*off)++; goto hom_f; }
	if(form[*off]=='('){ (*off)++; k[oper]=calk_form(form,len,off,0,0); }
	if(symb) parm = k[oper]; else rez = k[oper];
	goto hom_f;
    }
    if(b_form=='C')                              //Konstans
    {
	if(symb) parm = *(float *)(form+(++(*off)));
	else     rez  = *(float *)(form+(++(*off)));
	(*off)+=4; goto hom_f;
    }
    //-----------------------
    if(b_form=='&' || b_form=='|')                 {i=5;  goto m1;}
    if(b_form=='>' || b_form=='<' || b_form=='~')  {i=10; goto m1;}
    if(b_form=='+' || b_form=='-')                 {i=15; goto m1;}
    if(b_form=='*' || b_form=='/')                 {i=20; goto m1;}
    if(b_form=='^')
    {
	i=25;
	m1:
	if(!symb) { symb = b_form; flow_prior=i; ++(*off); goto hom_f; }
	if(flow_prior == i){ calk_oper(symb,&rez,parm); symb = b_form; ++(*off); goto hom_f; }
	if(flow_prior > i)
    	{
	    calk_oper(symb,&rez,parm);
    	    if(h_prior) return rez;
	    flow_prior=i;
	    symb=b_form;
	    ++(*off);
	    goto hom_f;
	}
	if(flow_prior < i) { parm = calk_form(form,len,off,parm,1); goto hom_f; }
    }
    //            ---- Function ----
    oper = *(dword *)(form+(*off));
    if((word)oper == *(word *)"pi")               //pi
    { (*off)+=2; if(symb) parm=3.14159256; else rez=3.14159256; goto hom_f; }
    if((oper&0x00FFFFFF) == 'krb'){ (*off) = len; goto exit_f;}   //brk
    if((oper&0x00FFFFFF) == 'qrf')                 //frq
    {
	(*off)+=3;
	if(symb) parm = 1000*((TVContr &)owner()).Iterate()*sysconf(_SC_CLK_TCK)/((TVContr &)owner()).Period();
	else     rez  = 1000*((TVContr &)owner()).Iterate()*sysconf(_SC_CLK_TCK)/((TVContr &)owner()).Period();
	goto hom_f;
    }
    if(oper == *(dword *)"exp(")                             //exp()
    {
	(*off)+=4;
    	if(symb) parm = exp(calk_form(form,len,off,0,0));
	else     rez  = exp(calk_form(form,len,off,0,0));
	goto hom_f;
    }
    if(oper == *(dword *)"abs(")                             //abs()
    {
    	(*off)+=4;
	if(symb) parm = fabs(calk_form(form,len,off,0,0));
	else     rez  = fabs(calk_form(form,len,off,0,0));
	goto hom_f;
    }
    if(oper == *(dword *)"sig(")                             //sig()
    {
	(*off)+=4;
	temp[0] = calk_form(form,len,off,0,0);
	if(temp[0] >= 0.) temp[0]=1.0; else temp[0]=-1.0;
	if(symb) parm = temp[0]; else rez = temp[0];
	goto hom_f;
    }
    if(oper == *(dword *)"cos(")                             //cos()
    {
	(*off)+=4;
    	if(symb) parm = cos(calk_form(form,len,off,0,0));
	else     rez  = cos(calk_form(form,len,off,0,0));
	goto hom_f;
    }
    if((oper&0x00FFFFFF) =='(nl')                       //ln()
    {
	(*off)+=3;
	if(symb) parm = log(calk_form(form,len,off,0,0));
	else     rez  = log(calk_form(form,len,off,0,0));
    	goto hom_f;
    }
    if(oper == *(dword *)"sin(")                             //sin()
    {
	(*off)+=4;
	if(symb) parm = sin(calk_form(form,len,off,0,0));
	else     rez  = sin(calk_form(form,len,off,0,0));
	goto hom_f;
    }
    if(oper == *(dword *)"rnd(")                             //rnd()
    {
	(*off)+=4;
	if(symb) parm = ((float)rand()/(float)RAND_MAX) * calk_form(form,len,off,0,0);
	else     rez  = ((float)rand()/(float)RAND_MAX) * calk_form(form,len,off,0,0);
	goto hom_f;
    }
    if(oper == *(dword *)"max(")                             //max()
    {
	(*off)+=4;
	temp[1] = calk_form(form,len,off,0,0); 
	temp[2] = calk_form(form,len,off,0,0);
	if(symb) { if(temp[1] > temp[2]) parm = temp[1]; else parm = temp[2]; }
	else     { if(temp[1] > temp[2]) rez  = temp[1]; else rez  = temp[2]; }
	goto hom_f;
    }
    if(oper == *(dword *)"min(")                             //min()
    {
	(*off)+=4;
    	temp[1] = calk_form(form,len,off,0,0); 
	temp[2] = calk_form(form,len,off,0,0);
	if(symb) { if(temp[1] < temp[2]) parm = temp[1]; else parm = temp[2]; }
	else     { if(temp[1] < temp[2]) rez  = temp[1]; else rez  = temp[2]; }
	goto hom_f;
    }
    if(oper == *(dword *)"msg(")                             //msg()
    {
	(*off)+=4;
	temp[9] = calk_form(form,len,off,0,0);
	for(;form[*off] != '"'; (*off)++);
	for(i=0 , (*off)++; (*off)+i <= len-1;i++)
	if(form[(*off)+i]=='"')
	{
	    /*strncpy(buf_glb,PTR_STR+FORM_OFFS,i);*/ 
	    (*off)+=i; 
	    /*buf_glb[i]=0;*/ 
	    i=0;
	    break; 
	}
	for((*off)++;(*off)<=len-1;(*off)++)
	{
	    if(form[*off]==',' || form[*off]==')')
	    {
    		(*off)++; ii=0;
		while(form[(*off)-1]==',') { temp[ii] = calk_form(form,len,off,0,0); ii++;}
		//       sprintf(srv_mes.mess,buf_glb,temp[0],temp[1],temp[2],temp[3],temp[4],temp[5]);
		//       srv_mes.tip_mes=temp[9]; s_mes();
                if(symb) parm= 0.; else rez= 0.;
                goto hom_f;
	    }
	}
    }
    if((oper&0x00FFFFFF) =='(fi')                             //if()  :
    {
    	(*off)+=3;
	if(calk_form(form,len,off,0,0) > 0)
	{
	    if(symb) parm = calk_form(form,len,off,0,0);
	    else     rez  = calk_form(form,len,off,0,0);
	    if(form[(*off)-1]==')') { (*off)--; goto hom_f; }
	    if( (*off) > len-1) goto hom_f;
	    i=0;
	    while((*off) <= len-1)
	    {
		tmb_b = form[*off];
		if(tmb_b=='K' || tmb_b=='X') (*off)+=1;
		if(tmb_b=='C')               (*off)+=4;
    		if(tmb_b=='(') i++;
		if(tmb_b==')') { i--; if(i== -1) { (*off)++; goto hom_f;} }
		if(tmb_b=='"') for((*off)++; ((*off) <= len-1) && form[*off]!='"'; (*off)++);
		(*off)++;
	    }
	    goto hom_f;
	}
	i=0;
    	while( (*off) <= len-1)
	{
	    tmb_b = form[*off];
	    if(tmb_b=='K' || tmb_b=='X') (*off)+=1;
    	    if(tmb_b=='C')               (*off)+=4;
	    if(tmb_b=='(') i++;
	    if(tmb_b==')') {i--;if(i== -1)goto hom_f;}
	    if(tmb_b==',' && !i)
	    {
		(*off)++;
		if(symb) parm = calk_form(form,len,off,0,0);
		else     rez  = calk_form(form,len,off,0,0);
		goto hom_f;
	    }
    	    if(tmb_b=='"')
	    for( (*off)++;( (*off) <= len-1) && form[*off]!='"';(*off)++);
	    (*off)++;
	}
	goto hom_f;
    }
    return 0.;
}
//--------------------------------------------------------------
void TVPrm::calk_oper(byte symb,float *rez,float parm)
{
    if(symb=='+'){ *rez +=parm; return;}
    if(symb=='-'){ *rez -=parm; return;}
    if(symb=='*'){ *rez *=parm; return;}
    if(symb=='/'){ *rez /=parm; return;}
    if(symb=='^'){ *rez=pow(*rez,parm); return;}
    if(symb=='>'){ if(*rez >parm)*rez=1;else *rez=0; return;}
    if(symb=='<'){ if(*rez <parm)*rez=1;else *rez=0; return;}
    if(symb=='~'){ if(*rez==parm)*rez=1;else *rez=0; return;}
    if(symb=='&'){ *rez=*rez &&parm; return;}
    if(symb=='|'){ *rez=*rez ||parm; return;}
}

//************************************************************
//* float pid_n(a_fli *GB);                                  *
//*  Function of a PID regulator                             *
//************************************************************
float TVPrm::pid_n( )
{
    float  err=0.,vhod=0.,KInt,Kzdif,Dif,Kf,k1,k2,k3,k4,in;

    if(!pid) return(1E+10);
    
    
    int    period      = ((TVContr &)owner()).Period();
    int    HZ          = 1000*sysconf(_SC_CLK_TCK);
    int    cnt_in_cycl = ((TVContr &)owner()).Iterate();
    float  sp          = pid->sp->getR();
    float  out         = pid->out->getR();
    char   stat        = pid->stat->getI();
    
    k1 = 100 * X(1)/( (labs((long)Y_MAX()) > labs((long)Y_MIN()))?labs((long)Y_MAX()):labs((long)Y_MIN()) );
    k2 = 100 * X(2)/( (labs((long)Y_MAX()) > labs((long)Y_MIN()))?labs((long)Y_MAX()):labs((long)Y_MIN()) );
    k3 = 100 * X(3)/( (labs((long)Y_MAX()) > labs((long)Y_MIN()))?labs((long)Y_MAX()):labs((long)Y_MIN()) );
    k4 = 100 * X(4)/( (labs((long)Y_MAX()) > labs((long)Y_MIN()))?labs((long)Y_MAX()):labs((long)Y_MIN()) );
    
    if(x_id[5] >= 0)  sp = 100.*(X(5)-Y_MIN())/(Y_MAX()-Y_MIN());

    if(stat != R_MAN && stat != R_AUTO && stat != R_CAS) stat = R_MAN;
    if(stat == R_CAS && x_id[6] < 0) stat = R_AUTO;
    //if(stat == R_CAS) sp =    c_ptr[ALGB->inp[6].nc].a_ptr[ALGB->inp[6].nz].out_cod;
    in = 100. * (X(0)-Y_MIN())/(Y_MAX()-Y_MIN());

    vhod = in+k[0]*k1+k[1]*k2;
    if(vhod >  100.) vhod =  100.;
    if(vhod < -100.) vhod = -100.;
	    
    err = sp - vhod;
    if((err > 0. && err < k[6]) || (err < 0. && err > -k[6])) err = 0.;
    if( err > 0. ) err -= k[6];
    if( err < 0. ) err += k[6];

    err *= k[9];
    if(err >  100.) err =  100.;
    if(err < -100.) err = -100.;
	    
    if( k[12] > period/(HZ*cnt_in_cycl) ) Kf = period/(k[12]*HZ*cnt_in_cycl);
    else                                  Kf = 1.;
    k[7] += Kf*(err - k[7]);

    if(stat != R_MAN)
    {
	if(k[10] > period/(HZ*cnt_in_cycl)) KInt = period/(k[10]*HZ*cnt_in_cycl);
	else                                KInt = 1.;
	if(k[11] > period/(HZ*cnt_in_cycl)) Kzdif = period/(k[11]*HZ*cnt_in_cycl);
	else                                Kzdif = 1.;

	k[14] -= Kzdif * (k[14]-k[7]);
	k[14] -= Kzdif * (k[14]-k[7]);
	Dif = k[7]-k[14];
	k[13] += KInt * k[7];
	out = (k[7] + k[13] + Dif);

	out += (k[2] * k3 + k[3] * k4);

	if(out > k[5]){ out = k[5]; k[13] = out-k[7]-Dif-(k[2] * k3 + k[3] * k4);}
	if(out < k[4]){ out = k[4]; k[13] = out-k[7]-Dif-(k[2] * k3 + k[3] * k4);}
        /*
	if(x_id[6].hd_prm >= 0)
	{
	    if( pid->stat == R_CAS) pid->sp = ((c_ptr+ALGB->inp[6].nc)->a_ptr+ALGB->inp[6].nz)->out_cod;
	    else
	    {
		((c_ptr+ALGB->inp[6].nc)->a_ptr+ALGB->inp[6].nz)->out_cod=ptr_a->set_cod;
		((c_ptr+ALGB->inp[6].nc)->a_ptr+ALGB->inp[6].nz)->mod_cod=R_MAN;
	    }
	}
	*/
    }
    else
    {
    	k[13] = out - k[2] * k3 - k[3] * k4 - k[7];
	k[14] = k[7];
    }

    if( x_id[7] >= 0 ) X(7,out);

    pid->sp->setR(sp);
    pid->out->setR(out);
    pid->stat->setI(stat);

    if( x_id[0] < 0 )  return 0.;
    return X(0);
}

//======================================================================
//==== TVirtAlgb =======================================================
//====================================================================== 

TVirtAlgb::TVirtAlgb(string cfg_file) : file(cfg_file)
{
    load();
}

TVirtAlgb::~TVirtAlgb( )
{
    Free();
}

void TVirtAlgb::load(string f_alg)
{
    int        buf_len = 100000;
    dword      ofs_alg;
    word       form_am, k_alg, tp_alg, len_2;
    byte       len_1;
    char       *buf, *file_alg;
    
    if(f_alg.size())     file_alg = (char *)f_alg.c_str();
    else if(file.size()) file_alg = (char *)file.c_str();
    else throw TError("algb","Algobloc's file no present!");
   
    int fh = open(file_alg,O_RDONLY);
    if(fh == -1) throw TError("algb","Open file <%s> for read, error!",file_alg);    

    Free();

    buf = (char *)malloc(buf_len);
    
    read(fh,&ofs_alg,4); 
    read(fh,&form_am,2);
    for(unsigned i_frm = 0; i_frm < form_am; i_frm++)
    {
	SFrm *frm = new SFrm;
       	read(fh,&len_1,1);
	if(len_1)
	{
	    read(fh,buf,len_1); buf[len_1]=0;
	    frm->name = Mess->SconvIn("CP866",buf);
	}
       
	read(fh,&frm->tip,1);
	read(fh,&frm->n_inp,1); 
	read(fh,&frm->n_koef,1);
	if(frm->n_inp)
   	    for(unsigned i_inp=0;i_inp < frm->n_inp;i_inp++)
   	    {
		if( i_inp == frm->name_inp.size() ) 
		    frm->name_inp.push_back("");
       		read(fh,&len_1,1);
	       	read(fh,buf,len_1); buf[len_1] = 0;
	       	frm->name_inp[i_inp] = buf;
	    }
	if(frm->n_koef)
	    for(unsigned i_kf=0;i_kf < frm->n_koef;i_kf++)
	    {
    		if( i_kf == frm->name_kf.size() ) 
		    frm->name_kf.push_back("");
		read(fh,&len_1,1);
		read(fh,buf,len_1); buf[len_1] = 0;
		frm->name_kf[i_kf] = buf;
	    }
	read(fh,&len_2,2);
    	read(fh,buf,len_2); buf[len_2] = 0;
      	frm->formul = buf;
	if(frm->tip==5)
    	{
	    if(!frm->formul.size()) continue;
       	    char *str1 = (char *)frm->formul.c_str();
   	    unsigned i_c, i_n;
	    for( i_c = 0, i_n=0;i_c < strlen(str1);i_c++)
   	    {
       		if(*(word *)(str1+i_c) == *(word *)"//")
	   	{ 
		    for( ; (i_c < strlen(str1) && str1[i_c]!=0x0D);i_c++) ; 
		    continue;
		}
		if(str1[i_c] == '"' )
		{
		    buf[i_n++] = str1[i_c++];
		    for( ; i_c < strlen(str1);i_c++,i_n++)
		    {
	      		buf[i_n] = str1[i_c];
		    	if(str1[i_c] == '"') 
			{
			    buf[i_n++] = str1[i_c]; 
			    break;
			}
		    }
		    continue;
		}
		if(str1[i_c] == ' ' || str1[i_c] == '\x0D' || str1[i_c] == '\x0A') continue;
		if(str1[i_c] == 'K' || str1[i_c] == 'X')
    		{
	 	    buf[i_n] = str1[i_c];
     		    i_c++; i_n++;
     		    buf[i_n] = str1[i_c];
     		    i_n++;
     		    continue;
	 	}
		if(str1[i_c] == 'C')
		{
		    buf[i_n] = str1[i_c];
		    i_c++; i_n++;
		    *(float*)(buf+i_n) = *(float*)(str1+i_c);
		    i_c+=3; i_n+=4;
		    continue;
		}
		buf[i_n]=str1[i_c]; 
		i_n++;
	    }
	    buf[i_n] = 0;
            frm->form_e  = strdup(buf);
	    frm->l_frm_e = i_n;
	}
	frm_s.push_back(frm);
	//if(i_frm == frm_s.size()) frm_s.push_back(frm);
	//else                      frm_s[i_frm] = frm;
    }
    lseek(fh,ofs_alg,SEEK_SET);
    read(fh,&k_alg,2);
    for(unsigned i_alg=0; i_alg < k_alg; i_alg++)
    {
	SAlgb *algb = new SAlgb;
	
    	read(fh,buf,9); 
	buf[9] = 0;
	for(int i=8; i >= 0; i--) 
	    if(buf[i]==' ' || buf[i]== 0) buf[i]=0; 
	    else break; 
	algb->name = Mess->SconvIn("CP866",buf);

	read(fh,&len_1,1); 
	read(fh,buf,len_1); buf[len_1]=0;
	algb->descr = Mess->SconvIn("CP866",buf);
	
	read(fh,&tp_alg,2);
	algb->tp_alg = tp_alg;
	
	unsigned i_n = frm_s[tp_alg]->n_inp;
	if(i_n)
	    for(unsigned i_x=0;i_x < i_n;i_x++)
	    { 
		if( i_x == algb->io.size() ) algb->io.push_back("");
		
		read(fh,buf,9); buf[9] = 0;
		for(int i=8; i >= 0; i--) 
		    if(buf[i]==' ' || buf[i]== 0) buf[i]=0; 
		    else break;
		algb->io[i_x] = Mess->SconvIn("CP866",buf);
	    }
	i_n = frm_s[tp_alg]->n_koef;
	if(i_n)
	    for(unsigned i_k = 0;i_k < i_n; i_k++)
	    {
		if( i_k == algb->kf.size() ) algb->kf.push_back(0.0);
		read(fh,&(algb->kf[i_k]),4);
	    }
	algb_s.push_back(algb);
    }
    free(buf);
    close(fh); 
}

void TVirtAlgb::save(string f_alg)
{
    char   *file_alg;
    
    if(f_alg.size())     file_alg = (char *)f_alg.c_str();
    else if(file.size()) file_alg = (char *)file.c_str();
    else throw TError("algb","Algobloks file no present!");
   
    int fh = open(file_alg,O_WRONLY);
    if(fh == -1) throw TError("algb","Open file <%s> for write, error!",file_alg);
    //----------------
    //make in future
    //----------------    
    close(fh); 
}

void TVirtAlgb::Free( )
{
    while( algb_s.size() )
    {
        delete algb_s[0];	
	algb_s.erase(algb_s.begin());
    }
    while( frm_s.size() )
    {
	if( frm_s[0]->tip==5 ) free( frm_s[0]->form_e );
        delete frm_s[0];
	frm_s.erase(frm_s.begin());
    }
}


SAlgb *TVirtAlgb::GetAlg(string name)
{
    for(unsigned i_alg = 0; i_alg < algb_s.size(); i_alg++)
	if(algb_s[i_alg]->name == name) 
	    return(algb_s[i_alg]);
    throw TError("algb","Algoblok <%s> no present!",name.c_str());
}

SFrm *TVirtAlgb::GetFrm(unsigned id)
{
    if( id < frm_s.size() ) return( frm_s[id] );
    throw TError("algb","Formula %d no present!",id);
}

