
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


//==== OpenBD ====

int TBD::OpenBD( string name )
{
    vector<int> id;

    for(int i=0; i < Moduls.size(); i++)
       	id.push_back(OpenBD(i,name));
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

//==== CloseBD ====

int TBD::CloseBD( int hd )
{
    int cnt=0;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 )
	{
	    CloseBD(i,hd_m);
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

//==== GetCell ====

int TBD::GetCell( int hd, int row, int line, string & cell)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    else kz=GetCell(i,hd_m,row,line,cell);
	}
    }
    return(kz);
}

int TBD::GetCell( string nametype, int hd, int row, int line, string & cell)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(GetCell(idtype,hd,row,line,cell));
}

int TBD::GetCell( int idtype, int hd, int row, int line, string & cell)
{
    int (TModule::*GetCell)( int hd, int row, int line, string & cell );
    
    if(idtype >= Moduls.size()) return(-1);
    Moduls[idtype]->modul->GetFunc("GetCell1",  (void (TModule::**)()) &GetCell);
    return( (Moduls[idtype]->modul->*GetCell)(hd,row,line,cell) );
}


int TBD::GetCell( int hd, string row, int line, string & cell)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line )	line-=NLines(i,hd_m);
	    else kz=GetCell(i,hd_m,row,line,cell);
	}
    }
    return(kz);
}

int TBD::GetCell( string nametype, int hd, string row, int line, string & cell)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(GetCell(idtype,hd,row,line,cell));
}

int TBD::GetCell( int idtype, int hd, string row, int line, string & cell)
{
    int (TModule::*GetCell)( int hd, string row, int line, string & cell );
    
    if(idtype >= Moduls.size()) return(-1);
    Moduls[idtype]->modul->GetFunc("GetCell2",  (void (TModule::**)()) &GetCell);
    return( (Moduls[idtype]->modul->*GetCell)(hd,row,line,cell) );
}

//==== NLines ====

int TBD::NLines( int hd )
{
    int cnt=0;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) cnt+=NLines(i,hd_m);
    }
    return(cnt);
}

int TBD::NLines( string nametype, int hd )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(NLines(idtype,hd));
}

int TBD::NLines( int idtype, int hd )
{
    int (TModule::*NLines)( int hd );
    
    if(idtype >= Moduls.size()) return(-1);
    Moduls[idtype]->modul->GetFunc("NLines",  (void (TModule::**)()) &NLines);
    return( (Moduls[idtype]->modul->*NLines)(hd) );
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
    string str;
    
    int hd = OpenBD("apv001.dbf");    
    App->Mess->put(0, "Open BD1: %d !",hd);    
    int numb = NLines(hd);    
    App->Mess->put(0, "Numb lines: %d !",numb);
    for(int i=0;i<numb;i++)
	if(GetCell(hd,"SHIFR",i,str)==0)
	    App->Mess->put(0, "%d: Shifr: %s !",i,str.c_str());
	
}

