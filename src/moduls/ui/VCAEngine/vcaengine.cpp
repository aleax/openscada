
//OpenSCADA system module UI.VCSEngine file: vcaengine.cpp
/***************************************************************************
 *   Copyright (C) 2006 by Roman Savochenko
 *   rom_as@diyaorg.dp.ua                                                     
 *                                                                         
 *   This program is free software; you can redistribute it and/or modify  
 *   it under the terms of the GNU General Public License as published by  
 *   the Free Software Foundation; either version 2 of the License, or     
 *   (at your option) any later version.                                   
 *                                                                         
 *   This program is distributed in the hope that it will be useful,       
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *   GNU General Public License for more details.                          
 *                                                                         
 *   You should have received a copy of the GNU General Public License     
 *   along with this program; if not, write to the                         
 *   Free Software Foundation, Inc.,                                       
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             
 ***************************************************************************/

#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>       

#include <tsys.h>
#include <tmess.h>

#include "widget.h"
#include "libwidg.h"
#include "origwidg.h"
#include "vcaengine.h"

//============ Modul info! =====================================================
#define MOD_ID      "VCAEngine"
#define MOD_NAME    "Visual control area engine"
#define MOD_TYPE    "UI"
#define MOD_SUBTYPE "VCAEngine"
#define VER_TYPE    VER_UI
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Generic visual control area engine."
#define LICENSE     "GPL"
//============================================================================== 


VCA::Engine *VCA::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
    	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
    	    AtMod.t_ver = VER_TYPE;
	}
	else AtMod.id	= "";

	return AtMod;
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	VCA::Engine *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = VCA::mod = new VCA::Engine( source );

	return self_addr;
    }
}

using namespace VCA;

//==============================================================================
//==================== VCA::Engine =============================================
//==============================================================================

Engine::Engine( string name )
{
    mId		= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;
    
    id_wlb = grpAdd("wlb_");
}

Engine::~Engine()
{
    nodeDelAll();
}

void Engine::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string Engine::modInfo( const string &name )
{
    if( name == "SubType" ) return MOD_SUBTYPE;
    else return TModule::modInfo( name);
}

string Engine::optDescr( )
{
    char buf[STR_BUF_LEN];
    
    snprintf(buf,sizeof(buf),I18N(
        "======================= The module <%s:%s> options =======================\n"
        "---------- Parameters of the module section <%s> in config file ----------\n\n"),
    	    MOD_TYPE,MOD_ID,nodePath().c_str());
		
    return buf;
}

void Engine::modLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
        {"help"    ,0,NULL,'h'},
        {NULL      ,0,NULL,0  }
    };
	    
    optind=opterr=0;
    do
    {
        next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
        switch(next_opt)
        {
    	    case 'h': fprintf(stdout,optDescr().c_str()); break;
            case -1 : break;
        }
    } while(next_opt != -1);
    
    //========== Load parameters =============

    //========== Load widget's libraries =============
    try
    {
        //Search and create new libraries
        TConfig c_el(&elWdgLib());
        vector<string> tdb_ls, db_ls;
					
	//- Search into DB -
	SYS->db().at().modList(tdb_ls);
        for( int i_tp = 0; i_tp < tdb_ls.size(); i_tp++ )
        {
            SYS->db().at().at(tdb_ls[i_tp]).at().list(db_ls);
            for( int i_db = 0; i_db < db_ls.size(); i_db++ )
            {
                string wbd=tdb_ls[i_tp]+"."+db_ls[i_db];
                int lib_cnt = 0;
                while(SYS->db().at().dataSeek(wbd+"."+wlbTable(),"",lib_cnt++,c_el) )
                {
                    string l_id = c_el.cfg("ID").getS();
                    if(!wlbPresent(l_id)) wlbReg(new LibWdg(l_id.c_str(),"",(wbd==SYS->workDB())?"*.*":wbd));
        	    c_el.cfg("ID").setS("");
                }
            }
        }
	
	//- Search into config file -
        int lib_cnt = 0;
        while(SYS->db().at().dataSeek("",nodePath()+"wlb/",lib_cnt++,c_el) )
        {
            string l_id = c_el.cfg("ID").getS();
            if(!wlbPresent(l_id)) wlbReg(new LibWdg(l_id.c_str(),"","*.*"));
        	c_el.cfg("ID").setS("");
	}
	
	//- Load present libraries -
        wlbList(tdb_ls);
        for( int l_id = 0; l_id < tdb_ls.size(); l_id++ )
    	    wlbAt(tdb_ls[l_id]).at().load();
    }catch( TError err )
    {
	Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
        Mess->put(nodePath().c_str(),TMess::Error,mod->I18N("Load widget's libraries error."));
    }
}

