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

#include "os_contr.h"
#include "da_mem.h"

using namespace SystemCntr;

//======================================================================
//==== Memory
//======================================================================
Mem::Mem( )
{
    //Memory value structure
    fldAdd( new TFld("free",mod->I18N("Free (kB)"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("total",mod->I18N("Total (kB)"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("use",mod->I18N("Use (kB)"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("buff",mod->I18N("Buffers (kB)"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("cache",mod->I18N("Cached (kB)"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("sw_free",mod->I18N("Swap free (kB)"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("sw_total",mod->I18N("Swap total (kB)"),TFld::Dec,FLD_NWR,"","0") );
    fldAdd( new TFld("sw_use",mod->I18N("Swap use (kB)"),TFld::Dec,FLD_NWR,"","0") );
}

Mem::~Mem()
{

}

void Mem::init( TMdPrm *prm ) 
{
    prm->cfg("SUBT").view(false);
}

void Mem::deInit( TMdPrm *prm )
{
    prm->cfg("SUBT").view(true);
}    

void Mem::getVal( TMdPrm *prm )
{    
    long m_total, m_free, m_buff, m_cach, sw_total, sw_free;
    char buf[256];
    
    FILE *f = fopen("/proc/meminfo","r");
    if( f == NULL ) return;
    
    while( fgets(buf,sizeof(buf),f) != NULL )
    {
	sscanf(buf,"MemTotal: %d kB\n",&m_total);
	sscanf(buf,"MemFree: %d kB\n",&m_free);
	sscanf(buf,"Buffers: %d kB\n",&m_buff);
	sscanf(buf,"Cached: %d kB\n",&m_cach);
	sscanf(buf,"SwapTotal: %d kB\n",&sw_total);
	sscanf(buf,"SwapFree: %d kB\n",&sw_free);
    }
    fclose(f);
    
    prm->vlAt("free").at().setI(m_free+m_buff+m_cach,NULL,true);
    prm->vlAt("total").at().setI(m_total,NULL,true);
    prm->vlAt("use").at().setI(m_total-m_free-m_buff-m_cach,NULL,true);
    prm->vlAt("buff").at().setI(m_buff,NULL,true);
    prm->vlAt("cache").at().setI(m_cach,NULL,true);
    prm->vlAt("sw_free").at().setI(sw_free,NULL,true);
    prm->vlAt("sw_total").at().setI(sw_total,NULL,true);
    prm->vlAt("sw_use").at().setI(sw_total-sw_free,NULL,true);			
}

void Mem::makeActiveDA( TController *a_cntr )
{
    string ap_nm = "MemInfo";
    
    if(!a_cntr->present(ap_nm))
    {
	FILE *f = fopen("/proc/meminfo","r");
	if( f != NULL )
	{
	    a_cntr->add(ap_nm,0);
    	    a_cntr->at(ap_nm).at().cfg("TYPE").setS(id());
    	    a_cntr->at(ap_nm).at().cfg("EN").setB(true);
	    fclose(f);
	}
    }
}
