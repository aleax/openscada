
//OpenSCADA system module Transport.Mail file: mail.cpp
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

#include <unistd.h>
#include <getopt.h>
#include <string>
#include <errno.h>

#include <tsys.h>
#include <tmess.h>
#include <tmodule.h>
#include "mail.h"

//============ Modul info! =====================================================
#define MOD_ID      "Mail"
#define MOD_NAME    "Mail"
#define MOD_TYPE    "Transport"
#define VER_TYPE    VER_TR
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow mail based transport."
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
	TTransMail *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new TTransMail( source );       

	return ( self_addr );
    }
}
//==============================================================================
//== TTransMail ================================================================
//==============================================================================

TTransMail::TTransMail( string name ) 
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

TTransMail::~TTransMail()
{

}

string TTransMail::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return(buf);
}

void TTransMail::modLoad( )
{
    //========== Load parameters from command line ============
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
    
    //========== Load parameters from config file =============
}

TTransportIn *TTransMail::In( const string &name, const string &idb )
{
    return new TMailIn(name,idb,&owner().inEl());
}

TTransportOut *TTransMail::Out( const string &name, const string &idb )
{
    return new TMailOut(name,idb,&owner().outEl());
}

//==============================================================================
//== TMailIn =================================================================
//==============================================================================

TMailIn::TMailIn(string name, const string &idb, TElem *el ) : 
    TTransportIn(name,idb,el)
{
    
}

TMailIn::~TMailIn()
{
    
}

//==============================================================================
//== TMailOut ================================================================
//==============================================================================

TMailOut::TMailOut( string name, const string &idb, TElem *el ) : 
    TTransportOut(name,idb,el)
{
    
}

TMailOut::~TMailOut()
{
    
}

