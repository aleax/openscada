
//OpenSCADA system module Special.SystemTests file: test_mess.h
/***************************************************************************
 *   Copyright (C) 2005-2010 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#ifndef TEST_SOATTACH_H
#define TEST_SOATTACH_H

#include <tfunction.h>

namespace KernelTest
{

//*************************************************
//* TestSOAttach: Attach/Detach module test.      *
//*************************************************
class TestSOAttach : public TFunction
{
    public:
	TestSOAttach( ) : TFunction("SOAttach",SSPC_ID)
	{
	    ioAdd( new IO("rez",_("Result"),IO::String,IO::Return) );
	    ioAdd( new IO("name",_("Path to module"),IO::String,IO::Default) );
	    ioAdd( new IO("mode",_("Mode (1-attach;-1-detach;0-change)"),IO::Integer,IO::Default,"0") );
	    ioAdd( new IO("full",_("Full attach(to start)"),IO::Boolean,IO::Default,"1") );
	}

	string name( )	{ return _("Attach SO"); }
	string descr( )	{ return _("Attach/detach module test."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		mod->mess(id(),_("Test: Start"));

		SYS->modSchedul();
		string SO_name = val->getS(1);
		TModSchedul::SHD so_st = SYS->modSchedul().at().lib(SO_name);
		if(val->getI(2) > 0)		SYS->modSchedul().at().libAtt(so_st.name, val->getB(3));
		else if(val->getI(2) < 0)	SYS->modSchedul().at().libDet(so_st.name);
		else
		{
		    if(so_st.hd) SYS->modSchedul().at().libDet(so_st.name);
		    else SYS->modSchedul().at().libAtt(so_st.name, val->getB(3));
		}

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

#endif //TEST_SOATTACH_H
