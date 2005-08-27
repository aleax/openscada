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

#include "tmess.h"
#include "tcntrnode.h"

using std::string;
using std::vector;

//Export functions object
class TSubSYS;

class TModule : public TCntrNode 
{
    friend class TSubSYS;
    /** Public methods: */
    public:
	//Attach module struct
	struct SAt
	{
	    string id;          //Name module
	    string type;        //Type module
	    int    t_ver;       //Type version module
	};
	
	class ExpFunc
	{
	    public:
		ExpFunc( const string &iprot, const string &idscr, void (TModule::*iptr)() ) :
		    prot(iprot), dscr(idscr), ptr(iptr)	{ };
		string prot;		//Prototip
		string dscr;		//Description
		void (TModule::*ptr)();	//Adress
	};												
	
	TModule( );
	virtual ~TModule(  );
	
        const string &modId()   { return mId; }
	string modName();		       
    
	virtual void modLoad( ) { }
        virtual void modStart( ) { }
        virtual void modStop( ) { }
    
	virtual void   modInfo( vector<string> &list );
	virtual string modInfo( const string &name );
    
	//Export functions
	void modFuncList( vector<string> &list );
	ExpFunc &modFunc( const string &prot );
	void modFunc( const string &prot, void (TModule::**offptr)() );
	
        //================== Translate functions ======================
	const char *I18N( const char *mess );
        string I18Ns( const string &mess );				
    
	TSubSYS &owner() { return *(TSubSYS *)nodePrev(); }
    
    protected:    
	virtual void modConnect(  );
	string nodeName(){ return modId(); }
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	AutoHD<TCntrNode> ctrAt1( const string &br );
	
	//Reg export function
	void modFuncReg( ExpFunc *func ){ m_efunc.push_back(func); }
	
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

    //private:
	//void modConnect( TSubSYS *owner ); 
	
    private:
	string         	lc_id;        // Locale id. For gettext.
	vector<ExpFunc *> m_efunc;	// Export function list
	
	static const char *l_info[];    // list avoid info options
};

#endif // TMODULE_H
