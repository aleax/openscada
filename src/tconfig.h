
//OpenSCADA system file: tconfig.h
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

class TCfg
{
    public:
	//Data
	enum AttrFlg
	{
	    NoVal	= 0x200,	//No value mirrored
	    Key		= 0x400,	//Primary key
	    Hide	= 0x800		//Attribute hide
	};

	//Methods
	TCfg( TFld &fld, TConfig &owner );
	~TCfg( );

	const string &name( );

	bool operator==( TCfg &cfg );
	TCfg &operator=( TCfg &cfg );

	bool	view( )			{ return mView; }
	bool	keyUse( )		{ return mKeyUse; }
	void	setView( bool vw )	{ mView = vw; }
	void	setKeyUse( bool vl )	{ if( fld().flg()&Key ) mKeyUse = vl; }

	TFld &fld( )			{ return *mFld; }

	//- Universal access -
	string	getSEL( );
	string	getS( );
	double	getR( );
	int	getI( );
	bool	getB( );
	//- Direct access -
	string	&getSd( );
	double	&getRd( );
	int	&getId( );
	bool	&getBd( );

	void setSEL( const string &val, bool forcUse = false );
	void setS( const string &val, bool forcUse = false );
	void setR( double val, bool forcUse = false );
	void setI( int val, bool forcUse = false );
	void setB( bool val, bool forcUse = false );

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

	TFld	*mFld;
	TConfig	&mOwner;
};

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
	void cfgViewAll( bool val = true );	//Hide all no key elements
	void cfgKeyUseAll( bool val );

	TElem &elem( );
	void setElem( TElem *Elements, bool first = false );

	void cntrCmdMake( XMLNode *fld, const string &path, int pos,
		const string &user = "root", const string &grp = "root", int perm = 0664 );
        void cntrCmdProc( XMLNode *fld, const string &elem,
		const string &user = "root", const string &grp = "root", int perm = 0664 );

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
	bool		single;
};

#endif // TCONFIG_H
