/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <unistd.h>
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
#define LICENSE     "LGPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

SExpFunc TDirectDB::ExpFuncLc[] = {
    {"NewBD",   ( void ( TModule::* )(  ) ) &TDirectDB::NewBD, "int NewBD( string name );",
     "Create new BD <name>",10,0},
    {"DelBD",   ( void ( TModule::* )(  ) ) &TDirectDB::DelBD, "int DelBD( string name );",
     "Delete BD <name>",10,0},
    {"OpenBD",  ( void ( TModule::* )(  ) ) &TDirectDB::OpenBD, "int OpenBD( string name );",
     "Open BD <name>",10,0},
    {"CloseBD", ( void ( TModule::* )(  ) ) &TDirectDB::CloseBD, "int CloseBD( int hdi );",
     "Close BD <hdi>",10,0},
    {"SaveBD",  ( void ( TModule::* )(  ) ) &TDirectDB::SaveBD, "int SaveBD(unsigned int hdi );",
     "Save BD <hdi>",10,0},
    {"GetCellS", ( void ( TModule::* )(  ) ) &TDirectDB::GetCellS, "int GetCellS( int hdi, int row, int line, string & cell);",
     "Get string cell from BD <hdi>",10,0},
    {"GetCellN", ( void ( TModule::* )(  ) ) &TDirectDB::GetCellN, "int GetCellN( int hdi, int row, int line, double & val);",
     "Get numberic cell from BD <hdi>",10,0},
    {"SetCellS", ( void ( TModule::* )(  ) ) &TDirectDB::SetCellS, "int SetCellS( int hdi, int row, int line, const string & cell);",
     "Set string cell to BD <hdi>",10,0},
    {"SetCellN", ( void ( TModule::* )(  ) ) &TDirectDB::SetCellN, "int SetCellN( int hdi, int row, int line, double val)",
     "Set numberic cell to BD <hdi>",10,0},
    {"NLines", ( void ( TModule::* )(  ) ) &TDirectDB::NLines, "int NLines( int hdi );",
     "Get number of lines into BD <hdi>",10,0},
    {"AddLine", ( void ( TModule::* )(  ) ) &TDirectDB::AddLine, "int AddLine(unsigned int hdi, unsigned int line);",
     "Add line with number <line> into BD <hdi>",10,0},
    {"DelLine", ( void ( TModule::* )(  ) ) &TDirectDB::DelLine, "int DelLine(unsigned int hdi, unsigned int line);",
     "del line with number <line> into BD <hdi>",10,0},
    {"NRows", ( void ( TModule::* )(  ) ) &TDirectDB::NRows, "int NRows( int hdi );",
     "Get number of rows into BD <hdi>",10,0},
    {"AddRow", ( void ( TModule::* )(  ) ) &TDirectDB::AddRow, "int AddRow(unsigned int hdi, string row, char type, unsigned int len, unsigned int dec);",
     "Add <row> to BD <hdi>",10,0},
    {"DelRow", ( void ( TModule::* )(  ) ) &TDirectDB::DelRow, "int DelRow(unsigned int hdi, string row);",
     "Del <row> from BD <hdi>",10,0},
    {"GetRowAttr", ( void ( TModule::* )(  ) ) &TDirectDB::GetRowAttr, "int GetRowAttr(unsigned int hdi, int row, string & namerow, char & type, unsigned int & len, unsigned int & dec);",
     "Get <row> atributes from BD <hdi>",10,0},
    {"RowNameToId", ( void ( TModule::* )(  ) ) &TDirectDB::RowNameToId, "int RowNameToId(unsigned int hdi, string namerow);",
     "Search id <row> from <namerow> from BD <hdi>",10,0},
    {"GetCodePageBD", ( void ( TModule::* )(  ) ) &TDirectDB::GetCodePageBD, "int GetCodePageBD(int hdi, string & codepage );",
     "Get a internal charset of BD <hdi>",10,0},
    {"SetCodePageBD", ( void ( TModule::* )(  ) ) &TDirectDB::SetCodePageBD, "int SetCodePageBD(int hdi, string codepage );",
     "Set a internal charset of BD <hdi>",10,0}     
};


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

int TDirectDB::init( void *param )
{
    CheckCommandLine(  );
    TModule::init( param );
    return ( MOD_NO_ERR );
}

