
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

#ifndef TEST_XML_H
#define TEST_XML_H

#include <tfunction.h>

namespace KernelTest
{

//*************************************************
//* TestXML: XML file parsing test.               *
//*************************************************
class TestXML : public TFunction
{
    public:
	TestXML( ) : TFunction("XML",SSPC_ID)
	{
	    ioAdd( new IO("rez",_("Result"),IO::String,IO::Return) );
	    ioAdd( new IO("file",_("XML file"),IO::String,IO::Default) );
	}

	string name( )	{ return _("XML parsing"); }
	string descr( )	{ return _("XML file parsing test. Parse and show selected file structure."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		mod->mess(id(),_("Test: Start"));

		int hd = open(val->getS(1).c_str(),O_RDONLY);
		if( hd < 0 ) throw TError(nodePath().c_str(),_("Open file '%s' error."),val->getS(1).c_str());
		int cf_sz = lseek(hd,0,SEEK_END);
		lseek(hd,0,SEEK_SET);
		char *buf = (char *)malloc(cf_sz);
		read(hd,buf,cf_sz);
		close(hd);
		string s_buf(buf,cf_sz);
		free(buf);
		XMLNode node;
		node.load(s_buf);
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

#endif //TEST_XML_H
