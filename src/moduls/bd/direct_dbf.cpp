/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <unistd.h>
#include <string>
#include <errno.h>

#include "../../terror.h"
#include "../../tapplication.h"
#include "../../tmessage.h"
#include "dbf.h"
#include "direct_dbf.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "direct_dbf"
#define NAME_TYPE   "BaseDate"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Modul for direct use DB files *.dbf type, ver 3.0 !"
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

SExpFunc TDirectDB::ExpFuncLc[] = {
    {"NewBD",   ( void ( TModule::* )(  ) ) &TDirectDB::NewBD, "int NewBD( string name );",
     "Create new BD <name>",10,0},
    {"DelBD",   ( void ( TModule::* )(  ) ) &TDirectDB::DelBD, "void DelBD( string name );",
     "Delete BD <name>",10,0},
    {"OpenBD",  ( void ( TModule::* )(  ) ) &TDirectDB::OpenBD, "int OpenBD( string name );",
     "Open BD <name>",10,0},
    {"CloseBD", ( void ( TModule::* )(  ) ) &TDirectDB::CloseBD, "void CloseBD( unsigned int hdi );",
     "Close BD <hdi>",10,0},
    {"SaveBD",  ( void ( TModule::* )(  ) ) &TDirectDB::SaveBD, "void SaveBD(unsigned int hdi );",
     "Save BD <hdi>",10,0},
    {"GetCellS", ( void ( TModule::* )(  ) ) &TDirectDB::GetCellS, "string GetCellS( unsigned int hdi, int colm, int line);",
     "Get string cell from BD <hdi>",10,0},
    {"GetCellR", ( void ( TModule::* )(  ) ) &TDirectDB::GetCellR, "double GetCellR( unsigned int hdi, int colm, int line);",
     "Get numberic cell from BD <hdi>",10,0},
    {"GetCellI", ( void ( TModule::* )(  ) ) &TDirectDB::GetCellI, "int GetCellI( unsigned int hdi, int colm, int line);",
     "Get numberic cell from BD <hdi>",10,0},
    {"GetCellB", ( void ( TModule::* )(  ) ) &TDirectDB::GetCellB, "bool GetCellB( unsigned int hdi, int colm, int line);",
     "Get numberic cell from BD <hdi>",10,0},
    {"SetCellS", ( void ( TModule::* )(  ) ) &TDirectDB::SetCellS, "void SetCellS( unsigned int hdi, int colm, int line, const string cell);",
     "Set string cell to BD <hdi>",10,0},
    {"SetCellR", ( void ( TModule::* )(  ) ) &TDirectDB::SetCellR, "void SetCellR( unsigned int hdi, int colm, int line, double val)",
     "Set numberic cell to BD <hdi>",10,0},
    {"SetCellI", ( void ( TModule::* )(  ) ) &TDirectDB::SetCellI, "void SetCellI( unsigned int hdi, int colm, int line, int val)",
     "Set numberic cell to BD <hdi>",10,0},
    {"SetCellB", ( void ( TModule::* )(  ) ) &TDirectDB::SetCellB, "void SetCellB( unsigned int hdi, int colm, int line, bool val)",
     "Set numberic cell to BD <hdi>",10,0},
    {"NLines", ( void ( TModule::* )(  ) ) &TDirectDB::NLines, "int NLines( unsigned int hdi );",
     "Get number of lines into BD <hdi>",10,0},
    {"AddLine", ( void ( TModule::* )(  ) ) &TDirectDB::AddLine, "int AddLine(unsigned int hdi, unsigned int line);",
     "Add line with number <line> into BD <hdi>",10,0},
    {"DelLine", ( void ( TModule::* )(  ) ) &TDirectDB::DelLine, "void DelLine(unsigned int hdi, unsigned int line);",
     "del line with number <line> into BD <hdi>",10,0},
    {"NColums", ( void ( TModule::* )(  ) ) &TDirectDB::NColums, "int NColums( unsigned int hdi );",
     "Get number of columns into BD <hdi>",10,0},
    {"AddColum", ( void ( TModule::* )(  ) ) &TDirectDB::AddColum, "int AddColum(unsigned int hdi, SColmAttr *colm);",
     "Add <column> to BD <hdi>",10,0},
    {"DelColum", ( void ( TModule::* )(  ) ) &TDirectDB::DelColum, "void DelColum(unsigned int hdi, string colm);",
     "Del <column> from BD <hdi>",10,0},
    {"GetColumAttr", ( void ( TModule::* )(  ) ) &TDirectDB::GetColumAttr, "void GetColumAttr(unsigned int hdi, int colm, SColmAttr *attr );",
     "Get <column> atributes from BD <hdi>",10,0},
    {"SetColumAttr", ( void ( TModule::* )(  ) ) &TDirectDB::SetColumAttr, "void SetColumAttr(unsigned int hdi, int colm, SColmAttr *attr );",
     "Set <clumn> atributes to BD <hdi>",10,0}, 
    {"ColumNameToId", ( void ( TModule::* )(  ) ) &TDirectDB::ColumNameToId, "int ColumNameToId(unsigned int hdi, string colm);",
     "Search id <row> from <namerow> from BD <hdi>",10,0},
    {"GetCodePageBD", ( void ( TModule::* )(  ) ) &TDirectDB::GetCodePageBD, "string GetCodePageBD(unsigned int hdi);",
     "Get a internal charset of BD <hdi>",10,0},
    {"SetCodePageBD", ( void ( TModule::* )(  ) ) &TDirectDB::SetCodePageBD, "void SetCodePageBD(unsigned int hdi, string codepage );",
     "Set a internal charset of BD <hdi>",10,0}     
};

