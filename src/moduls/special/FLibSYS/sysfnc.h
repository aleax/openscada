
//OpenSCADA system module Special.FLibSYS file: sysfnc.h
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
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
//* System access functions                       *

//*************************************************
//* System's commands call                        *
//*************************************************
class sysCall : public TFunction
{
    public:
	sysCall( ) : TFunction("sysCall")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::String,IO::Return|IO::FullText) );
	    ioAdd( new IO("com",_("Command"),IO::String,IO::Default) );
	}

	string name( )	{ return _("Sys: Call"); }
	string descr( )	{ return _("System call commands."); }

	void calc( TValFunc *val )
	{
	    FILE *fp = popen(val->getS(1).c_str(),"r");
	    if( !fp ) return;

	    char buf[STR_BUF_LEN];
	    string rez;
	    for( int r_cnt = 0; (r_cnt=fread(buf,1,sizeof(buf),fp)); )
	    rez.append(buf,r_cnt);

	    pclose(fp);
	    val->setS(0,rez);
	}
};

//*************************************************
//* SQL request to DB                             *
//*************************************************
class dbReqSQL : public TFunction
{
    public:
	dbReqSQL( ) : TFunction("dbReqSQL")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::Object,IO::Return) );
	    ioAdd( new IO("addr",_("DB address"),IO::String,IO::Default) );
	    ioAdd( new IO("req",_("SQL request"),IO::String,IO::Default) );
	}

	string name( )	{ return _("DB: SQL request"); }
	string descr( )	{ return _("Send SQL request to DB."); }

	void calc( TValFunc *val )
	{
	    string sdb = TBDS::realDBName(val->getS(1));
	    TAreaObj *rez = new TAreaObj();
	    try
	    {
		vector< vector<string> > rtbl;
		AutoHD<TBD> db = SYS->db().at().nodeAt(sdb,0,'.');
		db.at().sqlReq( val->getS(2), &rtbl );
		for( int i_r = 0; i_r < rtbl.size(); i_r++ )
		{
		    TAreaObj *row = new TAreaObj();
		    for( int i_c = 0; i_c < rtbl[i_r].size(); i_c++ )
			row->propSet(TSYS::int2str(i_c),rtbl[i_r][i_c]);
		    rez->propSet(TSYS::int2str(i_r),row);
		}
	    }catch(...){ }
	    val->setO(0,rez);
	}
};

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
	    ioAdd( new IO("addr",_("Address of archive or atribute of parameter"),IO::String,IO::Default) );
	}

	string name( )	{ return _("Varch: Begin"); }
	string descr( )	{ return _("Begin of opened value archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(0);
	    string addr = val->getS(4);
	    if( !addr.empty() )	id = mod->varchOpen(addr);

	    long long vbg;
	    if(mod->isArch(id))
		vbg = mod->varch(id).at().begin(val->getS(3));
	    else
	    {
		TValBuf* vb = mod->vbuf(id);
		if( !vb )	return;
		vbg = vb->begin();
	    }
	    val->setI(1,vbg/1000000);
	    val->setI(2,vbg%1000000);

	    if( !addr.empty() )	mod->varchClose(id);
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
	    ioAdd( new IO("addr",_("Address of archive or atribute of parameter"),IO::String,IO::Default) );
	}

	string name( )	{ return _("Varch: End"); }
	string descr( )	{ return _("End of opened value archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(0);
	    string addr = val->getS(4);
	    if( !addr.empty() )	id = mod->varchOpen(addr);

	    long long vend;
	    if(mod->isArch(id))
		vend = mod->varch(id).at().end(val->getS(3));
	    else
	    {
		TValBuf* vb = mod->vbuf(id);
		if( !vb )	return;
		vend = vb->end();
	    }
	    val->setI(1,vend/1000000);
	    val->setI(2,vend%1000000);

	    if( !addr.empty() )	mod->varchClose(id);
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
		mod->varch(sid).at().getVals(*vb,(long long)val->getI(2)*1000000+val->getI(3),
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
		mod->varch(did).at().setVals(*vb,(long long)val->getI(2)*1000000+val->getI(3),
						(long long)val->getI(4)*1000000+val->getI(5),val->getS(6));
	    }
	    else
	    {
		TValBuf* svb = mod->vbuf(sid);
		TValBuf* dvb = mod->vbuf(did);
		if(!svb || !dvb) return;
		svb->getVals(*dvb,(long long)val->getI(2)*1000000+val->getI(3),
				 (long long)val->getI(4)*1000000+val->getI(5));
	    }
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
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default,"-1") );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",_("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",_("Archivator"),IO::String,IO::Default) );
	    ioAdd( new IO("addr",_("Address of archive or atribute of parameter"),IO::String,IO::Default) );
	}

	string name( )	{ return _("Varch: Get integer"); }
	string descr( )	{ return _("Get integer from archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(1);
	    string addr = val->getS(6);
	    try
	    {
		if( !addr.empty() )	id = mod->varchOpen(addr);

		long long vtm = (long long)val->getI(2)*1000000+val->getI(3);
		if(mod->isArch(id))
		    val->setI(0,mod->varch(id).at().getVal(&vtm,val->getB(4),val->getS(5)).getI());
		else
		{
		    TValBuf* vb = mod->vbuf(id);
		    if( !vb )	return;
		    val->setI(0,vb->getI(&vtm,val->getB(4)));
		}
		val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);

		if( !addr.empty() )	mod->varchClose(id);
	    }
	    catch(TError err) { val->setI(0,EVAL_INT); }
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
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default,"-1") );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",_("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",_("Archivator"),IO::String,IO::Default) );
	    ioAdd( new IO("addr",_("Address of archive or atribute of parameter"),IO::String,IO::Default) );
	}
	
	string name( )	{ return _("Varch: Get real"); }
	string descr( )	{ return _("Get real from archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(1);
	    string addr = val->getS(6);
	    try
	    {
		if( !addr.empty() )	id = mod->varchOpen(addr);

		long long vtm = (long long)val->getI(2)*1000000+val->getI(3);
		if( mod->isArch(id) )
		    val->setR(0,mod->varch(id).at().getVal(&vtm,val->getB(4),val->getS(5)).getR());
		else
		{
		    TValBuf* vb = mod->vbuf(id);
		    if( !vb )	return;
		    val->setR(0,vb->getR(&vtm,val->getB(4)));
		}
		val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);

		if( !addr.empty() )	mod->varchClose(id);
	    }
	    catch(TError err) { val->setR(0,EVAL_REAL); }
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
	    ioAdd( new IO("val",_("Return value"),IO::Boolean,IO::Return) );
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default,"-1") );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",_("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",_("Archivator"),IO::String,IO::Default) );
	    ioAdd( new IO("addr",_("Address of archive or atribute of parameter"),IO::String,IO::Default) );
	}

	string name( )	{ return _("Varch: Get boolean"); }
	string descr( )	{ return _("Get boolean from archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(1);
	    string addr = val->getS(6);
	    try
	    {
		if( !addr.empty() )	id = mod->varchOpen(addr);

		long long vtm = (long long)val->getI(2)*1000000+val->getI(3);
		if(mod->isArch(id))
		    val->setB(0,mod->varch(id).at().getVal(&vtm,val->getB(4),val->getS(5)).getB());
		else
		{
		    TValBuf* vb = mod->vbuf(id);
		    if( !vb )	return;
		    val->setB(0,vb->getB(&vtm,val->getB(4)));
		}
		val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);

		if( !addr.empty() )	mod->varchClose(id);
	    }
	    catch(TError err) { val->setB(0,EVAL_BOOL); }
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
	    ioAdd( new IO("id",_("Buffer id"),IO::Integer,IO::Default,"-1") );
	    ioAdd( new IO("sec",_("Seconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("usec",_("Microseconds"),IO::Integer,IO::Output) );
	    ioAdd( new IO("up_ord",_("Up order"),IO::Boolean,IO::Default,"0") );
	    ioAdd( new IO("archtor",_("Archivator"),IO::String,IO::Default) );
	    ioAdd( new IO("addr",_("Address of archive or atribute of parameter"),IO::String,IO::Default) );
	}

	string name( )	{ return _("Varch: Get string"); }
	string descr( )	{ return _("Get string from archive or buffer."); }

	void calc( TValFunc *val )
	{
	    int id = val->getI(1);
	    string addr = val->getS(6);
	    try
	    {
		if( !addr.empty() )	id = mod->varchOpen(addr);

		long long vtm = (long long)val->getI(2)*1000000+val->getI(3);
		if(mod->isArch(id))
		val->setS(0,mod->varch(id).at().getVal(&vtm,val->getB(4),val->getS(5)).getS());
		else
		{
		    TValBuf* vb = mod->vbuf(id);
		    if( !vb )	return;
		    val->setS(0,vb->getS(&vtm,val->getB(4)));
		}
		val->setI(2,vtm/1000000); val->setI(3,vtm%1000000);

		if( !addr.empty() )	mod->varchClose(id);
	    }
	    catch(TError err) { val->setS(0,EVAL_STR); }
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
		if( !vb )	return;
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
		if( !vb )	return;
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
		if( !vb )	return;
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
		if( !vb )	return;
		vb->setS(val->getS(1),(long long)val->getI(2)*1000000+val->getI(3));
	    }
	}
};

//*************************************************
//* Messages function.                            *

//*************************************************
//* Get message                                   *
//*************************************************
class messGet : public TFunction
{
    public:
	messGet( ) : TFunction("messGet")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::Object,IO::Return) );
	    ioAdd( new IO("btm",_("Begin time"),IO::Integer,IO::Default) );
	    ioAdd( new IO("etm",_("End time"),IO::Integer,IO::Default) );
	    ioAdd( new IO("cat",_("Category"),IO::String,IO::Default) );
	    ioAdd( new IO("lev",_("Level"),IO::Integer,IO::Default) );
	    ioAdd( new IO("arch",_("Archivator"),IO::String,IO::Default) );
	}

	string name( )	{ return _("Mess: Get"); }
	string descr( )	{ return _("Get messages from system."); }

	void calc( TValFunc *val )
	{
	    vector<TMess::SRec> recs;
	    SYS->archive().at().messGet( val->getI(1), val->getI(2), recs, val->getS(3), val->getI(4), val->getS(5) );
	    TAreaObj *rez = new TAreaObj();
	    for( int i_m = 0; i_m < recs.size(); i_m++ )
	    {
		TVarObj *am = new TVarObj();
		am->propSet("tm",(int)recs[i_m].time);
		am->propSet("utm",recs[i_m].utime);
		am->propSet("categ",recs[i_m].categ);
		am->propSet("level",recs[i_m].level);
		am->propSet("mess",recs[i_m].mess);
		rez->propSet(TSYS::int2str(i_m),am);
	    }
	    val->setO(0,rez);
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
//* String manipulation functions                 *

//*************************************************
//* String size                                   *
//*************************************************
class strSize : public TFunction
{
    public:
	strSize( ) : TFunction("strSize")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::Integer,IO::Return) );
	    ioAdd( new IO("str",_("String"),IO::String,IO::Default) );
	}

	string name( )	{ return _("String: Get size"); }
	string descr( )	{ return _("Use for string size getting."); }

	void calc( TValFunc *val )
	{
	    val->setI(0,val->getS(1).size());
	}
};

