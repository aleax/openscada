#ifndef TTIPCONTROLLER_H
#define TTIPCONTROLLER_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tmodule.h"
#include "tvalue.h"
#include "tconfig.h"

class TController;
class TModule;

class TTipParam : public TConfigElem
{
    public:
	TTipParam( string name, string descr, string bd ) : _name(name), _descr(descr), _bd(bd) { };

	string Name()  { return(_name); }
	string Descr() { return(_descr); }
	string BD()    { return(_bd); }
    private:
	string _name;
	string _descr;
	string _bd;
};

class TTipController : public TModule, public TConfigElem
{
    /** Public methods: */
    public:
	friend class TController;
	
	TTipController( );
	virtual ~TTipController();
    
    	/*
       	 * List controllers 
	 */
	void List( vector<string> & List );
	int NameToHd( string Name );
	TController &at( unsigned int id_hd );
	TController &operator[]( unsigned id_hd ){ return( at(id_hd) ); }	
	unsigned Add( string name, string t_bd, string n_bd, string n_tb);
	void Del( unsigned id );
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
	//int                   idmod;   // ID module into TGRPModule
    /** Protected methods: */
    protected: 
	virtual TController *ContrAttach(string name, string t_bd, string n_bd, string n_tb)
	{ throw TError("%s: Error controller %s attach!",o_name,name.c_str()); }
    /** Protected atributes: */
    private:    
	vector<TTipParam *>   paramt;  // List type parameter and Structure configs of parameter.
	vector<TValueElem *>  val_el;  // Value types for value of parameter            
	vector<TController *> contr;   // List controller      !! move to private
	vector< int >         hd;      // Headers for short access to controllers

	static SCfgFld        Elem_Ctr[];
	static SCfgFld        Elem_TPrm[];

	static const char     *o_name;
    /** Private methods: */
    private:
	unsigned Size() { return(contr.size()); }
	
	int HdIns( int id );
	void HdFree( int id );
	void HdChange( int id1, int id2 );
};

#endif // TTIPCONTROLLER_H