string TDirectDB::o_name = NAME_MODUL;

TDirectDB::TDirectDB( char *name ):TModule(  )
{
    NameModul = NAME_MODUL;
    NameType = NAME_TYPE;
    Vers = VERSION;
    Autors = AUTORS;
    DescrMod = DESCRIPTION;
    License = LICENSE;
    FileName = strdup( name );

    ExpFunc = ( SExpFunc * ) ExpFuncLc;
    NExpFunc = sizeof( ExpFuncLc ) / sizeof( SExpFunc );

    pathsBD ="./";
    extens  =".dbf";
}

TDirectDB::~TDirectDB(  )
{
    free( FileName );
}

TModule *attach( char *FName, int n_mod )
{
    TDirectDB *self_addr;
    if(n_mod==0) self_addr = new TDirectDB( FName );
    else         self_addr = NULL;
    return ( self_addr );
}

void TDirectDB::info( const string & name, string & info )
{
    info.erase(  );
    TModule::info( name, info );
}


void TDirectDB::pr_opt_descr( FILE * stream )
{
    fprintf( stream, 
    "-------------------- %s options --------------------------------------\n"
    "    --DirBDPath=<path>   set dirs alocate BD files (*.dbf);\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "bd_path=<path>           set dirs alocate BD files (*.dbf);\n"
    "\n", NAME_MODUL,NAME_MODUL );
}


void TDirectDB::CheckCommandLine(  )
{
    int next_opt;
    char *short_opt = "h";
    struct option long_opt[] = {
	{"DirBDPath", 1 ,NULL ,'m'},
	{NULL, 0, NULL, 0}
    };

    optind = opterr = 0;
    do
    {
	next_opt = getopt_long( App->argc, ( char *const * ) App->argv, short_opt, long_opt, NULL );
	switch ( next_opt )
	{
	case 'h': pr_opt_descr( stdout ); break;
	case 'm': pathsBD = optarg;       break;
	case -1:  break;
	}
    }
    while ( next_opt != -1 );
}

void TDirectDB::UpdateOpt()
{
    try{ pathsBD = App->GetOpt(NAME_MODUL,"bd_path"); } catch(...){  }
}

void TDirectDB::init( void *param )
{
    TModule::init( param );
}

int TDirectDB::NewBD( string name )
{
    unsigned i;
    
    for(i=0; i < hd.size(); i++)
	if(hd[i].name_bd == name) throw TError(o_name+": BD "+name+"alrady open!");
    TBasaDBF *basa = new TBasaDBF(  );

    for(i=0; i < hd.size(); i++)
	if(hd[i].use <= 0) break;
    if(i == hd.size())
	hd.push_back( );
    hd[i].use      = 1;
    hd[i].name_bd  = name;
    hd[i].basa     = basa;
    hd[i].codepage = "CP866";
	
    return ( i );
}


int TDirectDB::OpenBD( string name )
{
    unsigned i;

    for(i=0; i < hd.size(); i++)
	if(hd[i].name_bd == name) break;
    if(i < hd.size())
    {
    	hd[i].use++; 
	return(i);
    }

    TBasaDBF *basa = new TBasaDBF(  );    
    if( basa->LoadFile( (char *)(pathsBD+'/'+name+extens).c_str() ) == -1 )
    {
	delete basa;
	throw TError(o_name+": open BD "+name+" error!");
    }

    for(i=0; i < hd.size(); i++)
	if(hd[i].use <= 0) break;
    if(i == hd.size())
	hd.push_back( );
    hd[i].use     = 1;
    hd[i].name_bd = name;
    hd[i].basa    = basa;
    hd[i].codepage = "CP866";
    
    return ( i );
}

void TDirectDB::CloseBD( unsigned int hdi )
{
    CheckHD(hdi);
    if( --(hd[hdi].use) > 0) return;
    if(hd[hdi].basa != NULL) delete hd[hdi].basa;
    hd[hdi].use = 0;
    hd[hdi].name_bd.erase();
}

