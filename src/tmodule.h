
#ifndef TMODULE_H
#define TMODULE_H

#include <string>
#include <vector>

#include "xml.h"
#include "tkernel.h"

using std::string;
using std::vector;

//=====================================================
//======== Structs for external use ===================
//=====================================================

//Attach module struct
struct SAtMod
{
    string name;   //Name module
    string type;   //Type module
    int    t_ver;  //Type version module
};

//====== Structura for Exportin function =======
class TModule;
struct SExpFunc
{
    char *NameFunc;
    void (TModule::*ptr)();
    char *prototip;
    char *descript;
    int  resource;
    int  access;
};


//=====================================================
//======== Structs for internal use ===================
//=====================================================

//Export function description struct
struct SFunc
{
    string prototip;      //Prototip function
    string descript;      //Description function
    int  resource;        //Resources number for access to function
    int  access;          //Access counter
};


class TGRPModule;

class TModule : public TContr 
{
    friend class TGRPModule;
    /** Public methods: */
    public:
	TModule( );     

	virtual ~TModule(  );
    
	virtual string mod_info( const string name );
	virtual void   mod_info( vector<string> &list );
    
	virtual void mod_CheckCommandLine( );

	virtual void mod_UpdateOpt();    
	// Get XML module node
	XMLNode *mod_XMLCfgNode();
	// Get list exporting function.
	void mod_ListFunc( vector<string> &list );
	// Get address exporting function and registre of use function.
    	void mod_GetFunc( string NameFunc, void (TModule::**offptr)() );
	// Unregistre function
	void mod_FreeFunc( string NameFunc );
	// Get param exporting function.
	void mod_Func( string name, SFunc &func );
 
	string &mod_Name() { return(NameModul); }
    
	TGRPModule &Owner() { return( *owner ); }
    
    /** Public Attributes: */
    public:
    
    protected:
	virtual void mod_connect(  ); 
    /** Protected Attributes: */
    protected:
	string Source;       // Source of module (SO, in build, ....)
	string NameModul;    // Name module
	string NameType;     // Name type module
	string Vers;         // Version module
	string Autors;       // Autors module
	string DescrMod;     // Describe module
	string License;      // License module 

	SExpFunc *ExpFunc;  // List of export function
	int  NExpFunc;      // Number export function

    private:
	void mod_connect( TGRPModule *owner ); 
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode &inf );
	void ctr_opt_apply( XMLNode &opt );
    private:
	TGRPModule        *owner;
	static const char *l_info[];    // list avoid info options
    
	static const char *i_cntr;
	static const char *o_name;
};



#endif // TMODULE_H
