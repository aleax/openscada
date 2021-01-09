
//OpenSCADA file: tuis.h
/***************************************************************************
 *   Copyright (C) 2003-2020 by Roman Savochenko, <roman@oscada.org>       *
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

#define SUI_VER		20	//UIS type modules version
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
	enum GetOpts { GetPathURL = 1, GetContent = 2 };

	//Methods
	TUIS( );

	int subVer( )		{ return SUI_VER; }

	AutoHD<TUI> at( const string &name ) const	{ return modAt(name); }

	// All system UI functions
	static string icoGet( const string &inm, string *tp = NULL, bool retPath = false );
	// Return the EXEC command by default to open the document
	static string docGet( const string &inm, string *tp = NULL, unsigned opt = 0 );
	static string docKeyGet( const string &itxt );

	static string mimeGet( const string &inm, const string &fDt, const string &orig = "" );

    protected:
	void load_( );

    private:
	string optDescr( );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
};

}

#endif // TUIS_H
