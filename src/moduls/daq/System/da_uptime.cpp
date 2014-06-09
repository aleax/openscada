
//OpenSCADA system module DAQ.System file: da_uptime.cpp
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

#include <sys/times.h>

#include <tsys.h>

#include "os_contr.h"
#include "da_uptime.h"

using namespace SystemCntr;

//*************************************************
//* UpTime                                        *
//*************************************************
UpTime::UpTime( )
{
    st_tm = time(NULL);

    //> Uptime value structure
    fldAdd(new TFld("full",_("Full seconds"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("sec",_("Seconds"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("min",_("Minutes"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("hour",_("Hours"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("day",_("Days"),TFld::Integer,TFld::NoWrite));
}

UpTime::~UpTime( )
{

}

void UpTime::init( TMdPrm *prm )
{
    //> Create config
    TCfg &c_subt = prm->cfg("SUBT");
    c_subt.fld().setDescr("");

    c_subt.fld().setValues("sys;stat");
    c_subt.fld().setSelNames(string(_("System"))+";"+_("Station"));

    if(!TRegExp("(^|;)"+c_subt.getS()+";").test(c_subt.fld().values())) c_subt.setS("sys");
}

void UpTime::getVal( TMdPrm *prm )
{
    unsigned long val;
    bool devOK = false;

    string trg = prm->cfg("SUBT").getS();

    if(trg == "sys")
    {
	FILE *f = fopen("/proc/uptime","r");
	if(f == NULL) return;
	devOK = (fscanf(f,"%lu",&val) == 1);
	fclose(f);
    }
    else { val = time(NULL) - st_tm; devOK = true; }

    if(devOK)
    {
	prm->daErr = "";
	prm->vlAt("full").at().setI(val, 0, true);
	prm->vlAt("day").at().setI(val/86400, 0, true);
	prm->vlAt("hour").at().setI((val%86400)/3600, 0, true);
	prm->vlAt("min").at().setI(((val%86400)%3600)/60, 0, true);
	prm->vlAt("sec").at().setI(((val%86400)%3600)%60, 0, true);
    }
    else if(!prm->daErr.getVal().size())
    {
        prm->setEval();
        prm->daErr = _("10:Device is not available.");
    }
}

void UpTime::makeActiveDA( TMdContr *aCntr )
{
    vector<string> pLs;
    aCntr->list(pLs);

    unsigned i_p;
    for(i_p = 0; i_p < pLs.size(); i_p++)
    {
	AutoHD<TMdPrm> p = aCntr->at(pLs[i_p]);
	if(p.at().cfg("TYPE").getS() == id() && p.at().cfg("SUBT").getS() == "sys") break;
    }
    if(i_p >= pLs.size())
    {
	string ap_nm = "UpTimeSystem";
	while(aCntr->present(ap_nm)) ap_nm = TSYS::strLabEnum(ap_nm);
	FILE *f = fopen("/proc/uptime","r");
	if(f != NULL)
	{
	    aCntr->add(ap_nm, 0);
	    AutoHD<TMdPrm> dprm = aCntr->at(ap_nm);
	    dprm.at().setName(_("System up time"));
	    dprm.at().autoC(true);
	    dprm.at().cfg("TYPE").setS(id());
	    dprm.at().cfg("SUBT").setS("sys");
	    dprm.at().cfg("EN").setB(true);
	    if(aCntr->enableStat()) dprm.at().enable();
	    fclose(f);
	}
    }

    for(i_p = 0; i_p < pLs.size(); i_p++)
    {
	AutoHD<TMdPrm> p = aCntr->at(pLs[i_p]);
	if(p.at().cfg("TYPE").getS() == id() && p.at().cfg("SUBT").getS() == "stat") break;
    }
    if(i_p >= pLs.size())
    {
	string ap_nm = "UpTimeStation";
	while(aCntr->present(ap_nm)) ap_nm = TSYS::strLabEnum(ap_nm);
	aCntr->add(ap_nm, 0);
	AutoHD<TMdPrm> dprm = aCntr->at(ap_nm);
	dprm.at().setName(_("Station up time"));
	dprm.at().autoC(true);
	dprm.at().cfg("TYPE").setS(id());
	dprm.at().cfg("SUBT").setS("stat");
	dprm.at().cfg("EN").setB(true);
	if(aCntr->enableStat()) dprm.at().enable();
    }
}
