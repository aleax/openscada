//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module BD.Tmpl file: module.h
/***************************************************************************
 *   Copyright (C) 2012 by MyName MyFamily                                 *
 *   my@email.org                                                          *
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

//!!! All module's objects you must include into self (individual) namespace. Change namespace for your module.
namespace BDTmpl
{

//!!! BD-subsystem table object realisation define. Add methods and attributes for your need.
//************************************************
//* BDTmpl::Table				 *
//************************************************
class MBD;
class MTable : public TTable
{
    public:
	//Public methods
        //!!! Constructor for DB-subsystem table object.
	MTable( string name, MBD *iown, bool create );
        //!!! Destructor for DB-subsystem table object.
	~MTable( );

	//> Field's functions
        //!!! Processing virtual function for getting the field's structure(value's type of the field and it's primary key flag )
	void fieldStruct( TConfig &cfg );
        //!!! Processing virtual functions for seeking, getting, setting and deleting the field
	bool fieldSeek( int row, TConfig &cfg );
	void fieldGet( TConfig &cfg );
	void fieldSet( TConfig &cfg );
	void fieldDel( TConfig &cfg );

        //!!! Direct link to table's owner database
	MBD &owner( );

    private:
	//Private methods
        //!!! Processing the postDisable function
	void postDisable(int flag);
        //!!! Processing the fieldFix function
	void fieldFix( TConfig &cfg );
        //!!! Processing the getStructDB function
        void getStructDB( string name, vector< vector<string> > &tblStrct );
        
        //!!! Processing the getVal and setVal functions
	string getVal( TCfg &cfg );
	void   setVal( TCfg &cfg, const string &vl );

        //!!! Processing the UTCtoSQL and SQLtoUTC functions
	string UTCtoSQL( time_t val );
	time_t SQLtoUTC( const string &val );

	//Private attributes
        //!!! The table strucure attribute
	vector< vector<string> > tblStrct;
};

//!!! BD-subsystem database object realisation define. Add methods and attributes for your need.
//************************************************
//* BDTmpl::MBD				 	 *
//************************************************
class BDMod;
class MBD : public TBD
{
    friend class MTable;

    public:
	//Public methods
        //!!! Constructor for DB-subsystem database object.
	MBD( string iid, TElem *cf_el );
        //!!! Destructor for DB-subsystem database object.
	~MBD(  );

        //!!! Processing virtual functions for enable and disable database
	void enable( );
	void disable( );

        //!!! Processing virtual function to get the list of tables available in the database
	void allowList( vector<string> &list );
        //!!! Processing virtual function to send the request to the  database and to process its result
	void sqlReq( const string &req, vector< vector<string> > *tbl = NULL );

        //!!! Processing functions to BEGIN and COMMIT the transaction
        void transOpen( );
        void transCommit( );


    protected:
	//Protected methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Private methods
        //!!! Processing the postDisable function
	void postDisable(int flag);
        //!!! Processing the openTable function
	TTable *openTable( const string &name, bool create );


	//Private attributes
        //!!! Address of the database attributes
        string host, hostaddr, user, pass, db, port, connect_timeout;
        //!!! Code page of the database
        string cd_pg;

        //!!! The resource for the access to the database.
	Res    conn_res;
};

//!!! Root module module object realisation define.
//************************************************
//* BDTmpl::BDMod                                *
//************************************************
class BDMod: public TTipBD
{
    public:
	//Public methods
        //!!! Constructor for Root module object.
	BDMod( string name );
        //!!! Destructor for Root module object.
	~BDMod();

    private:
	//Private methods
        //!!! Processing the openBD function
	TBD *openBD( const string &iid );
};

//!!! The module root link
extern BDMod *mod;

}//End namespace BDTmpl

#endif // MODULE_H
