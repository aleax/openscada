
//OpenSCADA system module Archive.DBArch file: mess.h
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

#ifndef DB_MESS_H
#define DB_MESS_H

#include <string>

using std::string;
using namespace OSCADA;

namespace DBArch
{

//************************************************
//* DBArch::ModMArch - Messages archivator       *
//************************************************
class ModMArch: public TMArchivator
{
    public:
	//Methods
	ModMArch( const string &id, const string &db, TElem *cf_el );
	~ModMArch( );

	string archTbl( )		{ return "DBAMsg_"+id(); }
	double maxSize( )		{ return mMaxSize; }

	void setMaxSize( double vl )	{ mMaxSize = vl; modif(); }

	time_t begin( );
	time_t end( );

	bool put( vector<TMess::SRec> &mess );
	void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category = "", char level = 0, time_t upTo = 0 );

	void start( );
	void stop( );

    protected:
	//Methods
	void load_( );
	void save_( );

	void postDisable( int flag );		//Delete all DB if flag 1
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Attributes
	double	tm_calc;			//Archiving time
	time_t	mBeg, mEnd;
	double	mMaxSize;			//Maximum archive size (hours)
};

}

#endif //DB_MESS_H
