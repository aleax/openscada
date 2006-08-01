
//OpenSCADA system module Special.FLibTime file: sysfnc.h
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#ifndef SYSFNC_H
#define SYSFNC_H

#include <time.h>

#include <string>
#include <vector>

#include <tfunction.h>

#include "statfunc.h"

using std::string;
using std::vector;

namespace FLibSYS
{

//Archive subsystem's functions

//- Value archive open -
class avalOpen : public TFunction
{
    public:
	avalOpen() : TFunction("avalOpen")
	{
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Return) );
	    ioAdd( new IO("name",mod->I18N("Name"),IO::String,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch open"); }
	string descr()	{ return mod->I18N("Open value archive for fastest access at next."); }

	void calc( TValFunc *val )
	{
	    val->setI(0,mod->varchOpen(val->getS(1)));
	}
};

//- Value archive close -
class avalClose : public TFunction
{
    public:
	avalClose() : TFunction("avalClose")
	{
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch close"); }
	string descr()	{ return mod->I18N("Close opened value archive."); }

	void calc( TValFunc *val )
	{
	    mod->varchClose(val->getI(0));
	}
};

//- Value archive get integer -
class avalGetI : public TFunction
{
    public:
	avalGetI() : TFunction("avalGetI")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::Integer,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch get integer"); }
	string descr()	{ return mod->I18N("Value archive get integer."); }

	void calc( TValFunc *val )
	{
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);
	    val->setI(0,mod->varch(val->getI(1)).at().getI(&vtm,false,val->getS(4)));
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);
	}
};

//- Value archive get real -
class avalGetR : public TFunction
{
    public:
	avalGetR() : TFunction("avalGetR")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::Real,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch get real"); }
	string descr()	{ return mod->I18N("Value archive get real."); }

	void calc( TValFunc *val )
	{
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);
	    val->setR(0,mod->varch(val->getI(1)).at().getR(&vtm,false,val->getS(4)));
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);
	}
};

//- Value archive get bool -
class avalGetB : public TFunction
{
    public:
	avalGetB() : TFunction("avalGetB")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::Boolean,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch get boolean"); }
	string descr()	{ return mod->I18N("Value archive get boolean."); }

	void calc( TValFunc *val )
	{
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);
	    val->setB(0,mod->varch(val->getI(1)).at().getB(&vtm,false,val->getS(4)));
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);	    
	}
};

//- Value archive get string -
class avalGetS : public TFunction
{
    public:
	avalGetS() : TFunction("avalGetS")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::String,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch get string"); }
	string descr()	{ return mod->I18N("Value archive get string."); }

	void calc( TValFunc *val )
	{
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);
	    val->setS(0,mod->varch(val->getI(1)).at().getS(&vtm,false,val->getS(4)));
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);	    	    
	}
};

//- Value archive set integer -
class avalSetI : public TFunction
{
    public:
	avalSetI() : TFunction("avalSetI")
	{
	    ioAdd( new IO("val",mod->I18N("Value"),IO::Integer,IO::Input) );
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch set integer"); }
	string descr()	{ return mod->I18N("Value archive set integer."); }

	void calc( TValFunc *val )
	{
	    mod->varch(val->getI(1)).at().setI(val->getI(0),(long long)val->getI(2)*1000000+val->getI(3));
	}
};

//- Value archive set real -
class avalSetR : public TFunction
{
    public:
	avalSetR() : TFunction("avalSetR")
	{
	    ioAdd( new IO("val",mod->I18N("Value"),IO::Real,IO::Input) );
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch set real"); }
	string descr()	{ return mod->I18N("Value archive set real."); }

	void calc( TValFunc *val )
	{
	    mod->varch(val->getI(1)).at().setR(val->getR(0),(long long)val->getI(2)*1000000+val->getI(3));
	}
};

//- Value archive set boolean -
class avalSetB : public TFunction
{
    public:
	avalSetB() : TFunction("avalSetB")
	{
	    ioAdd( new IO("val",mod->I18N("Value"),IO::Boolean,IO::Input) );
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch set boolean"); }
	string descr()	{ return mod->I18N("Value archive set boolean."); }

	void calc( TValFunc *val )
	{
	    mod->varch(val->getI(1)).at().setB(val->getB(0),(long long)val->getI(2)*1000000+val->getI(3));
	}
};

//- Value archive set string -
class avalSetS : public TFunction
{
    public:
	avalSetS() : TFunction("avalSetS")
	{
	    ioAdd( new IO("val",mod->I18N("Value"),IO::String,IO::Input) );
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch set string"); }
	string descr()	{ return mod->I18N("Value archive set string."); }

	void calc( TValFunc *val )
	{
	    mod->varch(val->getI(1)).at().setS(val->getS(0),(long long)val->getI(2)*1000000+val->getI(3));
	}
};

//- Put message -
class messPut : public TFunction
{
    public:
	messPut() : TFunction("messPut")
	{
	    ioAdd( new IO("cat",mod->I18N("Category"),IO::String,IO::Input) );
	    ioAdd( new IO("lev",mod->I18N("Level"),IO::Integer,IO::Input) );
	    ioAdd( new IO("mess",mod->I18N("Message"),IO::String,IO::Input) );
	}
	
	string name()	{ return mod->I18N("Message put"); }
	string descr()	{ return mod->I18N("Put message to system."); }

	void calc( TValFunc *val )
	{
	    Mess->put(val->getS(0).c_str(),(TMess::Type)val->getI(1),"%s",val->getS(2).c_str());
	}
};

}

#endif //SYSFNC_H

