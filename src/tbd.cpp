
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

//==== NewBD ====
int TBD::NewBD( string nametype, string name )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(NewBD(idtype,name));
}

int TBD::NewBD( int idtype, string name )
{
    int (TModule::*NewBD)(string name );

    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("NewBD",  (void (TModule::**)()) &NewBD) == MOD_ERR)
	return(-1);
    return( (Moduls[idtype]->modul->*NewBD)(name) );
}

//==== DelBD ====
int TBD::DelBD( string nametype, string name )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(DelBD(idtype,name));
}

int TBD::DelBD( int idtype, string name )
{
    int (TModule::*DelBD)(string name );

    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("DelBD",  (void (TModule::**)()) &DelBD) == MOD_ERR)
	return(-1);
    return( (Moduls[idtype]->modul->*DelBD)(name) );
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

    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("OpenBD",  (void (TModule::**)()) &OpenBD) == MOD_ERR)
	return(-1);
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
    
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("CloseBD",  (void (TModule::**)()) &CloseBD) == MOD_ERR )
	return(-1);
    return( (Moduls[idtype]->modul->*CloseBD)(hd) );
}

//==== SaveBD ====
int TBD::SaveBD(unsigned int hd )
{
    if(hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) SaveBD(i,hd_m);
    }
    return(0);
}

int TBD::SaveBD( string nametype, unsigned int hdi)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(SaveBD(idtype,hdi));
}

int TBD::SaveBD( unsigned int idtype, unsigned int hdi )
{
    int (TModule::*SaveBD)( unsigned int hdi );
    
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("SaveBD",  (void (TModule::**)()) &SaveBD) == MOD_ERR )
	return(-1);
    return( (Moduls[idtype]->modul->*SaveBD)(hdi) );
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
    char *(TModule::*GetCharSetBD)( int hd );
   
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("GetCharSetBD",  (void (TModule::**)()) &GetCharSetBD) == MOD_ERR)
	return(-1);
    if(Moduls[idtype]->modul->GetFunc("GetCell1",  (void (TModule::**)()) &GetCell) == MOD_ERR)
	return(-1);
    int kz = (Moduls[idtype]->modul->*GetCell)(hd,row,line,cell);
    App->Mess->SconvIn( (Moduls[idtype]->modul->*GetCharSetBD)(hd),cell);
    return( kz );
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
    char *(TModule::*GetCharSetBD)( int hd );
    
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("GetCharSetBD",  (void (TModule::**)()) &GetCharSetBD) == MOD_ERR)
	return(-1);
    if(Moduls[idtype]->modul->GetFunc("GetCell2",  (void (TModule::**)()) &GetCell) == MOD_ERR)
	return(-1);
    int kz = (Moduls[idtype]->modul->*GetCell)(hd,row,line,cell);
    App->Mess->SconvIn( (Moduls[idtype]->modul->*GetCharSetBD)(hd),cell);
    return( kz );
}

//==== SetCell ====

int TBD::SetCell( int hd, int row, int line, const string & cell)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    else kz=SetCell(i,hd_m,row,line,cell);
	}
    }
    return(kz);
}

int TBD::SetCell( string nametype, int hd, int row, int line, const string & cell)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(SetCell(idtype,hd,row,line,cell));
}

int TBD::SetCell( int idtype, int hd, int row, int line, const string & cell)
{
    int (TModule::*SetCell)( int hd, int row, int line, const string & cell );
    char *(TModule::*GetCharSetBD)( int hd );
    
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("GetCharSetBD",  (void (TModule::**)()) &GetCharSetBD) == MOD_ERR)
	return(-1);
    if(Moduls[idtype]->modul->GetFunc("SetCell1",  (void (TModule::**)()) &SetCell) == MOD_ERR)
	return(-1);
    string cell_t(cell);
    App->Mess->SconvOut( (Moduls[idtype]->modul->*GetCharSetBD)(hd), cell_t);
    return( (Moduls[idtype]->modul->*SetCell)(hd,row,line,cell_t) );
}


