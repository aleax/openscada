#include <getopt.h>

#include "tapplication.h"
#include "tmessage.h"
#include "ttipcontroller.h"

 TTipController::TTipController(  ) : TGRPModule("Controller") 
{

}



string TTipController::GetContrList( string NameContrTip )
{

}


void TTipController::Init(  )
{
    string StrPath;

    CheckCommandLine();
    LoadAll(StrPath+App->ModPath+","+DirPath);
    InitAll();
}


int TTipController::InitTask( string NameBDTask )
{

}


void TTipController::Start(  )
{

}


int TTipController::SaveBD(  )
{

}


int TTipController::GetContrInfo( string NameTipCtr, string NameCtr )
{

}


int TTipController::SendContrCom( string NameCtr, string Command )
{

}


string TTipController::GetParamTipList( string NameCtr, string NameTask )
{

}


int TTipController::ReloadBDCtr( string Command )
{

}


int TTipController::PutCom( string type )
{

}


string TTipController::GetContrTipList(  )
{

}



int TTipController::OperContr( string NameTip, int NameCtr, string NameSecond, string opper )
{

}

void TTipController::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= TipController options ===========================\n"
    "    --TCModPath=<path>  Set moduls <path>;\n"
    "\n");
}

void TTipController::CheckCommandLine(  )
{
    int i,next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"TCModPath" ,1,NULL,'m'},
	{NULL        ,0,NULL,0  }
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



