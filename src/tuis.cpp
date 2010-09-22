//OpenSCADA system file: tuis.cpp
/***************************************************************************
 *   Copyright (C) 2003-2009 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#include <gd.h>

#include "tsys.h"
#include "tmess.h"
#include "tuis.h"

using namespace OSCADA;

//*************************************************
//* TUIS                                          *
//*************************************************
TUIS::TUIS( ) : TSubSYS("UI","User interfaces",true)
{
    gdFTUseFontConfig(1);
}

string TUIS::optDescr( )
{
    return(_(
	"===================== Subsystem \"User interfaces\" options ===============\n\n"));
}

void TUIS::load_( )
{
    //- Load parameters from command line -
    int next_opt;
    const char *short_opt="h";
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
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //- Load parameters from config file -

}

void TUIS::subStart(  )
{
#if OSC_DEBUG >= 1
    mess_debug(nodePath().c_str(),_("Start subsystem."));
#endif

    TSubSYS::subStart( );
}

void TUIS::subStop( )
{
#if OSC_DEBUG >=1
    mess_debug(nodePath().c_str(),_("Stop subsystem."));
#endif

    TSubSYS::subStop( );
}

bool TUIS::icoPresent(const string &inm, string *tp)
{
    int hd = open(icoPath(inm).c_str(),O_RDONLY);
    if( hd != -1 )
    {
	if( tp ) *tp = "png";
	close(hd);
	return true;
    }
    return false;
}

string TUIS::icoGet(const string &inm, string *tp )
{
    int len, hd = -1, i_t;
    char buf[STR_BUF_LEN];
    string rez;
    char types[][5] = {"png","gif","jpg","jpeg"};

    for( i_t = 0; i_t < sizeof(types)/5; i_t++ )
    {
	hd = open(icoPath(inm,types[i_t]).c_str(),O_RDONLY);
	if( hd != -1 ) break;
    }
    if( hd != -1 )
    {
	if( tp ) *tp = types[i_t];
	while((len=read(hd,buf,sizeof(buf))) > 0) rez.append(buf,len);
	close(hd);
    }

    return rez;
}

string TUIS::icoPath( const string &ico, const string &tp )
{
    return SYS->icoDir()+"/"+ico+"."+tp;
}

void TUIS::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","10");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) ) opt->setText(optDescr());
    else TSubSYS::cntrCmdProc(opt);
}

//*************************************************
//* TUI                                           *
//*************************************************
TUI::TUI() : run_st(false)
{

}

void TUI::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TModule::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/prm",_("User interface")))
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
		ctrMkNode("fld",opt,-1,"/prm/st/r_st",_("Runing"),0664,"root","root",1,"tp","bool");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/st/r_st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEC_RD) )	opt->setText(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEC_WR) )	atoi(opt->text().c_str())?modStart():modStop();
    }
    else TModule::cntrCmdProc(opt);
}
