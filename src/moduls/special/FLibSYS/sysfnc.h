
//OpenSCADA system module Special.FLibSYS file: sysfnc.h
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

#include <tfunction.h>

#include "statfunc.h"

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
	    ioAdd( new IO("name",mod->I18N("Name"),IO::String,IO::Default) );
	}
	
	string name()	{ return mod->I18N("Varch: Open"); }
	string descr()	{ return mod->I18N("Open value archive."); }

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
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Default) );
	}
	
	string name()	{ return mod->I18N("Varch: Close"); }
	string descr()	{ return mod->I18N("Close opened value archive or buffer."); }

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
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sek",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usek",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Default) );	    
	}
	
	string name()	{ return mod->I18N("Varch: Begin"); }
	string descr()	{ return mod->I18N("Begin of opened value archive or buffer."); }

	void calc( TValFunc *val )
	{	    
	    int id = val->getI(0);
	    long long vbg;
	    if(mod->isArch(id))
		vbg = mod->varch(id).at().begin(val->getS(3));
	    else 
	    {
		TValBuf* vb = mod->vbuf(id);
        	if(!vb)     return;
                vbg = vb->begin();	    
	    }
	    val->setI(1,vbg/1000000);
	    val->setI(2,vbg%1000000);
	}
};

//- Value archive end -
class varhEnd : public TFunction
{
    public:
	varhEnd() : TFunction("varhEnd")
	{	    
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sek",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usek",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Default) );	    
	}
	
	string name()	{ return mod->I18N("Varch: End"); }
	string descr()	{ return mod->I18N("End of opened value archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(0);
	    long long vend;
	    if(mod->isArch(id))
		vend = mod->varch(id).at().end(val->getS(3));
	    else 
	    {
		TValBuf* vb = mod->vbuf(id);
        	if(!vb)     return;
                vend = vb->end();	    
	    }
	    val->setI(1,vend/1000000);
	    val->setI(2,vend%1000000);
	}
};

//- Value copy among archives and buffers -
class varhCopyBuf : public TFunction
{
    public:
	varhCopyBuf() : TFunction("varhCopyBuf")
	{	    
	    ioAdd( new IO("sid",mod->I18N("Source buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("did",mod->I18N("Destination buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("begSek",mod->I18N("Begin seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("begUsek",mod->I18N("Begin microseconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("endSek",mod->I18N("End seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("endUsek",mod->I18N("End microseconds"),IO::Integer,IO::Default) );    
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Default) );
	}
	
	string name()	{ return mod->I18N("Varch: Copy values"); }
	string descr()	{ return mod->I18N("Copy values among value archives and buffers."); }

	void calc( TValFunc *val )
	{
	    int sid = val->getI(0);
	    int did = val->getI(1);
	    AutoHD<TVArchive> tarch;
	    if(mod->isArch(sid))
	    {
		TValBuf* vb = NULL;
		if(mod->isArch(did))
		{
		    tarch = mod->varch(did);
		    vb = &tarch.at();
		}
		else vb = mod->vbuf(did);
		if(!vb)     return;
		mod->varch(sid).at().getVal(*vb,(long long)val->getI(2)*1000000+val->getI(3),
						(long long)val->getI(4)*1000000+val->getI(5),val->getS(6));
	    }
	    else if(mod->isArch(did))
	    {
		TValBuf* vb = NULL;
		if(mod->isArch(sid))
		{
		    tarch = mod->varch(sid);
		    vb = &tarch.at();
		}
		else vb = mod->vbuf(sid);
		if(!vb)     return;
		mod->varch(did).at().setVal(*vb,(long long)val->getI(2)*1000000+val->getI(3),
						(long long)val->getI(4)*1000000+val->getI(5),val->getS(6));		
	    }
	    else
	    {
		TValBuf* svb = mod->vbuf(sid);
		TValBuf* dvb = mod->vbuf(did);
        	if(!svb || !dvb) return;
		svb->getVal(*dvb,(long long)val->getI(2)*1000000+val->getI(3),
				 (long long)val->getI(4)*1000000+val->getI(5));
	    }
    	}
};

//- Value buffer open -
class varhBufOpen : public TFunction
{
    public:
	varhBufOpen() : TFunction("varhBufOpen")
	{
	    ioAdd( new IO("id",mod->I18N("Archive id"),IO::Integer,IO::Return) );
	    ioAdd( new IO("tp",mod->I18N("Type"),IO::Integer,IO::Default,"1") );
	    ioAdd( new IO("sz",mod->I18N("Size"),IO::Integer,IO::Default,"100") );
	    ioAdd( new IO("per",mod->I18N("Period (us)"),IO::Integer,IO::Default,"1000000") );
	    ioAdd( new IO("hgrd",mod->I18N("Hard grid"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("hres",mod->I18N("High resolution"),IO::Boolean,IO::Default,"0") );        
	}
	
	string name()	{ return mod->I18N("Varch: Buffer open"); }
	string descr()	{ return mod->I18N("Open value buffer for temporary values storing."); }

	void calc( TValFunc *val )
	{
	    val->setI(0,mod->varchBufOpen((TFld::Type)val->getI(1),val->getI(2),val->getI(3),val->getB(4),val->getB(5)));
	}
};

//- Get integer -
class varhGetI : public TFunction
{
    public:
	varhGetI() : TFunction("varhGetI")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::Integer,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",mod->I18N("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Default) );
	}
	
	string name()	{ return mod->I18N("Varch: Get integer"); }
	string descr()	{ return mod->I18N("Get integer from archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(1);
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);	    
	    if(mod->isArch(id))
		val->setI(0,mod->varch(id).at().getI(&vtm,val->getB(4),val->getS(5)));
	    else
	    {	
		TValBuf* vb = mod->vbuf(id);
        	if(!vb)     return;	    
		val->setI(0,vb->getI(&vtm,val->getB(4)));
	    }
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);
	}
};

//- Get real -
class varhGetR : public TFunction
{
    public:
	varhGetR() : TFunction("varhGetR")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::Real,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",mod->I18N("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Default) );	    
	}
	
