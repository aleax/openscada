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
const char *TBDS::i_cntr = 
    "<area id='a_bd' acs='0440'>"
    " <fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
    "</area>";

TBDS::TBDS( TKernel *app ) : TGRPModule(app,"BaseDate") 
{
    s_name = "Data bases";
}

TBDS::~TBDS(  )
{

}

void TBDS::list( vector<SBDS> &list )
{
    list.clear();
    vector<string> m_list;
    gmd_list(m_list);
    for( unsigned i_m = 0; i_m < m_list.size(); i_m++ )
    {
	unsigned m_hd = gmd_att( m_list[i_m] );
	vector<string> b_list;
	gmd_at(m_hd).list(b_list);
	for( unsigned i_b = 0; i_b < b_list.size(); i_b++ )
	{
    	    unsigned b_hd = gmd_at(m_hd).open( b_list[i_b], false );
    	    vector<string> t_list;
    	    gmd_at(m_hd).at(b_hd).list(t_list);
    	    for( unsigned i_t = 0; i_t < t_list.size(); i_t++ )
    		list.push_back( SBDS( m_list[i_m], b_list[i_b], t_list[i_t]) );
    	    gmd_at(m_hd).close( b_hd );
	}
    	gmd_det( m_hd );
    }
}

SHDBD TBDS::open( SBDS bd_t, bool create )
{
    SHDBD HDBD;
    HDBD.h_tp = gmd_att( bd_t.tp );
    try{ HDBD.h_bd = gmd_at(HDBD.h_tp).open( bd_t.bd, create ); }
    catch(...) { gmd_det( HDBD.h_tp ); throw; }
    try{ HDBD.h_tbl = gmd_at(HDBD.h_tp).at(HDBD.h_bd).open( bd_t.tbl, create ); }
    catch(...) { gmd_at(HDBD.h_tp).close(HDBD.h_bd); gmd_det( HDBD.h_tp ); throw; }
    
    return( HDBD );
}

void TBDS::close( SHDBD &hd )
{
    gmd_at(hd.h_tp).at(hd.h_bd).close(hd.h_tbl);
    gmd_at(hd.h_tp).close(hd.h_bd);
    gmd_det(hd.h_tp);
}

void TBDS::del( SBDS bd_t )
{
    SHDBD HDBD;
    HDBD.h_tp = gmd_att( bd_t.tp );
    try{ HDBD.h_bd = gmd_at(HDBD.h_tp).open( bd_t.bd, false ); }
    catch(...) { gmd_det( HDBD.h_tp ); throw; }
    try{ gmd_at(HDBD.h_tp).at(HDBD.h_bd).del( bd_t.tbl ); }
    catch(...) { gmd_at(HDBD.h_tp).close(HDBD.h_bd); gmd_det( HDBD.h_tp ); throw; }
    gmd_at(HDBD.h_tp).close(HDBD.h_bd); 
    gmd_det( HDBD.h_tp );   
}

string TBDS::opt_descr(  )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
    	"=========================== The BD subsystem options ======================\n"
	"    --BDMPath=<path>    Set moduls <path>;\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"mod_path    <path>      set path to modules;\n"),gmd_Name().c_str());

    return(buf);
}


void TBDS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );
    
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
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

void TBDS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();
    
    try{ DirPath = gmd_XMLCfgNode()->get_child("id","mod_path")->get_text(); }
    catch(...) {  }
}

//================== Controll functions ========================
void TBDS::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    TGRPModule::ctr_fill_info( inf );
    
    XMLNode *n_add = inf->add_child();
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Subsystem control"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Options help"));
}

void TBDS::ctr_din_get_( string a_path, XMLNode *opt )
{
    TGRPModule::ctr_din_get_( a_path, opt );
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_bd" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "g_help" ) ctr_opt_setS( opt, opt_descr() );       
    }
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

unsigned TTipBD::open( string name, bool create )
{
    TBD *t_bd = BDOpen(name,create);
    try { m_hd_bd.obj_add( t_bd, &t_bd->Name() ); }
    catch(TError err) {	delete t_bd; }
    int hd = m_hd_bd.hd_att( name );    
    return( hd );
}

void TTipBD::close( unsigned hd )
{
    string name = at(hd).Name();
    m_hd_bd.hd_det( hd );
    if( !m_hd_bd.obj_use( name ) )
    	delete (TBD *)m_hd_bd.obj_del( name );
}

//================================================================
//=========== TBD ================================================
//================================================================

const char *TBD::o_name = "TBD";


TBD::TBD( string &name ) : m_name(name), m_hd_tb(o_name) 
{    

}

TBD::~TBD()
{
    m_hd_tb.lock();
    SYS->event_wait( m_hd_tb.obj_free(), true, string(o_name)+": Tables is closing....");
}

unsigned TBD::open( string table, bool create )
{
    TTable *tbl = TableOpen(table, create);
    try { m_hd_tb.obj_add( tbl, &tbl->Name() ); }
    catch(TError err) {	delete tbl; }
    return( m_hd_tb.hd_att( tbl->Name() ) );
}

void TBD::close( unsigned hd )
{
    string name = at(hd).Name();
    m_hd_tb.hd_det( hd );
    if( !m_hd_tb.obj_use( name ) )
    	delete (TTable *)m_hd_tb.obj_del( name );
}

//================================================================
//=========== TTable =============================================
//================================================================
const char *TTable::o_name = "TTable";
char *TTable::_err   = "(%s) function %s no support!";

TTable::TTable( string &name ) :  m_name(name)
{

};

TTable::~TTable()
{ 

};  
    

