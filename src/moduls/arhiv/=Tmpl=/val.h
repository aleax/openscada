
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module Archive.Tmpl file: val.h
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

//!!! Multi-including this header file prevent. Change for your include file name
#ifndef VAL_H
#define VAL_H

//!!! System's includings. Add need for your module includings.
#include <string>

//!!! OpenSCADA module's API includings. Add need for your module includings.
#include <tarchives.h>

using std::string;
using namespace OSCADA;

//!!! All module's object's include into self (individual) namespace. Change namespace for your module.
namespace ModTmpl
{

//************************************************
//* ModTmpl::ModVArchEl - Value archive element  *
//************************************************
class ModVArch;

class ModVArchEl: public TVArchEl
{
    public:
	//Methods
	//!!! Constructor for the object.
	ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator );
	//!!! Destructor for the object.
	~ModVArchEl( );

	//!!! Archive element's properties: begin, end and period of processed data
	int64_t begin( )	{ return mBeg; }
	int64_t end( )		{ return mEnd; }
	int64_t period( )	{ return mPer; }

	//!!! Direct link function to value archivator object
	ModVArch &archivator()	{ return (ModVArch&)TVArchEl::archivator(); }

    protected:
	//Methods
	//!!! Get and set data virtual functions for access to archive.
	TVariant getValProc( int64_t *tm, bool up_ord );
	void getValsProc( TValBuf &buf, int64_t beg, int64_t end );
	void setValsProc( TValBuf &buf, int64_t beg, int64_t end );

    private:
	//Attributes
	//!!! Archive element's properties: begin, end and period of processed data
	int64_t mBeg, mEnd, mPer;
};

//************************************************
//* ModTmpl::ModVArch - Value archivator         *
//************************************************
class ModVArch: public TVArchivator
{
    public:
	//Methods
	//!!! Constructor for the object.
	ModVArch( const string &iid, const string &idb, TElem *cf_el );
	//!!! Destructor for the object.
	~ModVArch( );

	//!!! Inherited (virtual) start and stop archivator functions
	void start( );
	void stop( );

    protected:
	//Methods
	//!!! Inherited (virtual) load and save object's node methods.
	void load_( );
	void save_( );

	//!!! OpenSCADA control interface comands process virtual function.
	void cntrCmdProc( XMLNode *opt );

	//!!! Inherited (virtual) function for create self value archive element
	TVArchEl *getArchEl( TVArchive &arch );
};

} //End namespace ModTmpl

#endif //VAL_H
