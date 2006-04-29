
//OpenSCADA system module Protocol.HTTP file: http.h
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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
 
#ifndef HTTP_H
#define HTTP_H

#include <string>
using std::string;

#include <tprotocols.h>

namespace pr_http
{

//================================================================
//=========== TProtIn ============================================
//================================================================
class TProtIn: public TProtocolIn
{
    public:
	TProtIn( string name );
	~TProtIn();

	bool mess( const string &request, string &answer, const string &sender );
    private:
	void index( string &answer );
	
	string w_ok( );
	string w_head( );
	string w_body( );	    
    private:
	bool m_nofull;
	string m_buf;
    
	// HTTP-reply, header and body page for error request!
        static char *bad_request_response;
	// HTTP-reply, header and template of page no found!
        static char *not_found_response_template;
	// HTTP-reply, header and template of page no present method!
        static char *bad_method_response_template;	
	// Info page
	static char *w_head_;
	static char *w_body_;
};

//================================================================
//=========== TProt ==============================================
//================================================================
class TProt: public TProtocol
{
    public:
	TProt( string name );
	~TProt();
	
	void modLoad( );
	
    public:

    private:
	string optDescr( );
	TProtocolIn *in_open( const string &name );
};

} //End namespace pr_http
#endif //HTTP_H

