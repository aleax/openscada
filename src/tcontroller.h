
#ifndef TCONTROLLER_H
#define TCONTROLLER_H

#include <time.h>
#include <string>

#include "tkernel.h"
#include "tconfig.h"
#include "tbds.h"
//#include "tparams.h"
#include "tparamcontr.h"

#define TCNTR_ENABLE  0x01   //enabled
#define TCNTR_RUN     0x02   //run
#define TCNTR_ERR     0x04   //error

using std::string;

class TTipController;

class TController : public TConfig
{
    /** Public methods: */
    public:
     	TController( string name_c, SBDS bd, TTipController *tcntr, TConfigElem *cfgelem );

	virtual ~TController(  );
	
	string &Name() { return(name); }
	char   Stat() { return(stat); }

	virtual void Load(  );
	virtual void Save(  );
	virtual void Free(  );
	virtual void Start(  );
	virtual void Stop(  );
	virtual void Enable(  );
	virtual void Disable(  );

	/*
	 * Avoid parameters list
	 */
	void list( vector<string> &list )
	{ m_hd.obj_list( list ); }
	/*
	 * Add parameter
	 */
	void add( string name, string type, int pos = -1);
	/*
	 * Del parameter
 	 */
	void del( string name );
	/*
	 * Rotate parameters (positions)
	 */
	void rotate( string name1, string name2)
	{ m_hd.obj_rotate( name1, name2 ); }
	/*
	 * Attach to parameter
         * Return parameter header
	 */
	unsigned att( string name )
	{ return( m_hd.hd_att( name ) ); }
	/*
	 * Detach from parameter
     	 */
	void det( unsigned hd )
    	{ m_hd.hd_det( hd ); }
    	/*
	 * Get attached parameter
	 */
	TParamContr &at( unsigned hd )
	{ return( *(TParamContr *)m_hd.hd_at( hd ) ); }
	TParamContr &operator[]( unsigned hd ){ return( at(hd) ); }
									  

	/*
	 * Add parameter with type Name_P and <name> to position <pos> (<0 - to end) 
	 */
	//unsigned  Add( string Name_TP, string name, int pos );
	/*
         * Delete parameter with type Name_P and <name>
	 */    
	//void  Del( string name );
	/*
	 * Rotated parameter with type Name_P between name1 and name2
	 */
	//void  Rotate( string name1, string name2);
	
	//void List( vector<string> & List );
	/*
	 * Convert Name parameter to hd (hd - individual number of parameter for fast calling to parameter )
	 */
	//unsigned NameToHd( string Name );
	/*
	 * Get Parameter throw hd (individual parameter number)
	 */
	//TParamContr &at( unsigned id_hd );
	//TParamContr &operator[]( unsigned id_hd ){ return( at(id_hd) ); }

	/*
	 * Registering parameter(s)
	 */
	void RegParamS();
	void RegParam( string name );
	/*
	 * UnRegistering parameter(s)
	 */
	void UnRegParamS();
	void UnRegParam( string name );
    
	TTipController &Owner() { return( *owner ); }
	
    protected:    
	//vector< int > hd;                 //header of parameter 
	//vector< TParamContr * > cntr_prm; //config parameter
	
    /** Private methods: */
    private:
    	/*
	 * Hd operations
         */
	//unsigned HdIns( unsigned id );
	//void HdFree( unsigned id );
	//void HdChange( unsigned id1, unsigned id2 );

	void LoadParmCfg(  );
	void SaveParmCfg(  );
	void FreeParmCfg(  );
	
	virtual TParamContr *ParamAttach( string name, int type );
	
    /**Attributes: */
    private:    
    	string  name;
	char    stat;
	SBDS    m_bd;

	THD     m_hd;     //hd 
    
	TTipController *owner;    

	static const char *o_name;
};


#endif // TCONTROLLER_H
