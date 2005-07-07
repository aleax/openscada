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

#include <unistd.h>
#include <getopt.h>
#include <string>
#include <errno.h>

#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include <tmodule.h>
#include "mail.h"

//============ Modul info! =====================================================
#define MOD_ID      "mail"
#define MOD_NAME    "Mail transport"
#define MOD_TYPE    "Transport"
#define VER_TYPE    VER_TR
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Transport based to mail protocols: IMAP, POP, SMTP"
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
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= name;
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
	MOD_TYPE,MOD_ID,MOD_ID);

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

TTransportIn *TTransMail::In( const string &name )
{
    TMailIn *sock = new TMailIn(name,this);
    return(sock);
}

TTransportOut *TTransMail::Out( const string &name )
{
    return(new TMailOut(name,this));
}

//==============================================================================
//== TMailIn =================================================================
//==============================================================================

TMailIn::TMailIn(string name, TTipTransport *owner) 
    : TTransportIn(name,owner)
{
    
}

TMailIn::~TMailIn()
{
    
}

//==============================================================================
//== TMailOut ================================================================
//==============================================================================

TMailOut::TMailOut( string name, TTipTransport *owner ) : TTransportOut(name,owner)
{
    
}

TMailOut::~TMailOut()
{
    
}

