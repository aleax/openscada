
#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "tbds.h"

//================================================================
//=========== TBDS ===============================================
//================================================================

const char *TBDS::o_name = "TBDS";
const char *TBDS::n_opt  = "bd";

TBDS::TBDS( TKernel *app ) : TGRPModule(app,"BaseDate")
{

}

TBDS::~TBDS(  )
{
    for(unsigned i_m = 0; i_m < TBD.size(); i_m++) gmd_DelM(i_m);
}

int TBDS::OpenTable( string tb_name, string b_name, string t_name, bool create )
{
    int id, id_tb, id_b, id_t;
    id_tb = gmd_NameToId(tb_name);
    id_b  = TBD[id_tb]->OpenBD(b_name,create);
    id_t  = TBD[id_tb]->at(id_b)->OpenTable(t_name,create);
    //Find dublicate
    for(id = 0; id < (int)Table.size(); id++)
	if( Table[id].use > 0 && Table[id].type_bd == id_tb && 
	    Table[id].bd == id_b && Table[id].table == id_t)
    	    break;
    if(id < (int)Table.size()) Table[id].use++;
    else
    {
	for(id = 0; id < (int)Table.size(); id++)
	    if(Table[id].use <= 0) break;
	if(id == (int)Table.size()) Table.push_back();
        Table[id].use     = 1;
	Table[id].type_bd = id_tb;
	Table[id].bd      = id_b;
	Table[id].table   = id_t;
    }

    return(id);
}

void TBDS::CloseTable( unsigned int id )
{
    if(id > Table.size() || Table[id].use <= 0) throw TError("%s: table identificator error!",o_name);
    TBD[Table[id].type_bd]->at(Table[id].bd)->CloseTable(Table[id].table);
    TBD[Table[id].type_bd]->CloseBD(Table[id].bd);
    Table[id].use--;
}

TTable *TBDS::at_tbl( unsigned int id )
{
    if(id > Table.size() || Table[id].use <= 0) throw TError("%s: table identificator error!",o_name);
    return(TBD[Table[id].type_bd]->at(Table[id].bd)->at(Table[id].table));
}

void TBDS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= %s options ======================================\n"
    "    --BDMPath=<path>    Set moduls <path>;\n"
    "------------------ Fields <%s> sections of config file ----------------\n"
    "modules_path=<path>    set path to modules;\n"
    "\n",gmd_NameTMod().c_str(),n_opt);
}


void TBDS::gmd_CheckCommandLine( )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"BDMPath" ,1,NULL,'m'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath  = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}

void TBDS::gmd_UpdateOpt()
{
    string opt;
    if( SYS->GetOpt(n_opt,"modules_path",opt) ) DirPath = opt;
}

int TBDS::gmd_AddM( TModule *modul )
{
    int hd=TGRPModule::gmd_AddM(modul);
    if(hd < 0) return(hd);
    if(hd == (int)TBD.size()) TBD.push_back(static_cast< TTipBD *>(modul) );
    else if(TBD[hd]==TO_FREE) TBD[hd] = static_cast< TTipBD *>(modul);
    TBD[hd]->owner = this;
    return(hd);
}

void TBDS::gmd_DelM( unsigned hd )
{
    if(hd >= TBD.size() || TBD[hd]==TO_FREE) 
	throw TError("%s: Module header %d error!",o_name,hd);	
    TBD[hd] = TO_FREE;    
    TGRPModule::gmd_DelM(hd);
}


//================================================================
//=========== TTipBD =============================================
//================================================================

const char *TTipBD::o_name = "TTipBD";
TTipBD::TTipBD(  )
{ 
    hd_res = SYS->ResCreate();
};

TTipBD::~TTipBD( )
{
    SYS->ResRequest(hd_res);
    for(unsigned id=0; id < bd.size(); id++) 
	if(bd[id].use > 0) 
	{ 
	    delete bd[id].bd;
	    bd[id].use = 0;
	}
    SYS->ResDelete(hd_res);    
}

unsigned int TTipBD::OpenBD( string name, bool create )
{    
    unsigned id;

    TBD *t_bd = BDOpen(name,create);
    //find dublicate bd
    SYS->ResRequest(hd_res);
    for(id=0; id < bd.size(); id++) if(bd[id].use > 0 && bd[id].bd == t_bd ) break;
    if(id < bd.size()) bd[id].use++; 
    else
    {
	for(id=0; id < bd.size(); id++) if(bd[id].use <= 0) break;
	if(id == bd.size()) bd.push_back();
	bd[id].use = 1;
	bd[id].bd  = t_bd;
    }
    SYS->ResRelease(hd_res);    

    return(id);
}

void TTipBD::CloseBD( unsigned int id )
{
    SYS->ResRequest(hd_res);
    if(id > bd.size() || bd[id].use <= 0 )
    { 
    	SYS->ResRelease(hd_res);    
	throw TError("%s: bd identificator error!",o_name);
    }
    if((--bd[id].use) <= 0 ) delete bd[id].bd;
    SYS->ResRelease(hd_res);    
}

TBD *TTipBD::at( unsigned int id ) 
{ 
    if(id > bd.size() || bd[id].use <= 0 ) throw TError("%s: bd identificator error!",o_name); 
    return(bd[id].bd);
}

//================================================================
//=========== TBD ================================================
//================================================================

const char *TBD::o_name = "TBD";


TBD::TBD()
{    
    hd_res = SYS->ResCreate();
}

TBD::~TBD()
{
    SYS->ResRequest(hd_res);
    for(unsigned id=0; id < table.size(); id++) 
	if(table[id].use > 0) 
	{
	    delete table[id].tbl;
	    table[id].use = 0;
	}
    SYS->ResDelete(hd_res);    
}