//*************************************************
//* String substring                              *
//*************************************************
class strSubstr : public TFunction
{
    public:
	strSubstr( ) : TFunction("strSubstr")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::String,IO::Return) );
	    ioAdd( new IO("str",_("String"),IO::String,IO::Default) );
	    ioAdd( new IO("pos",_("Position"),IO::Integer,IO::Default,"0") );
	    ioAdd( new IO("n"  ,_("Number"),IO::Integer,IO::Default,"-1") );
	}

	string name( )	{ return _("String: Get substring"); }
	string descr( )	{ return _("Use for substring getting."); }

	void calc( TValFunc *val )
	{
	    string vl  = val->getS(1);
	    int    pos = val->getI(2);
	    if( pos<0 || pos>=vl.size() ) return;
	    int	   n   = val->getI(3);
	    if( n < 0 )	n = vl.size();
	    n = vmin(vl.size()-pos,n);
	    val->setS(0,vl.substr(pos,n));
	}
};

//*************************************************
//* String insertion                              *
//*************************************************
class strInsert : public TFunction
{
    public:
	strInsert( ) : TFunction("strInsert")
	{
	    ioAdd( new IO("str",_("String"),IO::String,IO::Output) );
	    ioAdd( new IO("pos",_("Position"),IO::Integer,IO::Default,"0") );
	    ioAdd( new IO("ins",_("Insert string"),IO::String,IO::Default) );
	}

