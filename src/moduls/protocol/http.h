#ifndef HTTP_H
#define HTTP_H

#include <string>
using std::string;

#include "../../tprotocols.h"

class TProtHttp: public TProtocol
{
    public:
	TProtHttp( string name );
	~TProtHttp();

	void in_mess(string &request, string &answer );
	
	void mod_CheckCommandLine( );
	void mod_UpdateOpt(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );

    private:
	/*
	 * HTTP-reply and header, returned if request process passed!
	 */
        static char *ok_response;
	/*
	 * HTTP-reply, header and body page for error request!
	 */
        static char *bad_request_response;
	/*
	 * HTTP-reply, header and template of page no found!
	 */
        static char *not_found_response_template;
	/*
	 * HTTP-reply, header and template of page no avoid method!
	 */
        static char *bad_method_response_template;
};

#endif //HTTP_H

