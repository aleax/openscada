
//OpenSCADA system file: tvalue.h
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef TVALUE_H
#define TVALUE_H

#include <string>
#include <vector>

#include "terror.h"
#include "tvariant.h"
#include "tconfig.h"

using std::string;
using std::vector;

namespace OSCADA
{

//*************************************************
//* TVal                                          *
//*************************************************
class TValue;
class TVArchive;

class TVal: public TCntrNode
{
    public:
	//Data
	enum AttrFlg {
	    DirRead  = 0x100,
	    DirWrite = 0x200
	};

	//Methods
	TVal( );
	TVal( TFld &fld );
	TVal( TCfg &cfg );
	~TVal( );

	string objName( );

	string DAQPath( );

	void setFld( TFld &fld );
	void setCfg( TCfg &cfg );

	string	name( );
	int64_t time( )	{ return mTime; }
	bool	dataActive( );

	//> Read current value (direct)
	string	getSEL( int64_t *tm = NULL, bool sys = false );
	TVariant get( int64_t *tm = NULL, bool sys = false );
	string	getS( int64_t *tm = NULL, bool sys = false );
	double	getR( int64_t *tm = NULL, bool sys = false );
	int64_t	getI( int64_t *tm = NULL, bool sys = false );
	char	getB( int64_t *tm = NULL, bool sys = false );
	AutoHD<TVarObj> getO( int64_t *tm = NULL, bool sys = false );

	//> Set current value
	void setSEL( const string &value, int64_t tm = 0, bool sys = false );
	void set( const TVariant &value, int64_t tm = 0, bool sys = false );
	void setS( const string &value, int64_t tm = 0, bool sys = false );
	void setR( double value, int64_t tm = 0, bool sys = false );
	void setI( int64_t value, int64_t tm = 0, bool sys = false );
	void setB( char value, int64_t tm = 0, bool sys = false );
	void setO( AutoHD<TVarObj> value, int64_t tm = 0, bool sys = false );

	AutoHD<TVArchive> arch( );
	void setArch( const AutoHD<TVArchive> &vl );
	string setArch( const string &nm = "" );

	bool reqFlg( )			{ return mReqFlg; }
	bool resB1( )			{ return mResB1; }
	bool resB2( )			{ return mResB2; }

	void setReqFlg( bool vl )	{ mReqFlg = vl; }
	void setResB1( bool vl )	{ mResB1 = vl; }
	void setResB2( bool vl )	{ mResB2 = vl; }

	TValue &owner( );
	TFld &fld( );

    protected:
	//Methods
	void preDisable( int flag );
	void cntrCmdProc( XMLNode *opt );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Methods
	const char *nodeName( );

	//Attributes
	union {
	    string	*s;		//String value
	    double	r;		//Real value
	    int64_t	i;		//Integer value
	    char	b;		//Boolean value
	    AutoHD<TVarObj> *o;		//Object value
	} val;

	unsigned char	mCfg	: 1;		//Configuration id
	unsigned char	mReqFlg	: 1;		//Request to attribute flag
	unsigned char	mResB1	: 1;		//Reserve Boolean
	unsigned char	mResB2	: 1;		//Reserve Boolean
	union {
	    TFld *fld;
	    TCfg *cfg;
	} src;
	int64_t mTime;		//Last value's time (usec)
	AutoHD<TVArchive>	mArch;
};

//*************************************************
//* TValue                                        *
//*************************************************
class TValue: public TCntrNode, public TValElem
{
    friend class TVal;

    public:
	TValue( );
	virtual ~TValue();

	virtual bool dataActive( )	{ return false; }

	string objName( );

	virtual string DAQPath( );

	//> Atributes
	void vlList( vector<string> &list )	{ chldList(m_vl, list); }
	bool vlPresent( const string &name )	{ return chldPresent(m_vl, name); }
	AutoHD<TVal> vlAt( const string &name )	{ return chldAt(m_vl, name); }

    protected:
	//Methods
	void chldAdd( int8_t igr, TCntrNode *node, int pos = -1, bool noExp = false );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//> Manipulation for configuration element
	TConfig *vlCfg( )			{ return mCfg; }
	void setVlCfg( TConfig *cfg );		//Set configs. NULL - clear configurations.

	//> Manipulation for elements of value
	bool vlElemPresent( TElem *ValEl );
	void vlElemAtt( TElem *ValEl );
	void vlElemDet( TElem *ValEl );
	TElem &vlElem( const string &name );

	virtual TVal* vlNew( );
	virtual void vlGet( TVal &vo )		{ };
	virtual void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl )		{ };
	virtual void vlArchMake( TVal &val )	{ };

    private:
	//Methods
	//> TElem commands
	void detElem( TElem *el );
	void addFld( TElem *el, unsigned id_val);
	void delFld( TElem *el, unsigned id_val);

	//Attributes
	char		m_vl;
	vector<TElem*>	elem;		// Elements (dinamic parts)

	short int	l_cfg;		// Configuration len
	TConfig*	mCfg;		// Configurations (static parts)
};

}

#endif // TVALUE_H