int TDirectDB::NewBD( string name )
{
    int    i;

    for(i=0; i < hd.size(); i++)
	if(hd[i].name_bd == name) return(-1);
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
    int    i;

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
	return(-1);
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

int TDirectDB::CloseBD( int hdi )
{
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    if( --(hd[hdi].use) > 0) return(0);
    
    if(hd[hdi].basa != NULL) delete hd[hdi].basa;
    hd[hdi].use=0;
    hd[hdi].name_bd.erase();

    return(0);
}

int TDirectDB::SaveBD(unsigned int hdi )
{
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    return( hd[hdi].basa->SaveFile((char *)(pathsBD+'/'+hd[hdi].name_bd+extens).c_str()) );
}


int TDirectDB::DelBD(string name )
{
    return(unlink( (char *)(pathsBD+'/'+name+extens).c_str() ));
}

int TDirectDB::GetCodePageBD(int hdi, string & codepage )
{
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(NULL);
    codepage=hd[hdi].codepage;
    return(0);
}

int TDirectDB::SetCodePageBD(int hdi, string codepage )
{
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(NULL);
    hd[hdi].codepage=codepage;
    return(0);
}

int TDirectDB::GetCellS( int hdi, int row, int line, string & cell)
{
    int i,kz;
    
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    kz = hd[hdi].basa->GetFieldIt( line, row, cell );
    for(i = cell.size(); i > 0; i--)
	if(cell[i-1]!=' ') break;
    if(i != cell.size()) cell.resize(i);
	    
    return(kz);    
}

int TDirectDB::GetCellN( int hdi, int row, int line, double & val)
{
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    string cell;
    int kz = hd[hdi].basa->GetFieldIt( line, row, cell );
    val=atof(cell.c_str());

    return(kz);    
}

int TDirectDB::SetCellS( int hdi, int row, int line, const string & cell)
{
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    int kz = hd[hdi].basa->ModifiFieldIt( line, row, (char *)cell.c_str() );

    return(kz);    
}

int TDirectDB::SetCellN( int hdi, int row, int line, double val)
{
    char str[200];
    db_str_rec *fld_rec;

    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    if((fld_rec = hd[hdi].basa->getField(row)) == NULL)    return(-1);
    sprintf(str,"%*.*f",fld_rec->len_fild,fld_rec->dec_field,val);
    int kz = hd[hdi].basa->ModifiFieldIt( line, row, str );

    return(kz);    
}

int TDirectDB::NLines( int hdi )
{
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(0);
    return( hd[hdi].basa->GetCountItems(  ) );
}


int TDirectDB::AddLine(unsigned int hdi, unsigned int line)
{
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    return( hd[hdi].basa->CreateItems(line));
}

int TDirectDB::DelLine(unsigned int hdi, unsigned int line)
{
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    return( hd[hdi].basa->DeleteItems(line,1));
}

int TDirectDB::NRows( int hdi )
{
    int cnt=0;
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(0);
    while( hd[hdi].basa->getField(cnt) != NULL ) cnt++;
    return( cnt );
}

int TDirectDB::AddRow(unsigned int hdi, string row, char type, unsigned int len, unsigned int dec)
{
    db_str_rec fld_rec;

    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    strncpy(fld_rec.name,row.c_str(),11);
    fld_rec.tip_fild = type;
    fld_rec.len_fild = len;
    fld_rec.dec_field = dec;    
    memset(fld_rec.res,0,14);    
    return( hd[hdi].basa->addField(10000,&fld_rec));
}

int TDirectDB::DelRow(unsigned int hdi, string row)
{
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    return( hd[hdi].basa->DelField((char *)row.c_str()));
}

int TDirectDB::GetRowAttr(unsigned int hdi, int row, string & namerow, char & type, unsigned int & len, unsigned int & dec)
{
    db_str_rec *fld_rec;

    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    if((fld_rec = hd[hdi].basa->getField(row)) == NULL)      return(-1);
    namerow = fld_rec->name;
    type = fld_rec->tip_fild;
    len = fld_rec->len_fild;
    dec = fld_rec->dec_field;

    return( 0 );
}

int TDirectDB::RowNameToId(unsigned int hdi, string namerow)
{
    db_str_rec *fld_rec;
    if(hdi>=hd.size() || hd[hdi].use <= 0 ) return(-1);
    for(int i=0;(fld_rec = hd[hdi].basa->getField(i)) != NULL;i++)
	if( namerow == fld_rec->name ) return(i);

    return( -1 );
}

