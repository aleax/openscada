
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
TVision::TVision( string name ) : end_run(false)
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
}

TVision::~TVision()
{
    //- Free widget's shapes -
    for( int i_sw = 0; i_sw < shapesWdg.size(); i_sw++ )
	delete shapesWdg[i_sw];
    shapesWdg.clear();
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
        "StartUser  <user>    No password requested start user.\n\n"),
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
    start_user = TBDS::genDBGet(nodePath()+"StartUser",start_user);    
}

void TVision::modSave( )
{
#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Save module."));
#endif
    //========== Save parameters to DB =============
    TBDS::genDBSet(nodePath()+"StartUser",start_user);
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

    string user_open = start_user;
    if(!SYS->security().at().usrPresent(user_open))	
	while(true)
	{
    	    DlgUser d_usr;
            int rez = d_usr.exec();
            if( rez == DlgUser::SelCancel )	return NULL;
	    if( rez == DlgUser::SelErr )
	    {
                postMess(nodePath().c_str(),_("Auth wrong!!!"));
		continue;
            }
            user_open = d_usr.user().toAscii().data();
            break;
        }
	
    return new VisDevelop(user_open);
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
        if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )   opt->setText(start_user);
        if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )   start_user = opt->text();
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

int TVision::cntrIfCmd( XMLNode &node )
{
    //- Check local station request -
    //unsigned long long st_tm = SYS->shrtCnt();
    SYS->ui().at().at("VCAEngine").at().cntrCmd(&node);
    //double tm = 1.0e3*((double)(SYS->shrtCnt()-st_tm))/((double)SYS->sysClk());
    //if( tm > 2 ) printf("TEST 00: Req <%s(%s)> time: %fms\n",node.name().c_str(),node.attr("path").c_str(),tm);
    return atoi(node.attr("rez").c_str());
}
