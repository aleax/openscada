
//OpenSCADA system module UI.VCAEngine file: widget.h
/***************************************************************************
 *   Copyright (C) 2006-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef WIDGET_H
#define WIDGET_H

#include <tcntrnode.h>
#include <tvariant.h>
#include <tconfig.h>

#define ATTR_OI_DEPTH	10	//Order index field depth = real limit to attributes
#define ATTR_CON_DEPTH	6	//Connection counter depth = simultaneously connections to single attribute

using namespace OSCADA;

namespace VCA
{

//************************************************
//* Attr: Widget attribute                       *
//************************************************
class Widget;

//#pragma pack(push,1)
class Attr
{
    friend class Widget;

    public:
	//Data
	// Attribute flags
	enum GlobalAttrFlgs {
	    Active	= 0x00100,	//Active attribute for primitives process

	    Image	= 0x00200,	//Store image link to DB or to file
	    DateTime	= 0x00200,	//Store data and time into integer attribute
	    Color	= 0x00400,	//Store color
	    Font	= 0x00800,	//Store font
	    Address	= 0x01000,	//Store address to a parameter or an attribute

	    IsUser	= 0x02000,	//User created element
	    Mutable	= 0x08000,	//Mutable attribute, present depend from "Active" attribute value
	    Generic	= 0x10000,	//Generic atributes' flag. This atributes loaded independent from enabled state
	    DirRead	= 0x20000	//Direct read attribute, through widget
	};

	// Link types
	enum SelfAttrFlgs {
	    CfgConst	= 0x01,		//Constant
	    CfgLnkIn	= 0x02,		//Input link
	    CfgLnkOut	= 0x04,		//Output link
	    ProcAttr	= 0x08,		//Process attribute
	    FromStyle	= 0x40,		//Get value from style

	    SessAttrInh	= 0x10,		//Inherit attribute into running session
	    IsInher	= 0x20		//Inherit attribute
	};

	//Methods
	//> Main
	Attr( TFld *fld, bool inher );
	~Attr( );

	const string &id( );
	string name( );
	TFld::Type type( );
	int flgGlob( );		//Global attribite's flags
	SelfAttrFlgs flgSelf( )	{ return (SelfAttrFlgs)self_flg; }
	unsigned modif( )	{ return mModif; }
	string cfgTempl( );
	string cfgVal( );

	void setFlgSelf( SelfAttrFlgs flg );
	void setModif( unsigned set )	{ mModif = set; }
	void setCfgTempl( const string &vl );
	void setCfgVal( const string &vl );

	//> Get value
	string getSEL( bool sys = false );
	TVariant get( bool sys = false );
	string getS( bool sys = false );
	double getR( bool sys = false );
	int64_t getI( bool sys = false );
	char   getB( bool sys = false );
	AutoHD<TVarObj> getO( bool sys = false );

	// Set value
	void setSEL( const string &val, bool strongPrev = false, bool sys = false );
	void set( const TVariant &val, bool strongPrev = false, bool sys = false );
	void setS( const string &val, bool strongPrev = false, bool sys = false );
	void setR( double val, bool strongPrev = false, bool sys = false );
	void setI( int64_t val, bool strongPrev = false, bool sys = false );
	void setB( char val, bool strongPrev = false, bool sys = false );
	void setO( AutoHD<TVarObj> val, bool strongPrev = false, bool sys = false );

	TFld &fld( )			{ return *mFld; }
	void setFld( TFld *fld, bool inher );

	void AHDConnect( );
	bool AHDDisConnect( );

	Widget *owner( )		{ return mOwner; }

    private:
	//Data
	// Storing
	union {
	    string	*s;		//String
	    double	r;		//Real
	    int64_t	i;		//Integer
	    char	b;		//Boolean
	    AutoHD<TVarObj>	*o;	//Object
	}mVal;
	// Attributes
	TFld		*mFld;		//Base field
	unsigned	mModif;		//Modify counter
	unsigned short	self_flg;	//Self attributes flags
	unsigned short	mOi	:ATTR_OI_DEPTH;		//Order index, up to 256 attributes
	unsigned short	mConn	:ATTR_CON_DEPTH;	//Connections counter

	string	cfg;			//Configuration template and value

	Widget	*mOwner;
};
//#pragma pack(pop)

//************************************************
//* Widget                                       *
//************************************************
class Widget : public TCntrNode
{
    friend class Attr;

    public:
	//Methods
	Widget( const string &id, const string &isrcwdg = "" );
	~Widget( );

	TCntrNode &operator=( TCntrNode &node );

	string id( )			{ return mId.c_str(); }	//Identifier
	virtual string path( );					//Curent widget path
	virtual string name( );					//Name
	virtual string descr( );				//Description
	virtual string ico( )		{ return ""; }		//Icon
	virtual string type( )		{ return "Generic"; }	//Widget hierarchy type
	string owner( );					//Widget owner
	string grp( );						//Widget group
	short  permit( );					//Permission for access to widget
	virtual string calcId( );				//Compile function identifier
	virtual string calcLang( )	{ return ""; }		//Calc procedure language
	virtual string calcProg( )	{ return ""; }		//Calc procedure
	virtual int    calcPer( )	{ return -1; }		//Calc widget period. 0 value talk for calc on session period.
	virtual bool   isContainer( );				//Is container (Is define of the terminator)
	virtual bool   isLink( )	{ return m_lnk; }	//Widget as link
	bool stlLock( )			{ return mStlLock; }	//Style lock

	virtual void setName( const string &inm );
	virtual void setDescr( const string &idscr );
	virtual void setIco( const string &ico )	{ };
	void setOwner( const string &iown );
	void setGrp( const string &igrp );
	void setPermit( short iperm );
	virtual void setCalcLang( const string &ilng )	{ };
	virtual void setCalcProg( const string &iprg )	{ };
	virtual void setCalcPer( int vl )		{ };
	void setStlLock( bool vl )	{ mStlLock = vl; }

	// Storing
	virtual void loadIO( )		{ }			//Load widget's IO
	virtual void saveIO( )		{ }			//Save widget's IO
	virtual void wClear( );					//Widget's changes clear
	virtual void wChDown( const string &ia = "" );		//Widget's changes put down

	// Enable stat
	bool enable( );
	virtual void setEnable( bool val );

	// Inheritance methods
	virtual string parentNm( )	{ return mParentNm; }	//Parent widget name
	virtual string rootId( );				//Root widget id
	AutoHD<Widget> parent( );				//Parent widget
	AutoHD<Widget> parentNoLink( );				//Parent no link widget
	void heritReg( Widget *wdg );				//Register heritator
	void heritUnreg( Widget *wdg );				//Unregister heritator
	vector< AutoHD<Widget> > &herit( ) { return m_herit; }
	virtual void setParentNm( const string &isw );
	virtual void inheritAttr( const string &attr = "" );	//Inherit parent attributes
	void inheritIncl( const string &wdg = "" );		//Inherit parent include widgets

	// Widget's attributes
	void attrList( vector<string> &list );
	virtual void attrAdd( TFld *attr, int pos = -1, bool inher = false, bool forceMdf = false );
	void attrDel( const string &attr, bool allInher = false );
	virtual bool attrPresent( const string &attr );
	int  attrPos( const string &iattr );
	virtual AutoHD<Attr> attrAt( const string &attr, int lev = -1 );

	// Include widgets
	void wdgList( vector<string> &list, bool fromLnk = false );
	bool wdgPresent( const string &wdg );
	virtual void wdgAdd( const string &wid, const string &name, const string &path, bool force = false );
	void wdgDel( const string &wid, bool full = false )	{ chldDel(inclWdg, wid, -1, full); }
	virtual AutoHD<Widget> wdgAt( const string &wdg, int lev = -1, int off = 0 );

	// Data access
	virtual void resourceList( vector<string> &ls )				{ }
	virtual string resourceGet( const string &id, string *mime = NULL )	{ return ""; }

	// Context helps
	static string helpImg( );
	static string helpColor( );
	static string helpFont( );

    protected:
	//Methods
	const char *nodeName()	{ return mId.c_str(); }

	void postEnable( int flag );
	void preDisable( int flag );

	virtual bool cntrCmdServ( XMLNode *opt );
	virtual bool cntrCmdGeneric( XMLNode *opt );
	virtual bool cntrCmdAttributes( XMLNode *opt, Widget *src = NULL );
	virtual bool cntrCmdLinks( XMLNode *opt, bool lnk_ro = false );
	virtual bool cntrCmdProcess( XMLNode *opt );

	virtual bool attrChange( Attr &cfg, TVariant prev );   //Process attribute change local and into terminator
	virtual unsigned int modifVal( Attr &cfg )	{ return 0; }
	virtual TVariant vlGet( Attr &a );
	virtual TVariant stlReq( Attr &a, const TVariant &vl, bool wr );
	virtual bool eventProc( const string &ev, Widget *src = NULL );	//Return "true" for terminate next processing

	virtual void disable( Widget *base );
	virtual void calc( Widget *base );
	virtual TVariant objFuncCall_w( const string &id, vector<TVariant> &prms, const string &user, Widget *src = NULL );

	virtual pthread_mutex_t	&mtxAttr( )	{ return mtxAttrM; }

	//Attributes
	// Generic data
	const string	mId;			//Widget identifier

	unsigned char	inclWdg		:3;
	unsigned char	mEnable		:1;	//Enable status
	unsigned char	m_lnk		:1;	//Widget as link
	unsigned char	mStlLock	:1;	//Style lock
	unsigned char	BACrtHoldOvr	:1;	//Base attrs creation hold over to enable and inherit stage
	unsigned char	ChldResrv	:1;	//Childs reserve attribute

	string		mParentNm;		//Parent widget name
	AutoHD<Widget>	mParent;		//Parent widget
	vector< AutoHD<Widget> > m_herit;	//Heritators
	map<string, Attr* >	mAttrs;
	pthread_mutex_t	mtxAttrM;
	static pthread_mutex_t	mtxAttrCon;
};

}

#endif //WIDGET_H
