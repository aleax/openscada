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

#include <unistd.h>
#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "tbds.h"

//================================================================
//=========== TBDS ===============================================
//================================================================

const char *TBDS::o_name = "TBDS";

TBDS::TBDS( TKernel *app ) : TGRPModule(app,"BD") 
{
    s_name = "BD";
}

TBDS::~TBDS(  )
{

}

AutoHD<TTable> TBDS::open( const SBDS &bd_t, bool create )
{
    try{
    AutoHD<TTipBD> tpbd = gmdAt(bd_t.tp);    
    if( !tpbd.at().openStat(bd_t.bd) ) tpbd.at().open(bd_t.bd,create);
    if( !tpbd.at().at(bd_t.bd).at().openStat(bd_t.tbl) )
	tpbd.at().at(bd_t.bd).at().open(bd_t.tbl,create);
    return tpbd.at().at(bd_t.bd).at().at(bd_t.tbl);
    }catch(TError err) 
    { 
	Mess->put_s("SYS",MESS_ERR,err.what()); 
	throw;
    }
}

void TBDS::close( const SBDS &bd_t )
{
    try
    {
	AutoHD<TTipBD> tpbd = gmdAt(bd_t.tp);
	if( tpbd.at().at(bd_t.bd).at().openStat(bd_t.tbl) )
	    tpbd.at().at(bd_t.bd).at().close(bd_t.tbl);
	if( tpbd.at().openStat(bd_t.bd) ) tpbd.at().close(bd_t.bd);
    }catch(TError err) { Mess->put_s("SYS",MESS_ERR,err.what()); }
}

string TBDS::opt_descr(  )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
    	"=========================== The BD subsystem options ======================\n"
	"    --BDMPath=<path>    Set moduls <path>;\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"mod_path    <path>      set path to modules;\n\n"),gmdName().c_str());

    return(buf);
}


void TBDS::gmdCheckCommandLine( )
{
    TGRPModule::gmdCheckCommandLine( );
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{"BDMPath" ,1,NULL,'m'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'm': DirPath  = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}

void TBDS::gmdUpdateOpt()
{
    TGRPModule::gmdUpdateOpt();
    
    try{ DirPath = gmdXMLCfgNode()->get_child("id","mod_path")->get_text(); }
    catch(...) {  }
}

//================== Controll functions ========================
void TBDS::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    
    TGRPModule::ctr_fill_info( inf );
    
    char *i_help = 
	"<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";
    
    XMLNode *n_add = inf->get_child("id","help")->add_child();
    n_add->load_xml(i_help);
    n_add->set_attr(dscr,Mess->I18N("Options help"));
}

void TBDS::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/help/g_help" ) ctr_opt_setS( opt, opt_descr() );       
    else TGRPModule::ctr_din_get_( a_path, opt );
}

//================================================================
//=========== TTipBD =============================================
//================================================================

const char *TTipBD::o_name = "TTipBD";

TTipBD::TTipBD(  ) : m_hd_bd(o_name)
{ 

};

TTipBD::~TTipBD( )
{
    m_hd_bd.lock();
    SYS->event_wait( m_hd_bd.obj_free(), true, string(o_name)+": BDs is closing....");
}

void TTipBD::open( const string &name, bool create )
{
    if( m_hd_bd.obj_avoid(name) ) return;
    
    TBD *t_bd = BDOpen(name,create);
    try { m_hd_bd.obj_add( t_bd, &t_bd->name() ); }
    catch(TError err) {	delete t_bd; }
}

//================================================================
//=========== TBD ================================================
//================================================================

const char *TBD::o_name = "TBD";


TBD::TBD( const string &name ) : m_name(name), m_hd_tb(o_name) 
{    

}

TBD::~TBD()
{
    m_hd_tb.lock();
    SYS->event_wait( m_hd_tb.obj_free(), true, string(o_name)+": Tables is closing....");
}

void TBD::open( const string &table, bool create )
{
    if( m_hd_tb.obj_avoid(table) ) return;
    
    TTable *tbl = TableOpen(table, create);
    try { m_hd_tb.obj_add( tbl, &tbl->name() ); }
    catch(TError err) {	delete tbl; }
}

//================================================================
//=========== TTable =============================================
//================================================================
const char *TTable::o_name = "TTable";
char *TTable::_err   = "(%s) function %s no support!";

TTable::TTable( const string &name, TBD *owner ) :  m_name(name), m_owner(owner)
{

};

TTable::~TTable()
{ 

};  
    

