
//OpenSCADA system module BD.PostgreSQL file: postgre.h
/***************************************************************************
 *   Copyright (C) 2010 by Maxim Lysenko, mlisenko@oscada.org              *
 *                 2013-2016 by Roman Savochenko, rom_as@oscada.org        *
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

#ifndef POSTGRE_SQL_H
#define POSTGRE_SQL_H

#include <string>

#include <tmodule.h>
#include <tbds.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using namespace OSCADA;

namespace BDPostgreSQL
{

//************************************************
//* BDPostgreSQL::Table				 *
//************************************************
class MBD;
class MTable : public TTable
{
    public:
	//Public methods
	MTable( string name, MBD *iown, vector< vector<string> > *tblStrct = NULL );
	~MTable( );

	// Field's functions
	void fieldStruct( TConfig &cfg );
	bool fieldSeek( int row, TConfig &cfg );
	void fieldGet( TConfig &cfg );
	void fieldSet( TConfig &cfg );
	void fieldDel( TConfig &cfg );

	MBD &owner( );

    private:
	//Private methods
	bool isEmpty( );
	void postDisable( int flag );
	void fieldFix( TConfig &cfg, bool recurse = false );
	void fieldPrmSet( TCfg &cfg, const string &last, string &req );


	string getVal( TCfg &cfg, uint8_t RqFlg = 0 );
	void   setVal( TCfg &cfg, const string &vl, bool tr = false );

	string UTCtoSQL( time_t val );
	time_t SQLtoUTC( const string &val );

	//Private attributes
	vector< vector<string> > tblStrct;
};

//************************************************
//* BDPostgreSQL::MBD				 *
//************************************************
class BDMod;
class MBD : public TBD
{
    friend class MTable;

    public:
	//Data
	enum SQLerrs { SQL_CONN = 1, SQL_QUERY = 2 };

	//Public methods
	MBD( string iid, TElem *cf_el );
	~MBD( );

	void enable( );
	void disable( );

	void allowList( vector<string> &list );
	void sqlReq( const string &req, vector< vector<string> > *tbl = NULL, char intoTrans = EVAL_BOOL );

	void create( const string &nm, bool toCreate = false );
	void getStructDB( const string &nm, vector< vector<string> > &tblStrct );

	void transOpen( );
	void transCommit( );
	void transCloseCheck( );

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Private methods
	void postDisable( int flag );
	TTable *openTable( const string &name, bool create );

	//Private attributes
	string host, hostaddr, user, pass, db, port, connect_timeout, cd_pg, conninfo;
	PGconn	*connection;
	int	reqCnt;
	time_t	reqCntTm, trOpenTm;
	ResMtx	connRes;
};

//************************************************
//* BDPostgreSQL::BDMod                          *
//************************************************
class BDMod: public TTypeBD
{
    public:
	//Public methods
	BDMod( string name );
	~BDMod( );

	static string sqlReqCode( const string &req, char symb = '\'' );

    private:
	//Private methods
	TBD *openBD( const string &iid );
};

extern BDMod *mod;

}

#endif // POSTGRE_SQL_H
