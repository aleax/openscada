
//OpenSCADA module DAQ.System file: da_fs.cpp
/***************************************************************************
 *   Copyright (C) 2016-2023 by Roman Savochenko, <roman@oscada.org>       *
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

#include <sys/vfs.h>
#include <string.h>
#include <errno.h>

#include <tsys.h>

#include "os_contr.h"
#include "da_fs.h"

using namespace SystemCntr;

//*************************************************
//* FS                                            *
//*************************************************
FS::FS( )
{
    fldAdd(new TFld("total",trS("Total (GB)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("used",trS("Used (GB)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("free",trS("Free (GB)"),TFld::Real,TFld::NoWrite));
    fldAdd(new TFld("totalN",trS("Total file nodes"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("usedN",trS("Used file nodes"),TFld::Integer,TFld::NoWrite));
    fldAdd(new TFld("freeN",trS("Free file nodes"),TFld::Integer,TFld::NoWrite));
}

FS::~FS( )	{ }

void FS::getVal( TMdPrm *prm )
{
    struct statfs	st;
    bool mOK = !statfs(prm->cfg("SUBT").getS().c_str(), &st);

    if(mOK) {
	prm->daErr = "";
	prm->vlAt("total").at().setR(1e-9*st.f_blocks*st.f_bsize, 0, true);
	prm->vlAt("used").at().setR(1e-9*(st.f_blocks-st.f_bfree)*st.f_bsize, 0, true);
	prm->vlAt("free").at().setR(1e-9*st.f_bfree*st.f_bsize, 0, true);
	prm->vlAt("totalN").at().setI(st.f_files, 0, true);
	prm->vlAt("usedN").at().setI(st.f_files-st.f_ffree, 0, true);
	prm->vlAt("freeN").at().setI(st.f_ffree, 0, true);
    }
    else if(!prm->daErr.getVal().size()) {
	prm->setEval();
	prm->daErr = _("10:Mountpoint isn't available.");
    }
}

void FS::dList( vector<string> &list, TMdPrm *prm )
{
    char name[512];
    char buf[1024], *s;

    FILE *f = fopen("/etc/fstab", "r");
    while(f && fgets(buf,sizeof(buf),f) != NULL) {
	s = buf;
	while(isblank(*s)) ++s;	//Walk through blank in begin
	if(*s == '\0' || *s == '#' || *s == '\n') continue;	//Pass comments and wrong lines
	name[0] = 0;
	sscanf(s, "%*s %511s %*s %*s", name);
	if(name[0] == 0 || !strcmp(name, "devpts") || !strcmp(name, "swap") || !strcmp(name, "proc") ||
		!strcmp(name, "sysfs") || !strcmp(name, "usbdevfs") || !strcmp(name, "usbfs") || !strcmp(name, "ignore"))
	    continue;
	list.push_back(name);
    }

    if(f && fclose(f) != 0) mess_warning(mod->nodePath().c_str(), _("Closing the file %p error '%s (%d)'!"), f, strerror(errno), errno);
}
