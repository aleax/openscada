
//OpenSCADA system module BD.SQLite file: bd_sqlite.h
/***************************************************************************
 *   Copyright (C) 2003-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef BD_SQLITE
#define BD_SQLITE

#include <string>
#include <vector>
#include <tmodule.h>
#include <tbds.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace BDSQLite
{

//************************************************
//* MBDMySQL::Table				 *
//************************************************
class MBD;
class MTable : public TTable
{
    public:
	//Public methods
	MTable( string name, MBD *bd );
	~MTable(  );

	// Field's operations
	void fieldStruct( TConfig &cfg );
	bool fieldSeek( int row, TConfig &cfg, vector< vector<string> > *full = NULL );
	void fieldGet( TConfig &cfg );
	void fieldSet( TConfig &cfg );
	void fieldDel( TConfig &cfg );

	MBD &owner( ) const;

    private:
	//Private methods
	void postDisable( int flag );
	void fieldFix( TConfig &cfg );

	string	getVal( TCfg &cf, uint8_t RqFlg = 0 );
	void	setVal( TCfg &cf, const string &vl, bool tr = false );

	//Private attributes
	vector< vector<string> > tblStrct;
};

//************************************************
//* BDSQLite::MBD				 *
//************************************************
class MBD : public TBD
{
    friend class MTable;
    public:
	//Public methods
	MBD( const string &iid, TElem *cf_el );
	~MBD(  );

	void enable( );
	void disable( );

	void allowList( vector<string> &list ) const;
	void sqlReq( const string &req, vector< vector<string> > *tbl = NULL, char intoTrans = EVAL_BOOL );

	void transOpen( );
	void transCommit( );
	void transCloseCheck( );

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

    private:
	//Private methods
	void postDisable(int flag);
	TTable *openTable( const string &name, bool create );

	//Private attributes
	string	cd_pg;
	sqlite3	*m_db;
	int	reqCnt;
	time_t	reqCntTm, trOpenTm;
	ResMtx	connRes;
	int	trans_reqs;
};

//*************************************************
//* BDSQLite::BDMod				  *
//*************************************************
class BDMod: public TTypeBD
{
    public:
	//Public methods
	BDMod( string name );
	~BDMod( );

	AutoHD<MBD> at( const string &name )	{ return TTypeBD::at(name); }

    protected:
	//Methods
	void modStop( );

    private:
	//Private methods
	TBD *openBD( const string &iid );
};

extern BDMod *mod;
}

#endif // BD_SQLITE
