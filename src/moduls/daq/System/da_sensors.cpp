
//OpenSCADA module DAQ.System file: da_sensors.cpp
/***************************************************************************
 *   Copyright (C) 2005-2024 by Roman Savochenko, <roman@oscada.org>       *
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
#include <unistd.h>
#include <stddef.h>
#include <dirent.h>

#include <tsys.h>

#if HAVE_SENSORS_SENSORS_H
#include <sensors/sensors.h>
#endif

#include "os_contr.h"
#include "da_sensors.h"

using namespace SystemCntr;

const char *Sensors::mbmon_cmd = "mbmon -r -c 1";	//write one try to stdout

//*************************************************
//* Sensors                                       *
//*************************************************
Sensors::Sensors( ) : libsensor_ok(false)
{
#if HAVE_SENSORS_SENSORS_H
    //Libsensor API init
#if SENSORS_API_VERSION >= 0x400
    if(sensors_init(NULL) == 0) libsensor_ok = true;
#else
    FILE *f = fopen("/etc/sensors.conf", "r");
    if(f) {
	if(sensors_init(f) == 0) libsensor_ok = true;
	if(fclose(f) != 0) mess_warning(nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
    }
#endif
#endif
}

Sensors::~Sensors( )
{
#if HAVE_SENSORS_SENSORS_H
    //Libsensor API cleanup
    if(libsensor_ok) sensors_cleanup();
#endif
}

void Sensors::getVal( TMdPrm *prm )
{
    bool devOK = false;
    string	s_id, s_val;

    //Using directly the Linux sensors
    // reading /sys/devices/virtual/thermal/*/temp
    DIR *IdDir = NULL;
    dirent *scan_dirent = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);
    if((IdDir=opendir(DIR_VDEV "thermal"))) {
	for(dirent *sRez = NULL; readdir_r(IdDir,scan_dirent,&sRez) == 0 && sRez; )
	    if((s_val=devRead(string("thermal")+"/"+sRez->d_name+"/temp")) != EVAL_STR) {
		if(!prm->vlPresent((s_id=string(sRez->d_name)+"_temp")))
		    fldAdd(new TFld(s_id.c_str(),string(sRez->d_name)+" temp",TFld::Real,TFld::NoWrite));
		prm->vlAt(s_id).at().setR(1e-3*s2r(s_val), 0, true);
		devOK = true;
	    }

	closedir(IdDir);
    }
    //  reading /sys/devices/virtual/hwmon/*/temp*
    if((IdDir=opendir(DIR_VDEV "hwmon"))) {
	dirent	*scan_dirent2 = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);
	for(dirent *sRez = NULL; readdir_r(IdDir,scan_dirent,&sRez) == 0 && sRez; ) {
	    if(!devChkAccess(string("hwmon")+"/"+sRez->d_name+"/temp1_input")) continue;
	    DIR *IdDir2 = opendir((string(DIR_VDEV "hwmon")+"/"+sRez->d_name).c_str());
	    if(IdDir2) {
		for(dirent *sRez2 = NULL; readdir_r(IdDir2,scan_dirent2,&sRez2) == 0 && sRez2; ) {
		    if(string(sRez2->d_name).find("temp") == 0 && (s_val=devRead(string("hwmon")+"/"+sRez->d_name+"/"+sRez2->d_name)) != EVAL_STR) {
			if(!prm->vlPresent((s_id=string(sRez->d_name)+"_"+sRez2->d_name)))
			    fldAdd(new TFld(s_id.c_str(),string(sRez->d_name)+" "+sRez2->d_name,TFld::Real,TFld::NoWrite));
			prm->vlAt(s_id).at().setR(1e-3*s2r(s_val), 0, true);
			devOK = true;
		    }
		}
		closedir(IdDir2);
	    }
	}

	closedir(IdDir);
	free(scan_dirent2);
    }
    free(scan_dirent);

    //Using libsensor
    if(libsensor_ok) {
#if HAVE_SENSORS_SENSORS_H
	int nr = 0;
	double	val;
	const sensors_chip_name	*name;
#if SENSORS_API_VERSION >= 0x400
	while((name=sensors_get_detected_chips(NULL,&nr))) {
	    const sensors_subfeature *feature = NULL;
	    const sensors_feature *main_feature;
	    int nr1 = 0;
	    while((main_feature=sensors_get_features(name, &nr1))) {
		switch(main_feature->type) {
		    case SENSORS_FEATURE_IN:
			feature = sensors_get_subfeature(name, main_feature, SENSORS_SUBFEATURE_IN_INPUT);
			break;
		    case SENSORS_FEATURE_FAN:
			feature = sensors_get_subfeature(name, main_feature, SENSORS_SUBFEATURE_FAN_INPUT);
			break;
		    case SENSORS_FEATURE_TEMP:
			feature = sensors_get_subfeature(name, main_feature, SENSORS_SUBFEATURE_TEMP_INPUT);
			break;
		    default: break;
		}
		if(!feature) continue;
		s_id = string(name->prefix)+"_"+main_feature->name;
		if(!prm->vlPresent(s_id))
		    fldAdd(new TFld(s_id.c_str(),(string(name->prefix)+" "+main_feature->name).c_str(),TFld::Real,TFld::NoWrite));
		if(sensors_get_value(name,feature->number,&val) == 0) prm->vlAt(s_id).at().setR(val, 0, true);
		devOK = true;
	    }
	}
#else
	while((name=sensors_get_detected_chips(&nr))) {
	    int nr1 = 0, nr2 = 0;
	    const sensors_feature_data *feature;
	    while((feature=sensors_get_all_features(*name,&nr1,&nr2)))
		if(sensors_get_ignored(*name,feature->number) == 1 && feature->mapping == SENSORS_NO_MAPPING) {
		    s_id = string(name->prefix) + "_" + feature->name;
		    if(!prm->vlPresent(s_id))
			fldAdd(new TFld(s_id.c_str(),(string(name->prefix)+" "+feature->name).c_str(), TFld::Real,TFld::NoWrite));
		    sensors_get_feature(*name, feature->number, &val);
		    prm->vlAt(s_id).at().setR(val, 0, true);
		    devOK = true;
		}
	}
#endif
#endif
    }
    //Using mbmon
    else {
	char buf[100], name[31];
	float val;
	FILE *fp = popen(mbmon_cmd, "r");
	if(fp == NULL) return;

	while(fgets(buf,sizeof(buf),fp)) {
	    if(sscanf(buf,"%31s : %f",name,&val) != 2) continue;
	    if(!prm->vlPresent(name))	fldAdd(new TFld(name,name,TFld::Real,TFld::NoWrite));
	    prm->vlAt(name).at().setR(val, 0, true);
	    devOK = true;
	}
	if(pclose(fp) == -1)
	    mess_warning(prm->nodePath().c_str(), _("Closing the pipe %p error '%s (%d)'!"), fp, strerror(errno), errno);
    }

    if(devOK) prm->daErr = "";
    else if(!prm->daErr.getVal().size()) {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}

