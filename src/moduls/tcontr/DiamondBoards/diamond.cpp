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

#include <sys/time.h>
#include <getopt.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <tsys.h>
#include <ttiparam.h>

#include "diamond.h"

//============ Modul info! =====================================================
#define MOD_ID      "DiamondBoards"
#define MOD_NAME    "Diamond DA boards"
#define MOD_TYPE    "Controller"
#define VER_TYPE    VER_CNTR
#define VERSION     "0.5.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow access to Diamond systems DA boards. Include support of Athena board."
#define LICENSE     "GPL"
//==============================================================================

Diamond::TTpContr *Diamond::mod;

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
	Diamond::TTpContr *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = Diamond::mod = new Diamond::TTpContr( source );

	return ( self_addr );
    }
}

using namespace Diamond;

//======================================================================
//==== TTpContr ======================================================== 
//======================================================================
TTpContr::TTpContr( string name ) : 
    m_init(false), elem_ai("AI"), elem_ao("AO"), elem_di("DI"), elem_do("DO")
{
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;    
}

TTpContr::~TTpContr()
{    
    BYTE result;
    ERRPARAMS errparams;
    
    if( m_init )    
	if((result = dscFree()) != DE_NONE)
	{
	    dscGetLastError(&errparams);
	    Mess->put(nodePath().c_str(),TMess::Error,"dscFree failed: %s (%s)", dscGetErrorString(result), errparams.errstring);
	}
}

void TTpContr::postEnable()
{    
    BYTE result;
    ERRPARAMS errparams;
    
    TModule::postEnable();
    
    //==== Controler's bd structure ====
    fldAdd( new TFld("BOARD",I18N("Diamond system board"),TFld::Dec,FLD_SELECT,"3","0",
	"0;1;2;3;4;5;6;7;8;9;10;11;12;12;13;14;15;16;17;18;19;20;21;22;22;23;24;24;24;25",
	"DMM16;RMM;TMM;OPMM;DMM;SMM;GMM;QMM;ZMM;PMM;OMM;RMM416;DMM32;DMM32AT;EMMDIO;RMM1612;DMMAT;DMM16AT;IR104;EMM8;PROM;HERCEBX;CPT;DMM48;DMM48AT;OMMDIO;DIO82C55;MRC;EMMOPTO;ATHENA") );
    fldAdd( new TFld("PRM_BD_A",I18N("Analog parameters' table"),TFld::String,0,"30","diamond_prm_a") );
    fldAdd( new TFld("PRM_BD_D",I18N("Digital parameters' table"),TFld::String,0,"30","diamond_prm_d") );    
    fldAdd( new TFld("ADDR",I18N("Base board address"),TFld::Hex,0,"3","768") );
    fldAdd( new TFld("INT",I18N("Interrupt vector"),TFld::Dec,0,"2","5") );
    fldAdd( new TFld("DMA",I18N("DMA number"),TFld::Dec,0,"1","1") );
    fldAdd( new TFld("DIO_CFG",I18N("Digit IO config byte"),TFld::Hex,0,"2","0") );
    
    //==== Parameter type bd structure ====
    //---- Analog ----
    int t_prm = tpParmAdd("a_prm","PRM_BD_A",I18N("Analog parameter"));
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",I18N("Analog parameter type"),TFld::Dec,FLD_SELECT|FLD_NOVAL|FLD_PREV,"1","0","0;1",I18N("Input;Output")) );
    tpPrmAt(t_prm).fldAdd( new TFld("CNL",I18N("Channel"),TFld::Dec,FLD_NOVAL|FLD_PREV,"2","0") );
    tpPrmAt(t_prm).fldAdd( new TFld("GAIN",I18N("A/D converter gain"),TFld::Dec,FLD_SELECT|FLD_NOVAL|FLD_PREV,"1","0","0;1;2;3","x1;x2;x4;x8") );
    //tpPrmAt(t_prm).fldAdd( new TFld("POLAR",I18N("Polarity"),TFld::Bool,FLD_SELECT|FLD_NOVAL|FLD_PREV,"1","false","false;true","Bipolar;Unipolar") );
    //---- Digit ----
    t_prm = tpParmAdd("d_prm","PRM_BD_D",I18N("Digital parameter"));
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",I18N("Digital parameter type"),TFld::Dec,FLD_SELECT|FLD_NOVAL|FLD_PREV,"1","0","0;1",I18N("Input;Output")) );
    tpPrmAt(t_prm).fldAdd( new TFld("PORT",I18N("Port"),TFld::Dec,FLD_SELECT|FLD_NOVAL,"2","0","0;1;2","A;B;C") );
    tpPrmAt(t_prm).fldAdd( new TFld("CNL",I18N("Channel"),TFld::Dec,FLD_NOVAL,"1") );

    //==== Init value elements =====
    //---- Analog input ----
    elem_ai.fldAdd( new TFld("value",I18N("Value %"),TFld::Real,FLD_NWR|FLD_DRD,"6.2","0") );
    elem_ai.fldAdd( new TFld("voltage",I18N("Voltage V"),TFld::Real,FLD_NWR|FLD_DRD,"7.4","0") );
    elem_ai.fldAdd( new TFld("code",I18N("A/D code"),TFld::Hex,FLD_NWR|FLD_DRD,"4","0") );
    //---- Analog output ----
    elem_ao.fldAdd( new TFld("value",I18N("Value %"),TFld::Real,FLD_DWR,"6.2","0") );
    elem_ao.fldAdd( new TFld("voltage",I18N("Voltage V"),TFld::Real,FLD_DWR,"7.4","0") );
    //---- Digit input ----
    elem_di.fldAdd( new TFld("value",I18N("Value"),TFld::Bool,FLD_NWR|FLD_DRD,"1","0") );
    //---- Digit output ----
    elem_do.fldAdd( new TFld("value",I18N("Value"),TFld::Bool,FLD_DWR,"1","0") );    

    //==== Init DSC ====
    if((result = dscInit(DSC_VERSION)) != DE_NONE)
    {
        dscGetLastError(&errparams);
	Mess->put(nodePath().c_str(),TMess::Error,"dscInit failed: %s (%s)", dscGetErrorString(result), errparams.errstring);
    }
    else m_init = true;
}

