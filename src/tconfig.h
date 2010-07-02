
//OpenSCADA system file: tconfig.h
/***************************************************************************
 *   Copyright (C) 2003-2009 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#include "telem.h"

using std::string;
using std::vector;
using std::map;

//*************************************************
//* TCfg                                          *
//*************************************************
class TConfig;

//#pragma pack(push,1)
class TCfg
{
    public:
	//Data
	enum AttrFlg
	{
	    TransltText = 0x100,	//String value type: Use translation variable texts mechanism
	    NoVal	= 0x200,	//No value mirrored
	    Key		= 0x400,	//Primary key
	    Hide	= 0x800		//Attribute hide
	};
	enum ReqFlg
	{
	    ForceUse	= 0x01,		//Force use flag
	};

	//Methods
	TCfg( TFld &fld, TConfig &owner );
	~TCfg( );

	const string &name( );

	bool operator==( TCfg &cfg );
	TCfg &operator=( TCfg &cfg );

	bool	view( )			{ return mView; }
	bool	keyUse( )		{ return mKeyUse; }
	bool	noTransl( )		{ return mNoTransl; }
	void	setView( bool vw )	{ mView = vw; }
	void	setKeyUse( bool vl )	{ if( fld().flg()&Key ) mKeyUse = vl; }
	void	setNoTransl( bool vl )	{ mNoTransl = vl; }

	TFld &fld( )			{ return *mFld; }

	//> Universal access
	string	getSEL( char RqFlg = 0 );
	string	getS( char RqFlg = 0 );
	double	getR( char RqFlg = 0 );
	int	getI( char RqFlg = 0 );
	bool	getB( char RqFlg = 0 );
	//> Direct access
	string	&getSd( );
	double	&getRd( );
	int	&getId( );
	bool	&getBd( );

	void setSEL( const string &val, char RqFlg = 0 );
	void setS( const string &val, char RqFlg = 0 );
	void setR( double val, char RqFlg = 0 );
	void setI( int val, char RqFlg = 0 );
	void setB( bool val, char RqFlg = 0 );

    private:
	//Data
	union
	{
	    ResString	*s_val;
	    double	r_val;
	    int		i_val;
	    bool	b_val;
	}m_val;

	//Attributes
	char mView	: 1;
	char mKeyUse	: 1;
	char mNoTransl	: 1;

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
	~TConfig( );

	TConfig &operator=( TConfig &cfg );

	void cfgList( vector<string> &list );
	bool cfgPresent( const string &n_val );
	TCfg &cfg( const string &n_val );
	TCfg *at( const string &n_val, bool noExpt = false );
	void cfgViewAll( bool val = true );	//Hide all no key elements
	void cfgKeyUseAll( bool val );

	TElem &elem( );
	void setElem( TElem *Elements, bool first = false );

	void cntrCmdMake( XMLNode *fld, const string &path, int pos,
		const string &user = "root", const string &grp = "root", int perm = 0664 );
        void cntrCmdProc( XMLNode *fld, const string &elem,
		const string &user = "root", const string &grp = "root", int perm = 0664 );

	bool noTransl( )	{ return mNoTransl; }
	void setNoTransl( bool vl )		{ mNoTransl = vl; }

    protected:
	//Methods
	virtual bool cfgChange( TCfg &cfg )	{ return true; }

	void detElem( TElem *el );
	void addFld( TElem *el, unsigned id );
	void delFld( TElem *el, unsigned id );

    private:
	//Attributes
	TCfgMap		value;
	TElem		*m_elem;
	char		single		: 1;
	char		mNoTransl	: 1;
};

#endif // TCONFIG_H
