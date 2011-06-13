
//OpenSCADA system module Special.SystemTests file: test_val.h
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

#ifndef TEST_VAL_H
#define TEST_VAL_H

#include <tfunction.h>

namespace KernelTest
{

//*************************************************
//* TestVal: Parameter attribute's value test.    *
//*************************************************
class TestVal : public TFunction
{
    public:
	TestVal( ) : TFunction("Val",SSPC_ID)
	{
	    ioAdd( new IO("rez",_("Result"),IO::String,IO::Return) );
	    ioAdd( new IO("name",_("Parameter attribute path"),IO::String,IO::Default,"System.AutoDA.CPULoad.load") );
	    ioAdd( new IO("arch_len",_("Archive value getting depth (s)"),IO::Integer,IO::Default,"10") );
	    ioAdd( new IO("arch_per",_("Archive value getting period (us)"),IO::Integer,IO::Default,"1000000") );
	}

	string name( )	{ return _("Parameter attribute"); }
	string descr( )	{ return _("Parameter attribute's value test.\nPeriodic make gathering for last value of selected attribute,"
				    " and also gathering from archive for selected depth."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		mod->mess(id(),_("Test: Start"));

		string s_prm = val->getS(1);
		int a_len = val->getI(2);
		int a_per = val->getI(3);

		mod->mess(id(),_("Value of: %s."),s_prm.c_str());

		AutoHD<TVal> aval = SYS->daq().at().at(TSYS::strSepParse(s_prm,0,'.')).at().
						    at(TSYS::strSepParse(s_prm,1,'.')).at().
						    at(TSYS::strSepParse(s_prm,2,'.')).at().
						    vlAt(TSYS::strSepParse(s_prm,3,'.'));
		mod->mess(id(),_("Last value = %s"), aval.at().getS(NULL).c_str() );
		if( a_len && a_per )
		{
		    int64_t cur = TSYS::curTime()-a_per;
		    for( int i_v = 1; i_v <= a_len; i_v++, cur-=a_per )
		    {
			int64_t rvtm = cur;
			mod->mess(id(),_("Value %d = %s."),i_v, aval.at().getS(&rvtm).c_str() );  
		    }
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

#endif //TEST_VAL_H