TController *TTpContr::ContrAttach( const string &name, const TBDS::SName &bd)
{
    return( new TMdContr(name,bd,this));    
}

//======================================================================
//==== TMdContr 
//======================================================================
TMdContr::TMdContr( string name_c, const TBDS::SName &bd, ::TElem *cfgelem) :
	::TController(name_c,bd,cfgelem) 
{
    cfg("PRM_BD_A").setS("Diam_"+name_c+"_prm_a");
    cfg("PRM_BD_D").setS("Diam_"+name_c+"_prm_d");
}

TMdContr::~TMdContr()
{
    
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{    
    return(new TMdPrm(name,&owner().tpPrmAt(type)));
}

void TMdContr::load( )
{
    TController::load();
}

void TMdContr::save( )
{
    TController::save();
}

void TMdContr::start( )
{
    BYTE result;
    ERRPARAMS errparams;
    
    TController::start();
    
    //Check inited of Diamond API    
    if( !((TTpContr &)owner()).initStat() )
        throw TError(mod->nodePath().c_str(),"Module no inited!");
    
    if( !run_st )
    {
	DSCCB dsccb;
	
	dsccb.base_address	= cfg("ADDR").getI();
        dsccb.int_level 	= cfg("INT").getI();
        dsccb.dma_level 	= cfg("DMA").getI();

	if((result = dscInitBoard(cfg("BOARD").getI(), &dsccb, &dscb)) != DE_NONE)
	{
	    dscGetLastError(&errparams);
	    throw TError(nodePath().c_str(),"%s (%s)!",dscGetErrorString(result), errparams.errstring);				    
	}
	
	//Init IO ports	
	BYTE config_bytes = cfg("DIO_CFG").getI()|0x80;
	if((result = dscDIOSetConfig(dscb, &config_bytes)) != DE_NONE)
        {
	    dscGetLastError(&errparams);
            throw TError(nodePath().c_str(),"%s (%s)!",dscGetErrorString(result), errparams.errstring);
	}				
		
	run_st = true;
	
	//Enable all parameters
	vector<string> prm_list;
	for( int i_prm = 0; i_prm < prm_list.size(); i_prm++ )
	    if( at(prm_list[i_prm]).at().toEnable() )
		at(prm_list[i_prm]).at().enable();
    }    
}

void TMdContr::stop( )
{  
    BYTE result;
    ERRPARAMS errparams;
    
    TController::stop();
    
    if( run_st )
    {
	if((result = dscFreeBoard(dscb)) != DE_NONE)
	{
	    dscGetLastError(&errparams);
            throw TError(nodePath().c_str(),"%s (%s)!",dscGetErrorString(result), errparams.errstring);	
	}	
	run_st = false;
    }    
} 

void TMdContr::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
        TController::cntrCmd_( a_path, opt, cmd );
	
        ctrMkNode("area",opt,a_path.c_str(),"/board",owner().I18N("Board config"));
        ctrMkNode("area",opt,a_path.c_str(),"/board/dio",owner().I18N("Digital IO ports. Select input!"));
	if( cfg("BOARD").getI() == DSC_PROM )
	{
	    ctrMkNode("fld",opt,a_path.c_str(),"/board/dio/a",owner().I18N("Port A"),0664,0,0,"bool");
	    ctrMkNode("fld",opt,a_path.c_str(),"/board/dio/b",owner().I18N("Port B"),0664,0,0,"bool");
	    ctrMkNode("fld",opt,a_path.c_str(),"/board/dio/c1",owner().I18N("Port C1"),0664,0,0,"bool");
	    ctrMkNode("fld",opt,a_path.c_str(),"/board/dio/c2",owner().I18N("Port C2"),0664,0,0,"bool");
	}
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path.substr(0,11) == "/board/dio/" )
	{
	    string port_n = TSYS::pathLev(a_path,2);
	    int	cfg_b = cfg("DIO_CFG").getI();
	    if( port_n == "a" )		ctrSetB( opt, cfg_b&0x10 );
	    else if( port_n == "b" ) 	ctrSetB( opt, cfg_b&0x02 );
	    else if( port_n == "c1" )	ctrSetB( opt, cfg_b&0x01 );
	    else if( port_n == "c2" )   ctrSetB( opt, cfg_b&0x08 );	
	}
	else TController::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path.substr(0,11) == "/board/dio/" )
        {
            string port_n = TSYS::pathLev(a_path,2);
	    int cfg_b = cfg("DIO_CFG").getI();
	    if( port_n == "a" )         cfg("DIO_CFG").setI(ctrGetB(opt)?cfg_b|0x10:cfg_b&(~0x10));
    	    else if( port_n == "b" )    cfg("DIO_CFG").setI(ctrGetB(opt)?cfg_b|0x02:cfg_b&(~0x02));
	    else if( port_n == "c1" )   cfg("DIO_CFG").setI(ctrGetB(opt)?cfg_b|0x01:cfg_b&(~0x01));
	    else if( port_n == "c2" )   cfg("DIO_CFG").setI(ctrGetB(opt)?cfg_b|0x08:cfg_b&(~0x08));
	    
	    //Change DI config if started
	    if( run_st )
	    {
		BYTE result;
		ERRPARAMS errparams;
		BYTE config_bytes = cfg("DIO_CFG").getI()|0x80;
		if((result = dscDIOSetConfig(dscb, &config_bytes)) != DE_NONE)
		{
            	    dscGetLastError(&errparams);
                    throw TError(nodePath().c_str(),"%s (%s)!",dscGetErrorString(result), errparams.errstring);
        	}
	    }								   
	}
	else TController::cntrCmd_( a_path, opt, cmd );
    }
}			    

