
//OpenSCADA module DAQ.System file: da_power.cpp
/***************************************************************************
 *   Copyright (C) 2023 by Roman Savochenko, <roman@oscada.org>            *
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
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <stddef.h>
#include <dirent.h>

#include <tsys.h>

#include "os_contr.h"
#include "da_power.h"

using namespace SystemCntr;

//*************************************************
//* Power                                        *
//*************************************************
void Power::getVal( TMdPrm *prm )
{
    const char *lab;
    string trg = prm->cfg("SUBT").getS(),
	   pType = devRead(trg,"type"), tVl;

    //Creating the parameters
    if(!prm->vlPresent(lab="type")) {
	// Appending for main parameters
	((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Type"),TFld::String,TFld::NoWrite|TVal::DirRead));
	if(devChkAccess(trg,lab="technology"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Technology"),TFld::String,TFld::NoWrite|TVal::DirRead));
	if(devChkAccess(trg,lab="scope"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Scope"),TFld::String,TFld::NoWrite|TVal::DirRead));
	if(devChkAccess(trg,lab="present"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Present"),TFld::Boolean,TFld::NoWrite));
	if(devChkAccess(trg,lab="manufacturer")) {
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Manufacturer, model, SN"),TFld::String,TFld::NoWrite|TVal::DirRead));
	    if(devChkAccess(trg,lab="model_name"))
		((TElem*)prm->daData)->fldAdd(new TFld(lab,"",TFld::String,TFld::NoWrite|TVal::DirRead));
	    if(devChkAccess(trg,lab="serial_number"))
		((TElem*)prm->daData)->fldAdd(new TFld(lab,"",TFld::String,TFld::NoWrite|TVal::DirRead));
	}
	if(devChkAccess(trg,lab="status"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Status"),TFld::String,TFld::NoWrite));
	if(devChkAccess(trg,lab="online"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Online"),TFld::Boolean,TFld::NoWrite));
	if(devChkAccess(trg,lab="health"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Health"),TFld::String,TFld::NoWrite));
	if(devChkAccess(trg,lab="capacity"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Capacity"),TFld::Integer,TFld::NoWrite));
	if(devChkAccess(trg,lab="capacity_level"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Capacity level"),TFld::String,TFld::NoWrite));
	if(devChkAccess(trg,lab="charge_now")) {
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Charge now, full, design\nIn Ah"),TFld::Real,TFld::NoWrite));
	    ((TElem*)prm->daData)->fldAdd(new TFld("charge_full","",TFld::Real,TFld::NoWrite));
	    ((TElem*)prm->daData)->fldAdd(new TFld("charge_full_design","",TFld::Real,TFld::NoWrite));
	}
	if(devChkAccess(trg,lab="energy_now")) {
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Energy now, full, design\nIn Ah"),TFld::Real,TFld::NoWrite));
	    ((TElem*)prm->daData)->fldAdd(new TFld("energy_full","",TFld::Real,TFld::NoWrite));
	    ((TElem*)prm->daData)->fldAdd(new TFld("energy_full_design","",TFld::Real,TFld::NoWrite));
	}
	if(devChkAccess(trg,lab="voltage_now"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Voltage"),TFld::Real,TFld::NoWrite));
	if(devChkAccess(trg,lab="voltage_min_design"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Voltage minimum"),TFld::Real,TFld::NoWrite));
	if(devChkAccess(trg,lab="cycle_count"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Cycle count"),TFld::Integer,TFld::NoWrite));
	if(devChkAccess(trg,lab="power_now"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Power\nIn Watts"),TFld::Real,TFld::NoWrite));
	if(devChkAccess(trg,lab="current_now"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Current\nIn mAmpers"),TFld::Real,TFld::NoWrite));
	if(devChkAccess(trg,lab="temp"))
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Temperature\nIn degC"),TFld::Real,TFld::NoWrite));

	if(devChkAccess(trg,lab="time_to_empty_now")) {
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Time to empty now, average"),TFld::Real,TFld::NoWrite));
	    ((TElem*)prm->daData)->fldAdd(new TFld("time_to_empty_avg","",TFld::Real,TFld::NoWrite));
	    ((TElem*)prm->daData)->fldAdd(new TFld("time_to_full_now",trS("Time to full"),TFld::Real,TFld::NoWrite));
	}

	if(devChkAccess(trg,lab="charge_start_threshold")) {
	    bool isWr = devChkAccess(trg, lab, "w");
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Charge threshold start, stop"),TFld::Integer,
						isWr?unsigned(TVal::DirWrite|TVal::NoSave):unsigned(TFld::NoWrite)));
	    ((TElem*)prm->daData)->fldAdd(new TFld("charge_stop_threshold","",TFld::Integer,
						isWr?unsigned(TVal::DirWrite|TVal::NoSave):unsigned(TFld::NoWrite)));
	}

	if(devChkAccess(trg,lab="charge_control_start_threshold")) {
	    bool isWr = devChkAccess(trg, lab, "w");
	    ((TElem*)prm->daData)->fldAdd(new TFld(lab,trS("Charge control threshold start, end"),TFld::Integer,
						isWr?unsigned(TVal::DirWrite|TVal::NoSave):unsigned(TFld::NoWrite)));
	    ((TElem*)prm->daData)->fldAdd(new TFld("charge_control_end_threshold","",TFld::Integer,
						isWr?unsigned(TVal::DirWrite|TVal::NoSave):unsigned(TFld::NoWrite)));
	}

	// Appending for very specific parameters
	DIR *IdDir = opendir((DIR_PS+trg).c_str());
	if(IdDir) {
	    struct stat file_stat;
	    dirent  *scan_rez = NULL,
	    *scan_dirent = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);
	    while(readdir_r(IdDir,scan_dirent,&scan_rez) == 0 && scan_rez) {
		if(strcmp(scan_rez->d_name,"..") == 0 || strcmp(scan_rez->d_name,".") == 0 ||
			strcmp(scan_rez->d_name,"alarm") == 0 || strcmp(scan_rez->d_name,"uevent") == 0 ||
			prm->vlPresent(scan_rez->d_name))
		    continue;
		stat((DIR_PS+trg+"/"+scan_rez->d_name).c_str(), &file_stat);
		if((file_stat.st_mode&S_IFMT) != S_IFREG || !devChkAccess(trg,scan_rez->d_name)) continue;

		((TElem*)prm->daData)->fldAdd(new TFld(scan_rez->d_name,scan_rez->d_name,TFld::String,
		    devChkAccess(trg,scan_rez->d_name,"w")?unsigned(TVal::DirWrite|TVal::NoSave):unsigned(TFld::NoWrite|TVal::DirRead)));
	    }

	    free(scan_dirent);
	    closedir(IdDir);
	}
    }

    //Updating the operative parameters
    AutoHD<TVal> vO;
    if(!(vO=prm->vlAt(lab="present",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="status",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="online",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="health",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="capacity",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="capacity_level",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="charge_now",true)).freeStat())	vO.at().setR(((tVl=devRead(trg,lab)) != EVAL_STR) ? 1e-6*s2r(tVl) : EVAL_REAL, 0, true);
    if(!(vO=prm->vlAt(lab="charge_full",true)).freeStat())	vO.at().setR(((tVl=devRead(trg,lab)) != EVAL_STR) ? 1e-6*s2r(tVl) : EVAL_REAL, 0, true);
    if(!(vO=prm->vlAt(lab="charge_full_design",true)).freeStat())	vO.at().setR(((tVl=devRead(trg,lab)) != EVAL_STR) ? 1e-6*s2r(tVl) : EVAL_REAL, 0, true);
    if(!(vO=prm->vlAt(lab="energy_now",true)).freeStat())	vO.at().setR(((tVl=devRead(trg,lab)) != EVAL_STR) ? 1e-6*s2r(tVl) : EVAL_REAL, 0, true);
    if(!(vO=prm->vlAt(lab="energy_full",true)).freeStat())	vO.at().setR(((tVl=devRead(trg,lab)) != EVAL_STR) ? 1e-6*s2r(tVl) : EVAL_REAL, 0, true);
    if(!(vO=prm->vlAt(lab="energy_full_design",true)).freeStat())	vO.at().setR(((tVl=devRead(trg,lab)) != EVAL_STR) ? 1e-6*s2r(tVl) : EVAL_REAL, 0, true);
    if(!(vO=prm->vlAt(lab="voltage_now",true)).freeStat())	vO.at().setR(((tVl=devRead(trg,lab)) != EVAL_STR) ? 1e-6*s2r(tVl) : EVAL_REAL, 0, true);
    if(!(vO=prm->vlAt(lab="voltage_min_design",true)).freeStat())	vO.at().setR(((tVl=devRead(trg,lab)) != EVAL_STR) ? 1e-6*s2r(tVl) : EVAL_REAL, 0, true);
    if(!(vO=prm->vlAt(lab="cycle_count",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="power_now",true)).freeStat())	vO.at().setR(((tVl=devRead(trg,lab)) != EVAL_STR) ? 1e-6*s2r(tVl) : EVAL_REAL, 0, true);
    if(!(vO=prm->vlAt(lab="current_now",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="temp",true)).freeStat())	vO.at().setR(((tVl=devRead(trg,lab)) != EVAL_STR) ? 0.1*s2r(tVl) : EVAL_REAL, 0, true);
    if(!(vO=prm->vlAt(lab="time_to_empty_now",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="time_to_empty_avg",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="time_to_full_now",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="charge_start_threshold",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="charge_stop_threshold",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="charge_control_start_threshold",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
    if(!(vO=prm->vlAt(lab="charge_control_end_threshold",true)).freeStat())	vO.at().setS(devRead(trg,lab), 0, true);
}

void Power::vlGet( TMdPrm *prm, TVal &val )
{
    val.setS(devRead(prm->cfg("SUBT").getS(),val.name()), 0, true);
}

void Power::vlSet( TMdPrm *prm, TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    FILE *f = NULL;
    string trg = prm->cfg("SUBT").getS();

    if((vo.name() == "chargeStart" && (f=fopen(TSYS::strMess(DIR_PS "%s/charge_start_threshold",trg.c_str()).c_str(),"w"))) ||
	    (vo.name() == "chargeStop" && (f=fopen(TSYS::strMess(DIR_PS "%s/charge_stop_threshold",trg.c_str()).c_str(),"w"))) ||
	    (vo.name() == "chargeCntrStart" && (f=fopen(TSYS::strMess(DIR_PS "%s/charge_control_start_threshold",trg.c_str()).c_str(),"w"))) ||
	    (vo.name() == "chargeCntrEnd" && (f=fopen(TSYS::strMess(DIR_PS "%s/charge_control_end_threshold",trg.c_str()).c_str(),"w"))))
	fputs(vl.getS().c_str(), f);

    if(f && fclose(f) != 0) mess_warning(prm->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
}

void Power::dList( vector<string> &list, TMdPrm *prm )
{
    DIR *IdDir = opendir(DIR_PS);
    if(IdDir == NULL) return;

    dirent  *scan_rez = NULL,
	    *scan_dirent = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);
    while(readdir_r(IdDir,scan_dirent,&scan_rez) == 0 && scan_rez)
	if(strcmp(scan_rez->d_name,"..") && strcmp(scan_rez->d_name,".") && devChkAccess(string(scan_rez->d_name),"type"))
	    list.push_back(scan_rez->d_name);

    free(scan_dirent);
    closedir(IdDir);
}

bool Power::devChkAccess( const string &dev, const string &file, const string &acs )
{
    FILE *f = fopen(TSYS::strMess(DIR_PS "%s/%s",dev.c_str(),file.c_str()).c_str(), acs.c_str());
    bool rez = f;
    if(f && fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);

    return rez;
}

string Power::devRead( const string &dev, const string &file )
{
    char buf[256];

    FILE *f = fopen(TSYS::strMess(DIR_PS "%s/%s",dev.c_str(),file.c_str()).c_str(), "r");
    string rez = (f && fgets(buf,sizeof(buf),f) != NULL) ? TSYS::strLine(buf,0) : EVAL_STR;
    if(f && fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);

    return rez;
}
