
#ifndef TGRPMODULE_H
#define TGRPMODULE_H

#include <string>
#include <vector>

#include "terror.h"
#include "thd.h"
#include "xml.h"
#include "tcontr.h"
#include "tmodule.h"

using std::string;
using std::vector;

class TModule;
class TModSchedul;
class TKernel;

class TGRPModule : public TContr
{
    /** Public methods: */
    public:
	TGRPModule( TKernel *app, char * NameT );
    
	virtual ~TGRPModule(  );

	string &Name(){ return(s_name); }

	// Type/grp module version
	virtual int gmd_Ver( ) = 0;
	// Init group modules.
	virtual void gmd_Init( ) { }
	// Start group modules    
	virtual void gmd_Start( ) { }
	virtual void gmd_Stop( ) { }
    
	// Avoid modules list
	void gmd_list( vector<string> &list )
	{ m_hd.obj_list( list ); }
	// Add modul
	virtual void gmd_add( TModule *modul );
	// Del modul
	virtual void gmd_del( string name );
	/*
	 * Attach to modul
	 * Return module header
	 */
	unsigned gmd_att( string name )
	{ return( m_hd.hd_att( name ) ); }
	// Detach from modul
	void gmd_det( unsigned hd )
	{ m_hd.hd_det( hd ); }
	// Get attached modul
	TModule &gmd_at( unsigned hd ) 
	{ return( *(TModule *)m_hd.hd_at( hd ) ); }
	TModule &operator[]( unsigned hd )
	{ return(gmd_at(hd)); }

	virtual void gmd_CheckCommandLine( );
	virtual void gmd_UpdateOpt();

	void gmd_CheckCommandLineMods();
	void gmd_UpdateOptMods();
	// Get XML section node
	XMLNode *gmd_XMLCfgNode();
    
	string &gmd_Name()    { return(NameType); }
	string &gmd_ModPath() { return(DirPath); }
    
	TKernel &Owner() { return(*owner); }
	//================== Message functions ========================
	void m_put( string categ, int level, char *fmt,  ... );
	void m_put_s( string categ, int level, string mess ); 
    /**Public Attributes: */
    public:

    /** Protected methods: */
    protected:
	// Register how user of function
	TModule *gmd_FUse(unsigned int hd, char * func, void (TModule::**offptr)());
	// Register how user of function    
	void gmd_FFree(unsigned int hd, char * func)
	{ gmd_at(hd).mod_FreeFunc(func); }

    /** Protected Attributes: */
    protected:    
	string            DirPath;
	string            s_name;
    /** Private methods: */
    private:
        //================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( string path, XMLNode *opt );
	unsigned ctr_att( string br ) { return( gmd_att( ctr_path_l(br,2) ) ); }    
	void     ctr_det( unsigned hd ) { gmd_det( hd ); }
	TContr  &ctr_at( unsigned hd )   { return( (TContr&)gmd_at(hd) ); } 
    /** Private Attributes: */
    private:
	TKernel           *owner;    
	THD               m_hd;
	
	string            NameType;
	static const char *i_cntr;
	static const char *o_name;
};

#endif // TGRPMODULE_H
