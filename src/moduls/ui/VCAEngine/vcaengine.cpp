
//OpenSCADA system module UI.VCAEngine file: vcaengine.cpp
/***************************************************************************
 *   Copyright (C) 2006-2008 by Roman Savochenko                           * 
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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
#include <sys/types.h>
#include <unistd.h>

#include <tsys.h>
#include <tmess.h>

#include "origwidg.h"
#include "vcaengine.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"VCAEngine"
#define MOD_NAME	"Visual control area engine"
#define MOD_TYPE	"UI"
#define MOD_SUBTYPE	"VCAEngine"
#define VER_TYPE	VER_UI
#define VERSION		"0.7.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Generic visual control area engine."
#define LICENSE		"GPL"
//*************************************************

VCA::Engine *VCA::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new VCA::Engine( source );
	return NULL;
    }
}

using namespace VCA;

//************************************************
//* Engine                                       *
//************************************************
Engine::Engine( string name ) : mSynthCom("echo \"%t\" | ru_tts | sox -t raw -s -b -r 10000 -c 1 -v 0.8 - -t ogg -")
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;

    id_wlb = grpAdd("wlb_");
    id_prj = grpAdd("prj_");
    id_ses = grpAdd("ses_");
}

Engine::~Engine( )
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

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	    MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void Engine::postEnable( int flag )
{
    TModule::postEnable( flag );

    if( !(flag&TCntrNode::NodeConnect) ) return;

    //- Make lib's DB structure -
    lbwdg_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30") );
    lbwdg_el.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"50") );
    lbwdg_el.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TFld::NoFlag,"300") );
    lbwdg_el.fldAdd( new TFld("DB_TBL",_("Data base"),TFld::String,TFld::NoFlag,"30") );
    lbwdg_el.fldAdd( new TFld("ICO",_("Icon"),TFld::String,TFld::NoFlag,"10000") );

    //- Make library widgets data container -
    wdgdata_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30") );
    wdgdata_el.fldAdd( new TFld("MIME",_("Mime type"),TFld::String,TFld::NoFlag,"30") );
    wdgdata_el.fldAdd( new TFld("DATA",_("Mime data"),TFld::String,TFld::NoFlag,"500000") );

    //- Make widgets DB structure -
    wdg_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30") );
    wdg_el.fldAdd( new TFld("ICO",_("Icon"),TFld::String,TFld::NoFlag,"10000") );
    wdg_el.fldAdd( new TFld("PARENT",_("Parent widget"),TFld::String,TFld::NoFlag,"200") );
    wdg_el.fldAdd( new TFld("PROC",_("Procedure text and language"),TFld::String,TFld::NoFlag,"10000") );
    wdg_el.fldAdd( new TFld("PROC_PER",_("Procedure calc period"),TFld::Integer,TFld::NoFlag,"5","-1") );
    wdg_el.fldAdd( new TFld("USER",_("User"),TFld::String,TFld::NoFlag,"20","root") );
    wdg_el.fldAdd( new TFld("GRP",_("Group"),TFld::String,TFld::NoFlag,"20","UI") );
    wdg_el.fldAdd( new TFld("PERMIT",_("Permision"),TFld::Integer,TFld::OctDec,"3","436") );
    wdg_el.fldAdd( new TFld("ATTRS",_("Changed attributes"),TFld::String,TFld::NoFlag,"10000","*") );

    //- Make include widgets DB structure -
    inclwdg_el.fldAdd( new TFld("IDW",_("IDW"),TFld::String,TCfg::Key,"100") );
    inclwdg_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30") );
    inclwdg_el.fldAdd( new TFld("PARENT",_("Parent widget"),TFld::String,TFld::NoFlag,"200") );
    inclwdg_el.fldAdd( new TFld("ATTRS",_("Changed attributes"),TFld::String,TFld::NoFlag,"10000","*") );
    inclwdg_el.fldAdd( new TFld("USER",_("User"),TFld::String,TFld::NoFlag,"20","root") );
    inclwdg_el.fldAdd( new TFld("GRP",_("Group"),TFld::String,TFld::NoFlag,"20","UI") );
    inclwdg_el.fldAdd( new TFld("PERMIT",_("Permision"),TFld::Integer,TFld::OctDec,"3","436") );

    //- Make widget's IO DB structure -
    wdgio_el.fldAdd( new TFld("IDW",_("Widget ID"),TFld::String,TCfg::Key,"100") );
    wdgio_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"61") );
    wdgio_el.fldAdd( new TFld("IO_VAL",_("Attribute value"),TFld::String,TFld::NoFlag,"100000") );
    wdgio_el.fldAdd( new TFld("SELF_FLG",_("Attribute self flags"),TFld::Integer,TFld::NoFlag,"5") );
    wdgio_el.fldAdd( new TFld("CFG_TMPL",_("Configuration template"),TFld::String,TFld::NoFlag,"30") );
    wdgio_el.fldAdd( new TFld("CFG_VAL",_("Configuration value"),TFld::String,TFld::NoFlag,"1000") );

    //- Make widget's user IO DB structure -
    wdguio_el.fldAdd( new TFld("IDW",_("Widget ID"),TFld::String,TCfg::Key,"100") );
    wdguio_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"61") );
    wdguio_el.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"50") );
    wdguio_el.fldAdd( new TFld("IO_TYPE",_("Attribute generic flags and type"),TFld::Integer,TFld::NoFlag,"10") );
    wdguio_el.fldAdd( new TFld("IO_VAL",_("Attribute value"),TFld::String,TFld::NoFlag,"100000") );
    wdguio_el.fldAdd( new TFld("SELF_FLG",_("Attribute self flags"),TFld::Integer,TFld::NoFlag,"5") );
    wdguio_el.fldAdd( new TFld("CFG_TMPL",_("Configuration template"),TFld::String,TFld::NoFlag,"30") );
    wdguio_el.fldAdd( new TFld("CFG_VAL",_("Configuration value"),TFld::String,TFld::NoFlag,"1000") );

    //- Make project's DB structure -
    prj_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30") );
    prj_el.fldAdd( new TFld("NAME",_("Name"),TFld::String,TFld::NoFlag,"50") );
    prj_el.fldAdd( new TFld("DESCR",_("Description"),TFld::String,TFld::NoFlag,"300") );
    prj_el.fldAdd( new TFld("DB_TBL",_("Data base"),TFld::String,TFld::NoFlag,"30") );
    prj_el.fldAdd( new TFld("ICO",_("Icon"),TFld::String,TFld::NoFlag,"10000") );
    prj_el.fldAdd( new TFld("USER",_("User"),TFld::String,TFld::NoFlag,"20","root") );
    prj_el.fldAdd( new TFld("GRP",_("Group"),TFld::String,TFld::NoFlag,"20","UI") );
    prj_el.fldAdd( new TFld("PERMIT",_("Permision"),TFld::Integer,TFld::OctDec,"3","436") );
    prj_el.fldAdd( new TFld("PER",_("Calc period (ms)"),TFld::Integer,TFld::NoFlag,"4","100") );
    prj_el.fldAdd( new TFld("FLGS",_("Flags"),TFld::Integer,TFld::NoFlag,"4") );

    //- Make pages DB structure -
    page_el.fldAdd( new TFld("OWNER",_("Owner"),TFld::String,TCfg::Key,"100") );
    page_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30") );
    page_el.fldAdd( new TFld("ICO",_("Icon"),TFld::String,TFld::NoFlag,"10000") );
    page_el.fldAdd( new TFld("PARENT",_("Parent widget"),TFld::String,TFld::NoFlag,"200") );
    page_el.fldAdd( new TFld("PROC",_("Procedure text and language"),TFld::String,TFld::NoFlag,"10000") );
    page_el.fldAdd( new TFld("PROC_PER",_("Procedure calc period"),TFld::Integer,TFld::NoFlag,"5","-1") );
    page_el.fldAdd( new TFld("USER",_("User"),TFld::String,TFld::NoFlag,"20","root") );
    page_el.fldAdd( new TFld("GRP",_("Group"),TFld::String,TFld::NoFlag,"20","UI") );
    page_el.fldAdd( new TFld("PERMIT",_("Permision"),TFld::Integer,TFld::OctDec,"3","436") );
    page_el.fldAdd( new TFld("FLGS",_("Flags"),TFld::Integer,TFld::NoFlag,"1","0") );
    page_el.fldAdd( new TFld("ATTRS",_("Changed attributes"),TFld::String,TFld::NoFlag,"10000","*") );

    //- Make sessions' IO values of projects DB structure -
    prj_ses_el.fldAdd( new TFld("IDW",_("Widget ID"),TFld::String,TCfg::Key,"200") );
    prj_ses_el.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30") );
    prj_ses_el.fldAdd( new TFld("IO_VAL",_("Attribute value"),TFld::String,TFld::NoFlag,"100000") );

    //- Init original widgets library -
    wlbAdd("originals",_("Original widget's library"));
    //-- Set default library icon --
    if( TUIS::icoPresent("VCA.lwdg_root") )
        wlbAt("originals").at().setIco(TSYS::strEncode(TUIS::icoGet("VCA.lwdg_root"),TSYS::base64));
    //-- Add main original widgets --
    wlbAt("originals").at().add( new OrigElFigure() );
    wlbAt("originals").at().add( new OrigFormEl() );
    wlbAt("originals").at().add( new OrigText() );
    wlbAt("originals").at().add( new OrigMedia() );
    wlbAt("originals").at().add( new OrigDiagram() );
    wlbAt("originals").at().add( new OrigProtocol() );
    wlbAt("originals").at().add( new OrigDocument() );
    wlbAt("originals").at().add( new OrigFunction() );
    wlbAt("originals").at().add( new OrigBox() );
    wlbAt("originals").at().add( new OrigLink() );
}

void Engine::preDisable( int flag )
{
    modStop();

    TModule::preDisable( flag );
}

void Engine::load_( )
{
    mess_info(nodePath().c_str(),_("Load module."));

    //- Load parameters from command line -
    int next_opt;
    const char *short_opt="h";
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

    //- Load parameters from config file and DB -
    setSynthCom( TBDS::genDBGet(nodePath()+"SynthCom",synthCom()) );
    setSynthCode( TBDS::genDBGet(nodePath()+"SynthCode",synthCode()) );

    //-- Load widget's libraries --
    try
    {
	//--- Search and create new libraries ---
	TConfig c_el(&elWdgLib());
	c_el.cfgViewAll(false);
	vector<string> db_ls;

	//---- Search into DB ----
	SYS->db().at().dbList(db_ls);
	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	{
	    int lib_cnt = 0;
	    while(SYS->db().at().dataSeek(db_ls[i_db]+"."+wlbTable(),"",lib_cnt++,c_el) )
	    {
		string l_id = c_el.cfg("ID").getS();
		c_el.cfg("ID").setS("");
		if(!wlbPresent(l_id)) wlbAdd(l_id,"",(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
	    }
	}

	//---- Search into config file ----
	int lib_cnt = 0;
	while(SYS->db().at().dataSeek("",nodePath()+"LIB/",lib_cnt++,c_el) )
	{
	    string l_id = c_el.cfg("ID").getS();
	    c_el.cfg("ID").setS("");
	    if( !wlbPresent(l_id) )	wlbAdd(l_id,"","*.*");
	}

	//--- Load present libraries ---
	wlbList(db_ls);
	for( int l_id = 0; l_id < db_ls.size(); l_id++ )
	    wlbAt(db_ls[l_id]).at().load();
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Load widget's libraries error."));
    }

    //-- Load projects --
    try
    {
	//--- Search and create new projects ---
	TConfig c_el(&elProject());
	c_el.cfgViewAll(false);
	vector<string> db_ls;

	//---- Search into DB ----
	SYS->db().at().dbList(db_ls);
	for( int i_db = 0; i_db < db_ls.size(); i_db++ )
	{
	    int lib_cnt = 0;
	    while(SYS->db().at().dataSeek(db_ls[i_db]+"."+prjTable(),"",lib_cnt++,c_el) )
	    {
		string prj_id = c_el.cfg("ID").getS();
		c_el.cfg("ID").setS("");
		if( !prjPresent(prj_id) )
		    prjAdd(prj_id,"",(db_ls[i_db]==SYS->workDB())?"*.*":db_ls[i_db]);
	    }
	}

	//---- Search into config file ----
	int el_cnt = 0;
	while(SYS->db().at().dataSeek("",nodePath()+"PRJ/",el_cnt++,c_el) )
	{
	    string prj_id = c_el.cfg("ID").getS();
	    c_el.cfg("ID").setS("");
	    if( !prjPresent(prj_id) )	prjAdd(prj_id,"","*.*");
	}

	//--- Load present projects ---
	prjList(db_ls);
	for( int el_id = 0; el_id < db_ls.size(); el_id++ )
	    prjAt(db_ls[el_id]).at().load();
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Load projects error."));
    }

    //- Libraries enable -
    vector<string> ls;
    wlbList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
	wlbAt(ls[l_id]).at().setEnable(true);

    //- Projects enable -
    prjList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
	prjAt(ls[l_id]).at().setEnable(true);

    modifGClr( );
}

void Engine::save_( )
{
    mess_info(nodePath().c_str(),_("Save module."));

    //- Save parameters to DB -
    TBDS::genDBSet( nodePath()+"SynthCom", synthCom() );
    TBDS::genDBSet( nodePath()+"SynthCode", synthCode() );
}

void Engine::modStart()
{
    mess_info(nodePath().c_str(),_("Start module."));

    //- Libraries enable -
    vector<string> ls;
    wlbList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
	wlbAt(ls[l_id]).at().setEnable(true);

    //- Projects enable -
    prjList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
	prjAt(ls[l_id]).at().setEnable(true);

    run_st = true;
}

void Engine::modStop()
{
    mess_info(nodePath().c_str(),_("Stop module."));

    vector<string> ls;

    //- Stop sessions -
    sesList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
	sesAt(ls[l_id]).at().setEnable(false);

    //- Libraries stop -
    wlbList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
	wlbAt(ls[l_id]).at().setEnable(false);

    //- Projects stop -
    prjList(ls);
    for( int l_id = 0; l_id < ls.size(); l_id++ )
	prjAt(ls[l_id]).at().setEnable(false);

    run_st = false;
}

void Engine::wlbAdd( const string &iid, const string &inm, const string &idb )
{
    if(wlbPresent(iid))	return;
    chldAdd(id_wlb, new WidgetLib(iid,inm,idb));
}

AutoHD<WidgetLib> Engine::wlbAt( const string &id )
{
    return chldAt(id_wlb,id);
}

void Engine::prjAdd( const string &iid, const string &inm, const string &idb )
{
    if(prjPresent(iid))	return;
    chldAdd(id_prj, new Project(iid,inm,idb));
}

AutoHD<Project> Engine::prjAt( const string &id )
{
    return chldAt(id_prj,id);
}

void Engine::sesAdd( const string &iid, const string &iproj )
{
    if(sesPresent(iid))	return;
    chldAdd(id_ses, new Session(iid,iproj));
}

AutoHD<Session> Engine::sesAt( const string &id )
{
    return chldAt(id_ses,id);
}

string Engine::callSynth( const string &itxt )
{
    int		comPos = 0;
    char	buf[STR_BUF_LEN];
    string	rez;
    const char	*synthRez = "/var/tmp/oscadaSynthTmp";
    string	txt = Mess->codeConvOut(synthCode(),itxt);

    ResAlloc res( mSynthRes, true );

    string com = synthCom( );

    //- Put text to command -
    bool textToPipe = false;
    if( (comPos=com.find("%t")) != string::npos )
	com.replace( comPos, 2, txt );
    else textToPipe = true;

    //- Put result file name to command -
    bool rezFromPipe = false;
    if( (comPos=com.find("%f")) != string::npos )
	com.replace( comPos, 2, synthRez );
    else rezFromPipe = true;

    if( rezFromPipe && textToPipe )	return "";

    //- Open pipe -
    FILE *fp = popen( com.c_str(), textToPipe?"w":"r" );
    if( !fp )	return "";
    //- Write text to pipe -
    if( textToPipe )	fwrite( txt.c_str(), txt.size(), 1, fp );
    //- Read result from pipe -
    if( rezFromPipe )
	while( comPos = fread( buf, 1, sizeof(buf), fp ) )
	    rez.append(buf,comPos);
    pclose(fp);
    //- Read result from result file -
    if( !rezFromPipe )
    {
	FILE *fp = fopen( synthRez, "r" );
	if( !fp ) return "";
	while( comPos = fread( buf, 1, sizeof(buf), fp ) )
	    rez.append(buf,comPos);
	fclose(fp);
	remove( synthRez );
    }

    return TSYS::strEncode( rez, TSYS::base64 );
}

void Engine::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //- Service commands process -
    if( a_path == "/serv/sess" )	//Session operation
    {
	if( ctrChkNode(opt,"list",RWRWRW,"root","root",SEQ_RD) ) //List session for some project
	{
	    string prj = opt->attr("prj");
	    vector<string> ls;
	    sesList(ls);
	    for( int i_l = 0; i_l < ls.size(); i_l++ )
		if( prj.empty() || sesAt(ls[i_l]).at().projNm() == prj )
		{
		    AutoHD<Project> prj = sesAt(ls[i_l]).at().parent();
		    if( SYS->security().at().access(opt->attr("user"),SEQ_RD,prj.at().owner(),prj.at().grp(),prj.at().permit()) )
			opt->childAdd("el")->setText(ls[i_l]);
		}
	}
	else if( ctrChkNode(opt,"connect",RWRWRW,"root","root",SEQ_WR) )
	{
	    string sess = opt->attr("sess");
	    string prj  = opt->attr("prj");

	    //-- User permission check --
	    AutoHD<Project> wprj = (!sess.empty()) ? sesAt(sess).at().parent() : prjAt(prj);
	    if( !SYS->security().at().access(opt->attr("user"),SEQ_RD,wprj.at().owner(),wprj.at().grp(),wprj.at().permit()) )
		throw TError(nodePath().c_str(),_("Connection to session no permit for '%s'."),opt->attr("user").c_str());
	    //-- Connect to present session --
	    if( !sess.empty() )	sesAt(sess).at().connect();
	    //-- Create session --
	    else if( !prj.empty() )
	    {
		//--- Prepare session name ---
		sess = prj;
		for( int p_cnt = 0; sesPresent(sess); p_cnt++ )
		    sess = prj+TSYS::int2str(p_cnt);
		sesAdd( sess, prj );
		sesAt(sess).at().setUser(opt->attr("user"));
		sesAt(sess).at().setStart(true);
		sesAt(sess).at().connect();
		opt->setAttr("sess",sess);
	    }else throw TError(nodePath().c_str(),_("Connect/create session arguments error."));
	}
	else if( ctrChkNode(opt,"disconnect",RWRWRW,"root","root",SEQ_WR) )
	{
	    string sess = opt->attr("sess");
	    sesAt(sess).at().disconnect();
	    if( sesAt(sess).at().connects( ) == 0 && !sesAt(sess).at().backgrnd( ) )
		sesDel(sess);
	}
	return;
    }

    //- Get page info -
    if( opt->name() == "info" )
    {
	TUI::cntrCmdProc(opt);
	ctrMkNode("grp",opt,-1,"/br/prj_",_("Project"),0664,"root","UI",1,"idm","1");
	ctrMkNode("grp",opt,-1,"/br/wlb_",_("Widget's library"),0664,"root","UI",1,"idm","1");
	ctrMkNode("grp",opt,-1,"/br/ses_",_("Session"),0664,"root","UI");
	if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Configuration"),0444,"root","UI"))
	{
	    ctrMkNode("list",opt,-1,"/prm/cfg/prj",_("Project"),0664,"root","UI",4,"tp","br","idm","1","s_com","add,del","br_pref","prj_");
	    ctrMkNode("list",opt,-1,"/prm/cfg/wlb",_("Widget's libraries"),0664,"root","UI",4,"tp","br","idm","1","s_com","add,del","br_pref","wlb_");
	}
	if(ctrMkNode("area",opt,1,"/ses",_("Sessions"),0444,"root","UI"))
	    ctrMkNode("list",opt,-1,"/ses/ses",_("Sessions"),0664,"root","UI",3,"tp","br","s_com","add,del","br_pref","ses_");
	if(ctrMkNode("area",opt,2,"/tts",_("Speech text synthesis"),0444,"root","UI"))
	{
	    ctrMkNode("fld",opt,-1,"/tts/comm",_("Command"),0664,"root","UI",4,"tp","str","dest","sel_ed","select","/tts/comm_ls","help",
		_("Command line for call speech text synthesis engine.\n"
		  "Use next words for replace:\n"
		  "  %t - synthesis text;\n"
		  "  %f - result file name.\n"
		  "If result file name not used then result readed from pipe.\n"
		  "If used result file name and not used %t then synthesis text sending to pipe."));
	    ctrMkNode("fld",opt,-1,"/tts/code",_("Text code"),0664,"root","UI",2,"tp","str","help",_("Engine text codepage for text encode to it."));
	}
	return;
    }

    //- Process command for page -
    if( a_path == "/br/prj_" || a_path == "/prm/cfg/prj" )
    {
	if( ctrChkNode(opt,"get",0664,"root","UI",SEQ_RD) )
	{
	    vector<string> lst;
	    prjList(lst);
	    bool chkUserPerm = atoi(opt->attr("chkUserPerm").c_str());
	    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
	    {
		if( chkUserPerm )
		{
		    AutoHD<Project> prj = prjAt(lst[i_a]);
		    if( !SYS->security().at().access(opt->attr("user"),SEQ_RD,prj.at().owner(),prj.at().grp(),prj.at().permit()) )
			continue;
		}
		opt->childAdd("el")->setAttr("id",lst[i_a])->setText(prjAt(lst[i_a]).at().name());
	    }
	}
	if( ctrChkNode(opt,"add",0664,"root","UI",SEQ_WR) )
	{
	    prjAdd(opt->attr("id"),opt->text());
	    prjAt(opt->attr("id")).at().setOwner(opt->attr("user"));
	}
	if( ctrChkNode(opt,"del",0664,"root","UI",SEQ_WR) )   prjDel(opt->attr("id"),true);
    }
    else if( a_path == "/br/wlb_" || a_path == "/prm/cfg/wlb" )
    {
	if( ctrChkNode(opt,"get",0664,"root","UI",SEQ_RD) )
	{
	    vector<string> lst;
	    wlbList(lst);
	    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
		opt->childAdd("el")->setAttr("id",lst[i_a])->setText(wlbAt(lst[i_a]).at().name());
	}
	if( ctrChkNode(opt,"add",0664,"root","UI",SEQ_WR) )	wlbAdd(opt->attr("id"),opt->text());
	if( ctrChkNode(opt,"del",0664,"root","UI",SEQ_WR) )	wlbDel(opt->attr("id"),true);
    }
    else if( a_path == "/prm/cfg/cp/cp" && ctrChkNode(opt,"set",0660,"root","UI",SEQ_WR) )
	nodeCopy( nodePath(0,true)+opt->attr("src"), nodePath(0,true)+opt->attr("dst"), opt->attr("user") );
    else if( a_path == "/br/ses_" || a_path == "/ses/ses" )
    {
	if( ctrChkNode(opt,"get",0664,"root","UI",SEQ_RD) )
	{
	    vector<string> lst;
	    sesList(lst);
	    bool chkUserPerm = atoi(opt->attr("chkUserPerm").c_str());
	    for( unsigned i_a=0; i_a < lst.size(); i_a++ )
	    {
		if( chkUserPerm )
		{
		    AutoHD<Project> prj = sesAt(lst[i_a]).at().parent();
		    if( !SYS->security().at().access(opt->attr("user"),SEQ_RD,prj.at().owner(),prj.at().grp(),prj.at().permit()) )
			continue;
		}
		opt->childAdd("el")->setText(lst[i_a]);
	    }
	}
	if( ctrChkNode(opt,"add",0664,"root","UI",SEQ_WR) )
	{
	    sesAdd(opt->text());
	    sesAt(opt->text()).at().setUser(opt->attr("user"));
	    sesAt(opt->text()).at().setBackgrnd(true);
	}
	if( ctrChkNode(opt,"del",0664,"root","UI",SEQ_WR) )   sesDel(opt->text(),true);
    }
    else if( a_path == "/tts/code" )
    {
	if( ctrChkNode(opt,"get",0664,"root","UI",SEQ_RD) )	opt->setText(synthCode());
	if( ctrChkNode(opt,"set",0664,"root","UI",SEQ_WR) )	setSynthCode(opt->text());
    }
    else if( a_path == "/tts/comm" )
    {
	if( ctrChkNode(opt,"get",0664,"root","UI",SEQ_RD) )	opt->setText(synthCom());
	if( ctrChkNode(opt,"set",0664,"root","UI",SEQ_WR) )	setSynthCom(opt->text());
    }
    else if( a_path == "/tts/comm_ls" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setText("echo \"%t\" | ru_tts | sox -t raw -s -b -r 10000 -c 1 -v 0.8 - -t ogg -");
    }
    else TUI::cntrCmdProc(opt);
}
