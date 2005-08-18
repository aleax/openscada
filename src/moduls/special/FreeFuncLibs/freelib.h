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

#ifndef FREELIB_H
#define FREELIB_H

#include <string>
#include <vector>

#include <tfunctions.h>

using std::string;
using std::vector;

namespace FreeFunc
{    
//Free library
class Libs;

class Lib : public TConfig, public ::TLibFunc
{
    public:
	Lib( const char *id, Libs *own, const char *name = "" );
	~Lib();

	string name()	{ return m_name; }
        string descr()	{ return m_descr; }

	void load( );
        void save( );
	
	void copyFunc( const string &f_id, const string &l_id, const string &to_id, const string &to_name );
	
	TBDS::SName     BD();

	Libs &owner();
	
    protected:
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );

    private:
	string	&m_name;
	string	&m_descr;
	string	&m_bd_tp;
	string	&m_bd_nm;
	string	&m_bd_tbl;
	Libs	*m_owner;
};

} //End namespace FreeLib

#endif //FREELIB_H

