
//OpenSCADA system file: tspecials.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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
#include "tmess.h"
#include "tspecials.h"

//================================================================
//=========== TSpecialS ==========================================
//================================================================
TSpecialS::TSpecialS( ) : TSubSYS("Special","Specials",true)
{

}

string TSpecialS::optDescr( )
{
    return(Mess->I18N(
    	"======================= Subsystem \"Special\" options =====================\n\n"));
}

void TSpecialS::subLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{NULL       ,0,NULL,0  }
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
    TSubSYS::subLoad();
}

//=========== Control ==========================================
void TSpecialS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TSubSYS::cntrCmd_( a_path, opt, cmd );       //Call parent

	ctrMkNode("fld",opt,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,"str")->
	    attr_("cols","90")->attr_("rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/help/g_help" ) ctrSetS( opt, optDescr() );
	else TSubSYS::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
	TSubSYS::cntrCmd_( a_path, opt, cmd );
}


//================================================================
//=========== TSpecial ===========================================
//================================================================
TSpecial::TSpecial() : run_st(false)
{

}

//================== Controll functions ========================
void TSpecial::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    switch(cmd)
    {
	case TCntrNode::Info:
	    TModule::cntrCmd_( a_path, opt, cmd );

	    ctrInsNode("area",0,opt,a_path.c_str(),"/prm",Mess->I18N("Special"));
	    ctrMkNode("area",opt,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	    ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/st",Mess->I18N("Runing"),0664,0,0,"bool");
	    break;
	case TCntrNode::Get:
	    if( a_path == "/prm/st/st" )	ctrSetB( opt, run_st );
	    else TModule::cntrCmd_( a_path, opt, cmd );
	    break;
	case TCntrNode::Set:
	    if( a_path == "/prm/st/st" )	ctrGetB( opt )?modStart():modStop();
	    else TModule::cntrCmd_( a_path, opt, cmd );
    }
}

