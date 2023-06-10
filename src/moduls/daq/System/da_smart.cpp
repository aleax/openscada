
//OpenSCADA module DAQ.System file: da_smart.cpp
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
#include "da_smart.h"

#define SCSI_MAJOR 8

using namespace SystemCntr;

const char *HddSmart::smartval_cmd = "smartctl -A -v N,raw48 %s 2> /dev/null";

//*************************************************
//* HddSmart                                      *
//*************************************************
HddSmart::HddSmart( )	{ }

HddSmart::~HddSmart( )	{ }

void HddSmart::getVal( TMdPrm *prm )
{
    int id;
    unsigned long val;
    char buf[256];
    char name[31];
    bool devOK = false;

    string dev = prm->cfg("SUBT").getS();

    //SMART attributes
    string cmd = TSYS::strMess(smartval_cmd,("/dev/"+dev+((dev.size()&&dev[0]=='s')?" -d ata":"")).c_str());
    FILE *fp = popen(cmd.c_str(),"r");
    while(fp && fgets(buf,sizeof(buf),fp) != NULL) {
	if(sscanf(buf,"%d %30s %*x %*d %*d %*d %*s %*s %*s %lu\n",&id,name,&val) != 3) continue;
	string sId = i2s(id);
	if(!prm->vlPresent(sId))
	    ((tval*)prm->daData)->els.fldAdd(new TFld(sId.c_str(),(string(name)+" ("+sId+")").c_str(),TFld::Integer,TFld::NoWrite));
	prm->vlAt(sId).at().setI(val,0,true);
	devOK = true;
    }
    if(fp && pclose(fp) == -1)
	mess_warning(prm->nodePath().c_str(), _("Closing the pipe %p error '%s (%d)'!"), fp, strerror(errno), errno);

    if(devOK) prm->daErr = "";
    else if(!prm->daErr.getVal().size()) {
	prm->setEval();
	prm->daErr = _("10:Device is not available.");
    }
}

void HddSmart::dList( vector<string> &list, TMdPrm *prm )
{
    int major, minor;
    char name[11];
    char buf[256];

    FILE *f = fopen("/proc/partitions","r");
    if(f == NULL) return;

    while(fgets(buf,sizeof(buf),f) != NULL) {
	if(sscanf(buf,"%d %d %*d %10s",&major,&minor,name) != 3) continue;
	if(/*!part &&*/ ((major != SCSI_MAJOR && minor != 0) || (major == SCSI_MAJOR && (minor%16)) || !strncmp(name,"md",2)))
	    continue;

	string cmd = TSYS::strMess(smartval_cmd,(string("/dev/")+name+((major==SCSI_MAJOR)?" -d ata":"")).c_str());
	FILE *fp = popen(cmd.c_str(),"r");
	if(fp) {
	    int val;
	    bool access_true = false;
	    while(fgets(buf,sizeof(buf),fp) != NULL) {
		if(sscanf(buf,"%*d %*s %*x %*d %*d %*d %*s %*s %*s %d\n",&val) != 1) continue;
		access_true = true;
		break;
	    }
	    if(pclose(fp) == -1) mess_warning(mod->nodePath().c_str(), _("Closing the pipe %p error '%s (%d)'!"), fp, strerror(errno), errno);
	    if(access_true) list.push_back(name);
	}
    }
    if(fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
}
