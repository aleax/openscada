
//OpenSCADA system module DAQ.System file: da_mem.cpp
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#include <tsys.h>

#include "os_contr.h"
#include "da_mem.h"

using namespace SystemCntr;

//*************************************************
//* Mem: Memory                                   *
//*************************************************
Mem::Mem( )
{
    //- Memory value structure -
    fldAdd( new TFld("free",_("Free (kB)"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("total",_("Total (kB)"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("use",_("Use (kB)"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("buff",_("Buffers (kB)"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("cache",_("Cached (kB)"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("sw_free",_("Swap free (kB)"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("sw_total",_("Swap total (kB)"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
    fldAdd( new TFld("sw_use",_("Swap use (kB)"),TFld::Integer,TFld::NoWrite,"",TSYS::int2str(EVAL_INT).c_str()) );
}

Mem::~Mem( )
{

}

void Mem::init( TMdPrm *prm ) 
{
    prm->cfg("SUBT").setView(false);
}

void Mem::deInit( TMdPrm *prm )
{
    prm->cfg("SUBT").setView(true);
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
    
    prm->vlAt("free").at().setI(m_free+m_buff+m_cach,0,true);
    prm->vlAt("total").at().setI(m_total,0,true);
    prm->vlAt("use").at().setI(m_total-m_free-m_buff-m_cach,0,true);
    prm->vlAt("buff").at().setI(m_buff,0,true);
    prm->vlAt("cache").at().setI(m_cach,0,true);
    prm->vlAt("sw_free").at().setI(sw_free,0,true);
    prm->vlAt("sw_total").at().setI(sw_total,0,true);
    prm->vlAt("sw_use").at().setI(sw_total-sw_free,0,true);			
}

void Mem::makeActiveDA( TMdContr *a_cntr )
{
    string ap_nm = "MemInfo";
    
    if(!a_cntr->present(ap_nm))
    {
	FILE *f = fopen("/proc/meminfo","r");
	if( f != NULL )
	{
	    a_cntr->add(ap_nm,0);
	    a_cntr->at(ap_nm).at().setName(_("Memory info"));
	    a_cntr->at(ap_nm).at().autoC(true);
    	    a_cntr->at(ap_nm).at().cfg("TYPE").setS(id());
    	    a_cntr->at(ap_nm).at().cfg("EN").setB(true);
	    fclose(f);
	}
    }
}
