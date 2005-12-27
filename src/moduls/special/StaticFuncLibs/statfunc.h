/***************************************************************************
 *   Copyright (C) 2005 by Roman Savochenko                                *
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

#ifndef STATFUNC_H
#define STATFUNC_H

#include <string>
#include <vector>

#include <tspecials.h>

using std::string;
using std::vector;

namespace FuncLibComplex1
{

//Complex1 functions library
class Lib : public TSpecial
{
    public:
	Lib( string src );
	~Lib();
	
	void modStart( );
        void modStop( );		
	
	void list( vector<string> &ls )         { chldList(m_fnc,ls); }
        bool present( const string &id )        { return chldPresent(m_fnc,id); }
        AutoHD<TFunction> at( const string &id )        { return chldAt(m_fnc,id); }
        void reg( TFunction *fnc )              { chldAdd(m_fnc,fnc); }
        void unreg( const char *id )            { chldDel(m_fnc,id); }					

    private:
	void postEnable( );
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
    private:
        int     m_fnc;
};

extern Lib *st_lib;

} //End namespace FuncLibComplex1

#endif //STATFUNC_H

