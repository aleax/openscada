
//OpenSCADA system file: tmodule.h
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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
    public:
	//Data
	//- Attach module struct -
	struct SAt
	{
	    string id;          //id module
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
	
	//Methods
	TModule( );
	virtual ~TModule(  );
	
        const string &modId()   { return mId; }
	string modName();		       
    
	virtual void modLoad( ) { }
	virtual void modSave( ) { }
        virtual void modStart( ) { }
        virtual void modStop( ) { }
    
	virtual void   modInfo( vector<string> &list );
	virtual string modInfo( const string &name );
    
	//- Export functions -
	void modFuncList( vector<string> &list );
	bool modFuncPresent( const string &prot );
	ExpFunc &modFunc( const string &prot );
	void modFunc( const string &prot, void (TModule::**offptr)() );
	
        //- Translate functions -
	const char *I18N( const char *mess );
        string I18Ns( const string &mess );				
    
	TSubSYS &owner() 	{ return *(TSubSYS *)nodePrev(); }
    
    protected:    
	//Methods
	void postEnable();
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	//- Reg export function -
	void modFuncReg( ExpFunc *func )	{ m_efunc.push_back(func); }
	
	//Attributes
	string mId;  	// Identificator
	string mName;	// Name
	string mDescr;  // Describe
	string mType;  	// Type
	string mVers;  	// Version
	string mAutor;	// Autor
	string mLicense;// License 
	string mSource; // Source of module (SO, in build, ....)

    private:
	//Methods
	string nodeName()       { return modId(); }

	//Attributes
	string         	lc_id;        // Locale id. For gettext.
	vector<ExpFunc *> m_efunc;	// Export function list
	
	static const char *l_info[];    // list of the present info options
};

#endif // TMODULE_H
