
//OpenSCADA system module Archive.FSArch file: mess.h
/***************************************************************************
 *   Copyright (C) 2003-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef FS_MESS_H
#define FS_MESS_H

#include <deque>
#include <string>

#define CACHE_POS 100

using std::string;
using std::deque;
using namespace OSCADA;

namespace FSArch
{
//*************************************************
//* FSArch::MFileArch - Messages archivator file  *
//*************************************************
class ModMArch;

class MFileArch
{
    public:
	//Methods
	MFileArch( ModMArch *owner );
	MFileArch( const string &name, time_t beg, ModMArch *owner, const string &charset = "UTF-8", bool ixml = true);
	~MFileArch( );
	void delFile();

	void attach( const string &name, bool full = true );
	bool put( TMess::SRec mess );
	time_t get( time_t bTm, time_t eTm, vector<TMess::SRec> &mess, const string &category, char level, time_t upTo = 0 );
	// Write changes to Archive file
	//  free - surely free used memory
	void check( bool free = false );

	string	name( )		{ return mName; }
	bool	xmlM( )		{ return mXML; }
	int	size( )		{ return mSize; }
	time_t	begin( )	{ return mBeg; }
	time_t	end( )		{ return mEnd; }
	string	charset( )	{ return mChars; }
	bool	err( )		{ return mErr; }
	bool	isPack( )	{ return mPack; }

	ModMArch &owner( ) const{ return *mOwner; }

	//Attributes
	bool	scan;		// Archive scaned (for check deleted files). Use from ModMArch

    private:
	// Cache methods
	long	cacheGet( int64_t tm );
	void	cacheSet( int64_t tm, long off, bool last = false );
	void	cacheUpdate( int64_t tm, long v_add );

	// Base parameters
	ResMtx	dtRes;

	MtxString mName;	// name Archive file;
	bool	mXML;		// XML mode file
	int	mSize;		// Archive size
	string	mChars;		// Archive charset;
	// State parameters
	bool	mErr;		// Archive err
	bool	mWrite;		// Archive had changed but no wrote to file
	bool	mLoad;		// Archive load to mNode
	bool	mPack;		// Archive packed
	time_t	mAcces;		// last of time acces to Archive file
	time_t	mBeg;		// begin Archive file;
	time_t	mEnd;		// end Archive file;
	// XML-mode parametrs
	XMLNode	*mNode;		// XML-file tree
	// Cache parameters
	struct CacheEl {
	    int64_t	tm;
	    long	off;
	};
	vector<CacheEl> cache;
	CacheEl cach_pr;
	// Specific parameters
	ResRW	mRes;		// resource to access;
	ModMArch *mOwner;
    };

//************************************************
//* FSArch::ModMArch - Messages archivator       *
//************************************************
class ModMArch: public TMArchivator
{
    public:
	//Methods
	ModMArch( const string &iid, const string &idb, TElem *cf_el );
	~ModMArch( );

	string infoDBnm( );

	time_t begin();
	time_t end();
	bool put( vector<TMess::SRec> &mess, bool force = false );
	time_t get( time_t bTm, time_t eTm, vector<TMess::SRec> &mess, const string &category = "", char level = 0, time_t upTo = 0 );
	void start();
	void stop();

	int  size();

	bool useXML( )		{ return mUseXml; }
	int  maxSize( )		{ return mMaxSize; }
	int  numbFiles( )	{ return mNumbFiles; }
	int  timeSize( )	{ return mTimeSize; }
	int  checkTm( )		{ return mChkTm; }
	int  packTm( )		{ return mPackTm; }
	bool packInfoFiles( )	{ return mPackInfoFiles; }
	bool prevDbl( )		{ return mPrevDbl; }
	bool prevDblTmCatLev( )	{ return mPrevDblTmCatLev; }

	void setUseXML( bool vl )	{ mUseXml = vl; modif(); }
	void setMaxSize( int vl )	{ mMaxSize = vl; modif(); }
	void setNumbFiles( int vl )	{ mNumbFiles = vl; modif(); }
	void setTimeSize( int vl )	{ mTimeSize = vl; modif(); }
	void setCheckTm( int vl )	{ mChkTm = vl; modif(); }
	void setPackTm( int vl )	{ mPackTm = vl; modif(); }
	void setPackInfoFiles( bool vl ){ mPackInfoFiles = vl; modif(); }
	void setPrevDbl( bool vl )	{ mPrevDbl = vl; modif(); }
	void setPrevDblTmCatLev( bool vl ) { mPrevDblTmCatLev = vl; modif(); }

	void checkArchivator( bool now = false );

	//Attributes
	MtxString	infoTbl;

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	bool	mUseXml;	// use XML for archive files
	int	mMaxSize,	// maximum size kb of Archives file
		mNumbFiles,	// number of Archive files
		mTimeSize,	// number days to one file
		mChkTm,		// period of check archive files directory;
		mPackTm;	// pack the archive files timeout
	bool	mPackInfoFiles,	// use info files for packed archives
		mPrevDbl,	// duple messages prevent
		mPrevDblTmCatLev;// mean and prevent to duples by time, category and level

	ResRW	mRes;		// resource to access;
	double	tmCalc;		// Archiving time
	time_t	mLstCheck;	// Last check directory time

	deque<MFileArch *>	files;
};

}

#endif //FS_MESS_H
