#ifndef WEB_DBG_H
#define WEB_DBG_H

#include "../../tspecials.h"

class TSP_WEB_DBG: public TSpecial
{
    public:
	TSP_WEB_DBG(char *name);
	~TSP_WEB_DBG();
	
	void mod_CheckCommandLine( );
    public:

    private:
        void HttpGet(string &url, string &page);
	
	void pr_opt_descr( FILE * stream );	
    private:
	static SExpFunc ExpFuncLc[];	

        static char *w_head;	
        static char *w_head_;	
        static char *w_body;	
        static char *w_body_;	
};

#endif //WEB_DBG_H
