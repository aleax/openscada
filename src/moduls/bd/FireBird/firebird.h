
//OpenSCADA system module BD.FireBird file: firebird.h
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef FIREBIRD_H
#define FIREBIRD_H

#include <string>
#include <vector>
#include <tmodule.h>
#include <tbds.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace FireBird
{

//************************************************
//* FireBird::Table				 *
//************************************************
class MBD;
class MTable : public TTable
{
    public:
	//Public methods
	MTable( string name, MBD *bd, bool create );
	~MTable( );

	// Field's operations
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
	void fieldFix( TConfig &cfg );
	void getStructDB( vector< vector<string> > &tblStrct );

	string getVal( TCfg &cfg );
	void   setVal( TCfg &cfg, const string &vl, bool tr = false );

	//Private attributes
	vector< vector<string> > tblStrct;
};

//************************************************
//* FireBird::MBD				 *
//************************************************
class MBD : public TBD
{
    friend class MTable;
    public:
	//Public methods
	MBD( const string &iid, TElem *cf_el );
	~MBD( );

	void enable( );
	void disable( );

	void allowList( vector<string> &list );
	void sqlReq( const string &req, vector< vector<string> > *tbl = NULL, char intoTrans = EVAL_BOOL );
	string clrEndSpace( const string &vl );

	void transOpen( );
	void transCommit( );
	void transCloseCheck( );

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    private:
	//Private methods
	void postDisable( int flag );
	TTable *openTable( const string &name, bool create );

	string getErr( ISC_STATUS_ARRAY status );

	//Private attributes
	string		user, pass, conTm, fdb, cd_pg;
	isc_db_handle	hdb;
	isc_tr_handle	htrans;
	int		reqCnt;
	time_t		reqCntTm, trOpenTm;
	pthread_mutex_t	connRes;
};

//*************************************************
//* FireBird::BDMod				  *
//*************************************************
class BDMod: public TTipBD
{
    public:
	//Public methods
	BDMod( string name );
	~BDMod( );

	static string sqlReqCode( const string &req, char symb = '\'' );

    protected:
	//Protected methods
	void load_( );

    private:
	//Private methods
	TBD *openBD( const string &iid );
};

extern BDMod *mod;
}

#endif // FIREBIRD
