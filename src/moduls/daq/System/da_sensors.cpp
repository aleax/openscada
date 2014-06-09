
//OpenSCADA system module DAQ.System file: da_sensors.cpp
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
    //> Libsensor API init
#if SENSORS_API_VERSION >= 0x400
    if(sensors_init(NULL) == 0) libsensor_ok = true;
#else
    FILE *f = fopen("/etc/sensors.conf", "r");
    if(f)
    {
	if(sensors_init(f) == 0) libsensor_ok = true;
	fclose(f);
    }
#endif
#endif
}

Sensors::~Sensors( )
{
#if HAVE_SENSORS_SENSORS_H
    //> Libsensor API cleanup
    if(libsensor_ok) sensors_cleanup();
#endif
}

void Sensors::init( TMdPrm *prm )
{
    prm->cfg("SUBT").setView(false);
    getSensors(prm, true);
}

void Sensors::deInit( TMdPrm *prm )
{
    prm->cfg("SUBT").setView(true);
}

void Sensors::getVal( TMdPrm *prm )
{
    getSensors(prm);
}

void Sensors::getSensors( TMdPrm *prm, bool onlyCreate )
{
    bool devOK = false;
    //> Use libsensor
    if(libsensor_ok)
    {
#if HAVE_SENSORS_SENSORS_H
	int nr = 0;
	double val;
	string	s_id;
	const sensors_chip_name	*name;
#if SENSORS_API_VERSION >= 0x400
        while((name=sensors_get_detected_chips(NULL,&nr)))
	{
	    const sensors_subfeature *feature = NULL;
	    const sensors_feature *main_feature;
	    int nr1 = 0;
	    while((main_feature=sensors_get_features(name, &nr1)))
	    {
		switch(main_feature->type)
		{
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
		if(!feature) continue;
		s_id = string(name->prefix)+"_"+main_feature->name;
		if(!prm->vlPresent(s_id))
		    fldAdd( new TFld(s_id.c_str(),(string(name->prefix)+" "+main_feature->name).c_str(),TFld::Real,TFld::NoWrite) );
		if(!onlyCreate && sensors_get_value(name, feature->number, &val) == 0)
		    prm->vlAt(s_id).at().setR(val,0,true);
		devOK = true;
	    }
	}
#else
	while(name = sensors_get_detected_chips(&nr))
	{
	    int nr1 = 0, nr2 = 0;
	    const sensors_feature_data *feature;
	    while( feature = sensors_get_all_features( *name, &nr1, &nr2 ) )
		if( sensors_get_ignored( *name, feature->number ) == 1 && feature->mapping == SENSORS_NO_MAPPING )
		{
		    s_id = string(name->prefix)+"_"+feature->name;
		    if( !prm->vlPresent(s_id) )
			fldAdd( new TFld(s_id.c_str(),(string(name->prefix)+" "+feature->name).c_str(), TFld::Real,TFld::NoWrite) );
		    if( !onlyCreate )
		    {
			sensors_get_feature( *name, feature->number, &val);
			prm->vlAt(s_id).at().setR(val,0,true);
		    }
		    devOK = true;
		}
	}
#endif
#endif
    }
    //> Use mbmon
    else
    {
	char buf[100], name[31];
	float val;
	FILE *fp = popen( mbmon_cmd, "r" );
	if( fp == NULL ) return;

	while( fgets(buf,sizeof(buf),fp) )
	{
	    if( sscanf(buf, "%31s : %f", name, &val) != 2 ) continue;
	    if( !prm->vlPresent(name) )	fldAdd( new TFld(name,name,TFld::Real,TFld::NoWrite) );
	    if( !onlyCreate ) prm->vlAt(name).at().setR(val,0,true);
	    devOK = true;
	}
	pclose(fp);
    }

    if(devOK) prm->daErr = "";
    else if(!onlyCreate && !prm->daErr.getVal().size())
    {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}

void Sensors::makeActiveDA( TMdContr *aCntr )
{
    char buf[100], name[31];
    float val;
    string ap_nm = "SensorsData";

    vector<string> pLs;
    // Find propper parameter's object
    aCntr->list(pLs);

    unsigned i_p;
    for(i_p = 0; i_p < pLs.size(); i_p++)
	if(aCntr->at(pLs[i_p]).at().cfg("TYPE").getS() == id()) break;
    if(i_p < pLs.size()) return;
    while(aCntr->present(ap_nm)) ap_nm = TSYS::strLabEnum(ap_nm);

    bool sens_allow = false;
    //> Use libsensor check
    if(libsensor_ok)
    {
#if HAVE_SENSORS_SENSORS_H
	int nr = 0;
	const sensors_chip_name	*name;
#if SENSORS_API_VERSION >= 0x400
	while((name=sensors_get_detected_chips(NULL,&nr)))
	{
	    const sensors_subfeature *feature = NULL;
	    const sensors_feature *main_feature;
	    int nr1 = 0;
	    while((main_feature = sensors_get_features(name, &nr1)))
	    {
		switch(main_feature->type)
		{
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
	while(name = sensors_get_detected_chips(&nr))
	{
	    const sensors_feature_data *feature;
	    int nr1 = 0, nr2 = 0;
	    while(feature = sensors_get_all_features(*name,&nr1,&nr2))
		if(sensors_get_ignored(*name,feature->number) == 1 && feature->mapping == SENSORS_NO_MAPPING)
		{ sens_allow |= true; break; }
	}
#endif
#endif
    }
    //> Check monitor present
    else
    {
	FILE *fp = popen(mbmon_cmd, "r");
	if(fp != NULL)
	{
	    while(fgets(buf,sizeof(buf),fp))
		if(sscanf(buf,"%31s : %f",name,&val) == 2)
		{ sens_allow = true; break; }
	    pclose(fp);
	}
    }
    //> Sensor parameter create
    if(sens_allow)
    {
	aCntr->add(ap_nm, 0);
	AutoHD<TMdPrm> dprm = aCntr->at(ap_nm);
	dprm.at().setName(_("Data sensors"));
	dprm.at().autoC(true);
	dprm.at().cfg("TYPE").setS(id());
	dprm.at().cfg("EN").setB(true);
	if(aCntr->enableStat()) dprm.at().enable();
    }
}
