#ifndef SELF_H
#define SELF_H

#include "../../tprotocols.h"

namespace pr_self
{

//================================================================
//=========== TProtIn ============================================
//================================================================
class TProtIn: public TProtocolIn
{
    public:
	TProtIn( string name, TProtocol *owner );
	~TProtIn();

	bool mess( string &request, string &answer, string sender );
};

//================================================================
//=========== TProt ==============================================
//================================================================
class TProt: public TProtocol
{
    public:
	TProt( string name );
	~TProt();

	void mod_CheckCommandLine( );	
    public:

    private:
	void pr_opt_descr( FILE * stream );
	TProtocolIn *in_open( string name );
    private:
};

} //End namespace pr_self
#endif //SELF_H

