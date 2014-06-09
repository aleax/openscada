
//OpenSCADA system module Special.SystemTests file: test_Base64Code.h
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

#ifndef TEST_BASE64CODE_H
#define TEST_BASE64CODE_H

#include <tfunction.h>

namespace KernelTest
{

//*************************************************
//* TestBase64Code: Mime Base64 algorithm tests.  *
//*************************************************
class TestBase64Code : public TFunction
{
    public:
	TestBase64Code( ) : TFunction("Base64Code",SSPC_ID)
	{
	    ioAdd( new IO("rez",_("Result"),IO::String,IO::Return) );
	}

	string name( )	{ return _("Base64 encoding"); }
	string descr( )	{ return _("Mime Base64 encoding algorithm tests."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		mod->mess(id(),_("Test: Start"));

		string inbuf, outbuf;
		for(int i_s = 0; i_s < 256; i_s++) inbuf.push_back((unsigned char)i_s);

		mod->mess(id(),_("Test1: Encoding."));
		int64_t st_cnt = TSYS::curTime();
		outbuf = TSYS::strEncode(inbuf,TSYS::base64);
		mod->mess(id(),_("Encoded text by length %d for time %f ms."),inbuf.size(),1e-3*(TSYS::curTime()-st_cnt));
		mod->mess(id(),TSYS::strMess(_("Encoded text: %s"),outbuf.c_str()).c_str());
		mod->mess(id(),_("Test1: Passed."));

		mod->mess(id(),_("Test2: Decoding."));
		st_cnt = TSYS::curTime();
		inbuf = TSYS::strDecode(outbuf,TSYS::base64);
		mod->mess(id(),_("Decoded text by length %d for time %f ms."),outbuf.size(),1e-3*(TSYS::curTime()-st_cnt));
		for(int i_s = 0; i_s < 256; i_s++) 
		    if((unsigned char)inbuf[i_s] != i_s) 
			throw TError(nodePath().c_str(),_("Test2: Failed. Decoding error."));
		mod->mess(id(),_("Test2: Passed."));

		mod->mess(id(),_("Test: Passed"));
		val->setS(0,_("Passed"));
	    }
	    catch( TError err )
	    {
		mod->mess(id(),_("Test: Failed: %s"),err.mess.c_str());
		val->setS(0,TSYS::strMess(_("Failed: %s"),err.mess.c_str()));
	    }
	}
};

}

#endif //TEST_BASE64CODE_H
