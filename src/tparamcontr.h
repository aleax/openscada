
//OpenSCADA file: tparamcontr.h
/***************************************************************************
 *   Copyright (C) 2003-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef TPARAMCONTR_H
#define TPARAMCONTR_H

#include <time.h>
#include <string>
using std::string;

#include "tcntrnode.h"
#include "tvalue.h"

namespace OSCADA
{

//*************************************************
//* TParamContr                                   *
//*************************************************
class TController;
class TTypeParam;

class TParamContr : public TConfig, public TValue
{
    public:
	//Public data
	enum EnDisFlag {
	    NodeRemove_NoArch = TCntrNode::NodeSpecific
	};

	//Methods
	TParamContr( const string &name, TTypeParam *tpprm );
	virtual ~TParamContr( );

	string objName( );

	string DAQPath( );
	string ownerPath( bool inclSelf = false );	//Owner parameter path

	TCntrNode &operator=( const TCntrNode &node );

	bool operator==( TParamContr &PrmCntr )	{ return (id() == PrmCntr.id()); }

	string	id( )			{ return mId.c_str(); }
	string	name( );
	string	descr( );
	bool toEnable( )		{ return cfg("EN").getB(); }
	int64_t	timeStamp( );
	bool enableStat( ) const	{ return mEn; }
	bool dataActive( );

	void setName( const string &inm );
	void setDescr( const string &idsc );
	void setToEnable( bool vl )		{ cfg("EN").setB(vl); }

	// Included parameters
	void list( vector<string> &list ) const;
	bool present( const string &id ) const;
	string add( const string &id, unsigned type = 0 );
	void del( const string &id, int flags = NodeNoFlg );
	AutoHD<TParamContr> at( const string &id, const string &who = "th_prm" ) const;

	TTypeParam &type( ) const	{ return *tpParm; }

	virtual TElem *dynElCntr( )	{ return NULL; }

	virtual void enable( );			// Enable parameter and open access to value
	virtual void disable( );		// Disable parameter and close access to value

	TController &owner( ) const;

	//Attributes
	time_t	mRdPrcTm;	//Redundancy processing time, mostly for the dynamic DAQ attributes

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void load_( TConfig *cfg );
	void save_( );
	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );

	bool cfgChange( TCfg &co, const TVariant &pc );

	void vlGet( TVal &vo );
	void vlSet( TVal &vo, const TVariant &vl, const TVariant &pvl );
	bool vlSetRednt( TVal &vo, const TVariant &vl, const TVariant &pvl );
	void vlArchMake( TVal &val );

	virtual void setType( const string &tpId );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user_lang );

    private:
	//Methods
	const char *nodeName( ) const	{ return mId.c_str(); }

	void LoadParmCfg( );

	//Attributes
	int8_t	mPrm;		//Included parameters group identifier
	bool	mEn;
	string	mId;

	TTypeParam	*tpParm;
};

}

#endif // TPARAMCONTR_H