//======================================================================
//==== TMdPrm 
//======================================================================
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) :
    TParamContr(name,tp_prm), m_cnl(cfg("CNL").getId()), m_tp(NONE)
{          

}

TMdPrm::~TMdPrm( )
{   

}

void TMdPrm::postEnable()
{
    TParamContr::postEnable();

    if( TParamContr::type().name() == "a_prm" )     	type(AI);
    else if( TParamContr::type().name() == "d_prm" )	type(DI);
}

void TMdPrm::preDisable( int flag )
{    
    type(NONE);
    
    TParamContr::preDisable(flag);
}

void TMdPrm::type( TMdPrm::Type vtp )
{
    //Free previos type
    if( m_tp == AI )		vlDetElem( &((TTpContr&)owner().owner()).elemAI() );
    else if( m_tp == AO )	vlDetElem( &((TTpContr&)owner().owner()).elemAO() );
    else if( m_tp == DI )  	vlDetElem( &((TTpContr&)owner().owner()).elemDI() );
    else if( m_tp == DO )  	vlDetElem( &((TTpContr&)owner().owner()).elemDO() );    
    
    //Init new type
    if( vtp == AI )
    {    
	cfg("GAIN").view(true);
	ad_set.current_channel      = m_cnl;
	ad_set.gain                 = cfg("GAIN").getI();
	ad_set.range                = 1;
	ad_set.polarity             = 0;// cfg("POLAR").getB();
	ad_set.load_cal             = 0;
	
	vlAttElem( &((TTpContr&)owner().owner()).elemAI() );
    }
    else if( vtp == AO )
    {
	cfg("GAIN").view(false);    
	vlAttElem( &((TTpContr&)owner().owner()).elemAO() );
    }
    else if( vtp == DI )
    {
	vlAttElem( &((TTpContr&)owner().owner()).elemDI() );
    }
    else if( vtp == DO )
    {
        vlAttElem( &((TTpContr&)owner().owner()).elemDO() );
    }    
    m_tp = vtp;
}

