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

string TSpecialS::opt_descr( )
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
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
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
void TSpecialS::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    
    TGRPModule::ctr_fill_info( inf );
	
    char *i_help =
        "<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";
		    
    XMLNode *n_add = inf->get_child("id","help")->add_child();
    n_add->load_xml(i_help);
    n_add->set_attr(dscr,Mess->I18N("Options help"));
}

void TSpecialS::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/help/g_help" ) ctr_opt_setS( opt, opt_descr() );       
    else TGRPModule::ctr_din_get_( a_path, opt );
}
//================================================================
//=========== TSpecial ===========================================
//================================================================
const char *TSpecial::o_name = "TSpecial";

TSpecial::TSpecial() : run_st(false)
{

}

//================== Controll functions ========================
void TSpecial::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
	"<area id='prm'>"
	" <fld id='st' acs='0664' tp='bool'/>"
	"</area>";
    char *dscr = "dscr";
    
    TModule::ctr_fill_info( inf );
    
    XMLNode *n_add = inf->ins_child(0);
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Special"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Run"));
}

void TSpecial::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/st" )	ctr_opt_setB( opt, run_st );
    else TModule::ctr_din_get_( a_path, opt );
}

void TSpecial::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/prm/st" )	if( ctr_opt_getB( opt ) ) start(); else stop();
    else TModule::ctr_din_set_( a_path, opt );
}

