
#include <getopt.h>

#include "tapplication.h"
#include "tmessage.h"
#include "./moduls/gener/tmodule.h"
#include "tbd.h"


TBD::TBD(  ) : TGRPModule("BaseDate")
{

}


 TBD::~TBD(  )
{

}


void TBD::Init(  )
{
    string StrPath;

    CheckCommandLine();
    LoadAll(StrPath+App->ModPath+","+DirPath);
    InitAll();
//--- Test ---
    CloseBD(OpenBD("NameBD"));    
//------------
}


//---- OpenBD ------

int TBD::OpenBD( string name )
{
    vector<int> id;
    int (TModule::*OpenBD)(string name );
    for(int i=0; i < Moduls.size(); i++) 
    {
    	Moduls[i]->modul->GetFunc("OpenBD",  (void (TModule::**)()) &OpenBD);
	int hd=(Moduls[i]->modul->*OpenBD)(name);
	id.push_back(hd);
    }
    for(int i=0; i < id.size(); i++)
	if(id[i] >= 0) 
	{
	    int ii,iii;
	    for(ii=0; ii < hdBD.size(); ii++)
	    {
		for(iii=0; iii < hdBD[ii].size(); iii++) if(hdBD[ii][iii] >= 0) break;
		if(iii >= hdBD[ii].size()) break;
	    }
	    if(ii >= hdBD.size()) hdBD.push_back(id);
	    else                  hdBD[ii]=id;
	    return(ii);
	}
    return(-1);
}


//---- CloseBD ------

int TBD::CloseBD( int hd )
{
    int (TModule::*CloseBD)( int hd );

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0; i < Moduls.size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 )
	{
    	    Moduls[i]->modul->GetFunc("CloseBD",  (void (TModule::**)()) &CloseBD);
    	    (Moduls[i]->modul->*CloseBD)(hd_m);
	    hdBD[hd][i]= -1;
	}
    }
    return(0);
}


void TBD::Start(  )
{

}


string TBD::GetListBDTip(  )
{
    
}

void TBD::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= BD options ======================================\n"
    "    --BDModPath=<path>  Set moduls <path>;\n"
    "\n");
}

void TBD::CheckCommandLine(  )
{
    int i,next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"BDModPath" ,1,NULL,'m'},
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

bool TBD::AddM(char *name)
{
    bool kz;
    
    if((kz=TGRPModule::AddM(name)) == true)
	for(int i=0; i<hdBD.size(); i++) hdBD[i].push_back(-1);
    return(kz);
}


