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

class TGRPModule;

class TModule : public TContr 
{
    friend class TGRPModule;
    /** Public methods: */
    public:
	//Attach module struct
	struct SAt
	{
	    string id;          //Name module
	    string type;        //Type module
	    int    t_ver;       //Type version module
	};		    
	
	//Export function description struct
	struct SFunc
	{
	    string prototip;      //Prototip function
	    string descript;      //Description function
	    int  resource;        //Resources number for access to function
    	    int  access;          //Access counter
	};
	
	//====== Structura for Exportin function =======
	struct SExpFunc
	{
	    char *NameFunc;
	    void (TModule::*ptr)();
	    char *prototip;
    	    char *descript;
	    int  resource;
            int  access;
	};				
    
	TModule( );
	virtual ~TModule(  );
    
	virtual void   modInfo( vector<string> &list );
	virtual string modInfo( const string &name );
    
	virtual void modCheckCommandLine( );
	virtual void modUpdateOpt();    
	
	// Get XML module node
	XMLNode *modCfgNode();
	// Get list exporting function.
	void modListFunc( vector<string> &list );
	// Get address exporting function and registre of use function.
    	void modGetFunc( const string &NameFunc, void (TModule::**offptr)() );
	// Unregistre function
	void modFreeFunc( const string &NameFunc );
	// Get param exporting function.
	void modFunc( const string &name, SFunc &func );
 
	string &modName() { return(mId); }
		
	//================== Message functions ========================
        void mPut( const string &categ, int level, char *fmt,  ... );
        void mPutS( const string &categ, int level, const string &mess );
	
        //================== Translate functions ======================
	char *I18N( char *mess );
        string I18Ns( const string &mess );				
    
	TGRPModule &owner() { return( *m_owner ); }
    
    protected:
	virtual void modConnect(  );	
	//================== Controll functions ========================
	void ctrStat_( XMLNode *inf );
	void ctrDinGet_( const string &a_path, XMLNode *opt );
	void ctrDinSet_( const string &a_path, XMLNode *opt );
	AutoHD<TContr> ctrAt1( const string &br );
	
    /** Protected Attributes: */
    protected:
	string Source; 	// Source of module (SO, in build, ....)
	string mId;  	// Identificator module
	string mName;	// Name module
	string mType;  	// Type module
	string Vers;   	// Version module
	string Autors; 	// Autors module
	string DescrMod;// Describe module
	string License;	// License module 

	SExpFunc *ExpFunc;  // List of export function
	int  NExpFunc;      // Number export function

    private:
	void modConnect( TGRPModule *owner ); 
	
    private:
	string            lc_id;        // Locale id. For gettext.
	TGRPModule        *m_owner;
	static const char *l_info[];    // list avoid info options
    
	static const char *o_name;
};



#endif // TMODULE_H
