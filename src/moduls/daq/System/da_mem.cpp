
//OpenSCADA module DAQ.System file: da_mem.cpp
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

#include <tsys.h>

#include "os_contr.h"
#include "da_mem.h"

using namespace SystemCntr;

//*************************************************
//* Mem: Memory                                   *
//*************************************************
Mem::Mem( )
{
    //Memory value structure
    fldAdd(new TFld("free",trS("Free (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("total",trS("Total (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("use",trS("Used (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("buff",trS("Buffers (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("cache",trS("Cached (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("sw_free",trS("Swap free (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("sw_total",trS("Swap total (KiB)"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("sw_use",trS("Swap used (KiB)"),TFld::Integer,TFld::NoWrite));
}

Mem::~Mem( )	{ }

void Mem::getVal( TMdPrm *prm )
{
    int m_total, m_free, m_buff, m_cach, sw_total, sw_free;
    char buf[256];

    FILE *f = fopen("/proc/meminfo","r");
    m_total = m_free = m_buff = m_cach = sw_total = sw_free = 0;
    while(f && fgets(buf,sizeof(buf),f) != NULL)
	if(!m_total && sscanf(buf,"MemTotal: %d kB\n",&m_total)) ;
	else if(!m_free && sscanf(buf,"MemFree: %d kB\n",&m_free)) ;
	else if(!m_buff && sscanf(buf,"Buffers: %d kB\n",&m_buff)) ;
	else if(!m_cach && sscanf(buf,"Cached: %d kB\n",&m_cach)) ;
	else if(!sw_total && sscanf(buf,"SwapTotal: %d kB\n",&sw_total)) ;
	else if(!sw_free && sscanf(buf,"SwapFree: %d kB\n",&sw_free)) ;

    if(f && fclose(f) != 0)
	mess_warning(prm->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);

    if(m_total || m_free || m_buff || m_cach || sw_total || sw_free) {
	prm->daErr = "";
	prm->vlAt("free").at().setI(m_free+m_buff+m_cach, 0, true);
	prm->vlAt("total").at().setI(m_total, 0, true);
	prm->vlAt("use").at().setI(m_total-m_free-m_buff-m_cach, 0, true);
	prm->vlAt("buff").at().setI(m_buff, 0, true);
	prm->vlAt("cache").at().setI(m_cach, 0, true);
	prm->vlAt("sw_free").at().setI(sw_free, 0, true);
	prm->vlAt("sw_total").at().setI(sw_total, 0, true);
	prm->vlAt("sw_use").at().setI(sw_total-sw_free, 0, true);
    }
    else if(!prm->daErr.getVal().size()) {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}
