
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
    test(0);
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

//---- OpenBDdir ------
int TBD::OpenBD( string nametype, string name )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(OpenBD(idtype,name));
}

int TBD::OpenBD( int idtype, string name )
{
    int (TModule::*OpenBD)(string name );

    if(idtype >= Moduls.size()) return(-1);
    Moduls[idtype]->modul->GetFunc("OpenBD",  (void (TModule::**)()) &OpenBD);
    return( (Moduls[idtype]->modul->*OpenBD)(name) );
}

//---- CloseBD ------

int TBD::CloseBD( int hd )
{
    int cnt=0;
    int (TModule::*CloseBD)( int hd );

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 )
	{
    	    Moduls[i]->modul->GetFunc("CloseBD",  (void (TModule::**)()) &CloseBD);
    	    (Moduls[i]->modul->*CloseBD)(hd_m);
	    hdBD[hd][i]= -1;
	    cnt++;
	}
    }

    if(cnt > 0) return(0);
    else	return(-1);
}

int TBD::CloseBD( string nametype, int hd )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(CloseBD(idtype,hd));
}

int TBD::CloseBD( int idtype, int hd )
{
    int (TModule::*CloseBD)( int hd );
    
    if(idtype >= Moduls.size()) return(-1);
    Moduls[idtype]->modul->GetFunc("CloseBD",  (void (TModule::**)()) &CloseBD);
    return( (Moduls[idtype]->modul->*CloseBD)(hd) );
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

bool TBD::test(int idtype)
{
    int kz;
    int hd1 = OpenBD(idtype,"apv001.dbf");    
    App->Mess->put(0, "Open BD1: %d !",hd1);

    int hd2 = OpenBD(idtype,"apv002.dbf");    
    App->Mess->put(0, "Open BD2: %d !",hd2);

    int hd3 = OpenBD(idtype,"apv002.dbf");    
    App->Mess->put(0, "Open BD2: %d !",hd3);

    kz = CloseBD(idtype,hd1);
    App->Mess->put(0, "Clos BD1: %d !",kz);
    kz = CloseBD(idtype,hd2);
    App->Mess->put(0, "Clos BD2: %d !",kz);
    kz = CloseBD(idtype,hd3);
    App->Mess->put(0, "Clos BD2: %d !",kz);
}

