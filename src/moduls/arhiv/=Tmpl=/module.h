
//OpenSCADA module Archive.Tmpl file: module.h
/***************************************************************************
 *   Copyright (C) 2022 by MyName MyFamily, <my@email.org>                 *
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

// Preventing of the header file multi-including - change at the header file name changing
#ifndef MODULE_H
#define MODULE_H

// System includings - add need ones
#include <string>

// OpenSCADA API includings - add need ones
#include <tarchives.h>

// Own includings of the module - add need ones
#include "val.h"
#include "mess.h"

// Definition of the individual module translation functions - do not change
#undef _
#define _(mess) mod->I18N(mess).c_str()
#undef trS
#define trS(mess) mod->I18N(mess,mess_PreSave)

using namespace OSCADA;

// All the module objects in own (individual) namespace - change for your module
namespace ArhTmpl
{

// Definition the root module object of the subsystem "Archives-History" - add methods and attributes at your need
//*************************************************
//* ArhTmpl::ModArch                              *
//*************************************************
class ModArch: public TTypeArchivator
{
    public:
	ModArch( const string &name );
	~ModArch();

	AutoHD<ModMArch> messAt( const string &iid )	{ return TTypeArchivator::messAt(iid); }
	AutoHD<ModVArch> valAt( const string &iid )	{ return TTypeArchivator::valAt(iid); }

    protected:
	//Methods
	void postEnable( int flag );

	TMArchivator *AMess( const string &id, const string &db );
	TVArchivator *AVal( const string &id, const string &db );
};

extern ModArch *mod;	//The module root link

} //End the namespace ArhTmpl

#endif //MODULE_H