	string name()	{ return mod->I18N("Varch: Get real"); }
	string descr()	{ return mod->I18N("Get real from archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(1);
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);	    
	    if(mod->isArch(id))
		val->setR(0,mod->varch(id).at().getR(&vtm,val->getB(4),val->getS(5)));
	    else
	    {	
		TValBuf* vb = mod->vbuf(id);
        	if(!vb)     return;	    
		val->setR(0,vb->getR(&vtm,val->getB(4)));
	    }
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);
	}
};

//- Get boolean -
class varhGetB : public TFunction
{
    public:
	varhGetB() : TFunction("varhGetB")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::Integer,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",mod->I18N("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Default) );
	}
	
	string name()	{ return mod->I18N("Varch: Get boolean"); }
	string descr()	{ return mod->I18N("Get boolean from archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(1);
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);	    
	    if(mod->isArch(id))
		val->setB(0,mod->varch(id).at().getB(&vtm,val->getB(4),val->getS(5)));
	    else
	    {	
		TValBuf* vb = mod->vbuf(id);
        	if(!vb)     return;	    
		val->setB(0,vb->getB(&vtm,val->getB(4)));
	    }
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);
	}
};

//- Get string -
class varhGetS : public TFunction
{
    public:
	varhGetS() : TFunction("varhGetS")
	{
	    ioAdd( new IO("val",mod->I18N("Return value"),IO::String,IO::Return) );
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",mod->I18N("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",mod->I18N("Archivator"),IO::String,IO::Default) );	    
	}
	
	string name()	{ return mod->I18N("Varch: Get string"); }
	string descr()	{ return mod->I18N("Get string from archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(1);
	    long long vtm = (long long)val->getI(2)*1000000+val->getI(3);	    
	    if(mod->isArch(id))
		val->setS(0,mod->varch(id).at().getS(&vtm,val->getB(4),val->getS(5)));
	    else
	    {	
		TValBuf* vb = mod->vbuf(id);
        	if(!vb)     return;	    
		val->setS(0,vb->getS(&vtm,val->getB(4)));
	    }
	    val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);
	}
};

