
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

//==== NewBD ====
int TBD::NewBD( string name )
{
    vector<int> id;
    
    //Get default BD type
    int idtype = name_to_id(def_t_bd);
    if(idtype < 0) 
    {
	//Get first worked BD type	
    	for(idtype=0; idtype < Size(); idtype++)
	    if(MChk(idtype)) break;
	if(idtype == Size()) return(-1);
    }
    for(int i=0; i < Size(); i++) id.push_back(-1);
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
    TModule *mod;
    int (TModule::*NewBD)(string name );
    char *n_f = "NewBD";

    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &NewBD)) == NULL) return(-1);
    int kz = (mod->*NewBD)(name);
//    int kz = (Moduls[idtype].modul->*NewBD)(name);
    FFree(idtype,n_f);

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
    	for(idtype=0; idtype < Size(); idtype++)
	    if(MChk(idtype)) break;
	if(idtype == Size()) return(-1);
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
    TModule *mod;
    int (TModule::*DelBD)(string name );
    char *n_f = "DelBD";

    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &DelBD)) == NULL) return(-1);
    int kz = (mod->*DelBD)(name);
//    int kz = (Moduls[idtype].modul->*DelBD)(name);
    FFree(idtype,n_f);

    return(kz); 
}

//==== OpenBD ====

int TBD::OpenBD( string name )
{
    vector<int> id;

    for(int i=0; i < Size(); i++)
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
    TModule *mod;
    int (TModule::*OpenBD)(string name );
    char *n_f = "OpenBD";

    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &OpenBD)) == NULL) return(-1);
    int kz = (mod->*OpenBD)(name);
    FFree(idtype,n_f);

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
    TModule *mod;
    int (TModule::*CloseBD)( int hd );
    char *n_f = "CloseBD";
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &CloseBD)) == NULL) return(-1);
    int kz = (mod->*CloseBD)(hd);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "SaveBD";
    int (TModule::*SaveBD)( unsigned int hdi );

    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &SaveBD)) == NULL) return(-1);
    int kz = (mod->*SaveBD)(hdi);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "GetCellS";
    int (TModule::*GetCellS)( int hd, int row, int line, string & cell );
   
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &GetCellS)) == NULL) return(-1);
    int kz = (mod->*GetCellS)(hd,row,line,cell);
    string str;
    if(GetCodePage(idtype,hd,str)==0) App->Mess->SconvIn(str.c_str(),cell);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "GetCellN";
    int (TModule::*GetCellN)( int hdi, int row, int line, double & val);
   
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &GetCellN)) == NULL) return(-1);
    int kz = (mod->*GetCellN)(hd,row,line,val);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "SetCellS";
    int (TModule::*SetCellS)( int hd, int row, int line, const string & cell );
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &SetCellS)) == NULL) return(-1);
    string str,cell_t(cell);
    if(GetCodePage(idtype,hd,str)==0) App->Mess->SconvOut(str.c_str(),cell_t);
    int kz = (mod->*SetCellS)(hd,row,line,cell_t);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "SetCellN";
    int (TModule::*SetCellN)( int hdi, int row, int line, double val );
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &SetCellN)) == NULL) return(-1);
    int kz = (mod->*SetCellN)(hd,row,line,val);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "NLines";
    int (TModule::*NLines)( int hd );
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &NLines)) == NULL) return(-1);
    int kz = (mod->*NLines)(hd);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "AddLine";
    int (TModule::*AddLine)( unsigned int hdi, unsigned int line );
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &AddLine)) == NULL) return(-1);
    int kz = (mod->*AddLine)(hdi,line);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "DelLine";
    int (TModule::*DelLine)( unsigned int hdi, unsigned int line );
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &DelLine)) == NULL) return(-1);
    int kz = (mod->*DelLine)(hdi,line);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "NRows";
    int (TModule::*NRows)( int hd );
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &NRows)) == NULL) return(-1);
    int kz = (mod->*NRows)(hd);
    FFree(idtype,n_f);

    return(kz);   
}

//==== AddRow ====

