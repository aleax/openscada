
//OpenSCADA system file: main.cpp
/***************************************************************************
 *   Copyright (C) 2003-2015 by Roman Savochenko, <rom_as@oscada.org>      *                                                     *
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

#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>

#include "terror.h"
#include "tmess.h"
#include "tsys.h"

using namespace OSCADA;

int main( int argc, char *argv[], char *envp[] )
{
    int rez = 0, pid = -1;
    string argCom, argVl, pidFile;

    //Check for the daemon mode and switch to at first
    for(int argPos = 0; (argCom=TSYS::getCmdOpt_(argPos,&argVl,argc,argv)).size(); )
	if(argCom == "demon") {
	    if((pid=fork()) == -1) { printf("Error: fork error!\n"); return -1; }
	    if(pid != 0) return 0;	//Original process close

	    // Prepare the demon environment
	    umask(0);
	    setsid();
	    int null_fd = open("/dev/null", O_WRONLY);
	    if(null_fd >= 0) {
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		dup2(null_fd, STDIN_FILENO);
		dup2(null_fd, STDOUT_FILENO);
		dup2(null_fd, STDERR_FILENO);
	    }
	    break;
	}

    //Check for other system's command line parameters
    for(int argPos = 0; (argCom=TSYS::getCmdOpt_(argPos,&argVl,argc,argv)).size(); )
	if(argCom == "CoreDumpAllow") {
	    // Set the Dumpable state to be enabled
	    prctl(PR_SET_DUMPABLE, 1, 0, 0, 0);
	    // Set the core dump limitation to be unlimited
	    struct rlimit rlim;
	    rlim.rlim_cur = RLIM_INFINITY;
	    rlim.rlim_max = RLIM_INFINITY;
	    setrlimit(RLIMIT_CORE, &rlim);
	}
	else if(argCom == "pid-file" && argVl.size() && (pid=open(argVl.c_str(),O_CREAT|O_TRUNC|O_WRONLY,0664)) >= 0) {
	    pidFile = argVl;
	    argVl = i2s(getpid());
	    write(pid, argVl.data(), argVl.size());
	    close(pid);
	}

    //Same load and start the core object TSYS
    SYS = new TSYS(argc, argv, envp);
    try {
	SYS->load();
	if((rez=SYS->stopSignal()) > 0) throw TError(SYS->nodePath().c_str(),"Stop by signal %d on load.",rez);
	rez = SYS->start();
    } catch(TError err) { mess_err(err.cat.c_str(), "%s", err.mess.c_str()); }

    //Free OpenSCADA system's root object
    if(SYS) delete SYS;

    printf("OpenSCADA system correct exit by code %d.\n", rez);

    if(pidFile.size()) remove(pidFile.c_str());

    return rez;
}
