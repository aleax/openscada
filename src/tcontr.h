#ifndef TCNTR_H
#define TCNTR_H

#include <string>
#include <vector>

#include "terror.h"

using std::string;
using std::vector;

class XMLNode;

class TContr
{
    public:
	TContr( const char *i_tmpl );
	~TContr();

	// Get control info
	XMLNode *ctr_info();
	
	// Change of control dinamics from <opt>
	virtual void ctr_din_set( XMLNode *opt ){ };
	virtual void ctr_din_get( XMLNode *opt ){ };
	//========== Options manipulation ===================================
	//void ctr_opts_apply( XMLNode &inf );
	XMLNode *ctr_opt( XMLNode *inf, unsigned numb );  //scan options node
	XMLNode *ctr_id( XMLNode *inf, string name_id );  //get node for he individual number
	// Check fld valid
	string chk_opt_val( XMLNode *fld, bool fix = false );
	// Get option's values
	string ctr_opt_getS( XMLNode *fld );      //string
	int    ctr_opt_getI( XMLNode *fld );      //integer
	double ctr_opt_getR( XMLNode *fld );      //real
	bool   ctr_opt_getB( XMLNode *fld );      //boolean
	// Set option's values	
	void ctr_opt_setS( XMLNode *fld, string val );    //string
	void ctr_opt_setI( XMLNode *fld, int val );       //integer
	void ctr_opt_setR( XMLNode *fld, double val );    //real
	void ctr_opt_setB( XMLNode *fld, bool val );      //boolean
	
	//========== Commands manipulation ===================================	
	virtual void ctr_cmd_go( XMLNode *fld ){};        // Command go 
	
	//========== Branchs manipulation ===================================
	//---------- att mode ------------------
	virtual unsigned ctr_att( XMLNode *br )
	{ throw TError("%s: Function \"ctr_att\" no support!",o_name); }
	virtual void ctr_det( unsigned hd )
	{ throw TError("%s: Function \"ctr_det\" no support!",o_name); }
	virtual TContr &ctr_at( unsigned hd )
	{ throw TError("%s: Function \"ctr_at\" no support!",o_name); }
	//---------- at mode ------------------
	virtual TContr &ctr_at( XMLNode *br )	
	{ throw TError("%s: Function \"ctr_at\" no support!",o_name); }
    protected:
	virtual void ctr_fill_info( XMLNode *inf ){ };
	//========== Branchs manipulation ===================================
	void ctr_br_putlist( XMLNode *inf, vector<string> &list ); //put branchs list to <inf>
    private:
	
    private:
        const char *m_inf;
        static const char *o_name;	
};

#endif //TCNTR_H

