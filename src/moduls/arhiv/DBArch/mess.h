
//OpenSCADA module Archive.DBArch file: mess.h
/***************************************************************************
 *   Copyright (C) 2007-2019 by Roman Savochenko, <rom_as@oscada.org>      *
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

	TCntrNode &operator=( const TCntrNode &node );

	string archTbl( )		{ return "DBAMsg_"+id(); }
	double maxSize( )		{ return mMaxSize; }	//In days
	bool tmAsStr( )			{ return mTmAsStr; }

	void setMaxSize( double vl )	{ mMaxSize = (vl<0.1) ? 0 : vl; modif(); }
	void setTmAsStr( bool vl )	{ mTmAsStr = vl; modif(); }

	time_t begin( );
	time_t end( );

	bool put( vector<TMess::SRec> &mess, bool force = false );
	time_t get( time_t bTm, time_t eTm, vector<TMess::SRec> &mess, const string &category = "", char level = 0, time_t upTo = 0 );

	void start( );
	void stop( );

    protected:
	//Methods
	void load_( );
	void save_( );

	void postDisable( int flag );		//Delete all DB if flag 1
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	bool readMeta( );

    private:
	//Attributes
	float	tmProc, tmProcMax;		//Archiving-processing time
	time_t	mBeg, mEnd;
	double	mMaxSize;			//Maximum archive size (hours)
	bool	mTmAsStr,			//Store time as pure integer
		needMeta;

	TElem	reqEl;				//Requests structure
};

}

#endif //DB_MESS_H
