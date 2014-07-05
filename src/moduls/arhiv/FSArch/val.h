
//OpenSCADA system module Archive.FSArch file: val.h
/***************************************************************************
 *   Copyright (C) 2003-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef FS_VAL_H
#define FS_VAL_H

#include <deque>
#include <string>

#include <tarchives.h>

#define VAL_CACHE_POS 160000

using std::string;
using std::deque;
using namespace OSCADA;

namespace FSArch
{
//************************************************
//* FSArch::VFileArch - Value archivator file    *
//************************************************
class ModVArchEl;

class VFileArch
{
    public:
	//Methods
	VFileArch( ModVArchEl *owner );
	VFileArch( const string &iname, int64_t ibeg, int64_t iend, int64_t iper, TFld::Type itp, ModVArchEl *owner);
	~VFileArch();
	void delFile();

	void attach( const string &name );

	string	&name( )	{ return mName; }
	int	size( )		{ return mSize; }
	int64_t	begin( )	{ return mBeg; }
	int64_t	end( )		{ return mEnd; }
	int64_t	period( )	{ return mPer; }
	int64_t	endData( );
	TFld::Type type( )	{ return mTp; }
	bool	err( )		{ return mErr; }
	bool	isPack( )	{ return mPack; }

	void	setVals( TValBuf &buf, int64_t beg, int64_t end );
	void	getVals( TValBuf &buf, int64_t beg, int64_t end );
	TVariant getVal( int pos );

	ModVArchEl &owner()	{ return *mOwner; }

	void check( );

	int maxPos( )		{ return mpos; }

	//Attributes
	static string afl_id;
	struct FHead
	{
	    char		f_tp[20];	//Archive system name ("OpenSCADA Val Arch.")
	    char		archive[20];	//Value archive name
	    int64_t		beg;		//Time archive begin
	    int64_t		end;		//Time archive end
	    int64_t		period;		//Time value period
	    unsigned char	vtp   :3;	//Value type (bool, int, real, string)
	    unsigned char	hgrid :1;	//Hard griding flag (reserved)
	    unsigned char	hres  :1;	//High resolution value time (reserved)
	    unsigned char	vtpExt:3;	//Value type extension (int16, int32, int64, float(4), double(8))
	    char		reserve[14];	//Reserved
	    char		term;		//Header terminator (0x55)
	};

    private:
	//Methods
	int cacheGet( int &pos, int *vsz = NULL );
	void cacheSet( int pos, int off, int vsz, bool last = false, bool wr = false );
	void cacheDrop( int pos );

	int calcVlOff( int hd, int pos, int *vsz = NULL, bool wr = false );
	string getValue( int hd, int ioff, int vsz );
	void setValue( int hd, int ioff, const string &ival );
	void moveTail( int hd, int old_st, int new_st );

	int getPkVal( int hd, int pos );
	void setPkVal( int hd, int pos, int vl );

	void repairFile( int hd );

	//> Base parameters
	string	mName;		// Name Archive file
	int	mSize;		// Archive size
	TFld::Type mTp;		// Value typ
	int64_t	mBeg;		// Begin of archive file
	int64_t	mEnd;		// End of archive file
	int64_t	mPer;		// Values period

	//> State parameters
	bool	mErr;		// Archive err
	bool	mPack;		// Archive packed
	Res	mRes;		// resource to access
	time_t	mAcces;		// Last access time

	//> File access atributes
	bool	fixVl;		// Fix size values
	int	vSize;		// Fix value size or address size (bytes)
	string	eVal;		// Eval data type value
	int	mpos;		// Maximum value position into file
	char	tbt;		// Temporary byte

	//> Cache parameters
	struct CacheEl
	{
	    int pos;
	    int off;
	    int vsz;
	};
	vector<CacheEl> cache;
	CacheEl cach_pr_rd, cach_pr_wr;

	//> Specific parameters
	ModVArchEl *mOwner;
};

//************************************************
//* FSArch::ModVArchEl - Value archive element   *
//************************************************
class ModVArch;

class ModVArchEl: public TVArchEl
{
    friend class VFileArch;
    public:
	//Methods
	ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator );
	~ModVArchEl( );
	void fullErase( );

	int size( );

	int64_t	begin( );
	int64_t	end( );

	ModVArch &archivator( )	{ return (ModVArch&)TVArchEl::archivator(); }

	void checkArchivator( bool now = false, bool cpctLim = false );
	void fileAdd( const string &file );

    protected:
	//Methods
	TVariant getValProc( int64_t *tm, bool up_ord );
	void getValsProc( TValBuf &buf, int64_t beg, int64_t end );
	bool setValsProc( TValBuf &buf, int64_t beg, int64_t end );

    private:
	//Attributes
	Res	mRes;		// resource to access;
	deque<VFileArch *>	arh_f;
	int64_t	realEnd;
};

//************************************************
//* FSArch::ModVArch - Value archivator          *
//************************************************
class ModVArch: public TVArchivator
{
    public:
	//Methods
	ModVArch( const string &iid, const string &idb, TElem *cf_el );
	~ModVArch( );

	double curCapacity();

	double	fileTimeSize()	{ return time_size; }
	unsigned numbFiles()	{ return mNumbFiles; }
	double	maxCapacity()	{ return mMaxCapacity; }
	double	roundProc()	{ return round_proc; }
	int	checkTm()	{ return mChkTm; }
	int	packTm()	{ return mPackTm; }
	bool	packInfoFiles()	{ return mPackInfoFiles; }

	void setFileTimeSize( double vl )	{ time_size = vl; modif(); }
	void setNumbFiles( unsigned vl )	{ mNumbFiles = vl; modif(); }
	void setMaxCapacity( double vl )	{ mMaxCapacity = vl; modif(); }
	void setRoundProc( double vl )		{ round_proc = vl; modif(); }
	void setCheckTm( int vl )		{ mChkTm = vl; modif(); }
	void setPackTm( int vl )		{ mPackTm = vl; modif(); }
	void setPackInfoFiles( bool vl )	{ mPackInfoFiles = vl; modif(); }

	void start( );
	void stop( );

	void checkArchivator( bool now = false );

	//> Packing archives
	bool filePrmGet( const string &anm, string *archive, TFld::Type *vtp, int64_t *abeg, int64_t *aend, int64_t *aper );

	//> Export archive data
	void expArch(const string &arch_nm, time_t beg, time_t end, const string &file_tp, const string &file_nm);

    public:
	//Methods
	bool	chkANow;

    protected:
	//Methods
	void load_( );
	void save_( );
	bool cfgChange( TCfg &co, const TVariant &pc );

    private:
	//Methods
	TVArchEl *getArchEl( TVArchive &arch );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	double	time_size;			// number hours into one file
	unsigned mNumbFiles;			// number of Archive files
	double	mMaxCapacity;			// maximum archives capacity in megabytes
	double	round_proc;			// numeric values rounding procent (0-50)
	int	mChkTm;				// period of check the archive files directory;
	int	mPackTm;			// pack the archive files timeout
	bool	mPackInfoFiles;			// use info files for packed archives

	time_t	mLstCheck;			// Last check directory time
    };
}

#endif //FS_VAL_H
