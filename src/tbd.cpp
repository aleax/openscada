
#include <getopt.h>

#include "tapplication.h"
#include "tmessage.h"
#include "tmodule.h"
#include "tbd.h"


const char *TBD::o_name = "TBD";
const char *TBD::n_opt  = "bd";

TBD::TBD(  ) : TGRPModule("BaseDate"), def_t_bd("direct_dbf")
{

}


TBD::~TBD(  )
{

}

//==== NewBD ====
int TBD::NewBD( string name )
{
    vector<int> id;
    int idtype;
    
    //Get default BD type
    try { idtype = name_to_id(def_t_bd); } 
    catch(TError error)
    {
	//Get first worked BD type	
    	for(idtype=0; idtype < Size(); idtype++)
	    if(MChk(idtype)) break;
	if(idtype == Size()) throw TError("%s: no one active BD!",o_name);
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
    throw TError("%s: new error!",o_name);        
}

int TBD::NewBD( int idtype, string name )
{
    int hd;
    TModule *mod;
    int (TModule::*NewBD)(string name );
    char *n_f = "NewBD";

    mod=FUse(idtype,n_f,(void (TModule::**)()) &NewBD);
    try{ hd = (mod->*NewBD)(name); }
    catch(...)
    {
	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);

    return(hd); 
}

//==== DelBD ====
void TBD::DelBD( string name )
{
    int idtype;
    
    //Get default BD type
    try { idtype = name_to_id(def_t_bd); } 
    catch(TError error)
    {
	//Get first worked BD type	
    	for(idtype=0; idtype < Size(); idtype++)
	    if(MChk(idtype)) break;
	if(idtype == Size()) throw TError("%s: no one active BD!",o_name); 
    }
    DelBD(idtype,name);   
}

void TBD::DelBD( int idtype, string name )
{
    TModule *mod;
    void (TModule::*DelBD)(string name );
    char *n_f = "DelBD";

    mod=FUse(idtype,n_f,(void (TModule::**)()) &DelBD);
    try{ (mod->*DelBD)(name); }
    catch(...)
    {
	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
}

//==== OpenBD ====

int TBD::OpenBD( string name )
{
    int hd;
    vector<int> id;

    for(int i=0; i < Size(); i++)
    {
       	try{ hd = OpenBD(i,name); }
	catch(...) { hd = -1; }
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
    throw TError("%s: BD %s no avoid!",o_name,name.c_str());
}

int TBD::OpenBD( int idtype, string name )
{
    int hd;
    TModule *mod;
    int (TModule::*OpenBD)(string name );
    char *n_f = "OpenBD";

    mod=FUse(idtype,n_f,(void (TModule::**)()) &OpenBD);
    try{ hd = (mod->*OpenBD)(name); }
    catch(...) 
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);

    return(hd); 
}

//==== CloseBD ====

void TBD::CloseBD( unsigned int hd )
{
    int cnt=0;

    CheckHD(hd);
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
    if(cnt > 0) return;
    else	throw TError("%s: BD's hd error!",o_name);
}

void TBD::CloseBD( unsigned int idtype, unsigned int hd )
{
    TModule *mod;
    void (TModule::*CloseBD)( unsigned int hd );
    char *n_f = "CloseBD";
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &CloseBD);
    try{ (mod->*CloseBD)(hd); }
    catch(...)
    {
	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
}

//==== SaveBD ====
void TBD::SaveBD(unsigned int hd )
{
    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) SaveBD(i,hd_m);
    }
    return;
}

void TBD::SaveBD( unsigned int idtype, unsigned int hdi )
{
    TModule *mod;
    char *n_f = "SaveBD";
    void (TModule::*SaveBD)( unsigned int hdi );

    mod=FUse(idtype,n_f,(void (TModule::**)()) &SaveBD);
    try{ (mod->*SaveBD)(hdi); }
    catch(...)
    {
	FFree(idtype,n_f);
        throw;
    }
    FFree(idtype,n_f);
}
    
//==== GetCellS ====

string TBD::GetCellS( unsigned int hd, int colm, int line )
{
    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    else return(GetCellS(i,hd_m,colm,line));
	}
    }
    throw TError("%s: table's cell error!",o_name);
}

