
#include <getopt.h>

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
    while(TBD.size())
    {
	delete TBD[0];
	TBD.erase(TBD.begin());
    }
}

void TBDS::ConnectAll( )
{
    for(unsigned i=0;i < Moduls.size();i++)
	if(Moduls[i].stat == GRM_ST_OCCUP) Moduls[i].modul->connect(TBD[i]);
}

int TBDS::OpenTable( string tb_name, string b_name, string t_name, bool create )
{
    int id, id_tb, id_b, id_t;
    id_tb = NameToId(tb_name);
    id_b  = TBD[id_tb]->OpenBD(b_name,create);
    id_t  = TBD[id_tb]->at(id_b)->OpenTable(t_name,create);
    //Find dublicate
    for(id = 0; id < (int)Table.size(); id++)
	if(Table[id].use > 0 && Table[id].type_bd == id_tb && Table[id].bd == id_b && Table[id].table == id_t)
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
    "========================= BD options ======================================\n"
    "    --BDMPath=<path>    Set moduls <path>;\n"
    "------------------ Fields <%s> sections of config file ----------------\n"
    "modules_path=<path>    set path to modules;\n"
    "\n",n_opt);
}


void TBDS::CheckCommandLine( char **argv, int argc )
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
	next_opt=getopt_long(argc,(char * const *)argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath  = optarg; break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}

void TBDS::UpdateOpt()
{
    try{ DirPath = owner->GetOpt(n_opt,"modules_path"); } catch(...){  }
}

int TBDS::AddM( TModule *modul )
{
    int kz=TGRPModule::AddM(modul);
    if(kz < 0) return(kz);
    if(kz == (int)TBD.size()) TBD.push_back( new TTipBD(this,modul) );
    else if(TBD[kz]==TO_FREE) TBD[kz] = new TTipBD(this,modul);
    return(kz);
}

int TBDS::DelM( int hd )
{
    int kz;
    kz=TGRPModule::DelM(hd);
    delete TBD[hd]; 
    TBD[hd] = TO_FREE;    
    
    return(0);
}


//================================================================
//=========== TTipBD =============================================
//================================================================

const char *TTipBD::o_name = "TTipBD";

TTipBD::~TTipBD( )
{
    for(unsigned id=0; id < bd.size(); id++) 
	while(bd[id].use > 0) CloseBD(id);
}

int TTipBD::OpenBD( string name, bool create )
{    
    TBD *(TModule::*BDOpen)(string name, bool create );
    char *n_f = "BDOpen";
    int id;
    TBD *t_bd;

    //want resource bd request //????
    module->GetFunc(n_f,(void (TModule::**)()) &BDOpen);
    try{ t_bd = (module->*BDOpen)(name,create); }
    catch(...)
    {
    	module->FreeFunc(n_f);
    	//want resource bd free //????
	throw;
    }
    module->FreeFunc(n_f);
    //find dublicate bd
    for(id=0; id < (int)bd.size(); id++) if(bd[id].use > 0 && bd[id].bd == t_bd ) break;
    if(id < (int)bd.size()) bd[id].use++; 
    else
    {
	for(id=0; id < (int)bd.size(); id++) if(bd[id].use <= 0) break;
	if(id == (int)bd.size()) bd.push_back();
	bd[id].use = 1;
	bd[id].bd  = t_bd;
    }
    //want resource bd free //????

    return(id);
}

void TTipBD::CloseBD( unsigned int id )
{
    if(id > bd.size() || bd[id].use <= 0 ) throw TError("%s: bd identificator error!",o_name);
    //want resource bd request //????
    if((--bd[id].use) > 0 )
    {
    	//want resource bd free //????
	return;
    }
    else delete bd[id].bd;
    //want resource bd free //????
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

TBD::~TBD()
{
    for(unsigned id=0; id < table.size(); id++) 
	while(table[id].use > 0) CloseTable(id);
}

int TBD::OpenTable( string name, bool create )
{
    int id;

    //want resource table request //????
    TTable *tbl = TableOpen(name, create);
    //find dublicate table
    for(id=0; id < (int)table.size(); id++) if(table[id].use > 0 && table[id].tbl == tbl ) break;
    if(id < (int)table.size()) table[id].use++; 
    else
    {
	for(id=0; id < (int)table.size(); id++) if(table[id].use <= 0) break;
	if(id == (int)table.size()) table.push_back();
	table[id].use = 1;
	table[id].tbl = tbl;
    }
    //want resource table free //????

    return(id);
}

void TBD::CloseTable( unsigned int id )
{
    if(id > table.size() || table[id].use <= 0 ) throw TError("%s: table identificator error!",o_name);
    //want resource bd request //????
    if((--table[id].use) > 0 )
    {
    	//want resource bd free //????
	return;
    }
    else delete table[id].tbl;
    //want resource bd free //????
}

//================================================================
//=========== TTable =============================================
//================================================================
const char *TTable::o_name = "TTable";
char *TTable::_err   = "%s: function %s no support!";

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

