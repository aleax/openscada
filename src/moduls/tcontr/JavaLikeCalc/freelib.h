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

#ifndef FREELIB_H
#define FREELIB_H

#include <string>
#include <vector>

#include <tcntrnode.h>
#include <freefunc.h>

using std::string;
using std::vector;

namespace JavaLikeCalc
{    
//Free library
class TipContr;

class Lib : public TCntrNode, public TConfig
{
    public:
	Lib( const char *id, const char *name = "" );
	~Lib();

	string id()	{ return m_id; }
	string name()	{ return m_name; }
        string descr()	{ return m_descr; }
	
	bool startStat( )	{ return run_st; }
        void start( bool val );

	void load( );
        void save( );
	
	void list( vector<string> &ls )	{ chldList(m_fnc,ls); }
	bool present( const string &id ){ return chldPresent(m_fnc,id); }
	AutoHD<Func> at( const string &id )
        { return chldAt(m_fnc,id); }	
	void add( const char *id, const char *name = "" );
	void del( const char *id );
	
	void copyFunc( const string &f_id, const string &l_id, const string &to_id, const string &to_name );
	
	TBDS::SName     BD();

    protected:
	string nodeName()	{ return m_id; }
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
	void preDisable(int flag);
	void postDisable(int flag);

    private:
	bool 	run_st;
	int     m_fnc;
	string	&m_id, &m_name, &m_descr, &m_bd_tp, &m_bd_nm, &m_bd_tbl;
};

} //End namespace FreeLib

#endif //FREELIB_H

