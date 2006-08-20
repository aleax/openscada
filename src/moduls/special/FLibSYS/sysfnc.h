
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
class varhOpen : public TFunction
{
    public:
	varhOpen() : TFunction("varhOpen")
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
class varhClose : public TFunction
{
    public:
	varhClose() : TFunction("varhClose")
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

//- Value archive begin -
class varhBeg : public TFunction
{
    public:
	varhBeg() : TFunction("varhBeg")
	{	    
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Input) );
	    ioAdd( new IO("sek",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usek",mod->I18N("Microseconds"),IO::Integer,IO::Output) );    
	}
	
	string name()	{ return mod->I18N("V arch begin"); }
	string descr()	{ return mod->I18N("Begin of opened value archive."); }

	void calc( TValFunc *val )
	{
	    long long vbg = mod->varch(val->getI(0)).at().begin(val->getS(1));
	    val->setI(2,vbg/1000000);
	    val->setI(3,vbg%1000000);
	}
};

//- Value archive end -
class varhEnd : public TFunction
{
    public:
	varhEnd() : TFunction("varhEnd")
	{	    
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Input) );
	    ioAdd( new IO("sek",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usek",mod->I18N("Microseconds"),IO::Integer,IO::Output) );    
	}
	
	string name()	{ return mod->I18N("V arch end"); }
	string descr()	{ return mod->I18N("End of opened value archive."); }

	void calc( TValFunc *val )
	{
	    long long vend = mod->varch(val->getI(0)).at().end(val->getS(1));
	    val->setI(2,vend/1000000);
	    val->setI(3,vend%1000000);
	}
};

//- Value archive get values -
class varhGetVal : public TFunction
{
    public:
	varhGetVal() : TFunction("varhGetVal")
	{	    
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("vid",mod->I18N("Value buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("begSek",mod->I18N("Begin seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("begUsek",mod->I18N("Begin microseconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("endSek",mod->I18N("End seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("endUsek",mod->I18N("End microseconds"),IO::Integer,IO::Input) );    
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch get vallue"); }
	string descr()	{ return mod->I18N("Get value from opened value archive to opened value buffer."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(1));
            if(!vb)     return;
	    mod->varch(val->getI(0)).at().getVal(*vb,(long long)val->getI(2)*1000000+val->getI(3),
						    (long long)val->getI(4)*1000000+val->getI(5),val->getS(6));
    	}
};

//- Value archive set values -
class varhSetVal : public TFunction
{
    public:
	varhSetVal() : TFunction("varhSetVal")
	{	    
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("vid",mod->I18N("Value buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("begSek",mod->I18N("Begin seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("begUsek",mod->I18N("Begin microseconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("endSek",mod->I18N("End seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("endUsek",mod->I18N("End microseconds"),IO::Integer,IO::Input) );    
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V arch set vallue"); }
	string descr()	{ return mod->I18N("Set value to opened value archive from opened value buffer."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(1));
            if(!vb)     return;
	    mod->varch(val->getI(0)).at().setVal(*vb,(long long)val->getI(2)*1000000+val->getI(3),
						    (long long)val->getI(4)*1000000+val->getI(5),val->getS(6));
    	}
};

//- Value buffer open -
class vbufOpen : public TFunction
{
    public:
	vbufOpen() : TFunction("vbufOpen")
	{
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Return) );
	    ioAdd( new IO("tp",mod->I18N("Type"),IO::Integer,IO::Input,"1") );
	    ioAdd( new IO("sz",mod->I18N("Size"),IO::Integer,IO::Input,"100") );
	    ioAdd( new IO("per",mod->I18N("Period (us)"),IO::Integer,IO::Input,"1000000") );
	    ioAdd( new IO("hgrd",mod->I18N("Hard grid"),IO::Boolean,IO::Input,"0") );
	    ioAdd( new IO("hres",mod->I18N("High resolution"),IO::Boolean,IO::Input,"0") );        
	}
	
	string name()	{ return mod->I18N("V buffer open"); }
	string descr()	{ return mod->I18N("Open value buffer for temporary values storing."); }

	void calc( TValFunc *val )
	{
	    val->setI(0,mod->vbufOpen((TFld::Type)val->getI(1),val->getI(2),val->getI(3),val->getB(4),val->getB(5)));
	}
};

//- Value buffer close -
class vbufClose : public TFunction
{
    public:
	vbufClose() : TFunction("vbufClose")
	{
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V buffer close"); }
	string descr()	{ return mod->I18N("Close opened value buffer."); }

	void calc( TValFunc *val )
	{
	    mod->vbufClose(val->getI(0));
	}
};

//- Value buffer begin -
class vbufBeg : public TFunction
{
    public:
	vbufBeg() : TFunction("vbufBeg")
	{	    
	    ioAdd( new IO("id",mod->I18N("Value buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sek",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usek",mod->I18N("Microseconds"),IO::Integer,IO::Output) );    
	}
	
	string name()	{ return mod->I18N("V buffer begin"); }
	string descr()	{ return mod->I18N("Begin of opened value buffer."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(0));
	    if(!vb)	return;
	    long long vbg = vb->begin();
	    val->setI(1,vbg/1000000);
	    val->setI(2,vbg%1000000);
	}
};

//- Value buffer end -
class vbufEnd : public TFunction
{
    public:
	vbufEnd() : TFunction("vbufEnd")
	{	    
	    ioAdd( new IO("id",mod->I18N("Value buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sek",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usek",mod->I18N("Microseconds"),IO::Integer,IO::Output) );    
	}
	
	string name()	{ return mod->I18N("V buffer end"); }
	string descr()	{ return mod->I18N("End of opened value buffer."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(0));
	    if(!vb)	return;
	    long long vend = vb->end();
	    val->setI(1,vend/1000000);
	    val->setI(2,vend%1000000);
	}
};

//- Value buffer get integer -
class vbufGetI : public TFunction
{
    public:
	vbufGetI() : TFunction("vbufGetI")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::Integer,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",mod->I18N("Up order"),IO::Boolean,IO::Input,"0") );
	}
	
	string name()	{ return mod->I18N("V buffer get integer"); }
	string descr()	{ return mod->I18N("Value buffer get integer."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(1));
            if(!vb)     return;
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);	    
	    val->setI(0,vb->getI(&vtm,val->getB(4)));
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);
	}
};

//- Value buffer get real -
class vbufGetR : public TFunction
{
    public:
	vbufGetR() : TFunction("vbufGetR")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::Real,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",mod->I18N("Up order"),IO::Boolean,IO::Input,"0") );
	}
	
	string name()	{ return mod->I18N("V buffer get real"); }
	string descr()	{ return mod->I18N("Value buffer get real."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(1));
            if(!vb)     return;
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);	    
	    val->setR(0,vb->getR(&vtm,val->getB(4)));
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);
	}
};

//- Value buffer get boolean -
class vbufGetB : public TFunction
{
    public:
	vbufGetB() : TFunction("vbufGetB")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::Boolean,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",mod->I18N("Up order"),IO::Boolean,IO::Input,"0") );
	}
	
	string name()	{ return mod->I18N("V buffer get boolean"); }
	string descr()	{ return mod->I18N("Value buffer get boolean."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(1));
            if(!vb)     return;
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);	    
	    val->setB(0,vb->getB(&vtm,val->getB(4)));
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);
	}
};

//- Value buffer get string -
class vbufGetS : public TFunction
{
    public:
	vbufGetS() : TFunction("vbufGetS")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::String,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",mod->I18N("Up order"),IO::Boolean,IO::Input,"0") );
	}
	
	string name()	{ return mod->I18N("V buffer get string"); }
	string descr()	{ return mod->I18N("Value buffer get string."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(1));
            if(!vb)     return;
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);	    
	    val->setS(0,vb->getS(&vtm,val->getB(4)));
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);
	}
};

//- Value buffer set integer -
class vbufSetI : public TFunction
{
    public:
	vbufSetI() : TFunction("vbufSetI")
	{
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("val",mod->I18N("Value"),IO::Integer,IO::Input) );	    
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V buffer set integer"); }
	string descr()	{ return mod->I18N("Value buffer set integer."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(0));
	    if(!vb)     return;			
	    vb->setI(val->getI(1),(long long)val->getI(2)*1000000+val->getI(3));
	}
};

//- Value buffer set real -
class vbufSetR : public TFunction
{
    public:
	vbufSetR() : TFunction("vbufSetR")
	{
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("val",mod->I18N("Value"),IO::Real,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V buffer set real"); }
	string descr()	{ return mod->I18N("Value buffer set real."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(0));
	    if(!vb)     return;			
	    vb->setR(val->getR(1),(long long)val->getI(2)*1000000+val->getI(3));
	}
};

//- Value buffer set boolean -
class vbufSetB : public TFunction
{
    public:
	vbufSetB() : TFunction("vbufSetB")
	{
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("val",mod->I18N("Value"),IO::Boolean,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V buffer set boolean"); }
	string descr()	{ return mod->I18N("Value buffer set boolean."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(0));
	    if(!vb)     return;			
	    vb->setB(val->getB(1),(long long)val->getI(2)*1000000+val->getI(3));
	}
};

//- Value buffer set string -
class vbufSetS : public TFunction
{
    public:
	vbufSetS() : TFunction("vbufSetS")
	{
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Input) );
	    ioAdd( new IO("val",mod->I18N("Value"),IO::Real,IO::Input) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Input) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Input) );
	}
	
	string name()	{ return mod->I18N("V buffer set string"); }
	string descr()	{ return mod->I18N("Value buffer set string."); }

	void calc( TValFunc *val )
	{
	    TValBuf* vb = mod->vbuf(val->getI(0));
	    if(!vb)     return;			
	    vb->setS(val->getS(1),(long long)val->getI(2)*1000000+val->getI(3));
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

