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

#include "telem.h"

using std::string;
using std::vector;

class TConfig;

class TCfg
{
    public:
	TCfg( TFld &fld, TConfig &owner );
	~TCfg();
	
	const string &name();
	bool  view( ){ return(m_view); }
	void  view( bool vw ){ m_view = vw; }
	TFld &fld(){ return(*m_fld); }	
	
        string getSEL( );
	string &getS( );
	double &getR( );
	int    &getI( );
	bool   &getB( );
	
	void setSEL( const string &val );
	void setS( const string &val );
	void setR( double val );
	void setI( int val );
	void setB( bool val );

	bool operator==(TCfg & cfg);
	TCfg &operator=(TCfg & cfg);

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
	static const char       *o_name;
};


class TTable;

class TConfig: public TContElem
{
    /** Public methods: */
    public:
	TConfig( TElem *Elements );
	~TConfig();

	friend class TElem;

	TCfg &cfg( const string &n_val, unsigned int id = 0 );    

	unsigned cfSize(){ return(value.size()); }
	// Add record <id_rec>. 
	int cf_AddRecord( unsigned int id);
	// Free record <id_rec> whith rotated other record. 
	void cfFreeRecord( unsigned int id);
	/*
	 * Free dublicated record
	 *   n_val - field for search dubl;
	 *   mode  - mode search (false - begin; true - end)
	 */
	void cfFreeDubl( const string &n_val, bool mode );
	// Load value for record <id_rec> from BD <bd>.     
	void cfLoadValBD( const string &NameFld, TTable &table, unsigned int id_rec=0 );
	void cfLoadValBD( int line_bd, TTable &table, unsigned int id_rec=0 );
	/*
	 * Save value for record <id_rec> to BD <bd>. 
	 * If BD absent then create new BD into default BD type.
	 * If field absent into BD then it created;
	 * If field no use then no change.
	 */
	void cfSaveValBD( const string &NameFld, TTable &table, unsigned int id_rec=0 );
	void cfSaveValBD( int line_bd, TTable &table, unsigned int id_rec=0 );
	/*
	 * Load all value from BD <bd> into whith add internal value
	 *   table    - poiner to bd table;
	 */
	void cfLoadAllValBD( TTable &table, bool free = true );
	// Save all internal value into BD <bd> whith free <bd>
	int cfSaveAllValBD( TTable &table );
	// Equalited congigs
	TConfig & operator=(TConfig & Cfg);

	void cfConfElem(TElem *Elements); 
	TElem &cfConfElem();

	void cfListEl( vector<string> &list, unsigned int id = 0 );
	
	virtual bool cfChange( TCfg &cfg ){ return(true); }
    /** Public methods: */
    private:
	// Add elem into TElem
	void addElem( TElem &el, unsigned id );
	// Del elem without TElem
	void delElem( TElem &el, unsigned id );	
    /**Attributes: */
    private:
	vector< vector<TCfg*> > value;
	TElem                   *elem;
        bool                    single;
	
	static const char       *o_name;
};

#endif // TCONFIG_H
