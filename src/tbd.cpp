
#include <getopt.h>

#include "tapplication.h"
#include "tmessage.h"
#include "./moduls/gener/tmodule.h"
#include "tbd.h"


TBD::TBD(  ) : TGRPModule("BaseDate"), def_t_bd("direct_bd")
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
//    test(0);
//------------
}

//==== NewBD ====
int TBD::NewBD( string name )
{
    vector<int> id;
    
    //Get default BD type
    int idtype = name_to_id(def_t_bd);
    if(idtype < 0) 
    {
	//Get first worked BD type	
    	for(idtype=0; idtype < Moduls.size(); idtype++)
	    if(MUse(idtype) == 0) 
	    {
		MFree(idtype);
		break;
	    }
	if(idtype == Moduls.size()) return(-1);
    }
    for(int i=0; i < Moduls.size(); i++) id.push_back(-1);
    id[idtype]=NewBD(idtype,name);
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
    return(-2);    
}

int TBD::NewBD( string nametype, string name )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(NewBD(idtype,name));
}

int TBD::NewBD( int idtype, string name )
{
    int (TModule::*NewBD)(string name );

    if(MUse(idtype,"NewBD",(void (TModule::**)()) &NewBD) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*NewBD)(name);
    MFree(idtype);

    return(kz); 
}

//==== DelBD ====
int TBD::DelBD( string name )
{
    //Get default BD type
    int idtype = name_to_id(def_t_bd);
    if(idtype < 0) 
    {
	//Get first worked BD type	
    	for(idtype=0; idtype < Moduls.size(); idtype++)
	    if(MUse(idtype) == 0) 
	    {
		MFree(idtype);
		break;
	    }
	if(idtype == Moduls.size()) return(-1);
    }
    return(DelBD(idtype,name));   
}

int TBD::DelBD( string nametype, string name )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(DelBD(idtype,name));
}

int TBD::DelBD( int idtype, string name )
{
    int (TModule::*DelBD)(string name );

    if(MUse(idtype,"DelBD",(void (TModule::**)()) &DelBD) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*DelBD)(name);
    MFree(idtype);

    return(kz); 
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

    if(MUse(idtype,"OpenBD",(void (TModule::**)()) &OpenBD) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*OpenBD)(name);
    MFree(idtype);

    return(kz); 
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
    
    if(MUse(idtype,"CloseBD",(void (TModule::**)()) &CloseBD) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*CloseBD)(hd);
    MFree(idtype);

    return(kz); 
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

    if(MUse(idtype,"SaveBD",(void (TModule::**)()) &SaveBD) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*SaveBD)(hdi);
    MFree(idtype);

    return(kz); 
}
    
//==== GetCellS ====

int TBD::GetCellS( int hd, int row, int line, string & cell)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    else kz=GetCellS(i,hd_m,row,line,cell);
	}
    }
    return(kz);
}

int TBD::GetCellS( string nametype, int hd, int row, int line, string & cell)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(GetCellS(idtype,hd,row,line,cell));
}

int TBD::GetCellS( int idtype, int hd, int row, int line, string & cell)
{
    int (TModule::*GetCellS)( int hd, int row, int line, string & cell );
   
    if(MUse(idtype,"GetCellS",(void (TModule::**)()) &GetCellS) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*GetCellS)(hd,row,line,cell);
    string str;
    if(GetCodePage(idtype,hd,str)==0) App->Mess->SconvIn(str.c_str(),cell);
    MFree(idtype);

    return(kz);
}


int TBD::GetCellS( int hd, string row, int line, string & cell)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line )	line-=NLines(i,hd_m);
	    else kz=GetCellS(i,hd_m,row,line,cell);
	}
    }
    return(kz);
}

int TBD::GetCellS( string nametype, int hd, string row, int line, string & cell)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(GetCellS(idtype,hd,row,line,cell));
}

int TBD::GetCellS( int idtype, int hd, string row, int line, string & cell)
{
    int row_id;

    if((row_id = RowNameToId(idtype,hd,row)) < 0) return(-1);
    return(GetCellS(idtype,hd,row_id,line,cell));
}


//==== GetCellN ====

int TBD::GetCellN( int hd, int row, int line, double & val)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    else kz=GetCellN(i,hd_m,row,line,val);
	}
    }
    return(kz);
}

int TBD::GetCellN( string nametype, int hd, int row, int line, double & val)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(GetCellN(idtype,hd,row,line,val));
}

int TBD::GetCellN( int idtype, int hd, int row, int line, double & val)
{
    int (TModule::*GetCellN)( int hdi, int row, int line, double & val);
   
    if(MUse(idtype,"GetCellN",(void (TModule::**)()) &GetCellN) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*GetCellN)(hd,row,line,val);
    MFree(idtype);

    return(kz);
}


int TBD::GetCellN( int hd, string row, int line, double & val)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line )	line-=NLines(i,hd_m);
	    else kz = GetCellN(i,hd_m,row,line,val);
	}
    }
    return(kz);
}

