
//OpenSCADA system module DAQ.System file: da_smart.cpp
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <tsys.h>

#include "os_contr.h"
#include "da_smart.h"

#define SCSI_MAJOR 8

using namespace SystemCntr;

const char *HddSmart::smartval_cmd = "smartctl -A -v N,raw48 %s 2> /dev/null";

//*************************************************
//* HddSmart                                      *
//*************************************************
HddSmart::HddSmart( )	{ }

HddSmart::~HddSmart( )	{ }

void HddSmart::init( TMdPrm *prm )
{
    prm->daData = new tval();
    prm->vlElemAtt(&((tval*)prm->daData)->els);

    TCfg &c_subt = prm->cfg("SUBT");

    //Create Configuration
    c_subt.fld().setDescr(_("Disk"));

    vector<string> list;
    dList(list);
    string dls;
    for(unsigned i_l = 0; i_l < list.size(); i_l++)
	dls += list[i_l]+";";
    c_subt.fld().setValues(dls);
    c_subt.fld().setSelNames(dls);

    if(list.size() && !TRegExp("(^|;)"+c_subt.getS()+";").test(dls)) c_subt.setS(list[0]);
}

void HddSmart::deInit( TMdPrm *prm )
{
    prm->vlElemDet(&((tval*)prm->daData)->els);
    delete (tval*)prm->daData;
    prm->daData = NULL;
}

void HddSmart::dList( vector<string> &list, bool part )
{
    int major, minor;
    char name[11];
    char buf[256];

    FILE *f = fopen("/proc/partitions","r");
    if(f == NULL) return;

    while(fgets(buf,sizeof(buf),f) != NULL) {
	if(sscanf(buf,"%d %d %*d %10s",&major,&minor,name) != 3) continue;
	if(!part && ((major != SCSI_MAJOR && minor != 0) || (major == SCSI_MAJOR && (minor%16)) || !strncmp(name,"md",2)))
	    continue;

	string cmd = TSYS::strMess(smartval_cmd,(string("/dev/")+name+((major==SCSI_MAJOR)?" -d ata":"")).c_str());
	FILE *fp = popen(cmd.c_str(),"r");
	if(fp) {
	    int val;
	    bool access_true = false;
	    while(fgets(buf,sizeof(buf),fp) != NULL) {
		if(sscanf(buf,"%*d %*s %*x %*d %*d %*d %*s %*s %*s %d\n",&val) != 1) continue;
		access_true = true;
		break;
	    }
	    pclose(fp);
	    if(access_true) list.push_back(name);
	}
    }
    fclose(f);
}

void HddSmart::getVal( TMdPrm *prm )
{
    int id;
    unsigned long val;
    char buf[256];
    char name[31];
    bool devOK = false;

    string dev = prm->cfg("SUBT").getS();

    //SMART attributes
    string cmd = TSYS::strMess(smartval_cmd,("/dev/"+dev+((dev.size()&&dev[0]=='s')?" -d ata":"")).c_str());
    FILE *fp = popen(cmd.c_str(),"r");
    while(fp && fgets(buf,sizeof(buf),fp) != NULL) {
	if(sscanf(buf,"%d %30s %*x %*d %*d %*d %*s %*s %*s %lu\n",&id,name,&val) != 3) continue;
	string s_id = i2s(id);
	if(!prm->vlPresent(s_id)) ((tval*)prm->daData)->els.fldAdd(new TFld(s_id.c_str(),name,TFld::Integer,TFld::NoWrite));
	prm->vlAt(s_id).at().setI(val,0,true);
	devOK = true;
    }
    if(fp) fclose(fp);

    if(devOK) prm->daErr = "";
    else if(!prm->daErr.getVal().size()) {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}

void HddSmart::makeActiveDA( TMdContr *aCntr )
{
    string ap_nm = "Smart_";

    vector<string> list;
    dList(list);
    for(unsigned i_hd = 0; i_hd < list.size(); i_hd++) {
	vector<string> pLs;
	// Find propper parameter's object
	aCntr->list(pLs);

	unsigned i_p;
	for(i_p = 0; i_p < pLs.size(); i_p++) {
	    AutoHD<TMdPrm> p = aCntr->at(pLs[i_p]);
	    if(p.at().cfg("TYPE").getS() == id() && p.at().cfg("SUBT").getS() == list[i_hd])	break;
	}
	if(i_p < pLs.size()) continue;

	string hddprm = ap_nm+list[i_hd];
	while(aCntr->present(hddprm)) hddprm = TSYS::strLabEnum(hddprm);
	aCntr->add(hddprm,0);
	AutoHD<TMdPrm> dprm = aCntr->at(hddprm);
	dprm.at().setName(_("HDD SMART: ")+list[i_hd]);
	dprm.at().autoC(true);
	dprm.at().cfg("TYPE").setS(id());
	dprm.at().cfg("SUBT").setS(list[i_hd]);
	dprm.at().cfg("EN").setB(true);
	if(aCntr->enableStat()) dprm.at().enable();
    }
}
