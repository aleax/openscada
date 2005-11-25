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

#ifndef TVALUE_H
#define TVALUE_H

#include <string>
#include <vector>

#include "terror.h"
#include "tconfig.h"

using std::string;
using std::vector;

struct STime
{
    time_t s;
    long   us;
};

struct SBasync
{
    int           i_head;          //id of header
    vector<STime> time;            //element's time
    union
    {
        vector<string> *val_s;     //string value
	vector<double> *val_r;     //real value
	vector<int>    *val_i;     //integer value
	vector<bool>   *val_b;     //boolean value
    }val;
};

struct SBsync
{
    STime t_head;            //element's time
    STime t_per;             //element's period
    int   i_head;
    int   i_tail;    
    union
    {
        vector<string> *val_s;     //string value
	vector<double> *val_r;     //real value
	vector<int>    *val_i;     //integer value
	vector<bool>   *val_b;     //boolean value
    }val;
};

union SBUF 
{
    SBsync  *sync;     
    SBasync *async;
};

//Element type flags
#define FLD_DRD    0x10  //Direct read
#define FLD_DWR    0x20  //Direct write

class TValue;

class TVal : public TCntrNode
{
    public:
	TVal(TFld &fld, TValue *owner);
	TVal(TCfg &cfg, TValue *owner);
	~TVal();
	
	const string &name();

	TFld &fld();
	
       	// stat element 
    	bool valid()           { return(m_valid); }
    	void valid( bool val ) { m_valid = val; }
	
	// Read curent value (direct)
	string getSEL( STime *tm = NULL, bool sys = false );
	string getS( STime *tm = NULL, bool sys = false );
	double getR( STime *tm = NULL, bool sys = false );
	int    getI( STime *tm = NULL, bool sys = false );
	bool   getB( STime *tm = NULL, bool sys = false );
	
	// Set curent value
	void setSEL( const string &value, STime *tm = NULL, bool sys = false );
	void setS( const string &value, STime *tm = NULL, bool sys = false );
	void setR( double value, STime *tm = NULL, bool sys = false );
	void setI( int value, STime *tm = NULL, bool sys = false );
	void setB( bool value, STime *tm = NULL, bool sys = false );    
	
    protected:
	void vlSet(  );
	void vlGet(  );
	
    private:
	string nodeName(){ return name(); }	
	
    private:
	union
	{ 
	    string *val_s;   //string value
	    double val_r;    //real value
	    int    val_i;    //integer value	
	    bool   val_b;    //boolean value
	} val;
	
	bool     m_cfg;    //Config id
        union
	{
	    TFld *fld;
	    TCfg *cfg;
	} src;	
	bool     m_valid;  
	STime    time;     // Time
};


class TConfig;

class TValue: public TCntrNode, public TValElem
{
    friend class TVal;
    /** Public methods: */
    public:
	TValue( );
	TValue( TConfig *cfg );
	virtual ~TValue();

	// Atributes
	void vlList( vector<string> &list )	{ chldList(m_vl,list); }
	bool vlPresent( const string &name )	{ return chldPresent(m_vl,name); }
	AutoHD<TVal> vlAt( const string &name )	{ return chldAt(m_vl,name); }

    /** Protected metods */
    protected:
	// Manipulation for elements of value
	void vlAttElem( TElem *ValEl );
	void vlDetElem( TElem *ValEl );
	TElem &vlElem( const string &name );
	
	virtual void vlSet( TVal &val ){};
	virtual void vlGet( TVal &val ){};

	//Control functions
	void cntrMake( XMLNode *fld, const char *req, const char *path, int pos );
	void cntrCmd( const string &elem, XMLNode *fld, TCntrNode::Command cmd );
	
	void postEnable();
	
    /** Private metods */
    private:
	// Set value direct into controller param's
	virtual void vlSet( int id_elem )
	{ throw TError(nodePath().c_str(),"Direct access to write value no admit."); }
	// Get value direct from controller param's    
	virtual void vlGet( int id_elem )
	{ throw TError(nodePath().c_str(),"Direct access to read value no admit."); }
	
	// Add elem into TElem
	void addElem( TElem &el, unsigned id_val); 
	// Del elem without TElem
	void delElem( TElem &el, unsigned id_val); 
	
    /** Private atributes: */
    private:
	int		m_vl;
	vector<TElem*>	elem;  // elements  

	int	     	l_cfg;  // Config len
	TConfig       	*m_cfg; // Configs (static part)    
};

#endif // TVALUE_H

