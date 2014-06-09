
//OpenSCADA system file: tsubsys.h
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef TGRPMODULE_H
#define TGRPMODULE_H

#include <string>
#include <vector>

#include "tmodule.h"

using std::string;
using std::vector;

namespace OSCADA
{

//*************************************************
//* TSubSYS                                       *
//*************************************************
class TModSchedul;
class TSYS;

class TSubSYS : public TCntrNode
{
    public:
	//Public methods
	TSubSYS( const char *id, const char *name, bool mod = false );
	virtual ~TSubSYS( );

	string objName( );

	string	subId( )	{ return m_id.c_str(); }
	string	subName( );
	bool subStartStat( )	{ return mStart; }

	bool subModule( )	{ return m_mod_sys; }	//Module subsystem

	virtual int subVer( )	{ return 0; }		//Type/grp module version

	//> Start procedures
	virtual void subStart( );
	virtual void subStop( );

	//> Modules
	void modList( vector<string> &list );
	bool modPresent( const string &name );
	void modAdd( TModule *modul );
	void modDel( const string &name );
	AutoHD<TModule> modAt( const string &name );

	virtual void perSYSCall( unsigned int cnt );

	TSYS &owner( );

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	//Private attributes
	bool	mStart;

    private:
	//Private methods
	const char *nodeName( )	{ return m_id.c_str(); }

	//Private attributes
	bool	m_mod_sys;
	int	m_mod;

	string	m_id;		//Id
	string	m_name;		//Name
};

}

#endif // TGRPMODULE_H
