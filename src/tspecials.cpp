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
#include "tspecials.h"

//================================================================
//=========== TSpecialS ==========================================
//================================================================
const char *TSpecialS::o_name = "TSpecialS";

TSpecialS::TSpecialS( TKernel *app ) : TGRPModule(app,"Special")  
{
    s_name = "Specials";
}

string TSpecialS::optDescr( )
{
    return(Mess->I18N(
    	"======================= The special subsystem options =====================\n"
	"    --SPModPath=<path>  Set moduls <path>;\n\n"));
}

void TSpecialS::gmdCheckCommandLine( )
{
    TGRPModule::gmdCheckCommandLine( );
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{"SPModPath",1,NULL,'m'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TSpecialS::gmdUpdateOpt()
{
    TGRPModule::gmdUpdateOpt();
}

void TSpecialS::gmdStart( )
{
    vector<string> list;
    gmdList(list);
    for(unsigned i_sp = 0; i_sp < list.size(); i_sp++)
	((TSpecial &)gmdAt(list[i_sp]).at()).start();
}              

void TSpecialS::gmdStop( )
{
    vector<string> list;
    gmdList(list);
    for(unsigned i_sp = 0; i_sp < list.size(); i_sp++)
	((TSpecial &)gmdAt(list[i_sp]).at()).stop();
}

//=========== Control ==========================================
void TSpecialS::ctrStat_( XMLNode *inf )
{
    char *dscr = "dscr";
    
    TGRPModule::ctrStat_( inf );
	
    char *i_help =
        "<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";
		    
    XMLNode *n_add = inf->childGet("id","help")->childAdd();
    n_add->load(i_help);
    n_add->attr(dscr,Mess->I18N("Options help"));
}

void TSpecialS::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/help/g_help" ) ctrSetS( opt, optDescr() );       
    else TGRPModule::ctrDinGet_( a_path, opt );
}
//================================================================
//=========== TSpecial ===========================================
//================================================================
const char *TSpecial::o_name = "TSpecial";

TSpecial::TSpecial() : run_st(false)
{

}

//================== Controll functions ========================
void TSpecial::ctrStat_( XMLNode *inf )
{
    char *i_cntr = 
	"<area id='prm'>"
	 "<area id='st'>"
	  "<fld id='st' acs='0664' tp='bool'/>"
	 "</area>"
	"</area>";
    char *dscr = "dscr";
    
    TModule::ctrStat_( inf );
    
    XMLNode *n_add = inf->childIns(0);
    n_add->load(i_cntr);
    n_add->attr(dscr,Mess->I18N("Special"));
    n_add->childGet(0)->attr(dscr,Mess->I18N("State"));
    n_add->childGet(0)->childGet(0)->attr(dscr,Mess->I18N("Runing"));
}

void TSpecial::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/st/st" )	ctrSetB( opt, run_st );
    else TModule::ctrDinGet_( a_path, opt );
}

void TSpecial::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/st/st" )        if( ctrGetB( opt ) ) start(); else stop();
    else TModule::ctrDinSet_( a_path, opt );
}

