
//OpenSCADA module DAQ.System file: da_netstat.cpp
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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <tsys.h>

#include "os_contr.h"
#include "da_netstat.h"

using namespace SystemCntr;

//*************************************************
//* HddStat                                       *
//*************************************************
NetStat::NetStat( )
{
    fldAdd(new TFld("rcv",trS("Received (B)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("rcvSp",trS("Receive speed (B/s)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("trns",trS("Transmitted (B)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("trnsSp",trS("Transmit speed (B/s)"),TFld::Real,TFld::NoWrite));
}

NetStat::~NetStat( )	{ }

void NetStat::getVal( TMdPrm *prm )
{
    unsigned long rcv, trns;
    double rcvVl = 0, trnsVl = 0;
    char sc_pat[50] = "",
	 buf[256] = "";
    bool devOK = false;

    string dev = prm->cfg("SUBT").getS();
    FILE *f = fopen("/proc/net/dev","r");
    if(f) {
	snprintf(sc_pat,sizeof(sc_pat)," %s %%lu %%*d %%*d %%*d %%*d %%*d %%*d %%*d %%lu",dev.c_str());
	while(fgets(buf,sizeof(buf),f) != NULL) {
	    for(unsigned i = 0; i < sizeof(buf); i++)
		if(buf[i] == ':') buf[i] = ' ';
	    if(!sscanf(buf,sc_pat,&rcv,&trns)) continue;
	    rcvVl = rcv;
	    trnsVl = trns;
	    devOK = true;
	    break;
	}
	if(fclose(f) != 0)
	    mess_warning(prm->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
    }

    if(devOK) {
	prm->daErr = "";
	double lstVl = prm->vlAt("rcv").at().getR(0, true);
	prm->vlAt("rcvSp").at().setR((lstVl != EVAL_REAL && rcvVl > lstVl) ?
		1e6*(rcvVl-lstVl)/vmax(1,TSYS::curTime()-prm->vlAt("rcv").at().time()) : 0, 0, true);
	lstVl = prm->vlAt("trns").at().getR(0, true);
	prm->vlAt("trnsSp").at().setR((lstVl != EVAL_REAL && trnsVl > lstVl) ?
		1e6*(trnsVl-lstVl)/vmax(1,TSYS::curTime()-prm->vlAt("trns").at().time()) : 0, 0, true);

	prm->vlAt("rcv").at().setR(rcvVl,0,true);
	prm->vlAt("trns").at().setR(trnsVl,0,true);
    }
    else if(!prm->daErr.getVal().size()) {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}

void NetStat::dList( vector<string> &list, TMdPrm *prm )
{
    long unsigned int rcv, trns;
    char name[11] = "",
	 buf[256] = "";

    FILE *f = fopen("/proc/net/dev","r");
    while(f && fgets(buf,sizeof(buf),f) != NULL) {
	for(unsigned i = 0; i < sizeof(buf); i++)
	    if(buf[i] == ':') buf[i] = ' ';
	if(sscanf(buf,"%10s %lu %*d %*d %*d %*d %*d %*d %*d %lu",name,&rcv,&trns) != 3) continue;
	list.push_back(name);
    }
    if(f && fclose(f) != 0)
	mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
}
