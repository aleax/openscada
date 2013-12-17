
//OpenSCADA system module Special.FLibSYS file: gd.h
/***************************************************************************
 *   Copyright (C) 2013 by Roman Savochenko                                *
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

#ifndef GD_H
#define GD_H

#include <tfunction.h>
#include <tvariant.h>

#include "statfunc.h"

namespace FLibSYS
{

//*************************************************
//* GDObj object (stream, file)			  *
//*************************************************
class GDObj : public TVarObj
{
    public:
        //Methods
        GDObj( const string &inm, bool file = false );
        ~GDObj( );

        string objName( )       { return "GD"; }

        bool load( const string &inm, bool file = false );
        string save( const string &inm, bool file = false );

        TVariant propGet( const string &id );
        void propSet( const string &id, TVariant val );
        TVariant funcCall( const string &id, vector<TVariant> &prms );

    private:
	//Attributes
};

//*************************************************
//* GD object creation function			  *
//*************************************************
class GD : public TFunction
{
    public:
	GD( ) : TFunction("GD", SSPC_ID)
	{
	    ioAdd(new IO("rez",_("Result"),IO::Object,IO::Return));
	    ioAdd(new IO("name",_("Name"),IO::String,IO::Default));
	    ioAdd(new IO("file",_("From file"),IO::Boolean,IO::Default,"0"));
	}

	string name( )	{ return _("LibGD2"); }
	string descr( )	{ return _("Graphical library GD2 images processing."); }

	void calc( TValFunc *val )	{ val->setO(0, new GDObj(val->getS(1),val->getB(2))); }
};

}

#endif //GD_H
