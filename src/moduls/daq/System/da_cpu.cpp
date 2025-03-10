
//OpenSCADA module DAQ.System file: da_cpu.cpp
/***************************************************************************
 *   Copyright (C) 2005-2025 by Roman Savochenko, <roman@oscada.org>       *
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
#include <errno.h>

#include <tsys.h>

#include "os_contr.h"
#include "da_cpu.h"

using namespace SystemCntr;

//*************************************************
//* CPU                                           *
//*************************************************
CPU::CPU( )	{ }

CPU::~CPU( )	{ }

void CPU::updGen( bool cntr )
{
    string tVl, tVl1;
    FILE *f;
    float SYS_F = 0;

    if(!((!contN() && !cntr) || (contN() && cntr))) return;

    ResAlloc res(mRes, true);

    //CPU Load
    if((f=fopen("/proc/stat","r"))) {
	// Initialisation
	if(!fldPresent("load")) {
	    fldAdd(new TFld("load",trS("Load whole, system, user, idle\nIn %"),TFld::Real,TFld::NoWrite));
	    fldAdd(new TFld("sys","",TFld::Real,TFld::NoWrite));
	    fldAdd(new TFld("user","",TFld::Real,TFld::NoWrite));
	    fldAdd(new TFld("idle","",TFld::Real,TFld::NoWrite));
	}

	// Updating
	string resLoad, resSys, resUser, resIdle;
	long long unsigned user, nice, sys, idle, iowait;
	char buf[256];
	for(int n = 0, unit = -1; fgets(buf,sizeof(buf),f) != NULL; ) {
	    if(!isdigit(buf[3]) && (n=sscanf(buf,"cpu %llu %llu %llu %llu %llu\n",&user,&nice,&sys,&idle,&iowait))) unit = -1;
	    else if((n=sscanf(buf,"cpu%d %llu %llu %llu %llu %llu\n",&unit,&user,&nice,&sys,&idle,&iowait))) ;
	    else continue;

	    while(load.size() < (unit+2)) load.push_back(tval());

	    if(n == ((unit<0)?5:6)) idle += iowait;

	    float sum = (user+nice+sys+idle) - (load[unit+1].user+load[unit+1].nice+load[unit+1].sys+load[unit+1].idle);
	    if(sum == 0) continue;

	    resLoad += r2s(100*(float(user+sys+nice-load[unit+1].user-load[unit+1].sys-load[unit+1].nice))/sum) + ";";
	    resSys += r2s(100*(float(sys-load[unit+1].sys))/sum) + ";";
	    resUser += r2s(100*(float(user-load[unit+1].user))/sum) + ";";
	    resIdle += r2s(100*(float(idle-load[unit+1].idle))/sum) + ";";

	    load[unit+1] = tval(user, nice, sys, idle);
	}
	if(fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);

	fldAt(fldId("load")).setReserve(resLoad);
	fldAt(fldId("sys")).setReserve(resSys);
	fldAt(fldId("user")).setReserve(resUser);
	fldAt(fldId("idle")).setReserve(resIdle);
    }

    //CPU Frequency
    if(devChkAccess(0,"scaling_driver")) {
	bool isWr;
	unsigned cpuCnt = SYS->nCPU();
	int frqDrv = -1, frqGov = -1, frqCur = -1, frqMin = -1, frqMinCur = -1, frqMax = -1, frqMaxCur = -1;

	// Initialisation
	if(!fldPresent("frqDrv")) {
	    frqDrv = fldAdd(new TFld("frqDrv",trS("Frequency: driver"),TFld::String,TFld::NoWrite|TVal::DirRead));

	    if((isWr=devChkAccess(0,"scaling_governor","w")) && (tVl=devRead(0,"scaling_available_governors")) != EVAL_STR)
		tVl = TRegExp("\\s","g").replace(tVl, ";");
	    frqGov = fldAdd(new TFld("frqGov",trS("Frequency: governor"),TFld::String,
				isWr?TVal::DirWrite|TVal::NoSave|TFld::Selectable:TFld::NoWrite,"","","",tVl.c_str()));

	    string scAvFrqs;
	    if((tVl=devRead(0,"scaling_available_frequencies")) != EVAL_STR)
		for(int off = 0; (tVl1=TSYS::strParse(tVl,0," ",&off)).size(); )
		    scAvFrqs += r2s(1e-3*s2r(tVl1)) + ";";

	    if((isWr=devChkAccess(0,"scaling_setspeed","w")) && ((tVl=devRead(0,"scaling_setspeed")) == EVAL_STR || tVl == "<unsupported>"))
		isWr = false;
	    fldAdd(new TFld("frqCur",trS("Frequency: current\nIn MHz"),TFld::Real,
			isWr?(unsigned(TVal::DirWrite|TVal::NoSave)|(scAvFrqs.size()?TFld::Selectable:0)):unsigned(TFld::NoWrite),
			"","","",scAvFrqs.c_str()));

	    if(devChkAccess(0,"cpuinfo_min_freq")) {
		frqMin = fldAdd(new TFld("frqMin",trS("Frequency: minimum, set\nIn MHz"),TFld::Real,TFld::NoWrite|TVal::DirRead));
		if(devChkAccess(0,"scaling_min_freq")) {
		    isWr = devChkAccess(0, "scaling_min_freq", "w");
		    frqMinCur = fldAdd(new TFld("frqMinCur","",TFld::Real,
			isWr?(unsigned(TVal::DirWrite|TVal::NoSave)|(scAvFrqs.size()?TFld::Selectable:0)):unsigned(TFld::NoWrite),
			"","","",scAvFrqs.c_str()));
		}
	    }

	    if(devChkAccess(0,"cpuinfo_max_freq")) {
		frqMax = fldAdd(new TFld("frqMax",trS("Frequency: maximum, set\nIn MHz"),TFld::Real,TFld::NoWrite|TVal::DirRead));
		if(devChkAccess(0,"scaling_max_freq")) {
		    isWr = devChkAccess(0, "scaling_max_freq", "w");
		    frqMaxCur = fldAdd(new TFld("frqMaxCur","",TFld::Real,
			isWr?(unsigned(TVal::DirWrite|TVal::NoSave)|(scAvFrqs.size()?TFld::Selectable:0)):unsigned(TFld::NoWrite),
			"","","",scAvFrqs.c_str()));
		}
	    }
	}

	// Global updating once
	if(frqDrv >= 0) {
	    tVl = "";
	    for(unsigned cpuN = 0; cpuN < cpuCnt; ++cpuN)
		tVl += devRead(cpuN,"scaling_driver") + "\n";
	    fldAt(frqDrv).setReserve(tVl);
	}
	if(frqMin >= 0) {
	    tVl = "";
	    for(unsigned cpuN = 0; cpuN < cpuCnt; ++cpuN)
		tVl += devRead(cpuN,"cpuinfo_min_freq") + "\n";
	    fldAt(frqMin).setReserve(tVl);
	}
	if(frqMax >= 0) {
	    tVl = "";
	    for(unsigned cpuN = 0; cpuN < cpuCnt; ++cpuN)
		tVl += devRead(cpuN,"cpuinfo_max_freq") + "\n";
	    fldAt(frqMax).setReserve(tVl);
	}
	// ... at connections - presence the Data Sources
	if((frqGov >= 0 || (frqGov=fldId("frqGov",true)) != fldSize()) && contN() && cntr) {
	    tVl = "";
	    for(unsigned cpuN = 0; cpuN < cpuCnt; ++cpuN)
		tVl += devRead(cpuN,"scaling_governor") + "\n";
	    fldAt(frqGov).setReserve(tVl);
	}
	if((frqMinCur >= 0 || (frqMinCur=fldId("frqMinCur",true)) != fldSize()) && contN() && cntr) {
	    tVl = "";
	    for(unsigned cpuN = 0; cpuN < cpuCnt; ++cpuN)
		tVl += devRead(cpuN,"scaling_min_freq") + "\n";
	    fldAt(frqMinCur).setReserve(tVl);
	}
	if((frqMaxCur >= 0 || (frqMaxCur=fldId("frqMaxCur",true)) != fldSize()) && contN() && cntr) {
	    tVl = "";
	    for(unsigned cpuN = 0; cpuN < cpuCnt; ++cpuN)
		tVl += devRead(cpuN,"scaling_max_freq") + "\n";
	    fldAt(frqMaxCur).setReserve(tVl);
	}
	// ... constantly
	if(frqCur >= 0 || (frqCur=fldId("frqCur",true)) != fldSize()) {
	    tVl = "";
	    float sumFrq = 0;
	    for(unsigned cpuN = 0; cpuN < cpuCnt; ++cpuN) {
		tVl1 = devRead(cpuN,"scaling_cur_freq");
		tVl += tVl1 + "\n";
		sumFrq += s2r(tVl1);
	    }
	    tVl += r2s(sumFrq/cpuCnt) + "\n";

	    fldAt(frqCur).setReserve(tVl);

	    if(!SYS_F) SYS_F = 1e-3*sumFrq/cpuCnt;
	}
    }

    //CPU Information from /proc/cpuinfo
    if((f=fopen("/proc/cpuinfo","r"))) {
	char buf[255], sval[100];
	int unit = 0;
	float fval;
	string ComModel, models, MHzs, BogoMIPSs;
	int model = -1, MHz = -1, BogoMIPS = -1;

	while(fgets(buf,sizeof(buf),f) != NULL) {
	    if(sscanf(buf,"Processor : %[^\n]\n",sval))		ComModel = sval;
	    else if(sscanf(buf,"processor : %d\n",&unit)) ;
	    else if(sscanf(buf,"model name : %[^\n]",sval))	models += string(sval) + "\n";
	    else if(sscanf(buf,"cpu MHz : %f\n",&fval))		MHzs += r2s(fval) + "\n";
	    else if(sscanf(buf,"bogomips : %f\n",&fval) || sscanf(buf,"BogoMIPS : %f\n",&fval))
		BogoMIPSs += r2s(fval) + "\n";
	}
	if(fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);

	// Initialisation
	if(!fldPresent("model")) {
	    model = fldAdd(new TFld("model",trS("Model name"),TFld::String,TFld::NoWrite|TVal::DirRead));
	    if(MHzs.size()) MHz = fldAdd(new TFld("MHz",trS("MHz"),TFld::Real,TFld::NoWrite));
	    if(BogoMIPSs.size()) BogoMIPS = fldAdd(new TFld("BogoMIPS",trS("BogoMIPS"),TFld::Real,TFld::NoWrite|TVal::DirRead));
	}

	// Global updating once
	if(model >= 0) {
	    if(models.empty() && ComModel.size())
		for(int iU = 0; iU <= unit; ++iU) models += ComModel + "\n";
	    fldAt(model).setReserve(models);
	}
	// ... at connections - presence the Data Sources
	if((BogoMIPS >= 0 || (BogoMIPS=fldId("BogoMIPS",true)) != fldSize()) && contN())
	    fldAt(BogoMIPS).setReserve(BogoMIPSs);
	// ... constantly
	if(MHz >= 0 || (MHz=fldId("MHz",true)) != fldSize()) {
	    int cpuCnt = 0;
	    float sumFrq = 0;
	    for(int off = 0; (tVl=TSYS::strLine(MHzs,0,&off)).size(); ++cpuCnt)
		sumFrq += s2r(tVl);
	    MHzs += r2s(sumFrq/cpuCnt) + "\n";
	    fldAt(MHz).setReserve(MHzs);

	    if(!SYS_F) SYS_F = sumFrq/cpuCnt;
	}
    }

#if defined (__i386__) || defined (__x86_64__)
    //RDTSC of x86
    int rdtscID = -1;
    if(!fldPresent("rdtsc")) {
	rdtsc = rdtscTm = 0;
	rdtscID = fldAdd(new TFld("rdtsc",trS("MHz at rdtsc"),TFld::Real,TFld::NoWrite));
    }

    if(rdtscID >= 0 || (rdtscID=fldId("rdtsc",true)) != fldSize()) {
	unsigned int cntl, cnth;
	asm volatile("rdtsc; movl %%eax,%0; movl %%edx,%1;":"=r"(cntl),"=r"(cnth)::"%eax","%edx");

	int64_t rdtsc_ = ((uint64_t)cnth<<32)+cntl;
	time_t  rdtscTm_ = time(NULL);
	if(rdtsc) fldAt(rdtscID).setReserve((tVl=r2s(1e-6*double(rdtsc_-rdtsc)/(rdtscTm_-rdtscTm))));
	rdtsc = rdtsc_; rdtscTm = rdtscTm_;

	if(!SYS_F) SYS_F = s2ll(tVl);
    }
#endif

    //Writing SYS_F to SYS
    if(SYS_F) SYS->setSysClk(SYS_F);
}

void CPU::getVal( TMdPrm *prm )
{
    string trg = prm->cfg("SUBT").getS();

    ResAlloc res(mRes, false);

    string tVl;
    int cpuN = s2i(trg);
    AutoHD<TVal> vO;
    if(!(vO=prm->vlAt("load",true)).freeStat())
	vO.at().setS(TSYS::strParse(vO.at().fld().reserve(),(trg=="gen")?0:cpuN+1,";"), 0, true);
    if(!(vO=prm->vlAt("sys",true)).freeStat())
	vO.at().setS(TSYS::strParse(vO.at().fld().reserve(),(trg=="gen")?0:cpuN+1,";"), 0, true);
    if(!(vO=prm->vlAt("user",true)).freeStat())
	vO.at().setS(TSYS::strParse(vO.at().fld().reserve(),(trg=="gen")?0:cpuN+1,";"), 0, true);
    if(!(vO=prm->vlAt("idle",true)).freeStat())
	vO.at().setS(TSYS::strParse(vO.at().fld().reserve(),(trg=="gen")?0:cpuN+1,";"), 0, true);

    if(!(vO=prm->vlAt("MHz",true)).freeStat())
	vO.at().setS(TSYS::strLine(vO.at().fld().reserve(),cpuN), 0, true);
    if(!(vO=prm->vlAt("rdtsc",true)).freeStat())
	vO.at().setS(vO.at().fld().reserve(), 0, true);

    if(prm->vlPresent("frqDrv")) {
	if(!(vO=prm->vlAt("frqCur",true)).freeStat())
	    vO.at().setR(s2r(TSYS::strLine(vO.at().fld().reserve(),(trg=="gen")?SYS->nCPU():cpuN))/1000, 0, true);
	if(!(vO=prm->vlAt("frqGov",true)).freeStat())
	    vO.at().setS(TSYS::strLine(vO.at().fld().reserve(),cpuN), 0, true);
	if(!(vO=prm->vlAt("frqMinCur",true)).freeStat())
	    vO.at().setR(((tVl=TSYS::strLine(vO.at().fld().reserve(),cpuN)) != EVAL_STR) ? s2r(tVl)/1000 : EVAL_REAL, 0, true);
	if(!(vO=prm->vlAt("frqMaxCur",true)).freeStat())
	    vO.at().setR(((tVl=TSYS::strLine(vO.at().fld().reserve(),cpuN)) != EVAL_STR) ? s2r(tVl)/1000 : EVAL_REAL, 0, true);
    }

    //Device error
    /*if(devOK) prm->daErr = "";
    else if(!prm->daErr.getVal().size()) {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }*/
}

