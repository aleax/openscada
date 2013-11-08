
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module Protocol.Tmpl file: module.h
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
#include <map>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tprotocols.h>

//!!! Individual module's translation function define. Don't change it!
#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::map;
using namespace OSCADA;

//!!! All module's objects you must include into self (individual) namespace. Change namespace for your module.
namespace ModTmpl
{

//!!! Input protocol realisation object define. Add methods and attributes for your need.
//*************************************************
//* ModTmpl::TProtIn                              *
//*************************************************
class TProtIn: public TProtocolIn
{
    public:
	//Methods
	//!!! Constructor for input protocol object.
	TProtIn( string name );
	//!!! Destructor for input protocol object.
	~TProtIn( );

	//!!! The inherited (virtual) function of process input and preparint output data for called input transport
	bool mess( const string &request, string &answer );
};

//!!! Root module object define. Add methods and attributes for your need.
//*************************************************
//* ModTmpl::TProt                                *
//*************************************************
class TProt: public TProtocol
{
    public:
	//Methods
	//!!! Constructor for root module object.
	TProt( string name );
	//!!! Destructor for root module object.
	~TProt( );

    protected:
	//Methods
	//!!! Inherited (virtual) load and save object's node methods. Call from OpenSCADA kernel.
	void load_( );
	void save_( );

    private:
	//Methods
	//!!! Module's comandline options for print help function.
	string optDescr( );
	//!!! Main subsystem API function for self modules input protocol object creation.
	TProtocolIn *in_open( const string &name );

	//!!! OpenSCADA control interface comands process virtual function.
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

};

//!!! The module root link
extern TProt *mod;

} //End namespace ModTmpl

#endif //MODULE_H
