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

#include <locale.h>
#include <unistd.h>
#include <getopt.h>

#include "terror.h"
#include "tmess.h"
#include "tsys.h"

int main(int argc, char *argv[], char *envp[] )
{
    int rez = 0, i_krn = 0;
    
    //Check demon mode start
    int next_opt;
    optind=opterr=0;
    struct option long_opt[] = { {"demon" ,0,NULL,'d'}, {NULL    ,0,NULL,0  } };	
    while((next_opt=getopt_long(argc,argv,"",long_opt,NULL)) != -1)
	if( next_opt == 'd' )
	{
	    printf("Start into demon mode!\n");
	    int pid = fork();
	    if( pid == -1 )
	    {	
		printf("Error: fork error!\n");
		return(-1);
	    }
	    if( pid != 0 )	return(0);
	    
	    //Prepare demon environment
	    setsid();
	    
	    break;	    
	}
    
    //while(*envp) printf("%s\n",*envp++);
    try
    {
	SYS = new TSYS(argc,argv,envp);	
	
	SYS->load();
	rez = SYS->start();    
	
	delete SYS;
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }

    return rez;
}

