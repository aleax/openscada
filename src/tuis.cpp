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

string TUIS::opt_descr( )
{
    return(Mess->I18N(
    	"===================== The user interface subsystem options ================\n"
	"    --GUIModPath=<path>  Set moduls <path>;\n\n"));
}

void TUIS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );

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
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}

void TUIS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();

}

void TUIS::gmd_Start( )
{
    vector<string> list;
    gmd_list(list);
    for(unsigned i_sp = 0; i_sp < list.size(); i_sp++)
	((TUI &)gmd_at(list[i_sp]).at()).start();
}

void TUIS::gmd_Stop( )
{
    vector<string> list;
    gmd_list(list);
    for(unsigned i_sp = 0; i_sp < list.size(); i_sp++)
	((TUI &)gmd_at(list[i_sp]).at()).stop();
}

//=========== Control ==========================================
void TUIS::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
	"<area id='a_gn' acs='0440'>"
	" <fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
	"</area>";
    char *dscr = "dscr";
    
    TGRPModule::ctr_fill_info( inf );
    
    XMLNode *n_add = inf->add_child();
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Subsystem control"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Options help"));
}

void TUIS::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_gn/g_help" ) ctr_opt_setS( opt, opt_descr() );       
    else TGRPModule::ctr_din_get_( a_path, opt );
}
//================================================================
//================== TUI =========================================
//================================================================
const char *TUI::o_name = "TUI";

TUI::TUI() : run_st(false)
{

}
    
//================== Controll functions ========================
void TUI::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
	"<area id='a_prm'>"
	" <fld id='r_st' acs='0664' tp='bool'/>"
	"</area>";
    char *dscr = "dscr";
    
    TModule::ctr_fill_info( inf );
    
    XMLNode *n_add = inf->add_child();
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("User interface control"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Runing"));
}

void TUI::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_prm/r_st" )  ctr_opt_setB( opt, run_st );
    else TModule::ctr_din_get_( a_path, opt );
}

void TUI::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_prm/r_st" ) if( ctr_opt_getB( opt ) ) start(); else stop();
    else TModule::ctr_din_set_( a_path, opt );
}

