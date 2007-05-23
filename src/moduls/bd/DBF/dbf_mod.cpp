
//OpenSCADA system module BD.DBF file: dbf_mod.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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
#include <tmess.h>
#include "dbf.h"
#include "dbf_mod.h"

//============ Modul info! =====================================================
#define MOD_ID      "DBF"
#define MOD_NAME    "DB DBF"
#define MOD_TYPE    "BD"
#define VER_TYPE    VER_BD
#define VERSION     "1.8.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "BD modul. Allow support of the *.dbf files, version 3.0."
#define LICENSE     "GPL"
//==============================================================================

BDDBF::BDMod *BDDBF::mod;

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
    	    self_addr = BDDBF::mod = new BDDBF::BDMod( source );

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
    mVers      	= VERSION; 
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION; 
    mLicense   	= LICENSE; 
    mSource    	= name;
}

BDMod::~BDMod(  )
{

}

TBD *BDMod::openBD( const string &iid )
{
    return new MBD(iid,&owner().openDB_E());
}

string BDMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
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
MBD::MBD( string iid, TElem *cf_el ) : TBD(iid,cf_el)
{

}

MBD::~MBD(  )
{
    
}

void MBD::postDisable(int flag)
{
    TBD::postDisable(flag);
    
    if( flag && owner().fullDeleteDB() )
        if(rmdir(addr().c_str()) != 0)
    	    mess_warning(nodePath().c_str(),_("Delete DB error!"));
}

void MBD::enable( )
{
    char   buf[STR_BUF_LEN];
    
    char *rez = getcwd(buf,sizeof(buf));
    if(chdir(addr().c_str()) != 0 && (!create() || mkdir(addr().c_str(),S_IRWXU|S_IRGRP|S_IROTH) != 0))
        throw TError(nodePath().c_str(),_("Error create DB directory <%s>!"),addr().c_str());
    if( rez && chdir(buf) ) throw TError(nodePath().c_str(),_("Restore previous directory as curent is error."));

    TBD::enable( );
}

TTable *MBD::openTable( const string &nm, bool create )
{    
    if( !enableStat() )
	throw TError(nodePath().c_str(),_("Error open table <%s>. DB disabled."),nm.c_str());
    return new MTable(nm,this,create);
}

//=============================================================
//==================== BDDBF::MTable ==========================
//=============================================================
MTable::MTable(const string &inm, MBD *iown, bool create) : 
    TTable(inm), m_modify(false)
{
    string tbl_nm = name();
    nodePrev(iown);

    //Set file extend
    if( !(tbl_nm.size() > 4 && tbl_nm.substr(tbl_nm.size()-4,4) == ".dbf") )
	tbl_nm=tbl_nm+".dbf";

    codepage = owner().codePage().size()?owner().codePage():Mess->charset();
    n_table = owner().addr()+'/'+tbl_nm;
    
    m_res = ResAlloc::resCreate( );
    basa = new TBasaDBF(  );
    if( basa->LoadFile( (char *)n_table.c_str() ) == -1 && !create )
    {
	delete basa;
	throw TError(nodePath().c_str(),_("Open table error!"));
    }
}

MTable::~MTable(  )
{
    delete basa;
    ResAlloc::resDelete( m_res );   
}

void MTable::postDisable(int flag)
{
    if(m_modify) save();

    if( flag )
    {
	string n_tbl = name();
	//Set file extend
	if( !(n_tbl.size() > 4 && n_tbl.substr(n_tbl.size()-4,4) == ".dbf") )
	    n_tbl=n_tbl+".dbf";
	
	if(remove((owner().addr()+"/"+n_tbl).c_str()) < 0 )
	    mess_err(nodePath().c_str(),"%s",strerror(errno));
    }
}

