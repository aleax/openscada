
//OpenSCADA system module Special.SystemTests file: test_prm.h
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

#ifndef TEST_PRM_H
#define TEST_PRM_H

#include <tfunction.h>

namespace KernelTest
{

//*************************************************
//* TestPrm: DAQ parameters test                  *
//*************************************************
class TestPrm : public TFunction
{
    public:
	TestPrm( ) : TFunction("Param",SSPC_ID)
	{
	    ioAdd( new IO("rez",_("Result"),IO::String,IO::Return) );
	    ioAdd( new IO("name",_("DAQ parameter address"),IO::String,IO::Default,"System.AutoDA.CPULoad") );
	}

	string name( )	{ return _("Parameter"); }
	string descr( )	{ return _("DAQ parameters test. Make read a parameter's attributes and configuration fields."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		string paddr = val->getS(1);
		AutoHD<TParamContr> prm = SYS->daq().at().at(TSYS::strSepParse(paddr,0,'.')).at().
							  at(TSYS::strSepParse(paddr,1,'.')).at().
							  at(TSYS::strSepParse(paddr,2,'.'));

		mod->mess(id(),_("Test: Start"));

		vector<string> list_el;
		prm.at().vlList(list_el);
		mod->mess(id(),_("Value attributes present: %d"),list_el.size());
		for(unsigned i = 0; i < list_el.size(); i++)
		{
		    AutoHD<TVal> val = prm.at().vlAt(list_el[i]);
		    if( val.at().fld().flg()&TFld::Selected )
			mod->mess(id(),_("%s(SELECT): %s"),list_el[i].c_str(), val.at().getSEL().c_str() );
		    switch( val.at().fld().type() )
		    {
			case TFld::String:
			    mod->mess(id(),_("%s(STRING): %s"),list_el[i].c_str(), val.at().getS().c_str() );
			    break;
			case TFld::Integer:
			    mod->mess(id(),_("%s(INTEGER): %d"),list_el[i].c_str(), val.at().getI() );
			    break;
			case TFld::Real:
			    mod->mess(id(),_("%s(REAL): %f"),list_el[i].c_str(), val.at().getR() );
			    break;
			case TFld::Boolean:
			    mod->mess(id(),_("%s(BOOLEAN): %d"),list_el[i].c_str(), val.at().getB() );
			    break;
			default: break;
		    }
		}

		prm.at().cfgList(list_el);
		mod->mess(id(),_("Configuration fields: %d"),list_el.size());
		for(unsigned i = 0; i < list_el.size(); i++)
		{
		    if( prm.at().cfg(list_el[i]).fld().flg()&TFld::Selected )
			mod->mess(id(),_("%s(SELECT): %s"),list_el[i].c_str(), prm.at().cfg(list_el[i]).getSEL().c_str() );
		    switch(prm.at().cfg(list_el[i]).fld().type())
		    {
			case TFld::String:
			    mod->mess(id(),_("%s(STRING): %s"),list_el[i].c_str(), prm.at().cfg(list_el[i]).getS().c_str() );
			    break;
			case TFld::Integer:
			    mod->mess(id(),_("%s(INTEGER): %d"),list_el[i].c_str(), prm.at().cfg(list_el[i]).getI() );
			    break;
			case TFld::Real:
			    mod->mess(id(),_("%s(REAL): %f"),list_el[i].c_str(), prm.at().cfg(list_el[i]).getR() );
			    break;
			case TFld::Boolean:
			    mod->mess(id(),_("%s(BOOLEAN): %d"),list_el[i].c_str(), prm.at().cfg(list_el[i]).getB() );
			    break;
			default: break;
		    }
		}
		/*XMLNode node("info");
		node.attr("path","")->attr("user","root");
		prm.at().cntrCmd(&node);
		mod->prXMLNode(TTest::cat()+name(), &node);*/
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

#endif //TEST_PRM_H
