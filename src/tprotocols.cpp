
//OpenSCADA system file: tprotocols.cpp
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
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

#include <getopt.h>

#include "tsys.h"
#include "tmess.h"
#include "tprotocols.h"

//************************************************
//* TProtocolS                                   *
//************************************************
TProtocolS::TProtocolS( ) : TSubSYS("Protocol","Transport protocols",true)
{

}

TProtocolS::~TProtocolS( )
{

}

string TProtocolS::optDescr(  )
{
    return(_(
	"=============== Subsystem \"Transport protocols\" options =================\n\n"));
}

void TProtocolS::load_()
{
    //> Load parameters from command line
    int next_opt;
    const char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"      ,0,NULL,'h'},
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,TSYS::strEncode(optDescr(),TSYS::FormatPrint).c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //> Load parameters from config file

}

void TProtocolS::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TSubSYS::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root",subId().c_str(),3,"tp","str","cols","90","rows","10");
	return;
    }
    //> Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440,"root",subId().c_str()) )	opt->setText(optDescr());
    else TSubSYS::cntrCmdProc(opt);
}


//************************************************
//* TProtocol                                    *
//************************************************
TProtocol::TProtocol()
{
    m_pr = grpAdd("pr_");
}

TProtocol::~TProtocol()
{

}

void TProtocol::open( const string &name, const string &tr )
{
    if( chldPresent(m_pr,name) ) return;
    chldAdd(m_pr,in_open(name));
    at(name).at().setSrcTr(tr);
}

void TProtocol::close( const string &name )
{
    chldDel(m_pr,name);
}

//************************************************
//* TProtocolIn                                  *
//************************************************
TProtocolIn::TProtocolIn( const string &name ) : mName(name)
{
    modifClr( );
}

TProtocolIn::~TProtocolIn()
{

}

TProtocol &TProtocolIn::owner( )	{ return *(TProtocol*)nodePrev(); }
