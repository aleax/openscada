
//OpenSCADA system file: main.cpp
/***************************************************************************
 *   Copyright (C) 2003-2009 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#include <getopt.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "terror.h"
#include "tmess.h"
#include "tsys.h"

using namespace OSCADA;

int main(int argc, char *argv[], char *envp[] )
{
    int rez = 0;

    //Check demon mode start and CoreDump allow enable
    int next_opt;
    optind=opterr=0;
    struct option long_opt[] =
	{
	    { "demon", 0, NULL, 'd' },
	    { "CoreDumpAllow", 0, NULL, 'c'},
	    { NULL, 0, NULL, 0 }
	};

    while((next_opt=getopt_long(argc,argv,"",long_opt,NULL)) != -1)
	if(next_opt == 'd')
	{
	    printf("Start into demon mode!\n");
	    int pid = fork();
	    if(pid == -1)
	    {
		printf("Error: fork error!\n");
		return -1;
	    }
	    if(pid != 0) return 0;

	    //Prepare demon environment
	    setsid();

	    break;
	}
	else if(next_opt == 'c')
	{
	    // Set the Dumpable state to be enabled
	    prctl(PR_SET_DUMPABLE, 1, 0, 0, 0);
	    // Set the core dump limitation to be unlimited
	    struct rlimit rlim;
	    rlim.rlim_cur = RLIM_INFINITY;
	    rlim.rlim_max = RLIM_INFINITY;
	    setrlimit(RLIMIT_CORE, &rlim);
	}

    try
    {
	SYS = new TSYS(argc,argv,envp);

	SYS->load();
	if( (rez=SYS->stopSignal()) > 0 ) return rez;
	rez = SYS->start();

	delete SYS;
    }catch(TError err) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }

    printf("OpenSCADA system is correctly exited by cause %d.\n",rez);

    return rez;
}
