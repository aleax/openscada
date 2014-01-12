//OpenSCADA system file: tuis.cpp
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tsys.h"
#include "tmess.h"
#include "tuis.h"

#if HAVE_GD_CORE
#include <gd.h>
#endif

using namespace OSCADA;

//*************************************************
//* TUIS                                          *
//*************************************************
TUIS::TUIS( ) : TSubSYS(SUI_ID,_("User interfaces"),true)
{
#if HAVE_GD_CORE
    gdFTUseFontConfig(1);
#endif
}

string TUIS::optDescr( )
{
    return(_(
	"===================== Subsystem \"User interfaces\" options ===============\n\n"));
}

void TUIS::load_( )
{
    //> Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
        if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //> Load parameters from config-file
}

void TUIS::subStart( )
{
    mess_info(nodePath().c_str(), _("Start subsystem."));

    TSubSYS::subStart( );
}

void TUIS::subStop( )
{
    mess_info(nodePath().c_str(), _("Stop subsystem."));

    TSubSYS::subStop( );
}

string TUIS::icoGet( const string &inm, string *tp, bool retPath )
{
    unsigned i_t = 0;
    int hd = -1;
    string rez, pathi;
    char types[][5] = {"png","gif","jpg","jpeg"};

    for(int off = 0; hd == -1 && (pathi=TSYS::strParse(SYS->icoDir(),0,";",&off)).size(); )
	for(i_t = 0; i_t < sizeof(types)/5; i_t++)
	    if((hd=open((pathi+"/"+inm+"."+types[i_t]).c_str(),O_RDONLY)) != -1) break;
    if(hd != -1)
    {
	if(tp) *tp = types[i_t];
	if(retPath) rez = pathi+"/"+inm+"."+types[i_t];
	else
	{
	    char buf[STR_BUF_LEN];
	    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) rez.append(buf,len);
	}
	close(hd);
    }

    return rez;
}

void TUIS::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),R_R___,"root",SUI_ID,3,"tp","str","cols","90","rows","10");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/help/g_help" && ctrChkNode(opt,"get",R_R___,"root",SUI_ID))	opt->setText(optDescr());
    else TSubSYS::cntrCmdProc(opt);
}

//*************************************************
//* TUI                                           *
//*************************************************
TUI::TUI( const string &id ) : TModule(id), run_st(false)
{

}

void TUI::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TModule::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,0,"/prm",_("User interface")))
	    if(ctrMkNode("area",opt,-1,"/prm/st",_("State")))
		ctrMkNode("fld",opt,-1,"/prm/st/r_st",_("Running"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/r_st")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(run_st?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	atoi(opt->text().c_str())?modStart():modStop();
    }
    else TModule::cntrCmdProc(opt);
}
