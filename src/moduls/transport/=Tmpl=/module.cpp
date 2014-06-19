
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module Transport.Tmpl file: module.cpp
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily                                 *
 *   my@email.org                                                          *
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

//!!! System's includings. Add need for your module includings.
#include <unistd.h>
#include <string>
#include <errno.h>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tsys.h>
#include <tmess.h>
#include <tprotocols.h>
#include <tmodule.h>

//!!! Self your module's includings. Add need for your module includings.
#include "module.h"

//!!! Module's meta-information. Change for your module.
//************************************************
//* Modul info!                                  *
#define MOD_ID		"Tmpl"
#define MOD_NAME	_("Transport template")
#define MOD_TYPE	STR_ID
#define VER_TYPE	STR_VER
#define MOD_VER		"0.0.1"
#define AUTHORS		_("MyName MyFamily")
#define DESCRIPTION	_("Transport's subsystem template module.")
#define LICENSE		"MyLicense"
//************************************************

ModTmpl::TTr *ModTmpl::mod;

//!!! Required section for binding OpenSCADA kernel's to this module. Gives information and create module's root object.
//!!! Not remove this section!
extern "C"
{
#ifdef MOD_INCL
    TModule::SAt tr_Tmpl_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if( n_mod==0 ) return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *tr_Tmpl_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new ModTmpl::TTr( source );
	return NULL;
    }
}

//!!! Include for default call into your module's namespace.
using namespace ModTmpl;

//************************************************
//* TTr						 *
//************************************************
//!!! Constructor for module's root object. Append into for your need.
TTr::TTr( string name ) : TTipTransport(MOD_ID)
{
    //!!! Init shortcut to module's root object. No change it.
    mod		= this;

    //!!! Loading module's meta-information into root object. No change it.
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;
}

//!!! Destructor for module's root object. Append into for your need.
TTr::~TTr( )
{

}

//!!! Module's comandline options the print help function. Add your module commandline parameters info.
string TTr::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

//!!! Inherited (virtual) load object's node method. Append your module need data loadings
void TTr::load_( )
{
    //!!! Load self module command line parameters' values. Append your addition parameters process.
    //> Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
        if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //!!! Load addition your module specific data. For example, make loading addition module's parameters from OpenSCADA system DB or from main config-file
}

//!!! Main subsystem API functions for self modules input and output transport objects creation. Change only your class names.
TTransportIn *TTr::In( const string &name, const string &idb )
{
    return new TTrIn(name,idb,&owner().inEl());
}

TTransportOut *TTr::Out( const string &name, const string &idb )
{
    return new TTrOut(name,idb,&owner().outEl());
}

//!!! OpenSCADA control interface comands process virtual function.
//!!! For example, process access from standard configurators of OpenSCADA to individual module's parameters.
//!!! Modify for self needs
void TTr::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TTipTransport::cntrCmdProc(opt);
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    TTipTransport::cntrCmdProc(opt);
}

//************************************************
//* TTrIn                                        *
//************************************************
//!!! Constructor for input transport object. Append into for your need.
TTrIn::TTrIn( string name, const string &idb, TElem *el ) : TTransportIn(name,idb,el)
{

}

//!!! Destructor for input transport object. Append into for your need.
TTrIn::~TTrIn()
{

}

//!!! The inherited (virtual) status interface function. Append into for your need.
string TTrIn::getStatus( )
{
    string rez = TTransportIn::getStatus( );

    if( startStat() )
	rez += TSYS::strMess(_("Connections %d, opened %d. Traffic in %.4g kb, out %.4g kb."),connNumb,cl_id.size(),trIn,trOut);

    return rez;
}

//!!! The inherited (virtual) start and stop interface functions. Append into for your need.
void TTrIn::start()
{
    if( run_st ) return;

    //> Status clear
    trIn = trOut = 0;
    connNumb = 0;

    //!!! Your code

    SYS->taskCreate(nodePath('.',true), 0, Task, this);
}

void TTrIn::stop()
{
    if( !run_st ) return;

    //> Status clear
    trIn = trOut = 0;
    connNumb = 0;

    SYS->taskDestroy(nodePath('.',true), &endrun);

    //!!! Your code
}

//!!! Thread's function for process input connections. Append into for your need.
void *TTrIn::Task( void *tr_in )
{
    TTrIn *tr = (TTrIn *)tr_in;
    AutoHD<TProtocolIn> prot_in;

    tr->run_st	= true;
    tr->endrun	= false;

    while( !tr->endrun )
    {
	//!!! Your code
    }

    tr->run_st = false;

    return NULL;
}

