#include <locale.h>

#include "terror.h"
#include "tmessage.h"
#include "tsys.h"
#include "tkernel.h"

TKernel *App;

int main(int argc, char *argv[], char *envp[] )
{
    int rez;
    
    setlocale(LC_ALL,"");
    //while(*envp) printf("%s\n",*envp++);
    SYS  = new TSYS();
    Mess = new TMessage();
    
    App = new TKernel(argc,argv,envp);
    if((rez = App->run()) == 0 ) Mess->Start();    

    delete App;
    delete Mess;    
    delete SYS;

    return(rez);
}

