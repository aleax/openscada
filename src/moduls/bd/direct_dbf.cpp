/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../tsys.h"
#include "../../terror.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "dbf.h"
#include "direct_dbf.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "direct_dbf"
#define NAME_TYPE   "BaseDate"
#define VERSION     "0.1.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Modul for direct use DB files *.dbf type, ver 3.0 !"
#define LICENSE     "GPL"
//==============================================================================

extern "C" TModule *attach( char *FName, int n_mod );

string TDirectDB::o_name = NAME_MODUL;

TDirectDB::TDirectDB( char *name )
{
    NameModul = NAME_MODUL;
    NameType = NAME_TYPE;
    Vers = VERSION;
    Autors = AUTORS;
    DescrMod = DESCRIPTION;
    License = LICENSE;
    FileName = strdup( name );
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
    return static_cast< TModule *>( self_addr );
}

/*
void TDirectDB::info( const string & name, string & info )
{
    info.erase(  );
    TModule::info( name, info );
}
*/

TBD *TDirectDB::BDOpen( string name, bool create )
{
    char   buf[STR_BUF_LEN];           //!!!!

    getcwd(buf,sizeof(buf));
    if(chdir(name.c_str()) != 0)
	if(create == false)               throw TError("%s: open bd %s error!",o_name.c_str(),name.c_str());
	else if(mkdir(name.c_str(),S_IRWXU|S_IRGRP|S_IROTH) != 0) throw TError("%s: create bd %s error!",o_name.c_str(),name.c_str());
    name=buf;
    getcwd(buf,sizeof(buf));
    chdir(name.c_str());
    name=buf;
    //want chek already open bd (directory) //????
    return(new TBDdir(name));
}

void TDirectDB::pr_opt_descr( FILE * stream )
{
    fprintf( stream, 
    "==================== Module %s options ==============================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n", NAME_MODUL,NAME_MODUL );
}


void TDirectDB::mod_CheckCommandLine(  )
{
    int next_opt;
    char *short_opt = "h";
    struct option long_opt[] = {
	{NULL, 0, NULL, 0}
    };

    optind = opterr = 0;
    do
    {
	next_opt = getopt_long( SYS->argc, ( char *const * ) SYS->argv, short_opt, long_opt, NULL );
	switch ( next_opt )
	{
	case 'h': pr_opt_descr( stdout ); break;
	case -1:  break;
	}
    }
    while ( next_opt != -1 );
}

void TDirectDB::mod_UpdateOpt()
{
    
}
/*
void TDirectDB::connect( void *obj )
{
    Tbd = (TTipBD *)obj;
}
*/
//=============================================================
//====================== TBD ==================================
//=============================================================
TBDdir::TBDdir( string name ) : p_bd(name)
{
    char   buf[1024];           //!!!!

    getcwd(buf,sizeof(buf));
    if(chdir(name.c_str()) != 0) throw TError("%s: open bd %s error!",NAME_MODUL,name.c_str());
    chdir(buf);
};

TBDdir::~TBDdir(  )
{
    
}

TTable *TBDdir::TableOpen( string name, bool create )
{
    for(unsigned i=0; i < table.size(); i++)
	if(table[i].use >= 0 && ((TTableDir *)(table[i].tbl))->Name() == name ) return(table[i].tbl);

    return( new TTableDir(p_bd+'/'+name,create) );
}

void TBDdir::TableDel( string table )
{
    if(remove( (char *)(p_bd+'/'+table).c_str() ) < 0 )
	throw TError("%s: %s",NAME_MODUL,strerror(errno));
}

//=============================================================
//====================== TTableDir ============================
//=============================================================
TTableDir::TTableDir(string name, bool create) : n_table(name), codepage("CP866")
{
    basa = new TBasaDBF(  );
    if(create != true)
    	if( basa->LoadFile( (char *)n_table.c_str() ) == -1 )
	{
	    delete basa;
	    throw TError("%s: open table %s error!",NAME_MODUL,n_table.c_str());
	}
}

TTableDir::~TTableDir(  )
{
    delete basa;
}

void TTableDir::_Save( )
{
    basa->SaveFile((char *)n_table.c_str());
}

string TTableDir::_GetCellS( int colm, int line)
{
    int i;
    string val;
    
    if(basa->GetFieldIt( line, colm, val ) < 0) throw TError("%s: cell error!",NAME_MODUL);
    for(i = val.size(); i > 0; i--) if(val[i-1]!=' ') break;
    if(i != (int)val.size()) val.resize(i);
	    
    return(val);
}

double TTableDir::_GetCellR( int colm, int line)
{
    string val;
    
    if(basa->GetFieldIt( line, colm, val ) < 0) throw TError("%s: cell error!",NAME_MODUL);
    return(atof(val.c_str()));
}

