
//OpenSCADA module UI.VCAEngine file: widget.h
/***************************************************************************
 *   Copyright (C) 2006-2020 by Roman Savochenko, <roman@oscada.org>       *
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

	    Image	= 0x00200,	//Store image link to a DB's mime or a file, expand generic type "String"
	    DateTime	= 0x00200,	//Store data and time, expand generic type "Integer"
	    Color	= 0x00400,	//Store color
	    Font	= 0x00800,	//Store font
	    Address	= 0x01000,	//Store address to a parameter or an attribute

	    IsUser	= 0x02000,	//User created element
	    Mutable	= 0x08000,	//Mutable attribute, present depend from "Active" attribute value
	    Generic	= 0x10000,	//Generic atributes' flag. This atributes loaded independent from enabled state
	    OnlyRead	= 0x20000,	//Only read attribute, through widget, no the attribute storing location
	    PreRead	= 0x40000,	//Preprocessed read
	    NotStored	= 0x80000	//Not stored and loaded - only dynamic
	};

	// Link types
	enum SelfAttrFlgs {
	    CfgConst	= 0x01,		//Constant
	    CfgLnkIn	= 0x02,		//Input link
	    CfgLnkOut	= 0x04,		//Output link
	    ProcAttr	= 0x08,		//Process attribute
	    FromStyle	= 0x40,		//Get value from style
	    VizerSpec	= 0x80,		//Visualizer specified attribute, for allow it to modification control and transmit to the visualizer.
					//Sets at and by a session of running project activation.

	    SessAttrInh	= 0x10,		//Inherited attribute into the running session
	    IsInher	= 0x20		//Inherited attribute
	};

	//Methods
	// Main
	Attr( TFld *fld, bool inher );
	~Attr( );

	const string &id( ) const;
	string name( ) const;
	TFld::Type type( );
	int flgGlob( );		//Global attribite's flags
	SelfAttrFlgs flgSelf( )	{ return (SelfAttrFlgs)mFlgSelf; }
	unsigned modif( )	{ return mModif; }
	string cfgTempl( );
	string cfgVal( );
	static bool isTransl( TFld::Type tp, int flgGlb, int flgSelf = -1 ) {
	    return (tp == TFld::String && (flgGlb&(TFld::TransltText|Attr::IsUser)) &&
		!(flgGlb&(Attr::OnlyRead|Attr::Image|Attr::DateTime|Attr::Color|Attr::Font|Attr::Address)) &&
		(flgSelf == -1 || (flgSelf&(Attr::CfgConst|Attr::CfgLnkIn))));
	}
	bool isTransl( bool cfg = false )	{ return Attr::isTransl(type(), flgGlob(), (cfg?flgSelf():-1)); }

	void setFlgSelf( SelfAttrFlgs flg, bool sys = false );
	void setModif( unsigned set )	{ mModif = set; }
	void setCfgTempl( const string &vl );
	void setCfgVal( const string &vl );

	// Get value
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

	TFld &fld( ) const			{ return *mFld; }
	void setFld( TFld *fld, bool inher );

	void AHDConnect( );
	bool AHDDisConnect( );

	Widget *owner( ) const		{ return mOwner; }

    private:
	//Data
	// Storing
	union {
	    string	*s;	//String
	    double	r;	//Real
	    int64_t	i;	//Integer
	    char	b;	//Boolean
	    AutoHD<TVarObj> *o;	//Object
	} mVal;
	// Attributes
	TFld		*mFld;		//Base field
	unsigned	mModif;		//Modify counter
	unsigned short	mFlgSelf;	//Self attributes flags
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

	TCntrNode &operator=( const TCntrNode &node );

	string id( ) const		{ return mId.c_str(); }	//Identifier
	virtual string path( ) const;				//Curent widget path
	virtual string name( ) const;				//Name
	virtual string descr( ) const;				//Description
	virtual string ico( ) const	{ return ""; }		//Icon
	virtual string type( )		{ return "Generic"; }	//Widget hierarchy type
	string owner( ) const;					//Widget owner
	string grp( ) const;					//Widget group
	short  permit( ) const;					//Permission for access to widget
	virtual string getStatus( );
	virtual string calcId( );				//Compile function identifier
	virtual string calcLang( ) const { return ""; }		//Calc procedure language
	virtual bool   calcProgTr( )	{ return true; }	//Calc procedure translation
	virtual string calcProg( ) const { return ""; }		//Calc procedure
	virtual string calcProgStors( const string &attr = "" )	{ return ""; }	//Storages get
	virtual int    calcPer( ) const	{ return -1; }		//Calc widget period. 0 value talk for calc on session period.
	virtual bool   isContainer( ) const;			//Is container (Is define of the terminator)
	virtual bool   isLink( ) const	{ return mLnk; }	//Widget as link
	bool stlLock( )			{ return mStlLock; }	//Style lock

	virtual void setName( const string &inm );
	virtual void setDescr( const string &idscr );
	virtual void setIco( const string &ico )	{ };
	void setOwner( const string &iown );
	void setGrp( const string &igrp );
	void setPermit( short iperm );
	virtual void setCalcLang( const string &ilng )	{ };
	virtual void setCalcProgTr( bool vl )		{ };
	virtual void setCalcProg( const string &iprg )	{ };
	virtual void setCalcPer( int vl )		{ };
	void setStlLock( bool vl )	{ mStlLock = vl; }

	// Storing
	virtual void loadIO( )		{ }			//Load widget's IO
	virtual void saveIO( )		{ }			//Save widget's IO
	virtual void wClear( );					//Widget's changes clear
	virtual string wChDown( const string &ia = "" );	//Widget's changes put down

	// Enable stat
	bool enable( ) const;
	virtual void setEnable( bool val, bool force = false );
	void linkToParent( );

	// Inheritance methods
	virtual string parentNm( ) const { return mParentNm; }	//Parent widget name
	virtual string rootId( ) const;				//Root widget id
	AutoHD<Widget> parent( ) const;				//Parent widget
	AutoHD<Widget> parentNoLink( );				//Parent no link widget
	void heritReg( Widget *wdg );				//Register heritator
	void heritUnreg( Widget *wdg );				//Unregister heritator
	vector< AutoHD<Widget> > &herit( ) { return mHerit; }
	virtual void setParentNm( const string &isw );
	virtual void inheritAttr( const string &attr = "" );	//Inherit parent attributes
	void inheritIncl( const string &wdg = "" );		//Inherit parent include widgets

	virtual void procChange( bool src = true )	{ }	//Process the procedure change

	// Widget's attributes
	void attrList( vector<string> &list ) const;
	virtual void attrAdd( TFld *attr, int pos = -1, bool inher = false, bool forceMdf = false, bool allInher = false );
	void attrDel( const string &attr, bool allInher = false );
	virtual bool attrPresent( const string &attr ) const;
	int  attrPos( const string &iattr );
	virtual AutoHD<Attr> attrAt( const string &attr, int lev = -1 ) const;

	// Include widgets
	void wdgList( vector<string> &list, bool fromLnk = false ) const;
	bool wdgPresent( const string &wdg ) const;
	virtual void wdgAdd( const string &wid, const string &name, const string &path, bool force = false );
	void wdgDel( const string &wid, bool full = false )	{ chldDel(inclWdg, wid, -1, full); }
	virtual AutoHD<Widget> wdgAt( const string &wdg, int lev = -1, int off = 0 ) const;

	// Data access
	virtual void resourceList( vector<string> &ls )	{ }
	virtual string resourceGet( const string &id, string *mime = NULL )	{ return ""; }

	// Context helps
	static string helpImg( );
	static string helpColor( );
	static string helpFont( );

	virtual TVariant stlReq( Attr &a, const TVariant &vl, bool wr );

    protected:
	//Methods
	const char *nodeName( ) const	{ return mId.c_str(); }

	void postEnable( int flag );
	void preDisable( int flag );

	virtual bool cntrCmdServ( XMLNode *opt );
	virtual bool cntrCmdGeneric( XMLNode *opt );
	virtual bool cntrCmdAttributes( XMLNode *opt, Widget *src = NULL );
	virtual bool cntrCmdLinks( XMLNode *opt, bool lnk_ro = false );
	virtual bool cntrCmdProcess( XMLNode *opt );

	virtual bool attrChange( Attr &cfg, TVariant prev );	//Process an attribute change local and into the terminator
	virtual unsigned int modifVal( Attr &cfg )	{ return 0; }
	virtual TVariant vlGet( Attr &a );
	virtual bool eventProc( const string &ev, Widget *src = NULL );	//Return "true" for terminate next processing

	virtual void disable( Widget *base );
	virtual void calc( Widget *base );
	virtual TVariant objFuncCall_w( const string &id, vector<TVariant> &prms, const string &user, Widget *src = NULL );

	ResMtx	&mtxAttr( )	{ return mtxAttrM; }

	//Attributes
	// Generic data
	const string	mId;			//Widget identifier

	unsigned char	inclWdg		:3;
	unsigned char	mEnable		:1;	//Enable status
	unsigned char	mLnk		:1;	//Widget as link
	unsigned char	mStlLock	:1;	//Style lock
	unsigned char	BACrtHoldOvr	:1;	//Base attrs creation hold over to enable and inherit stage
	unsigned char	ChldResrv	:1;	//Childs reserve attribute

	string		mParentNm;		//Parent widget name
	AutoHD<Widget>	mParent;		//Parent widget
	ResRW		mHeritRes;		//Heritators lock
	vector< AutoHD<Widget> > mHerit;	//Heritators
	map<string, Attr* >	mAttrs;
	ResMtx		mtxAttrM;
};

}

#endif //WIDGET_H