//!!! OpenSCADA control interface comands process virtual function.
//!!! For example, process access from standard confifurators of OpenSCADA to individual module's parameters.
//!!! Modify for self needs
void TTrIn::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TTransportIn::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",STR_ID,1,"help",
	    _("Socket's input transport has address format:\n"
	    "  TCP:[addr]:[port]:[mode] - TCP socket:\n"
	    "    addr - address for socket to be opened, empty address opens socket for all interfaces;\n"
	    "    port - network port (/etc/services);\n"
	    "    mode - work mode (0 - break connection; 1 - keep alive).\n"
	    "  UDP:[addr]:[port] - UDP socket:\n"
	    "    addr - address for socket to be opened, empty address opens socket for all interfaces;\n"
	    "    port - network port (/etc/services).\n"
	    "  UNIX:[name]:[mode] - UNIX socket:\n"
	    "    name - UNIX-socket's file name;\n"
	    "    mode - work mode (0 - break connection; 1 - keep alive)."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/q_ln",_("Queue length"),RWRW__,"root",STR_ID,2,"tp","dec","help",_("Used for TCP and UNIX sockets."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/cl_n",_("Clients maximum"),RWRW__,"root",STR_ID,2,"tp","dec","help",_("Used for TCP and UNIX sockets."));
	ctrMkNode("fld",opt,-1,"/prm/cfg/bf_ln",_("Input buffer (kbyte)"),RWRW__,"root",STR_ID,1,"tp","dec");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/q_ln")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(TSYS::int2str(maxQueue()));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setMaxQueue(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/cl_n")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(TSYS::int2str(maxFork()));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setMaxFork(atoi(opt->text().c_str()));
    }
    else if(a_path == "/prm/cfg/bf_ln")
    {
	if(ctrChkNode(opt,"get",RWRW__,"root",STR_ID,SEC_RD))	opt->setText(TSYS::int2str(bufLen()));
	if(ctrChkNode(opt,"set",RWRW__,"root",STR_ID,SEC_WR))	setBufLen(atoi(opt->text().c_str()));
    }
    else TTransportIn::cntrCmdProc(opt);
}

//************************************************
//* TTrOut                                   *
//************************************************
//!!! Constructor for output transport object. Append into for your need.
TTrOut::TTrOut(string name, const string &idb, TElem *el) : TTransportOut(name,idb,el)
{

}

//!!! Destructor for output transport object. Append into for your need.
TTrOut::~TTrOut()
{

}

//!!! The inherited (virtual) status interface function. Append into for your need.
string TTrOut::getStatus( )
{
    string rez = TTransportOut::getStatus( );

    if( startStat() )	rez += TSYS::strMess(_("Traffic in %.4g kb, out %.4g kb."),trIn,trOut);

    return rez;
}

//!!! The inherited (virtual) start and stop interface functions. Append into for your need.
void TTrOut::start()
{
    if( run_st ) return;

    //> Status clear
    trIn = trOut = 0;

    //!!! Your code

    run_st = true;
}

void TTrOut::stop()
{
    if( !run_st ) return;

    //> Status clear
    trIn = trOut = 0;

    //!!! Your code

    run_st = false;
}

//!!! The inherited (virtual) sending data and receiving interface function implementation. Append into for your need.
int TTrOut::messIO( const char *obuf, int len_ob, char *ibuf, int len_ib, int time, bool noRes )
{
    if( !run_st ) throw TError(nodePath().c_str(),_("Transport is not started!"));

    //!!! Your code

    return 0;	//Recived messages size
}

//!!! OpenSCADA control interface comands process virtual function.
//!!! For example, process access from standard confifurators of OpenSCADA to individual module's parameters.
//!!! Modify for self needs
void TTrOut::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TTransportOut::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/cfg/ADDR",EVAL_STR,RWRWR_,"root",STR_ID,1,"help",
	    _("Socket's output transport has address format:\n"
	    "  TCP:[addr]:[port] - TCP socket:\n"
	    "    addr - address for remote socket to be opened;\n"
	    "    port - network port (/etc/services);\n"
	    "  UDP:[addr]:[port] - UDP socket:\n"
	    "    addr - address for remote socket to be opened;\n"
	    "    port - network port (/etc/services).\n"
	    "  UNIX:[name] - UNIX socket:\n"
	    "    name - UNIX-socket's file name."));
	return;
    }

    //> Process command to page
    TTransportOut::cntrCmdProc(opt);
}
