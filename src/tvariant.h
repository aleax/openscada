
//OpenSCADA system file: tvariant.h
/***************************************************************************
 *   Copyright (C) 2010-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef TVARIANT_H
#define TVARIANT_H

#include <string>
#include <vector>
#include <map>

#include "autohd.h"
#include "xml.h"
#include "tcntrnode.h"

using std::string;
using std::vector;
using std::map;

//Error values, generic
#define EVAL_BOOL	2
#define EVAL_INT	-9223372036854775807ll
#define EVAL_REAL	-1.79E308
#define EVAL_STR	"<EVAL>"

//Error values, extended
#define EVAL_INT16	-32767
#define EVAL_INT32	-2147483647
#define EVAL_INT64	-9223372036854775807ll
#define EVAL_RFlt	-3.29E38
#define EVAL_RDbl	-1.79E308

namespace OSCADA
{

//*************************************************
//* TVariant                                      *
//*************************************************
class TVarObj;

class TVariant
{
    public:
	//Data
	enum Type {
	    Null	= 0,
	    Boolean	= 1,
	    Integer	= 2,
	    Real	= 3,
	    String	= 4,
	    Object	= 5
	};

	//Methods
	TVariant( );
	TVariant( bool ivl );
	TVariant( char ivl );
	TVariant( int ivl );
	TVariant( int64_t ivl );
	TVariant( double ivl );
	TVariant( const string &ivl );
	TVariant( const char *var );
	TVariant( AutoHD<TVarObj> ivl );
	TVariant( TVarObj *ivl );
	TVariant( const TVariant &var );

	virtual ~TVariant( );

	bool operator==( const TVariant &vr ) const;
	bool operator!=( const TVariant &vr ) const;
	TVariant &operator=( const TVariant &vr );

	bool isNull( ) const	{ return (type()==Null); }
	bool isEVal( ) const;
	Type type( ) const	{ return (Type)mType; }
	void setType( Type tp, bool fix = false );
	bool isModify( )	{ return mModify; }
	void setModify( bool vl = true )	{ mModify = vl; }

	virtual char	getB( ) const;
	virtual int64_t	getI( ) const;
	virtual double	getR( ) const;
	virtual string	getS( ) const;
	virtual AutoHD<TVarObj> getO( ) const;

	operator char( )	{ return getB(); }
	operator int( )		{ return getI(); }
	operator int64_t( )	{ return getI(); }
	operator double( )	{ return getR(); }
	operator string( )	{ return getS(); }
	operator AutoHD<TVarObj>( )	{ return getO(); }

	virtual void setB( char val );
	virtual void setI( int64_t val );
	virtual void setR( double val );
	virtual void setS( const string &val );
	virtual void setO( AutoHD<TVarObj> val );
	virtual void setO( TVarObj *val );

    protected:
	//Data
	union
	{
	    char	b;
	    int64_t	i;
	    double	r;
	    char	*sPtr;
	    char	sMini[8];
	    AutoHD<TVarObj>	*o;
	}val;

	//Attributes
	unsigned mSize		: 27;
	unsigned mType		: 3;
	unsigned mModify	: 1;
	unsigned mFixedTp	: 1;
};

//***********************************************************
//* TVarObj                                                 *
//*   Variable object, by default included properties       *
//***********************************************************
class TVarObj
{
    public:
	//Methods
	TVarObj( );
	virtual ~TVarObj( );

	virtual string objName( )	{ return "object"; }

	void AHDConnect( );
	bool AHDDisConnect( );

	virtual void propList( vector<string> &ls );
	virtual TVariant propGet( const string &id );
	virtual void propSet( const string &id, TVariant val );

	virtual string getStrXML( const string &oid = "" );
	static AutoHD<TVarObj> parseStrXML( const string &str, XMLNode *nd = NULL, AutoHD<TVarObj> prev = NULL );

	virtual TVariant funcCall( const string &id, vector<TVariant> &prms );

    protected:
	//Attributes
	map<string,TVariant> mProps;
	unsigned int mUseCnt;
	static pthread_mutex_t	connM;	//Connection mutex
	Res oRes;
};

//*****************************************************************
//* TEValObj                                                      *
//*   Special EVal object — Scalar bool, int, real, string analog *
//*****************************************************************
class TEValObj : public TVarObj
{
    public:
	//Methods
	TEValObj( );
	~TEValObj( );

	string objName( )	{ return "EVAL"; }

	string getStrXML( const string &oid = "" );
	static AutoHD<TVarObj> parseStrXML( XMLNode *nd = NULL );

	TVariant funcCall( const string &id, vector<TVariant> &prms );
};

//***********************************************************
//* TArrayObj                                               *
//*   Array object included indexed properties              *
//***********************************************************
class TArrayObj : public TVarObj
{
    public:
	//Methods
	TArrayObj( )	{ };

	string objName( )	{ return "array"; }

	int size( )		{ return mEls.size(); }

	TVariant propGet( const string &id );
	void propSet( const string &id, TVariant val );

	string getStrXML( const string &oid = "" );
	static AutoHD<TVarObj> parseStrXML( XMLNode *nd = NULL );

	TVariant funcCall( const string &id, vector<TVariant> &prms );

	size_t arSize( )	{ return mEls.size(); }
	TVariant arGet( int id );
	void arSet( int id, TVariant val );

    private:
	//Attributes
	vector<TVariant> mEls;

	//Methods
	static bool compareLess( const TVariant &v1, const TVariant &v2 );
};

//***********************************************************
//* TRegExp                                                 *
//*   Regular expression object				    *
//***********************************************************
class TRegExp : public TVarObj
{
    public:
	//Methods
	TRegExp( const string &rule = "", const string &flg = "" );
	~TRegExp( );

	string objName( )	{ return "RegExp"; }
	bool isNull( )		{ return !regex; }

	void setPattern( const string &rule, const string &flg = "" );

	TArrayObj *match( const string &vl, bool all = false );
	bool test( const string &vl );
	int search( const string &vl, int off = 0 );
	string replace( const string &vl, const string &str );
	TArrayObj *split( const string &vl, int limit = 0 );

	TVariant propGet( const string &id );
	void propSet( const string &id, TVariant val );

	string getStrXML( const string &oid = "" );

	TVariant funcCall( const string &id, vector<TVariant> &prms );

	//Attribute
	string	err;
	int	lastIndex;	//lastIndex

    private:
	//Methods
	string substExprRepl( const string &str, const string &val, int *capv, int n );

	//Attributes
	string	pattern;
	unsigned global		: 1;
	unsigned ignoreCase	: 1;
	unsigned multiline	: 1;
	unsigned isSimplePat	: 1;

	void	*regex;
	int	vSz, *capv;
};

//*************************************************
//* XMLNodeObj - XML node object                  *
//*************************************************
class XMLNodeObj : public TVarObj
{
    public:
	//Methods
	XMLNodeObj( const string &name = "" );
	~XMLNodeObj( );

	string objName( )       { return "XMLNode"; }

	string name( );
	string text( );

	void setName( const string &vl );
	void setText( const string &vl );

	unsigned childSize( )	{ return mChilds.size(); }
	AutoHD<XMLNodeObj> childGet( unsigned id );
	AutoHD<XMLNodeObj> childGet( const string &name, unsigned num = 0 );
	void childAdd( AutoHD<XMLNodeObj> nd );
	void childIns( unsigned id, AutoHD<XMLNodeObj> nd );
	void childDel( unsigned id );

	string getStrXML( const string &oid = "" );
	static AutoHD<TVarObj> parseStrXML( XMLNode *nd = NULL );

	TVariant funcCall( const string &id, vector<TVariant> &prms );

	void toXMLNode( XMLNode &nd );
	void fromXMLNode( XMLNode &nd );

	AutoHD<XMLNodeObj> getElementBy( const string &attr, const string &val );

    private:
	//Attributes
	string		mName, mText;
	vector<AutoHD<XMLNodeObj> >	mChilds;
	XMLNodeObj	*parent;
};

//***********************************************************
//* TCntrNodeObj                                            *
//*   TCntrNode object for access to system's objects       *
//***********************************************************
class TCntrNodeObj: public TVarObj
{
    public:
	//Methods
	TCntrNodeObj( AutoHD<TCntrNode> nd, const string &user );

	string objName( );

	string user( )		{ return mUser; }

	void setUser( const string &user )	{ mUser = user; }

	TVariant propGet( const string &id );
	void propSet( const string &id, TVariant val );

	string getStrXML( const string &oid = "" );

	TVariant funcCall( const string &id, vector<TVariant> &prms );

    private:
	//Attributes
	string	mUser;
	AutoHD<TCntrNode> cnd;
};

}

#endif // TVARIANT_H
