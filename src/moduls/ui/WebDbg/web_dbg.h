
//OpenSCADA system module UI.WebDbg file: web_dbg.h
/***************************************************************************
 *   Copyright (C) 2004-2006 by Roman Savochenko                           *
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

#include <string>
#include <vector>

#include <tuis.h>

using std::string;
using std::vector;

namespace WebDbg
{
    class TWEB: public TUI
    {
	public:
	    //Methods
	    TWEB( string name );
	    ~TWEB();

	    void modLoad( );
	    void modSave( );
    
	private:
	    //Methods
	    void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars );
	    void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein );
	    
	    string optDescr( );
	    string modInfo( const string &name );
	    void   modInfo( vector<string> &list );
 
	    void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
	    string http_head( const string &rcode, int cln, const string &cnt_tp = "text/html", const string &addattr = "" );
	    string w_head( );
            string w_tail( );
	    
	    //Attributes
	    int	h_sz, 		//Horizontal trend size
		v_sz, 		//Vertical trend size		
		trnd_len,	//Trand length (sek)
		trnd_tm;	//Trand back time offset (sec)
	    
	    vector<string>	trnd_lst;
    };    
}
#endif //WEB_DBG_H
