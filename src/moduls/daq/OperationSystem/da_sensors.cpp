
//OpenSCADA system module DAQ.OperationSystem file: da_sensors.cpp
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#include "os_contr.h"
#include "da_sensors.h"

using namespace SystemCntr;

char *Sensors::mbmon_cmd = "mbmon -r -c 1";	//write one try to stdout
 
//======================================================================
//==== Sensors
//======================================================================
Sensors::Sensors( )
{

}

Sensors::~Sensors()
{

}

void Sensors::init( TMdPrm *prm )
{
    prm->cfg("SUBT").view(false);
}

void Sensors::deInit( TMdPrm *prm )
{
    prm->cfg("SUBT").view(true);
}	

void Sensors::getVal( TMdPrm *prm )
{    
    char buf[100], name[31];
    float val;
    FILE *fp = popen(mbmon_cmd,"r");
    if( fp == NULL ) return;
    
    while(fgets(buf,sizeof(buf),fp))
    {
	if( sscanf(buf, "%31s : %f", name, &val) != 2 ) continue;
	if(!prm->vlPresent(name))
	    fldAdd( new TFld(name,name,TFld::Real,TFld::NoWrite,"",TSYS::real2str(EVAL_REAL).c_str()) );
	prm->vlAt(name).at().setR(val,0,true);
    }
    pclose(fp);
}

void Sensors::setEVAL( TMdPrm *prm )
{
    char buf[100], name[31];
    FILE *fp = popen(mbmon_cmd,"r");
    if( fp == NULL ) return;
    
    while(fgets(buf,sizeof(buf),fp))
	if( sscanf(buf, "%31s : %*f", name) == 1 && prm->vlPresent(name) )
	    prm->vlAt(name).at().setR(EVAL_REAL,0,true);
    
    pclose(fp);
}	

void Sensors::makeActiveDA( TMdContr *a_cntr )
{
    char buf[100], name[31];
    float val;
    string ap_nm = "SensorsData";

    if( !a_cntr->present(ap_nm) )
    {	
	FILE *fp = popen(mbmon_cmd,"r");
	if( fp != NULL )
	{
	    //Check monitor present
	    bool sens_avoid = false;
	    while(fgets(buf,sizeof(buf),fp))
		if( sscanf(buf, "%31s : %f", name, &val) == 2 )
		{
		    sens_avoid = true;
		    break;
		}
	    if( sens_avoid )
	    {	
		a_cntr->add(ap_nm,0);
		a_cntr->at(ap_nm).at().name(mod->I18N("Data sensors"));
		a_cntr->at(ap_nm).at().autoC(true);
    		a_cntr->at(ap_nm).at().cfg("TYPE").setS(id());
    		a_cntr->at(ap_nm).at().cfg("EN").setB(true);
	    }
	    pclose(fp);
	}	
    }
}

