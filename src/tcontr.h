#ifndef TCNTR_H
#define TCNTR_H

#include <string>
#include <vector>

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
	
	//========== Options manipulation ===================================
	// Apply changed control options from <opt>
	void ctr_opts_apply( XMLNode &inf );
	XMLNode *ctr_opt( XMLNode &inf, unsigned numb );
	// Get option's values
	string ctr_opt_getS( XMLNode &inf, string name );              //string
	int    ctr_opt_getI( XMLNode &inf, string name );              //integer
	double ctr_opt_getR( XMLNode &inf, string name );              //real
	bool   ctr_opt_getB( XMLNode &inf, string name );              //boolean
	// Set option's values	
	void ctr_opt_setS( XMLNode &inf, string name, string val, bool mdf = false );    //string
	void ctr_opt_setI( XMLNode &inf, string name, int val, bool mdf = false );       //integer
	void ctr_opt_setR( XMLNode &inf, string name, double val, bool mdf = false );    //real
	void ctr_opt_setB( XMLNode &inf, string name, bool val, bool mdf = false );      //boolean
	
	//========== Commands manipulation ===================================
	// Command go
	virtual void ctr_cmd_go( string name ){};
	
	//========== Branchs manipulation ===================================
	virtual void ctr_br_add( string name ){};           //add new branch
	virtual void ctr_br_del( string name ){};           //del branch
	virtual void ctr_br_rot( string name ){};           //rotate branch 
    protected:
	virtual void ctr_fill_info( XMLNode &inf ){ };
	virtual void ctr_opt_apply( XMLNode &inf, XMLNode &opt ){ };
	//========== Branchs manipulation ===================================
	void ctr_br_putlist( XMLNode &inf, vector<string> &list ); //put branchs list to <inf>
    private:
	
    private:
        const char *m_inf;
	
};

#endif //TCNTR_H

