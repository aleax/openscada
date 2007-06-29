
//OpenSCADA system file: tconfig.h
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
  
#ifndef TCONFIG_H
#define TCONFIG_H

#include <string>
#include <vector>
#include <map>

#include "telem.h"

using std::string;
using std::vector;
using std::map;

class TConfig;

class TCfg
{
    public:
	//Data
	enum AttrFlg
	{
	    Prevent = 0x100,	//Prevent owner for change
	    NoVal   = 0x200,	//No value mirrored
	    Key     = 0x400	//Primary key
	};	
    
	//Methods
	TCfg( TFld &fld, TConfig &owner );
	~TCfg();
	
	const string &name();
	
	bool operator==(TCfg &cfg);
        TCfg &operator=(TCfg &cfg);		
	
	bool  view( )		{ return m_view; }
	void  view( bool vw )	{ m_view = vw; }
	
	TFld &fld()		{ return *m_fld; }	
	
	//- Universal access -
        string getSEL( );
	string getS( );
	double getR( );
	int    getI( );
	bool   getB( );
	//- Direct access -
        string &getSd( );
        double &getRd( );
        int    &getId( );
        bool   &getBd( );					
	
	void setSEL( const string &val );
	void setS( const string &val );
	void setR( double val );
	void setI( int val );
	void setB( bool val );
	
    private:
	//Data
    	union 
	{
	    string *s_val;
	    double r_val;
	    int    i_val;
	    bool   b_val;
	}m_val;	
	
	//Attributes
	bool m_view;
	
	TFld     *m_fld;
	TConfig  &m_owner;	
};

typedef map< string, TCfg* > TCfgMap;

class TTable;

class TConfig: public TValElem
{
    friend class TCfg;

    public:
	//Methods
	TConfig( TElem *Elements = NULL );
	~TConfig();

	TConfig &operator=(TConfig &cfg);

	void cfgList( vector<string> &list );
	bool cfgPresent( const string &n_val );
	TCfg &cfg( const string &n_val );
	void cfgViewAll( bool val = true );	//Hide all no key elements
	
	TElem &elem();
	void setElem(TElem *Elements, bool first = false); 	

	void cntrCmdMake( XMLNode *fld, const string &path, int pos, 
		const string &user = "root", const string &grp = "root", int perm = 0664 );
        void cntrCmdProc( XMLNode *fld, const string &elem, 
		const string &user = "root", const string &grp = "root", int perm = 0664 );

    protected:	
	//Methods
	virtual bool cfgChange( TCfg &cfg )	{ return true; }
	
	void detElem( TElem *el );
	void addFld( TElem *el, unsigned id );
	void delFld( TElem *el, unsigned id );	

    private:	
	//Attributes
	TCfgMap		value;
	TElem   	*m_elem;
        bool     	single;
};

#endif // TCONFIG_H
