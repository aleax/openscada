
//OpenSCADA system module Archive.DBArch file: val.h
/***************************************************************************
 *   Copyright (C) 2007-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <tarchives.h>

using namespace OSCADA;

namespace DBArch
{

//************************************************
//* DBArch::ModVArchEl - Value archive element   *
//************************************************
class ModVArch;

class ModVArchEl: public TVArchEl
{
    friend class ModVArch;
    public:
	//Methods
	ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator );
	~ModVArchEl( );

	string archTbl( );

	void fullErase( );

	int64_t begin( )	{ return mBeg; }
	int64_t end( )		{ return mEnd; }
	int64_t period( )	{ return mPer; }

	ModVArch &archivator( )	{ return (ModVArch&)TVArchEl::archivator(); }

    protected:
	//Methods
	TVariant getValProc( int64_t *tm, bool up_ord );
	void getValsProc( TValBuf &buf, int64_t beg, int64_t end );
	int64_t setValsProc( TValBuf &buf, int64_t beg, int64_t end, bool toAccum );

	bool readMeta( );

    private:
	//Attributes
	int64_t mBeg, mEnd, mPer;
	bool	needMeta;

	TElem	reqEl;
};

//************************************************
//* DBArch::ModVArch - Value archivator          *
//************************************************
class ModVArch: public TVArchivator
{
    public:
	//Data
	class SGrp
	{
	    public:
	    SGrp( ) : dbOK(true), pos(0), beg(0), end(0), per(0), accmBeg(0), accmEnd(0)	{ }
	    SGrp( int ipos ) : dbOK(true), pos(ipos), beg(0), end(0), per(0), accmBeg(0), accmEnd(0)	{ }

	    bool	dbOK;			//DB accessible and the group ready for next data
	    int		pos;			//Position
	    int64_t	beg, end, per,		//The grouped parameter's data range [beg:end] and period.
			accmBeg, accmEnd;	//Accumulated range
	    TElem	tblEl;			//Group's DB structure
	    map<string, TValBuf> els;		//Elements
	};

	//Methods
	ModVArch( const string &iid, const string &idb, TElem *cf_el );
	~ModVArch( );

	double maxSize( )		{ return mMaxSize; }
	bool tmAsStr( )			{ return mTmAsStr; }
	int groupPrms( )		{ return mGroupPrms; }

	void setMaxSize( double vl )	{ mMaxSize = (vl<0.1) ? 0 : vl; modif(); }
	void setTmAsStr( bool vl )	{ mTmAsStr = vl; modif(); }
	void setGroupPrms( int vl )	{ mGroupPrms = vmax(0,vmin(10000,vl)); modif(); }

	void start( );
	void stop( bool full_del = false );

	void checkArchivator( unsigned int cnt = 0 );

	string archTbl( int iG = -1 );	// -1 - for preffix to the archivator tables

	TValBuf &accmGetReg( const string &aNm, SGrp **grp = NULL, TFld::Type tp = TFld::Real, int prefGrpPos = -1 );
	void accmUnreg( const string &aNm );
	bool grpLimits( SGrp &oG, int64_t *beg = NULL, int64_t *end = NULL );
	void grpMetaUpd( SGrp &oG, const string *aLs = NULL );

	//Attributes
	bool	needMeta,			//Need meta mark
		needRePushGrps;			//Need to repush the groups, mostly at DB access errors
	ResMtx	reqRes;				//Request functional resource allocator, mostly for the grouping mode

    protected:
	//Methods
	void load_( );
	void save_( );

	void cntrCmdProc( XMLNode *opt );
	bool cfgChange( TCfg &co, const TVariant &pc );

	TVArchEl *getArchEl( TVArchive &arch );

	void pushAccumVals( );

    private:
	//Attributes
	double	mMaxSize;			//Maximum archive size (days)
	bool	mTmAsStr;			//Store time as pure integer
	int	mGroupPrms;			//Grouping parameters into single table, 0 - table per parameter

	vector<SGrp>	accm;
};

}

#endif //DB_VAL_H
