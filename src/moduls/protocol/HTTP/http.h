
//OpenSCADA system module Protocol.HTTP file: http.h
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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
 
#ifndef HTTP_H
#define HTTP_H

#include <string>

#include <tprotocols.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;

namespace PrHTTP
{

//*************************************************
//* TProtIn                                       *
//*************************************************
class TProtIn: public TProtocolIn
{
    public:
	//Methods
	TProtIn( string name );
	~TProtIn( );

	bool mess( const string &request, string &answer, const string &sender );
    
    private:
	//Methods
	void index( string &answer );
	
	string http_head( const string &rcode, int cln, const string &addattr = "" );
	string w_head( );
	string w_tail( );
	
	//Attributes
	bool m_nofull;
	string m_buf;
};

//*************************************************
//* TProt                                         *
//*************************************************
class TProt: public TProtocol
{
    public:
	//Methods
	TProt( string name );
	~TProt();
	
	void modLoad( );

    private:
	//Methods
	string optDescr( );
	TProtocolIn *in_open( const string &name );
};

extern TProt *mod;
} //End namespace PrHTTP

#endif //HTTP_H

