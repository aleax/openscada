
//!!! The module name, the file name and the module's license. Change for your need.
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

//!!! System's includings. Add need for your module includings.
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tsys.h>

//!!! Self your module's includings. Add need for your module includings.
#include "arch.h"
#include "mess.h"

//!!! Include for default call into your module's namespace.
using namespace ModTmpl;

//************************************************
//* ModTmpl::ModMArch - Messages archivator       *
//************************************************
//!!! Constructor for the object. Append into for your need.
ModMArch::ModMArch( const string &iid, const string &idb, TElem *cf_el ) :
    TMArchivator(iid,idb,cf_el), mBeg(0), mEnd(0)
{
    setAddr("*.*");
}

//!!! Destructor for the object.
ModMArch::~ModMArch( )
{
    try{ stop(); } catch(...) { }
}

//!!! Module's post disable call. Place here for your data remove at the object remove
void ModMArch::postDisable( int flag )
{
    TMArchivator::postDisable( flag );

    if(flag&NodeRemove) {
	//Remove info record
	TConfig cfg(&mod->archEl());
	cfg.cfg("TBL").setS(archTbl(),true);
	TBDS::dataDel(addr()+"."+mod->mainTbl(), "", cfg);
    }
}

//!!! Inherited (virtual) load object's node methods. Place here for object's data loading
void ModMArch::load_( )
{
    TMArchivator::load_();

    //> Init address to DB
    if( addr().empty() ) setAddr("*.*");

    //> Load message archive parameters
    TConfig cfg(&mod->archEl());
    cfg.cfg("TBL").setS(archTbl());
    if(TBDS::dataGet(addr()+"."+mod->mainTbl(),"",cfg,TBDS::NoException)) {
	mBeg = atoi(cfg.cfg("BEGIN").getS().c_str());
	mEnd = atoi(cfg.cfg("END").getS().c_str());
    }

    //!!! Addon parameters of the object loading.
    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(mAPrms);
	vl = prmNd.attr("Size"); if( !vl.empty() ) setMaxSize(s2r(vl));
    } catch(...) { }
}

//!!! Inherited (virtual) save object's node methods. Place here for object's data saving
void ModMArch::save_( )
{
    //!!! Addon parameters of the object saving.
    XMLNode prmNd("prms");
    prmNd.setAttr("Size", r2s(maxSize()));
    mAPrms = prmNd.save(XMLNode::BrAllPast);

    TMArchivator::save_();
}

//!!! Inherited (virtual) start archivator functions.
void ModMArch::start( )
{
    //> Connection to DB and enable status check
    string wdb = TBDS::realDBName(addr());
    AutoHD<TBD> db = SYS->db().at().nodeAt(wdb,0,'.');
    if( !db.at().enableStat( ) ) db.at().enable();

    runSt = true;
}

//!!! Inherited (virtual) stop archivator functions.
void ModMArch::stop( )
{
    runSt = false;
}

//!!! Put data virtual functions for place data to archive.
void ModMArch::put( vector<TMess::SRec> &mess )
{
    if(!runSt) throw TError(nodePath().c_str(),_("Archive is not started!"));

    //!!! Code for place data to archive.
}

//!!! Get data virtual functions for request data from archive.
void ModMArch::get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category, char level )
{
    if(!runSt) throw TError(nodePath().c_str(),_("Archive is not started!"));

    //!!! Code for request data from archive.
}

//!!! OpenSCADA control interface comands process virtual function.
//!!! For example, process access from standard configurators of OpenSCADA to individual module's parameters.
//!!! Modify for self needs.
void ModMArch::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TMArchivator::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/st/tarch",_("Archiving time (msek)"),R_R_R_,"root",SARH_ID,1,"tp","real");
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",SARH_ID,3,
	    "dest","select","select","/db/list:onlydb","help",TMess::labStor(true).c_str());
	ctrMkNode("fld",opt,-1,"/prm/cfg/sz",_("Archive size (hours)"),RWRWR_,"root",SARH_ID,1,"tp","real");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/tarch" && ctrChkNode(opt))	opt->setText(r2s(tm_calc));
    else if(a_path == "/prm/cfg/sz")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(maxSize()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMaxSize(s2r(opt->text()));
    }
    else TMArchivator::cntrCmdProc(opt);
}
