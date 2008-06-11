
//OpenSCADA system module Archive.DBArch file: arch.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#include <sys/stat.h>
#include <signal.h>
#include <getopt.h>
#include <string>

#include <tsys.h>
#include <resalloc.h>
#include <tmess.h>

#include "arch.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"DBArch"
#define MOD_NAME	"To DB archivator"
#define MOD_TYPE	"Archive"
#define VER_TYPE	VER_ARH
#define VERSION		"0.6.0"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"The Archive module. Allow functions for messages and values arhiving to DB."
#define LICENSE		"GPL"
//*************************************************

DBArch::ModArch *DBArch::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 ) 	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) ) 
	    return new DBArch::ModArch( source );
	return NULL;
    }
}

using namespace DBArch;

//*************************************************
//* DBArch::ModArch                               *
//*************************************************
ModArch::ModArch( const string &name)
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;
}

void ModArch::postEnable( int flag )
{
    TModule::postEnable( flag );

    if( flag&TCntrNode::NodeConnect )
    {
	//- Add self DB-fields for messages archive -
	owner().messE().fldAdd( new TFld("DBArchSize",_("Archive size (hours)"),TFld::Real,TFld::NoFlag,"20.5","24") );

	//- Add self DB-fields for value archive -
	owner().valE().fldAdd( new TFld("DBArchSize",_("Archive size (hours)"),TFld::Real,TFld::NoFlag,"20.5","24") );

	//- Archivators info table DB structure -
	el_arch.fldAdd( new TFld("TBL",_("Table"),TFld::String,TCfg::Key,"50") );
	el_arch.fldAdd( new TFld("BEGIN",_("Begin"),TFld::String,TFld::NoFlag,"20") );
	el_arch.fldAdd( new TFld("END",_("End"),TFld::String,TFld::NoFlag,"20") );
	el_arch.fldAdd( new TFld("PRM1",_("Parameter 1"),TFld::String,TFld::NoFlag,"20") );
	el_arch.fldAdd( new TFld("PRM2",_("Parameter 2"),TFld::String,TFld::NoFlag,"20") );
	el_arch.fldAdd( new TFld("PRM3",_("Parameter 3"),TFld::String,TFld::NoFlag,"20") );

	//- Message DB archive DB structure -
	el_mess.fldAdd( new TFld("TM",_("Time (c)"),TFld::Integer,TCfg::Key,"10") );
	el_mess.fldAdd( new TFld("CATEG",_("Category"),TFld::String,TCfg::Key,"100") );
	el_mess.fldAdd( new TFld("MESS",_("Message"),TFld::String,TCfg::Key,"255") );
	el_mess.fldAdd( new TFld("LEV",_("Level"),TFld::Integer,TFld::NoFlag,"1") );

	//- Boolean and integer value DB archive DB structure -
	el_vl_int.fldAdd( new TFld("TM",_("Time (c)"),TFld::Integer,TCfg::Key,"10") );
	el_vl_int.fldAdd( new TFld("TMU",_("Time (uc)"),TFld::Integer,TCfg::Key,"10") );
	el_vl_int.fldAdd( new TFld("VAL",_("Value"),TFld::Integer,TFld::NoFlag,"10") );

	//- Real value DB archive DB structure -
	el_vl_real.fldAdd( new TFld("TM",_("Time (c)"),TFld::Integer,TCfg::Key,"10") );
	el_vl_real.fldAdd( new TFld("TMU",_("Time (uc)"),TFld::Integer,TCfg::Key,"10") );
	el_vl_real.fldAdd( new TFld("VAL",_("Value"),TFld::Real,TFld::NoFlag,"30.15") );

	//- String value DB archive DB structure -
	el_vl_str.fldAdd( new TFld("TM",_("Time (c)"),TFld::Integer,TCfg::Key,"10") );
	el_vl_str.fldAdd( new TFld("TMU",_("Time (uc)"),TFld::Integer,TCfg::Key,"10") );
	el_vl_str.fldAdd( new TFld("VAL",_("Value"),TFld::String,TFld::NoFlag,"255") );
    }
}

ModArch::~ModArch()
{
    try{ modStop(); }catch(...){}
}

string ModArch::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void ModArch::load_()
{
    //- Load parameters from command line -
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void ModArch::modStart( )
{

}

void ModArch::modStop( )
{

}

TMArchivator *ModArch::AMess(const string &iid, const string &idb)
{
    return new ModMArch(iid,idb,&owner().messE());
}


TVArchivator *ModArch::AVal(const string &iid, const string &idb)
{
    return new ModVArch(iid,idb,&owner().valE());
}

void ModArch::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TTipArchivator::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) ) opt->setText(optDescr());
    else TTipArchivator::cntrCmdProc(opt);
}
