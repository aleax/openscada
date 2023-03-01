
//OpenSCADA module Protocol.Tmpl file: module.h
/***************************************************************************
 *   Copyright (C) 2022 by MyName MyFamily, <my@email.org>                 *
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

// Preventing of the header file multi-including - change at the header file name changing
#ifndef MODULE_H
#define MODULE_H

// System includings - add need ones
#include <string>

// OpenSCADA API includings - add need ones
#include <tprotocols.h>

// Definition of the individual module translation functions - do not change
#undef _
#define _(mess) mod->I18N(mess).c_str()
#undef trS
#define trS(mess) mod->I18N(mess,mess_PreSave)

using std::string;
using namespace OSCADA;

// All the module objects in own (individual) namespace - change for your module
namespace PrtTmpl
{

// Definition the input transport protocol object of the subsystem "Transport protocols" - add methods and attributes at your need
//*************************************************
//* PrtTmpl::TProtIn                              *
//*************************************************
class TProtIn: public TProtocolIn
{
    public:
	//Methods
	TProtIn( string name );
	~TProtIn( );

	bool mess( const string &request, string &answer );
};

// Definition the root module object of the subsystem "Transport protocols" - add methods and attributes at your need
//*************************************************
//* PrtTmpl::TProt                                *
//*************************************************
class TProt: public TProtocol
{
    public:
	//Methods
	TProt( string name );
	~TProt( );

	void outMess( XMLNode &io, TTransportOut &tro );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	TProtocolIn *in_open( const string &id );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process
};

extern TProt *mod;	//The module root link

} //End the namespace PrtTmpl

#endif //MODULE_H
