
//OpenSCADA system module Archive.DBArch file: val.h
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

#ifndef DB_VAL_H
#define DB_VAL_H

#include <string>

#include <tarchives.h>

using std::string;
using namespace OSCADA;

namespace DBArch
{

//************************************************
//* DBArch::ModVArchEl - Value archive element   *
//************************************************
class ModVArch;

class ModVArchEl: public TVArchEl
{
    public:
	//Methods
	ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator );
	~ModVArchEl( );

	string archTbl( );

	void fullErase( );

	int64_t begin( )	{ return mBeg; }
	int64_t end( )		{ return mEnd; }
	int64_t period( )	{ return mPer; }

	ModVArch &archivator()	{ return (ModVArch&)TVArchEl::archivator(); }

    protected:
	//Methods
	TVariant getValProc( int64_t *tm, bool up_ord );
	void getValsProc( TValBuf &buf, int64_t beg, int64_t end );
	bool setValsProc( TValBuf &buf, int64_t beg, int64_t end );

	bool readMeta( );

    private:
	//Attributes
	int64_t mBeg, mEnd, mPer;
	bool	needMeta;
};

//************************************************
//* DBArch::ModVArch - Value archivator          *
//************************************************
class ModVArch: public TVArchivator
{
    public:
	//Methods
	ModVArch( const string &iid, const string &idb, TElem *cf_el );
	~ModVArch( );

	double maxSize( )		{ return mMaxSize; }

	void setMaxSize( double vl )	{ mMaxSize = vl; modif(); }

	void start( );
	void stop( );

    protected:
	//Methods
	void load_( );
	void save_( );

	void cntrCmdProc( XMLNode *opt );

	TVArchEl *getArchEl( TVArchive &arch );

    private:
	//Attributes
	double	mMaxSize;			//Maximum archive size (hours)
};

}

#endif //DB_VAL_H
