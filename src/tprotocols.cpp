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
#include "tprotocols.h"

//================================================================
//=========== TProtocolS =========================================
//================================================================
const char *TProtocolS::o_name = "TProtocolS";

TProtocolS::TProtocolS( TKernel *app ) : TGRPModule(app,"Protocol") 
{
    s_name = "Protocols";
}

string TProtocolS::opt_descr(  )
{
    return(Mess->I18N(
    	"======================= The protocol subsystem options ====================\n"
	"    --PRCModPath=<path>  Set moduls <path>;\n\n"));
}

void TProtocolS::gmdCheckCommandLine( )
{
    TGRPModule::gmdCheckCommandLine( );
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"      ,0,NULL,'h'},
	{"PRCModPath",1,NULL,'m'},
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
}    

void TProtocolS::gmdUpdateOpt()
{
    TGRPModule::gmdUpdateOpt();
}

//=========== Control ==========================================
void TProtocolS::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    
    TGRPModule::ctr_fill_info( inf );
    
    char *i_help = 
	"<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";
    
    XMLNode *n_add = inf->get_child("id","help")->add_child();
    n_add->load_xml(i_help);
    n_add->set_attr(dscr,Mess->I18N("Options help"));
}

void TProtocolS::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/help/g_help" ) ctr_opt_setS( opt, opt_descr() );       
    else TGRPModule::ctr_din_get_( a_path, opt );
}

//================================================================
//=========== TProtocol ==========================================
//================================================================
const char *TProtocol::o_name = "TProtocol";

TProtocol::TProtocol() : m_hd(o_name)
{

}

TProtocol::~TProtocol()
{
    m_hd.lock();
    SYS->event_wait( m_hd.obj_free(), true, string(o_name)+": input protocols are closing...." );
}

unsigned TProtocol::open( const string &name )
{
    TProtocolIn *t_prt = in_open(name);
    try { m_hd.obj_add( t_prt, &t_prt->name() ); }
    catch(TError err) { delete t_prt; }
    return( m_hd.hd_att( t_prt->name() ) );
}

void TProtocol::close( unsigned hd )
{
    string name = at(hd).name();
    m_hd.hd_det( hd );
    if( !m_hd.obj_use( name ) )
	delete (TProtocolIn *)m_hd.obj_del( name );
}

//================================================================
//=========== TProtocolIn ========================================
//================================================================
const char *TProtocolIn::o_name = "TProtocolIn";

TProtocolIn::TProtocolIn( const string &name, TProtocol *owner ) : m_name(name), m_wait(false), m_owner(owner)
{

}

TProtocolIn::~TProtocolIn()
{

}

