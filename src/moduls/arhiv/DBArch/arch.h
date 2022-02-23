
//OpenSCADA module Archive.DBArch file: arch.h
/***************************************************************************
 *   Copyright (C) 2007-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef ARCH_H
#define ARCH_H

#include <tarchives.h>

#include "val.h"
#include "mess.h"

#undef _
#define _(mess) mod->I18N(mess).c_str()
#undef trS
#define trS(mess) mod->I18N(mess,mess_PreSave)

using namespace OSCADA;

namespace DBArch
{

//*************************************************
//* DBArch::ModArch                               *
//*************************************************
class ModArch: public TTypeArchivator
{
    public:
	ModArch( const string &name );
	~ModArch();

	void modStart( );
	void modStop( );

	AutoHD<ModMArch> messAt( const string &iid )	{ return TTypeArchivator::messAt(iid); }
	AutoHD<ModVArch> valAt( const string &iid )	{ return TTypeArchivator::valAt(iid); }

	string mainTbl( )	{ return modId(); }

	TElem &archEl( )	{ return elArch; }

	void perSYSCall( unsigned int cnt );

    protected:
	//Methods
	void load_( );

	void postEnable( int flag );

	TMArchivator *AMess( const string &id, const string &db );
	TVArchivator *AVal( const string &id, const string &db );

    private:
	//Attributes
	TElem elArch;
};

extern ModArch *mod;
}

#endif //ARCH_H
