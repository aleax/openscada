
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


//--------- Flags ------------
#define FLD_NOFLG   0x00	//No flag
#define FLD_SELECT  0x01   	//Connnect to simple elements
#define FLD_SELF    0x02   	//Create self field
#define FLD_NWR     0x04  	//No writeable
#define FLD_PREV    0x08  	//Prevent owner for change

using std::string;
using std::vector;

class XMLNode;

//Internal structures
class TFld
{
    public:
	//Data
	enum Type { Bool, Dec, Hex, Oct, Real, String };
	
	//Methods
	TFld( );
	TFld( const char *name, const char *descr, Type itype, unsigned char iflg,
	    const char *valLen = "", const char *valDef = "", 
	    const char *vals = "", const char *nSel = "", int w_id = 0 );
	~TFld();
	
	TFld &operator=( TFld &fld );	

	const string &name() 	{ return m_name; }	// Name of element (name column into BD);
	string &descr()      	{ return m_descr; }	// Description of element;
	int len()	     	{ return m_len; }	// field len 
	int dec()            	{ return m_dec; }	// field dec (for real)
	Type type()		{ return m_type; }	// Type (Dec, Hex, Oct, ...)
	unsigned char flg()	{ return m_flg; }	// element flags (FLD_SELECT, FLD_SELF ...); 
	const string &def()  	{ return m_def; }	// default value;
	int workId()		{ return m_wid; }
	
	vector<string> &selValS();
	vector<int>    &selValI();
	vector<double> &selValR();
	vector<bool>   &selValB();
       	//- selectable element's name -
	vector<string> &selNm();

	string selVl2Nm( const string &val );
	string selVl2Nm( int val );
	string selVl2Nm( double val );
	string selVl2Nm( bool val );
	
	string &selNm2VlS( const string &name );	
	int     selNm2VlI( const string &name );	
	double  selNm2VlR( const string &name );	
	bool    selNm2VlB( const string &name );	

	void 	cntrCmdMake( XMLNode *opt, const char *path, int pos );
	
    private:    
	//Attributes
	string          m_name;  
	string          m_descr; 
	int		m_len;
	int		m_dec;
	Type		m_type;
	unsigned char	m_flg;  
	string          m_def;
	int		m_wid;
	
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
	
	string &elName( )	{ return(m_name); }
	
	void fldList( vector<string> &list );
	unsigned fldSize()	{ return(elem.size()); }
	unsigned fldId( const string &name);	
	bool fldPresent( const string &name );
	int fldAdd( TFld *fld, int id = -1 );
	void fldDel(unsigned int id);
	TFld &fldAt( unsigned int id );

        void valAtt( TValElem *cnt ); 
        void valDet( TValElem *cnt ); 
	
    private:
	//Attributes
        string       	m_name;
	vector<TFld*>	elem;
	vector<TValElem*>	cont;        //Conteiners
	static const char	*o_name;
};

//**********************************************************************
//************** TValElem - container of elements **********************
//**********************************************************************

class TValElem
{
    friend class TElem;
    
    public:	
	//Methods
	TValElem();
	virtual ~TValElem();
	
    protected:
	//Methods
	virtual void detElem( TElem *el ) = 0;
	virtual void addFld( TElem *el, unsigned id ) = 0;
	virtual void delFld( TElem *el, unsigned id ) = 0;
};


#endif // TCONFIGELEM_H