void Engine::modSave( )
{
    //========== Save parameters =============
	
    //=========== Save widget's libraries =============
    vector<string> ls;
    wlbList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
	wlbAt(ls[l_id]).at().save();
}

void Engine::postEnable( )
{
    TModule::postEnable( );
   
    //Lib's DB structure
    lbwdg_el.fldAdd( new TFld("ID",I18N("ID"),TFld::String,TCfg::Key,"20") );
    lbwdg_el.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,TFld::NoFlag,"50") );
    lbwdg_el.fldAdd( new TFld("DESCR",I18N("Description"),TFld::String,TFld::NoFlag,"300") );
    lbwdg_el.fldAdd( new TFld("DB_TBL",I18N("Data base"),TFld::String,TFld::NoFlag,"30") );
    lbwdg_el.fldAdd( new TFld("ICO",I18N("Icon"),TFld::String,TFld::NoFlag,"10000") );
    lbwdg_el.fldAdd( new TFld("USER",I18N("User"),TFld::String,TFld::NoFlag,"20") );
    lbwdg_el.fldAdd( new TFld("GRP",I18N("Group"),TFld::String,TFld::NoFlag,"20") );    
    lbwdg_el.fldAdd( new TFld("PERMIT",I18N("Permision"),TFld::Integer,TFld::OctDec,"3") );
    
    //Widgets DB structure
    wdg_el.fldAdd( new TFld("ID",I18N("ID"),TFld::String,TCfg::Key,"20") );
    wdg_el.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,TFld::NoFlag,"50") );
    wdg_el.fldAdd( new TFld("DESCR",I18N("Description"),TFld::String,TFld::NoFlag,"300") );
    wdg_el.fldAdd( new TFld("ICO",I18N("Icon"),TFld::String,TFld::NoFlag,"10000") );
    wdg_el.fldAdd( new TFld("ORIGWDG",I18N("Original widget"),TFld::String,TFld::NoFlag,"20") );
    wdg_el.fldAdd( new TFld("PROC",I18N("Procedure text and language"),TFld::String,TFld::NoFlag,"10000") );
    wdg_el.fldAdd( new TFld("USER",I18N("User"),TFld::String,TFld::NoFlag,"20") );
    wdg_el.fldAdd( new TFld("GRP",I18N("Group"),TFld::String,TFld::NoFlag,"20") );
    wdg_el.fldAdd( new TFld("PERMIT",I18N("Permision"),TFld::Integer,TFld::OctDec,"3") );
    
    //Include widgets DB structure
    inclwdg_el.fldAdd( new TFld("IDW",I18N("IDW"),TFld::String,TCfg::Key,"20") );
    inclwdg_el.fldAdd( new TFld("ID",I18N("ID"),TFld::String,TCfg::Key,"20") );
    inclwdg_el.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,TFld::NoFlag,"50") );
    inclwdg_el.fldAdd( new TFld("DESCR",I18N("Description"),TFld::String,TFld::NoFlag,"300") );
    inclwdg_el.fldAdd( new TFld("ORIGWDG",I18N("Original widget"),TFld::String,TFld::NoFlag,"20") );
    
    //Widget's IO DB structure
    wdgio_el.fldAdd( new TFld("IDW",I18N("Widget ID"),TFld::String,TCfg::Key,"20") );
    wdgio_el.fldAdd( new TFld("ID",I18N("ID"),TFld::String,TCfg::Key,"40") );
    wdgio_el.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,TFld::NoFlag,"50") );
    wdgio_el.fldAdd( new TFld("IO_TYPE",I18N("Attribute generic flags and type"),TFld::Integer,TFld::NoFlag,"10") );
    wdgio_el.fldAdd( new TFld("IO_VAL",I18N("Attribute value"),TFld::String,TFld::NoFlag,"100000") );
    wdgio_el.fldAdd( new TFld("SELF_FLG",I18N("Attribute self flags"),TFld::Integer,TFld::NoFlag,"5") );
    wdgio_el.fldAdd( new TFld("LNK_TMPL",I18N("Link template"),TFld::String,TFld::NoFlag,"30") );
    wdgio_el.fldAdd( new TFld("LNK_VAL",I18N("Link value"),TFld::String,TFld::NoFlag,"100") );
}

