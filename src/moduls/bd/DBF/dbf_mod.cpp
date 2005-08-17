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
#include <resalloc.h>
#include <terror.h>
#include <tmessage.h>
#include "dbf.h"
#include "dbf_mod.h"

//============ Modul info! =====================================================
#define MOD_ID      "DBF"
#define MOD_NAME    "DB DBF"
#define MOD_TYPE    "BD"
#define VER_TYPE    VER_BD
#define VERSION     "1.5.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "BD modul. Allow support of the *.dbf files, version 3.0."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;

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

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	BDDBF::BDMod *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
    	    self_addr = new BDDBF::BDMod( source );       

	return ( self_addr );
    }
}

using namespace BDDBF;

//==============================================================================
//======================= BDDBF::BDMod =========================================
//==============================================================================

BDMod::BDMod( string name ) 
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

BDMod::~BDMod(  )
{

}


TBD *BDMod::openBD( const string &name, bool create )
{
    char   buf[STR_BUF_LEN];           //!!!!

    getcwd(buf,sizeof(buf));
    if(chdir(name.c_str()) != 0)
	if(create == false)
	    throw TError(nodePath().c_str(),"Open bd <%s> error!",name.c_str());
	else if(mkdir(name.c_str(),S_IRWXU|S_IRGRP|S_IROTH) != 0)
	    throw TError(nodePath().c_str(),"Create bd <%s> error!",name.c_str());
    chdir(buf);

    return(new MBD(name,this));
}

void BDMod::delBD( const string &name )
{
    if(rmdir(name.c_str()) != 0)
    	throw TError(nodePath().c_str(),"Delete bd <%s> error!",name.c_str());
}

string BDMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return(buf);
}

void BDMod::modLoad( )
{
    //========== Load parameters from command line ============
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
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1:  break;
	}
    }
    while ( next_opt != -1 );
    
    //========== Load parameters from config file =============
}

//=============================================================
//=================== BDDBF::MBD ==============================
//=============================================================
MBD::MBD( string name, BDMod *iown ) : TBD(name,iown)
{
    char   buf[STR_BUF_LEN];           //!!!!

    getcwd(buf,sizeof(buf));
    if(chdir(name.c_str()) != 0) throw TError(nodePath().c_str(),"Open bd error!");
    chdir(buf);
};

MBD::~MBD(  )
{
    
}

TTable *MBD::openTable( const string &nm, bool create )
{
    return( new MTable(nm,this,create) );
}

void MBD::delTable( const string &table )
{
    if(remove( (char *)(name()+'/'+table).c_str() ) < 0 )
	throw TError(nodePath().c_str(),strerror(errno));
}

//=============================================================
//==================== BDDBF::MTable ==========================
//=============================================================
MTable::MTable(string name, MBD *iown, bool create) : 
    TTable(name,iown), codepage("CP866"), m_modify(false)
{
    n_table = owner().name()+'/'+name;
    
    m_res = ResAlloc::resCreate( );
    basa = new TBasaDBF(  );
    if( basa->LoadFile( (char *)n_table.c_str() ) == -1 && !create )
    {
	delete basa;
	throw TError(nodePath().c_str(),"Open table error!");
    }
}

MTable::~MTable(  )
{
    if(m_modify) save();
    delete basa;
    ResAlloc::resDelete( m_res );   
}

bool MTable::fieldSeek( int i_ln, TConfig &cfg )
{
    int i_clm;
    
    ResAlloc res(m_res,false);
    
    if( i_ln >= basa->GetCountItems( ) )	return false;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);
    
    //Write data to cfg    
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    {
	TCfg &e_cfg = cfg.cfg(cf_el[i_cf]);
	
	//Find collumn
	db_str_rec *fld_rec;
	for(i_clm = 0;(fld_rec = basa->getField(i_clm)) != NULL;i_clm++)
	    if( cf_el[i_cf] == fld_rec->name ) break;
    	if(fld_rec == NULL) continue;
	
	//Get table volume
	string val;
	if( basa->GetFieldIt( i_ln, i_clm, val ) < 0) 
	    throw TError(nodePath().c_str(),"Cell error!");
	
	//Write value
	switch(e_cfg.fld().type())
	{
	    case TFld::String:
	    {
    		//Remove spaces from end
		int i;
    		for(i = val.size(); i > 0; i--) if(val[i-1]!=' ') break;
    		if(i != (int)val.size()) val.resize(i);
	    
    		e_cfg.setS(Mess->SconvIn(codepage.c_str(),val));
		break;
    	    }
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:	
				e_cfg.setI(atoi(val.c_str()));	break;
	    case TFld::Real:    e_cfg.setR(atof(val.c_str()));	break;
	    case TFld::Bool:	e_cfg.setB((val.c_str()[0] == 'T')?true:false);	break;
	}
    }

    return true;
}

