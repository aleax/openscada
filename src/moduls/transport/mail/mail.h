#ifndef MAIL_H
#define MAIL_H

#include <ttransports.h>

class TMailIn;

class TMailIn: public TTransportIn
{
    public:
	/*
	 * Open input socket <name> for locale <address>
	 * address : <type:<specific>>
	 * 	type: 
	 * 	  TCP  - TCP socket with  "UDP:<host>:<port>"
	 * 	  UDP  - UDP socket with  "TCP:<host>:<port>"
	 * 	  UNIX - UNIX socket with "UNIX:<path>"
	 */
	TMailIn(string name, TTipTransport *owner);
	~TMailIn();

    private:
	
    private:
};

class TMailOut: public TTransportOut
{
    public:
    	TMailOut(string name, TTipTransport *owner);
	~TMailOut();

    private:
};

class TTransMail: public TTipTransport
{
    public:
	TTransMail( string name );
	~TTransMail();
        
	TTransportIn  *In(string name);
	TTransportOut *Out(string name);	    
	
	void mod_CheckCommandLine( );
	void mod_UpdateOpt();	
    public:

    private:
	
	void pr_opt_descr( FILE * stream );
    private:
};

#endif //MAIL_H

