#include "tapplication.h"

TApplication *App;

int main(int argc, char *argv[] )
{
    App = new TApplication(argc,argv);
    int rez = App->run();
    delete App;

    return(rez);
}

