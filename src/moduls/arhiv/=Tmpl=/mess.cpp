
//OpenSCADA module Archive.Tmpl file: mess.cpp
/***************************************************************************
 *   Copyright (C) 2022 by MyName MyFamily, <my@email.org>                 *
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

// System includings - add need ones
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

// OpenSCADA API includings - add need ones
#include <tsys.h>

// Own includings of the module - add need ones
#include "mess.h"

using namespace ArhTmpl;

//************************************************
//* ArhTmpl::ModMArch - Message archiver         *
//************************************************
ModMArch::ModMArch( const string &iid, const string &idb, TElem *cf_el ) :
    TMArchivator(iid,idb,cf_el), mBeg(0), mEnd(0)
{
    //???? Set here the default message archiver address
    setAddr("*.*");
}

ModMArch::~ModMArch( )
{
    try{ stop(); } catch(...) { }
}

void ModMArch::postDisable( int flag )
{
    TMArchivator::postDisable( flag );

    if(flag&NodeRemove) {
	//???? Code of removing the archiver data
    }
}

void ModMArch::load_( )
{
    //TMArchivator::load_();

    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	//vl = prmNd.attr("XML"); if(!vl.empty()) setUseXML(s2i(vl));
	//???? Append loading the additional configuration attributes
    } catch(...) { }
}

void ModMArch::save_( )
{
    XMLNode prmNd("prms");
    //prmNd.setAttr("XML", i2s(useXML()));
    //???? Append saving the additional configuration attributes
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TMArchivator::save_();
}

void ModMArch::start( )
{
    //???? Prepare own structures and check available storages

    TMArchivator::start();
}

void ModMArch::stop( )
{
    TMArchivator::stop();

    //???? Releasing own structures
}

bool ModMArch::put( vector<TMess::SRec> &mess, bool force )
{
    TMArchivator::put(mess, force);	//Allow redundancy

    if(!runSt) throw err_sys(_("Archive is not started!"));

    //???? Code of placing the messages group in the module specific storage
}

time_t ModMArch::get( time_t bTm, time_t eTm, vector<TMess::SRec> &mess, const string &category, char level, time_t upTo )
{
    if(!runSt) throw err_sys(_("Archive is not started!"));

    //???? Code of getting the messages from the module specific storage

    return 0;
}

void ModMArch::cntrCmdProc( XMLNode *opt )
{
    //???? Change and append for your specific configuration

    //Getting the page info
    if(opt->name() == "info") {
	TMArchivator::cntrCmdProc(opt);
	//ctrMkNode("fld",opt,-1,"/prm/st/tarch",_("Archiving time (msek)"),R_R_R_,"root",SARH_ID,1,"tp","real");
	//ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",SARH_ID,3,
	//    "dest","select","select","/db/list:onlydb","help",TMess::labStor(true).c_str());
	//ctrMkNode("fld",opt,-1,"/prm/cfg/sz",_("Archive size (hours)"),RWRWR_,"root",SARH_ID,1,"tp","real");
	return;
    }
    //Processing for commands to the page
    string a_path = opt->attr("path");
    /*if(a_path == "/prm/st/tarch" && ctrChkNode(opt))	opt->setText(r2s(tm_calc));
    else if(a_path == "/prm/cfg/sz")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(maxSize()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMaxSize(s2r(opt->text()));
    }
    else*/ TMArchivator::cntrCmdProc(opt);
}