void MTable::fieldGet( TConfig &cfg )
{    
    int i_ln, i_clm;

    //Alloc resource
    ResAlloc res(m_res,false);
    
    //Get key line
    i_ln = findKeyLine( cfg );    
    if( i_ln < 0 ) throw TError(nodePath().c_str(),"Field no avoid!");
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);
    
    //Write data to cfg    
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    {
	TCfg &e_cfg = cfg.cfg(cf_el[i_cf]);
	
	//Find collumn
	db_str_rec *fld_rec;
	for(i_clm = 0;(fld_rec = basa->getField(i_clm)) != NULL;i_clm++)
	    if( cf_el[i_cf] == fld_rec->name ) break;
    	if(fld_rec == NULL) continue;
	
	//Get table volume
	string val;
	if( basa->GetFieldIt( i_ln, i_clm, val ) < 0) 
	    throw TError(nodePath().c_str(),"Cell error!");	
	
	//Write value
	switch(e_cfg.fld().type())
	{
	    case TFld::String:
	    {
    		//Remove spaces from end
		int i;
    		for(i = val.size(); i > 0; i--) if(val[i-1]!=' ') break;
    		if(i != (int)val.size()) val.resize(i);
	    
    		e_cfg.setS(Mess->SconvIn(codepage.c_str(),val));
		break;
    	    }
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:
				e_cfg.setI(atoi(val.c_str()));	break;
	    case TFld::Real:    e_cfg.setR(atof(val.c_str()));	break;
	    case TFld::Bool:	e_cfg.setB((val.c_str()[0] == 'T')?true:false);	break;
	}
    }    
}

void MTable::fieldSet( TConfig &cfg )
{
    int i_ln, i_clm;

    //Alloc resource
    ResAlloc res(m_res,true);

    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);
    
    //Check and fix structure of table
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    {
	TCfg &e_cfg = cfg.cfg(cf_el[i_cf]);
	
	//Find collumn
	db_str_rec *fld_rec;
	for(i_clm = 0;(fld_rec = basa->getField(i_clm)) != NULL;i_clm++)
	    if( cf_el[i_cf] == fld_rec->name ) break;
	if(fld_rec == NULL) 
	{
	    //Create new collumn
	    db_str_rec n_rec;
	    
	    fieldPrmSet( e_cfg, n_rec );    
	    if( basa->addField(i_cf,&n_rec) < 0 )
		throw TError(nodePath().c_str(),"Column error!"); 	    
	}
	else
	{
	    //Check collumn parameters
	    switch(e_cfg.fld().type())
	    {
		case TFld::String:
		    if( fld_rec->tip_fild == 'C' && e_cfg.fld().len() == fld_rec->len_fild )	continue;
		    break;		    
		case TFld::Dec:	case TFld::Oct:	case TFld::Hex:	
		    if( fld_rec->tip_fild == 'N' && e_cfg.fld().len() == fld_rec->len_fild )	continue;  
		    break;
		case TFld::Real:    
		    if( fld_rec->tip_fild == 'N' && e_cfg.fld().len() == fld_rec->len_fild &&
			    e_cfg.fld().dec() == fld_rec->dec_field )continue;  
		    break;
		case TFld::Bool:	
		    if( fld_rec->tip_fild == 'L' )	continue;   
		    break;
	    }
	    
	    db_str_rec n_rec;
	    
	    fieldPrmSet( e_cfg, n_rec );
	    if( basa->setField(i_clm,&n_rec) < 0 ) 
		throw TError(nodePath().c_str(),"Column error!");
	}
    }
    //Del no used collumn
    db_str_rec *fld_rec;
    for(i_clm = 0;(fld_rec = basa->getField(i_clm)) != NULL;i_clm++)
    {
	int i_cf;
	for( i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	    if( cf_el[i_cf] == fld_rec->name ) break;
	if( i_cf >= cf_el.size() )
	    if( basa->DelField(i_clm) < 0 ) 
		throw TError(nodePath().c_str(),"Delete field error!");
    }    
    
    //Get key line
    i_ln = findKeyLine( cfg );    
    if( i_ln < 0 ) i_ln = basa->CreateItems(-1);
    
    //Write data to bd    
    for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
    {
	TCfg &e_cfg = cfg.cfg(cf_el[i_cf]);
	
	//Find collumn
	db_str_rec *fld_rec;
	for(i_clm = 0;(fld_rec = basa->getField(i_clm)) != NULL;i_clm++)
	    if( cf_el[i_cf] == fld_rec->name ) break;
	if(fld_rec == NULL) continue;

	//Prepare value
	string val;
       	switch(e_cfg.fld().type())
	{
	    case TFld::String:	val = Mess->SconvOut(codepage,e_cfg.getS());	break;		    
	    case TFld::Dec: case TFld::Oct: case TFld::Hex:
				val = SYS->int2str(e_cfg.getI());break;
	    case TFld::Real:    
	    {
		char str[200];	    
		snprintf(str,sizeof(str),"%*.*f",fld_rec->len_fild,fld_rec->dec_field,e_cfg.getR());	
		val = str;
		break;
	    }
	    case TFld::Bool:	val = (e_cfg.getB() == true)?"T":"F";	break;
	}
	
	//Set table volume
	if( basa->ModifiFieldIt( i_ln, i_clm,(char *)val.c_str() ) < 0 )
	    throw TError(nodePath().c_str(),"Cell error!");	    
    }    
    
    m_modify = true;
}

