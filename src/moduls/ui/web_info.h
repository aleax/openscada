#ifndef WEB_INFO_H
#define WEB_INFO_H

#include "../../tuis.h"

namespace WebInfo
{
    class TWEB: public TUI
    {
	public:
	    TWEB( string name );
	    ~TWEB();
	
	    void mod_CheckCommandLine( );	    
	public:
    
	private:
	    void HttpGet( string &url, string &page );
	    void get_info( string &url, string &page, TContr &cntr, string path );
	    void get_cfg( XMLNode &node, string &page );
	    void get_branch( XMLNode &node, string &page, string &path );
	    void get_cmd( XMLNode &node, string &page );    
	
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

#endif //WEB_INFO_H
