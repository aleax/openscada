
//OpenSCADA system module UI.VISION file: tvision.cpp
/***************************************************************************
 *   Copyright (C) 2005-2006 by Evgen Zaichuk
 *                 2006-2007 by Roman Savochenko (rom_as@diyaorg.dp.ua)
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

//#include <QApplication>
//#include <QCloseEvent>
#include <QIcon>
#include <QMessageBox>
#include <QErrorMessage>

#include "vis_devel.h"
#include "vis_run.h"
#include "vis_widgs.h"
#include "vis_shapes.h"
#include "vis_shape_elfig.h"
#include "tvision.h"

//============ Modul info! =====================================================
#define MOD_ID      "Vision"
#define MOD_NAME    "Operation user interface (QT)"
#define MOD_TYPE    "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "QT"
#define VERSION     "0.1.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Visual operation user interface."
#define LICENSE     "GPL"
//==============================================================================

VISION::TVision *VISION::mod;

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
	else
	    AtMod.id	= "";

	return AtMod;
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	VISION::TVision *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = VISION::mod = new VISION::TVision( source );

	return self_addr;
    }
}

using namespace VISION;

//==============================================================================
//================= QTCFG::TVision =============================================
//==============================================================================
TVision::TVision( string name ) : end_run(false), vca_station(".")
{
    mId		= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;
    
    //- Export functions -
    modFuncReg( new ExpFunc("QIcon icon();","Module QT-icon",(void(TModule::*)( )) &TVision::icon) );
    modFuncReg( new ExpFunc("QMainWindow *openWindow();","Start QT GUI.",(void(TModule::*)( )) &TVision::openWindow) );
    
    //External hosts' conection DB struct
    el_ext.fldAdd( new TFld("OP_USER",I18N("Open user"),TFld::String,TCfg::Key,"20") );
    el_ext.fldAdd( new TFld("ID",I18N("ID"),TFld::String,TCfg::Key,"20") );
    el_ext.fldAdd( new TFld("NAME",I18N("Name"),TFld::String,0,"50") );
    el_ext.fldAdd( new TFld("TRANSP",I18N("Transport"),TFld::String,0,"20") );
    el_ext.fldAdd( new TFld("ADDR",I18N("Transport address"),TFld::String,0,"50") );
    el_ext.fldAdd( new TFld("USER",I18N("Request user"),TFld::String,0,"20") );
    el_ext.fldAdd( new TFld("PASS",I18N("Request password"),TFld::String,0,"30") );    
}

TVision::~TVision()
{
    //- Free widget's shapes -
    for( int i_sw = 0; i_sw < shapesWdg.size(); i_sw++ )
        delete shapesWdg[i_sw];
    shapesWdg.clear();
}

string TVision::extTranspBD()
{
    return SYS->workDB()+".CfgExtHosts";
}

void TVision::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TVision::modInfo( const string &name )
{
    if( name == "SubType" ) return SUB_TYPE;
    else return TModule::modInfo( name);
}

string TVision::optDescr( )
{
    char buf[STR_BUF_LEN];
    
    snprintf(buf,sizeof(buf),_(
        "======================= The module <%s:%s> options =======================\n"
        "---------- Parameters of the module section <%s> in config file ----------\n"
        "StartUser  <user>    No password requested start user.\n"
	"RunPrjs    <list>    Run projects list on the module start.\n"
	"VCAstation <id>      VCA station id ('.' - local)\n"),	
        MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TVision::modLoad( )
{
#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Load module."));
#endif

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
    
    //========== Load parameters from config file and DB =============
    setStartUser(TBDS::genDBGet(nodePath()+"StartUser",startUser()));
    setRunPrjs(TBDS::genDBGet(nodePath()+"RunPrjs",runPrjs()));
    setVCAStation(TBDS::genDBGet(nodePath()+"VCAstation",VCAStation()));
}

void TVision::modSave( )
{
#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Save module."));
#endif
    //========== Save parameters to DB =============
    TBDS::genDBSet(nodePath()+"StartUser",startUser());
    TBDS::genDBSet(nodePath()+"RunPrjs",runPrjs());
    TBDS::genDBSet(nodePath()+"VCAstation",VCAStation());
}

void TVision::postEnable( int flag )
{
   TModule::postEnable(flag);
}

QIcon TVision::icon()
{
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("UI.Vision").c_str())) ico_t.load(":/images/vision.png");
    return QPixmap::fromImage(ico_t);
}

QMainWindow *TVision::openWindow()
{
    //- Register support widget's shapes -
    if( shapesWdg.empty() )
    {
	shapesWdg.push_back( new ShapeElFigure );
	shapesWdg.push_back( new ShapeFormEl );
	shapesWdg.push_back( new ShapeText );
	shapesWdg.push_back( new ShapeMedia );
	shapesWdg.push_back( new ShapeDiagram );
	shapesWdg.push_back( new ShapeProtocol );
	shapesWdg.push_back( new ShapeDocument );
	shapesWdg.push_back( new ShapeFunction );
	shapesWdg.push_back( new ShapeBox );
	shapesWdg.push_back( new ShapeLink );
    }

    string user_open = startUser();
    if(!SYS->security().at().usrPresent(user_open))	
	while(true)
	{
    	    DlgUser d_usr;
            int rez = d_usr.exec();
            if( rez == DlgUser::SelCancel ) return NULL;
	    if( rez == DlgUser::SelErr )
	    {
                postMess(nodePath().c_str(),_("Auth wrong!!!"));
		continue;
            }
            user_open = d_usr.user().toAscii().data();
            break;
        }

    //- Check for run projects need -
    bool runPrj = false;
    string sprj;
    for( int p_off = 0; (sprj=TSYS::strSepParse(run_prjs,0,';',&p_off)).size(); )    
    {
	//-- Find for already opened run window --
	int i_w = 0;
	for( ; i_w < mn_winds.size(); i_w++ )
	    if( qobject_cast<VisRun*>(mn_winds[i_w]) && ((VisRun*)mn_winds[i_w])->srcProject( ) == sprj )
		break;
	if( i_w < mn_winds.size() ) continue;
	VisRun *sess = new VisRun( "/prj_"+sprj, user_open, VCAStation(), true );
	sess->show();
	sess->raise();
	sess->activateWindow();
	runPrj = true;
    }
    	
    return runPrj ? NULL : new VisDevelop(user_open,VCAStation());
}

void TVision::modStart()
{
#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Start module."));
#endif

    end_run = false;
    run_st  = true;
}

void TVision::modStop()
{   
#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Stop module."));
#endif
    end_run = true;

    for(int i_w = 0; i_w < mn_winds.size(); i_w++ )
	while(mn_winds[i_w]) usleep(STD_WAIT_DELAY*1000);	

    run_st = false;
}

WdgShape *TVision::getWdgShape( const string &iid )
{
    for( int i_sw = 0; i_sw < shapesWdg.size(); i_sw++ )
        if( shapesWdg[i_sw]->id() == iid )
            return shapesWdg[i_sw];

    return NULL;
}

void TVision::regWin( QMainWindow *mwd )
{
    int i_w;
    for( i_w = 0; i_w < mn_winds.size(); i_w++ )
	if( mn_winds[i_w] == NULL ) break;
    if( i_w == mn_winds.size() ) mn_winds.push_back(NULL);
    mn_winds[i_w] = mwd;
}

void TVision::unregWin( QMainWindow *mwd )
{
    for( int i_w = 0; i_w < mn_winds.size(); i_w++ )
        if( mn_winds[i_w] == mwd ) mn_winds[i_w] = NULL;
}

void TVision::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TUI::cntrCmdProc(opt);
        if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
        {
            ctrMkNode("fld",opt,-1,"/prm/cfg/start_user",_("Configurator start user"),0664,"root","root",3,"tp","str","dest","select","select","/prm/cfg/u_lst");
            ctrMkNode("fld",opt,-1,"/prm/cfg/run_prj",_("Run projects list (';' - sep)"),0664,"root","root",1,"tp","str");
            ctrMkNode("fld",opt,-1,"/prm/cfg/stationVCA",_("VCA engine station"),0664,"root","root",4,"tp","str","idm","1","dest","select","select","/prm/cfg/vca_lst");
	    if(ctrMkNode("table",opt,-1,"/prm/cfg/ehost",_("External hosts poll"),0666,"root","root",2,"s_com","add,del","key","id"))
            {
	        ctrMkNode("list",opt,-1,"/prm/cfg/ehost/id",_("Id"),0666,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/prm/cfg/ehost/name",_("Name"),0666,"root","root",1,"tp","str");
	        ctrMkNode("list",opt,-1,"/prm/cfg/ehost/transp",_("Transport"),0666,"root","root",4,"tp","str","idm","1","dest","select","select","/prm/cfg/transps");
	        ctrMkNode("list",opt,-1,"/prm/cfg/ehost/addr",_("Address"),0666,"root","root",1,"tp","str");
	        ctrMkNode("list",opt,-1,"/prm/cfg/ehost/user",_("User"),0666,"root","root",1,"tp","str");
	        ctrMkNode("list",opt,-1,"/prm/cfg/ehost/pass",_("Password"),0666,"root","root",1,"tp","str");
	    }
            ctrMkNode("comm",opt,-1,"/prm/cfg/load",_("Load"),0660);
            ctrMkNode("comm",opt,-1,"/prm/cfg/save",_("Save"),0660);
        }
        ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/start_user" )
    {
        if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )   opt->setText(startUser());
        if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )   setStartUser(opt->text());
    }
    else if( a_path == "/prm/cfg/run_prj" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(runPrjs());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setRunPrjs(opt->text());
    }
    else if( a_path == "/prm/cfg/stationVCA" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(VCAStation());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setVCAStation(opt->text());
    }
    else if( a_path == "/prm/cfg/ehost" )
    {
        TConfig c_el(&el_ext);
        c_el.cfg("OP_USER").setS(opt->attr("user"));
        if( ctrChkNode(opt,"get",0666,"root","root",SEQ_RD) )
        {
            XMLNode *n_id       = ctrMkNode("list",opt,-1,"/prm/cfg/ehost/id","",0666);
            XMLNode *n_nm       = ctrMkNode("list",opt,-1,"/prm/cfg/ehost/name","",0666);
	    XMLNode *n_tr       = ctrMkNode("list",opt,-1,"/prm/cfg/ehost/transp","",0666);
            XMLNode *n_addr     = ctrMkNode("list",opt,-1,"/prm/cfg/ehost/addr","",0666);
            XMLNode *n_user     = ctrMkNode("list",opt,-1,"/prm/cfg/ehost/user","",0666);
            XMLNode *n_pass     = ctrMkNode("list",opt,-1,"/prm/cfg/ehost/pass","",0666);

    	    for( int fld_cnt = 0; SYS->db().at().dataSeek(extTranspBD(),nodePath()+"ExtTansp/",fld_cnt,c_el); fld_cnt++ )	
    	    {
                if(n_id)        n_id->childAdd("el")->setText(c_el.cfg("ID").getS());
                if(n_nm)        n_nm->childAdd("el")->setText(c_el.cfg("NAME").getS());
                if(n_tr)        n_tr->childAdd("el")->setText(c_el.cfg("TRANSP").getS());
                if(n_addr)      n_addr->childAdd("el")->setText(c_el.cfg("ADDR").getS());
                if(n_user)      n_user->childAdd("el")->setText(c_el.cfg("USER").getS());
                if(n_pass)      n_pass->childAdd("el")->setText(c_el.cfg("PASS").getS().size()?"*******":"");
		c_el.cfg("ID").setS("");
            }
        }
        if( ctrChkNode(opt,"add",0666,"root","root",SEQ_WR) )
	{
	    c_el.cfg("ID").setS(_("newHost"));
	    c_el.cfg("NAME").setS(_("New external host"));
	    c_el.cfg("USER").setS(opt->attr("user"));
	    SYS->db().at().dataSet(extTranspBD(),nodePath()+"ExtTansp/",c_el);
	}
        if( ctrChkNode(opt,"del",0666,"root","root",SEQ_WR) )
	{
	    c_el.cfg("ID").setS(opt->attr("key_id"));
	    SYS->db().at().dataDel(extTranspBD(),nodePath()+"ExtTansp/",c_el);
	}
        if( ctrChkNode(opt,"set",0666,"root","root",SEQ_WR) )
        {
            string col  = opt->attr("col");
	    c_el.cfg("ID").setS(opt->attr("key_id"));	    
            if( col == "id" )
            {
		SYS->db().at().dataGet(extTranspBD(),nodePath()+"ExtTansp/",c_el);
		SYS->db().at().dataDel(extTranspBD(),nodePath()+"ExtTansp/",c_el);
		c_el.cfg("ID").setS(opt->text());
		SYS->db().at().dataSet(extTranspBD(),nodePath()+"ExtTansp/",c_el);
		return;
            }
	    c_el.cfgViewAll(false);
            if( col == "name" )    	c_el.cfg("NAME").setS(opt->text());
            else if( col == "transp" )  c_el.cfg("TRANSP").setS(opt->text());
            else if( col == "addr" )    c_el.cfg("ADDR").setS(opt->text());
            else if( col == "user" )    c_el.cfg("USER").setS(opt->text());
            else if( col == "pass" )    c_el.cfg("PASS").setS(opt->text());
	    SYS->db().at().dataSet(extTranspBD(),nodePath()+"ExtTansp/",c_el);
        }
    }
    else if( a_path == "/prm/cfg/transps" && ctrChkNode(opt) )
    {
        vector<string>  list;
        SYS->transport().at().modList(list);
        for( int i_a = 0; i_a < list.size(); i_a++ )
            opt->childAdd("el")->setAttr("id",list[i_a])->setText(SYS->transport().at().modAt(list[i_a]).at().modName());
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )   opt->setText(optDescr());
    else if( a_path == "/prm/cfg/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )  modLoad();
    else if( a_path == "/prm/cfg/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )  modSave();
    else if( a_path == "/prm/cfg/u_lst" && ctrChkNode(opt) )
    {
        vector<string> ls;
        SYS->security().at().usrList(ls);
        opt->childAdd("el")->setText("");
        for(int i_u = 0; i_u < ls.size(); i_u++)
    	    opt->childAdd("el")->setText(ls[i_u]);
    }
    else if( a_path == "/prm/cfg/vca_lst" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id",".")->setText("Local");    
        TConfig c_el(&el_ext);
	c_el.cfg("OP_USER").setS(opt->attr("user"));
        for( int fld_cnt = 0; SYS->db().at().dataSeek(extTranspBD(),nodePath()+"ExtTansp/",fld_cnt,c_el); fld_cnt++ )
	{
	    opt->childAdd("el")->setAttr("id",c_el.cfg("ID").getS())->setText(c_el.cfg("NAME").getS());
	    c_el.cfg("ID").setS("");
	}
    }
    else TUI::cntrCmdProc(opt);
}

void TVision::postMess( const QString &cat, const QString &mess, TVision::MessLev type, QWidget *parent )
{
    //Put system message.
    message(cat.toAscii().data(),(type==TVision::Crit) ? TMess::Crit :
			(type==TVision::Error)?TMess::Error:
			(type==TVision::Warning)?TMess::Warning:TMess::Info,"%s",mess.toAscii().data());
    //QT message
    switch(type)
    {
	case TVision::Info:	
	    QMessageBox::information(parent,_(MOD_NAME),mess);	break;
	case TVision::Warning:
	    QMessageBox::warning(parent,_(MOD_NAME),mess);	break;
	case TVision::Error:
	    QMessageBox::critical(parent,_(MOD_NAME),mess);	break;
	case TVision::Crit:
	    QErrorMessage::qtHandler()->showMessage(mess);	break;
    }
}

int TVision::cntrIfCmd( XMLNode &node, VCAHost &host, bool glob )
{
    //- Check for local VCAEngine path - 
    if( !glob ) node.setAttr("path","/UI/VCAEngine"+node.attr("path"));
    
    //- Local station request -
    if( host.stat.empty() || host.stat == "." ) 
    {
	SYS->cntrCmd(&node);
	return atoi(node.attr("rez").c_str());
    }
    
    //- Externel station request -
    //-- Check transport --
    string tr_nm = "VCAStat"+host.stat;
    if(!SYS->transport().at().at(host.transp).at().outPresent(tr_nm))
    {
        SYS->transport().at().at(host.transp).at().outAdd(tr_nm);
        SYS->transport().at().at(host.transp).at().outAt(tr_nm).at().setAddr(host.addr);
    }
    AutoHD<TTransportOut> tr = SYS->transport().at().at(host.transp).at().outAt(tr_nm);
    if(!tr.at().startStat())    tr.at().start();
    //-- Request to remote station --
    char buf[1000], buf1[256];
    string req, resp;
    int rez, resp_len;

    try
    {
        while(true)
        {
            //--- Session open ---
            if(host.ses_id < 0)
            {
                req = "SES_OPEN "+host.user+" "+host.pass+"\n";
                resp_len = tr.at().messIO(req.c_str(),req.size(),buf,sizeof(buf),20);
                buf[resp_len] = 0;
                buf1[0] = 0;
                sscanf(buf,"REZ %d %255s\n",&rez,buf1);
                if(rez == 1)    throw TError(mod->nodePath().c_str(),_("Station <%s> auth error: %s!"),host.stat.c_str(),buf1);
                else if(rez > 0)throw TError(mod->nodePath().c_str(),_("Station <%s> error: %s!"),host.stat.c_str(),buf1);
                host.ses_id = atoi(buf1);
            }
            //--- Request ---
	    string data = node.save();
            req = "REQ "+TSYS::int2str(host.ses_id)+" "+TSYS::int2str(data.size())+"\n"+data;
            buf[0] = 0;
            resp_len = tr.at().messIO(req.c_str(),req.size(),buf,sizeof(buf),20);
            resp.assign(buf,resp_len);
	    //Get head
            buf1[0] = 0;
            if(sscanf(resp.c_str(),"REZ %d %255s\n",&rez,buf1)<=0)
        	throw TError(mod->nodePath().c_str(),_("Station respond <%s> error!"),host.stat.c_str());
            if(rez == 1)        { host.ses_id = -1; continue; }
            if(rez > 0) throw TError(mod->nodePath().c_str(),_("Station <%s> error: %d:%s!"),host.stat.c_str(),rez,buf1);
    	    int head_end = resp.find("\n",0);
            if(head_end == string::npos)
        	throw TError(mod->nodePath().c_str(),_("Station <%s> error: Respond broken!"),host.stat.c_str());
            int resp_size = atoi(buf1);
            //Wait tail
            while(resp.size() < resp_size+head_end+sizeof('\n'))
            {
                resp_len = tr.at().messIO(NULL,0,buf,sizeof(buf),20);
                resp.append(buf,resp_len);
            }
	    
    	    node.load(resp.substr(head_end));
            host.link_ok = true;
            return atoi(node.attr("rez").c_str());
        }
    }
    catch(TError err)
    {
        if(host.link_ok)
        {
    	    host.link_ok = false;
            tr.at().stop();
        }
        throw;
    }    
}
