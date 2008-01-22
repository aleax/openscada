
//OpenSCADA system module Special.FLibSYS file: sysfnc.h
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#ifndef SYSFNC_H
#define SYSFNC_H

#include <time.h>

#include <tfunction.h>

#include "statfunc.h"

namespace FLibSYS
{

//*************************************************
//* Archive subsystem's functions                 *

//*************************************************
//* Value archive open                            *
//*************************************************
class varhOpen : public TFunction
{
    public:
	varhOpen( ) : TFunction("varhOpen")
	{
	    ioAdd( new IO("id",_("Archive id"),IO::Integer,IO::Return) );
	    ioAdd( new IO("name",_("Name"),IO::String,IO::Default) );
	}
	
	string name( )	{ return _("Varch: Open"); }
	string descr( )	{ return _("Open value archive."); }

	void calc( TValFunc *val )
	{
	    val->setI(0,mod->varchOpen(val->getS(1)));
	}
};

//*************************************************
//* Value archive close                           *
//*************************************************
class varhClose : public TFunction
{
    public:
	varhClose( ) : TFunction("varhClose")
	{
	    ioAdd( new IO("id",_("Archive id"),IO::Integer,IO::Default) );
	}
	
	string name( )	{ return _("Varch: Close"); }
	string descr( )	{ return _("Close opened value archive or buffer."); }

