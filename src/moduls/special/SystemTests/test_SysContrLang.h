
//OpenSCADA system module Special.SystemTests file: test_xml.h
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

#ifndef TEST_SYSCONTRLANG_H
#define TEST_SYSCONTRLANG_H

#include <tfunction.h>

namespace KernelTest
{

//*************************************************
//* TestSysContrLang: System control language test.*
//*************************************************
class TestSysContrLang : public TFunction
{
    public:
	TestSysContrLang( ) : TFunction("SysContrLang",SSPC_ID)
	{
	    ioAdd( new IO("rez",_("Result"),IO::String,IO::Return) );
	    ioAdd( new IO("path",_("Path to language element"),IO::String,IO::Default,"/Archive/BaseArh/mess_StatErrors/%2fprm%2fst") );
	}

	string name( )	{ return _("System control language"); }
	string descr( )	{ return _("System control language test.\n"
				   "Make request to language elements by full path set.\n"
				   "Full path to language element have view </Archive/%2fbd%2fm_per>.\n"
				   "Full path contained two included path.\n"
				   "First </d_Archive/> is path to the node of the control tree.\n"
				   "Second </bd/m_per> is path to concrete node's element."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		mod->mess(id(),_("Test: Start"));

		string path = val->getS(1);
		mod->mess(id(),_("SysContr test for path '%s'."),path.c_str());

		XMLNode node("info");
		node.setAttr("path",path)->setAttr("user","root");
		SYS->cntrCmd(&node);
		mod->prXMLNode(id(), &node);

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

#endif //TEST_SYSCONTRLANG_H
