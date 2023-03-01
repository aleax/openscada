
//OpenSCADA module BD.Tmpl file: module.h
/***************************************************************************
 *   Copyright (C) 2022 by MyName MyFamily, <my@email.org>                 *
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

// Preventing of the header file multi-including - change at the header file name changing
#ifndef MODULE_H
#define MODULE_H

// System includings - add need ones
#include <string>

// OpenSCADA API includings - add need ones
#include <tbds.h>

// Definition of the individual module translation functions - do not change
#undef _
#define _(mess) mod->I18N(mess).c_str()
#undef trS
#define trS(mess) mod->I18N(mess,mess_PreSave)

using std::string;
using namespace OSCADA;

// All the module objects in own (individual) namespace - change for your module
namespace BDTmpl
{

// Definition the table object of the subsystem "DB" - add methods and attributes at your need
//************************************************
//* BDTmpl::Table				 *
//************************************************
class MBD;
class MTable : public TTable
{
    public:
	//Public methods
	MTable( string name, MBD *iown );
	~MTable( );

	void fieldStruct( TConfig &cfg );
	bool fieldSeek( int row, TConfig &cfg, const string &cacheKey = "" );
	void fieldGet( TConfig &cfg );
	void fieldSet( TConfig &cfg );
	void fieldDel( TConfig &cfg );

	void fieldFix( TConfig &cfg, const string &langLs = "" );

	MBD &owner( ) const;

    private:
	//Private methods
	void postDisable( int flag );

	string	getSQLVal( TCfg &cf, uint8_t RqFlg = 0 );
	void	setSQLVal( TCfg &cf, const string &vl, bool tr = false );
};

// Definition the database object of the subsystem "DB" - add methods and attributes at your need
//************************************************
//* BDTmpl::MBD				 	 *
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
	void sqlReq( const string &req, vector< vector<string> > *tbl = NULL, char intoTrans = EVAL_BOOL );

	void transOpen( );
	void transCommit( );
	void transCloseCheck( );

	void getStructDB( const string &nm, vector<TTable::TStrIt> &tblStrct );

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Private methods
	void postDisable( int flag );
	TTable *openTable( const string &name, bool create );

	//Private attributes
	int	reqCnt;
	int64_t	reqCntTm, trOpenTm;
};

// Definition the root module object of the subsystem "DB" - add methods and attributes at your need
//************************************************
//* BDTmpl::BDMod                                *
//************************************************
class BDMod: public TTypeBD
{
    public:
	//Public methods
	BDMod( string name );
	~BDMod( );

	int lsPr( )	{ return 3; }
	string features( );

    private:
	//Private methods
	TBD *openBD( const string &iid );
};

extern BDMod *mod;	//The module root link

} //End the namespace BDTmpl

#endif // MODULE_H