void TDirectDB::SaveBD(unsigned int hdi )
{
    CheckHD(hdi);
    if( hd[hdi].basa->SaveFile((char *)(pathsBD+'/'+hd[hdi].name_bd+extens).c_str()) < 0 )
	throw TError(o_name+": save BD error!"); 
}


void TDirectDB::DelBD(string name )
{
    if(remove( (char *)(pathsBD+'/'+name+extens).c_str() ) < 0 )
	throw TError(o_name+": "+strerror(errno));
}

string TDirectDB::GetCodePageBD( unsigned int hdi )
{
    CheckHD(hdi);
    return(hd[hdi].codepage);
}

void TDirectDB::SetCodePageBD( unsigned int hdi, string codepage )
{
    CheckHD(hdi);
    hd[hdi].codepage=codepage;
}

string TDirectDB::GetCellS( unsigned int hdi, int colm, int line)
{
    int i;
    string val;
    
    CheckHD(hdi);
    if(hd[hdi].basa->GetFieldIt( line, colm, val ) < 0) throw TError(o_name+": cell error!");
    for(i = val.size(); i > 0; i--)
	if(val[i-1]!=' ') break;
    if(i != (int)val.size()) val.resize(i);
	    
    return(val);    
}

double TDirectDB::GetCellR( unsigned int hdi, int colm, int line)
{
    string val;
    
    CheckHD(hdi);
    if(hd[hdi].basa->GetFieldIt( line, colm, val ) < 0) throw TError(o_name+": cell error!");
    return(atof(val.c_str()));
}

int TDirectDB::GetCellI( unsigned int hdi, int colm, int line)
{
    string val;
    
    CheckHD(hdi);
    if(hd[hdi].basa->GetFieldIt( line, colm, val ) < 0) throw TError(o_name+": cell error!");
    return(atoi(val.c_str()));
}

bool TDirectDB::GetCellB( unsigned int hdi, int colm, int line)
{
    string val;
    bool   val1;
    
    CheckHD(hdi);
    if(hd[hdi].basa->GetFieldIt( line, colm, val ) < 0) throw TError(o_name+": cell error!");
    if(val.c_str()[0] == 'T')      val1 = true;
    else if(val.c_str()[0] == 'F') val1 = false;
    else		           val1 = false;
    
    return(val1);
}

void TDirectDB::SetCellS( unsigned int hdi, int colm, int line, const string cell)
{
    CheckHD(hdi);
    if( hd[hdi].basa->ModifiFieldIt( line, colm, (char *)cell.c_str() ) < 0 ) 
	throw TError(o_name+": cell error!");
}

void TDirectDB::SetCellR( unsigned int hdi, int colm, int line, double val)
{
    char str[200];
    db_str_rec *fld_rec;

    CheckHD(hdi);
    if((fld_rec = hd[hdi].basa->getField(colm)) == NULL)     throw TError(o_name+": cell error!");
    sprintf(str,"%*.*f",fld_rec->len_fild,fld_rec->dec_field,val);
    if( hd[hdi].basa->ModifiFieldIt( line, colm, str ) < 0 ) throw TError(o_name+": cell error!");
}

void TDirectDB::SetCellI( unsigned int hdi, int colm, int line, int val)
{
    char str[200];
    db_str_rec *fld_rec;

    CheckHD(hdi);
    if((fld_rec = hd[hdi].basa->getField(colm)) == NULL)     throw TError(o_name+": cell error!");
    sprintf(str,"%*d",fld_rec->len_fild,val);
    if( hd[hdi].basa->ModifiFieldIt( line, colm, str ) < 0 ) throw TError(o_name+": cell error!");
}

void TDirectDB::SetCellB( unsigned int hdi, int colm, int line, bool val)
{
    char str[2];
    db_str_rec *fld_rec;

    CheckHD(hdi);
    if((fld_rec = hd[hdi].basa->getField(colm)) == NULL)     throw TError(o_name+": cell error!");
    if(val == true) str[0] = 'T'; else str[0] = 'F'; str[1] = 0;   
    if( hd[hdi].basa->ModifiFieldIt( line, colm, str ) < 0 ) throw TError(o_name+": cell error!");
}
    

int TDirectDB::NLines( unsigned int hdi )
{
    CheckHD(hdi);
    return( hd[hdi].basa->GetCountItems(  ) );
}


int TDirectDB::AddLine(unsigned int hdi, unsigned int line)
{
    CheckHD(hdi);
    return( hd[hdi].basa->CreateItems(line));
}

void TDirectDB::DelLine(unsigned int hdi, unsigned int line)
{
    CheckHD(hdi);
    if(hd[hdi].basa->DeleteItems(line,1) < 0) throw TError(o_name+": line error!");
}

