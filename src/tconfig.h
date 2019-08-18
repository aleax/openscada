
//OpenSCADA file: tconfig.h
/***************************************************************************
 *   Copyright (C) 2003-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef TCONFIG_H
#define TCONFIG_H

#include <string>
#include <vector>
#include <map>

#include "tvariant.h"
#include "telem.h"

using std::string;
using std::vector;
using std::map;

namespace OSCADA
{

//*************************************************
//* TCfg                                          *
//*************************************************
class TConfig;

//#pragma pack(push,1)
class TCfg : public TVariant
{
    public:
	//Data
	enum AttrFlg {
	    NoVal	= 0x100,	//Do not mirrore to value
	    Key		= 0x200,	//Primary key
	    Hide	= 0x400		//Hide attribute
	};
	enum ReqFlg {
	    ForceUse	= 0x01,		//Force use flag
	    ExtValOne	= 0x02,		//One/first (and default) value: set value for key
	    ExtValTwo	= 0x04,		//Two/second value: base value for the key request or set
	    ExtValThree	= 0x08		//Three value: source address for translations
	};

	//Methods
	TCfg( TFld &fld, TConfig &owner );
	TCfg( const TCfg &cfg );
	~TCfg( );

	const string &name( );

	bool	operator==( TCfg &cfg );
	TCfg	&operator=( const TCfg &cfg );

	bool	view( ) const		{ return mView; }
	bool	keyUse( ) const		{ return mKeyUse; }
	bool	noTransl( ) const	{ return mNoTransl; }
	bool	reqKey( ) const		{ return mReqKey; }
	bool	isKey( ) const;						//Whether real or request key test
	bool	extVal( ) const		{ return mExtVal; }
	void	setView( bool vw )	{ mView = vw; }
	void	setKeyUse( bool vl )	{ if(fld().flg()&Key) mKeyUse = vl; }
	void	setNoTransl( bool vl )	{ mNoTransl = vl; }
	void	setReqKey( bool vl );
	void	setExtVal( bool vw )	{ mExtVal = vw; }
	void	toDefault( );

	TFld	&fld( ) const		{ return *mFld; }

	// Universal access
	string	getSEL( );
	string	getS( ) const;
	string	getS( uint8_t RqFlg );
	operator bool( )		{ return getB(); }

	// Direct access. Use only for readonly config-fields by no resourced!
	const char *getSd( );
	double	&getRd( );
	int64_t	&getId( );
	char	&getBd( );

	void	setSEL( const string &val, uint8_t RqFlg = 0 );
	void	setS( const string &val );
	void	setS( const string &val, uint8_t RqFlg );
	void	setR( double val );
	void	setR( double val, uint8_t RqFlg );
	void	setI( int64_t val );
	void	setI( int64_t val, uint8_t RqFlg );
	void	setB( char val );
	void	setB( char val, uint8_t RqFlg );

	TCfg	&operator=( const string &vl )	{ setS(vl); return *this; }
	TCfg	&operator=( const char *vl )	{ setS(vl); return *this; }
	TCfg	&operator=( double vl )		{ setR(vl); return *this; }
	TCfg	&operator=( int vl )		{ setI(vl); return *this; }
	TCfg	&operator=( int64_t vl )	{ setI(vl); return *this; }
	TCfg	&operator=( bool vl )		{ setB(vl); return *this; }

	TConfig	&owner( ) const		{ return mOwner; }

    private:
	//Attributes
	uint8_t	mView		: 1;
	uint8_t	mKeyUse		: 1;
	uint8_t	mNoTransl	: 1;
	uint8_t	mReqKey		: 1;	//Request's key, mostly for WHERE sentence of SQL
	uint8_t	mExtVal		: 1;	//Double value mode: For key and translation processing,
					//where TVariant type force to String and value separated by one value (set for key)
					//and two value (base for key) by 0 symbol
	uint8_t	mInCfgCh	: 1;

	TFld	*mFld;
	TConfig	&mOwner;
};
//#pragma pack(pop)

//*************************************************
//* TConfig                                       *
//*************************************************
typedef map< string, TCfg* > TCfgMap;

class TConfig: public TValElem
{
    friend class TCfg;

    public:
	//Methods
	TConfig( TElem *Elements = NULL );
	TConfig( const TConfig &src );
	virtual ~TConfig( );

	TConfig &operator=( const TConfig &cfg );
	TConfig &exclCopy( const TConfig &cfg, const string &passCpLs = "", bool cpElsToSingle = false );

	void cfgList( vector<string> &list ) const;
	bool cfgPresent( const string &n_val ) const;
	TCfg &cfg( const string &n_val ) const;
	TCfg *at( const string &n_val, bool noExpt = false ) const;
	void cfgViewAll( bool val = true );	// Show/Hide all no key elements
	void cfgKeyUseAll( bool val );
	void cfgToDefault( );

	TElem &elem( )			{ return *mElem; }
	void setElem( TElem *Elements, bool first = false );

	void cntrCmdMake( XMLNode *fld, const string &path, int pos,
		const string &user = "root", const string &grp = "root", int perm = 0664 );
        void cntrCmdProc( XMLNode *fld, const string &elem,
		const string &user = "root", const string &grp = "root", int perm = 0664 );

	bool incomplTblStruct( )	{ return mIncmplTblStrct; }
	void setIncmplTblStrct( bool vl ) { mIncmplTblStrct = vl; }
	bool reqKeys( )			{ return mReqKeys; }

	TVariant objFunc( const string &id, vector<TVariant> &prms, const string &user, int perm, const string &owner );

    protected:
	//Methods
	virtual bool cfgChange( TCfg &co, const TVariant &pc )	{ return true; }

	void detElem( TElem *el );
	void addFld( TElem *el, unsigned id );
	void delFld( TElem *el, unsigned id );

	void reqKeysUpdate( );

	//Attributes
	ResMtx	mRes;

    private:
	//Attributes
	TCfgMap		value;
	TElem		*mElem;
	uint8_t		single		: 1;
	uint8_t		mIncmplTblStrct	: 1;
	uint8_t		mReqKeys	: 1;
};

}

#endif // TCONFIG_H
