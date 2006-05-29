
//OpenSCADA system file: tuis.cpp
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>

#include "tsys.h"
#include "tmess.h"
#include "tuis.h"

//================================================================
//================== TUIS ========================================
//================================================================
TUIS::TUIS( ) : TSubSYS("UI","User interfaces",true)
{

}

string TUIS::optDescr( )
{
    return(Mess->I18N(
    	"===================== Subsystem \"User interfaces\" options ===============\n\n"));
}

void TUIS::subLoad( )
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
    TSubSYS::subLoad();
}

bool TUIS::presentIco(const string &inm, string *tp)
{
    int hd = open((string("./icons/")+inm+".png").c_str(),O_RDONLY);
    if( hd != -1 )
    {
	if( tp ) *tp = "png";
	close(hd);
	return true;
    }
    return false;    
}

string TUIS::getIco(const string &inm, string *tp )
{
    int len;
    char buf[STR_BUF_LEN];
    string rez;
    
    int hd = open((string("./icons/")+inm+".png").c_str(),O_RDONLY);
    if( hd != -1 )
    {
        if( tp ) *tp = "png";
        while( len = read(hd,buf,sizeof(buf)) )
    	    rez.append(buf,len);
        close(hd);
    }
    
    return rez;
}

void TUIS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TSubSYS::cntrCmd_( a_path, opt, cmd );       //Call parent

	ctrMkNode("fld",opt,-1,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,3,"tp","str","cols","90","rows","5");
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
//================== TUI =========================================
//================================================================
TUI::TUI() : run_st(false)
{

}
    
void TUI::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TModule::cntrCmd_( a_path, opt, cmd );       //Call parent
	
	ctrMkNode("area",opt,0,a_path.c_str(),"/prm",Mess->I18N("User interface"));
	ctrMkNode("area",opt,-1,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,-1,a_path.c_str(),"/prm/st/r_st",Mess->I18N("Runing"),0664,0,0,1,"tp","bool");
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

