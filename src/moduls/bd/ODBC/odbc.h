
//OpenSCADA system module BD.ODBC file: odbc.h
/***************************************************************************
 *   Copyright (C) 2015 by Roman Savochenko, <rom_as@oscada.org>           *
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

#ifndef ODBC_H
#define ODBC_H

#include <tmodule.h>
#include <tbds.h>

#undef _
#define _(mess) mod->I18N(mess)

using namespace OSCADA;

namespace BD_ODBC
{

//************************************************
//* BD_ODBC::Table				 *
//************************************************
class MBD;
class MTable : public TTable
{
    public:
	//Public methods
	MTable( string name, MBD *iown, bool create );
	~MTable( );

	// Field's functions
	//void fieldStruct( TConfig &cfg );
	//bool fieldSeek( int row, TConfig &cfg );
	//void fieldGet( TConfig &cfg );
	//void fieldSet( TConfig &cfg );
	//void fieldDel( TConfig &cfg );

	MBD &owner( );

    private:
	//Private methods
	void postDisable( int flag );
	//void fieldFix( TConfig &cfg );
	//void getStructDB( string name, vector< vector<string> > &tblStrct );

	//string getVal( TCfg &cfg, uint8_t RqFlg = 0 );
	//void   setVal( TCfg &cfg, const string &vl, bool tr = false );

	//string UTCtoSQL( time_t val );
	//time_t SQLtoUTC( const string &val );

	//Private attributes
	vector< vector<string> > tblStrct;
};

//************************************************
//* BD_ODBC::MBD				 *
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

	string errors( );
	string errState( );

	//Private attributes
	// ODBC specific
	HENV	henv;
	HDBC	hdbc;
	HSTMT	hstmt;
	SQLTCHAR outdsn[4096];

	int	reqCnt;
	time_t	reqCntTm, trOpenTm;
	pthread_mutex_t	connRes;
};

//************************************************
//* BD_ODBC::BDMod                                *
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

}//End namespace BD_ODBC

#endif // ODBC_H
