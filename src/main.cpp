#include <locale.h>

#include "terror.h"
#include "tmessage.h"
#include "tsys.h"
#include "tkernel.h"

vector<TKernel *> kern;

int main(int argc, char *argv[], char *envp[] )
{
    int rez = 0, i_krn = 0;
    
    //while(*envp) printf("%s\n",*envp++);
    SYS  = new TSYS(argc,argv,envp);
    Mess = new TMessage();    
    
    try
    { 
	while(true)
	    kern.push_back( new TKernel( SYS->XMLCfgNode()->get_child("kernel",i_krn++)->get_attr("id") ) ); 	    
    }
    catch(...) { }
    for( i_krn = 0; i_krn < kern.size(); i_krn++)
	if( !(kern[i_krn]->run()) ) rez++;
    if( rez ) Mess->Start();        

    for( i_krn = 0; i_krn < kern.size(); i_krn++) 
	delete kern[i_krn];
    delete Mess;    
    delete SYS;

    return(rez);
}

