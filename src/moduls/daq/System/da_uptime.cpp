
//OpenSCADA module DAQ.System file: da_uptime.cpp
/***************************************************************************
 *   Copyright (C) 2005-2023 by Roman Savochenko, <roman@oscada.org>       *
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
#include "da_uptime.h"

using namespace SystemCntr;

//*************************************************
//* UpTime                                        *
//*************************************************
UpTime::UpTime( )
{
    stTm = time(NULL);

    //Uptime value structure
    fldAdd(new TFld("full",trS("Full seconds"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("sec",trS("Seconds"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("min",trS("Minutes"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("hour",trS("Hours"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("day",trS("Days"),TFld::Integer,TFld::NoWrite));
}

UpTime::~UpTime( )	{ }

void UpTime::getVal( TMdPrm *prm )
{
    unsigned long val;
    bool devOK = false;

    string trg = prm->cfg("SUBT").getS();

    if(trg == "sys") {
	FILE *f = fopen("/proc/uptime","r");
	if(f == NULL) return;
	devOK = (fscanf(f,"%lu",&val) == 1);
	if(fclose(f) != 0)
	    mess_warning(prm->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
    }
    else { val = time(NULL) - stTm; devOK = true; }

    if(devOK) {
	prm->daErr = "";
	prm->vlAt("full").at().setI(val, 0, true);
	prm->vlAt("day").at().setI(val/86400, 0, true);
	prm->vlAt("hour").at().setI((val%86400)/3600, 0, true);
	prm->vlAt("min").at().setI(((val%86400)%3600)/60, 0, true);
	prm->vlAt("sec").at().setI(((val%86400)%3600)%60, 0, true);
    }
    else if(!prm->daErr.getVal().size()) {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}

void UpTime::dList( vector<string> &list, TMdPrm *prm )
{
    list.push_back(string("stat;")+_("Station"));
    FILE *f = fopen("/proc/uptime","r");
    if(f) list.push_back(string("sys;")+_("System"));
    if(f && fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
}