void MTable::fieldDel( TConfig &cfg )
{
    //Alloc resource
    ResAlloc res(m_res,true);
    
    //Get key line
    int i_ln = findKeyLine( cfg );    
    if( i_ln < 0 ) throw TError(nodePath().c_str(),"Field no present!");
    
    //Delete line
    if( basa->DeleteItems(i_ln,1) < 0 ) 
	throw TError(nodePath().c_str(),"Line error!");
    m_modify = true;
}


int MTable::findKeyLine( TConfig &cfg )
{
    int i_ln, i_clm;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);    

    //Find want field
    for( i_ln = 0; i_ln < basa->GetCountItems(  ); i_ln++ )
    {
	int cnt_key = 0;
	for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	    if( cfg.cfg(cf_el[i_cf]).fld().flg()&FLD_KEY )
	    {
		//string key = cfg.cfg(cf_el[i_cf]).name();
		//Check key
		//Find collumn
		db_str_rec *fld_rec;
		for(i_clm = 0;(fld_rec = basa->getField(i_clm)) != NULL;i_clm++)
		    if( cf_el[i_cf] == fld_rec->name ) break;
		if(fld_rec == NULL) 
		    throw TError(nodePath().c_str(),"Key column <%s> no avoid!",cf_el[i_cf].c_str());
		//Get table volume
		string val;
		if( basa->GetFieldIt( i_ln, i_clm, val ) < 0) 
		    throw TError(nodePath().c_str(),"Cell error!");
		//Remove spaces from end
		int i;
		for(i = val.size(); i > 0; i--) if(val[i-1]!=' ') break;
		if(i != (int)val.size()) val.resize(i);
		//Compare value
		if( val != cfg.cfg(cf_el[i_cf]).getS() )
		{
		    cnt_key = 0;
		    break;
		}
		cnt_key++;
	    }
	if(cnt_key) break;	
    }
    if(i_ln >= basa->GetCountItems(  )) return -1;
    
    return i_ln;
}

void MTable::fieldPrmSet( TCfg &e_cfg, db_str_rec &n_rec )
{
    strncpy(n_rec.name,e_cfg.name().c_str(),11);
    switch(e_cfg.fld().type())
    {
	case TFld::String:
	    n_rec.tip_fild  = 'C';
	    n_rec.len_fild  = e_cfg.fld().len();
	    n_rec.dec_field = 0; 
	    break;		    
	case TFld::Dec: case TFld::Oct:	case TFld::Hex:     
	    n_rec.tip_fild = 'N'; 
	    n_rec.len_fild = (e_cfg.fld().len() == 0)?5:e_cfg.fld().len();
	    n_rec.dec_field = 0; 
	    break;
	case TFld::Real:    
	    n_rec.tip_fild = 'N'; 
	    n_rec.len_fild = (e_cfg.fld().len() == 0)?7:e_cfg.fld().len();
	    n_rec.dec_field = (e_cfg.fld().dec() == 0)?2:e_cfg.fld().dec();
	    break;
	case TFld::Bool:
	    n_rec.tip_fild  = 'L'; 
	    n_rec.len_fild  = 1;
	    n_rec.dec_field = 0;
	    break;
    }
    memset(n_rec.res,0,14);
}


void MTable::save( )
{
    ResAlloc res(m_res,true);
    basa->SaveFile((char *)n_table.c_str());
    m_modify = false;
}
