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

    class TWEB: public TUI
    {
	public:
	    TWEB( string name );
	    ~TWEB();
	    
	    void modCheckCommandLine( );
	public:
    
	private:
	    void down_colont( const string &url, string &page, const string &sender, vector<string> &vars );

	    string w_ok( );
	    string w_head( );
	    string w_body( );	    
	
	    void HttpGet( const string &url, string &page, const string &sender, vector<string> &vars );
	    void get_about( string &page );
	    void get_head( XMLNode &root, TContr &cntr, string &page, const string &path, const string &ses_user, const string &sender );
       	    void get_info( string &url, string &page, TContr &cntr, const string &path, const string &ses_user, const string &sender );
	    void get_area( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string path, string a_path, string ses_user );
	    void get_cmd( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string &path, string a_path, string ses_user ); 
	    bool get_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string path, string a_path, string ses_user, bool rd = true );
	    void get_auth( string &url, string &page );
	    string get_cookie( string name, vector<string> &vars );
	    
	    void HttpPost( const string &url, string &page, const string &sender, vector<string> &vars, const string &contein );
	    int  post_info( string &url, string &page, TContr &cntr, const string &path, const string &ses_user, const string &sender, const string &contein, vector<string> &vars );
	    int  post_auth( string &url, string &page, vector<string> &vars, const string &contein, string &user );
	    int  post_area( XMLNode &root, XMLNode &node, TContr &cntr, string &page, const string &ses_user, const string &sender, vector<string> &name, vector<string> &val, const string &path, const string &prs_cat, const string &prs_path, int level = 0 );
	    int  post_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, vector<string> &name, vector<string> &val, string prs_path);
	    bool prepare_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, vector<string> &names, vector<string> &vals, string prs_path, bool compare );	    
	    int  post_cmd( XMLNode &root, XMLNode &node, XMLNode &rez, TContr &cntr, string &page, string ses_user, vector<string> &names, vector<string> &vals, string prs_path );
	    int  post_list( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, vector<string> &names, vector<string> &vals, string path, string prs_path );
	    int  post_table( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, vector<string> &names, vector<string> &vals, string path, string prs_path );
	    // Post message dialog 
	    //   type: 1 - message, 2 - warning, 3 - error; 
	    void post_mess( string &page, string mess, int type );
	    // Parse http contein
	    void cont_frm_data( const string &content, vector<string> &vars, vector<string> &name, vector<string> &val );
	    // chek access to fields
	    bool chk_access( XMLNode *fld, string user, char mode );
	    // Convert messages into html
	    string mess2html( string mess );
	    // URL metods
	    string url_code( string url, bool contr = false );
	    string url_encode( const string &url, bool contr = false  );
	    // Sesion manipulation function	    
	    int open_ses( string name );
	    string check_ses( int id );
	    
	    string opt_descr( );	
	    void modUpdateOpt();
	    string modInfo( const string &name );
	    void   modInfo( vector<string> &list );
	    //================== Controll functions ========================
	    void ctr_fill_info( XMLNode *inf );
	    void ctr_din_get_( const string &a_path, XMLNode *opt );
	    void ctr_din_set_( const string &a_path, XMLNode *opt );
	private:
	    static SExpFunc ExpFuncLc[];

	    int             m_res;
	    vector<SAuth *> m_auth;
	    int             m_t_auth;          //Time of sesion life (minutes)

	    string w_head_;
	    string w_body_; 
    };    
}

#endif //WEB_CFG_H
