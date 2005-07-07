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

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tuis.h"

//================================================================
//================== TUIS ========================================
//================================================================
const char *TUIS::o_name = "TUIS";
	
TUIS::TUIS( TKernel *app ) : TGRPModule(app,"UI","User interfaces")
{

}

string TUIS::optDescr( )
{
    return(Mess->I18N(
    	"===================== The user interface subsystem options ================\n\n"));
}

void TUIS::gmdLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"      ,0,NULL,'h'},
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
    //========== Load parameters from config file =============

    //Load modules
    TGRPModule::gmdLoad();
}

//=========== Control ==========================================
void TUIS::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TGRPModule::cntrCmd_( a_path, opt, cmd );       //Call parent

	ctrMkNode("fld",opt,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,"str")->
	    attr_("cols","90")->attr_("rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
    	if( a_path == "/help/g_help" ) ctrSetS( opt, optDescr() );
	else TGRPModule::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
	TGRPModule::cntrCmd_( a_path, opt, cmd );
}


//================================================================
//================== TUI =========================================
//================================================================
const char *TUI::o_name = "TUI";

TUI::TUI() : run_st(false)
{

}
    
//================== Controll functions ========================
void TUI::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TModule::cntrCmd_( a_path, opt, cmd );       //Call parent
	
	ctrInsNode("area",0,opt,a_path.c_str(),"/prm",Mess->I18N("User interface"));
	ctrMkNode("area",opt,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/r_st",Mess->I18N("Runing"),0664,0,0,"bool");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/st/r_st" )	ctrSetB( opt, run_st );
	else TModule::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/r_st" )	ctrGetB( opt )?modStart():modStop();
	else TModule::cntrCmd_( a_path, opt, cmd );
    }
}

