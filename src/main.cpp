#include <locale.h>

#include "terror.h"
#include "tmessage.h"
#include "tsys.h"
#include "tkernel.h"

TKernel *App;

int main(int argc, char *argv[], char *envp[] )
{
    int rez;
    
    //while(*envp) printf("%s\n",*envp++);
    SYS  = new TSYS(argc,argv,envp);
    Mess = new TMessage();
    
    App = new TKernel();
    if((rez = App->run()) == 0 ) Mess->Start();    

    delete App;
    delete Mess;    
    delete SYS;

    return(rez);
}

