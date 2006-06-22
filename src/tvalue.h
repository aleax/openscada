
//OpenSCADA system file: tvalue.h
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

#ifndef TVALUE_H
#define TVALUE_H

#include <string>
#include <vector>

#include "terror.h"
#include "tconfig.h"

using std::string;
using std::vector;

//Element type flags
#define FLD_DRD    0x10  //Direct read
#define FLD_DWR    0x20  //Direct write

//Error values
#define EVAL_BOOL 2
#define EVAL_INT  -2147483647
#define EVAL_REAL -3.3E308
#define EVAL_STR  "<EVAL>"

class TValue;
class TVArchive;

class TVal : public TCntrNode
{
    public:
	TVal(TFld &fld, TValue *owner);
	TVal(TCfg &cfg, TValue *owner);
	~TVal();
	
	const string &name();

	TFld &fld();
	
	//- Read curent value (direct) -
	string getSEL( long long *tm = NULL, bool sys = false );
	string getS( long long *tm = NULL, bool sys = false );
	double getR( long long *tm = NULL, bool sys = false );
	int    getI( long long *tm = NULL, bool sys = false );
	char   getB( long long *tm = NULL, bool sys = false );
	
	//- Set curent value -
	void setSEL( const string &value, long long tm = 0, bool sys = false );
	void setS( const string &value, long long tm = 0, bool sys = false );
	void setR( double value, long long tm = 0, bool sys = false );
	void setI( int value, long long tm = 0, bool sys = false );
	void setB( char value, long long tm = 0, bool sys = false );    
	
	AutoHD<TVArchive> arch();
	void arch(const AutoHD<TVArchive> &vl);	
	
    protected:    
	void vlSet(  );
	void vlGet(  );
	
    private:
        //Methods
	string nodeName(){ return name(); }
	
	//Attributes
	union
	{ 
	    string *val_s;   //string value
	    double val_r;    //real value
	    int    val_i;    //integer value	
	    char   val_b;    //boolean value
	} val;
	
	bool     m_cfg;    //Config id
        union
	{
	    TFld *fld;
	    TCfg *cfg;
	} src;
	long long time;	//Last value's time (usec)
	AutoHD<TVArchive>	m_arch;
};


class TConfig;

class TValue: public TCntrNode, public TValElem
{
    friend class TVal;

    public:
	TValue( );
	virtual ~TValue();

	//- Atributes -
	void vlList( vector<string> &list )	{ chldList(m_vl,list); }
	bool vlPresent( const string &name )	{ return chldPresent(m_vl,name); }
	AutoHD<TVal> vlAt( const string &name )	{ return chldAt(m_vl,name); }

    protected:
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	//- Manipulation for config element -
	TConfig *vlCfg()  { return m_cfg; }
	void vlCfg( TConfig *cfg );	//Set configs. NULL - clear configs.
    
	//- Manipulation for elements of value -
	bool vlElemPresent( TElem *ValEl );
	void vlElemAtt( TElem *ValEl );
	void vlElemDet( TElem *ValEl );
	TElem &vlElem( const string &name );
	
	virtual void vlSet( TVal &val ){};
	virtual void vlGet( TVal &val ){};
	
    private:
	//Methods
	//- Set value direct into controller param's -
	virtual void vlSet( int id_elem )
	{ throw TError(nodePath().c_str(),Mess->I18N("Direct access to write value no admit.")); }
	//- Get value direct from controller param's -
	virtual void vlGet( int id_elem )
	{ throw TError(nodePath().c_str(),Mess->I18N("Direct access to read value no admit.")); }
	
	//- TElem commands -
	void detElem( TElem *el );
	void addFld( TElem *el, unsigned id_val); 
	void delFld( TElem *el, unsigned id_val); 
	
	//Attributes
	int		m_vl;
	vector<TElem*>	elem;   // Elements (dinamic parts)

	int	     	l_cfg;  // Config len
	TConfig*	m_cfg;  // Configs (static parts)
};

#endif // TVALUE_H

