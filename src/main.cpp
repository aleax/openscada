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
    SYS  = new TSYS(argc,argv,envp);

    Mess->CheckCommandLine();
    Mess->UpdateOpt();		
    
    try
    { 
	while(true)
	    if( !SYS->KernMake( SYS->XMLCfgNode()->get_child("kernel",i_krn++)->get_attr("id") ).run() )
		rez++; 	    
    }
    catch(...) { }    
    if( rez ) SYS->Start();        
    
    delete SYS;
    delete Mess;    

    return(rez);
}

