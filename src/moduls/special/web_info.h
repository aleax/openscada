#ifndef WEB_INFO_H
#define WEB_INFO_H

#include "../../tspecials.h"

class TSP_WEB_INFO: public TSpecial
{
    public:
	TSP_WEB_INFO(char *name);
	~TSP_WEB_INFO();
	
	void mod_CheckCommandLine( );
    public:

    private:
        void HttpGet(string &url, string &page);
	
	void pr_opt_descr( FILE * stream );	
    private:
	static SExpFunc ExpFuncLc[];	

        static char *mess;	
};

#endif //WEB_INFO_H
