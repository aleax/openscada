
//OpenSCADA file: tspecials.cpp
/***************************************************************************
 *   Copyright (C) 2003-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#include "tsys.h"
#include "tmess.h"
#include "tspecials.h"

using namespace OSCADA;

//*************************************************
//* TSpecialS                                     *
//*************************************************
TSpecialS::TSpecialS( ) : TSubSYS(SSPC_ID, true)
{

}

string TSpecialS::optDescr( )
{
    return TSYS::strMess(_(
	"======================= Subsystem \"Specials\" options =====================\n\n")) + TSubSYS::optDescr();
}

void TSpecialS::load_( )
{
    //Load parameters from command line

    //Load parameters from config-file
}

void TSpecialS::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TSubSYS::cntrCmdProc(opt);
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    TSubSYS::cntrCmdProc(opt);
}

//*************************************************
//* TSpecial                                      *
//*************************************************
TSpecial::TSpecial( const string &id ) : TModule(id), runSt(false)
{

}

void TSpecial::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TModule::cntrCmdProc(opt);
	ctrMkNode("area",opt,0,"/prm",_("Special"));
	ctrMkNode("area",opt,-1,"/prm/st",_("State"));
	ctrMkNode("fld",opt,-1,"/prm/st/st",_("Running"),RWRWR_,"root",SSPC_ID,1,"tp","bool");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SSPC_ID,SEC_RD))	opt->setText(runSt?"1":"0");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SSPC_ID,SEC_WR))	s2i(opt->text()) ? modStart() : modStop();
    }
    else TModule::cntrCmdProc(opt);
}
