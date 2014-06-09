
//OpenSCADA system module Special.SystemTests file: test_TrOut.h
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef TEST_TROUT_H
#define TEST_TROUT_H

#include <tfunction.h>

namespace KernelTest
{

//*************************************************
//* TestTrOut: Output and/or input transports test.*
//*************************************************
class TestTrOut : public TFunction
{
    public:
	TestTrOut( ) : TFunction("TrOut",SSPC_ID)
	{
	    ioAdd( new IO("rez",_("Result"),IO::String,IO::Return) );
	    ioAdd( new IO("addr",_("Address"),IO::String,IO::Default,"TCP:127.0.0.1:10001") );
	    ioAdd( new IO("type",_("Transport module"),IO::String,IO::Default,"Sockets") );
	    ioAdd( new IO("req",_("Request text"),IO::String,IO::Default|IO::FullText) );
	}

	string name( )	{ return _("Transport"); }
	string descr( )	{ return _("Output and/or input transports test.\n"
				   "Make test for output transport by send the request to selected input transport."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		mod->mess(id(),_("Test: Start"));

		//Get transport param
		string addr = val->getS(1);
		string type = val->getS(2);
		string req = val->getS(3);

		char buf[200];

		int64_t stTm = TSYS::curTime();

		AutoHD<TTipTransport> tr = SYS->transport().at().modAt(type);
		if( !tr.at().outPresent(addr) )
		{
		    tr.at().outAdd(addr);
		    tr.at().outAt(addr).at().setAddr(addr);
		}
		if( !tr.at().outAt(addr).at().startStat() ) tr.at().outAt(addr).at().start();
		int len = tr.at().outAt(addr).at().messIO(req.c_str(),req.size(),buf,sizeof(buf)-1,1000);
		tr.at().outAt(addr).at().stop();
		buf[len] = 0;
		mod->mess(id(),_("%s: Put '%s'. Get: '%s'. Time = %g ms."),addr.c_str(),req.c_str(),buf,(1e-3*(TSYS::curTime()-stTm)));

		mod->mess(id(),_("Test: Passed"));
		val->setS(0,TSYS::strMess(_("Passed. Time = %g ms."),(1e-3*(TSYS::curTime()-stTm))));
	    }
	    catch( TError err )
	    {
		mod->mess(id(),_("Test: Failed: %s"),err.mess.c_str());
		val->setS(0,TSYS::strMess(_("Failed: %s"),err.mess.c_str()));
	    }
	}
};

}

#endif //TEST_TROUT_H
