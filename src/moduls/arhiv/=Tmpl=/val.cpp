
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module Archive.Tmpl file: val.cpp
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily                                 *
 *   my@email.org                                                          *
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

//!!! System's includings. Add need for your module includings.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <math.h>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tsys.h>
#include <tmess.h>

//!!! Self your module's includings. Add need for your module includings.
#include "arch.h"
#include "val.h"

//!!! Include for default call into your module's namespace.
using namespace ModTmpl;

//*************************************************
//* ModTmpl::ModVArch - Value archivator          *
//*************************************************
//!!! Constructor for the object. Append into for your need.
ModVArch::ModVArch( const string &iid, const string &idb, TElem *cf_el ) : TVArchivator(iid,idb,cf_el)
{
    setAddr("*.*");
}

//!!! Destructor for the object.
ModVArch::~ModVArch( )
{
    try{ stop(); }catch(...){}
}

//!!! Inherited (virtual) load object's node methods. Place here for object's data loading
void ModVArch::load_( )
{
    TVArchivator::load_();

    if( addr().empty() ) setAddr("*.*");

    //!!! Addon parameters of the object loading.
    try
    {
	XMLNode prmNd;
	string  vl;
	prmNd.load(mAPrms);
	vl = prmNd.attr("Size"); if( !vl.empty() ) setMaxSize(atof(vl.c_str()));
    } catch(...){ }
}

//!!! Inherited (virtual) save object's node methods. Place here for object's data saving
void ModVArch::save_( )
{
    //!!! Addon parameters of the object saving.
    XMLNode prmNd("prms");
    prmNd.setAttr("Size",TSYS::real2str(maxSize()));
    mAPrms = prmNd.save(XMLNode::BrAllPast);

    TVArchivator::save_();
}

//!!! Inherited (virtual) start archivator functions.
void ModVArch::start()
{
    //> Connection to DB and enable status check
    string wdb = TBDS::realDBName(addr());
    AutoHD<TBD> db = SYS->db().at().nodeAt(wdb,0,'.');
    if( !db.at().enableStat( ) ) db.at().enable();

    //> Start getting data cycle
    TVArchivator::start();
}

//!!! Inherited (virtual) stop archivator functions.
void ModVArch::stop()
{
    //> Stop getting data cicle an detach archives
    TVArchivator::stop();
}

//!!! Inherited (virtual) function for create self value archive element. Don't touch the function.
TVArchEl *ModVArch::getArchEl( TVArchive &arch )
{
    return new ModVArchEl(arch,*this);
}

//!!! OpenSCADA control interface comands process virtual function.
//!!! For example, process access from standard configurators of OpenSCADA to individual module's parameters.
//!!! Modify for self needs.
void ModVArch::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TVArchivator::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",SARH_ID,3,
	    "dest","select","select","/db/list","help",TMess::labDB());
	ctrMkNode("fld",opt,-1,"/prm/cfg/sz",_("Archive size (hours)"),RWRWR_,"root",SARH_ID,1,"tp","real");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/sz")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(TSYS::real2str(maxSize()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMaxSize(atof(opt->text().c_str()));
    }
    else TVArchivator::cntrCmdProc(opt);
}

//*************************************************
//* ModTmpl::ModVArchEl - Value archive element   *
//*************************************************
//!!! Constructor for the object. Append into for your need.
ModVArchEl::ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator ) :
    TVArchEl(iachive,iarchivator), mBeg(0), mEnd(0), mPer(0)
{
    //> Load message archive parameters
    TConfig cfg(&mod->archEl());
    cfg.cfg("TBL").setS(archTbl());
    if(SYS->db().at().dataGet(archivator().addr()+"."+mod->mainTbl(),"",cfg))
    {
	mBeg = strtoll(cfg.cfg("BEGIN").getS().c_str(),NULL,10);
	mEnd = strtoll(cfg.cfg("END").getS().c_str(),NULL,10);
	mPer = strtoll(cfg.cfg("PRM1").getS().c_str(),NULL,10);
    }
    if( !mPer ) mPer = (int64_t)(archivator().valPeriod()*1000000.);
}

//!!! Destructor for the object.
ModVArchEl::~ModVArchEl( )
{

}

//!!! Get values block virtual functions for access to archive.
void ModVArchEl::getValsProc( TValBuf &buf, int64_t ibegIn, int64_t iendIn )
{
    //!!! Code for request values block from archive.
}

//!!! Get value virtual functions for access to archive.
TVariant ModVArchEl::getValProc( int64_t *tm, bool up_ord )
{
    //!!! Code for request value from archive.
}

//!!! Set values block virtual functions for place data to archive.
void ModVArchEl::setValsProc( TValBuf &buf, int64_t beg, int64_t end )
{
    //!!! Code for set values block to archive.
}
