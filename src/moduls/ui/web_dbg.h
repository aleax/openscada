#ifndef WEB_DBG_H
#define WEB_DBG_H

#include "../../tuis.h"

namespace WebDbg
{
    class TWEB: public TUI
    {
	public:
	    TWEB( string name );
	    ~TWEB();
	
	    void mod_CheckCommandLine( );
	public:
    
	private:
	    void HttpGet(string &url, string &page, string &sender, vector<string> &vars );
	    void HttpPost(string &url, string &page, string &sender, vector<string> &vars, string &contein );
	    
	    void pr_opt_descr( FILE * stream );	
	    string mod_info( const string name );
	    void   mod_info( vector<string> &list );
	private:
	    static SExpFunc ExpFuncLc[];	

	    static char *w_head;	
	    static char *w_head_;	
	    static char *w_body;	
	    static char *w_body_;	
    };    
}
#endif //WEB_DBG_H