void Sensors::makeActiveDA( TMdContr *aCntr, const string &dIdPref, const string &dNmPref )
{
    //Early checking for the corresponded parameter presence
    vector<string> pLs;
    aCntr->list(pLs);

    unsigned iP;
    for(iP = 0; iP < pLs.size(); iP++)
	if(aCntr->at(pLs[iP]).at().cfg("TYPE").getS() == id()) break;
    if(iP < pLs.size()) return;

    //Checking for the sensors presence
    bool sens_allow = false;

    // ... directly the Linux sensors
    //  Testing /sys/devices/virtual/thermal/*/temp
    DIR *IdDir = NULL;
    dirent *scan_dirent = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);
    if((IdDir=opendir(DIR_VDEV "thermal"))) {
	for(dirent *sRez = NULL; !sens_allow && readdir_r(IdDir,scan_dirent,&sRez) == 0 && sRez; )
	    sens_allow = (devChkAccess(string("thermal")+"/"+sRez->d_name+"/temp"));

	closedir(IdDir);
    }
    //  Testing /sys/devices/virtual/hwmon/*/temp*
    if(!sens_allow && (IdDir=opendir(DIR_VDEV "hwmon"))) {
	for(dirent *sRez = NULL; !sens_allow && readdir_r(IdDir,scan_dirent,&sRez) == 0 && sRez; )
	    sens_allow = (devChkAccess(string("hwmon")+"/"+sRez->d_name+"/temp1_input"));

	closedir(IdDir);
    }
    free(scan_dirent);

    // ... libsensor
    if(libsensor_ok) {
#if HAVE_SENSORS_SENSORS_H
	int nr = 0;
	const sensors_chip_name	*name;
#if SENSORS_API_VERSION >= 0x400
	while((name=sensors_get_detected_chips(NULL,&nr))) {
	    const sensors_subfeature *feature = NULL;
	    const sensors_feature *main_feature;
	    int nr1 = 0;
	    while((main_feature=sensors_get_features(name,&nr1))) {
		switch(main_feature->type) {
		    case SENSORS_FEATURE_IN:
			feature = sensors_get_subfeature(name,main_feature,SENSORS_SUBFEATURE_IN_INPUT);
			break;
		    case SENSORS_FEATURE_FAN:
			feature = sensors_get_subfeature(name, main_feature, SENSORS_SUBFEATURE_FAN_INPUT);
			break;
		    case SENSORS_FEATURE_TEMP:
			feature = sensors_get_subfeature(name, main_feature, SENSORS_SUBFEATURE_TEMP_INPUT);
			break;
		    default: break;
		}
		if(feature) { sens_allow |= true; break; }
	    }
	}
#else
	while(name = sensors_get_detected_chips(&nr)) {
	    const sensors_feature_data *feature;
	    int nr1 = 0, nr2 = 0;
	    while(feature = sensors_get_all_features(*name,&nr1,&nr2))
		if(sensors_get_ignored(*name,feature->number) == 1 && feature->mapping == SENSORS_NO_MAPPING)
		{ sens_allow |= true; break; }
	}
#endif
#endif
    }
    // ... monitor
    else {
	char buf[100], name[31];
	float val;
	FILE *fp = popen(mbmon_cmd, "r");
	if(fp != NULL) {
	    while(fgets(buf,sizeof(buf),fp))
		if(sscanf(buf,"%31s : %f",name,&val) == 2)
		{ sens_allow = true; break; }
	    if(pclose(fp) == -1)
		mess_warning(aCntr->nodePath().c_str(), _("Closing the pipe %p error '%s (%d)'!"), fp, strerror(errno), errno);
	}
    }

    //Sensor parameter creating
    if(sens_allow) DA::makeActiveDA(aCntr, "Sensors", _("Sensors"));
}

bool Sensors::devChkAccess( const string &file, const string &acs )
{
    FILE *f = fopen(TSYS::strMess(DIR_VDEV "%s",file.c_str()).c_str(), acs.c_str());
    bool rez = f;
    if(f && fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);

    return rez;
}

string Sensors::devRead( const string &file )
{
    char buf[256];
    string rez = EVAL_STR;
    FILE *f = fopen(TSYS::strMess(DIR_VDEV "%s",file.c_str()).c_str(), "r");
    if(f) {
	rez = "";
	while(fgets(buf,sizeof(buf),f) != NULL) rez.append(buf);
	if(rez.size() && rez[rez.size()-1] == '\n') rez.erase(rez.size()-1, 1);
	if(fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
    }

    return rez;
}
