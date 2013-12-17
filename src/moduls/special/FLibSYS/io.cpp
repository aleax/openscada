
//OpenSCADA system module Special.FLibSYS file: io.cpp
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

#include "io.h"

using namespace FLibSYS;

//*************************************************
//* IOObj object (stream, file)                   *
//*************************************************
IOObj::IOObj( const string &nm, const string &perm, const string &mchFormt, const string &encIn ) : fhd(NULL), pos(0)
{
    open(nm, perm, mchFormt, encIn);
}

IOObj::~IOObj( )
{
    close();
}

bool IOObj::open( const string &nm, const string &perm, const string &mchFormt, const string &encIn )
{
    //Try for file open
    if(perm.size()) fhd = fopen(nm.c_str(), perm.c_str());
    else { str = nm; pos = 0; }
}

void IOObj::close( )
{
    if(fhd) fclose(fhd);
    str = ""; pos = 0;
}

TVariant IOObj::propGet( const string &id )
{
    if(id == "length")
    {
	if(!fhd) return (int)str.size();
	pos = ftell(fhd);
	fseek(fhd, 0, SEEK_END);
	int end = ftell(fhd);
	fseek(fhd, pos, SEEK_CUR);
	return end;
    }
    if(id == "pos") return fhd ? (int)ftell(fhd) : (int)str.size();

    throw TError("IOObj", _("Properties no supported by the object."));
}

void IOObj::propSet( const string &id, TVariant val )
{
    if(id == "pos")
    {
	if(fhd)	fseek(fhd, pos, SEEK_CUR);
	else pos = vmin(str.size(),vmax(0,val.getI()));
    }

    throw TError("IOObj", _("Properties no supported by the object."));
}

TVariant IOObj::funcCall( const string &id, vector<TVariant> &prms )
{
    // string read(string valType, int cnt, string mchFmtEnc = "n") -
    //		read value <dtType> in <cnt> for machine format or string encodeIn <mchFmtEnc>
    //  valType - value type: '|s|str'
    //  cnt - values by data type counter; for no strings and multiply counter used Array as result;
    //  mchFmtEnc - machine format or encodeIn for string
    if(id == "read" && prms.size() >= 2)
    {
    
	return 0;
    }
    if(id == "write")
    {
    
	return 0;
    }

    throw TError("IOObj",_("Function '%s' error or not enough parameters."),id.c_str());
}
