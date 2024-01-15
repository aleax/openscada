
//OpenSCADA module DAQ.System file: da_proc.cpp
/***************************************************************************
 *   Copyright (C) 2024 by Roman Savochenko, <roman@oscada.org>            *
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

#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <dirent.h>
#include <algorithm>

#include <tsys.h>

#include "os_contr.h"
#include "da_proc.h"

using namespace SystemCntr;

//*************************************************
//* Proc                                           *
//*************************************************
Proc::Proc( )
{
    //Process' value structure
    fldAdd(new TFld("FDSize",trS("Number of file descriptor slots currently"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmPeak",trS("Peak virtual memory size (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmSize",trS("Virtual memory size (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmLck",trS("Locked memory size (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmPin",trS("Pinned memory size (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmHWM",trS("Peak resident set size (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmRSS",trS("Resident set size (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("RssAnon",trS("Size of resident anonymous memory (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("RssFile",trS("Size of resident file mappings (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("RssShmem",trS("Size of resident shared memory (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmData",trS("Size of data segment (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmStk",trS("Size of stack segment (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmExe",trS("Size of text segment (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmLib",trS("Shared library code size (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmPTE",trS("Page table entries size (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("VmSwap",trS("Swapped-out virtual memory size by anonymous private pages (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("HugetlbPages",trS("Size of hugetlb memory portions (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("Threads",trS("Number of threads in process"),TFld::Integer,TFld::NoWrite));
}

void Proc::init( TMdPrm *prm, bool update )
{
    DA::init(prm, update);
    if(!update) {
	if(prm->daData) delete (TElem*)prm->daData;
	prm->daData = new tval;
    }

    if(!update) {
	TCfg &cSubt = prm->cfg("SUBT");
	//cSubt.fld().setDescr(_("Process"));
	cSubt.fld().setFlg(cSubt.fld().flg()|TFld::SelEdit);
    }
}

void Proc::deInit( TMdPrm *prm )
{
    delete (tval*)prm->daData; prm->daData = NULL;
    DA::deInit(prm);

    TCfg &cSubt = prm->cfg("SUBT");
    cSubt.fld().setFlg(cSubt.fld().flg()&~(TFld::SelEdit));
}

void Proc::cfgChange( TMdPrm *prm, TCfg &co, const TVariant &pc )
{
    if(co.name() == "SUBT" && (tval*)prm->daData) ((tval*)prm->daData)->tmUpd = 0;
}

void Proc::getVal( TMdPrm *prm )
{
    char buf[256];
    string prcTmpl = prm->cfg("SUBT").getS(), comm, commL, pNm;
    long tVl;
    map<string, bool>	rdVls;
    map<string, long>	rdSum;
    DIR *IdDir = NULL;
    FILE *f = NULL;
    bool isRE = false;
    tval *tv = (tval*)prm->daData;

    //Processing the PIDs cache
    if(prcTmpl.empty())	tv->tmUpd = 0;
    // Forcing to SELF
    else if(prcTmpl == SELF_PROC) {
	if(tv->tmUpd == 0) { tv->prcCache.clear(); tv->tmUpd = SYS->sysTm(); tv->prcCache.push_back(getpid()); }
    }
    // Search for proper PIDs
    else if((tv->tmUpd == 0 || (SYS->sysTm()-tv->tmUpd) > PIDS_CACHE_UPD_TM) && (IdDir=opendir("/proc"))) {
	tv->prcCache.clear();
	tv->tmUpd = SYS->sysTm();

	dirent *scan_dirent = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);
	for(dirent *sRez = NULL; readdir_r(IdDir,scan_dirent,&sRez) == 0 && sRez; )
	    if(isdigit(sRez->d_name[0]) && (comm=devRead(string(sRez->d_name)+"/comm")) != EVAL_STR &&
		    ((!(isRE=(prcTmpl[0] == '/' && prcTmpl[prcTmpl.size()-1] == '/')) && comm == prcTmpl) ||
			(isRE && TRegExp(prcTmpl.substr(1,prcTmpl.size()-2)).test(((commL=devRead(string(sRez->d_name)+"/cmdline")).size()?commL:comm)))))
		tv->prcCache.push_back(s2i(sRez->d_name));
	free(scan_dirent);

	closedir(IdDir);
    }

    //Processing the PIDs list
    for(vector<int>::iterator iPid = tv->prcCache.begin(); iPid != tv->prcCache.end(); ++iPid) {
	rdVls.clear();
	if((f=fopen(("/proc/"+i2s(*iPid)+"/status").c_str(),"r"))) {
	    while(fgets(buf,sizeof(buf),f) != NULL)
		for(int iFld = 0; iFld < fldSize(); ++iFld)
		    if(rdVls.find((pNm=fldAt(iFld).name())) == rdVls.end() && sscanf(buf,TSYS::strMess("%s: %%ld",pNm.c_str()).c_str(),&tVl))
			rdVls[pNm] = true, rdSum[pNm] += tVl;

	    if(fclose(f) != 0)
		mess_warning(prm->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
	}
    }

    //Writing all accumulated values
    for(map<string,long>::iterator iSum = rdSum.begin(); iSum != rdSum.end(); ++iSum)
	prm->vlAt(iSum->first).at().setI(iSum->second, 0, true);

    //Setting EVAL
    for(int iFld = 0; iFld < fldSize(); ++iFld)
	if(rdSum.find((pNm=fldAt(iFld).name())) == rdSum.end())
	    prm->vlAt(pNm).at().setI(EVAL_INT, 0, true);
}

void Proc::dList( vector<string> &list, TMdPrm *prm )
{
    list.push_back(SELF_PROC);

    string s_val;
    DIR *IdDir = NULL;
    if((IdDir=opendir("/proc"))) {
	dirent *scan_dirent = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);
	for(dirent *sRez = NULL; readdir_r(IdDir,scan_dirent,&sRez) == 0 && sRez; )
	    if(isdigit(sRez->d_name[0]) && (s_val=devRead(string(sRez->d_name)+"/comm")) != EVAL_STR &&
		    std::find(list.begin(),list.end(),s_val) == list.end())
		list.push_back(s_val);
	free(scan_dirent);

	closedir(IdDir);
    }
}

string Proc::devRead( const string &file )
{
    char buf[256];

    string rez = EVAL_STR;
    FILE *f = fopen(TSYS::strMess("/proc/%s",file.c_str()).c_str(), "r");
    if(f) {
	rez = "";
	while(fgets(buf,sizeof(buf),f) != NULL) rez.append(buf);
	if(rez.size() && rez[rez.size()-1] == '\n') rez.erase(rez.size()-1, 1);
	if(fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
    }

    return rez;
}
