
//OpenSCADA system module DAQ.OperationSystem file: da_uptime.cpp
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

#include <tsys.h>

#include "os_contr.h"
#include "da_uptime.h"

using namespace SystemCntr;

//======================================================================
//==== UpTime
//======================================================================
UpTime::UpTime( )
{
    st_tm = time(NULL);
    
    //Uptime value structure
    fldAdd( new TFld("full",_("Full seconds"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("sec",_("Seconds"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("min",_("Minutes"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("hour",_("Hours"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("day",_("Days"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
}

UpTime::~UpTime()
{

}
    
void UpTime::init( TMdPrm *prm )
{
    //Create config
    TCfg &c_subt = prm->cfg("SUBT");
    c_subt.fld().descr("");
			
    c_subt.fld().values("sys;stat"); 
    c_subt.fld().selNames(string(_("System"))+";"+_("Station"));
    try{ c_subt.getSEL(); }
    catch(...) { c_subt.setS("sys"); }
}

void UpTime::getVal( TMdPrm *prm )
{
    long val;
    
    string trg = prm->cfg("SUBT").getS();
	    
    if( trg == "sys" )
    {
        FILE *f = fopen("/proc/uptime","r");
        if( f == NULL ) return;
        fscanf(f,"%lu",&val);
        fclose(f);
    }
    else val = time(NULL) - st_tm;
    prm->vlAt("full").at().setI(val,0,true);
    prm->vlAt("day").at().setI(val/86400,0,true);
    prm->vlAt("hour").at().setI((val%86400)/3600,0,true);
    prm->vlAt("min").at().setI(((val%86400)%3600)/60,0,true);
    prm->vlAt("sec").at().setI(((val%86400)%3600)%60,0,true);
}														    

void UpTime::setEVAL( TMdPrm *prm )
{
    prm->vlAt("full").at().setI(EVAL_INT,0,true);
    prm->vlAt("day").at().setI(EVAL_INT,0,true);
    prm->vlAt("hour").at().setI(EVAL_INT,0,true);
    prm->vlAt("min").at().setI(EVAL_INT,0,true);
    prm->vlAt("sec").at().setI(EVAL_INT,0,true);
}

void UpTime::makeActiveDA( TMdContr *a_cntr )
{
    string ap_nm = "UpTimeSystem";    
    if(!a_cntr->present(ap_nm))
    {
        FILE *f = fopen("/proc/uptime","r");
        if( f != NULL )
	{
    	    a_cntr->add(ap_nm,0);
	    a_cntr->at(ap_nm).at().name(_("System up time"));
	    a_cntr->at(ap_nm).at().autoC(true);
    	    a_cntr->at(ap_nm).at().cfg("TYPE").setS(id());
	    a_cntr->at(ap_nm).at().cfg("SUBT").setS("sys");
    	    a_cntr->at(ap_nm).at().cfg("EN").setB(true);
    	    fclose(f);
	}
    }
    ap_nm = "UpTimeStation";
    if(!a_cntr->present(ap_nm))
    {
	a_cntr->add(ap_nm,0);
	a_cntr->at(ap_nm).at().name(_("Station up time"));
	a_cntr->at(ap_nm).at().autoC(true);
        a_cntr->at(ap_nm).at().cfg("TYPE").setS(id());
	a_cntr->at(ap_nm).at().cfg("SUBT").setS("stat");
	a_cntr->at(ap_nm).at().cfg("EN").setB(true);
    }
}									    
				
