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
	    void HttpGet(string &url, string &page, string &sender, vector<string> &vars );
       	    void get_info( string &url, string &page, TContr &cntr, string path, string ses_user );
	    void get_cfg( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user );
	    void get_branch( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string &path, string ses_user );
	    void get_cmd( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string &path, string ses_user ); 
	    void get_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user );
	    char get_auth( string &page );
	    string get_cookie( string name, vector<string> &vars );
	    
	    void HttpPost(string &url, string &page, string &sender, vector<string> &vars, string &contein );
	    int  post_info( string &url, string &page, TContr &cntr, string path, string ses_user, string &contein );
	    int  post_auth( string &page, string &contein, string &user );
	    int  post_cfg( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, string &contein );
	    int  post_cmd( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, string &contein );
	    bool post_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, string &contein );
	    bool prepare_val( XMLNode &root, XMLNode &node, TContr &cntr, string &page, string ses_user, string &contein, bool compare );
	    
	    void cont_frm_data( const string &content, vector<string> &name, vector<string> &val );
	    bool chk_access( XMLNode *fld, string user, char mode );
	    // Sesion manipulation function	    
	    int open_ses( string name );
	    string check_ses( int id );
	    
	    void pr_opt_descr( FILE * stream );	
	    void mod_UpdateOpt();
	    string mod_info( const string name );
	    void   mod_info( vector<string> &list );
	private:
	    static SExpFunc ExpFuncLc[];

	    int             m_res;
	    vector<SAuth *> m_auth;
	    int             m_t_auth;          //Time of sesion life (minutes)

	    static char *w_head;
	    static char *w_head_;
	    static char *w_body;
	    static char *w_body_; 
    };    
}

#endif //WEB_CFG_H