bool MTable::fieldSeek( int i_ln, TConfig &cfg )
{
    int i_clm;
    
    ResAlloc res(m_res,false);
    
    i_ln = findKeyLine(cfg,i_ln);    
    if( i_ln < 0 ) return false;
    
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
	    if( cf_el[i_cf].substr(0,10) == fld_rec->name ) break;
    	if(fld_rec == NULL) continue;
	
	//Get table volume
	string val;
	if( basa->GetFieldIt( i_ln, i_clm, val ) < 0) 
	    throw TError(nodePath().c_str(),_("Cell error!"));
	
	//Write value
	switch(e_cfg.fld().type())
	{
	    case TFld::String:
	    {
    		//Remove spaces from end
		int i;
    		for(i = val.size(); i > 0; i--) if(val[i-1]!=' ') break;
    		if(i != (int)val.size()) val.resize(i);
	    
    		e_cfg.setS(Mess->codeConvIn(codepage.c_str(),val));
		break;
    	    }
	    case TFld::Integer:	e_cfg.setI(atoi(val.c_str()));	break;
	    case TFld::Real:    e_cfg.setR(atof(val.c_str()));	break;
	    case TFld::Boolean:	e_cfg.setB((val.c_str()[0] == 'T')?true:false);	break;
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
    if( i_ln < 0 ) throw TError(nodePath().c_str(),_("Field no avoid!"));
    
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
	    if( cf_el[i_cf].substr(0,10) == fld_rec->name ) break;
    	if(fld_rec == NULL) continue;
	
	//Get table volume
	string val;
	if( basa->GetFieldIt( i_ln, i_clm, val ) < 0) 
	    throw TError(nodePath().c_str(),_("Cell error!"));	
	
	//Write value
	switch(e_cfg.fld().type())
	{
	    case TFld::String:
	    {
    		//Remove spaces from end
		int i;
    		for(i = val.size(); i > 0; i--) if(val[i-1]!=' ') break;
    		if(i != (int)val.size()) val.resize(i);
	    
    		e_cfg.setS(Mess->codeConvIn(codepage.c_str(),val));
		break;
    	    }
	    case TFld::Integer:	e_cfg.setI(atoi(val.c_str()));	break;
	    case TFld::Real:    e_cfg.setR(atof(val.c_str()));	break;
	    case TFld::Boolean:	e_cfg.setB((val.c_str()[0] == 'T')?true:false);	break;
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
	    if( cf_el[i_cf].substr(0,10) == fld_rec->name ) break;
	if(fld_rec == NULL) 
	{
	    //Create new collumn
	    db_str_rec n_rec;
	    
	    fieldPrmSet( e_cfg, n_rec );    
	    if( basa->addField(i_cf,&n_rec) < 0 )
		throw TError(nodePath().c_str(),_("Column error!")); 	    
	}
	else
	{
	    //Check collumn parameters
	    switch(e_cfg.fld().type())
	    {
		case TFld::String:
		    if( fld_rec->tip_fild == 'C' && e_cfg.fld().len() == fld_rec->len_fild )	continue;
		    break;		    
		case TFld::Integer:	
		    if( fld_rec->tip_fild == 'N' && e_cfg.fld().len() == fld_rec->len_fild )	continue;  
		    break;
		case TFld::Real:    
		    if( fld_rec->tip_fild == 'N' && e_cfg.fld().len() == fld_rec->len_fild &&
			    e_cfg.fld().dec() == fld_rec->dec_field )continue;  
		    break;
		case TFld::Boolean:	
		    if( fld_rec->tip_fild == 'L' )	continue;   
		    break;
	    }
	    
	    db_str_rec n_rec;
	    
	    fieldPrmSet( e_cfg, n_rec );
	    if( basa->setField(i_clm,&n_rec) < 0 ) 
		throw TError(nodePath().c_str(),_("Column error!"));
	}
    }
    //Del no used collumn
    db_str_rec *fld_rec;
    for(i_clm = 0;(fld_rec = basa->getField(i_clm)) != NULL;i_clm++)
    {
	int i_cf;
	for( i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	    if( cf_el[i_cf].substr(0,10) == fld_rec->name ) break;
	if( i_cf >= cf_el.size() )
	    if( basa->DelField(i_clm) < 0 ) 
		throw TError(nodePath().c_str(),_("Delete field error!"));
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
	    if( cf_el[i_cf].substr(0,10) == fld_rec->name ) break;
	if(fld_rec == NULL) continue;

	//Prepare value	
	string val;
       	switch(e_cfg.fld().type())
	{
	    case TFld::String:	val = Mess->codeConvOut(codepage,e_cfg.getS());	break;		    
	    case TFld::Integer:	val = SYS->int2str(e_cfg.getI());break;
	    case TFld::Real:    
	    {
		char str[200];	    
		snprintf(str,sizeof(str),"%*.*f",fld_rec->len_fild,fld_rec->dec_field,e_cfg.getR());	
		val = str;
		break;
	    }
	    case TFld::Boolean:	val = (e_cfg.getB() == true)?"T":"F";	break;
	}
	
	//Set table volume
	if( basa->ModifiFieldIt( i_ln, i_clm,val.c_str() ) < 0 )
	    throw TError(nodePath().c_str(),_("Cell error!"));	    
    }    
    
    m_modify = true;
}

void MTable::fieldDel( TConfig &cfg )
{
    //Alloc resource
    ResAlloc res(m_res,true);
    
    //Get key line
    bool i_ok = false;
    int i_ln;     
    while((i_ln = findKeyLine(cfg)) >= 0)
    {
	if( basa->DeleteItems(i_ln,1) < 0 )
	    throw TError(nodePath().c_str(),_("Line error!"));
		
	i_ok = true;
	m_modify = true;
    }    
    if( !i_ok ) throw TError(nodePath().c_str(),_("Field no present!"));
}

int MTable::findKeyLine( TConfig &cfg, int cnt )
{
    int i_ln, i_clm, i_cnt = 0;
    
    //Get config fields list
    vector<string> cf_el;
    cfg.cfgList(cf_el);    

    //Find want field
    for( i_ln = 0; i_ln < basa->GetCountItems(  ); i_ln++ )
    {
	int cnt_key = 0;
	for( int i_cf = 0; i_cf < cf_el.size(); i_cf++ )
	    if( cfg.cfg(cf_el[i_cf]).fld().flg()&TCfg::Key )
	    {
		if( !cfg.cfg(cf_el[i_cf]).getS().size() )
		{
		    cnt_key++;
		    continue;		
		}
		//string key = cfg.cfg(cf_el[i_cf]).name();
		//Check key
		//Find collumn
		db_str_rec *fld_rec;
		for(i_clm = 0;(fld_rec = basa->getField(i_clm)) != NULL;i_clm++)
		    if( cf_el[i_cf].substr(0,10) == fld_rec->name ) break;
		if(fld_rec == NULL) 
		    throw TError(nodePath().c_str(),_("Key column <%s> no avoid!"),cf_el[i_cf].c_str());
		//Get table volume
		string val;
		if( basa->GetFieldIt( i_ln, i_clm, val ) < 0) 
		    throw TError(nodePath().c_str(),_("Cell error!"));
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
	if(cnt_key && cnt <= i_cnt++) break;	
    }
    if(i_ln >= basa->GetCountItems(  )) return -1;
    
    return i_ln;
}

void MTable::fieldPrmSet( TCfg &e_cfg, db_str_rec &n_rec )
{
    memset(&n_rec,0,sizeof(db_str_rec));
    
    strncpy(n_rec.name,e_cfg.name().c_str(),10);
    switch(e_cfg.fld().type())
    {
	case TFld::String:
	    n_rec.tip_fild  = 'C';
	    n_rec.len_fild  = (e_cfg.fld().len()>255)?255:e_cfg.fld().len();
	    n_rec.dec_field = 0; 
	    break;		    
	case TFld::Integer:     
	    n_rec.tip_fild = 'N'; 
	    n_rec.len_fild = (e_cfg.fld().len() == 0)?5:(e_cfg.fld().len()>255)?255:e_cfg.fld().len();
	    n_rec.dec_field = 0; 
	    break;
	case TFld::Real:    
	    n_rec.tip_fild = 'N'; 
	    n_rec.len_fild = (e_cfg.fld().len() == 0)?7:(e_cfg.fld().len()>255)?255:e_cfg.fld().len();
	    n_rec.dec_field = (e_cfg.fld().dec() == 0)?2:(e_cfg.fld().dec()>255)?255:e_cfg.fld().dec();
	    break;
	case TFld::Boolean:
	    n_rec.tip_fild  = 'L'; 
	    n_rec.len_fild  = 1;
	    n_rec.dec_field = 0;
	    break;
    }
}

void MTable::save( )
{
    ResAlloc res(m_res,true);
    basa->SaveFile((char *)n_table.c_str());
    m_modify = false;
}