	string name( )	{ return _("String: Insert string to other string"); }
	string descr( )	{ return _("Use for insertion string to other string."); }

	void calc( TValFunc *val )
	{
	    string vl = val->getS(0);
	    int   pos = val->getI(1);
	    pos = vmax(0,vmin(vl.size(),pos));
	    val->setS(0,vl.insert(pos,val->getS(2)));
	}
};

//*************************************************
//* String replace                                *
//*************************************************
class strReplace : public TFunction
{
    public:
	strReplace( ) : TFunction("strReplace")
	{
	    ioAdd( new IO("str",_("String"),IO::String,IO::Output) );
	    ioAdd( new IO("pos",_("Position"),IO::Integer,IO::Default,"0") );
	    ioAdd( new IO("n"  ,_("Number"),IO::Integer,IO::Default,"-1") );
	    ioAdd( new IO("repl",_("Replace string"),IO::String,IO::Default) );
	}

	string name( )	{ return _("String: Replace part string on other string"); }
	string descr( )	{ return _("Use for replacing part string on other string."); }

	void calc( TValFunc *val )
	{
	    string vl  = val->getS(0);
	    int    pos = val->getI(1);
	    if( pos<0 || pos>=vl.size() ) return;
	    int	   n   = val->getI(2);
	    if( n < 0 )	n = vl.size();
	    n = vmin(vl.size()-pos,n);
	    val->setS(0,vl.replace(pos,n,val->getS(3)));
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

//*************************************************
//* Path string convert to separated string       *
//*************************************************
class strPath2Sep : public TFunction
{
    public:
	strPath2Sep( ) : TFunction("strPath2Sep")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::String,IO::Return) );
	    ioAdd( new IO("src",_("Source"),IO::String,IO::Default) );
	    ioAdd( new IO("sep",_("Separator"),IO::String,IO::Default,".") );
	}

	string name( )	{ return _("String: Path to separated string"); }
	string descr( )	{ return _("Use for convert path to separated string."); }

	void calc( TValFunc *val )
	{
	    val->setS(0,TSYS::path2sepstr(val->getS(1),val->getS(2).size()?val->getS(2)[0]:'.'));
	}
};

