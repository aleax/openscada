
//OpenSCADA system module Special.FLibSYS file: io.h
/***************************************************************************
 *   Copyright (C) 2013-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef IO_H
#define IO_H

#include <tfunction.h>
#include <tvariant.h>

#include "statfunc.h"

namespace FLibSYS
{

//******************************************************
//* Data parameters:
//*  Permissions: r,w,a,r+,w+,a+
//*  Machine format: native(n), ieee-be(b), ieee-le(l)
//*  Encoding input: UTF-8, KOI8-RU, CP1251, CP866, ...
//*  Value type: char, {|u}int{|8|16|32|64}, float, double

//*************************************************
//* IOObj object (stream, file)			  *
//*************************************************
class IOObj : public TVarObj
{
    public:
        //Methods
        IOObj( const string &nm, const string &perm = "", const string &mFormat = "", const string &ienc = "" );
        ~IOObj( );

        string objName( )       { return "IO"; }

        void open( const string &inm, const string &perm = "", const string &mFormat = "", const string &ienc = "" );
        void close( );

        TVariant propGet( const string &id );
        void propSet( const string &id, TVariant val );
        TVariant funcCall( const string &id, vector<TVariant> &prms );

    private:
	//Data
	class TpDescr
	{
	    public:
		TpDescr( ) : szBt(1), real(false), sign(true) { }
		TpDescr( char iSzBt, bool iReal = false, bool iSign = false ) : szBt(iSzBt), real(iReal), sign(iSign) { }

		unsigned szBt : 4;
		unsigned real : 1;
		unsigned sign : 1;
	};

	//Methods
	TpDescr	&getTp( const string &dtT );

	//Attributes
	FILE 	*fhd;			//Openned file hd. For fhd < 0 used local string into "strFnm"
	string	str, mFormat, strEnc;	//String stream
	unsigned long	pos;		//String stream current position

	map<string,TpDescr> dTPs;
};

//*************************************************
//* IO object creation function			  *
//*************************************************
class IOCall : public TFunction
{
    public:
	IOCall( ) : TFunction("IO", SSPC_ID)
	{
	    ioAdd(new IO("rez",_("Result"),IO::Object,IO::Return));
	    ioAdd(new IO("name",_("File name or data (for string stream)"),IO::String,IO::Default));
	    ioAdd(new IO("accs",_("File access (''-string stream;'r[+]'-read;'w[+]'-write from zero;'a[+]'-append;...)"),IO::String,IO::Default));
	    ioAdd(new IO("mFormat",_("Machine data format ('n'-sys order;'b'-BigEndian;'l'-LittleEndian)"),IO::String,IO::Default,"n"));
	    ioAdd(new IO("enc",_("String encoding in file"),IO::String,IO::Default));
	}

	string name( )	{ return _("IO"); }
	string descr( )	{ return _("Input/Output to: file, string stream."); }

	void calc( TValFunc *val )	{ val->setO(0, new IOObj(val->getS(1),val->getS(2),val->getS(3),val->getS(4))); }
};

}

#endif //IO_H
