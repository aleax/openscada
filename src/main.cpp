#include <locale.h>

#include "terror.h"
#include "tapplication.h"

TApplication *App;

int main(int argc, char *argv[], char *envp[] )
{
    setlocale(LC_ALL,"");
    //while(*envp) printf("%s\n",*envp++);
    
    App = new TApplication(argc,argv,envp);
    int rez = App->run();
    delete App;    

    return(rez);
}

