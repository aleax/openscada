
//OpenSCADA system module DAQ.DiamondBoards file: diamond.cpp
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
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

#include <resalloc.h>
#include <tsys.h>
#include <ttiparam.h>

#include "diamond.h"

//============ Modul info! =====================================================
#define MOD_ID      "DiamondBoards"
#define MOD_NAME    "Diamond DA boards"
#define MOD_TYPE    "DAQ"
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

}

void TTpContr::postEnable()
{    
    TModule::postEnable();
    
    //==== Controler's bd structure ====
    fldAdd( new TFld("BOARD",I18N("Diamond system board"),TFld::Dec,FLD_SELECT,"1","0","0","ATHENA") );
    fldAdd( new TFld("PRM_BD_A",I18N("Analog parameters' table"),TFld::String,0,"30","diamond_prm_a") );
    fldAdd( new TFld("PRM_BD_D",I18N("Digital parameters' table"),TFld::String,0,"30","diamond_prm_d") );    
    fldAdd( new TFld("ADDR",I18N("Base board address"),TFld::Hex,0,"3","768") );
    //fldAdd( new TFld("INT",I18N("Interrupt vector"),TFld::Dec,0,"2","5") );
    //fldAdd( new TFld("DMA",I18N("DMA number"),TFld::Dec,0,"1","1") );
    fldAdd( new TFld("DIO_CFG",I18N("Digit IO config byte"),TFld::Hex,0,"2","0") );
    
    //==== Parameter type bd structure ====
    //---- Analog ----
    int t_prm = tpParmAdd("a_prm","PRM_BD_A",I18N("Analog parameter"));
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",I18N("Analog parameter type"),TFld::Dec,FLD_SELECT|FLD_NOVAL|FLD_PREV,"1","0","0;1",I18N("Input;Output")) );
    tpPrmAt(t_prm).fldAdd( new TFld("CNL",I18N("Channel"),TFld::Dec,FLD_NOVAL,"2","0") );
    tpPrmAt(t_prm).fldAdd( new TFld("GAIN",I18N("A/D converter gain"),TFld::Dec,FLD_SELECT|FLD_NOVAL|FLD_PREV,"1","0","0;1;2;3","x1;x2;x4;x8") );
    //---- Digit ----
    t_prm = tpParmAdd("d_prm","PRM_BD_D",I18N("Digital parameter"));
    tpPrmAt(t_prm).fldAdd( new TFld("TYPE",I18N("Digital parameter type"),TFld::Dec,FLD_SELECT|FLD_NOVAL|FLD_PREV,"1","0","0;1",I18N("Input;Output")) );
    tpPrmAt(t_prm).fldAdd( new TFld("PORT",I18N("Port"),TFld::Dec,FLD_SELECT|FLD_NOVAL|FLD_PREV,"2","0","0;1;2","A;B;C") );
    tpPrmAt(t_prm).fldAdd( new TFld("CNL",I18N("Channel"),TFld::Dec,FLD_NOVAL|FLD_PREV,"1") );

    //==== Init value elements =====
    //---- Analog input ----
    elem_ai.fldAdd( new TFld("value",I18N("Value %"),TFld::Real,FLD_NWR|FLD_DRD,"",TSYS::real2str(EVAL_REAL).c_str(),"0;100","",1) );
    elem_ai.fldAdd( new TFld("voltage",I18N("Voltage V"),TFld::Real,FLD_NWR|FLD_DRD,"",TSYS::real2str(EVAL_REAL).c_str(),"-10;10","",2) );
    elem_ai.fldAdd( new TFld("code",I18N("A/D code"),TFld::Dec,FLD_NWR|FLD_DRD,"",TSYS::int2str(EVAL_INT).c_str(),"","",3) );
    //---- Analog output ----
    elem_ao.fldAdd( new TFld("value",I18N("Value %"),TFld::Real,FLD_DWR,"",TSYS::real2str(EVAL_REAL).c_str(),"0;100","",1) );
    elem_ao.fldAdd( new TFld("voltage",I18N("Voltage V"),TFld::Real,FLD_DWR,"",TSYS::real2str(EVAL_REAL).c_str(),"-10;10","",2) );
    //---- Digit input ----
    elem_di.fldAdd( new TFld("value",I18N("Value"),TFld::Bool,FLD_NWR|FLD_DRD,"",TSYS::int2str(EVAL_BOOL).c_str(),"","",1) );
    //---- Digit output ----
    elem_do.fldAdd( new TFld("value",I18N("Value"),TFld::Bool,FLD_DWR,"",TSYS::int2str(EVAL_BOOL).c_str(),"","",1) );

    m_init = true;
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

//======================================================================
//==== TMdContr 
//======================================================================
TMdContr::TMdContr( string name_c, const string &daq_db, ::TElem *cfgelem) :
	::TController(name_c,daq_db,cfgelem), m_addr(cfg("ADDR").getId())
{
    port_res = ResAlloc::resCreate();
    ai_res   = ResAlloc::resCreate();
    ao_res   = ResAlloc::resCreate();
    dio_res  = ResAlloc::resCreate();
    
    cfg("PRM_BD_A").setS("Diam_"+name_c+"_prm_a");
    cfg("PRM_BD_D").setS("Diam_"+name_c+"_prm_d");
}

TMdContr::~TMdContr()
{
    ResAlloc::resDelete(port_res);
    ResAlloc::resDelete(ai_res);
    ResAlloc::resDelete(ao_res);
    ResAlloc::resDelete(dio_res);
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{    
    return new TMdPrm(name,&owner().tpPrmAt(type));
}

void TMdContr::load( )
{
    TController::load( );
}

void TMdContr::save( )
{
    TController::save();
}

void TMdContr::start( )
{
    TController::start();
    
    //Check inited of Diamond API    
    if( !mod->initStat() )
        throw TError(mod->nodePath().c_str(),"Module no inited!");
    
    if( !run_st )
    {
	port_hd = open("/dev/port",O_RDWR);
	if(port_hd < 0)
	    throw TError(nodePath().c_str(),"Error open ports file /dev/port.");
	
	//Load DIO config
	loadDIOCfg();
			    
	run_st = true;
	
	//Enable parameters
	vector<string> prm_list;
	list(prm_list);
	for( int i_prm = 0; i_prm < prm_list.size(); i_prm++ )
	    if( at(prm_list[i_prm]).at().toEnable() )
		at(prm_list[i_prm]).at().enable();
    }
}

void TMdContr::stop( )
{  
    TController::stop();
    
    if( !run_st ) return;
    
    //Disable parameters
    vector<string> prm_list;
    list(prm_list);
    for( int i_prm = 0; i_prm < prm_list.size(); i_prm++ )
        if( at(prm_list[i_prm]).at().enableStat() )
    	    at(prm_list[i_prm]).at().disable();							
    
    close(port_hd);
    run_st = false;
} 

void TMdContr::loadDIOCfg()
{
    ResAlloc::resRequestW(DIORes());
    outp(11,0x80|cfg("DIO_CFG").getI());
    ResAlloc::resReleaseW(DIORes());
}

unsigned char TMdContr::inp(int off)
{
    unsigned char rez;
    
    ResAlloc::resRequestW(port_res);    
    lseek(port_hd,m_addr+off,SEEK_SET);
    read(port_hd,&rez,1);    
    ResAlloc::resReleaseW(port_res);
    
    return rez;
}

void TMdContr::outp(int off, unsigned char vl)
{
    ResAlloc::resRequestW(port_res);
    lseek(port_hd,m_addr+off,SEEK_SET);
    write(port_hd,&vl,1);
    ResAlloc::resReleaseW(port_res);
}

void TMdContr::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
        TController::cntrCmd_( a_path, opt, cmd );
	
        ctrMkNode("area",opt,-1,a_path.c_str(),"/board",owner().I18N("Board config"));
        ctrMkNode("area",opt,-1,a_path.c_str(),"/board/dio",owner().I18N("Digital IO ports. Select input!"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/board/dio/a",owner().I18N("Port A"),0664,0,0,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/board/dio/b",owner().I18N("Port B"),0664,0,0,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/board/dio/c1",owner().I18N("Port C1"),0664,0,0,1,"tp","bool");
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/board/dio/c2",owner().I18N("Port C2"),0664,0,0,1,"tp","bool");
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
	    if( run_st ) loadDIOCfg();
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
    TParamContr::preDisable(flag); 
    type(NONE);
}

void TMdPrm::type( TMdPrm::Type vtp )
{
    //Free previos type
    switch(m_tp)
    {
	case AI: vlDetElem( &mod->elemAI() ); break;
	case AO: vlDetElem( &mod->elemAO() ); break;
	case DI: vlDetElem( &mod->elemDI() ); break;
	case DO: vlDetElem( &mod->elemDO() ); break;
    }
    
    //Init new type
    switch(vtp)
    {
	case AI:    
	    cfg("GAIN").view(true);
	    m_gain = cfg("GAIN").getI();
	    vlAttElem( &mod->elemAI() );
	    break;
	case AO:    
	    cfg("GAIN").view(false);    
	    vlAttElem( &mod->elemAO() );
	    break;
	case DI:
	    m_dio_port = (cfg("PORT").getI()<<8)+cfg("CNL").getI();
	    vlAttElem( &mod->elemDI() );
	    break;
	case DO:    
    	    vlAttElem( &mod->elemDO() );
	    break;
    }    
    m_tp = vtp;
}

bool TMdPrm::cfgChange( TCfg &i_cfg )
{
    //Change TYPE parameter
    if( i_cfg.name() == "TYPE" )
    {
	if( i_cfg.getI() == 0 && type() == AO )		type(AI);
	else if( i_cfg.getI() == 0 && type() == DO ) 	type(DI);
	else if( i_cfg.getI() == 1 && type() == AI ) 	type(AO);
        else if( i_cfg.getI() == 1 && type() == DI ) 	type(DO);
	else return false;
    }
    switch(type())    
    {
	case AI:    
	    if( i_cfg.name() == "GAIN" ) m_gain = i_cfg.getI();
	    else return false;
	case DI: case DO:
	    if( i_cfg.name() == "Port" ) 	m_dio_port = (i_cfg.getI()<<8)+cfg("CNL").getI();
	    else if( i_cfg.name() == "CNL" )	m_dio_port = (cfg("PORT").getI()<<8)+i_cfg.getI();
	    else return false;	        
    }
    
    return true;
}									

void TMdPrm::vlSet( TVal &val )
{    
    if( !enableStat() )	return;
    switch(type())
    {
	case AO:
	{
	    int code;
	    if(val.fld().workId()==1)		code = (int)(4095.*val.getR(0,true)/100.);
	    else if(val.fld().workId()==2)	code = (int)(4095.*val.getR(0,true)/10.);
	    
	    ResAlloc::resRequestW(owner().AORes());
	    while(owner().inp(3)&0x10);	//Wait
	    owner().outp(6,code&0xFF);
	    owner().outp(7,(code>>8)+(m_cnl<<6));
	    ResAlloc::resReleaseW(owner().AORes());
	    break;
	}
	case DO:
	{
	    bool code = val.getB(0,true);
	    ResAlloc::resRequestW(owner().DIORes());
	    char prev = owner().inp(8+m_dio_port/8);
	    owner().outp(8+m_dio_port/8,code?(prev|(0x01<<(m_dio_port%8))):(prev&(~(0x01<<(m_dio_port%8)))));
	    ResAlloc::resReleaseW(owner().DIORes());
	    break;
	}
    }
}

void TMdPrm::vlGet( TVal &val )
{
    if( val.fld().workId() == 0 )
    {
        if( !owner().startStat() )
            val.setS(mod->I18N("2:Controller stoped"),0,true);
        else if( !enableStat() )
            val.setS(mod->I18N("1:Parameter disabled"),0,true);
	else val.setS("0",0,true);
    }						

    switch(type())
    {
	case AI:
	{
	    short gval;
	    if( enableStat() )
	    {
		ResAlloc::resRequestW(owner().AIRes());
		//Select channel
		owner().outp(2,(m_cnl<<4)+m_cnl);
		//Select gain
		owner().outp(3,m_gain);
		while(owner().inp(3)&0x20);	//Wait to WAIT clear
		//Start A/D conversion
		owner().outp(0,0x80);
		while(owner().inp(3)&0x80);
		//Read data from board	
		gval = owner().inp(0);
		gval += owner().inp(1)<<8;
		ResAlloc::resReleaseW(owner().AIRes());
	    }
	    switch(val.fld().workId())
	    {
		case 1: val.setR(enableStat()?(100.*((double)gval/32768.)):EVAL_REAL,0,true); break;
		case 2: val.setR(enableStat()?(10.*((double)gval/32768.)):EVAL_REAL,0,true);  break;
		case 3: val.setI(enableStat()?gval:EVAL_INT,0,true);  break;
	    }
	    break;
	}
	case DI:
	{
	    char gval = EVAL_BOOL;
	    if( enableStat() )
            {
		ResAlloc::resRequestW(owner().DIORes());
		gval = (bool)(owner().inp(8+m_dio_port/8)&(0x01<<(m_dio_port%8)));
		ResAlloc::resReleaseW(owner().DIORes());
	    }
	    val.setB(gval,0,true);
	    break;
	}
    }
}	
