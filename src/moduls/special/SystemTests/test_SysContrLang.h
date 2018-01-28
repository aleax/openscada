
//OpenSCADA system module Special.SystemTests file: test_xml.h
/***************************************************************************
 *   Copyright (C) 2005-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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
//* TestSysContrLang: The test of the control     *
//*   language of the program.                    *
//*************************************************
class TestSysContrLang : public TFunction
{
    public:
	TestSysContrLang( ) : TFunction("SysContrLang",SSPC_ID)
	{
	    ioAdd( new IO("rez",_("Result"),IO::String,IO::Return) );
	    ioAdd( new IO("path",_("Path to the language element"),IO::String,IO::Default,"/Archive/BaseArh/mess_StatErrors/%2fprm%2fst") );
	}

	string name( )	{ return _("The control language of the program"); }
	string descr( )	{ return _("The test of the control language of the program.\n"
				   "Performs query of language elements through a complete path.\n"
				   "The full path to the language element looks like </Archive/%2fbd%2fm_per>.\n"
				   "The complete path consists of two nested paths.\n"
				   "The first </Archive/> is the path to the node of the control tree.\n"
				   "The second </bd/m_per> is the path to the specific node element."); }

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