int TBD::SetCell( int hd, string row, int line, const string & cell)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line )	line-=NLines(i,hd_m);
	    else kz=SetCell(i,hd_m,row,line,cell);
	}
    }
    return(kz);
}

int TBD::SetCell( string nametype, int hd, string row, int line, const string & cell)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(SetCell(idtype,hd,row,line,cell));
}

int TBD::SetCell( int idtype, int hd, string row, int line, const string & cell)
{
    int (TModule::*SetCell)( int hd, string row, int line, const string & cell );
    char *(TModule::*GetCharSetBD)( int hd );
    
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("GetCharSetBD",  (void (TModule::**)()) &GetCharSetBD) == MOD_ERR)
	return(-1);
    if(Moduls[idtype]->modul->GetFunc("SetCell2", (void (TModule::**)()) &SetCell) == MOD_ERR)
	return(-1);
    string cell_t(cell);
    App->Mess->SconvOut( (Moduls[idtype]->modul->*GetCharSetBD)(hd), cell_t);
    return( (Moduls[idtype]->modul->*SetCell)(hd,row,line,cell_t) );
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
    
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("NLines",  (void (TModule::**)()) &NLines) == MOD_ERR)
	return(-1);
    return( (Moduls[idtype]->modul->*NLines)(hd) );
}

//==== AddLine ====

int TBD::AddLine(unsigned int hd, unsigned int line)
{
    int cnt=0, n_ln, hd_m, hd_m_l, i, i_l;
    
    if(hd >= hdBD.size()) return(-1); 
    for(i=0, hd_m_l = -1 ; i < hdBD[hd].size(); i++) 
    {
	hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    n_ln=NLines(i,hd_m);
	    if( (cnt+n_ln) > line ) return(AddLine(i,hd_m,line-cnt));
	    cnt+=n_ln;
	    i_l=i; hd_m_l=hd_m;
	}
    }
    if(hd_m_l < 0) return(-1);
    return(AddLine(i_l,hd_m_l,line-cnt));
}

int TBD::AddLine( string nametype, unsigned int hdi, unsigned int line )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(AddLine(idtype,hdi,line));
}

int TBD::AddLine( unsigned int idtype, unsigned int hdi, unsigned int line )
{
    int (TModule::*AddLine)( unsigned int hdi, unsigned int line );
    
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("AddLine",  (void (TModule::**)()) &AddLine) == MOD_ERR)
	return(-1);
    return( (Moduls[idtype]->modul->*AddLine)(hdi,line) );
}

//==== DelLine ====

int TBD::DelLine(unsigned int hd, unsigned int line)
{
    int cnt=0, n_ln, hd_m, i;
    
    if(hd >= hdBD.size()) return(-1); 
    for(i=0; i < hdBD[hd].size(); i++) 
    {
	hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    n_ln=NLines(i,hd_m);
	    if( (cnt+n_ln) > line ) return(DelLine(i,hd_m,line-cnt));
	    cnt+=n_ln;
	}
    }
    return(-2);
}

int TBD::DelLine( string nametype, unsigned int hdi, unsigned int line )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(DelLine(idtype,hdi,line));
}

int TBD::DelLine( unsigned int idtype, unsigned int hdi, unsigned int line )
{
    int (TModule::*DelLine)( unsigned int hdi, unsigned int line );
    
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("DelLine",  (void (TModule::**)()) &DelLine) == MOD_ERR)
	return(-1);
    return( (Moduls[idtype]->modul->*DelLine)(hdi,line) );
}

//==== NRows ====

int TBD::NRows( int hd )
{
    int cnt=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	if(cnt==-1) cnt=NRows(i,hd_m);
	else if( NRows(i,hd_m)!=cnt ) return(-1);
    }
    return(cnt);
}

int TBD::NRows( string nametype, int hd )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(NRows(idtype,hd));
}