	void calc( TValFunc *val )
	{
	    mod->varchClose(val->getI(0));
	}
};

//*************************************************
//* Value archive begin                           *
//*************************************************
class varhBeg : public TFunction
{
    public:
	varhBeg( ) : TFunction("varhBeg")
	{	    
	    ioAdd( new IO("id",_("Archive id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sek",_("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usek",_("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("archtor",_("Archivator"),IO::String,IO::Default) );	    
	}
	
	string name( )	{ return _("Varch: Begin"); }
	string descr( )	{ return _("Begin of opened value archive or buffer."); }

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

//*************************************************
//* Value archive end                             *
//*************************************************
class varhEnd : public TFunction
{
    public:
	varhEnd( ) : TFunction("varhEnd")
	{	    
	    ioAdd( new IO("id",_("Archive id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sek",_("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usek",_("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("archtor",_("Archivator"),IO::String,IO::Default) );	    
	}
	
	string name( )	{ return _("Varch: End"); }
	string descr( )	{ return _("End of opened value archive or buffer."); }

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

//*************************************************
//* Value copy among archives and buffers         *
//*************************************************
class varhCopyBuf : public TFunction
{
    public:
	varhCopyBuf( ) : TFunction("varhCopyBuf")
	{	    
	    ioAdd( new IO("sid",_("Source buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("did",_("Destination buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("begSek",_("Begin seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("begUsek",_("Begin microseconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("endSek",_("End seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("endUsek",_("End microseconds"),IO::Integer,IO::Default) );    
	    ioAdd( new IO("archtor",_("Archivator"),IO::String,IO::Default) );
	}
	
	string name( )	{ return _("Varch: Copy values"); }
	string descr( )	{ return _("Copy values among value archives and buffers."); }

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

//*************************************************
//* Value buffer open                             *
//*************************************************
class varhBufOpen : public TFunction
{
    public:
	varhBufOpen( ) : TFunction("varhBufOpen")
	{
	    ioAdd( new IO("id",_("Archive id"),IO::Integer,IO::Return) );
	    ioAdd( new IO("tp",_("Type"),IO::Integer,IO::Default,"1") );
	    ioAdd( new IO("sz",_("Size"),IO::Integer,IO::Default,"100") );
	    ioAdd( new IO("per",_("Period (us)"),IO::Integer,IO::Default,"1000000") );
	    ioAdd( new IO("hgrd",_("Hard grid"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("hres",_("High resolution"),IO::Boolean,IO::Default,"0") );        
	}
	
	string name( )	{ return _("Varch: Buffer open"); }
	string descr( )	{ return _("Open value buffer for temporary values storing."); }

	void calc( TValFunc *val )
	{
	    val->setI(0,mod->varchBufOpen((TFld::Type)val->getI(1),val->getI(2),val->getI(3),val->getB(4),val->getB(5)));
	}
};

//*************************************************
//* Get integer                                   *
//*************************************************
class varhGetI : public TFunction
{
    public:
	varhGetI( ) : TFunction("varhGetI")
	{
	    ioAdd( new IO("val",_("Return value"),IO::Integer,IO::Return) );
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",_("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",_("Archivator"),IO::String,IO::Default) );
	}
	
	string name( )	{ return _("Varch: Get integer"); }
	string descr( )	{ return _("Get integer from archive or buffer."); }

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

//*************************************************
//* Get real                                      *
//*************************************************
class varhGetR : public TFunction
{
    public:
	varhGetR( ) : TFunction("varhGetR")
	{
	    ioAdd( new IO("val",_("Return value"),IO::Real,IO::Return) );
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",_("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",_("Archivator"),IO::String,IO::Default) );	    
	}
	
	string name( )	{ return _("Varch: Get real"); }
	string descr( )	{ return _("Get real from archive or buffer."); }

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

//*************************************************
//* Get boolean                                   *
//*************************************************
class varhGetB : public TFunction
{
    public:
	varhGetB( ) : TFunction("varhGetB")
	{
	    ioAdd( new IO("val",_("Return value"),IO::Integer,IO::Return) );
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",_("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",_("Archivator"),IO::String,IO::Default) );
	}
	
	string name( )	{ return _("Varch: Get boolean"); }
	string descr( )	{ return _("Get boolean from archive or buffer."); }

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

//*************************************************
//* Get string                                    *
//*************************************************
class varhGetS : public TFunction
{
    public:
	varhGetS( ) : TFunction("varhGetS")
	{
	    ioAdd( new IO("val",_("Return value"),IO::String,IO::Return) );
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",_("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",_("Archivator"),IO::String,IO::Default) );	    
	}
	
	string name( )	{ return _("Varch: Get string"); }
	string descr( )	{ return _("Get string from archive or buffer."); }

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

//*************************************************
//* Set integer                                   *
//*************************************************
class varhSetI : public TFunction
{
    public:
	varhSetI( ) : TFunction("varhSetI")
	{
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("val",_("Value"),IO::Integer,IO::Default) );	    
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Default) );
	}
	
	string name( )	{ return _("Varch: Set integer"); }
	string descr( )	{ return _("Set integer to archive or buffer."); }

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

//*************************************************
//* Set real                                      *
//*************************************************
class varhSetR : public TFunction
{
    public:
	varhSetR( ) : TFunction("varhSetR")
	{
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("val",_("Value"),IO::Real,IO::Default) );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Default) );
	}
	
	string name( )	{ return _("Varch: Set real"); }
	string descr( )	{ return _("Set real to archive or buffer."); }

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

//*************************************************
//* Set boolean                                   *
//*************************************************
class varhSetB : public TFunction
{
    public:
	varhSetB( ) : TFunction("varhSetB")
	{
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("val",_("Value"),IO::Boolean,IO::Default) );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Default) );
	}
	
	string name( )	{ return _("Varch: Set boolean"); }
	string descr( )	{ return _("Set boolean to archive or buffer."); }

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

//*************************************************
//* Set string                                    *
//*************************************************
class varhSetS : public TFunction
{
    public:
	varhSetS( ) : TFunction("varhSetS")
	{
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default) );
	    ioAdd( new IO("val",_("Value"),IO::Real,IO::Default) );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Default) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Default) );
	}
	
	string name( )	{ return _("Varch: Set string"); }
	string descr( )	{ return _("Set string to archive or buffer."); }

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

//*************************************************
//* Put message                                   *
//*************************************************
class messPut : public TFunction
{
    public:
	messPut( ) : TFunction("messPut")
	{
	    ioAdd( new IO("cat",_("Category"),IO::String,IO::Default) );
	    ioAdd( new IO("lev",_("Level"),IO::Integer,IO::Default) );
	    ioAdd( new IO("mess",_("Message"),IO::String,IO::Default) );
	}
	
	string name( )	{ return _("Mess: Put"); }
	string descr( )	{ return _("Put message to system."); }

	void calc( TValFunc *val )
	{
	    message(val->getS(0).c_str(),(TMess::Type)val->getI(1),"%s",val->getS(2).c_str());
	}
};

//*************************************************
//* String parsing on separator                   *
//*************************************************
class strParse : public TFunction
{
    public:
	strParse( ) : TFunction("strParse")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::String,IO::Return) );
	    ioAdd( new IO("str",_("String"),IO::String,IO::Default) );
	    ioAdd( new IO("lev",_("Level"),IO::Integer,IO::Default) );
	    ioAdd( new IO("sep",_("Separator"),IO::String,IO::Default,".") );
	    ioAdd( new IO("off",_("Offset"),IO::Integer,IO::Output) );
	}
	
	string name( )	{ return _("String: Parse on separator"); }
	string descr( )	{ return _("Use for parse string on separator."); }

	void calc( TValFunc *val )
	{
	    string sep = val->getS(3);
	    int off = val->getI(4);
	    val->setS(0,TSYS::strSepParse(val->getS(1),val->getI(2),sep.size()?sep[0]:' ',&off));
	    val->setI(4,off);
	}
};

//*************************************************
//* Path parsing                                  *
//*************************************************
class strParsePath : public TFunction
{
    public:
	strParsePath( ) : TFunction("strParsePath")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::String,IO::Return) );
	    ioAdd( new IO("path",_("Path"),IO::String,IO::Default) );
	    ioAdd( new IO("lev",_("Level"),IO::Integer,IO::Default) );
	    ioAdd( new IO("off",_("Offset"),IO::Integer,IO::Output) );
	}
	
	string name( )	{ return _("String: Path parse"); }
	string descr( )	{ return _("Use for parse path on elements."); }

	void calc( TValFunc *val )
	{
	    int off = val->getI(3);
	    val->setS(0,TSYS::pathLev(val->getS(1),val->getI(2),true,&off));
	    val->setI(3,off);
	}
};

}

#endif //SYSFNC_H

