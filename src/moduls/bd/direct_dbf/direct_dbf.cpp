/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <getopt.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <tsys.h>
#include <terror.h>
#include <tkernel.h>
#include <tmessage.h>
#include "dbf.h"
#include "direct_dbf.h"

//============ Modul info! =====================================================
#define MOD_ID      "direct_dbf"
#define MOD_NAME    "DB DBF"
#define MOD_TYPE    "BD"
#define VER_TYPE    VER_BD
#define VERSION     "1.0.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Modul for direct use DB files *.dbf type, ver 3.0 !"
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    SAtMod module( int n_mod )
    {
	SAtMod AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
    	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
    	}
	else
	    AtMod.id	= "";
	return( AtMod );
    }

    TModule *attach( const SAtMod &AtMod, const string &source )
    {
	TDirectDB *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
    	    self_addr = new TDirectDB( source );       

	return ( self_addr );
    }
}


TDirectDB::TDirectDB( string name ) 
{
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE; 
    Vers      	= VERSION; 
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION; 
    License   	= LICENSE; 
    Source    	= name;

}

TDirectDB::~TDirectDB(  )
{

}


TBD *TDirectDB::BDOpen( const string &name, bool create )
{
    char   buf[STR_BUF_LEN];           //!!!!

    getcwd(buf,sizeof(buf));
    if(chdir(name.c_str()) != 0)
	if(create == false)               throw TError("%s: open bd %s error!",MOD_ID,name.c_str());
	else if(mkdir(name.c_str(),S_IRWXU|S_IRGRP|S_IROTH) != 0) throw TError("%s: create bd %s error!",MOD_ID,name.c_str());
    chdir(buf);
    //name=buf;
    //getcwd(buf,sizeof(buf));
    //chdir(name.c_str());
    //name=buf;
    //want chek already open bd (directory) //????
    return(new TBDdir(name));
}

void TDirectDB::pr_opt_descr( FILE * stream )
{
    fprintf( stream, 
    "======================= The module <%s:%s> options =======================\n"
    "---------- Parameters of the module section <%s> in config file ----------\n"
    "\n",MOD_TYPE,MOD_ID,MOD_ID );
}


