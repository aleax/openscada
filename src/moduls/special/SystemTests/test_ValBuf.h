
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

#ifndef TEST_VALBUF_H
#define TEST_VALBUF_H

#include <tfunction.h>

namespace KernelTest
{

//*************************************************
//* TestValBuf: Value buffer tests.               *
//*************************************************
class TestValBuf : public TFunction
{
    public:
	TestValBuf( ) : TFunction("ValBuf",SSPC_ID)
	{
	    ioAdd( new IO("rez",_("Result"),IO::String,IO::Return) );
	}

	string name( )	{ return _("Value buffer"); }
	string descr( )	{ return _("Value buffer tests.\nContain 13 tests for all aspects of value buffer (subsystem \"Archives\")."); }

	void calc( TValFunc *val )
	{
	    int64_t rtm, wtm;
	    uint64_t st_cnt;
	    TValBuf *buf = NULL;
	    string test;

	    try
	    {
		mod->mess(id(),_("Test: Start"));

		//--------------------------- Test 1 ----------------------------------
		test = TSYS::strMess(_("Test%d"),1);
		mod->mess(id(),_("%s: Create buffer: Data = string, Size = 10, Period = 1s, HardGrid = yes, HighRes = no."),test.c_str());
		buf = new TValBuf( TFld::String, 10, 1000000, true, false );
		if( buf->valType() == TFld::String && buf->size() == 10 && buf->period() == 1000000 && buf->hardGrid() && !buf->highResTm() )
		    mod->mess(id(),_("%s: Passed."),test.c_str());
		else throw TError(nodePath().c_str(),_("Create buffer error."));

		//--------------------------- Test 2 ----------------------------------
		test = TSYS::strMess(_("Test%d"),2);
		mod->mess(id(),_("%s: Change buffer mode."),test.c_str());
		buf->setHardGrid( false );
		if( buf->hardGrid() == false )	mod->mess(id(),_("Disable hard grid ok."));
		else throw TError(nodePath().c_str(),_("Disable hard grid failed."));
		buf->setHighResTm(true);
		if( buf->highResTm() == true )	mod->mess(id(),_("Set high resolution time ok."));
		else throw TError(nodePath().c_str(),_("Set high resolution time failed."));
		buf->setSize( 500 );
		buf->setSize( 2000 );
		if( buf->size() == 2000 )	mod->mess(id(),_("Change buffer size ok."));
		else throw TError(nodePath().c_str(),_("Change buffer size failed."));
		buf->setPeriod(0);
		if( buf->period() == 0 )	mod->mess(id(),_("Change period ok."));
		else throw TError(nodePath().c_str(),_("Change period failed."));
		mod->mess(id(),_("%s: Passed."),test.c_str());

		//--------------------------- Test 3 ----------------------------------
		test = TSYS::strMess(_("Test%d"),3);
		mod->mess(id(),_("%s: Destroy buffer."),test.c_str());
		delete buf; buf = NULL;
		mod->mess(id(),_("%s: Passed."),test.c_str());

		//--------------------------- Test 4 ----------------------------------
		test = TSYS::strMess(_("Test%d"),4);
		mod->mess(id(),_("%s: Fill and check hard time grid string buffer."),test.c_str());
		buf = new TValBuf( TFld::String, 10, 100000, true, true );
		wtm = buf->period()*(TSYS::curTime()/buf->period());
		//Not full fill
		for(int i=0; i<buf->size()/2; i++)
		    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
		if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at filling half buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period() )
		{
		    rtm = i;
		    if(buf->getS(&rtm) != "Test: "+TSYS::int2str((i-wtm)/buf->period()))
			throw TError(nodePath().c_str(),_("Write a half buffer values error."));
		}
		mod->mess(id(),_("Write a half buffer ok."));
		//Roll buff
		for(int i=buf->size()/2; i<buf->size()+5; i++)
		    switch(i)
		    {
			//Pass one value
			case 7: continue;
			//Drift values
			case 9: buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()+buf->period()/2); break;
			case 11:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()-buf->period()/2); break;
			default:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
		    }
		if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+5))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at roll filling buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{
		    rtm = i;
		    if( !(((i-wtm)/buf->period() == 7 && buf->getS(&rtm) == EVAL_STR) ||
			    ((i-wtm)/buf->period() == 10 && buf->getS(&rtm) == "Test: 11" ) ||
			    ((i-wtm)/buf->period() == 11 && buf->getS(&rtm) == EVAL_STR) ||
			    buf->getS(&rtm) == "Test: "+TSYS::int2str((i-wtm)/buf->period())) )
			throw TError(nodePath().c_str(),_("Write a roll buffer values error."));
		}
		mod->mess(id(),_("Write a roll buffer ok."));
		rtm = buf->end();
		buf->setS("Test up.",rtm);
		if(buf->getS(&rtm) != "Test up.")
		    throw TError(nodePath().c_str(),_("Update buffer end error."));
		mod->mess(id(),_("Update buffer end ok."));

		wtm += buf->period()*(buf->size()+5);
		st_cnt = TSYS::curTime();
		buf->setSize(1000);
		for(int i=0; i<buf->size(); i++)
		    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
		mod->mess(id(),_("Wrote 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		st_cnt = TSYS::curTime();
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{ rtm = i; buf->getS(&rtm); }
		mod->mess(id(),_("Read 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		mod->mess(id(),_("%s: Passed."),test.c_str());
		delete buf; buf = NULL;

		//--------------------------- Test 5 ----------------------------------
		test = TSYS::strMess(_("Test%d"),5);
		mod->mess(id(),_("%s: Fill and check hard time grid integer buffer."),test.c_str());
		buf = new TValBuf(TFld::Integer, 10, 100000, true, true);
		wtm = buf->period()*(TSYS::curTime()/buf->period());
		//Not full fill
		for(int i=0; i<buf->size()/2; i++)
		    buf->setI(i,wtm+i*buf->period());
		if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at filling half buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{
		    rtm = i;
		    if(buf->getI(&rtm) != (i-wtm)/buf->period())
			throw TError(nodePath().c_str(),_("Write a half buffer values error."));
		}
		mod->mess(id(),_("Write a half buffer ok."));
		//Roll buff
		for(int i=buf->size()/2; i<buf->size()+5; i++)
		    switch(i)
		    {
			//Pass one value
			case 7: continue;
			//Drift values
			case 9: buf->setI(i,wtm+i*buf->period()+buf->period()/2); break;
			case 11:buf->setI(i,wtm+i*buf->period()-buf->period()/2); break;
			default:buf->setI(i,wtm+i*buf->period());
		    }
		if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+5))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at roll filling buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{
		    rtm = i;
		    if( !(((i-wtm)/buf->period() == 7 && buf->getI(&rtm) == EVAL_INT) ||
			    ((i-wtm)/buf->period() == 10 && buf->getI(&rtm) == 11 ) ||
			    ((i-wtm)/buf->period() == 11 && buf->getI(&rtm) == EVAL_INT) ||
			    buf->getI(&rtm) == (i-wtm)/buf->period()) )
			throw TError(nodePath().c_str(),_("Write a roll buffer values error."));
		}
		mod->mess(id(),_("Write a roll buffer ok."));
		rtm = buf->end();
		buf->setI(100,rtm);
		if(buf->getI(&rtm) != 100)
		    throw TError(nodePath().c_str(),_("Update buffer end error."));
		mod->mess(id(),_("Update buffer end ok."));

		wtm += buf->period()*(buf->size()+5);
		st_cnt = TSYS::curTime();
		buf->setSize(1000);
		for(int i=0; i<buf->size(); i++)
		    buf->setI(i,wtm+i*buf->period());
		mod->mess(id(),_("Wrote 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		st_cnt = TSYS::curTime();
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{ rtm = i; buf->getI(&rtm); }
		mod->mess(id(),_("Read 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		mod->mess(id(),_("%s: Passed."),test.c_str());
		delete buf; buf = NULL;

		//--------------------------- Test 6 ----------------------------------
		test = TSYS::strMess(_("Test%d"),6);
		mod->mess(id(),_("%s: Fill and check soft time grid string buffer (high time)."),test.c_str());
		buf = new TValBuf( TFld::String, 10, 100000, false, true );
		wtm = buf->period()*(TSYS::curTime()/buf->period());
		//Not full fill
		for(int i=0; i<buf->size()/2; i++)
		    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
		if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at filling half buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period() )
		{
		    rtm = i;
		    if(buf->getS(&rtm) != "Test: "+TSYS::int2str((i-wtm)/buf->period()))
			throw TError(nodePath().c_str(),_("Write a half buffer values error."));
		}
		mod->mess(id(),_("Write a half buffer ok."));
		//Roll buff
		for(int i=buf->size()/2; i<buf->size()+5; i++)
		    switch(i)
		    {
			//Pass one value
			case 7: continue;
			//Drift values
			case 9: buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()+buf->period()/2); break;
			case 11:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()-buf->period()/2); break;
			case 13:buf->setS("Test: "+TSYS::int2str(12),wtm+i*buf->period()); break;
			default:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
		    }
		if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+4))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at roll filling buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{
		    rtm = i;
		    if( !(((i-wtm)/buf->period() == 7 && buf->getS(&rtm) == EVAL_STR) ||
			    ((i-wtm)/buf->period() == 10 && buf->getS(&rtm) == "Test: 11" ) ||
			    ((i-wtm)/buf->period() == 11 && buf->getS(&rtm) == EVAL_STR) ||
			    ((i-wtm)/buf->period() == 13 && buf->getS(&rtm) == "Test: 12") ||
			    buf->getS(&rtm) == "Test: "+TSYS::int2str((i-wtm)/buf->period())) )
			throw TError(nodePath().c_str(),_("Write a roll buffer values error."));
		}
		mod->mess(id(),_("Write a roll buffer ok."));
		rtm = buf->end();
		buf->setS("Test up.",rtm);
		if(buf->getS(&rtm) != "Test up.")
		    throw TError(nodePath().c_str(),_("Update buffer end error."));
		mod->mess(id(),_("Update buffer end ok."));

		wtm += buf->period()*(buf->size()+5);
		st_cnt = TSYS::curTime();
		buf->setSize(1000);
		for(int i=0; i<buf->size(); i++)
		    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
		mod->mess(id(),_("Wrote 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		st_cnt = TSYS::curTime();
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{ rtm = i; buf->getS(&rtm); }
		mod->mess(id(),_("Read 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		mod->mess(id(),_("%s: Passed."),test.c_str());
		delete buf; buf = NULL;

		//--------------------------- Test 7 ----------------------------------
		test = TSYS::strMess(_("Test%d"),7);
		mod->mess(id(),_("%s: Fill and check soft time grid integer buffer (high time)."),test.c_str());
		buf = new TValBuf( TFld::Integer, 10, 100000, false, true );
		wtm = buf->period()*(TSYS::curTime()/buf->period());
		//Not full fill
		for(int i=0; i<buf->size()/2; i++)
		    buf->setI(i,wtm+i*buf->period());
		if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at filling half buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period() )
		{
		    rtm = i;
		    if(buf->getI(&rtm) != (i-wtm)/buf->period())
			throw TError(nodePath().c_str(),_("Write a half buffer values error."));
		}
		mod->mess(id(),_("Write a half buffer ok."));
		//Roll buff
		for(int i=buf->size()/2; i<buf->size()+5; i++)
		    switch(i)
		    {
			//Pass one value
			case 7: continue;
			//Drift values
			case 9: buf->setI(i,wtm+i*buf->period()+buf->period()/2); break;
			case 11:buf->setI(i,wtm+i*buf->period()-buf->period()/2); break;
			case 13:buf->setI(12,wtm+i*buf->period()); break;
			default:buf->setI(i,wtm+i*buf->period());
		    }
		if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+4))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at roll filling buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{
		    rtm = i;
		    if( !(((i-wtm)/buf->period() == 7 && buf->getI(&rtm) == EVAL_INT) ||
			    ((i-wtm)/buf->period() == 10 && buf->getI(&rtm) == 11 ) ||
			    ((i-wtm)/buf->period() == 11 && buf->getI(&rtm) == EVAL_INT) ||
			    ((i-wtm)/buf->period() == 13 && buf->getI(&rtm) == 12) ||
			    buf->getI(&rtm) == (i-wtm)/buf->period()) )
			throw TError(nodePath().c_str(),_("Write a roll buffer values error."));
		}
		mod->mess(id(),_("Write a roll buffer ok."));
		rtm = buf->end();
		buf->setI(1000,rtm);
		if(buf->getI(&rtm) != 1000)
		    throw TError(nodePath().c_str(),_("Update buffer end error."));
		mod->mess(id(),_("Update buffer end ok."));

		wtm += buf->period()*(buf->size()+5);
		st_cnt = TSYS::curTime();
		buf->setSize(1000);
		for(int i=0; i<buf->size(); i++)
		    buf->setI(i,wtm+i*buf->period());
		mod->mess(id(),_("Wrote 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		st_cnt = TSYS::curTime();
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{ rtm = i; buf->getI(&rtm); }
		mod->mess(id(),_("Read 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		mod->mess(id(),_("%s: Passed."),test.c_str());
		delete buf; buf = NULL;

		//--------------------------- Test 8 ----------------------------------
		test = TSYS::strMess(_("Test%d"),8);
		mod->mess(id(),_("%s: Fill and check soft time grid string buffer (low time)."),test.c_str());
		buf = new TValBuf( TFld::String, 10, 1000000, false, false );
		wtm = buf->period()*(TSYS::curTime()/buf->period());
		//Not full fill
		for(int i=0; i<buf->size()/2; i++)
		    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
		if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at filling half buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period() )
		{
		    rtm = i;
		    if(buf->getS(&rtm) != "Test: "+TSYS::int2str((i-wtm)/buf->period()))
			throw TError(nodePath().c_str(),_("Write a half buffer values error."));
		}
		mod->mess(id(),_("Write a half buffer ok."));
		//Roll buff
		for(int i=buf->size()/2; i<buf->size()+5; i++)
		    switch(i)
		    {
			//Pass one value
			case 7: continue;
			//Drift values
			case 9: buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()+buf->period()/2); break;
			case 11:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period()-buf->period()/2); break;
			case 13:buf->setS("Test: "+TSYS::int2str(12),wtm+i*buf->period()); break;
			default:buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
		    }
		if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+4))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at roll filling buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{
		    rtm = i;
		    if( !(((i-wtm)/buf->period() == 7 && buf->getS(&rtm) == EVAL_STR) ||
			    ((i-wtm)/buf->period() == 10 && buf->getS(&rtm) == "Test: 11") ||
			    ((i-wtm)/buf->period() == 11 && buf->getS(&rtm) == EVAL_STR) ||
			    ((i-wtm)/buf->period() == 13 && buf->getS(&rtm) == "Test: 12") ||
			    buf->getS(&rtm) == "Test: "+TSYS::int2str((i-wtm)/buf->period())) )
			throw TError(nodePath().c_str(),_("Write a roll buffer values error.") );
		}
		mod->mess(id(),_("Write a roll buffer ok."));
		rtm = buf->end();
		buf->setS("Test up.",rtm);
		if(buf->getS(&rtm) != "Test up.") throw TError(nodePath().c_str(),_("Update buffer end error."));
		mod->mess(id(),_("Update buffer end ok."));

		wtm += buf->period()*(buf->size()+5);
		st_cnt = TSYS::curTime();
		buf->setSize(1000);
		for(int i=0; i<buf->size(); i++)
		    buf->setS("Test: "+TSYS::int2str(i),wtm+i*buf->period());
		mod->mess(id(),_("Wrote 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		st_cnt = TSYS::curTime();
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{ rtm = i; buf->getS(&rtm); }
		mod->mess(id(),_("Read 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		mod->mess(id(),_("%s: Passed."),test.c_str());
		delete buf; buf = NULL;

		//--------------------------- Test 9 ----------------------------------
		test = TSYS::strMess(_("Test%d"),9);
		mod->mess(id(),_("%s: Fill and check soft time grid integer buffer (low time)."),test.c_str());
		buf = new TValBuf( TFld::Integer, 10, 1000000, false, false );
		wtm = buf->period()*(TSYS::curTime()/buf->period());
		//Not full fill
		for(int i=0; i<buf->size()/2; i++)
		    buf->setI(i,wtm+i*buf->period());
		if(!(buf->end()/buf->period() == wtm/buf->period()+(buf->size()/2)-1 && buf->begin()/buf->period() == wtm/buf->period()))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at filling half buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period() )
		{
		    rtm = i;
		    if(buf->getI(&rtm) != (i-wtm)/buf->period()) throw TError(nodePath().c_str(),_("Write a half buffer values error."));
		}
		mod->mess(id(),_("Write a half buffer ok."));
		//Roll buff
		for(int i=buf->size()/2; i<buf->size()+5; i++)
		    switch(i)
		    {
			//Pass one value
			case 7: continue;
			//Drift values
			case 9: buf->setI(i,wtm+i*buf->period()+buf->period()/2); break;
			case 11:buf->setI(i,wtm+i*buf->period()-buf->period()/2); break;
			case 13:buf->setI(12,wtm+i*buf->period()); break;
			default:buf->setI(i,wtm+i*buf->period());
		    }
		if(!(buf->end()/buf->period() == wtm/buf->period()+buf->size()+4 && buf->begin()/buf->period() == wtm/buf->period()+4))
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at roll filling buffer."));
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{
		    rtm = i;
		    if( !(((i-wtm)/buf->period() == 7 && buf->getI(&rtm) == EVAL_INT) ||
			    ((i-wtm)/buf->period() == 10 && buf->getI(&rtm) == 11) ||
			    ((i-wtm)/buf->period() == 11 && buf->getI(&rtm) == EVAL_INT) ||
			    ((i-wtm)/buf->period() == 13 && buf->getI(&rtm) == 12) ||
			    buf->getI(&rtm) == (i-wtm)/buf->period()) )
			throw TError(nodePath().c_str(),_("Write a roll buffer values error."));
		}
		mod->mess(id(),_("Write a roll buffer ok."));
		rtm = buf->end();
		buf->setI(1000,rtm);
		if(buf->getI(&rtm) != 1000) throw TError(nodePath().c_str(),_("Update buffer end error."));
		mod->mess(id(),_("Update buffer end ok."));

		wtm += buf->period()*(buf->size()+5);
		st_cnt = TSYS::curTime();
		buf->setSize(1000);
		for(int i=0; i<buf->size(); i++)
		    buf->setI(i,wtm+i*buf->period());
		mod->mess(id(),_("Wrote 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		st_cnt = TSYS::curTime();
		for(int64_t i = buf->begin(); i <= buf->end(); i+=buf->period())
		{ rtm = i; buf->getI(&rtm); }
		mod->mess(id(),_("Read 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		mod->mess(id(),_("%s: Passed."),test.c_str());
		delete buf; buf = NULL;

		//--------------------------- Test 10 ----------------------------------
		test = TSYS::strMess(_("Test%d"),10);
		mod->mess(id(),_("%s: Fill and check free time string buffer (high time)."),test.c_str());
		buf = new TValBuf( TFld::String, 10, 0, false, true );
		wtm = TSYS::curTime();
		int wper = 100000;
		//Not full fill
		for(int i=0; i<buf->size()/2; i++)
		    buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i);
		if(!(buf->end() == wtm+wper*((buf->size()/2)-1) && buf->begin() == wtm) )
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at filling half buffer."));
		int icnt = buf->size()/2 - 1;
		for(int64_t i = buf->end(); i >= buf->begin(); i--, icnt-- )
		    if(buf->getS(&i) != "Test: "+TSYS::int2str(icnt))
			throw TError(nodePath().c_str(),_("Write a half buffer values error."));
		mod->mess(id(),_("Write a half buffer ok."));
		//Roll buff
		for(int i=buf->size()/2; i<buf->size()+5; i++)
		    switch(i)
		    {
			//Pass one value
			case 7: continue;
			//Drift values
			case 9: buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i+wper/2);	break;
			case 11:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i-wper/2);	break;
			case 13:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*(i-1));	break;
			default:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i);
		    }
		if(!(buf->end() == wtm+wper*(buf->size()+4) && buf->begin() == wtm+3*wper) )
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at roll filling buffer."));
		icnt = buf->size()+4;
		for(int64_t i = buf->end(); i >= buf->begin(); i--, icnt--)
		{
		    //string val =  buf->getS(&rtm);
		    //printf("Value %lld: %s\n",rtm,val.c_str());
		    if(icnt == 12 || icnt == 7)	icnt--;
		    if( buf->getS(&i) != "Test: "+TSYS::int2str(icnt) )
			throw TError(nodePath().c_str(),_("Write a roll buffer values error."));
		}
		mod->mess(id(),_("Write a roll buffer ok."));
		rtm = buf->end();
		buf->setS("Test up.",rtm);
		if(buf->getS(&rtm) != "Test up.") throw TError(nodePath().c_str(),_("Update buffer end error."));
		mod->mess(id(),_("Update buffer end ok."));

		wtm += wper*(buf->size()+5);
		st_cnt = TSYS::curTime();
		buf->setSize(1000);
		for(int i=0; i<buf->size(); i++)
		    buf->setS("Test: "+TSYS::int2str(i),wtm+i*wper);
		mod->mess(id(),_("Wrote 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		st_cnt = TSYS::curTime();
		for(int64_t i = buf->end(); i > buf->begin(); i=rtm-1)
		{ rtm = i; buf->getS(&rtm); }
		mod->mess(id(),_("Read 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		mod->mess(id(),_("%s: Passed."),test.c_str());
		delete buf; buf = NULL;

		//--------------------------- Test 11 ----------------------------------
		test = TSYS::strMess(_("Test%d"),11);
		mod->mess(id(),_("%s: Fill and check free time integer buffer (high time)."),test.c_str());
		buf = new TValBuf( TFld::Integer, 10, 0, false, true );
		wtm = TSYS::curTime();
		wper = 100000;
		//Not full fill
		for(int i=0; i<buf->size()/2; i++)
		    buf->setI(i,wtm+wper*i);
		if(!(buf->end() == wtm+wper*((buf->size()/2)-1) && buf->begin() == wtm) )
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at filling half buffer."));
		icnt = buf->size()/2 - 1;
		for(int64_t i = buf->end(); i >= buf->begin(); i--, icnt-- )
		    if(buf->getI(&i) != icnt) throw TError(nodePath().c_str(),_("Write a half buffer values error."));
		mod->mess(id(),_("Write a half buffer ok."));
		//Roll buff
		for(int i=buf->size()/2; i<buf->size()+5; i++)
		    switch(i)
		    {
			//Pass one value
			case 7: continue;
			//Drift values
			case 9: buf->setI(i,wtm+wper*i+wper/2); break;
			case 11:buf->setI(i,wtm+wper*i-wper/2); break;
			case 13:buf->setI(i,wtm+wper*(i-1)); 	break;
			default:buf->setI(i,wtm+wper*i);
		    }
		if(!(buf->end() == wtm+wper*(buf->size()+4) && buf->begin() == wtm+3*wper) )
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at roll filling buffer."));
		icnt = buf->size()+4;
		for(int64_t i = buf->end(); i >= buf->begin(); i--, icnt--)
		{
		    if(icnt == 12 || icnt == 7)	icnt--;
		    if( buf->getI(&i) != icnt ) throw TError(nodePath().c_str(),_("Write a roll buffer values error."));
		}
		mod->mess(id(),_("Write a roll buffer ok."));
		rtm = buf->end();
		buf->setI(1000,rtm);
		if(buf->getI(&rtm) != 1000) throw TError(nodePath().c_str(),_("Update buffer end error."));
		mod->mess(id(),_("Update buffer end ok."));

		wtm += wper*(buf->size()+5);
		st_cnt = TSYS::curTime();
		buf->setSize(1000);
		for(int i=0; i<buf->size(); i++)
		    buf->setI(i,wtm+i*wper);
		mod->mess(id(),_("Wrote 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		st_cnt = TSYS::curTime();
		for(int64_t i = buf->end(); i > buf->begin(); i=rtm-1)
		{ rtm = i; buf->getI(&rtm); }
		mod->mess(id(),_("Read 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		mod->mess(id(),_("%s: Passed."),test.c_str());
		delete buf; buf = NULL;

		//--------------------------- Test 12 ----------------------------------
		test = TSYS::strMess(_("Test%d"),12);
		mod->mess(id(),_("%s: Fill and check free time string buffer (low time)."),test.c_str());
		buf = new TValBuf( TFld::String, 10, 0, false, false );
		wper = 1000000;
		wtm = wper*(TSYS::curTime()/wper);
		//Not full fill
		for(int i=0; i<buf->size()/2; i++)
		    buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i);
		if(!(buf->end() == wtm+wper*((buf->size()/2)-1) && buf->begin() == wtm) )
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at filling half buffer."));
		icnt = buf->size()/2 - 1;
		for(int64_t i = buf->end(); i >= buf->begin(); i--, icnt-- )
		    if(buf->getS(&i) != "Test: "+TSYS::int2str(icnt))
			throw TError(nodePath().c_str(),_("Write a half buffer values error."));
		mod->mess(id(),_("Write a half buffer ok."));
		//Roll buff
		for(int i=buf->size()/2; i<buf->size()+5; i++)
		    switch(i)
		    {
			//Pass one value
			case 7: continue;
			//Drift values
			case 9: buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i+wper/2);	break;
			case 11:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i-wper/2);	break;
			case 13:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*(i-1));	break;
			default:buf->setS("Test: "+TSYS::int2str(i),wtm+wper*i);
		    }
		if(!(buf->end() == wtm+wper*(buf->size()+4) && buf->begin() == wtm+2*wper) )
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at roll filling buffer."));
		icnt = buf->size()+4;
		for(int64_t i = buf->end(); i >= buf->begin(); i--, icnt--)
		{
		    if(icnt == 12 || icnt == 10 || icnt == 7)	icnt--;
		    if( buf->getS(&i) != "Test: "+TSYS::int2str(icnt) )
			throw TError(nodePath().c_str(),_("Write a roll buffer values error."));
		}
		mod->mess(id(),_("Write a roll buffer ok."));
		rtm = buf->end();
		buf->setS("Test up.",rtm);
		if(buf->getS(&rtm) != "Test up.") throw TError(nodePath().c_str(),_("Update buffer end error."));
		mod->mess(id(),_("Update buffer end ok."));

		wtm += wper*(buf->size()+5);
		st_cnt = TSYS::curTime();
		buf->setSize(1000);
		for(int i=0; i<buf->size(); i++)
		    buf->setS("Test: "+TSYS::int2str(i),wtm+i*wper);
		mod->mess(id(),_("Wrote 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		st_cnt = TSYS::curTime();
		for(int64_t i = buf->end(); i > buf->begin(); i=rtm-1)
		{ rtm = i; buf->getS(&rtm); }
		mod->mess(id(),_("Read 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		mod->mess(id(),_("%s: Passed."),test.c_str());
		delete buf; buf = NULL;

		//--------------------------- Test 13 ----------------------------------
		test = TSYS::strMess(_("Test%d"),13);
		mod->mess(id(),_("%s: Fill and check free time integer buffer (low time)."),test.c_str());
		buf = new TValBuf( TFld::Integer, 10, 0, false, false );
		wper = 1000000;
		wtm = wper*(TSYS::curTime()/wper);
		//Not full fill
		for(int i=0; i<buf->size()/2; i++)
		    buf->setI(i,wtm+wper*i);
		if(!(buf->end() == wtm+wper*((buf->size()/2)-1) && buf->begin() == wtm) )
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at filling half buffer."));
		icnt = buf->size()/2 - 1;
		for(int64_t i = buf->end(); i >= buf->begin(); i--, icnt-- )
		    if(buf->getI(&i) != icnt) throw TError(nodePath().c_str(),_("Write a half buffer values error."));
		mod->mess(id(),_("Write a half buffer ok."));
		//Roll buff
		for(int i=buf->size()/2; i<buf->size()+5; i++)
		    switch(i)
		    {
			//Pass one value
			case 7: continue;
			//Drift values
			case 9: buf->setI(i,wtm+wper*i+wper/2); break;
			case 11:buf->setI(i,wtm+wper*i-wper/2); break;
			case 13:buf->setI(i,wtm+wper*(i-1)); 	break;
			default:buf->setI(i,wtm+wper*i);
		    }
		if(!(buf->end() == wtm+wper*(buf->size()+4) && buf->begin() == wtm+2*wper) )
		    throw TError(nodePath().c_str(),_("Buffer begin or/and end error, at roll filling buffer."));
		icnt = buf->size()+4;
		for(int64_t i = buf->end(); i >= buf->begin(); i--, icnt--)
		{
		    if(icnt == 12 || icnt == 10 || icnt == 7)	icnt--;
		    if( buf->getI(&i) != icnt ) throw TError(nodePath().c_str(),_("Write a roll buffer values error."));
		}
		mod->mess(id(),_("Write a roll buffer ok."));
		rtm = buf->end();
		buf->setI(100,rtm);
		if(buf->getI(&rtm) != 100) throw TError(nodePath().c_str(),_("Update buffer end error."));
		mod->mess(id(),_("Update buffer end ok."));

		wtm += wper*(buf->size()+5);
		st_cnt = TSYS::curTime();
		buf->setSize(1000);
		for(int i=0; i<buf->size(); i++)
		    buf->setI(i,wtm+i*wper);
		mod->mess(id(),_("Wrote 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		st_cnt = TSYS::curTime();
		for(int64_t i = buf->end(); i > buf->begin(); i=rtm-1)
		{ rtm = i; buf->getI(&rtm); }
		mod->mess(id(),_("Read 1000 values for time %f ms."),1e-3*(TSYS::curTime()-st_cnt));

		mod->mess(id(),_("%s: Passed."),test.c_str());
		delete buf; buf = NULL;

		mod->mess(id(),_("Test: Passed"));
		val->setS(0,_("Passed"));
	    }
	    catch( TError err )
	    {
		if( buf ) delete buf;
		mod->mess(id(),_("%s: Failed: %s"),test.c_str(),err.mess.c_str());
		val->setS(0,TSYS::strMess(_("Failed: %s: %s"),test.c_str(),err.mess.c_str()));
	    }
	}
};

}

#endif //TEST_VALBUF_H