int TDirectDB::NColums( unsigned int hdi )
{
    int cnt=0;
    CheckHD(hdi);
    while( hd[hdi].basa->getField(cnt) != NULL ) cnt++;
    return( cnt );
}

int TDirectDB::AddColum(unsigned int hdi, SColmAttr *colm)
{
    db_str_rec fld_rec;

    CheckHD(hdi);
    strncpy(fld_rec.name,colm->name.c_str(),11);
    if( colm->tp == BD_ROW_STRING )
    {
	fld_rec.tip_fild  = 'C';
    	fld_rec.len_fild  = colm->len;
        fld_rec.dec_field = 0; 
    }	
    else if( colm->tp == BD_ROW_INT ) 
    {
	fld_rec.tip_fild = 'N'; 
	if(colm->len == 0) fld_rec.len_fild = 5; else fld_rec.len_fild = colm->len; 
        fld_rec.dec_field = 0; 
    }
    else if( colm->tp == BD_ROW_REAL ) 
    {
	fld_rec.tip_fild = 'N'; 
	if(colm->len == 0) fld_rec.len_fild  = 7; else fld_rec.len_fild  = colm->len;
	if(colm->dec == 0) fld_rec.dec_field = 2; else fld_rec.dec_field = colm->dec;
    }
    else if( colm->tp == BD_ROW_BOOLEAN ) 
    {
	fld_rec.tip_fild  = 'L'; 
	fld_rec.len_fild  = 1;
        fld_rec.dec_field = 0;
    } 
    else throw TError(o_name+": type bd error!");  
    memset(fld_rec.res,0,14);    
    int val = hd[hdi].basa->addField(NColums(hdi),&fld_rec);
    if(val < 0) throw TError(o_name+": row error!"); 
    return(val);
}

void TDirectDB::DelColum(unsigned int hdi, string colm)
{
    CheckHD(hdi);
    if( hd[hdi].basa->DelField((char *)colm.c_str()) < 0 ) throw TError(o_name+": row error!"); 
}

void TDirectDB::GetColumAttr(unsigned int hdi, int colm, SColmAttr *attr)
{
    db_str_rec *fld_rec;

    CheckHD(hdi);
    if((fld_rec = hd[hdi].basa->getField(colm)) == NULL) throw TError(o_name+": row error!");
    attr->name = fld_rec->name;
    if(fld_rec->tip_fild == 'C')                                 attr->tp = BD_ROW_STRING;
    else if(fld_rec->tip_fild == 'N' && fld_rec->dec_field == 0) attr->tp = BD_ROW_INT;
    else if(fld_rec->tip_fild == 'N' && fld_rec->dec_field != 0) attr->tp = BD_ROW_REAL;
    else if(fld_rec->tip_fild == 'L')                            attr->tp = BD_ROW_BOOLEAN;
    attr->len  = fld_rec->len_fild;
    attr->dec  = fld_rec->dec_field;
}

void TDirectDB::SetColumAttr(unsigned int hdi, int colm, SColmAttr *attr)
{
    db_str_rec fld_rec;

    CheckHD(hdi);
    strncpy(fld_rec.name, attr->name.c_str(),11);
    if( attr->tp == BD_ROW_STRING )
    {
	fld_rec.tip_fild  = 'C';
    	fld_rec.len_fild  = attr->len;
        fld_rec.dec_field = 0; 
    }	
    else if( attr->tp == BD_ROW_INT ) 
    {
	fld_rec.tip_fild = 'N'; 
	if(attr->len == 0) fld_rec.len_fild = 5; else fld_rec.len_fild = attr->len; 
        fld_rec.dec_field = 0; 
    }
    else if( attr->tp == BD_ROW_REAL ) 
    {
	fld_rec.tip_fild = 'N'; 
	if(attr->len == 0) fld_rec.len_fild  = 7; else fld_rec.len_fild  = attr->len;
	if(attr->dec == 0) fld_rec.dec_field = 2; else fld_rec.dec_field = attr->dec;
    }
    else if( attr->tp == BD_ROW_BOOLEAN ) 
    {
	fld_rec.tip_fild  = 'L'; 
	fld_rec.len_fild  = 1;
        fld_rec.dec_field = 0;
    } 
    else throw TError(o_name+": type bd error!"); 
    if(hd[hdi].basa->setField(colm,&fld_rec) < 0 ) throw TError(o_name+": row error!");
}
    
int TDirectDB::ColumNameToId(unsigned int hdi, string colm)
{
    db_str_rec *fld_rec;

    CheckHD(hdi);
    for(int i=0;(fld_rec = hd[hdi].basa->getField(i)) != NULL;i++)
	if( colm == fld_rec->name ) return(i);
    throw TError(o_name+": row "+colm+" no avoid!");
}

