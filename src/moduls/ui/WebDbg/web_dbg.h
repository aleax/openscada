
//OpenSCADA system module UI.WebDbg file: web_dbg.h
/***************************************************************************
 *   Copyright (C) 2004-2007 by Roman Savochenko                           *
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

#ifndef WEB_DBG_H
#define WEB_DBG_H

#include <string>
#include <vector>

#include <tuis.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;

namespace WebDbg
{

//************************************************
//* TWEB                                         *
//************************************************
class TWEB: public TUI
{
    public:
	//Methods
	TWEB( string name );
	~TWEB( );

	int nCol( )			{ return n_col; }
	int hSize( )			{ return h_sz; }
	int vSize( )			{ return v_sz; }
	int trndLen( )			{ return trnd_len; }
	int trndTm( )			{ return trnd_tm; }
	
	void setNCol( int vl )		{ n_col = vl; modif(); }
	void setHSize( int vl )		{ h_sz = vl; modif(); }
	void setVSize( int vl )		{ v_sz = vl; modif(); }
	void setTrndLen( int vl )	{ trnd_len = vl; modif(); }
	void setTrndTm( int vl )	{ trnd_tm = vl; modif(); }

    protected:
	//Methods
	void load_( );
	void save_( );
    
    private:
	//Methods
	void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars );
	void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein );
	    
	string optDescr( );
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );
 
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	string http_head( const string &rcode, int cln, const string &cnt_tp = "text/html", const string &addattr = "" );
	string w_head( );
	string w_tail( );
	    
	//Attributes
	int	n_col,		//Columns number
	    	h_sz, 		//Horizontal trend size
	    	v_sz, 		//Vertical trend size		
		trnd_len,	//Trand length (sek)
		trnd_tm;	//Trand back time offset (sec)
	    
	vector<string>	trnd_lst;
};    
    
extern TWEB *mod;
}
#endif //WEB_DBG_H
