
//OpenSCADA system module DAQ.System file: da_hddstat.cpp
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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
#include <string.h>

#include <tsys.h>

#include "os_contr.h"
#include "da_hddstat.h"

#define SCSI_MAJOR 8

using namespace SystemCntr;

//*************************************************
//* HddStat                                       *
//*************************************************
HddStat::HddStat( )
{
    fldAdd(new TFld("rd",_("Read (B)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("rdSp",_("Read speed (B/s)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("wr",_("Write (B)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("wrSp",_("Write speed (B/s)"),TFld::Real,TFld::NoWrite));
}

HddStat::~HddStat( )
{

}

void HddStat::init( TMdPrm *prm )
{
    TCfg &c_subt = prm->cfg("SUBT");

    //> Create Configuration
    c_subt.fld().setDescr(_("Disk(part)"));

    vector<string> list;
    dList(list,true);
    string dls;
    for(unsigned i_l = 0; i_l < list.size(); i_l++)
	dls += list[i_l]+";";
    c_subt.fld().setValues(dls);
    c_subt.fld().setSelNames(dls);

    if(list.size() && !TRegExp("(^|;)"+c_subt.getS()+";").test(dls)) c_subt.setS(list[0]);
}

void HddStat::dList( vector<string> &list, bool part )
{
    int major, minor;
    char name[11];
    char buf[256];

    FILE *f = fopen("/proc/partitions","r");
    while(f && fgets(buf,sizeof(buf),f) != NULL)
    {
	if(sscanf(buf,"%d %d %*d %10s",&major,&minor,name) != 3) continue;
	if(!part && ((major != SCSI_MAJOR && minor != 0) || (major == SCSI_MAJOR && (minor%16)) || !strncmp(name,"md",2)))
	    continue;
	list.push_back(name);
    }
    if(f) fclose(f);
}

void HddStat::getVal( TMdPrm *prm )
{
    unsigned long rd, rd1, wr, wr1;
    double rdVl = 0, wrVl = 0;
    char sc_pat[50], buf[256];
    FILE *f = NULL;
    bool devOK = false;

    string dev = prm->cfg("SUBT").getS();
    if((f=fopen("/proc/diskstats","r")))
    {
	//major minor name rio rmerge rsect ruse wio wmerge wsect wuse running use aveq
	//--or for a partition--
	//major minor name rio rsect wio wsect
	snprintf(sc_pat, sizeof(sc_pat), "%%*d %%*d %s %%*d %%lu %%lu %%lu %%*d %%*d %%lu", dev.c_str());
	for(int n; fgets(buf,sizeof(buf),f) != NULL; )
	{
	    if(!(n=sscanf(buf,sc_pat,&rd,&rd1,&wr,&wr1))) continue;
	    if(n == 4)	{ rd = rd1; wr = wr1; }
	    rdVl = (double)rd*512;
	    wrVl = (double)wr*512;
	    devOK = true;
	    break;
	}
	fclose(f);
    }
    if(!devOK && (f=fopen("/proc/partitions","r")))
    {
	//major minor #blocks name rio rmerge rsect ruse wio wmerge wsect wuse running use aveq
	snprintf(sc_pat,sizeof(sc_pat),"%%*d %%*d %%*d %s %%*d %%*d %%lu %%*d %%*d %%*d %%lu",dev.c_str());
	while(fgets(buf,sizeof(buf),f) != NULL)
	    if(sscanf(buf,sc_pat,&rd,&wr) == 2)
	    {
		rdVl = (double)rd*512;
		wrVl = (double)wr*512;
		devOK = true;
		break;
	    }
	fclose(f);
    }

    if(devOK)
    {
	prm->daErr = "";
	double lstVl = prm->vlAt("rd").at().getR(0, true);
	prm->vlAt("rdSp").at().setR((lstVl != EVAL_REAL && rdVl > lstVl) ?
		1e6*(rdVl-lstVl)/vmax(1,TSYS::curTime()-prm->vlAt("rd").at().time()) : 0, 0, true);
	lstVl = prm->vlAt("wr").at().getR(0, true);
	prm->vlAt("wrSp").at().setR((lstVl != EVAL_REAL && wrVl > lstVl) ?
		1e6*(wrVl-lstVl)/vmax(1,TSYS::curTime()-prm->vlAt("wr").at().time()) : 0, 0, true);

	prm->vlAt("rd").at().setR(rdVl, 0, true);
	prm->vlAt("wr").at().setR(wrVl, 0, true);
    }
    else if(!prm->daErr.getVal().size())
    {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}

void HddStat::makeActiveDA( TMdContr *aCntr )
{
    string ap_nm = "Statistic_";

    vector<string> list;
    dList(list,true);
    for(unsigned i_hd = 0; i_hd < list.size(); i_hd++)
    {
	vector<string> pLs;
	// Find propper parameter's object
	aCntr->list(pLs);

	unsigned i_p;
	for(i_p = 0; i_p < pLs.size(); i_p++)
	{
	    AutoHD<TMdPrm> p = aCntr->at(pLs[i_p]);
	    if(p.at().cfg("TYPE").getS() == id() && p.at().cfg("SUBT").getS() == list[i_hd])	break;
	}
	if(i_p < pLs.size()) continue;

	string hddprm = ap_nm+list[i_hd];
	while(aCntr->present(hddprm)) hddprm = TSYS::strLabEnum(hddprm);
	aCntr->add(hddprm, 0);
	AutoHD<TMdPrm> dprm = aCntr->at(hddprm);
	dprm.at().setName(_("HD statistic: ")+list[i_hd]);
	dprm.at().autoC(true);
	dprm.at().cfg("TYPE").setS(id());
	dprm.at().cfg("SUBT").setS(list[i_hd]);
	dprm.at().cfg("EN").setB(true);
	if(aCntr->enableStat()) dprm.at().enable();
    }
}
