
//OpenSCADA system file: tuis.h
/***************************************************************************
 *   Copyright (C) 2003-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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


#ifndef TUIS_H
#define TUIS_H

#define SUI_VER		12	//UIS type modules version
#define SUI_ID		"UI"

#include <string>

#include "tsubsys.h"

using std::string;

namespace OSCADA
{

//*************************************************
//* TUI                                           *
//*************************************************
class TUI : public TModule
{
    public:
	TUI( const string &id );

	bool startStat( ) const	{ return runSt; }

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	bool	runSt;
};

//*************************************************
//* TUIS                                          *
//*************************************************
class TUIS : public TSubSYS
{
    public:
	//Data
	enum GetOpts { GetContent = 0, GetFilePath = 1, GetExecCommand = 2 };

	//Methods
	TUIS( );

	int subVer( )		{ return SUI_VER; }

	AutoHD<TUI> at( const string &name ) const	{ return modAt(name); }

	// All system UI functions
	static string icoGet( const string &inm, string *tp = NULL, bool retPath = false );
	static string docGet( const string &inm, string *tp = NULL, unsigned opt = GetFilePath );

    protected:
	void load_( );

    private:
	string optDescr( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
};

}

#endif // TUIS_H
