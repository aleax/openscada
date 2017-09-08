
//OpenSCADA system module DB.ODBC file: mod_ldap.h
/***************************************************************************
 *   Copyright (C) 2017 by Roman Savochenko, <rom_as@oscada.org>           *
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

#ifndef MOD_LDAP_H
#define MOD_LDAP_H

#include <tmodule.h>
#include <tbds.h>

#undef _
#define _(mess) mod->I18N(mess)

using namespace OSCADA;

namespace DB_LDAP
{

//************************************************
//* DB_LDAP::Table				 *
//************************************************
class MBD;
class MTable : public TTable
{
    public:
	//Public methods
	MTable( string name, MBD *iown );
	~MTable( );

	// Field's functions
	void fieldStruct( TConfig &cfg );
	bool fieldSeek( int row, TConfig &cfg, vector< vector<string> > *full = NULL );
	void fieldGet( TConfig &cfg );
	void fieldSet( TConfig &cfg );
	//void fieldDel( TConfig &cfg );

	MBD &owner( ) const;

    private:
	//Private methods
	void postDisable( int flag );

	string getVal( TCfg &cf );
	void   setVal( TCfg &cf, const string &vl );

	//Private attributes
	string	codepage;
};

//************************************************
//* DB_LDAP::MBD				 *
//************************************************
class BDMod;
class MBD : public TBD
{
    friend class MTable;

    public:
	//Public methods
	MBD( string iid, TElem *cf_el );
	~MBD( );

	void enable( );
	void disable( );

	void allowList( vector<string> &list ) const;
	//void sqlReq( const string &req, vector< vector<string> > *tbl = NULL, char intoTrans = EVAL_BOOL );

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Private methods
	void postDisable( int flag );

	TTable *openTable( const string &name, bool create );

	//Private attributes
	string	bdn;	//Base DN

	// LDAP specific
	LDAP	*ldp;

	int	reqCnt;
	time_t	reqCntTm, trOpenTm;
	ResMtx	connRes;
};

//************************************************
//* DB_LDAP::BDMod				 *
//************************************************
class BDMod: public TTypeBD
{
    public:
	//Public methods
	BDMod( string name );
	~BDMod( );

    private:
	//Private methods
	TBD *openBD( const string &iid );
};

extern BDMod *mod;

}//End namespace DB_LDAP

#endif // MOD_LDAP_H
