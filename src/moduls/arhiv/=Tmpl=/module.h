
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module Archive.Tmpl file: module.h
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily                                 *
 *   my@email.org                                                          *
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

//!!! Multi-including this header file prevent. Change for your include file name
#ifndef MOD_TMPL_H
#define MOD_TMPL_H

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tarchives.h>

//!!! Self your module's includings. Add need for your module includings.
#include "val.h"
#include "mess.h"

//!!! Individual module's translation function define. Not change it!
#undef _
#define _(mess) mod->I18N(mess)

using namespace OSCADA;

//!!! All module's object's include into self (individual) namespace. Change namespace for your module.
namespace ModTmpl
{

//!!! Root module's object define. Add methods and attributes for your need.
//*************************************************
//* ModTmpl::ModArch                              *
//*************************************************
class ModArch: public TTipArchivator
{
    public:
	//!!! Constructor for root module's object.
	ModArch( const string &name );
	//!!! Destructor for root module's object.
	~ModArch();

	//AutoHD<ModMArch> messAt( const string &iid )	{ return TTipArchivator::messAt(iid); }
	//AutoHD<ModVArch> valAt( const string &iid )	{ return TTipArchivator::valAt(iid); }

    protected:
	//Methods
	//!!! Inherited (virtual) load and save object's node methods. Call from OpenSCADA kernel.
	void load_( );

	//!!! Module's post enable call. Add your module objects initialize code.
	void postEnable( int flag );

	//!!! Main subsystem API functions for self modules message and value archive objects creation.
	TMArchivator *AMess( const string &id, const string &db );
	TVArchivator *AVal( const string &id, const string &db );
};

extern ModArch *mod;

} //End namespace ModTmpl

#endif //MOD_TMPL_H
