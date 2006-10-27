
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

void TSpecialS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TSubSYS::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",Mess->I18N("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","10");
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440,"root","root") )	opt->text(optDescr());
    else TSubSYS::cntrCmdProc(opt);
}
		    
//================================================================
//=========== TSpecial ===========================================
//================================================================
TSpecial::TSpecial() : run_st(false)
{

}

void TSpecial::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	TModule::cntrCmdProc(opt);
	ctrMkNode("area",opt,0,"/prm",Mess->I18N("Special"));
	ctrMkNode("area",opt,-1,"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,-1,"/prm/st/st",Mess->I18N("Runing"),0664,"root","root",1,"tp","bool");
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	atoi(opt->text().c_str())?modStart():modStop();
    }
    else TModule::cntrCmdProc(opt);
}
