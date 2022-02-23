
//OpenSCADA file: tsubsys.h
/***************************************************************************
 *   Copyright (C) 2003-2022 by Roman Savochenko, <roman@oscada.org>       *
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
	TSubSYS( const string &id, bool mod = false );
	virtual ~TSubSYS( );

	string objName( );

	string subId( ) const	{ return mId.c_str(); }
	virtual string subName( ) const { return subId(); }
	bool subStartStat( )	{ return mStart; }

	bool subModule( ) const	{ return mModSys; }	//Module subsystem

	virtual int subVer( ) const { return 0; }	//Type/grp module version

	virtual void unload( );

	virtual void subStart( );
	virtual void subStop( );

	// Modules
	void modList( vector<string> &list );
	bool modPresent( const string &name );
	void modAdd( TModule *modul );
	void modDel( const string &name );
	AutoHD<TModule> modAt( const string &name ) const;

	virtual void perSYSCall( unsigned int cnt );
	virtual bool rdProcess( XMLNode *reqSt = NULL )	{ return false; }	//Main redundancy processing function in the subsystem specific

	virtual string optDescr( );

	TSYS &owner( ) const;

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Private attributes
	bool	mStart;

    private:
	//Private methods
	const char *nodeName( ) const	{ return mId.c_str(); }
	string nodeNameSYSM( ) const	{ return subName(); }

	//Private attributes
	bool	mModSys;
	int	mMod;

	string	mId;		//Id
};

}

#endif	// TGRPMODULE_H
