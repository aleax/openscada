/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>

#include "../../tapplication.h"
#include "../../tmessage.h"
#include "dbf.h"
#include "direct_bd.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "direct_bd"
#define NAME_TYPE   "BaseDate"
#define VERSION     "0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Modul for direct use DB files *.dbf type, ver 3.0 !"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

SExpFunc TDirectDB::ExpFuncLc[] = {
    {"NewBD",   ( void ( TModule::* )(  ) ) &TDirectDB::NewBD, "int NewBD( string name );",
     "Create new BD <name>"},
    {"OpenBD",  ( void ( TModule::* )(  ) ) &TDirectDB::OpenBD, "int OpenBD( string name );",
     "Open BD <name>"},
    {"CloseBD", ( void ( TModule::* )(  ) ) &TDirectDB::CloseBD, "int CloseBD( int hdi );",
     "Close BD <hdi>"},
    {"SaveBD",  ( void ( TModule::* )(  ) ) &TDirectDB::SaveBD, "int SaveBD(unsigned int hdi );",
     "Save BD <hdi>"},
    {"GetCharSetBD", ( void ( TModule::* )(  ) ) &TDirectDB::GetCharSetBD, "char *GetCharSetBD(int hdi);",
     "Get a internal charset of BD <hdi>"},
    {"GetCell1", ( void ( TModule::* )(  ) ) &TDirectDB::GetCell1, "int GetCell1( int hdi, int row, int line, string & cell);",
     "Get cell from BD <hdi>"},
    {"GetCell2", ( void ( TModule::* )(  ) ) &TDirectDB::GetCell2, "int GetCell2( int hdi, string row, int line, string & cell);",
     "Get cell from BD <hdi>"},
    {"SetCell1", ( void ( TModule::* )(  ) ) &TDirectDB::SetCell1, "int SetCell1( int hdi, int row, int line, const string & cell);",
     "Set cell to BD <hdi>"},
    {"SetCell2", ( void ( TModule::* )(  ) ) &TDirectDB::SetCell2, "int SetCell2( int hdi, string row, int line, const string & cell);",
     "Set cell to BD <hdi>"},
    {"NLines", ( void ( TModule::* )(  ) ) &TDirectDB::NLines, "int NLines( int hdi );",
     "Get number of lines into BD <hdi>"},
    {"AddLine", ( void ( TModule::* )(  ) ) &TDirectDB::AddLine, "int AddLine(unsigned int hdi, unsigned int line);",
     "Add line with number <line> into BD <hdi>"},
    {"DelLine", ( void ( TModule::* )(  ) ) &TDirectDB::DelLine, "int DelLine(unsigned int hdi, unsigned int line);",
     "del line with number <line> into BD <hdi>"},
    {"NRows", ( void ( TModule::* )(  ) ) &TDirectDB::NRows, "int NRows( int hdi );",
     "Get number of rows into BD <hdi>"},
    {"AddRow", ( void ( TModule::* )(  ) ) &TDirectDB::AddRow, "int AddRow(unsigned int hdi, string row, char type, unsigned int len=10, unsigned int dec=2);",
     "Add row <row> to BD <hdi>"},
    {"DelRow", ( void ( TModule::* )(  ) ) &TDirectDB::DelRow, "int DelRow(unsigned int hdi, string row);",
     "Del row <row> from BD <hdi>"}
};


TDirectDB::TDirectDB( char *name ):TModule(  )
{
    NameModul = NAME_MODUL;
    NameType = NAME_TYPE;
    Vers = VERSION;
    Autors = AUTORS;
    DescrMod = DESCRIPTION;
    FileName = strdup( name );

    ExpFunc = ( SExpFunc * ) ExpFuncLc;
    NExpFunc = sizeof( ExpFuncLc ) / sizeof( SExpFunc );

    pathsBD ="./";
    extens  =".dbf";

#if debug
    App->Mess->put( 1, "Run constructor %s file %s is OK!", NAME_MODUL, FileName );
#endif
}

TDirectDB::~TDirectDB(  )
{
#if debug
    App->Mess->put( 1, "Run destructor %s file %s is OK!", NAME_MODUL, FileName );
#endif
    free( FileName );
}

TModule *attach( char *FName, int n_mod )
{
    TDirectDB *self_addr;
    if(n_mod==0) self_addr = new TDirectDB( FName );
    else         self_addr = NULL;
    return ( self_addr );
}

int TDirectDB::info( const string & name, string & info )
{
    info.erase(  );
    TModule::info( name, info );
    return ( MOD_NO_ERR );
}


