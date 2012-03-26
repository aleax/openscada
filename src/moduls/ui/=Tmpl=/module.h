
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module UI.Tmpl file: module.h
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

using std::string;
using std::vector;
using namespace OSCADA;

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tuis.h>

//!!! Individual module's translation function define. Not change it!
#undef _
#define _(mess) mod->I18N(mess)

//!!! All module's object's include into self (individual) namespace. Change namespace for your module.
namespace ModTmpl
{

//!!! Root module's object define. Add methods and attributes for your need.
//************************************************
//* TWEB                                         *
//************************************************
class TWEB: public TUI
{
    public:
	//Methods
	//!!! Constructor for module's root object.
	TWEB( string name );
	//!!! Destructor for module's root object.
	~TWEB( );

	//!!! Individual module's get methods
	int nCol( )			{ return n_col; }
	int hSize( )			{ return h_sz; }
	int vSize( )			{ return v_sz; }
	int trndLen( )			{ return trnd_len; }
	int trndTm( )			{ return trnd_tm; }

	//!!! Individual module's set methods
	void setNCol( int vl )		{ n_col = vl; modif(); }
	void setHSize( int vl )		{ h_sz = vl; modif(); }
	void setVSize( int vl )		{ v_sz = vl; modif(); }
	void setTrndLen( int vl )	{ trnd_len = vl; modif(); }
	void setTrndTm( int vl )	{ trnd_tm = vl; modif(); }

    protected:
	//Methods
	//!!! Inherited (virtual) load and save object's node methods. Call from OpenSCADA kernel.
	void load_( );
	void save_( );

    private:
	//Methods
	//!!! Registered your module's export functions.
	void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars );
	void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein );

	//!!! Module's comandline options for print help function.
	string optDescr( );
	//!!! Append module's info attributes create and get functions.
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );

	//!!! OpenSCADA control interface comands process virtual function.
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	//!!! Your module self functions.
	string http_head( const string &rcode, int cln, const string &cnt_tp = "text/html", const string &addattr = "" );
	string w_head( );
	string w_tail( );

	//!!! Your module self attributes.
	//Attributes
	int	n_col,		//Columns number
		h_sz,		//Horizontal trend size
		v_sz,		//Vertical trend size
		trnd_len,	//Trand length (sek)
		trnd_tm;	//Trand back time offset (sec)

	vector<string>	trnd_lst;
};

//!!! Define shortcut for module's root object for fast call from other module's objects.
extern TWEB *mod;
}

#endif //MODULE_H
