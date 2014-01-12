
//OpenSCADA system module Special.FLibSYS file: gd.cpp
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

#include "gd.h"

using namespace FLibSYS;

//*************************************************
//* GDObj object (stream, file)                   *
//*************************************************
GDObj::GDObj( const string &inm, bool file )
{

}

GDObj::~GDObj( )
{

}

bool GDObj::load( const string &inm, bool file )
{

    return true;
}

string GDObj::save( const string &inm, bool file )
{

    return "";
}

TVariant GDObj::propGet( const string &id )
{
    throw TError("GDObj", _("Properties no supported by the object."));
}

void GDObj::propSet( const string &id, TVariant val )
{
    throw TError("GDObj", _("Properties no supported by the object."));
}

TVariant GDObj::funcCall( const string &id, vector<TVariant> &prms )
{
    //????
    throw TError("GDObj",_("Function '%s' error or not enough parameters."),id.c_str());
}
