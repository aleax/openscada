
//OpenSCADA system module UI.VCAEngine file: orig_doc.h
/***************************************************************************
 *   Copyright (C) 2008 by Roman Savochenko                                *
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

#ifndef ORIG_DOC_H
#define ORIG_DOC_H

#include "origwidg.h"

namespace VCA
{

//************************************************
//* OrigDocument: Document view original widget  *
//************************************************
class OrigDocument : public PrWidget
{
    public:
	//Methods
	OrigDocument( );

	string name( );
	string descr( );

    protected:
	//Methods
	void postEnable( int flag );
	bool attrChange( Attr &cfg, void *prev );

	string makeDoc( const string &tmpl );
};

}

#endif //ORIG_DOC_H