int TBD::AddRow(unsigned int hd, SRowAttr *row )
{
    int cnt;

    if(hd >= hdBD.size())     return(-1); 
    if((cnt = NRows(hd)) < 0) return(-2);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	AddRow(i,hd_m,row);
    }
    return(0);
}

int TBD::AddRow( string nametype, unsigned int hd, SRowAttr *row )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(AddRow(idtype,hd,row));
}

int TBD::AddRow( int idtype, unsigned int hd, SRowAttr *row )
{
    TModule *mod;
    char *n_f = "AddRow";
    int (TModule::*AddRow)( unsigned int hdi, SRowAttr *row );

    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &AddRow)) == NULL) return(-1);
    int kz = (mod->*AddRow)(hd,row);
    FFree(idtype,n_f);

    return(kz);   
}

//==== DelRow ====

int TBD::DelRow(unsigned int hd, string row)
{
    int cnt;

    if(hd >= hdBD.size())     return(-1); 
    if((cnt = NRows(hd)) < 0) return(-2);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
	if(hdBD[hd][i] >= 0) DelRow(i,hdBD[hd][i],row);

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
    TModule *mod;
    char *n_f = "DelRow";
    int (TModule::*DelRow)( unsigned int hdi, string row);

    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &DelRow)) == NULL) return(-1);
    int kz = (mod->*DelRow)(hd,row);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "GetCodePageBD";
    int (TModule::*GetCodePageBD)(int hdi, string & codepage );
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &GetCodePageBD)) == NULL) return(-1);
    int kz = (mod->*GetCodePageBD)(hd,codepage);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "SetCodePageBD";
    int (TModule::*SetCodePageBD)(int hdi, string codepage );
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &SetCodePageBD)) == NULL) return(-1);
    int kz = (mod->*SetCodePageBD)(hd,codepage);
    FFree(idtype,n_f);

    return(kz); 
}

//==== GetRowAttr ====
int TBD::GetRowAttr( unsigned int hd, int row, SRowAttr *attr)
{
    int cnt=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	return(GetRowAttr(i,hd_m,row,attr));
    }
    return(-1);
}

int TBD::GetRowAttr( string nametype, unsigned int hd, int row, SRowAttr *attr )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(GetRowAttr(idtype,hd,row,attr));
}

int TBD::GetRowAttr( int idtype, unsigned int hd, int row, SRowAttr *attr )
{
    TModule *mod;
    char *n_f = "GetRowAttr";
    int (TModule::*GetRowAttr)( unsigned int hd, int row, SRowAttr *attr );
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &GetRowAttr)) == NULL) return(-1);
    int kz = (mod->*GetRowAttr)(hd,row,attr);
    FFree(idtype,n_f);

    return(kz);
}

//==== SetRowAttr ====
int TBD::SetRowAttr( unsigned int hd, int row, SRowAttr *attr)
{
    int cnt=-1;

    if(hd < 0 || hd >= hdBD.size()) return(-1); 
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	return(SetRowAttr(i,hd_m,row,attr));
    }
    return(-1);
}

int TBD::SetRowAttr( string nametype, unsigned int hd, int row, SRowAttr *attr )
{
    int idtype = name_to_id(nametype);
    if(idtype < 0) return(-1);
    return(SetRowAttr(idtype,hd,row,attr));
}

int TBD::SetRowAttr( int idtype, unsigned int hd, int row, SRowAttr *attr )
{
    TModule *mod;
    char *n_f = "SetRowAttr";
    int (TModule::*SetRowAttr)( unsigned int hd, int row, SRowAttr *attr );
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &SetRowAttr)) == NULL) return(-1);
    int kz = (mod->*SetRowAttr)(hd,row,attr);
    FFree(idtype,n_f);

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
    TModule *mod;
    char *n_f = "RowNameToId";
    int (TModule::*RowNameToId)(unsigned int hdi, string namerow);
    
    if((mod=FUse(idtype,n_f,(void (TModule::**)()) &RowNameToId)) == NULL) return(-1);
    int kz = (mod->*RowNameToId)(hd,namerow);
    FFree(idtype,n_f);

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

