
//OpenSCADA system file: telem.h
/***************************************************************************
 *   Copyright (C) 2003-2016 by Roman Savochenko, <rom_as@oscada.org>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#include "resalloc.h"
#include "tfunction.h"

using std::string;
using std::vector;

namespace OSCADA
{

//*************************************************
//* TFld                                          *
//*************************************************
class XMLNode;

//#pragma pack(push,1)
class TFld
{
    public:
	//Data
	enum Type {	//Field's types
	    // Generic
	    Boolean	= 0,
	    Integer	= 1,
	    Real	= 4,
	    String	= 5,
	    Object	= 6,

	    GenMask	= 0x0F,

	    // Extended
	    Int16	= 0x11,
	    Int32	= 0x01,
	    Int64	= 0x21,
	    Float	= 0x14,
	    Double	= 0x04
	};
	enum AttrFlg {	//Base field's flags
	    NoFlag	= 0x00,		//No flag
	    Selected	= 0x01,		//Connnect to simple elements
	    SelEdit	= 0x40,		//Editable list
	    SelfFld	= 0x02,		//Create self field
	    NoWrite	= 0x04,		//No writeable
	    HexDec	= 0x08,		//Decimal value type: hexodecimal view prefer
	    OctDec	= 0x10,		//Decimal value type: octal view prefer
	    DateTimeDec	= 0x20,		//Decimal value type: UTC date seconds counter
	    FullText	= 0x08,		//String value type: full, multiline text
	    TransltText	= 0x10		//String value type: use translation and the variable texts mechanism for configs
	};

	//Methods
	TFld( );
	TFld( const TFld &ifld, const char *name = NULL );
	TFld( const char *name, const char *descr, Type type, unsigned flg,
	    const char *valLen = "", const char *valDef = "",
	    const char *vals = "", const char *nSel = "", const char *res = "" );
	~TFld( );

	TFld &operator=( const TFld &fld );

	// Main
	const string &name( ) const	{ return mName; }	//Name
	const string &descr( ) const	{ return mDescr; }	//Description
	int len( ) const		{ return mLen; }	//Length
	int dec( ) const		{ return mDec; }	//Float dec
	Type type( ) const		{ return (Type)mType; }	//Value type
	static Type type( IO::Type tp );			//Field type from IO
	IO::Type typeIO( ) const;				//Type to IO
	unsigned flg( ) const		{ return mFlg; }	//Flags
	const string &def( ) const	{ return mDef; }	//Default value
	string values( ) const;					//Values range or values list
	string selNames( ) const;				//Select names list
	const string &reserve( ) const	{ return mRes; }	//Reserve field

	void setDescr( const string &idscr )	{ mDescr = idscr; }
	void setLen( int ivl )			{ mLen = ivl; }
	void setDec( int ivl )			{ mDec = ivl; }
	void setDef( const string &idef )	{ mDef = idef; }
	void setFlg( unsigned iflg );
	void setValues( const string &vls );
	void setSelNames( const string &slnms );
	void setReserve( const string &ires )	{ mRes = ires; }

	// Selected
	const vector<string>	&selValS( ) const;
	const vector<int>	&selValI( ) const;
	const vector<double>	&selValR( ) const;
	const vector<bool>	&selValB( ) const;
	// selectable element's name
	const vector<string>	&selNm( ) const;

	string selVl2Nm( const string &val );
	string selVl2Nm( int64_t val );
	string selVl2Nm( double val );
	string selVl2Nm( bool val );
	string selVl2Nm( char val )	{ return selVl2Nm(val); }

	string	selNm2VlS( const string &name );
	int64_t	selNm2VlI( const string &name );
	double	selNm2VlR( const string &name );
	bool	selNm2VlB( const string &name );

	// Addition
	XMLNode *cntrCmdMake( XMLNode *opt, const string &path, int pos,
				const string &user = "root", const string &grp = "root", int perm = 0664 );

    private:
	//Attributes
	string		mName;		// Name of element (name column into BD);
	string		mDescr;		// Description of element;
	int		mLen;		// field len
	short int	mDec	:5;	// field dec (for real)
	short int	mType	:4;	// Type (Dec, Hex, Oct, ...)
	unsigned	mFlg;		// element flags (Selected, SelfFld ...);
	string		mDef;		// default value;
	string		mRes;		// reserve attribut

	union {
	    vector<string>	*s;
	    vector<double>	*r;
	    vector<int>		*i;
	    vector<bool>	*b;
	}mVal;
	vector<string>	*mSel;
};
//#pragma pack(pop)

//*************************************************
//* TElem                                         *
//*************************************************
class TValElem;

class TElem
{
    public:
	//Methods
	TElem( const string &name = "" );
	TElem( const TElem &src );
	virtual ~TElem( );

	TElem &operator=( const TElem &src );

	string &elName( )			{ return mName; }

	void fldList( vector<string> &list );
	unsigned fldSize( )			{ return elem.size(); }
	unsigned fldId( const string &name, bool noex = false );
	bool fldPresent( const string &name );
	int fldAdd( TFld *fld, int id = -1 );
	void fldDel( unsigned int id );
	TFld &fldAt( unsigned int id );
	void fldClear( );

	void valAtt( TValElem *cnt );
	void valDet( TValElem *cnt );

	ResMtx &resEl( )	{ return mResEl; }

    private:
	//Attributes
	string			mName;
	vector<TFld*>		elem;
	vector<TValElem*>	cont;		//Conteiners
	ResMtx			mResEl;
};

//*************************************************
//* TValElem - container of elements              *
//*************************************************
class TValElem
{
    friend class TElem;

    public:
	//Methods
	TValElem( )		{ };
	virtual ~TValElem( )	{ };

    protected:
	//Methods
	virtual void detElem( TElem *el ) = 0;
	virtual void addFld( TElem *el, unsigned id ) = 0;
	virtual void delFld( TElem *el, unsigned id ) = 0;
};

}

#endif // TELEM_H