void TDirectDB::pr_opt_descr( FILE * stream )
{
    fprintf( stream, 
    "-------------------- %s options --------------------------------------\n"
    "    --DirBDPath=<path>   Set dirs alocate BD files (*.dbf);\n"
    "\n", NAME_MODUL );
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
	case 'h':
	    pr_opt_descr( stdout );
	    break;
	case 'm':
	    pathsBD.assign(optarg); 
	    break;
	case -1:
	    break;
	}
    }
    while ( next_opt != -1 );
}

int TDirectDB::init(  )
{
    CheckCommandLine(  );
    TModule::init(  );
    return ( MOD_NO_ERR );
}

int TDirectDB::NewBD( string name )
{
    int    i;

    for(i=0; i < hd.size(); i++)
	if(hd[i]->name_bd == name) return(-1);
    TBasaDBF *basa = new TBasaDBF(  );

    for(i=0; i < hd.size(); i++)
	if(hd[i]->use <= 0) break;
    if(i == hd.size())
	hd.push_back(new Shd);
    hd[i]->use     = 1;
    hd[i]->name_bd = name;
    hd[i]->basa    = basa;
	
    return ( i );
}


int TDirectDB::OpenBD( string name )
{
    int    i;

    for(i=0; i < hd.size(); i++)
	if(hd[i]->name_bd == name) break;
    if(i < hd.size())
    {
    	hd[i]->use++; 
	return(i);
    }

    TBasaDBF *basa = new TBasaDBF(  );    
    if( basa->LoadFile( (char *)(pathsBD+'/'+name+extens).c_str() ) == -1 )
    {
	delete basa;
	return(-1);
    }

    for(i=0; i < hd.size(); i++)
	if(hd[i]->use <= 0) break;
    if(i == hd.size())
	hd.push_back(new Shd);
    hd[i]->use     = 1;
    hd[i]->name_bd = name;
    hd[i]->basa    = basa;
	
    return ( i );
}

int TDirectDB::CloseBD( int hdi )
{
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(-1);
    if( --(hd[hdi]->use) > 0) return(0);
    
    if(hd[hdi]->basa != NULL) delete hd[hdi]->basa;
    hd[hdi]->use=0;
    hd[hdi]->name_bd.erase();

    return(0);
}

int TDirectDB::SaveBD(unsigned int hdi )
{
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(-1);
    return( hd[hdi]->basa->SaveFile((char *)(pathsBD+'/'+hd[hdi]->name_bd+extens).c_str()) );
}
    
char *TDirectDB::GetCharSetBD(int hdi)
{
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(NULL);
    return("CP866");
}

int TDirectDB::GetCell1( int hdi, int row, int line, string & cell)
{
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(-1);
    int kz = hd[hdi]->basa->GetFieldIt( line, row, cell );

    return(kz);    
}

int TDirectDB::GetCell2( int hdi, string row, int line, string & cell)
{
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(-1);
    int kz = hd[hdi]->basa->GetFieldIt( line, (char *)row.c_str(), cell );

    return(kz);    
}

int TDirectDB::SetCell1( int hdi, int row, int line, const string & cell)
{
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(-1);
    int kz = hd[hdi]->basa->ModifiFieldIt( line, row, (char *)cell.c_str() );

    return(kz);    
}

int TDirectDB::SetCell2( int hdi, string row, int line, const string & cell)
{
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(-1);
    int kz = hd[hdi]->basa->ModifiFieldIt( line, (char *)row.c_str(), (char *)cell.c_str() );

    return(kz);    
}

int TDirectDB::NLines( int hdi )
{
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(0);
    return( hd[hdi]->basa->GetCountItems(  ) );
}

int TDirectDB::NRows( int hdi )
{
    int cnt=0;
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(0);
    while( hd[hdi]->basa->getField(cnt) != NULL ) cnt++;
    return( cnt );
}

int TDirectDB::AddLine(unsigned int hdi, unsigned int line)
{
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(-1);
    return( hd[hdi]->basa->CreateItems(line));
}

int TDirectDB::DelLine(unsigned int hdi, unsigned int line)
{
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(-1);
    return( hd[hdi]->basa->DeleteItems(line,1));
}

int TDirectDB::AddRow(unsigned int hdi, string row, char type, unsigned int len=10, unsigned int dec=2)
{
    db_str_rec fld_rec;
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(-1);
    strncpy(fld_rec.name,row.c_str(),11);
    fld_rec.tip_fild = type;
    fld_rec.len_fild = len;
    fld_rec.dec_field = dec;    
    memset(fld_rec.res,0,14);    
    return( hd[hdi]->basa->addField(10000,&fld_rec));
}

int TDirectDB::DelRow(unsigned int hdi, string row)
{
    if(hdi>=hd.size() || hd[hdi]->use <= 0 ) return(-1);
    return( hd[hdi]->basa->DelField((char *)row.c_str()));
}