string TBD::GetCellS( unsigned int idtype, unsigned int hd, int colm, int line)
{
    TModule *mod;
    char *n_f = "GetCellS";
    string (TModule::*GetCellS)( unsigned int hd, int colm, int line );
    string val;
   
    mod=FUse(idtype,n_f,(void (TModule::**)()) &GetCellS);
    try
    {
       	val = (mod->*GetCellS)(hd,colm,line);
	App->Mess->SconvIn(GetCodePage(idtype,hd).c_str(),val);
    }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
    return(val);
}

//==== GetCellN ====

double TBD::GetCellR( unsigned int hd, int colm, int line )
{
    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    return(GetCellR(i,hd_m,colm,line));
	}
    }
    throw TError("%s: table's cell error!",o_name);
}

double TBD::GetCellR( unsigned int idtype, unsigned int hd, int colm, int line)
{
    TModule *mod;
    char *n_f = "GetCellR";
    double (TModule::*GetCellR)( int hdi, int colm, int line);
    double val;
   
    mod=FUse(idtype,n_f,(void (TModule::**)()) &GetCellR);
    try{ val = (mod->*GetCellR)(hd,colm,line); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
    return(val);
}

//==== GetCellI ====

int TBD::GetCellI( unsigned int hd, int colm, int line )
{
    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    return(GetCellI(i,hd_m,colm,line));
	}
    }
    throw TError("%s: table's cell error!",o_name);
}

int TBD::GetCellI( unsigned int idtype, unsigned int hd, int colm, int line)
{
    TModule *mod;
    char *n_f = "GetCellI";
    int (TModule::*GetCellI)( int hdi, int colm, int line);
    int val;
   
    mod=FUse(idtype,n_f,(void (TModule::**)()) &GetCellI);
    try{ val = (mod->*GetCellI)(hd,colm,line); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
    return(val);
}

//==== GetCellB ====

bool TBD::GetCellB( unsigned int hd, int colm, int line )
{
    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    return(GetCellB(i,hd_m,colm,line));
	}
    }
    throw TError("%s: table's cell error!",o_name);
}

bool TBD::GetCellB( unsigned int idtype, unsigned int hd, int colm, int line)
{
    TModule *mod;
    char *n_f = "GetCellB";
    bool (TModule::*GetCellB)( int hdi, int colm, int line);
    bool val;
   
    mod=FUse(idtype,n_f,(void (TModule::**)()) &GetCellB);
    try{ val = (mod->*GetCellB)(hd,colm,line); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
    return(val);
}

//==== SetCellS ====

void TBD::SetCellS( unsigned int hd, int colm, int line, const string cell)
{
    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    else { SetCellS(i,hd_m,colm,line,cell); return; }
	}
    }
    throw TError("%s: table's cell error!",o_name);
}

void TBD::SetCellS( unsigned int idtype, unsigned int hd, int colm, int line, const string cell)
{
    TModule *mod;
    char *n_f = "SetCellS";
    void (TModule::*SetCellS)( unsigned int hd, int colm, int line, const string cell );
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &SetCellS);
    string cell_t(cell);
    App->Mess->SconvOut(GetCodePage(idtype,hd).c_str(),cell_t);
    try{ (mod->*SetCellS)(hd,colm,line,cell_t); }
    catch(...)
    {
     	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
}

//==== SetCellR ====

void TBD::SetCellR( unsigned int hd, int colm, int line, double val)
{
    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    else { SetCellR(i,hd_m,colm,line,val); return; }
	}
    }
    throw TError("%s: table's cell error!",o_name);
}

void TBD::SetCellR( unsigned int idtype, unsigned int hd, int colm, int line, double val)
{
    TModule *mod;
    char *n_f = "SetCellR";
    void (TModule::*SetCellR)( unsigned int hdi, int colm, int line, double val );
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &SetCellR);
    try{ (mod->*SetCellR)(hd,colm,line,val); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
}

//==== SetCellI ====

void TBD::SetCellI( unsigned int hd, int colm, int line, int val)
{
    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    else { SetCellI(i,hd_m,colm,line,val); return; }
	}
    }
    throw TError("%s: table's cell error!",o_name);
}

void TBD::SetCellI( unsigned int idtype, unsigned int hd, int colm, int line, int val)
{
    TModule *mod;
    char *n_f = "SetCellI";
    void (TModule::*SetCellI)( unsigned int hdi, int colm, int line, int val );
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &SetCellI);
    try{ (mod->*SetCellI)(hd,colm,line,val); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
}