void TDirectDB::modCheckCommandLine(  )
{
    int next_opt;
    char *short_opt = "h";
    struct option long_opt[] = 
    {
	{"help", 0, NULL,'h'},
	{NULL  , 0, NULL, 0}
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

void TDirectDB::modUpdateOpt()
{
    
}

//=============================================================
//====================== TBD ==================================
//=============================================================
TBDdir::TBDdir( string name ) : TBD(name)
{
    char   buf[STR_BUF_LEN];           //!!!!

    getcwd(buf,sizeof(buf));
    if(chdir(name.c_str()) != 0) throw TError("%s: open bd %s error!",MOD_ID,name.c_str());
    chdir(buf);
};

TBDdir::~TBDdir(  )
{
    
}

TTable *TBDdir::TableOpen( const string &nm, bool create )
{
    /*
    vector<string> t_list;
    list(t_list);
    for(unsigned i=0; i < t_list.size(); i++)
	if( t_list[i] == name ) return(table[i].tbl);
    */
    return( new TTableDir(nm,create,this) );
}

void TBDdir::TableDel( const string &table )
{
    if(remove( (char *)(name()+'/'+table).c_str() ) < 0 )
	throw TError("%s: %s",MOD_ID,strerror(errno));
}

//=============================================================
//====================== TTableDir ============================
//=============================================================
TTableDir::TTableDir(string name, bool create, TBD *owner) : TTable(name,owner), codepage("CP866")
{
    n_table = Owner().name()+'/'+name;
    
    m_res = ResAlloc::ResCreate( );
    basa = new TBasaDBF(  );
    if( basa->LoadFile( (char *)n_table.c_str() ) == -1 && !create )
    {
	delete basa;
	throw TError("%s: open table %s error!",MOD_ID,n_table.c_str());
    }
}

TTableDir::~TTableDir(  )
{
    delete basa;
    ResAlloc::ResDelete( m_res );
}

void TTableDir::save( )
{
    ResAlloc res(m_res,true);
    basa->SaveFile((char *)n_table.c_str());
}

string TTableDir::getCellS( int colm, int line)
{
    int i;
    string val;

    ResAlloc res(m_res,false);
    if( basa->GetFieldIt( line, colm, val ) < 0) 
	throw TError("%s: cell error!",MOD_ID);
    res.release();
    for(i = val.size(); i > 0; i--) 
	if(val[i-1]!=' ') break;
    if(i != (int)val.size()) val.resize(i);
	    
    return(Mess->SconvIn(codepage.c_str(),val));
}

double TTableDir::getCellR( int colm, int line)
{
    string val;
    
    ResAlloc res(m_res,false);
    if( basa->GetFieldIt( line, colm, val ) < 0)
	throw TError("%s: cell error!",MOD_ID);
    
    return(atof(val.c_str()));
}

int TTableDir::getCellI( int colm, int line)
{
    string val;
    
    ResAlloc res(m_res,false);
    if( basa->GetFieldIt( line, colm, val ) < 0) 
	throw TError("%s: cell error!",MOD_ID);
    return(atoi(val.c_str()));
}

bool TTableDir::getCellB( int colm, int line)
{
    string val;
    
    ResAlloc res(m_res,false);
    if( basa->GetFieldIt( line, colm, val ) < 0) 
	throw TError("%s: cell error!",MOD_ID);
    if(val.c_str()[0] == 'T')      return(true);
    else if(val.c_str()[0] == 'F') return(false);
    else		           return(false);
}

void TTableDir::setCellS( int colm, int line, const string &cell)
{    
    string t_cell = Mess->SconvOut(codepage,cell);
    ResAlloc res(m_res,true);
    if( basa->ModifiFieldIt( line, colm,(char *)t_cell.c_str() ) < 0 )
	throw TError("%s: cell error!",MOD_ID);
}

void TTableDir::setCellR( int colm, int line, double val)
{
    char str[200];
    db_str_rec *fld_rec;
    
    ResAlloc res(m_res,true);
    if((fld_rec = basa->getField(colm)) == NULL)
	throw TError("%s: cell error!",MOD_ID);
    sprintf(str,"%*.*f",fld_rec->len_fild,fld_rec->dec_field,val);
    if( basa->ModifiFieldIt( line, colm, str ) < 0 ) 
	throw TError("%s: cell error!",MOD_ID);
}

void TTableDir::setCellI( int colm, int line, int val)
{
    char str[200];
    db_str_rec *fld_rec;

    ResAlloc res(m_res,true);
    if((fld_rec = basa->getField(colm)) == NULL)
	throw TError("%s: cell error!",MOD_ID);
    sprintf(str,"%*d",fld_rec->len_fild,val);
    if( basa->ModifiFieldIt( line, colm, str ) < 0 ) 
	throw TError("%s: cell error!",MOD_ID);
}

void TTableDir::setCellB( int colm, int line, bool val)
{
    char str[2];
    db_str_rec *fld_rec;

    ResAlloc res(m_res,true);
    if((fld_rec = basa->getField(colm)) == NULL)
	throw TError("%s: cell error!",MOD_ID);
    if(val == true) str[0] = 'T'; else str[0] = 'F'; str[1] = 0;  
    if( basa->ModifiFieldIt( line, colm, str ) < 0 ) 
	throw TError("%s: cell error!",MOD_ID);
}

int TTableDir::nLines( )
{
    ResAlloc res(m_res,false);
    return( basa->GetCountItems(  ) );
}

int TTableDir::addLine( unsigned int line )
{
    ResAlloc res(m_res,true);
    return( basa->CreateItems(line) );
}

void TTableDir::delLine( unsigned int line )
{
    ResAlloc res(m_res,true);
    if( basa->DeleteItems(line,1) < 0 ) 
	throw TError("%s: line error!",MOD_ID);
}

int TTableDir::nColums(  )
{
    int cnt=0;

    ResAlloc res(m_res,false);
    while( basa->getField(cnt) != NULL ) cnt++;
    return( cnt );
}

int TTableDir::addColum( SColmAttr *colm )
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
    else throw TError("%s: type bd error!",MOD_ID);  
    memset(fld_rec.res,0,14);
    int n_col = nColums();
    
    ResAlloc res(m_res,true);
    int val = basa->addField(n_col,&fld_rec);
    if( val < 0 ) throw TError("%s: column error!",MOD_ID); 
    
    return(val);
}

void TTableDir::delColum( int colm )
{
    ResAlloc res(m_res,true);
    if( basa->DelField( colm ) < 0 ) 
	throw TError("%s: column error!",MOD_ID); 
}

void TTableDir::getColumAttr( int colm, SColmAttr *attr )
{
    db_str_rec *fld_rec;

    ResAlloc res(m_res,false);
    fld_rec = basa->getField(colm);
    res.release();
    if( fld_rec == NULL ) throw TError("%d: column error!",MOD_ID);    
    attr->name = fld_rec->name;
    if(fld_rec->tip_fild == 'C')                                 attr->tp = BD_ROW_STRING;
    else if(fld_rec->tip_fild == 'N' && fld_rec->dec_field == 0) attr->tp = BD_ROW_INT;
    else if(fld_rec->tip_fild == 'N' && fld_rec->dec_field != 0) attr->tp = BD_ROW_REAL;
    else if(fld_rec->tip_fild == 'L')                            attr->tp = BD_ROW_BOOLEAN;
    attr->len  = fld_rec->len_fild;
    attr->dec  = fld_rec->dec_field;
}

void TTableDir::setColumAttr( int colm, SColmAttr *attr )
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
    else throw TError("%s: type bd error!",MOD_ID); 

    ResAlloc res(m_res,true);
    if( basa->setField(colm,&fld_rec) < 0 ) 
	throw TError("%s: column error!",MOD_ID);
}

int TTableDir::columNameToId( const string &colm )
{
    db_str_rec *fld_rec;

    ResAlloc res(m_res,false);
    for(int i=0;(fld_rec = basa->getField(i)) != NULL;i++)
	if( colm == fld_rec->name )
	    return(i);	
    throw TError("%s: column %s no avoid!",MOD_ID,colm.c_str());
    return(-1);
}

string TTableDir::getCodePage( )
{
    ResAlloc res(m_res,false);
    return( codepage );
}

void TTableDir::setCodePage( const string &code )
{
    ResAlloc res(m_res,true);
    codepage=code;
}


