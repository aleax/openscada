
//!!! The module name, the file name and the module's license. Change for your need.
//OpenSCADA system module BD.Tmpl file: module.h
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily, <my@email.org>                 *
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

//!!! Multi-including of this header file prevent. Change it for your include file name
#ifndef MODULE_H
#define MODULE_H

//!!! System's includings. Add need for your module includings.
#include <string>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tmodule.h>
#include <tbds.h>

//!!! Individual module's translation function define. Don't change it!
#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using namespace OSCADA;

//!!! All the module's objects you should include into self (individual) namespace. Change the namespace for your module.
namespace BDTmpl
{

//!!! BD-subsystem table object realisation define. Add methods and attributes for your need
//************************************************
//* BDTmpl::Table				 *
//************************************************
class MBD;
class MTable : public TTable
{
    public:
	//Public methods
	//!!! Constructor for a DB-subsystem table object
	MTable( string name, MBD *iown );
	//!!! Destructor for a DB-subsystem table object
	~MTable( );

	// Field's functions
	//!!! Processing virtual function for getting the field's structure (value's type of the field and it's primary key flag)
	void fieldStruct( TConfig &cfg );
	//!!! Processing virtual functions for seeking, getting, setting and deleting a field
	bool fieldSeek( int row, TConfig &cfg, vector< vector<string> > *full = NULL );
	void fieldGet( TConfig &cfg );
	void fieldSet( TConfig &cfg );
	void fieldDel( TConfig &cfg );

	//!!! Back link to the table's owner database object
	MBD &owner( ) const;

    private:
	//Private methods
	//!!! Post disable processing function
	void postDisable( int flag );
	//!!! The table structure fixing function to the field
	void fieldFix( TConfig &cfg );
	//!!! The table structure obtain
	void getStructDB( string name, vector< vector<string> > &tblStrct );

	//!!! Get and set field item's value processing functions
	string getVal( TCfg &cfg, uint8_t RqFlg = 0 );
	void   setVal( TCfg &cfg, const string &vl, bool tr = false );

	//!!! Convert UTC time to SQL and vice-versa functions
	string UTCtoSQL( time_t val );
	time_t SQLtoUTC( const string &val );

	//Private attributes
	//!!! The table strucure attribute
	vector< vector<string> > tblStrct;
};

//!!! BD-subsystem's database object realisation define. Add methods and attributes for your need.
//************************************************
//* BDTmpl::MBD				 	 *
//************************************************
class BDMod;
class MBD : public TBD
{
    friend class MTable;

    public:
	//Public methods
	//!!! Constructor for a DB-subsystem database object
	MBD( string iid, TElem *cf_el );
	//!!! Destructor for a DB-subsystem database object
	~MBD( );

	//!!! Enable and disable the database object processing virtual functions
	void enable( );
	void disable( );

	//!!! Get the list of tables availabled in the database processing virtual function
	void allowList( vector<string> &list ) const;
	//!!! SQL request processing main virtual function
	void sqlReq( const string &req, vector< vector<string> > *tbl = NULL, char intoTrans = EVAL_BOOL );

	//!!! BEGIN and COMMIT a transaction processing functions
	void transOpen( );
	void transCommit( );

    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Private methods
	//!!! Post disable processing function
	void postDisable( int flag );
	//!!! Open a table processing function
	TTable *openTable( const string &name, bool create );

	//Private attributes
	//!!! The database attributes from the address
	string	host, hostaddr, user, pass, db, port, connectTimeout;
	//!!! The database code page
	string	cdPg;

	//!!! The database access resource
	pthread_mutex_t	connRes;
};

//!!! Root module object realisation define.
//************************************************
//* BDTmpl::BDMod                                *
//************************************************
class BDMod: public TTypeBD
{
    public:
	//Public methods
	//!!! Constructor for the Root module object.
	BDMod( string name );
	//!!! Destructor for the Root module object.
	~BDMod( );

    private:
	//Private methods
	//!!! Open DB processing function
	TBD *openBD( const string &iid );
};

//!!! The module root link
extern BDMod *mod;

}//End namespace BDTmpl

#endif // MODULE_H