//==== SetCellB ====

void TBD::SetCellB( unsigned int hd, int colm, int line, bool val)
{
    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    if( NLines(i,hd_m) < line ) line-=NLines(i,hd_m);
	    else { SetCellB(i,hd_m,colm,line,val); return; }
	}
    }
    throw TError("%s: table's cell error!",o_name);
}

void TBD::SetCellB( unsigned int idtype, unsigned int hd, int colm, int line, bool val)
{
    TModule *mod;
    char *n_f = "SetCellB";
    void (TModule::*SetCellB)( unsigned int hdi, int colm, int line, bool val );
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &SetCellB);
    try{ (mod->*SetCellB)(hd,colm,line,val); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
}

//==== NLines ====

int TBD::NLines( unsigned int hd )
{
    int cnt=0;

    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) cnt+=NLines(i,hd_m);
    }
    return(cnt);
}

int TBD::NLines( unsigned int idtype, unsigned int hd )
{
    TModule *mod;
    char *n_f = "NLines";
    int (TModule::*NLines)( int hd );
    int val;
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &NLines);
    try{ val = (mod->*NLines)(hd); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
    return(val); 
}

//==== AddLine ====

int TBD::AddLine(unsigned int hd, unsigned int line)
{
    int cnt=0, n_ln, hd_m, hd_m_l, i, i_l, cnt_l;
    
    CheckHD(hd);
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
    if(hd_m_l < 0) throw TError("%s: table's cell error!",o_name);
    return(AddLine(i_l,hd_m_l,line-cnt_l));
}

int TBD::AddLine( unsigned int idtype, unsigned int hdi, unsigned int line )
{
    TModule *mod;
    char *n_f = "AddLine";
    int (TModule::*AddLine)( unsigned int hdi, unsigned int line );
    int val;
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &AddLine);
    try{ val = (mod->*AddLine)(hdi,line);}
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
    return(val);   
}

//==== DelLine ====

void TBD::DelLine(unsigned int hd, unsigned int line)
{
    int cnt=0, n_ln, hd_m, i;
    
    CheckHD(hd);
    for(i=0; i < hdBD[hd].size(); i++) 
    {
	hd_m = hdBD[hd][i];
	if( hd_m >= 0 ) 
	{
	    n_ln=NLines(i,hd_m);
	    if( (cnt+n_ln) > line ) {DelLine(i,hd_m,line-cnt); return; }
	    cnt+=n_ln;
	}
    }
    throw TError("%s: no active bd!",o_name);
}

void TBD::DelLine( unsigned int idtype, unsigned int hdi, unsigned int line )
{
    TModule *mod;
    char *n_f = "DelLine";
    void (TModule::*DelLine)( unsigned int hdi, unsigned int line );
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &DelLine);
    try{ (mod->*DelLine)(hdi,line); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
}

//==== NRows ====

int TBD::NColums( unsigned int hd )
{
    int cnt=-1;

    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	if(cnt==-1) cnt=NColums(i,hd_m);
	else if( NColums(i,hd_m)!=cnt ) throw TError("%s: structure of bd error!",o_name);
    }
    return(cnt);
}

int TBD::NColums( unsigned int idtype, unsigned int hd )
{
    TModule *mod;
    char *n_f = "NColums";
    int (TModule::*NColums)( int hd );
    int val;
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &NColums);
    try{ val = (mod->*NColums)(hd); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);

    return(val);   
}

//==== AddRow ====

void TBD::AddColum(unsigned int hd, SColmAttr *colm )
{
    int cnt;

    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	AddColum(i,hd_m,colm);
    }
}

