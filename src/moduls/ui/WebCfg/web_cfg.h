
//OpenSCADA system module UI.WebCfg file: web_cfg.h
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

#ifndef WEB_CFG_H
#define WEB_CFG_H

#include <tuis.h>

namespace WebCfg
{
    struct SAuth
    {
	time_t t_auth;
	string name;
	int    id_ses;
    };
    
    class SSess
    {
	public:	
	    SSess( const string &iurl, const string &ipage, const string &isender, vector<string> &ivars, const string &icontent ) :
		url(iurl), page(ipage), sender(isender), vars(ivars), content(icontent) {  };
    	    string url;		//request URL 
	    string page;	
	    string sender;	//request sender 
	    string user;	//sesion user
	    XMLNode root;	//page node
	    //HTTP vars and contein
	    vector<string> vars;//request vars
	    string content;	//POST contein
	    //Parsed contein
	    vector<string> cnt_names;
	    vector<string> cnt_vals;
	    //No interrupt messages
	    vector<string> mess;
    };

    class TWEB: public TUI
    {
	public:
	    TWEB( string name );
	    ~TWEB();

	    void modLoad( );
	    void modSave( );
    
	private:
	    //Methods
	    void down_colont( SSess &ses );

	    string http_head( const string &rcode, int cln, const string &cnt_tp = "text/html", const string &addattr = "" );
	    string w_head( );
	    string w_tail( );	    
	
	    void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars );
	    void get_about( SSess &ses );
	    void get_head( SSess &ses );
	    void get_area( SSess &ses, XMLNode &node, string a_path );
	    void get_cmd( SSess &ses, XMLNode &node, string a_path ); 
	    bool get_val( SSess &ses, XMLNode &node, string a_path, bool rd = true );
	    void get_auth( SSess &ses );
	    string get_cookie( string name, vector<string> &vars );
	    
	    void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein );
	    int  post_auth( SSess &ses );
	    int  post_area( SSess &ses, XMLNode &node, const string &prs_comm, int level = 0 );
	    int  post_val( SSess &ses, XMLNode &node, string prs_path);
	    bool prepare_val( SSess &ses, XMLNode &node, string prs_path, bool compare );	    
	    int  post_cmd( SSess &ses, XMLNode &node, string prs_path );
	    int  post_list( SSess &ses, XMLNode &node, string prs_path );
	    int  post_table( SSess &ses, XMLNode &node, string prs_path );
	    // Post message dialog 
	    //   type: 1 - message, 2 - warning, 3 - error; 
	    void post_mess( string &page, const string &cat, const string &mess, int type );
	    // Parse http contein
	    void cont_frm_data( SSess &ses );
	    // chek access to fields
	    bool chk_access( XMLNode *fld, string user, char mode );
	    // Sesion manipulation function	    
	    int open_ses( string name );
	    void check_ses( SSess &ses );

	    //Get form content for name
	    string cntGet( SSess &ses, const string &nm );
	    
	    string optDescr( );	
	    string modInfo( const string &name );
	    void   modInfo( vector<string> &list );
	    
	    void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	    
	    //Attributes
	    int             m_res;
	    vector<SAuth *> m_auth;
	    int             m_t_auth;          //Time of sesion life (minutes)
    };    
}

#endif //WEB_CFG_H
