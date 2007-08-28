
//OpenSCADA system file: telem.h
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
    
#ifndef TELEM_H
#define TELEM_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class XMLNode;

//Internal structures
class TFld
{
    public:    
	//Data
	enum Type 	//Field's types
	{ 
	    Boolean = 0, 
	    Integer = 1, 
	    Real    = 4, 
	    String  = 5 
	};
	enum AttrFlg	//Base field's flags
        {
	    NoFlag   = 0x00,	//No flag
	    Selected = 0x01,	//Connnect to simple elements
	    SelfFld  = 0x02,	//Create self field
	    NoWrite  = 0x04,	//No writeable
	    HexDec   = 0x08,	//Hexodecimal view prefer for decimal value type
	    OctDec   = 0x10,	//Octal view prefer for decimal value type	    
	    FullText = 0x08	//Full, multiline text for string type

	};
	
	//Methods
	TFld( );
	TFld( TFld &ifld );
	TFld( const char *name, const char *descr, Type itype, unsigned iflg,
	    const char *valLen = "", const char *valDef = "", 
	    const char *vals = "", const char *nSel = "", int res = 0 );
	~TFld( );
	
	TFld &operator=( TFld &fld );	

	//- Base -
	const string &name() 	{ return m_name; }	//Name
	const string &descr() 	{ return m_descr; }	//Description
	int len()     		{ return m_len; }	//Length
	int dec()      		{ return m_dec; }	//Float dec
	Type type()		{ return m_type; }	//Value type
	unsigned flg()		{ return m_flg; }	//Flags
	const string &def()  	{ return m_def; }	//Default value
	string values();				//Values range or values list
	string selNames();				//Select names list
	int reserve()		{ return m_res; }	//Reserve field
	
	void setDescr( const string &idscr )	{ m_descr = idscr; }
	void setLen( int ivl )			{ m_len = ivl; }
	void setDec( int ivl )			{ m_dec = ivl; }
	void setDef( const string &idef )	{ m_def = idef; }
	void setFlg( unsigned iflg );
	void setValues( const string &vls );
	void setSelNames( const string &slnms );
	void setReserve( int ires )		{ m_res = ires; }	
	
	//- Selected -
	const vector<string> &selValS();
	const vector<int>    &selValI();
	const vector<double> &selValR();
	const vector<bool>   &selValB();
       	//- selectable element's name -
	const vector<string> &selNm();

	string selVl2Nm( const string &val );
	string selVl2Nm( int val );
	string selVl2Nm( double val );
	string selVl2Nm( bool val );
	
	string selNm2VlS( const string &name );	
	int    selNm2VlI( const string &name );	
	double selNm2VlR( const string &name );	
	bool   selNm2VlB( const string &name );	

	//- Addition -
	XMLNode *cntrCmdMake( XMLNode *opt, const string &path, int pos, 
				const string &user = "root", const string &grp = "root", int perm = 0664 );
	
    private:    
	//Attributes
	string          m_name;  	// Name of element (name column into BD);
	string          m_descr; 	// Description of element;
	int		m_len;		// field len
	int		m_dec;		// field dec (for real)
	Type		m_type;		// Type (Dec, Hex, Oct, ...)
	unsigned	m_flg;		// element flags (Selected, SelfFld ...);
	string          m_def;		// default value;
	int		m_res;
	
	union           
	{
	    vector<string> *v_s;
	    vector<double> *v_r;
	    vector<int>    *v_i;
	    vector<bool>   *v_b;
	}m_val;
	vector<string>     *m_sel;  
}; 

class TTable;
class TValElem;

class TElem
{
    public:
	//Methods
	TElem( const string &name = "" );
	~TElem();
	
	string &elName( )	{ return m_name; }
	
	void fldList( vector<string> &list );
	unsigned fldSize()	{ return elem.size(); }
	unsigned fldId( const string &name );	
	bool fldPresent( const string &name );
	int fldAdd( TFld *fld, int id = -1 );
	void fldDel( unsigned int id );
	TFld &fldAt( unsigned int id );

        void valAtt( TValElem *cnt ); 
        void valDet( TValElem *cnt ); 
	
    private:
	//Attributes
        string       	m_name;
	vector<TFld*>	elem;
	vector<TValElem*>	cont;        //Conteiners
};

//**********************************************************************
//************** TValElem - container of elements **********************
//**********************************************************************

class TValElem
{
    friend class TElem;
    
    public:	
	//Methods
	TValElem()	{ };
	
    protected:
	//Methods
	virtual void detElem( TElem *el ) = 0;
	virtual void addFld( TElem *el, unsigned id ) = 0;
	virtual void delFld( TElem *el, unsigned id ) = 0;
};


#endif // TCONFIGELEM_H
