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

string TProtocolS::optDescr(  )
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
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
void TProtocolS::ctrStat_( XMLNode *inf )
{
    char *dscr = "dscr";
    
    TGRPModule::ctrStat_( inf );
    
    char *i_help = 
	"<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";
    
    XMLNode *n_add = inf->childGet("id","help")->childAdd();
    n_add->load(i_help);
    n_add->attr(dscr,Mess->I18N("Options help"));
}

void TProtocolS::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/help/g_help" ) ctrSetS( opt, optDescr() );       
    else TGRPModule::ctrDinGet_( a_path, opt );
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
    vector<string> list_el;
    list(list_el);
    for( unsigned i_ls = 0; i_ls < list_el.size(); i_ls++)
        close(list_el[i_ls]);
}

void TProtocol::open( const string &name )
{
    if( m_hd.objAvoid(name) ) return;
    TProtocolIn *t_prt = in_open(name);
    try { m_hd.objAdd( t_prt, &t_prt->name() ); }
    catch(TError err) { delete t_prt; throw; }
}

void TProtocol::close( const string &name )
{
    delete (TProtocolIn *)m_hd.objDel( name );
}

//================================================================
//=========== TProtocolIn ========================================
//================================================================
const char *TProtocolIn::o_name = "TProtocolIn";

TProtocolIn::TProtocolIn( const string &name, TProtocol *owner ) : m_name(name), m_owner(owner)
{

}

TProtocolIn::~TProtocolIn()
{

}

