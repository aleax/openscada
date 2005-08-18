/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
 
#ifndef SELF_H
#define SELF_H

#include <tprotocols.h>

namespace pr_self
{

//================================================================
//=========== TProtIn ============================================
//================================================================
class TProtIn: public TProtocolIn
{
    public:
	TProtIn( string name, TProtocol *owner );
	~TProtIn();

	bool mess( const string &request, string &answer, const string &sender );
};

//================================================================
//=========== TProt ==============================================
//================================================================
class TProt: public TProtocol
{
    public:
	TProt( string name );
	~TProt();
	
	void modLoad( );
	
    public:

    private:
	string optDescr( );
	TProtocolIn *in_open( const string &name );
	
    private:
};

} //End namespace pr_self
#endif //SELF_H

