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
  
#ifndef TCONFIG_H
#define TCONFIG_H

#include <string>
#include <vector>

#include "tcntrnode.h"
#include "telem.h"

//Element type flags
#define FLD_NOVAL   0x10  //No value mirored
#define FLD_KEY     0x20  //Primary key

using std::string;
using std::vector;

class TConfig;

class TCfg
{
    public:
	TCfg( TFld &fld, TConfig &owner );
	~TCfg();
	
	const string &name();
	
	bool operator==(TCfg & cfg);
        TCfg &operator=(TCfg & cfg);		
	
	bool  view( )		{ return(m_view); }
	void  view( bool vw )	{ m_view = vw; }
	
	TFld &fld()		{ return(*m_fld); }	
	
	//Universal access
        string getSEL( );
	string getS( );
	double getR( );
	int    getI( );
	bool   getB( );
	//Direct access
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
    	union 
	{
	    string *s_val;
	    double r_val;
	    int    i_val;
	    bool   b_val;
	}m_val;	
	bool m_view;
	
	TFld     *m_fld;
	TConfig  &m_owner;	
};


class TTable;

class TConfig: public TValElem
{
    friend class TCfg;

    /** Public methods: */
    public:
	TConfig( TElem *Elements = NULL );
	~TConfig();

	TConfig &operator=(TConfig &cfg);

	void cfgList( vector<string> &list );
	TCfg &cfg( const string &n_val );
	
	void elem(TElem *Elements); 
	TElem &elem();

    protected:	
	virtual bool cfgChange( TCfg &cfg )	{ return(true); }
	
	void	cntrMake( XMLNode *fld, const char *req, const char *path, int pos );
        void    cntrCmd( const string &elem, XMLNode *fld, TCntrNode::Command cmd );
	
    /** Public methods: */
    private:
	// Add elem into TElem
	void addElem( TElem &el, unsigned id );
	// Del elem without TElem
	void delElem( TElem &el, unsigned id );	
	
    /**Attributes: */
    private:
	vector<TCfg*>	value;
	TElem   	*m_elem;
        bool     	single;
};

#endif // TCONFIG_H
