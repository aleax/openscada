
//OpenSCADA module Special.FLibSYS file: varchfnc.h
/***************************************************************************
 *   Copyright (C) 2009-2014,2019 by Roman Savochenko, <rom_as@oscada.org> *
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

#ifndef VARCHFNC_H
#define VARCHFNC_H

#include <tfunction.h>

#include "statfunc.h"

namespace FLibSYS
{

//*************************************************
//* VArchObj - Value archive object               *
//*************************************************
class VArchObj : public TVarObj
{
    public:
	//Methods
	VArchObj( const string &user = "" );
	~VArchObj( );

	string objName( )	{ return "arch"; }

	bool open( const string &inm );
	bool open( TFld::Type vtp, int isz, int64_t ipr, bool ihgrd, bool ihres );
	void close( );

	bool isArch( )		{ return mIsArch && mArch; }
	AutoHD<TVArchive> arch( );
	TValBuf *buf( );

	TVariant propGet( const string &id );
	void propSet( const string &id, TVariant val );
	TVariant funcCall( const string &id, vector<TVariant> &prms );

    private:
	//Attributes
	bool mIsArch;
	union {
	    AutoHD<TVArchive> *mArch;
	    TValBuf *mBuf;
	};
	string	mUser;
};

//*************************************************
//* Value archive open                            *
//*************************************************
class vArh : public TFunction
{
    public:
	vArh( ) : TFunction("vArh",SSPC_ID) {
	    ioAdd(new IO("res",_("Result"),IO::Object,IO::Return));
	    ioAdd(new IO("name",_("Name"),IO::String,IO::Default));
	}

	string name( )	{ return _("Val: Archive"); }
	string descr( )	{ return _("Getting the value archive object (VArchObj) through connecting to the archive at its address."); }

	void calc( TValFunc *val ) {
	    VArchObj *obj = new VArchObj(val->user());
	    obj->open(val->getS(1));
	    val->setO(0, obj);
	}
};

//*************************************************
//* Value buffer open                             *
//*************************************************
class vArhBuf : public TFunction
{
    public:
	vArhBuf( ) : TFunction("vArhBuf", SSPC_ID) {
	    ioAdd(new IO("res",_("Result"),IO::Object,IO::Return));
	    ioAdd(new IO("tp",_("Values type (0-Boolean, 1-Integer, 4-Real, 5-String)"),IO::Integer,IO::Default,"1"));
	    ioAdd(new IO("sz",_("Maximum buffer size"),IO::Integer,IO::Default,"100"));
	    ioAdd(new IO("per",_("Values period, microseconds"),IO::Integer,IO::Default,"1000000"));
	    ioAdd(new IO("hgrd",_("Hard grid"),IO::Boolean,IO::Default,"0"));
	    ioAdd(new IO("hres",_("High time resolution (microseconds)"),IO::Boolean,IO::Default,"0"));
	}

	string name( )	{ return _("Val: Archive buffer"); }
	string descr( )	{ return _("Getting a value buffer object (VArchObj) for temporary values storing and performing the intermediate operations on frames of data."); }

	void calc( TValFunc *val ) {
	    VArchObj *obj = new VArchObj(val->user());
	    obj->open((TFld::Type)val->getI(1), val->getI(2), val->getI(3), val->getB(4), val->getB(5));
	    val->setO(0, obj);
	}
};

}

#endif //VARCHFNC_H
