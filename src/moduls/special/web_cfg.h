#ifndef WEB_CFG_H
#define WEB_CFG_H

#include "../../tspecials.h"

class TSP_WEB_CFG: public TSpecial
{
    public:
	TSP_WEB_CFG(char *name);
	~TSP_WEB_CFG();
	
	void mod_CheckCommandLine( );
    public:

    private:
        void HttpGet(string &url, string &page);
	
	void pr_opt_descr( FILE * stream );	
    private:
	static SExpFunc ExpFuncLc[];	

        static char *mess;	
};

#endif //WEB_CFG_H
