#ifndef WEB_CFG_H
#define WEB_CFG_H

#include "../../tuis.h"

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
	    
	    void mod_CheckCommandLine( );
	public:
    
	private:
	    void down_colont( string &url, string &page, string &sender, vector<string> &vars );
	
	    void HttpGet(string &url, string &page, string &sender, vector<string> &vars );
	    void get_about( string &page );
	    void get_head( XMLNode &root, TContr &cntr, string &page, string path, string ses_user, string &sender );
       	    void get_info( string &url, string &page, TContr &cntr, string path, string ses_user, string &sender );
	    void get_area( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string path, string a_path, string ses_user );
	    void get_cmd( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string &path, string a_path, string ses_user ); 
	    bool get_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string path, string a_path, string ses_user );
	    char get_auth( string &page );
	    string get_cookie( string name, vector<string> &vars );
	    
	    void HttpPost(string &url, string &page, string &sender, vector<string> &vars, string &contein );
	    int  post_info( string &url, string &page, TContr &cntr, string path, string ses_user, string &sender, string &contein, vector<string> &vars );
	    int  post_auth( string &page, vector<string> &vars, string &contein, string &user );
	    int  post_area( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, string &sender, vector<string> &name, vector<string> &val, string path, string prs_cat, string prs_path, int level = 0 );
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
	    string url_code( string url );
	    string url_encode( string url );
	    // Sesion manipulation function	    
	    int open_ses( string name );
	    string check_ses( int id );
	    
	    string opt_descr( );	
	    void mod_UpdateOpt();
	    string mod_info( const string name );
	    void   mod_info( vector<string> &list );
	    //================== Controll functions ========================
	    void ctr_fill_info( XMLNode *inf );
	    void ctr_din_get_( string a_path, XMLNode *opt );
	    void ctr_din_set_( string a_path, XMLNode *opt );
	private:
	    static SExpFunc ExpFuncLc[];

	    int             m_res;
	    vector<SAuth *> m_auth;
	    int             m_t_auth;          //Time of sesion life (minutes)

	    static char *w_ok; 
	    static char *w_head;
	    static char *w_head_;
	    static char *w_body;
	    static char *w_body_; 
	    
	    static const char *i_cntr; 
    };    
}

#endif //WEB_CFG_H