//*************************************************
//* String encode to HTML                         *
//*************************************************
class strEnc2HTML : public TFunction
{
    public:
	strEnc2HTML( ) : TFunction("strEnc2HTML")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::String,IO::Return) );
	    ioAdd( new IO("src",_("Source"),IO::String,IO::Default) );
	}

	string name( )	{ return _("String: Encode string to HTML"); }
	string descr( )	{ return _("Use for encode string for use into HTML source."); }

	void calc( TValFunc *val )
	{
	    val->setS(0,TSYS::strEncode(val->getS(1),TSYS::Html));
	}
};

//*************************************************
//* Convert real to string                        *
//*************************************************
class real2str : public TFunction
{
    public:
	real2str( ) : TFunction("real2str")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::String,IO::Return) );
	    ioAdd( new IO("val",_("Value"),IO::Real,IO::Default) );
	    ioAdd( new IO("prc",_("Precision"),IO::Integer,IO::Default,"4") );
	    ioAdd( new IO("tp",_("Type"),IO::String,IO::Default,"f") );
	}

	string name( )	{ return _("String: Real to string"); }
	string descr( )	{ return _("Convert real to string."); }

	void calc( TValFunc *val )
	{
	    val->setS(0,TSYS::real2str(val->getR(1),val->getI(2),val->getS(3).size()?val->getS(3)[0]:'f') );
	}
};

