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
TBDS::TBDS( TKernel *app ) : TGRPModule(app,"BD") 
{
    s_name = "BD";
}

TBDS::~TBDS(  )
{

}

AutoHD<TTable> TBDS::open( const TBDS::SName &bd_t, bool create )
{
    AutoHD<TTable> tbl;
    try
    {
    	AutoHD<TTipBD> tpbd = gmdAt(bd_t.tp);    
    	if( !tpbd.at().openStat(bd_t.bd) ) tpbd.at().open(bd_t.bd,create);
    	if( !tpbd.at().at(bd_t.bd).at().openStat(bd_t.tbl) )
	    tpbd.at().at(bd_t.bd).at().open(bd_t.tbl,create);
	tbl = tpbd.at().at(bd_t.bd).at().at(bd_t.tbl);
    }
    catch(TError err)
    { Mess->put_s("SYS",MESS_ERR,err.what()); }

    return tbl;
}

void TBDS::close( const TBDS::SName &bd_t )
{
    try
    {
	AutoHD<TTipBD> tpbd = gmdAt(bd_t.tp);
	if( tpbd.at().at(bd_t.bd).at().openStat(bd_t.tbl) && tpbd.at().at(bd_t.bd).at().at(bd_t.tbl).at().use()==1 )
	    tpbd.at().at(bd_t.bd).at().close(bd_t.tbl);
	if( tpbd.at().openStat(bd_t.bd) && tpbd.at().at(bd_t.bd).at().use()==1 )
	    tpbd.at().close(bd_t.bd);
    }catch(TError err) { Mess->put_s("SYS",MESS_ERR,err.what()); }
}

bool TBDS::dataSeek( AutoHD<TTable> &tbl, const string &path, int lev, TConfig &cfg )
{
    int c_lev = 0;
    XMLNode *nd;

    if( !tbl.freeStat() )	
	return tbl.at().fieldSeek(lev,cfg);	
    
    //Load from Config file if tbl no avoid    
    try{ nd = ctrId(SYS->cfgNode(),path); }
    catch(...){ return false; }
    
    //Scan fields and fill Config
    for( int i_fld = 0; i_fld < nd->childSize(); i_fld++ )
    {
	XMLNode *el = nd->childGet(i_fld);
	if( el->name() == "fld" && lev == c_lev++ )
	{
	    vector<string> cf_el;
	    cfg.cfgList(cf_el);
	    
	    for( int i_el = 0; i_el < cf_el.size(); i_el++ )
	    {
		string v_el = el->attr(cf_el[i_el]);		
		TCfg &u_cfg = cfg.cfg(cf_el[i_el]);
		
		if( u_cfg.fld().type()&T_STRING )	u_cfg.setS(v_el);
	        else if( u_cfg.fld().type()&(T_DEC|T_OCT|T_HEX) )       u_cfg.setI(atoi(v_el.c_str()));
                else if( u_cfg.fld().type()&T_REAL )    u_cfg.setR(atof(v_el.c_str()));
                else if( u_cfg.fld().type()&T_BOOL )    u_cfg.setB(atoi(v_el.c_str()));
	    }
	    
	    return true;
	}
    }
    
    return false;
}

string TBDS::optDescr(  )
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case 'm': DirPath  = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TBDS::gmdUpdateOpt()
{
    TGRPModule::gmdUpdateOpt();
    
    try{ DirPath = gmdCfgNode()->childGet("id","mod_path")->text(); }
    catch(...) {  }
}

//================== Controll functions ========================
void TBDS::ctrStat_( XMLNode *inf )
{
    char *dscr = "dscr";
    
    TGRPModule::ctrStat_( inf );
    
    char *i_help = 
	"<fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>";
    
    XMLNode *n_add = inf->childGet("id","help")->childAdd();
    n_add->load(i_help);
    n_add->attr(dscr,Mess->I18N("Options help"));
}

void TBDS::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/help/g_help" ) ctrSetS( opt, optDescr() );       
    else TGRPModule::ctrDinGet_( a_path, opt );
}

//================================================================
//=========== TTipBD =============================================
//================================================================
TTipBD::TTipBD(  )
{ 
    m_db = grpAdd();
};

TTipBD::~TTipBD( )
{
}

void TTipBD::open( const string &name, bool create )
{
    if( chldAvoid(m_db,name) ) return;
    chldAdd(m_db,openBD(name,create)); 
}

//================================================================
//=========== TBD ================================================
//================================================================
TBD::TBD( const string &name ) : m_name(name)
{    
    m_tbl = grpAdd();
}

TBD::~TBD()
{
}

void TBD::open( const string &table, bool create )
{
    if( chldAvoid(m_tbl,table) ) return;
    chldAdd(m_tbl,openTable(table, create)); 
}

//================================================================
//=========== TTable =============================================
//================================================================
char *TTable::_err   = "(%s) function %s no support!";

TTable::TTable( const string &name, TBD *owner ) :  m_name(name), m_owner(owner)
{

};

TTable::~TTable()
{ 

};  
    

