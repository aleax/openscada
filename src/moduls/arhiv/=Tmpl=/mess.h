
//!!! Module name, file name and module's license. Change for your need.
//OpenSCADA system module Archive.Tmpl file: mess.h
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
#ifndef MESS_H
#define MESS_H

//!!! System's includings. Add need for your module includings.
#include <string>

using std::string;
using namespace OSCADA;

//!!! All module's object's include into self (individual) namespace. Change namespace for your module.
namespace ModTmpl
{

//************************************************
//* ModTmpl::ModMArch - Messages archivator      *
//************************************************
class ModMArch: public TMArchivator
{
    public:
	//Methods
	//!!! Constructor for the object.
	ModMArch( const string &id, const string &db, TElem *cf_el );
	//!!! Destructor for the object.
	~ModMArch( );

	//!!! Archive element's properties: begin and end of processed data
	time_t begin( )	{ return mBeg; }
	time_t end( )	{ return mEnd; }

	//!!! Get and put data virtual functions for access to archive.
	void put( vector<TMess::SRec> &mess );
	void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category = "", char level = 0, time_t upTo = 0 );

	//!!! Inherited (virtual) start and stop archivator functions
	void start( );
	void stop( );

    protected:
	//Methods
	//!!! Inherited (virtual) load and save object's node methods.
	void load_( );
	void save_( );

	//!!! Module's post disable call.
	void postDisable( int flag );		//Delete all DB if flag 1
	//!!! OpenSCADA control interface comands process virtual function.
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Attributes
	//!!! Archive element's properties: begin and end of processed data
	time_t	mBeg, mEnd;
};

} //End namespace ModTmpl

#endif //MESS_H
