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

#include "terror.h"
#include "tmessage.h"
#include "tsys.h"
#include "tkernel.h"

int main(int argc, char *argv[], char *envp[] )
{
    int rez = 0, i_krn = 0;
    
    //while(*envp) printf("%s\n",*envp++);
    Mess = new TMessage();
    try
    {
	SYS  = new TSYS(argc,argv,envp);
    
	Mess->checkCommandLine();
	Mess->updateOpt();		
    
	for( int i_krn = 0; i_krn < SYS->cfgNode()->childSize(); i_krn++)
    	    if( SYS->cfgNode()->childGet(i_krn)->name() == "kernel" )
    	    {
        	string k_name = SYS->cfgNode()->childGet(i_krn)->attr("id");
        	SYS->kAdd( k_name );
        	if( !SYS->kAt( k_name ).at().run() ) rez++;
    	    }
	if( rez ) SYS->start();        
    
	delete SYS;
	delete Mess;
    }catch(TError err)
    { Mess->put_s("SYS",MESS_ERR,err.what()); }

    return(rez);
}

