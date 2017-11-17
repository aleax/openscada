
//OpenSCADA system module Archive.FSArch file: base.h
/***************************************************************************
 *   Copyright (C) 2003-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef BASE_H
#define BASE_H

#include <tmodule.h>
#include <xml.h>
#include <tarchives.h>

#include "val.h"
#include "mess.h"

#undef _
#define _(mess) mod->I18N(mess)

#define CHECK_ARH_PER 60

//*************************************************
//* Info of the module                            *
#define MOD_ID		"FSArch"
#define MOD_NAME	_("Arhivator on the file system")
#define MOD_TYPE	SARH_ID
#define VER_TYPE	SARH_VER
#define MOD_VER		"2.11.1"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("The archiver module. Provides functions for messages and values archiving to file system.")
#define LICENSE		"GPL2"
//*************************************************

using namespace OSCADA;

namespace FSArch
{

//*************************************************
//* FSArch::ModArch                               *
//*************************************************
class ModArch: public TTypeArchivator
{
    public:
	ModArch( const string &name );
	~ModArch( );

	AutoHD<ModMArch> messAt( const string &iid )	{ return TTypeArchivator::messAt(iid); }
	AutoHD<ModVArch> valAt( const string &iid )	{ return TTypeArchivator::valAt(iid); }

	// Packing archives
	bool filePack( const string &anm );
	string packArch( const string &anm, bool replace = true );
	string unPackArch( const string &anm, bool replace = true );

	string filesDB( );
	TElem &packFE( )	{ return elPackfl; }

	void perSYSCall( unsigned int cnt );

	// Attributes
	bool	noArchLimit;

    protected:
	//Methods
	void load_( );
	void postEnable( int flag );

	TMArchivator *AMess( const string &iid, const string &idb );
	TVArchivator *AVal( const string &iid, const string &idb );

    private:
	//Methods
	string optDescr( );

	//Attributes
	TElem	elPackfl;	//Arch files elements
};

extern ModArch *mod;
}

#endif //BASE_H