void Engine::preDisable( int flag )
{
    modStop();

    TModule::postDisable( flag );
}

void Engine::modStart()
{
    //Libraries start
    vector<string> ls;
    wlbList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
	wlbAt(ls[l_id]).at().enable(true);
			
    run_st = true;
}

void Engine::modStop()
{
    //Libraries stop
    vector<string> ls;
    wlbList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
	wlbAt(ls[l_id]).at().enable(false);

    run_st = false;
}

void Engine::origList( vector<string> &ls )
{
    ls.clear();
    ls.push_back("ElFigure");
    ls.push_back("FormEl");
    ls.push_back("Text");
    ls.push_back("Media");
    ls.push_back("Trend");
    ls.push_back("Protocol");
    ls.push_back("Function");
    ls.push_back("UserEl");
    ls.push_back("AssocLine");
}

bool Engine::origPresent( const string &orig )
{
    vector<string> ls;
    origList(ls);
    for(int i_l = 0; i_l < ls.size(); i_l++)
	if(ls[i_l] == orig) return true;

    return false;
}

Widget *Engine::origGet( const string &orig, const string &id, bool lib_loc )
{
    if(orig == "ElFigure")	return new OrigElFigure(id,lib_loc);
    else if(orig == "FormEl")	return new OrigFormEl(id,lib_loc);
    else if(orig == "Text")	return new OrigText(id,lib_loc);
    else if(orig == "Media")	return new OrigMedia(id,lib_loc);
    else if(orig == "Trend")	return new OrigTrend(id,lib_loc);
    else if(orig == "Protocol")	return new OrigProtocol(id,lib_loc);
    else if(orig == "Function")	return new OrigFunction(id,lib_loc);
    else if(orig == "UserEl")	return new OrigUserEl(id,lib_loc);
    else if(orig == "AssocLine")return new OrigAssocLine(id,lib_loc);
    
    return NULL;
}

void Engine::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TUI::cntrCmdProc(opt);
        ctrMkNode("grp",opt,-1,"/br/wlb_",Mess->I18N("Widget's library"),0440,"root","root",1,"list","/prm/cfg/wlb");
	if(ctrMkNode("area",opt,-1,"/prm/cfg",I18N("Configuration"),0444,"root","root"))
	{
    	    ctrMkNode("list",opt,-1,"/prm/cfg/wlb",I18N("Widget's libraries"),0664,"root","root",4,"tp","br","idm","1","s_com","add,del","br_pref","wlb_");
	    ctrMkNode("comm",opt,-1,"/prm/cfg/load",I18N("Load"),0660);
    	    ctrMkNode("comm",opt,-1,"/prm/cfg/save",I18N("Save"),0660);
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/wlb" )
    {
        if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
        {
            vector<string> lst;
            wlbList(lst);
            for( unsigned i_a=0; i_a < lst.size(); i_a++ )
                opt->childAdd("el")->attr("id",lst[i_a])->text(wlbAt(lst[i_a]).at().name());
        }
        if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )
	{
	    wlbReg(new LibWdg(opt->attr("id").c_str(),opt->text().c_str()));
	    wlbAt(opt->attr("id")).at().user(opt->attr("user"));
	}
        if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )   wlbUnreg(opt->attr("id"),true);
    }
    else if( a_path == "/prm/cfg/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	modLoad();
    else if( a_path == "/prm/cfg/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	modSave();
    else TUI::cntrCmdProc(opt);
}
