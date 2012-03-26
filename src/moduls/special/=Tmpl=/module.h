
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module Special.Tmpl file: module.h
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily                                 *
 *   my@email.org                                                          *
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

//!!! Multi-including this header file prevent. Change for your include file name
#ifndef MODULE_H
#define MODULE_H

//!!! System's includings. Add need for your module includings.
#include <string>
#include <vector>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tspecials.h>

//!!! Individual module's translation function define. Not change it!
#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

//!!! All module's object's include into self (individual) namespace. Change namespace for your module.
namespace ModTmpl
{

//!!! Root module's object define. Add methods and attributes for your need.
//*************************************************
//* ModTmpl::Lib				  *
//*************************************************
class Lib : public TSpecial
{
    public:
	//Methods
	//!!! Constructor for input transport object.
	Lib( string src );
	//!!! Destructor for input transport object.
	~Lib( );

	//!!! Module's start and stop call. Place code for internal objects start and stop.
	void modStart( );
	void modStop( );

    private:
	//Methods
	//!!! Module's comandline options for print help function.
	string optDescr( );

	//!!! Module's post enable call. Add your module objects initialize code.
	void postEnable( int flag );
	//!!! OpenSCADA control interface comands process virtual function.
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
};

extern Lib *mod;

} //End namespace ModTmpl

#endif //MODULE_H
