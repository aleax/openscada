
//OpenSCADA system module Archive.FSArch file: val.h
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#define VAL_CACHE_POS 1000

using std::string;
using std::deque;

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
	VFileArch( const string &iname, long long ibeg, long long iend, long long iper, TFld::Type itp, ModVArchEl *owner);
	~VFileArch();
	void delFile();

	void attach( const string &name );

	string	&name( )	{ return m_name; }
	int	size( )		{ return m_size; }
	long long begin( )	{ return m_beg; }
	long long end( )	{ return m_end; }
	long long period( )	{ return m_per; }
	TFld::Type type( )	{ return m_tp; }
	bool	err( )		{ return m_err; }
	bool	isPack( )	{ return m_pack; }

	void	setVal( TValBuf &buf, long long beg, long long end );
	void	getVal( TValBuf &buf, long long beg, long long end );
	string	getS( int pos );
	double	getR( int pos );
	int	getI( int pos );
	char	getB( int pos );

	ModVArchEl &owner()	{ return *m_owner; }

	void check( );

	int maxPos( )		{ return mpos; }

	//Attributes
	static string afl_id;
	struct FHead
	{
	    char		f_tp[20];	//Archive system name ("OpenSCADA Val Arch.")
	    char		archive[20];	//Value archive name
	    long long		beg;		//Time archive begin
	    long long		end;		//Time archive end
	    long long		period;		//Time value period
	    unsigned char	vtp   :3;	//Value type (bool, int, real, string)
	    unsigned char	hgrid :1;	//Hard griding flag (reserved)
	    unsigned char	hres  :1;	//High resolution value time (reserved)
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

	void repairFile(int hd, bool fix = true );

	//- Base parameters -
	string	m_name;		// Name Archive file
	int	m_size;		// Arhive size
	TFld::Type m_tp;	// Value typ
	long long m_beg;	// Begin of archive file
	long long m_end;	// End of archive file
	long long m_per;	// Values period

	//- State parameters -
	bool	m_err;		// Archive err
	bool	m_pack;		// Archive packed
	Res	m_res;		// resource to access
	time_t	m_acces;	// Last access time

	//- File access atributes -
	bool	fixVl;		// Fix size values
	int	vSize;		// Fix value size or address size (bytes)
	string	eVal;		// Eval data type value
	int	mpos;		// Maximum value position into file
	char	tbt;		// Temporary byte

	//- Cache parameters -
	struct CacheEl
	{
	    int pos;
	    int off;
	    int vsz;
	};
	vector<CacheEl> cache;
	CacheEl cach_pr_rd, cach_pr_wr;

	//- Specific parameters -
	ModVArchEl *m_owner;
};

//************************************************
//* FSArch::ModVArchEl - Value archive element   *
//************************************************
class ModVArch;

class ModVArchEl: public TVArchEl
{
    public:
	ModVArchEl( TVArchive &iachive, TVArchivator &iarchivator );
	~ModVArchEl( );
	void fullErase();

	int size( );

	long long begin( );
	long long end( );

	void	getVal( TValBuf &buf, long long beg = 0, long long end = 0 );
	string	getS( long long *tm, bool up_ord );
	double	getR( long long *tm, bool up_ord );
	int	getI( long long *tm, bool up_ord );
	char	getB( long long *tm, bool up_ord );

	void setVal( TValBuf &buf, long long beg = 0, long long end = 0 );

	ModVArch &archivator()	{ return (ModVArch&)TVArchEl::archivator(); }

	void checkArchivator( bool now = false );
	void fileAdd( const string &file );

    private:
	Res	m_res;		// resource to access;
	deque<VFileArch *>	arh_f;
	long long realEnd;
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

	void setValPeriod( double iper );

	double	fileTimeSize()	{ return time_size; }
	int	fileNumber()	{ return numb_files; }
	double	roundProc()	{ return round_proc; }
	int	checkTm()	{ return m_chk_tm; }
	int	packTm()	{ return m_pack_tm; }

	void setFileTimeSize( double vl )	{ time_size = vl; modif(); }
	void setFileNumber( int vl )		{ numb_files = vl; modif(); }
	void setRoundProc( double vl )		{ round_proc = vl; modif(); }
	void setCheckTm( int vl )		{ m_chk_tm = vl; modif(); }
	void setPackTm( int vl )		{ m_pack_tm = vl; modif(); }

	void start();
	void stop();

	void checkArchivator( bool now = false );

	//- Packing archives -
	bool filePrmGet( const string &anm, string *archive, TFld::Type *vtp, long long *abeg, long long *aend, long long *aper );

	//- Export archive data -
	void expArch(const string &arch_nm, time_t beg, time_t end, const string &file_tp, const string &file_nm);

    public:
	//Methods
	bool	chkANow;

    private:
	//Methods
	TVArchEl *getArchEl( TVArchive &arch );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	double	&time_size;			// number hours into one file
	int	&numb_files;			// number of Archive files
	double	&round_proc;			// numberic values rounding procent (0-50)
	int	&m_chk_tm;			// period of check the archive files directory;
	int	&m_pack_tm;			// pack the archive files timeout

	time_t	mLstCheck;			// Last check directory time
    };
}

#endif //FS_VAL_H