int TBD::GetCellN( string nametype, int hd, string row, int line, double & val)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(GetCellN(idtype,hd,row,line,val));
}

int TBD::GetCellN( int idtype, int hd, string row, int line, double & val)
{
    int row_id;

    if((row_id = RowNameToId(idtype,hd,row)) < 0) return(-1);
    return(GetCellN(idtype,hd,row_id,line,val));
}


//==== SetCellS ====

int TBD::SetCellS( int hd, int row, int line, const string & cell)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    else kz=SetCellS(i,hd_m,row,line,cell);
	}
    }
    return(kz);
}

int TBD::SetCellS( string nametype, int hd, int row, int line, const string & cell)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(SetCellS(idtype,hd,row,line,cell));
}

int TBD::SetCellS( int idtype, int hd, int row, int line, const string & cell)
{
    int (TModule::*SetCellS)( int hd, int row, int line, const string & cell );
    
    if(MUse(idtype,"SetCellS",(void (TModule::**)()) &SetCellS) != 0) return(-1);
    string str,cell_t(cell);
    if(GetCodePage(idtype,hd,str)==0) App->Mess->SconvOut(str.c_str(),cell_t);
    int kz = (Moduls[idtype].modul->*SetCellS)(hd,row,line,cell_t);
    MFree(idtype);

    return(kz);
}


int TBD::SetCellS( int hd, string row, int line, const string & cell)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line )	line-=NLines(i,hd_m);
	    else kz=SetCellS(i,hd_m,row,line,cell);
	}
    }
    return(kz);
}

int TBD::SetCellS( string nametype, int hd, string row, int line, const string & cell)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(SetCellS(idtype,hd,row,line,cell));
}

int TBD::SetCellS( int idtype, int hd, string row, int line, const string & cell)
{
    int row_id;

    if((row_id = RowNameToId(idtype,hd,row)) < 0) return(-1);
    return(SetCellS(idtype,hd,row_id,line,cell));
}

//==== SetCellN ====

int TBD::SetCellN( int hd, int row, int line, double val)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    else kz=SetCellN(i,hd_m,row,line,val);
	}
    }
    return(kz);
}

int TBD::SetCellN( string nametype, int hd, int row, int line, double val)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(SetCellN(idtype,hd,row,line,val));
}

int TBD::SetCellN( int idtype, int hd, int row, int line, double val)
{
    int (TModule::*SetCellN)( int hdi, int row, int line, double val );
    
    if(MUse(idtype,"SetCellN",(void (TModule::**)()) &SetCellN) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*SetCellN)(hd,row,line,val);
    MFree(idtype);

    return(kz);
}


int TBD::SetCellN( int hd, string row, int line, double val)
{
    int kz=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line )	line-=NLines(i,hd_m);
	    else kz=SetCellN(i,hd_m,row,line,val);
	}
    }
    return(kz);
}

int TBD::SetCellN( string nametype, int hd, string row, int line, double val)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(SetCellN(idtype,hd,row,line,val));
}

int TBD::SetCellN( int idtype, int hd, string row, int line, double val)
{
    int row_id;

    if((row_id = RowNameToId(idtype,hd,row)) < 0) return(-1);
    return(SetCellN(idtype,hd,row_id,line,val));
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
    
    if(MUse(idtype,"NLines",(void (TModule::**)()) &NLines) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*NLines)(hd);
    MFree(idtype);

    return(kz); 
}

//==== AddLine ====

int TBD::AddLine(unsigned int hd, unsigned int line)
{
    int cnt=0, n_ln, hd_m, hd_m_l, i, i_l, cnt_l;
    
    if(hd >= hdBD.size()) return(-1); 
    for(i=0, hd_m_l = -1 ; i < hdBD[hd].size(); i++) 
    {
	hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    n_ln=NLines(i,hd_m);
	    if( (cnt+n_ln) > line ) return(AddLine(i,hd_m,line-cnt));
	    i_l=i; hd_m_l=hd_m, cnt_l=cnt;
	    cnt+=n_ln;
	}
    }
    if(hd_m_l < 0) return(-1);
    return(AddLine(i_l,hd_m_l,line-cnt_l));
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
    
    if(MUse(idtype,"AddLine",(void (TModule::**)()) &AddLine) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*AddLine)(hdi,line);
    MFree(idtype);

    return(kz);   
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
    
    if(MUse(idtype,"DelLine",(void (TModule::**)()) &DelLine) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*DelLine)(hdi,line);
    MFree(idtype);

    return(kz);   
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
    
    if(MUse(idtype,"NRows",(void (TModule::**)()) &NRows) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*NRows)(hd);
    MFree(idtype);

    return(kz);   
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

    if(MUse(idtype,"AddRow",(void (TModule::**)()) &AddRow) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*AddRow)(hd,row,type,len,dec);
    MFree(idtype);

    return(kz);   
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

    if(MUse(idtype,"DelRow",(void (TModule::**)()) &DelRow) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*DelRow)(hd,row);
    MFree(idtype);

    return(kz);   
}




