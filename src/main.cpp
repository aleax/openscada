#include <locale.h>
#include <unistd.h>

#include "terror.h"
#include "tmessage.h"
#include "tsys.h"
#include "tkernel.h"

int main(int argc, char *argv[], char *envp[] )
{
    int rez = 0, i_krn = 0;

    printf("char = %d\n",sizeof(char));
    printf("short = %d\n",sizeof(short));
    printf("int = %d\n",sizeof(int));
    printf("long = %d\n",sizeof(long));
    
    //while(*envp) printf("%s\n",*envp++);
    Mess = new TMessage();    
    SYS  = new TSYS(argc,argv,envp);

    Mess->CheckCommandLine();
    Mess->UpdateOpt();		
    
    try
    { 
	while(true)
	{
	    string k_name = SYS->XMLCfgNode()->get_child("kernel",i_krn++)->get_attr("id");
	    SYS->kern_add( k_name );
	    int k_hd = SYS->kern_att( k_name );	    
	    if( !SYS->kern_at( k_hd ).run() ) rez++; 	    
	    SYS->kern_det( k_hd );
	}
    }
    catch(...) { }    
    if( rez ) SYS->Start();        
    
    delete SYS;
    delete Mess;    

    return(rez);
}

