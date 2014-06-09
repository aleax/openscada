
//OpenSCADA system file: main.cpp
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *                                                     *
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

#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "terror.h"
#include "tmess.h"
#include "tsys.h"

using namespace OSCADA;

int main( int argc, char *argv[], char *envp[] )
{
    int rez = 0;
    SYS = NULL;

    try
    {
	//> OpenSCADA system's root object creation
	SYS = new TSYS(argc, argv, envp);

	//> Generic command line parameters check
	string argCom, argVl;
	for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
	    if(argCom == "demon")
	    {
		printf("Start into demon mode!\n");
		int pid = fork();
		if(pid == -1)
		{
		    printf("Error: fork error!\n");
		    return -1;
		}
		if(pid != 0) return 0;

		//>> Prepare demon environment
		setsid();
	    }
	    else if(argCom == "CoreDumpAllow")
	    {
		//>> Set the Dumpable state to be enabled
		prctl(PR_SET_DUMPABLE, 1, 0, 0, 0);
		//>> Set the core dump limitation to be unlimited
		struct rlimit rlim;
		rlim.rlim_cur = RLIM_INFINITY;
		rlim.rlim_max = RLIM_INFINITY;
		setrlimit(RLIMIT_CORE, &rlim);
	    }

	//> Load system
	SYS->load();
	if((rez=SYS->stopSignal()) > 0) throw TError(SYS->nodePath().c_str(),"Stop by signal %d on load.",rez);
	//> Start system
	rez = SYS->start();
    }catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    //> Free OpenSCADA system's root object
    if(SYS) delete SYS;

    printf("OpenSCADA system is correctly exited by cause %d.\n",rez);

    return rez;
}
