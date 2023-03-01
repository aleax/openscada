
//OpenSCADA module Archive.Tmpl file: val.h
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
#ifndef VAL_H
#define VAL_H

// System includings - add need ones
#include <string>

// OpenSCADA API includings - add need ones
#include <tarchives.h>

using std::string;
using namespace OSCADA;

// All the module objects in own (individual) namespace - change for your module
namespace ArhTmpl
{

// Definition the element object of the value archiver of the subsystem "Archives-History" - add methods and attributes at your need
//************************************************
//* ArhTmpl::ModVArchEl - Value archive element  *
//************************************************
class ModVArch;

class ModVArchEl: public TVArchEl
{
    public:
	//Methods
	ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator );
	~ModVArchEl( );

	int64_t begin( )	{ return mBeg; }
	int64_t end( )		{ return mEnd; }
	int64_t period( )	{ return mPer; }

	ModVArch &archivator()	{ return (ModVArch&)TVArchEl::archivator(); }

    protected:
	//Methods
	TVariant getValProc( int64_t *tm, bool up_ord );
	void getValsProc( TValBuf &buf, int64_t beg, int64_t end );
	int64_t setValsProc( TValBuf &buf, int64_t beg, int64_t end, bool toAccum );

    private:
	//Attributes
	int64_t mBeg, mEnd, mPer;
};

// Definition the value archiver object of the subsystem "Archives-History" - add methods and attributes at your need
//************************************************
//* ArhTmpl::ModVArch - Value archivator         *
//************************************************
class ModVArch: public TVArchivator
{
    public:
	//Methods
	ModVArch( const string &iid, const string &idb, TElem *cf_el );
	~ModVArch( );

	void start( );
	void stop( bool full_del = false );

    protected:
	//Methods
	void load_( );
	void save_( );

	void cntrCmdProc( XMLNode *opt );

	TVArchEl *getArchEl( TVArchive &arch );
};

} //End the namespace ArhTmpl

#endif //VAL_H
