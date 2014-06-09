
//OpenSCADA system module DAQ.System file: da_netstat.cpp
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
#include "da_netstat.h"

using namespace SystemCntr;

//*************************************************
//* HddStat                                       *
//*************************************************
NetStat::NetStat( )
{
    fldAdd(new TFld("rcv",_("Receive (B)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("rcvSp",_("Receive speed (B/s)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("trns",_("Transmit (B)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("trnsSp",_("Transmit speed (B/s)"),TFld::Real,TFld::NoWrite));
}

NetStat::~NetStat( )
{

}

void NetStat::init( TMdPrm *prm )
{
    TCfg &c_subt = prm->cfg("SUBT");

    //> Create Configuration
    c_subt.fld().setDescr(_("Interface"));

    vector<string> list;
    dList(list,true);
    string ifls;
    for(unsigned i_l = 0; i_l < list.size(); i_l++)
	ifls += list[i_l]+";";
    c_subt.fld().setValues(ifls);
    c_subt.fld().setSelNames(ifls);

    if(list.size() && !TRegExp("(^|;)"+c_subt.getS()+";").test(ifls)) c_subt.setS(list[0]);
}

void NetStat::dList( vector<string> &list, bool part )
{
    long unsigned int rcv, trns;
    char name[11] = "",
	 buf[256] = "";

    FILE *f = fopen("/proc/net/dev","r");
    while(f && fgets(buf,sizeof(buf),f) != NULL)
    {
	for(unsigned i=0; i < sizeof(buf); i++)
	    if(buf[i] == ':') buf[i] = ' ';
	if(sscanf(buf,"%10s %lu %*d %*d %*d %*d %*d %*d %*d %lu",name,&rcv,&trns) != 3) continue;
	list.push_back(name);
    }
    if(f) fclose(f);
}

void NetStat::getVal( TMdPrm *prm )
{
    unsigned long rcv, trns;
    double rcvVl = 0, trnsVl = 0;
    char sc_pat[50] = "",
	 buf[256] = "";
    bool devOK = false;

    string dev = prm->cfg("SUBT").getS();
    FILE *f = fopen("/proc/net/dev","r");
    if(f)
    {
	snprintf(sc_pat,sizeof(sc_pat)," %s %%lu %%*d %%*d %%*d %%*d %%*d %%*d %%*d %%lu",dev.c_str());
	while(fgets(buf,sizeof(buf),f) != NULL)
	{
	    for(unsigned i = 0; i < sizeof(buf); i++)
		if(buf[i] == ':') buf[i] = ' ';
	    if(!sscanf(buf,sc_pat,&rcv,&trns)) continue;
	    rcvVl = rcv;
	    trnsVl = trns;
	    devOK = true;
	    break;
	}
	fclose(f);
    }

    if(devOK)
    {
	prm->daErr = "";
	double lstVl = prm->vlAt("rcv").at().getR(0, true);
	prm->vlAt("rcvSp").at().setR((lstVl != EVAL_REAL && rcvVl > lstVl) ?
		1e6*(rcvVl-lstVl)/vmax(1,TSYS::curTime()-prm->vlAt("rcv").at().time()) : 0, 0, true);
	lstVl = prm->vlAt("trns").at().getR(0, true);
	prm->vlAt("trnsSp").at().setR((lstVl != EVAL_REAL && trnsVl > lstVl) ?
		1e6*(trnsVl-lstVl)/vmax(1,TSYS::curTime()-prm->vlAt("trns").at().time()) : 0, 0, true);

	prm->vlAt("rcv").at().setR(rcvVl,0,true);
	prm->vlAt("trns").at().setR(trnsVl,0,true);
    }
    else if(!prm->daErr.getVal().size())
    {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}

void NetStat::makeActiveDA( TMdContr *aCntr )
{
    string ap_nm = "Interface_";

    vector<string> list;
    dList(list);
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

	string intprm = ap_nm+list[i_hd];
	while(aCntr->present(intprm)) intprm = TSYS::strLabEnum(intprm);
	aCntr->add(intprm, 0);
	AutoHD<TMdPrm> dprm = aCntr->at(intprm);
	dprm.at().setName(_("Interface statistic: ")+list[i_hd]);
	dprm.at().autoC(true);
	dprm.at().cfg("TYPE").setS(id());
	dprm.at().cfg("SUBT").setS(list[i_hd]);
	dprm.at().cfg("EN").setB(true);
	if(aCntr->enableStat()) dprm.at().enable();
    }
}
