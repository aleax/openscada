
//OpenSCADA system module DAQ.System file: da_cpu.cpp
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
#include "da_cpu.h"

using namespace SystemCntr;

//*************************************************
//* CPU                                           *
//*************************************************
CPU::CPU( )
{
    //> CPU value structure
    fldAdd(new TFld("load",_("Load (%)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("sys",_("System (%)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("user",_("User (%)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("idle",_("Idle (%)"),TFld::Real,TFld::NoWrite));
}

CPU::~CPU( )
{

}

void CPU::init( TMdPrm *prm )
{
    char buf[256];
    prm->daData = new tval;

    //> Create config
    TCfg &c_subt = prm->cfg("SUBT");
    c_subt.fld().setDescr("");

    //> Init start value
    FILE *f = fopen("/proc/stat","r");
    string cpuLs, cpuLsNm;
    for(int n_cpu; f && fgets(buf,sizeof(buf),f) != NULL; )
	if(sscanf(buf,"cpu%d",&n_cpu))
	{
	    if(!isdigit(buf[3]))
	    {
		cpuLs += "gen;";
		cpuLsNm += string(_("General"))+";";
	    }
	    else
	    {
		cpuLs += TSYS::int2str(n_cpu)+";";
		cpuLsNm += TSYS::int2str(n_cpu)+";";
	    }
	}
    c_subt.fld().setValues(cpuLs);
    c_subt.fld().setSelNames(cpuLsNm);

    if(f) fclose(f);

    if(!TRegExp("(^|;)"+c_subt.getS()+";").test(cpuLs)) c_subt.setS("gen");
}

void CPU::deInit( TMdPrm *prm )
{
    delete (tval*)prm->daData;
}

void CPU::getVal( TMdPrm *prm )
{
    long unsigned user,nice,sys,idle,iowait;
    float sum;
    tval &c_vls = *(tval*)prm->daData;
    bool devOK = false;

    string trg = prm->cfg("SUBT").getS();

    /*if( trg == "OpenSCADA" )
    {
	struct tms p_tm;
	clock_t cur_tm = times(&p_tm);
	sum  = cur_tm - m_nice;
	user = p_tm.tms_utime + p_tm.tms_cutime - m_user;
	sys  = p_tm.tms_stime + p_tm.tms_cstime - m_sys;

	prm->vlAt("value").at().setR( 100.0*(float(user + sys))/sum,NULL,true);
	prm->vlAt("sys").at().setR( 100.0*(float(sys))/sum,NULL,true);
	prm->vlAt("user").at().setR( 100.0*(float(user))/sum,NULL,true);
	prm->vlAt("idle").at().setR( 100.0*(float(sum - user - sys))/sum,NULL,true);

	m_nice = cur_tm;
	m_user = p_tm.tms_utime + p_tm.tms_cutime;
	m_sys  = p_tm.tms_stime + p_tm.tms_cstime;

	return;
    }*/

    //> File /proc/stat scan
    char buf[256];
    FILE *f = fopen("/proc/stat","r");
    for(int n = 0; f && fgets(buf,sizeof(buf),f) != NULL; )
    {
	if(trg == "gen")
	    n = sscanf(buf,"cpu %lu %lu %lu %lu %lu\n",&user,&nice,&sys,&idle,&iowait);
	else if(isdigit(trg[0]))
	    n = sscanf(buf,(string("cpu")+trg+" %lu %lu %lu %lu %lu\n").c_str(),&user,&nice,&sys,&idle,&iowait);
	if(n)
	{
	    if(n == 5) idle += iowait;
	    sum = (float)(user+nice+sys+idle-c_vls.user-c_vls.nice-c_vls.sys-c_vls.idle);
	    prm->vlAt("load").at().setR(100*(float(user+sys-c_vls.user-c_vls.sys))/sum, 0, true);
	    prm->vlAt("sys").at().setR(100*(float(sys-c_vls.sys))/sum, 0, true);
	    prm->vlAt("user").at().setR(100*(float(user-c_vls.user))/sum, 0, true);
	    prm->vlAt("idle").at().setR(100*(float(idle-c_vls.idle))/sum, 0, true);
	    c_vls.user = user;
	    c_vls.nice = nice;
	    c_vls.sys  = sys;
	    c_vls.idle = idle;
	    devOK = true;
	    break;
	}
    }
    if(f) fclose(f);

    //> Device error
    if(devOK) prm->daErr = "";
    else if(!prm->daErr.getVal().size())
    {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}

void CPU::makeActiveDA( TMdContr *aCntr )
{
    char buf[256];

    FILE *f = fopen("/proc/stat","r");
    if(f == NULL) return;

    //> Check for allow CPU
    for(int n_cpu; fgets(buf,sizeof(buf),f) != NULL; )
	if(sscanf(buf,"cpu%d",&n_cpu))
	{
	    string pId = "CPULoad";
	    string pNm = _("Full CPU Load");
	    string pSTp = "gen";
	    if(isdigit(buf[3]))
	    {
		pId = "CPU"+TSYS::int2str(n_cpu)+"Load";
		pNm = _("CPU Load :")+TSYS::int2str(n_cpu);
		pSTp = TSYS::int2str(n_cpu);
	    }

	    vector<string> pLs;
	    // Find propper parameter's object
	    aCntr->list(pLs);
	    unsigned i_p;
	    for(i_p = 0; i_p < pLs.size(); i_p++)
	    {
		AutoHD<TMdPrm> p = aCntr->at(pLs[i_p]);
		if(p.at().cfg("TYPE").getS() == id() && p.at().cfg("SUBT").getS() == pSTp)	break;
	    }
	    if(i_p < pLs.size()) continue;

	    while(aCntr->present(pId)) pId = TSYS::strLabEnum(pId);
	    aCntr->add(pId, 0);
	    AutoHD<TMdPrm> dprm = aCntr->at(pId);
	    dprm.at().setName(pNm);
	    dprm.at().autoC(true);
	    dprm.at().cfg("TYPE").setS(id());
	    dprm.at().cfg("SUBT").setS(pSTp);
	    dprm.at().cfg("EN").setB(true);
	    if(aCntr->enableStat()) dprm.at().enable();
	}
    fclose(f);
}