//- Set integer -
class varhSetI : public TFunction
{
    public:
	varhSetI() : TFunction("varhSetI")
	{
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("val",mod->I18N("Value"),IO::Integer,IO::Default) );	    
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Default) );
	}
	
	string name()	{ return mod->I18N("Varch: Set integer"); }
	string descr()	{ return mod->I18N("Set integer to archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(0);
	    if(mod->isArch(id))	
		mod->varch(id).at().setI(val->getI(1),(long long)val->getI(2)*1000000+val->getI(3));
	    else	
	    {	
		TValBuf* vb = mod->vbuf(id);
	        if(!vb)     return;
		vb->setI(val->getI(1),(long long)val->getI(2)*1000000+val->getI(3));
	    }
	}
};

//- Set real -
class varhSetR : public TFunction
{
    public:
	varhSetR() : TFunction("varhSetR")
	{
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("val",mod->I18N("Value"),IO::Real,IO::Default) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Default) );
	}
	
	string name()	{ return mod->I18N("Varch: Set real"); }
	string descr()	{ return mod->I18N("Set real to archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(0);
	    if(mod->isArch(id))	
		mod->varch(id).at().setR(val->getR(1),(long long)val->getI(2)*1000000+val->getI(3));
	    else	
	    {	
		TValBuf* vb = mod->vbuf(id);
	        if(!vb)     return;
		vb->setR(val->getR(1),(long long)val->getI(2)*1000000+val->getI(3));
	    }
	}
};

//- Set boolean -
class varhSetB : public TFunction
{
    public:
	varhSetB() : TFunction("varhSetB")
	{
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("val",mod->I18N("Value"),IO::Boolean,IO::Default) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Default) );
	}
	
	string name()	{ return mod->I18N("Varch: Set boolean"); }
	string descr()	{ return mod->I18N("Set boolean to archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(0);
	    if(mod->isArch(id))	
		mod->varch(id).at().setB(val->getB(1),(long long)val->getI(2)*1000000+val->getI(3));
	    else	
	    {	
		TValBuf* vb = mod->vbuf(id);
	        if(!vb)     return;
		vb->setB(val->getB(1),(long long)val->getI(2)*1000000+val->getI(3));
	    }
	}
};

//- Set string -
class varhSetS : public TFunction
{
    public:
	varhSetS() : TFunction("varhSetS")
	{
	    ioAdd( new IO("id",mod->I18N("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("val",mod->I18N("Value"),IO::Real,IO::Default) );
	    ioAdd( new IO("sec",mod->I18N("Seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("usec",mod->I18N("Microseconds"),IO::Integer,IO::Default) );
	}
	
	string name()	{ return mod->I18N("Varch: Set string"); }
	string descr()	{ return mod->I18N("Set string to archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(0);
	    if(mod->isArch(id))	
		mod->varch(id).at().setS(val->getS(1),(long long)val->getI(2)*1000000+val->getI(3));
	    else	
	    {	
		TValBuf* vb = mod->vbuf(id);
	        if(!vb)     return;
		vb->setS(val->getS(1),(long long)val->getI(2)*1000000+val->getI(3));
	    }
	}
};

//- Put message -
class messPut : public TFunction
{
    public:
	messPut() : TFunction("messPut")
	{
	    ioAdd( new IO("cat",mod->I18N("Category"),IO::String,IO::Default) );
	    ioAdd( new IO("lev",mod->I18N("Level"),IO::Integer,IO::Default) );
	    ioAdd( new IO("mess",mod->I18N("Message"),IO::String,IO::Default) );
	}
	
	string name()	{ return mod->I18N("Mess: Put"); }
	string descr()	{ return mod->I18N("Put message to system."); }

	void calc( TValFunc *val )
	{
	    Mess->put(val->getS(0).c_str(),(TMess::Type)val->getI(1),"%s",val->getS(2).c_str());
	}
};

}

#endif //SYSFNC_H

