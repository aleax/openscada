
//OpenSCADA system module BD.MySQL file: my_sql.h
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

#ifndef MY_SQL_H
#define MY_SQL_H

#include <string>

#include <tmodule.h>
#include <tbds.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using namespace OSCADA;

namespace BDMySQL
{

//************************************************
//* MBDMySQL::Table				 *
//************************************************
class MBD;
class MTable : public TTable
{
    public:
	//Public methods
	MTable( string name, MBD *iown, bool create );
	~MTable( );

	//> Field's functions
	void fieldStruct( TConfig &cfg );
	bool fieldSeek( int row, TConfig &cfg );
	void fieldGet( TConfig &cfg );
	void fieldSet( TConfig &cfg );
	void fieldDel( TConfig &cfg );

	MBD &owner( );

    private:
	//Private methods
	void postDisable( int flag );
	bool isEmpty( );
	void fieldFix( TConfig &cfg );
	void fieldPrmSet( TCfg &cfg, const string &last, string &req, int keyCnt = 1 );

	string getVal( TCfg &cfg );
	void   setVal( TCfg &cfg, const string &vl, bool tr = false );

	string UTCtoSQL( time_t val );
	time_t SQLtoUTC( const string &val );

	//Private attributes
	vector< vector<string> > tblStrct;
};

//************************************************
//* BDMySQL::MBD				 *
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

	void allowList( vector<string> &list );
	void sqlReq( const string &req, vector< vector<string> > *tbl = NULL, char intoTrans = EVAL_BOOL );

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
	string	host, user, pass, bd, u_sock, cd_pg, names;
	int	port;

	int	reqCnt;
	time_t	reqCntTm, trOpenTm;
	MYSQL	connect;
	pthread_mutex_t	connRes;
};

//************************************************
//* BDMySQL::BDMod                               *
//************************************************ 
class BDMod: public TTipBD
{
    public:
	//Public methods
	BDMod( string name );
	~BDMod( );

    protected:
	//Protected methods
	void load_( );

    private:
	//Private methods
	TBD *openBD( const string &iid );
};

extern BDMod *mod;

}

#endif // MY_SQL_H