int TTableDir::_GetCellI( int colm, int line)
{
    string val;
    
    if(basa->GetFieldIt( line, colm, val ) < 0) throw TError("%s: cell error!",NAME_MODUL);
    return(atoi(val.c_str()));
}

bool TTableDir::_GetCellB( int colm, int line)
{
    string val;
    
    if(basa->GetFieldIt( line, colm, val ) < 0) throw TError("%s: cell error!",NAME_MODUL);
    if(val.c_str()[0] == 'T')      return(true);
    else if(val.c_str()[0] == 'F') return(false);
    else		           return(false);
}

void TTableDir::_SetCellS( int colm, int line, const string cell)
{
    if( basa->ModifiFieldIt( line, colm, (char *)cell.c_str() ) < 0 ) 
	throw TError("%s: cell error!",NAME_MODUL);
}

void TTableDir::_SetCellR( int colm, int line, double val)
{
    char str[200];
    db_str_rec *fld_rec;

    if((fld_rec = basa->getField(colm)) == NULL)     throw TError("%s: cell error!",NAME_MODUL);
    sprintf(str,"%*.*f",fld_rec->len_fild,fld_rec->dec_field,val);
    if( basa->ModifiFieldIt( line, colm, str ) < 0 ) throw TError("%s: cell error!",NAME_MODUL);
}

void TTableDir::_SetCellI( int colm, int line, int val)
{
    char str[200];
    db_str_rec *fld_rec;

    if((fld_rec = basa->getField(colm)) == NULL)     throw TError("%s: cell error!",NAME_MODUL);
    sprintf(str,"%*d",fld_rec->len_fild,val);
    if( basa->ModifiFieldIt( line, colm, str ) < 0 ) throw TError("%s: cell error!",NAME_MODUL);
}

void TTableDir::_SetCellB( int colm, int line, bool val)
{
    char str[2];
    db_str_rec *fld_rec;

    if((fld_rec = basa->getField(colm)) == NULL)     throw TError("%s: cell error!",NAME_MODUL);
    if(val == true) str[0] = 'T'; else str[0] = 'F'; str[1] = 0;   
    if( basa->ModifiFieldIt( line, colm, str ) < 0 ) throw TError("%s: cell error!",NAME_MODUL);
}

int TTableDir::_NLines( )
{
    return( basa->GetCountItems(  ) );
}

int TTableDir::_AddLine( unsigned int line )
{
    return( basa->CreateItems(line));
}

void TTableDir::_DelLine( unsigned int line )
{
    if( basa->DeleteItems(line,1) < 0) throw TError("%s: line error!",NAME_MODUL);
}

int TTableDir::_NColums(  )
{
    int cnt=0;
    while( basa->getField(cnt) != NULL ) cnt++;
    return( cnt );
}

int TTableDir::_AddColum( SColmAttr *colm )
{
    db_str_rec fld_rec;

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
    else throw TError("%s: type bd error!",NAME_MODUL);  
    memset(fld_rec.res,0,14);    
    int val = basa->addField(NColums(),&fld_rec);
    if(val < 0) throw TError("%s: column error!",NAME_MODUL); 
    return(val);
}

void TTableDir::_DelColum( int colm )
{
    if( basa->DelField( colm ) < 0 ) throw TError("%s: column error!",NAME_MODUL); 
}

void TTableDir::_GetColumAttr( int colm, SColmAttr *attr )
{
    db_str_rec *fld_rec;

    if((fld_rec = basa->getField(colm)) == NULL) throw TError("%d: column error!",NAME_MODUL);
    attr->name = fld_rec->name;
    if(fld_rec->tip_fild == 'C')                                 attr->tp = BD_ROW_STRING;
    else if(fld_rec->tip_fild == 'N' && fld_rec->dec_field == 0) attr->tp = BD_ROW_INT;
    else if(fld_rec->tip_fild == 'N' && fld_rec->dec_field != 0) attr->tp = BD_ROW_REAL;
    else if(fld_rec->tip_fild == 'L')                            attr->tp = BD_ROW_BOOLEAN;
    attr->len  = fld_rec->len_fild;
    attr->dec  = fld_rec->dec_field;
}

void TTableDir::_SetColumAttr( int colm, SColmAttr *attr )
{
    db_str_rec fld_rec;

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
    else throw TError("%s: type bd error!",NAME_MODUL); 
    if(basa->setField(colm,&fld_rec) < 0 ) throw TError("%s: column error!",NAME_MODUL);
}

int TTableDir::_ColumNameToId( string colm )
{
    db_str_rec *fld_rec;

    for(int i=0;(fld_rec = basa->getField(i)) != NULL;i++)
	if( colm == fld_rec->name ) return(i);
    throw TError("%s: column %s no avoid!",NAME_MODUL,colm.c_str());
    return(-1);
}

string TTableDir::_GetCodePage( )
{
    return(codepage);
}

void TTableDir::_SetCodePage( string codepage )
{
    codepage=codepage;
}


