/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TMODULE_H
#define TMODULE_H

#include <string>
#include <vector>

#include "xml.h"
#include "tcontr.h"
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
    
	virtual string mod_info( const string &name );
	virtual void   mod_info( vector<string> &list );
    
	virtual void mod_CheckCommandLine( );

	virtual void mod_UpdateOpt();    
	// Get XML module node
	XMLNode *mod_XMLCfgNode();
	// Get list exporting function.
	void mod_ListFunc( vector<string> &list );
	// Get address exporting function and registre of use function.
    	void mod_GetFunc( const string &NameFunc, void (TModule::**offptr)() );
	// Unregistre function
	void mod_FreeFunc( const string &NameFunc );
	// Get param exporting function.
	void mod_Func( const string &name, SFunc &func );
 
	string &mod_Name() { return(NameModul); }
    
	TGRPModule &Owner() { return( *owner ); }
	//================== Message functions ========================
	void m_put( const string &categ, int level, char *fmt,  ... );
	void m_put_s( const string &categ, int level, const string &mess );     
	//================== Translate functions ======================
	char *I18N( char *mess );  
	string I18Ns( const string &mess );
    /** Public Attributes: */
    public:
    
    protected:
	virtual void mod_connect(  );	
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	void ctr_din_get_( const string &a_path, XMLNode *opt );
	void ctr_din_set_( const string &a_path, XMLNode *opt );
	void ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez );
	AutoHD<TContr> ctr_at1( const string &br );
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
    private:
	string            lc_id;        // Locale id. For gettext.
	TGRPModule        *owner;
	static const char *l_info[];    // list avoid info options
    
	static const char *o_name;
};



#endif // TMODULE_H
