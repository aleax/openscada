#ifndef HTTP_H
#define HTTP_H

#include <string>
using std::string;

#include "../../tprotocols.h"

namespace pr_http
{

//================================================================
//=========== TProtIn ============================================
//================================================================
class TProtIn: public TProtocolIn
{
    public:
	TProtIn( string name, TProtocol *owner );
	~TProtIn();

	void mess( string &request, string &answer, string sender );
    private:
	void index( string &answer );
    private:
	// HTTP-reply and header, returned if request process passed!	 
        static char *ok_response;
	// HTTP-reply, header and body page for error request!
        static char *bad_request_response;
	// HTTP-reply, header and template of page no found!
        static char *not_found_response_template;
	// HTTP-reply, header and template of page no avoid method!
        static char *bad_method_response_template;	
	// Info page
	static char *w_head;
	static char *w_head_;
	static char *w_body;
	static char *w_body_;
};

//================================================================
//=========== TProt ==============================================
//================================================================
class TProt: public TProtocol
{
    public:
	TProt( string name );
	~TProt();

	//void in_mess(string &request, string &answer );
	
	void mod_CheckCommandLine( );
	void mod_UpdateOpt(  );
    public:

    private:
	void pr_opt_descr( FILE * stream );
	TProtocolIn *in_open( string name );
};

} //End namespace pr_http
#endif //HTTP_H