void CPU::vlGet( TMdPrm *prm, TVal &val )
{
    string tVl;
    int cpuN = s2i(prm->cfg("SUBT").getS());

    ResAlloc res(mRes, false);

    if(val.name() == "frqMin" || val.name() == "frqMax")
	val.setR(((tVl=TSYS::strLine(val.fld().reserve(),cpuN)) != EVAL_STR) ? s2r(tVl)/1000 : EVAL_REAL, 0, true);
    else val.setS(TSYS::strLine(val.fld().reserve(),cpuN), 0, true);
}

void CPU::vlSet( TMdPrm *prm, TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    FILE *f = NULL;
    string trg = prm->cfg("SUBT").getS();

    for(int cpuN = s2i(trg), iCp = cpuN; iCp <= ((trg=="gen")?SYS->nCPU()-1:cpuN); ++iCp) {
	if(vo.name() == "frqGov" && (f=fopen(TSYS::strMess("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_governor",iCp).c_str(),"w")))
	    fputs(vl.getS().c_str(), f);
	else if((vo.name() == "frqCur" && (f=fopen(TSYS::strMess("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_setspeed",iCp).c_str(),"w"))) ||
		(vo.name() == "frqMinCur" && (f=fopen(TSYS::strMess("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq",iCp).c_str(),"w"))) ||
		(vo.name() == "frqMaxCur" && (f=fopen(TSYS::strMess("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq",iCp).c_str(),"w"))))
	    fputs(i2s(vl.getR()*1000).c_str(), f);

	if(f && fclose(f) != 0) mess_warning(prm->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
    }
}

void CPU::dList( vector<string> &list, TMdPrm *prm )
{
    unsigned cpuCnt = SYS->nCPU();
    for(unsigned nCPU = 0; cpuCnt > 1 && nCPU < cpuCnt; ++nCPU)
	list.push_back(i2s(nCPU));
    list.push_back(string("gen")+SEP_SUBID+";"+_("General"));
}

bool CPU::devChkAccess( int cpuN, const string &file, const string &acs )
{
    FILE *f = fopen(TSYS::strMess("/sys/devices/system/cpu/cpu%d/cpufreq/%s",cpuN,file.c_str()).c_str(), acs.c_str());
    bool rez = f;
    if(f && fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);

    return rez;
}

string CPU::devRead( int cpuN, const string &file )
{
    char buf[256];

    FILE *f = fopen(TSYS::strMess("/sys/devices/system/cpu/cpu%d/cpufreq/%s",cpuN,file.c_str()).c_str(), "r");
    string rez = (f && fgets(buf,sizeof(buf),f) != NULL) ? TSYS::strLine(buf,0) : EVAL_STR;
    if(f && fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);

    return rez;
}
