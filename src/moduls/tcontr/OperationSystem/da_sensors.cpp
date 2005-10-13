/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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

#include "sys.h"
#include "da_sensors.h"

using namespace SystemCntr;
 
//======================================================================
//==== Lmsensors
//======================================================================
Lmsensors::Lmsensors( ) : s_path("/proc/sys/dev/sensors/")
{
    //Sensors value structure
    fldAdd( new TFld("value","",TFld::Real,FLD_NWR,"8.2","0") );
}

Lmsensors::~Lmsensors()
{

}

void Lmsensors::init( TMdPrm *prm )
{
    TCfg &c_subt = prm->cfg("SUBT");
    //Create config
    c_subt.fld().descr() = mod->I18N("Sensor");
    c_subt.fld().selValI().clear();
    c_subt.fld().selNm().clear();
    
    vector<string> list;
    dList(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	c_subt.fld().selValI().push_back(i_l);
	c_subt.fld().selNm().push_back(list[i_l]);
    }
    if( list.size() ) c_subt.setSEL(list[0]);        
}

void Lmsensors::dList( vector<string> &list )
{   
    dirent *s_dirent;
    
    DIR *IdDir = opendir(s_path.c_str());
    if(IdDir == NULL) return;    
    
    while((s_dirent = readdir(IdDir)) != NULL)
    {
	if( string("..") == s_dirent->d_name || string(".") == s_dirent->d_name ) continue;
	//Open temperatures
	for( int i_el = 0;i_el < 20;i_el++)
	{
	    string s_file = string(s_dirent->d_name)+"/temp"+TSYS::int2str(i_el);
	    if( access((s_path+s_file).c_str(),R_OK) ) continue;
	    list.push_back(s_file);
	}	
	//Open fans
	for( int i_el = 0;i_el < 20;i_el++)
	{
	    string s_file = string(s_dirent->d_name)+"/fan"+TSYS::int2str(i_el);
	    if( access((s_path+s_file).c_str(),R_OK) ) continue;
	    list.push_back(s_file);
	}	
	//Open powers
	for( int i_el = 0;i_el < 20;i_el++)
	{
	    string s_file = string(s_dirent->d_name)+"/in"+TSYS::int2str(i_el);
	    if( access((s_path+s_file).c_str(),R_OK) ) continue;
	    list.push_back(s_file);
	}	
    }
    closedir(IdDir);
}

void Lmsensors::getVal( TMdPrm *prm )
{    
    float max,min,val;
    
    TCfg &c_subt = prm->cfg("SUBT");
    string sens =  c_subt.getSEL();
    string tp   =  sens.substr(sens.find("/",0)+1);

    FILE *f = fopen((s_path+sens).c_str(),"r");
    if( f == NULL ) return;
    if( tp.find("temp",0) == 0 )
    {
	if( fscanf(f,"%f %f %f",&max,&min,&val) == 3 )
    	    prm->vlAt("value").at().setR(val,NULL,true);
    }
    else if( tp.find("fan",0) == 0 )
    {
	if( fscanf(f,"%f %f",&min,&val) == 2 )
	    prm->vlAt("value").at().setR(val,NULL,true);
    }
    else if( tp.find("in",0) == 0 )
    {
	if( fscanf(f,"%f %f %f",&min,&max,&val) == 3 )
    	    prm->vlAt("value").at().setR(val,NULL,true);
    }
    fclose(f);
}

void Lmsensors::chCfg( TMdPrm *prm, TCfg &i_cfg )
{
    if( i_cfg.name() == "SUBT" )
    {
	TCfg &c_subt = prm->cfg("SUBT");
	string sens =  c_subt.getSEL();
	sens = sens.substr(sens.find("/",0)+1);
	if( sens.find("temp",0) == 0 )	prm->vlAt("value").at().fld().descr() = mod->I18N("Temperature");
	else if( sens.find("fan",0) == 0 ) 	prm->vlAt("value").at().fld().descr() = mod->I18N("Fan turns");
	else if( sens.find("in",0) == 0 )  	prm->vlAt("value").at().fld().descr() = mod->I18N("Voltage");
    }
} 

void Lmsensors::makeActiveDA( TController *a_cntr )
{

}

