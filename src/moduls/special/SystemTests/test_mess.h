
//OpenSCADA system module Special.SystemTests file: test_mess.h
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

#ifndef TEST_MESS_H
#define TEST_MESS_H

#include <tfunction.h>

namespace KernelTest
{

//*************************************************
//* TestMess: Messages archive test.              *
//*************************************************
class TestMess : public TFunction
{
    public:
	TestMess( ) : TFunction("Mess",SSPC_ID)
	{
	    ioAdd( new IO("rez",_("Result"),IO::String,IO::Return) );
	    ioAdd( new IO("arhtor",_("Archivator"),IO::String,IO::Default,"FSArch.StatErrors") );
	    ioAdd( new IO("categ",_("Messages category pattern"),IO::String,IO::Default) );
	    ioAdd( new IO("depth",_("Messages depth (s)"),IO::Integer,IO::Default,"10") );
	}

	string name( )	{ return _("Messages"); }
	string descr( )	{ return _("Messages archive test. Periodic read new messages from archive, for selected archivator."); }

	void calc( TValFunc *val )
	{
	    try
	    {
		mod->mess(id(),_("Test: Start"));

		char c_tm[26];
		string n_arhtor = val->getS(1);
		vector<TMess::SRec> buf_rec;
		SYS->archive().at().messGet(time(NULL)-2*val->getI(3),time(NULL),buf_rec,val->getS(2),TMess::Debug,n_arhtor);
		mod->mess(id(),_("New messages present %d."),buf_rec.size());
		for(unsigned i_rec = 0; i_rec < buf_rec.size(); i_rec++)
		{
		    ctime_r(&buf_rec[i_rec].time, c_tm);
		    mod->mess(id(),"'%s' : '%s' : '%s'",TSYS::strParse(c_tm,0,"\n").c_str(),
			buf_rec[i_rec].categ.c_str(),buf_rec[i_rec].mess.c_str());
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

#endif //TEST_MESS_H
