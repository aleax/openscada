
//OpenSCADA system module DAQ.System file: da_fs.cpp
/***************************************************************************
 *   Copyright (C) 2016 by Roman Savochenko, <rom_as@oscada.org>           *
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

#include <sys/vfs.h>
#include <string.h>

#include <tsys.h>

#include "os_contr.h"
#include "da_fs.h"

using namespace SystemCntr;

//*************************************************
//* FS                                            *
//*************************************************
FS::FS( )
{
    fldAdd(new TFld("total",_("Total (GB)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("used",_("Used (GB)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("free",_("Free (GB)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("totalN",_("Total file nodes"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("usedN",_("Used file nodes"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("freeN",_("Free file nodes"),TFld::Integer,TFld::NoWrite));
}

FS::~FS( )
{

}

void FS::init( TMdPrm *prm )
{
    TCfg &cSubt = prm->cfg("SUBT");

    //Create Configuration
    cSubt.fld().setDescr(_("Mount point"));

    vector<string> list;
    mpList(list);
    string mpls;
    for(unsigned iL = 0; iL < list.size(); iL++) mpls += list[iL]+";";
    cSubt.fld().setValues(mpls);
    cSubt.fld().setSelNames(mpls);

    //if(list.size() && !TRegExp("(^|;)"+cSubt.getS()+";").test(mpls)) cSubt.setS(list[0]);
}

void FS::mpList( vector<string> &list )
{
    char name[512];
    char buf[1024], *s;

    FILE *f = fopen("/etc/fstab", "r");
    while(f && fgets(buf,sizeof(buf),f) != NULL) {
	s = buf;
	while(isblank(*s)) ++s;	//Walk through blank in begin
	if(*s == '\0' || *s == '#' || *s == '\n') continue;	//Pass comments and wrong lines
	name[0] = 0;
	sscanf(s, "%*s %511s %*s %*s", name);
	if(name[0] == 0 || !strcmp(name, "devpts") || !strcmp(name, "swap") || !strcmp(name, "proc") ||
		!strcmp(name, "sysfs") || !strcmp(name, "usbdevfs") || !strcmp(name, "usbfs") || !strcmp(name, "ignore"))
	    continue;
	list.push_back(name);
    }

    if(f) fclose(f);
}

void FS::getVal( TMdPrm *prm )
{
    struct statfs	st;
    bool mOK = !statfs(prm->cfg("SUBT").getS().c_str(), &st);

    if(mOK) {
	prm->daErr = "";
	prm->vlAt("total").at().setR(1e-9*st.f_blocks*st.f_bsize, 0, true);
	prm->vlAt("used").at().setR(1e-9*(st.f_blocks-st.f_bfree)*st.f_bsize, 0, true);
	prm->vlAt("free").at().setR(1e-9*st.f_bfree*st.f_bsize, 0, true);
	prm->vlAt("totalN").at().setI(st.f_files, 0, true);
	prm->vlAt("usedN").at().setI(st.f_files-st.f_ffree, 0, true);
	prm->vlAt("freeN").at().setI(st.f_ffree, 0, true);
    }
    else if(!prm->daErr.getVal().size()) {
	prm->setEval();
	prm->daErr = _("10:Mountpoint isn't available.");
    }
}

void FS::makeActiveDA( TMdContr *aCntr )
{
    string ap_nm = "FS";

    vector<string> list;
    mpList(list);
    for(unsigned iHD = 0; iHD < list.size(); iHD++) {
	vector<string> pLs;

	//Find propper parameter's object
	aCntr->list(pLs);

	unsigned iP;
	for(iP = 0; iP < pLs.size(); iP++) {
	    AutoHD<TMdPrm> p = aCntr->at(pLs[iP]);
	    if(p.at().cfg("TYPE").getS() == id() && p.at().cfg("SUBT").getS() == list[iHD])	break;
	}
	if(iP < pLs.size()) continue;

	string fsprm = ap_nm + TSYS::strEncode(list[iHD], TSYS::oscdID);
	while(aCntr->present(fsprm)) fsprm = TSYS::strLabEnum(fsprm);
	aCntr->add(fsprm, 0);
	AutoHD<TMdPrm> dprm = aCntr->at(fsprm);
	dprm.at().setName(TSYS::strMess(_("File system: '%s'"),list[iHD].c_str()));
	dprm.at().autoC(true);
	dprm.at().cfg("TYPE").setS(id());
	dprm.at().cfg("SUBT").setS(list[iHD]);
	dprm.at().cfg("EN").setB(true);
	if(aCntr->enableStat()) dprm.at().enable();
    }
}