int TBD::NRows( int idtype, int hd )
{
    int (TModule::*NRows)( int hd );
    
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("NRows",  (void (TModule::**)()) &NRows) == MOD_ERR)
	return(-1);
    return( (Moduls[idtype]->modul->*NRows)(hd) );
}

//==== AddRow ====

int TBD::AddRow(unsigned int hd, string row, char type, unsigned int len, unsigned int dec)
{
    int cnt;

    if(hd >= hdBD.size())     return(-1); 
    if((cnt = NRows(hd)) < 0) return(-2);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	AddRow(i,hd_m,row,type,len,dec);
    }
    return(0);
}

int TBD::AddRow( string nametype, unsigned int hd, string row, char type, unsigned int len, unsigned int dec)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(AddRow(idtype,hd,row,type,len,dec));
}

int TBD::AddRow( int idtype, unsigned int hd, string row, char type, unsigned int len, unsigned int dec)
{
    int (TModule::*AddRow)( unsigned int hdi, string row, char type, unsigned int len, unsigned int dec);
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("AddRow",  (void (TModule::**)()) &AddRow) == MOD_ERR)
	return(-1);
    return( (Moduls[idtype]->modul->*AddRow)(hd,row,type,len,dec) );
}

//==== DelRow ====

int TBD::DelRow(unsigned int hd, string row)
{
    int cnt;

    if(hd >= hdBD.size())     return(-1); 
    if((cnt = NRows(hd)) < 0) return(-2);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	DelRow(i,hd_m,row);
    }
    return(0);
}

int TBD::DelRow( string nametype, unsigned int hd, string row)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(DelRow(idtype,hd,row));
}

int TBD::DelRow( int idtype, unsigned int hd, string row)
{
    int (TModule::*DelRow)( unsigned int hdi, string row);
    if(idtype >= Moduls.size() || Moduls[idtype]->stat == GRM_ST_OFF ) return(-1);
    if(Moduls[idtype]->modul->GetFunc("DelRow",  (void (TModule::**)()) &DelRow) == MOD_ERR)
	return(-1);
    return( (Moduls[idtype]->modul->*DelRow)(hd,row) );
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


int TBD::AddM( TModule *modul )
{

    int kz=TGRPModule::AddM(modul);
    if(kz < 0) return(kz);
    if(kz == hdBD.size())
	for(int i=0; i<hdBD.size(); i++) hdBD[i].push_back(-1);
    else
	for(int i=0; i<hdBD.size(); i++) hdBD[i][kz] = -1;
    return(kz);
}

int TBD::DelM( int hd )
{
    int kz;
    kz=TGRPModule::DelM(hd);
    if(kz != 0) return(kz);
    for(int i=0; i < hdBD.size(); i++) hdBD[i][hd] = -1;
    return(0);
}

bool TBD::test(int idtype)
{
    int kz;
    string str,str1;
    
    int hd = OpenBD("apv001");    
    App->Mess->put(0, "Open BD1: %d !",hd);    
    int n_line = NLines(hd);    
    App->Mess->put(0, "Numb lines: %d !",n_line );
    int n_row = NRows(hd);    
    App->Mess->put(0, "Numb rows: %d !",n_row );
    for(int i=0;i<n_line;i++)
	if(GetCell(hd,"SHIFR",i,str)==0)
	{
	    App->Mess->SconvOut("KOI8-U",str);
	    App->Mess->put(0, "%d: Shifr: %s !",i,str.c_str());
	}
    GetCell(hd,"SHIFR",0,str);
    App->Mess->SconvOut("KOI8-U",str);
    App->Mess->put(0, "Shifr before: %s !",str.c_str());
    str1.assign("Test_õÓÔ");
    App->Mess->SconvIn("KOI8-U",str1);
    SetCell(hd,"SHIFR",0,str1);
    GetCell(hd,"SHIFR",0,str);
    App->Mess->SconvOut("KOI8-U",str);
    App->Mess->put(0, "Shifr after: %s !",str.c_str());
    SetCell(hd,"SHIFR",0,str);
    CloseBD(hd);
}

