
//OpenSCADA module Special.SystemTests file: test_archive.h
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

#ifndef TEST_ARCHIVE_H
#define TEST_ARCHIVE_H

#include <tfunction.h>

namespace KernelTest
{

//*************************************************************
//* TestArchive: The tests of location in the value archives. *
//*************************************************************
class TestArchive : public TFunction
{
    public:
	TestArchive( ) : TFunction("Archive",SSPC_ID)
	{
	    ioAdd(new IO("rez",_("Result"),IO::String,IO::Return));
	    ioAdd(new IO("arch",_("Value archive"),IO::String,IO::Default));
	    ioAdd(new IO("period",_("Period of the values, microseconds"),IO::Integer,IO::Default,"1000000"));
	    ioAdd(new IO("archtor",_("Archiver"),IO::String,IO::Default));
	}

	string name( )	{ return _("Value archive"); }
	string descr( )	{ return _("Test of location in the value archives.\n"
				   "Contains 7(8) tests of the value archiver to verify the correct functioning of the sequential packing mechanism."); }

	void calc( TValFunc *val )
	{
	    try {
		mod->mess(id(),_("Test: Start"));

		string arch = val->getS(1), archtor = val->getS(3);
		int64_t per = val->getI(2);

		AutoHD<TVArchive> o_arch = SYS->archive().at().valAt(arch);

		int buf_sz = 5;
		int64_t wtm = per*(TSYS::curTime()/per)-per*buf_sz;
		int64_t ttm;

		TValBuf buf(TFld::Integer, buf_sz, per, true, false);
		//--------------------------- Test 1 ----------------------------------
		mod->mess(id(),_("Test1: Simple fill and check archive."));
		for(int i_el = 0; i_el < buf_sz; i_el++)
		    buf.setI((int)pow(10,i_el),wtm+i_el*per);
		o_arch.at().setVals(buf, buf.begin(), buf.end(), archtor);
		for(int i_el = 0; i_el < buf_sz+2; i_el++) {
		    ttm = wtm + i_el*per;
		    int64_t val = o_arch.at().getVal(&ttm).getI();
		    if((i_el < buf_sz && val != pow(10,i_el)) || (i_el >= buf_sz && val != EVAL_INT))
			throw TError(nodePath().c_str(),_("Test1: Failed."));
		}
		mod->mess(id(),_("Test1: Passed."));
		//--------------------------- Test 2 ----------------------------------
		mod->mess(id(),_("Test2: Internal insert for double value (to down)."));
		buf.clear();
		buf.setI((int)pow(10,2),wtm+3*per);
		o_arch.at().setVals(buf,wtm+3*per,wtm+3*per,archtor);
		for(int i_el = 0; i_el < buf_sz+2; i_el++) {
		    ttm = wtm + i_el*per;
		    int64_t val = o_arch.at().getVal(&ttm).getI();
		    if((i_el < buf_sz && i_el != 3 && val != pow(10,i_el)) ||
			    (i_el == 3 && val != pow(10,2)) ||
			    (i_el >= buf_sz && val != EVAL_INT))
			throw TError(nodePath().c_str(),_("Test2: Failed."));
		}
		mod->mess(id(),_("Test2: Passed."));
		//--------------------------- Test 3 ----------------------------------
		mod->mess(id(),_("Test3: Internal insert for double value (to up)."));
		buf.clear();
		buf.setI((int)pow(10,4),wtm+3*per);
		o_arch.at().setVals(buf,wtm+3*per,wtm+3*per,archtor);
		for(int i_el = 0; i_el < buf_sz+2; i_el++) {
		    ttm = wtm+i_el*per;
		    int64_t val = o_arch.at().getVal(&ttm).getI();
		    if((i_el < buf_sz && i_el != 3 && val != pow(10,i_el)) ||
			    (i_el < buf_sz && i_el == 3 && val != pow(10,4)) ||
			    (i_el >= buf_sz && val != EVAL_INT))
			throw TError(nodePath().c_str(),_("Test3: Failed."));
		}
		mod->mess(id(),_("Test3: Passed."));
		//--------------------------- Test 4 ----------------------------------
		mod->mess(id(),_("Test4: Internal insert for double value (to down)."));
		buf.clear();
		buf.setI((int)pow(10,2),wtm+3*per);
		o_arch.at().setVals(buf,wtm+3*per,wtm+3*per,archtor);
		for(int i_el = 0; i_el < buf_sz+2; i_el++) {
		    ttm = wtm+i_el*per;
		    int64_t val = o_arch.at().getVal(&ttm).getI();
		    if((i_el < buf_sz && i_el != 3 && val != pow(10,i_el)) ||
			    (i_el < buf_sz && i_el == 3 && val != pow(10,2)) ||
			    (i_el >= buf_sz && val != EVAL_INT))
			throw TError(nodePath().c_str(),_("Test4: Failed."));
		}
		mod->mess(id(),_("Test4: Passed."));
		//--------------------------- Test 5 ----------------------------------
		mod->mess(id(),_("Test5: Internal insert for not double value."));
		buf.clear();
		buf.setI((int)pow(10,9),wtm+per);
		o_arch.at().setVals(buf,wtm+per,wtm+per,archtor);
		for(int i_el = 0; i_el < buf_sz+2; i_el++) {
		    ttm = wtm+i_el*per;
		    int64_t val = o_arch.at().getVal(&ttm).getI();
		    if((i_el < buf_sz && i_el != 3 && i_el != 1 && val != pow(10,i_el)) ||
			    (i_el < buf_sz && i_el == 3 && val != pow(10,2)) ||
			    (i_el < buf_sz && i_el == 1 && val != pow(10,9)) ||
			    (i_el >= buf_sz && val != EVAL_INT))
			throw TError(nodePath().c_str(),_("Test5: Failed."));
		}
		mod->mess(id(),_("Test5: Passed."));
		//--------------------------- Test 6 ----------------------------------
		mod->mess(id(),_("Test6: Internal insert for double value (to up)."));
		buf.clear();
		buf.setI((int)pow(10,2),wtm+per);
		o_arch.at().setVals(buf,wtm+per,wtm+per,archtor);
		for(int i_el = 0; i_el < buf_sz+2; i_el++) {
		    ttm = wtm+i_el*per;
		    int64_t val = o_arch.at().getVal(&ttm).getI();
		    if((i_el < buf_sz && i_el != 3 && i_el != 1 && val != pow(10,i_el)) ||
			    (i_el < buf_sz && i_el == 3 && val != pow(10,2)) ||
			    (i_el < buf_sz && i_el == 1 && val != pow(10,2)) ||
			    (i_el >= buf_sz && val != EVAL_INT))
			throw TError(nodePath().c_str(),_("Test6: Failed."));
		}
		mod->mess(id(),_("Test6: Passed."));
		//--------------------------- Test 7 ----------------------------------
		mod->mess(id(),_("Test7: Internal insert for a value instead double value."));
		buf.clear();
		buf.setI((int)pow(10,3),wtm+3*per);
		o_arch.at().setVals(buf,wtm+3*per,wtm+3*per,archtor);
		for(int i_el = 0; i_el < buf_sz+2; i_el++) {
		    ttm = wtm+i_el*per;
		    int64_t val = o_arch.at().getVal(&ttm).getI();
		    if((i_el < buf_sz && i_el != 1 && val != pow(10,i_el)) ||
			    (i_el < buf_sz && i_el == 1 && val != pow(10,2)) ||
			    (i_el >= buf_sz && val != EVAL_INT))
			throw TError(nodePath().c_str(),_("Test7: Failed"));
		}
		mod->mess(id(),_("Test7: Passed."));
		//--------------------------- Test 8 ----------------------------------
		/*sleep(2);
		mod->mess(id(),_("Test8: Set for three values to the end."));
		wtm = o_arch.at().end("");
		buf.clear();
		for( int i_el = -1; i_el <= 1; i_el++ )
		    buf.setI(i_el,wtm+i_el*per);
		o_arch.at().setVals(buf,buf.begin(),buf.end(),archtor);
		for( int i_el = -1; i_el <= 1; i_el++)
		{
		    ttm = wtm+i_el*per;
		    if( o_arch.at().getI(&ttm) != i_el )
			throw TError(nodePath().c_str(),_("Test8: Failed."));
		}
		mod->mess(id(),_("Test8: Passed."));*/

		mod->mess(id(),_("Test: Passed"));
		val->setS(0,_("Passed"));
	    }
	    catch(TError err) {
		mod->mess(id(),_("Test: Failed: %s"),err.mess.c_str());
		val->setS(0,TSYS::strMess(_("Failed: %s"),err.mess.c_str()));
	    }
	}
};

}

#endif //TEST_ARCHIVE_H