//*************************************************
//* Convert integer to string                     *
//*************************************************
class int2str : public TFunction
{
    public:
	int2str( ) : TFunction("int2str")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::String,IO::Return) );
	    ioAdd( new IO("val",_("Value"),IO::Integer,IO::Default) );
	    ioAdd( new IO("base",_("Base"),IO::Integer,IO::Default,"10") );
	}

	string name( )	{ return _("String: Integer to string"); }
	string descr( )	{ return _("Convert integer to string."); }

	void calc( TValFunc *val )
	{
	    switch( val->getI(2) )
	    {
		case 8:	val->setS(0,TSYS::int2str(val->getI(1),TSYS::Oct));	break;
		case 10:val->setS(0,TSYS::int2str(val->getI(1),TSYS::Dec));	break;
		case 16:val->setS(0,TSYS::int2str(val->getI(1),TSYS::Hex));	break;
		default: val->setS(0,"");
	    }
	}
};

//*************************************************
//* Convert string to real                        *
//*************************************************
class str2real : public TFunction
{
    public:
	str2real( ) : TFunction("str2real")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::Real,IO::Return) );
	    ioAdd( new IO("val",_("Value"),IO::String,IO::Default) );
	}

	string name( )	{ return _("String: String to real"); }
	string descr( )	{ return _("Convert string to real."); }

	void calc( TValFunc *val )	{ val->setR(0,atof(val->getS(1).c_str())); }
};

//*************************************************
//* Convert string to integer                     *
//*************************************************
class str2int : public TFunction
{
    public:
	str2int( ) : TFunction("str2int")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::Integer,IO::Return) );
	    ioAdd( new IO("val",_("Value"),IO::String,IO::Default) );
	}

	string name( )	{ return _("String: String to integer"); }
	string descr( )	{ return _("Convert string to integer."); }

	void calc( TValFunc *val )	{ val->setI(0,strtol(val->getS(1).c_str(),NULL,0)); }
};

//*************************************************
//* Split float to words                          *
//*************************************************
class floatSplitWord : public TFunction
{
    public:
	floatSplitWord( ) : TFunction("floatSplitWord")
	{
	    ioAdd( new IO("val",_("Value"),IO::Real,IO::Default) );
	    ioAdd( new IO("w1",_("Word 1"),IO::Integer,IO::Output) );
	    ioAdd( new IO("w2",_("Word 2"),IO::Integer,IO::Output) );
	}

	string name( )	{ return _("Float: Split to words"); }
	string descr( )	{ return _("Split float (4 byte) to words (2 byte)."); }

	void calc( TValFunc *val )
	{
	    float vl = val->getR(0);
	    val->setI(1,*((ui16*)&vl));
	    val->setI(2,*(((ui16*)&vl)+1));
	}
};

//*************************************************
//* Merge float from words                        *
//*************************************************
class floatMergeWord : public TFunction
{
    public:
	floatMergeWord( ) : TFunction("floatMergeWord")
	{
	    ioAdd( new IO("rez",_("Rezult"),IO::Real,IO::Return) );
	    ioAdd( new IO("w1",_("Word 1"),IO::Integer,IO::Default) );
	    ioAdd( new IO("w2",_("Word 2"),IO::Integer,IO::Default) );
	}

	string name( )	{ return _("Float: Merge from words"); }
	string descr( )	{ return _("Merge float (4 byte) from words (2 byte)."); }

	void calc( TValFunc *val )
	{
	    float vl = 0;
	    *((ui16*)&vl) = val->getI(1);
	    *(((ui16*)&vl)+1) = val->getI(2);
	    val->setR(0,vl);
	}
};

}

#endif //SYSFNC_H
