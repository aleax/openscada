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
	
TUIS::TUIS( TKernel *app ) : TGRPModule(app,"UI") 
{
    s_name = "User interfaces"; 
}

string TUIS::optDescr( )
{
    return(Mess->I18N(
    	"===================== The user interface subsystem options ================\n"
	"    --GUIModPath=<path>  Set moduls <path>;\n\n"));
}

void TUIS::gmdCheckCommandLine( )
{
    TGRPModule::gmdCheckCommandLine( );

    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"      ,0,NULL,'h'},
	{"GUIModPath",1,NULL,'m'},
	{NULL        ,0,NULL,0  }
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

void TUIS::gmdUpdateOpt()
{
    TGRPModule::gmdUpdateOpt();

}

void TUIS::gmdStart( )
{
    vector<string> list;
    gmdList(list);
    for(unsigned i_sp = 0; i_sp < list.size(); i_sp++)
	((TUI &)gmdAt(list[i_sp]).at()).start();
}

void TUIS::gmdStop( )
{
    vector<string> list;
    gmdList(list);
    for(unsigned i_sp = 0; i_sp < list.size(); i_sp++)
	((TUI &)gmdAt(list[i_sp]).at()).stop();
}

//=========== Control ==========================================
void TUIS::ctrStat_( XMLNode *inf )
{
    char *dscr = "dscr";
    TGRPModule::ctrStat_( inf );
    
    //Insert to Help
    char *i_help = "<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";
    
    XMLNode *n_add = inf->childGet("id","help")->childAdd();    
    n_add->load(i_help);
    n_add->attr(dscr,Mess->I18N("Options help"));
}

void TUIS::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/help/g_help" ) ctrSetS( opt, optDescr() );       
    else TGRPModule::ctrDinGet_( a_path, opt );
}
//================================================================
//================== TUI =========================================
//================================================================
const char *TUI::o_name = "TUI";

TUI::TUI() : run_st(false)
{

}
    
//================== Controll functions ========================
void TUI::ctrStat_( XMLNode *inf )
{
    char *dscr = "dscr";
    
    TModule::ctrStat_( inf );
    
    char *i_cntr = 
	"<area id='a_prm'>"
	" <area id='st'>"
	"  <fld id='r_st' acs='0664' tp='bool'/>"
	" </area>"
	"</area>";
    
    XMLNode *n_add = inf->childIns(0);
    n_add->load(i_cntr);
    n_add->attr(dscr,Mess->I18N("User interface"));
    n_add->childGet(0)->attr(dscr,Mess->I18N("State"));
    n_add->childGet(0)->childGet(0)->attr(dscr,Mess->I18N("Runing"));
}

void TUI::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_prm/st/r_st" ) ctrSetB( opt, run_st );
    else TModule::ctrDinGet_( a_path, opt );
}

void TUI::ctrDinSet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_prm/st/r_st" ) if( ctrGetB( opt ) ) start(); else stop();
    else TModule::ctrDinSet_( a_path, opt );
}

