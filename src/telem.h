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
    
#ifndef TELEM_H
#define TELEM_H

#include <string>
#include <vector>
//---- Type ----
#define T_STRING  0x01
#define T_DEC     0x02
#define T_OCT     0x04
#define T_HEX     0x08
#define T_REAL    0x10
#define T_BOOL    0x20

#define T_SELECT  0x40   //Connnect to simple elements

//---- Other flags ----
#define F_SELF    0x80   //Create self field
#define F_NWR     0x100  //No writeable
#define F_PREV    0x200  //Prevent owner for change

using std::string;
using std::vector;

//Internal structures
class TFld
{
    public:
	TFld( );
	TFld( const string &name, const string &descr, unsigned type,
	    const string &valLen = "", const string &valDef = "", 
	    const string &vals = "", const string &nSel = "" );
	~TFld();
	
	TFld &operator=( TFld &fld );	

	const string &name() { return m_name; }	// Name of element (name column into BD);
	string &descr()      { return m_descr; }// Description of element;
	int len()	     { return m_len; }	// field len 
	int dec()            { return m_dec; }	// field dec (for real)
	unsigned type()      { return m_type; }	// Type of element (T_STRING, T_DEC, T_OCT, T_HEX, T_REAL, T_SELECT, T_SELECT|T_STRING); 
	const string &def()  { return m_def; }	// default value;
	
	vector<string> &selValS();
	vector<int>    &selValI();
	vector<double> &selValR();
	vector<bool>   &selValB();
       	// selectable element's name        
	vector<string> &selNm();

	string selVl2Nm( const string &val );
	string selVl2Nm( int val );
	string selVl2Nm( double val );
	string selVl2Nm( bool val );
	
	string &selNm2VlS( const string &name );	
	int     selNm2VlI( const string &name );	
	double  selNm2VlR( const string &name );	
	bool    selNm2VlB( const string &name );	

    private:    
	string          m_name;  
	string          m_descr; 
	int		m_len;
	int		m_dec;
	unsigned	m_type;  
	string          m_def;
	//vector<string>  m_vals;  
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
    /** Public methods: */
    public:
	TElem( const string &name );
	~TElem();
	
	string &elName( ) { return(m_name); }
	
	void fldList( vector<string> &list );
	unsigned fldSize(){ return(elem.size()); }
	unsigned fldId( const string &name);	
	int fldAdd( TFld *fld, int id = -1 );
	void fldDel(unsigned int id);
	TFld &fldAt( unsigned int id );

        void valAtt( TValElem *cnt ); 
        void valDet( TValElem *cnt ); 
	
    /**Attributes: */
    private:
        string             m_name;
	vector<TFld*>      elem;
	vector<TValElem*> cont;        //Conteiners
	static const char  *o_name;
};

//**********************************************************************
//************** TValElem - container of elements **********************
//**********************************************************************

class TValElem
{
    friend class TElem;
    
    /** Public methods: */
    public:	
	TValElem();
	virtual ~TValElem();
    protected:
	// Add element
	virtual void addElem( TElem &el, unsigned id ) = 0;
	// Del element
	virtual void delElem( TElem &el, unsigned id ) = 0;
};


#endif // TCONFIGELEM_H
