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
	void ctr_din_set( string area_path, XMLNode *opt );
	void ctr_din_get( string area_path, XMLNode *opt );
	// Path parse
	string ctr_path_l(string path, int level);
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
	void ctr_opt_setS( XMLNode *fld, string val, int id=0 );    //string
	void ctr_opt_setI( XMLNode *fld, int val, int id=0 );       //integer
	void ctr_opt_setR( XMLNode *fld, double val, int id=0 );    //real
	void ctr_opt_setB( XMLNode *fld, bool val, int id=0 );      //boolean
	
	//========== Commands manipulation ===================================	
	virtual void ctr_cmd_go( string area_path, XMLNode *fld, XMLNode *rez ){};        // Command go 
	
	//========== Branchs manipulation ===================================
	//---------- att mode ------------------
	virtual unsigned ctr_att( string br )
	{ throw TError("(%s) Function <ctr_att> no support!",o_name); }
	virtual void ctr_det( string br, unsigned hd )
	{ throw TError("(%s) Function <ctr_det> no support!",o_name); }
	virtual TContr &ctr_at( string br, unsigned hd )
	{ throw TError("(%s) Function <ctr_at> no support!",o_name); }
	//---------- at mode ------------------
	virtual TContr &ctr_at( string br )	
	{ throw TError("(%s) Function <ctr_at> no support!",o_name); }
    protected:
	virtual void ctr_fill_info( XMLNode *inf ){ };
	virtual void ctr_din_set_( string area_path, XMLNode *opt ){ };
	virtual void ctr_din_get_( string area_path, XMLNode *opt ){ };
    private:
	
    private:
        const char *m_inf;
        static const char *o_name;	
};

#endif //TCNTR_H