int TBD::OpenTable( string name, bool create )
{
    int id;

    TTable *tbl = TableOpen(name, create);
    //find dublicate table
    SYS->ResRequest(hd_res);
    for(id=0; id < (int)table.size(); id++) if(table[id].use > 0 && table[id].tbl == tbl ) break;
    if(id < (int)table.size()) table[id].use++; 
    else
    {
	for(id=0; id < (int)table.size(); id++) if(table[id].use <= 0) break;
	if(id == (int)table.size()) table.push_back();
	table[id].use = 1;
	table[id].tbl = tbl;
    }
    SYS->ResRelease(hd_res);    

    return(id);
}

void TBD::CloseTable( unsigned int id )
{
    SYS->ResRequest(hd_res);
    if(id > table.size() || table[id].use <= 0 )
    { 
    	SYS->ResRelease(hd_res);    
	throw TError("%s: table identificator error!",o_name);
    }
    if((--table[id].use) <= 0 ) delete table[id].tbl;
    SYS->ResRelease(hd_res);    
}

//================================================================
//=========== TTable =============================================
//================================================================
const char *TTable::o_name = "TTable";
char *TTable::_err   = "%s: function %s no support!";

TTable::TTable() : use(0), hd_res(SYS->ResCreate(100))
{

};

TTable::~TTable()
{ 
    while(use); 
    SYS->ResDelete(hd_res);    
};

void TTable::Save()
{
    ENTER(); 
    try{ _Save(); } catch(...){ EXIT(); throw; } 
    EXIT();
}

string TTable::GetCellS( int colm, int line)
{
    string val;
    ENTER();
    try
    {
	val = _GetCellS(colm, line);
	Mess->SconvIn(_GetCodePage().c_str(),val);
    } catch(...){ EXIT(); throw; } 
    EXIT();
    return(val);
}

double TTable::GetCellR( int colm, int line)
{
    double val;
    ENTER();
    try{ val = _GetCellR( colm, line); } catch(...){ EXIT(); throw; } 
    EXIT();
    return(val);
}
    
int TTable::GetCellI( int colm, int line)
{
    int val;
    ENTER();
    try{ val = _GetCellI( colm, line); } catch(...){ EXIT(); throw; } 
    EXIT();
    return(val);
}
    
bool TTable::GetCellB( int colm, int line)
{
    bool val;
    ENTER();
    try{ val = _GetCellB( colm, line); } catch(...){ EXIT(); throw; } 
    EXIT();
    return(val);
}
    
void TTable::SetCellS( int colm, int line, const string cell)
{
    ENTER();
    string cell_t(cell);
    try
    {
	Mess->SconvOut(_GetCodePage( ).c_str(),cell_t);
	_SetCellS( colm, line, cell_t); 
    } catch(...){ EXIT(); throw; } 
    EXIT();
}

void TTable::SetCellR( int colm, int line, double val)
{
    ENTER();  
    try{ _SetCellR(colm,line,val); } catch(...){ EXIT(); throw; }
    EXIT();
}

void TTable::SetCellI( int colm, int line, int val)
{
    ENTER();  
    try{ _SetCellI(colm,line,val); } catch(...){ EXIT(); throw; }
    EXIT();
}

void TTable::SetCellB( int colm, int line, bool val)
{
    ENTER(); 
    try{ _SetCellB(colm,line,val); } catch(...){ EXIT(); throw; }
    EXIT();
}

int TTable::NLines( )
{
    int val;
    ENTER();
    try{ val = _NLines(); } catch(...){ EXIT(); throw; } 
    EXIT();
    return(val);
}

int TTable::AddLine( unsigned int line)
{
    int val;
    ENTER();
    try{ val = _AddLine(line); } catch(...){ EXIT(); throw; } 
    EXIT();
    return(val);
}

void TTable::DelLine( unsigned int line)
{
    ENTER();
    try{ _DelLine(line); } catch(...){ EXIT(); throw; }
    EXIT();
}

int TTable::NColums( )
{
    int val;
    ENTER();
    try{ val = _NColums(); } catch(...){ EXIT(); throw; } 
    EXIT();
    return(val);
}

int TTable::AddColum( SColmAttr *colm )
{
    int val;
    ENTER();
    try{ val = _AddColum(colm); } catch(...){ EXIT(); throw; } 
    EXIT();
    return(val);
}

void TTable::DelColum( int colm)
{
    ENTER(); 
    try{ _DelColum(colm); } catch(...){ EXIT(); throw; }   
    EXIT();
}
    
void TTable::GetColumAttr( int colm, SColmAttr *attr )
{
    ENTER(); 
    try{ _GetColumAttr(colm,attr); } catch(...){ EXIT(); throw; }
    EXIT();
}

void TTable::SetColumAttr( int colm, SColmAttr *attr )
{
    ENTER(); 
    try{ _SetColumAttr(colm,attr); } catch(...){ EXIT(); throw; }  
    EXIT();
}
    
int TTable::ColumNameToId( string colm )
{
    int val;
    ENTER();
    try{ val = _ColumNameToId(colm); } catch(...){ EXIT(); throw; }
    EXIT();
    return(val);
}

string TTable::GetCodePage( )
{
    string val;
    ENTER();
    try{ val = _GetCodePage( ); } catch(...){ EXIT(); throw; }
    EXIT();
    return(val);
}

void TTable::SetCodePage( string codepage )
{
    ENTER(); 
    try{ _SetCodePage( codepage ); } catch(...){ EXIT(); throw; }
    EXIT();
}

void TTable::ENTER()
{ 
    SYS->ResRequest(hd_res);
    use++;
}

void TTable::EXIT()
{ 
    SYS->ResRelease(hd_res);
    use--; 
}


