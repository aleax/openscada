
#ifndef TPARAMS_H
#define TPARAMS_H

#include <string>
#include <vector>

#include "thd.h"
#include "tconfig.h"
#include "terror.h"
#include "tcontr.h"

using std::string;
using std::vector;

class SCntrS;
class TParam;
class TParamContr;

class TParamS : public TConfig, public TContr  
{
    /** Public methods: */
    public:
	TParamS( TKernel *app );
    
	~TParamS(  );

	// Avoid parameters list
	void list( vector<string> &list )
	{ m_hd.obj_list( list ); }
	// Add parameter
	void add( SCntrS cntr, string param );
	// Del parameter
	void del( SCntrS cntr, string param );
	/*
	 * Attach to parameter
	 * Return parameter header
	 */
	unsigned att( string name, string user = "" )
	{ return( m_hd.hd_att( name, user ) ); }
	// Detach from parameter
    	void det( unsigned hd )
	{ m_hd.hd_det( hd ); }
	// Get attached parameter
	TParam &at( unsigned hd )
	{ return( *(TParam *)m_hd.hd_at( hd ) ); }
	TParam &operator[]( unsigned hd ){ return( at(hd) ); }
    
	TKernel &Owner() { return( *owner ); }
    
    //Start scheduler of params
    public:
    /** Private methods: */
    private:
	void ctr_fill_info( XMLNode &inf );
	void ctr_opt_apply( XMLNode &opt );    
    /**Attributes: */
    private:
	THD               m_hd;
	
	TKernel           *owner;
    
	static SCfgFld    gen_elem[];  
	static const char *i_cntr;
	static const char *o_name;
};

#endif // TPARAMS_H
