#ifndef SELF_H
#define SELF_H

#include "../../tprotocols.h"

class TProtSelf: public TProtocol
{
    public:
	TProtSelf( string name );
	~TProtSelf();

	void in_mess(string &request, string &answer );
	
	void mod_CheckCommandLine( );	
    public:

    private:
	void pr_opt_descr( FILE * stream );
    private:
};

#endif //SELF_H

