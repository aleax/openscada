#ifndef WEB_INFO_H
#define WEB_INFO_H

#include "../../tspecials.h"

namespace WebInfo
{
    class TWEB: public TModule
    {
	public:
	    TWEB(char *name);
	    ~TWEB();
	
	    void mod_CheckCommandLine( );	    
	public:
    
	private:
	    void HttpGet(string &url, string &page);
	
	    void pr_opt_descr( FILE * stream );	
	    string mod_info( const string name );
	    void   mod_info( vector<string> &list );
	private:
	    static SExpFunc ExpFuncLc[];	

	    static char *mess;	
    };
}

#endif //WEB_INFO_H
