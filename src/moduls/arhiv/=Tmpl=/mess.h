
//OpenSCADA module Archive.Tmpl file: mess.h
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
#ifndef MESS_H
#define MESS_H

// System includings - add need ones
#include <string>

using std::string;
using namespace OSCADA;

// All the module objects in own (individual) namespace - change for your module
namespace ArhTmpl
{

// Definition the message archiver object of the subsystem "Archives-History" - add methods and attributes at your need
//************************************************
//* ArhTmpl::ModMArch - Messages archiver        *
//************************************************
class ModMArch: public TMArchivator
{
    public:
	//Methods
	ModMArch( const string &id, const string &db, TElem *cf_el );
	~ModMArch( );

	time_t begin( )	{ return mBeg; }
	time_t end( )	{ return mEnd; }

	bool put( vector<TMess::SRec> &mess, bool force = false );
	time_t get( time_t bTm, time_t eTm, vector<TMess::SRec> &mess, const string &category = "", char level = 0, time_t upTo = 0 );

	void start( );
	void stop( );

    protected:
	//Methods
	void load_( );
	void save_( );

	void postDisable( int flag );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Attributes
	time_t	mBeg, mEnd;
};

} //End the namespace ArhTmpl

#endif //MESS_H