//==== GetCodePage ====

int TBD::GetCodePage( string nametype, unsigned int hd, string & codepage)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(GetCodePage(idtype,hd,codepage));
}

int TBD::GetCodePage( int idtype, unsigned int hd, string & codepage)
{
    int (TModule::*GetCodePageBD)(int hdi, string & codepage );
    
    if(MUse(idtype,"GetCodePageBD",(void (TModule::**)()) &GetCodePageBD) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*GetCodePageBD)(hd,codepage);
    MFree(idtype);

    return(kz); 
}

//==== SetCodePage ====

int TBD::SetCodePage( string nametype, unsigned int hd, string codepage)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(SetCodePage(idtype,hd,codepage));
}

int TBD::SetCodePage( int idtype, unsigned int hd, string codepage)
{
    int (TModule::*SetCodePageBD)(int hdi, string codepage );
    
    if(MUse(idtype,"SetCodePageBD",(void (TModule::**)()) &SetCodePageBD) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*SetCodePageBD)(hd,codepage);
    MFree(idtype);

    return(kz); 
}

//==== GetRowAttr ====
int TBD::GetRowAttr( unsigned int hd, int row, string & namerow, char & type, unsigned int & len, unsigned int & dec)
{
    int cnt=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	return(GetRowAttr(i,hd_m,row,namerow,type,len,dec));
    }
    return(-1);
}

int TBD::GetRowAttr( string nametype, unsigned int hd, int row, string & namerow, char & type, unsigned int & len, unsigned int & dec)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(GetRowAttr(idtype,hd,row,namerow,type,len,dec));
}

int TBD::GetRowAttr( int idtype, unsigned int hd, int row, string & namerow, char & type, unsigned int & len, unsigned int & dec)
{
    int (TModule::*GetRowAttr)( unsigned int hd, int row, string & namerow, char & type, unsigned int & len, unsigned int & dec);
    
    if(MUse(idtype,"GetRowAttr",(void (TModule::**)()) &GetRowAttr) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*GetRowAttr)(hd,row,namerow,type,len,dec);
    MFree(idtype);

    return(kz);
}

//==== RowNameToId ====
int TBD::RowNameToId( unsigned int hd, string namerow)
{
    int id = -1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	if(id == -1) id=RowNameToId(i,hd_m,namerow);
	else if( RowNameToId(i,hd_m,namerow) != id ) return(-1);
    }
    return(id);
}

int TBD::RowNameToId( string nametype, unsigned int hd, string namerow)
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(RowNameToId(idtype,hd,namerow));
}

int TBD::RowNameToId( int idtype, unsigned int hd, string namerow)
{
    int (TModule::*RowNameToId)(unsigned int hdi, string namerow);
    
    if(MUse(idtype,"RowNameToId",(void (TModule::**)()) &RowNameToId) != 0) return(-1);
    int kz = (Moduls[idtype].modul->*RowNameToId)(hd,namerow);
    MFree(idtype);

    return(kz);
}


void TBD::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= BD options ======================================\n"
    "    --BDMPath=<path>    Set moduls <path>;\n"
    "    --BDMDef=<mod_name> Set default modul name for create new BD;\n"
    "\n");
}

void TBD::CheckCommandLine(  )
{
    int i,next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"BDMPath" ,1,NULL,'m'},
	{"BDMDef"  ,1,NULL,'d'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(App->argc,(char * const *)App->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath  = strdup(optarg); break;
	    case 'd': def_t_bd = optarg; break;
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
	if(GetCellS(hd,"SHIFR",i,str)==0)
	{
	    App->Mess->SconvOut("KOI8-U",str);
	    App->Mess->put(0, "%d: Shifr: %s !",i,str.c_str());
	}
    GetCellS(hd,"SHIFR",0,str);
    App->Mess->SconvOut("KOI8-U",str);
    App->Mess->put(0, "Shifr before: %s !",str.c_str());
    str1.assign("Test_õÓÔ");
    App->Mess->SconvIn("KOI8-U",str1);
    SetCellS(hd,"SHIFR",0,str1);
    GetCellS(hd,"SHIFR",0,str);
    App->Mess->SconvOut("KOI8-U",str);
    App->Mess->put(0, "Shifr after: %s !",str.c_str());
    SetCellS(hd,"SHIFR",0,str);

    double val=0.0;
    kz=GetCellN(hd,"VG",0,val);
    App->Mess->put(0, "Val before: %f (%d) !",val,kz);
    SetCellN(hd,"VG",0,55.555);
    kz=GetCellN(hd,"VG",0,val);
    App->Mess->put(0, "Val after: %f (%d) !",val,kz);

    CloseBD(hd);
}

