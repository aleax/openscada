#ifndef TTIPCONTROLLER_H
#define TTIPCONTROLLER_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tmodule.h"
#include "tvalue.h"
#include "tconfig.h"
#include "tbds.h"
#include "tcontroller.h"

class TModule;
class TTipParam;

class TTipController : public TModule, public TConfigElem
{
    /** Public methods: */
    public:
	friend class TController;
	
	TTipController( );
	virtual ~TTipController();
    
	/*
	 * Avoid controllers list
	 */
	void list( vector<string> &list )
	{ m_hd_cntr.hd_obj_list( list ); }
	/*
	 * Add controller
	 */
    	void add( string name, SBDS &bd );
	/*
	 * Del controller
	 */
	void del( string name )
	{ delete (TController *)m_hd_cntr.hd_obj_del( name ); }
	/*
	 * Attach to controller
	 * Return controller header
	 */
	unsigned att( string name )
	{ return( m_hd_cntr.hd_att( name ) ); }
	/*
	 * Detach from controller
    	 */
	void det( unsigned hd )
	{ m_hd_cntr.hd_det( hd ); }
	/*
	 * Get attached controller
         */
	TController &at( unsigned hd )
	{ return( *(TController *)m_hd_cntr.hd_at( hd ) ); }
	TController &operator[]( unsigned hd ){ return( at(hd) ); }	
	
	void LoadCfg( SCfgFld *elements, int numb );
	
	unsigned NameTpPrmToId(string name_t);
	unsigned SizeTpPrm( ) { return( paramt.size()); }
	TTipParam &at_TpPrm( unsigned id )
	{ if(id >= paramt.size()) throw TError("%s: id of param type error!",o_name); return( *paramt[id]); }
	int AddTpParm(string name_t, string n_fld_bd, string descr);
	int LoadTpParmCfg(string name_t_prm, SCfgFld *elements, int numb );

	void ListTpVal( vector<string> & List );
	void AddTpVal(string name, SVAL *vl_el, int number);
	TValueElem &at_TpVal( string name);
    /** Public atributes: */
    public:
    /** Protected methods: */
    protected: 
	virtual TController *ContrAttach(string name, string t_bd, string n_bd, string n_tb)
	{ throw TError("%s: Error controller %s attach!",o_name,name.c_str()); }
    /** Protected atributes: */
    private:    
	vector<TTipParam *>   paramt;  // List type parameter and Structure configs of parameter.
	vector<TValueElem *>  val_el;  // Value types for value of parameter            
	THD                   m_hd_cntr;  // List controller       

	static SCfgFld        Elem_Ctr[];
	static SCfgFld        Elem_TPrm[];

	static const char     *o_name;
    /** Private methods: */
    private:
};

#endif // TTIPCONTROLLER_H

