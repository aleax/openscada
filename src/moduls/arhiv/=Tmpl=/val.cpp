
//OpenSCADA module Archive.Tmpl file: val.cpp
/***************************************************************************
 *   Copyright (C) 2022 by MyName MyFamily, <my@email.org>                 *
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

// System includings - add need ones
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

// OpenSCADA API includings - add need ones
#include <tsys.h>
#include <tmess.h>

// Own includings of the module - add need ones
#include "val.h"

using namespace ArhTmpl;

//*************************************************
//* ArhTmpl::ModVArch - Value archiver            *
//*************************************************
ModVArch::ModVArch( const string &iid, const string &idb, TElem *cf_el ) : TVArchivator(iid,idb,cf_el)
{
    //???? Set here the default value archiver address and the default selection priority
    setSelPrior(1);
    setAddr("*.*");
}

ModVArch::~ModVArch( )
{
    try{ stop(); } catch(...) { }
}

void ModVArch::load_( )
{
    //TVArchivator::load_();

    try {
	XMLNode prmNd;
	string  vl;
	prmNd.load(cfg("A_PRMS").getS());
	//vl = prmNd.attr("XML"); if(!vl.empty()) setUseXML(s2i(vl));
	//???? Append loading the additional configuration attributes
    } catch(...) { }
}

void ModVArch::save_( )
{
    XMLNode prmNd("prms");
    //prmNd.setAttr("XML", i2s(useXML()));
    //???? Append saving the additional configuration attributes
    cfg("A_PRMS").setS(prmNd.save(XMLNode::BrAllPast));

    TVArchivator::save_();
}

void ModVArch::start( )
{
    //???? Prepare own structures and check available storages

    TVArchivator::start();
}

void ModVArch::stop( bool full_del )
{
    TVArchivator::stop(full_del);

    //???? Releasing own structures
}

TVArchEl *ModVArch::getArchEl( TVArchive &arch ) { return new ModVArchEl(arch,*this); }

void ModVArch::cntrCmdProc( XMLNode *opt )
{
    //???? Change and append for your specific configuration

    //Getting the page info
    if(opt->name() == "info") {
	TVArchivator::cntrCmdProc(opt);
	//ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",SARH_ID,3,
	//    "dest","select","select","/db/list:onlydb","help",TMess::labStor(true).c_str());
	//ctrMkNode("fld",opt,-1,"/prm/cfg/sz",_("Archive size (hours)"),RWRWR_,"root",SARH_ID,1,"tp","real");
	return;
    }
    //Processing for commands to the page
    string a_path = opt->attr("path");
    /*if(a_path == "/prm/cfg/sz") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SARH_ID,SEC_RD))	opt->setText(r2s(maxSize()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SARH_ID,SEC_WR))	setMaxSize(s2r(opt->text()));
    }
    else*/ TVArchivator::cntrCmdProc(opt);
}

//*************************************************
//* ArhTmpl::ModVArchEl - Value archive element   *
//*************************************************
ModVArchEl::ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator ) :
    TVArchEl(iachive,iarchivator), mBeg(0), mEnd(0), mPer(0)
{

}

ModVArchEl::~ModVArchEl( )
{

}

void ModVArchEl::getValsProc( TValBuf &buf, int64_t ibeg, int64_t iend )
{
    //???? Code of getting the values from the module specific storage
}

TVariant ModVArchEl::getValProc( int64_t *tm, bool up_ord )
{
    //???? Code of getting one value from the module specific storage
}

int64_t ModVArchEl::setValsProc( TValBuf &buf, int64_t beg, int64_t end, bool toAccum )
{
    //???? Code of setting the values to the module specific storage
}
