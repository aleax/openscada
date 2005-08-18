/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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

#ifndef WEB_DBG_H
#define WEB_DBG_H

#include <tuis.h>

namespace WebDbg
{
    class TWEB: public TUI
    {
	public:
	    TWEB( string name );
	    ~TWEB();

	    void modLoad( );
    
	private:
	    void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars );
	    void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein );
	    
	    string optDescr( );
	    string modInfo( const string &name );
	    void   modInfo( vector<string> &list );
 
	private:
	    static char *w_head;	
	    static char *w_head_;	
	    static char *w_body;	
	    static char *w_body_;	
    };    
}
#endif //WEB_DBG_H