void TMdPrm::enable()
{
    TParamContr::enable();
}

void TMdPrm::disable()
{
    TParamContr::disable();
}

bool TMdPrm::cfgChange( TCfg &i_cfg )
{
    //Change TYPE parameter
    if( i_cfg.name() == "TYPE" )
    {
	if( i_cfg.getI() == 0 && type() == AO )		type(AI);
	else if( i_cfg.getI() == 0 && type() == DO ) 	type(DI);
	if( i_cfg.getI() == 1 && type() == AI ) 	type(AO);
        else if( i_cfg.getI() == 1 && type() == DI ) 	type(DO);
	else return false;
    }
    if( type() == AI )
    {
	if( i_cfg.name() == "GAIN" ) 		ad_set.gain = cfg("GAIN").getI();
	else if( i_cfg.name() == "CNL" )	ad_set.current_channel = m_cnl;
	//else if( i_cfg.name() == "POLAR" )	ad_set.polarity = cfg("POLAR").getB();
	else return false;
    }
    
    return(true);
}									

void TMdPrm::vlSet( TVal &val )
{
    BYTE result;
    ERRPARAMS errparams;
	
    if( type() == AO )
    {		
	if(val.name()=="value")
	{
	    Mess->put(nodePath().c_str(),TMess::Info,"AO %d. set value to: %f\%",m_cnl,val.getR(NULL,true) );
	    result = dscDAConvert( ((TMdContr&)owner()).cntrAccess(), m_cnl,(DSCDACODE)val.getR(NULL,true) );
	}
	else if(val.name()=="voltage")
	{
	    Mess->put(nodePath().c_str(),TMess::Info,"AO %d. set voltage to: %fv",m_cnl,val.getR(NULL,true) );
	    result = dscDAConvert( ((TMdContr&)owner()).cntrAccess(), m_cnl,(DSCDACODE)(4095*val.getR(NULL,true)/10.) );
	}
	if( result != DE_NONE )
        {
            dscGetLastError(&errparams);
            Mess->put(nodePath().c_str(),TMess::Error,"%s (%s)",dscGetErrorString(result), errparams.errstring);
	}
    }
    else if( type() == DO )
    {
	if((result = dscDIOOutputBit( ((TMdContr&)owner()).cntrAccess(),cfg("PORT").getI(),cfg("CNL").getI(),val.getB(NULL,true)) ) != DE_NONE )
	{
	    dscGetLastError(&errparams);
            Mess->put(nodePath().c_str(),TMess::Error,"%s (%s)",dscGetErrorString(result), errparams.errstring);
	}
    }	
}

void TMdPrm::vlGet( TVal &val )
{
    BYTE result;
    ERRPARAMS errparams;

    if( type() == AI )
    {
	DSCSAMPLE dscsample;
	
	//Controll request
	if((result = dscADSetSettings( ((TMdContr&)owner()).cntrAccess(), &ad_set)) != DE_NONE )
	{
	    dscGetLastError(&errparams);
	    Mess->put(nodePath().c_str(),TMess::Error,"%s (%s)",dscGetErrorString(result), errparams.errstring);
	}
	else if((result = dscADSample( ((TMdContr&)owner()).cntrAccess(), &dscsample)) != DE_NONE )
	{
	    dscGetLastError(&errparams);
	    Mess->put(nodePath().c_str(),TMess::Error,"%s (%s)",dscGetErrorString(result), errparams.errstring);
	}
	else
	{
	    if(val.name()=="value")	val.setR(100.*((double)dscsample/32768.),NULL,true);
	    else if(val.name()=="code")	val.setI(dscsample,NULL,true);
	    else if(val.name()=="voltage")	val.setR(10.*((double)dscsample/32768.),NULL,true);	    	    
	}
    }
    else if( type() == DI )
    {
	BYTE vl_bit;
	if((result = dscDIOInputBit(((TMdContr&)owner()).cntrAccess(), cfg("PORT").getI(), cfg("CNL").getI(), &vl_bit)) != DE_NONE )
	{
	    dscGetLastError(&errparams);
            Mess->put(nodePath().c_str(),TMess::Error,"%s (%s)",dscGetErrorString(result), errparams.errstring);
	}
	else val.setB(vl_bit,NULL,true);
    }
}	