int TBD::AddColum( int idtype, unsigned int hd, SColmAttr *colm )
{
    TModule *mod;
    char *n_f = "AddColum";
    int (TModule::*AddColum)( unsigned int hdi, SColmAttr *colm );
    int val;

    mod=FUse(idtype,n_f,(void (TModule::**)()) &AddColum);
    try{ val = (mod->*AddColum)(hd,colm); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
    return(val);   
}

//==== DelRow ====

void TBD::DelColum(unsigned int hd, string colm)
{
    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
	if(hdBD[hd][i] >= 0) DelColum(i,hdBD[hd][i],colm);
}

void TBD::DelColum( int idtype, unsigned int hd, string colm)
{
    TModule *mod;
    char *n_f = "DelColum";
    void (TModule::*DelColum)( unsigned int hdi, string colm);

    mod=FUse(idtype,n_f,(void (TModule::**)()) &DelColum);
    try{ (mod->*DelColum)(hd,colm); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
}

//==== GetCodePage ====

string TBD::GetCodePage( unsigned int idtype, unsigned int hd)
{
    TModule *mod;
    char *n_f = "GetCodePageBD";
    string (TModule::*GetCodePageBD)( unsigned int hdi );
    string val;
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &GetCodePageBD);
    try{ val = (mod->*GetCodePageBD)(hd); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);

    return(val); 
}

//==== SetCodePage ====

void TBD::SetCodePage( unsigned int idtype, unsigned int hd, string codepage)
{
    TModule *mod;
    char *n_f = "SetCodePageBD";
    int (TModule::*SetCodePageBD)( unsigned int hdi, string codepage );
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &SetCodePageBD);
    try{ (mod->*SetCodePageBD)(hd,codepage); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
}

//==== GetRowAttr ====
void TBD::GetColumAttr( unsigned int hd, int colm, SColmAttr *attr)
{
    int cnt=-1;

    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	GetColumAttr(i,hd_m,colm,attr);
	return;
    }
    throw TError("%s: no active bd!",o_name);
}

void TBD::GetColumAttr( int idtype, unsigned int hd, int colm, SColmAttr *attr )
{
    TModule *mod;
    char *n_f = "GetColumAttr";
    void (TModule::*GetColumAttr)( unsigned int hd, int colm, SColmAttr *attr );
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &GetColumAttr);
    try{ (mod->*GetColumAttr)(hd,colm,attr); }
    catch(...)
    {
	FFree(idtype,n_f);
        throw;
    }
    FFree(idtype,n_f);
}

//==== SetRowAttr ====
void TBD::SetColumAttr( unsigned int hd, int colm, SColmAttr *attr)
{
    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	SetColumAttr(i,hd_m,colm,attr);
	return;
    }
    throw TError("%s: no active bd!",o_name);
}

void TBD::SetColumAttr( int idtype, unsigned int hd, int colm, SColmAttr *attr )
{
    TModule *mod;
    char *n_f = "SetColumAttr";
    void (TModule::*SetColumAttr)( unsigned int hd, int colm, SColmAttr *attr );
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &SetColumAttr);
    try{ (mod->*SetColumAttr)(hd,colm,attr); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);
}

//==== RowNameToId ====
int TBD::ColumNameToId( unsigned int hd, string colm)
{
    int id = -1;

    CheckHD(hd);
    for(int i=0 ; i < hdBD[hd].size(); i++) 
    {
	int hd_m = hdBD[hd][i];
	if(hd_m < 0) continue;
	if(id == -1) id=ColumNameToId(i,hd_m,colm);
	else if( ColumNameToId(i,hd_m,colm) != id ) throw TError("%s: BD stucture error!",o_name);
    }
    if(id < 0) throw TError("%s: no active bd!",o_name);
    
    return(id);
}

int TBD::ColumNameToId( int idtype, unsigned int hd, string colm )
{
    TModule *mod;
    char *n_f = "ColumNameToId";
    int (TModule::*ColumNameToId)(unsigned int hdi, string colm );
    int val;
    
    mod=FUse(idtype,n_f,(void (TModule::**)()) &ColumNameToId);
    try{ val = (mod->*ColumNameToId)(hd,colm); }
    catch(...)
    {
    	FFree(idtype,n_f);
	throw;
    }
    FFree(idtype,n_f);

    return(val);
}


void TBD::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= BD options ======================================\n"
    "    --BDMPath=<path>    Set moduls <path>;\n"
    "    --BDMDef=<mod_name> Set default name BD for new tables;\n"
    "------------------ Fields <%s> sections of config file ----------------\n"
    "modules_path=<path>    set path to modules;\n"
    "\n",n_opt);
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
	    case 'm': DirPath  = optarg; break;
	    case 'd': def_t_bd = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}

void TBD::UpdateOpt()
{
    try{ DirPath = App->GetOpt(n_opt,"modules_path"); } catch(...){  }
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

