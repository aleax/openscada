#include <getopt.h>

#include "tapplication.h"
#include "tmessage.h"
#include "tgui.h"

TGUI::TGUI(  ) : TGRPModule("GUI")
{

}


void TGUI::Init(  )
{
    string StrPath;

    CheckCommandLine();
    LoadAll(StrPath+App->ModPath+","+DirPath);
    InitAll();	
}


void TGUI::Start(  )
{

}

void TGUI::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= Protocol options ================================\n"
    "    --GUIModPath=<path>  Set moduls <path>;\n"
    "\n");
}

void TGUI::CheckCommandLine(  )
{
    int i,next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"GUIModPath" ,1,NULL,'m'},
	{NULL         ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(App->argc,(char * const *)App->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath=strdup(optarg); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}


