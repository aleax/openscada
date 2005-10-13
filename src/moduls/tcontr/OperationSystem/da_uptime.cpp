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

#include <tsys.h>

#include "sys.h"
#include "da_uptime.h"

using namespace SystemCntr;

//======================================================================
//==== UpTime
//======================================================================
UpTime::UpTime( )
{
    st_tm = time(NULL);
    
    //Uptime value structure
    fldAdd( new TFld("value",mod->I18N("Full seconds"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("sec",mod->I18N("Seconds"),TFld::Dec,FLD_NWR,"2","0") );
    fldAdd( new TFld("min",mod->I18N("Minutes"),TFld::Dec,FLD_NWR,"2","0") );
    fldAdd( new TFld("hour",mod->I18N("Hours"),TFld::Dec,FLD_NWR,"2","0") );
    fldAdd( new TFld("day",mod->I18N("Days"),TFld::Dec,FLD_NWR,"","0") );
}

UpTime::~UpTime()
{

}
    
void UpTime::init( TMdPrm *prm )
{
    //Create config
    TCfg &c_subt = prm->cfg("SUBT");
    c_subt.fld().descr() = "";
    c_subt.fld().selValI().clear();
    c_subt.fld().selNm().clear();
			
    c_subt.fld().selValI().push_back(0); c_subt.fld().selNm().push_back(mod->I18N("System"));
    c_subt.fld().selValI().push_back(1); c_subt.fld().selNm().push_back(mod->I18N("Station"));
    c_subt.setI(0);
}

void UpTime::getVal( TMdPrm *prm )
{
    long val;
    
    TCfg &c_subt = prm->cfg("SUBT");
    int trg = c_subt.getI();
	    
    if( trg == 0 )
    {
        FILE *f = fopen("/proc/uptime","r");
        if( f == NULL ) return;
        fscanf(f,"%lu",&val);
        fclose(f);
    }
    else val = time(NULL) - st_tm;
    prm->vlAt("value").at().setI(val,NULL,true);
    prm->vlAt("day").at().setI(val/86400,NULL,true);
    prm->vlAt("hour").at().setI((val%86400)/3600,NULL,true);
    prm->vlAt("min").at().setI(((val%86400)%3600)/60,NULL,true);
    prm->vlAt("sec").at().setI(((val%86400)%3600)%60,NULL,true);
}														    

void UpTime::makeActiveDA( TController *a_cntr )
{
    string ap_nm = "UpTimeSystem";    
    if(!a_cntr->present(ap_nm))
    {
        FILE *f = fopen("/proc/uptime","r");
        if( f != NULL )
	{
    	    a_cntr->add(ap_nm,0);
    	    a_cntr->at(ap_nm).at().cfg("TYPE").setS(id());
	    a_cntr->at(ap_nm).at().cfg("SUBT").setSEL(mod->I18N("System"));
    	    a_cntr->at(ap_nm).at().cfg("EN").setB(true);
    	    fclose(f);
	}
    }
    ap_nm = "UpTimeStation";
    if(!a_cntr->present(ap_nm))
    {
	a_cntr->add(ap_nm,0);
        a_cntr->at(ap_nm).at().cfg("TYPE").setS(id());
	a_cntr->at(ap_nm).at().cfg("SUBT").setSEL(mod->I18N("Station"));
	a_cntr->at(ap_nm).at().cfg("EN").setB(true);			    
    }
}									    
				
