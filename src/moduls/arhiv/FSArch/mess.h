
//OpenSCADA system module Archive.FSArch file: mess.h
/***************************************************************************
 *   Copyright (C) 2003-2007 by Roman Savochenko                           *
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

#ifndef FS_MESS_H
#define FS_MESS_H

#include <deque>
#include <string>

#define CACHE_POS 100

using std::string;
using std::deque;

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
	~MFileArch();

	void attach( const string &name, bool full = true );
	void put( TMess::SRec mess );
	void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category, char level );
	//- Write changes to Archive file -
	//  free - surely free used memory
	void check( bool free = false );

	string &name() 		{ return m_name; }
	bool   xmlM()		{ return m_xml; }
	int    size()		{ return m_size; }
	time_t begin()		{ return m_beg; }
	time_t end()  		{ return m_end; }
	string charset()	{ return m_chars; }
	bool   err()  		{ return m_err; }

	ModMArch &owner() 	{ return *m_owner; }
	    
	//Attributes
	bool    scan;    	// Archive scaned (for check deleted files). Use from ModMArch
	
    private:
	//- Cache methods -
	long    cacheGet( time_t tm );
	void    cacheSet( time_t tm, long off, bool last = false );
	void    cacheUpdate( time_t tm, long v_add );
	    
	//- Base parameters -
	string  m_name;    	// name Archive file;
	bool    m_xml;		// XML mode file
	int    	m_size;  	// Arhive size
	string  m_chars;   	// Archive charset;
	//- State parameters -
	bool    m_err;     	// Archive err
	bool    m_write;   	// Archive had changed but no writed to file
	bool    m_load;    	// Archive load to m_node
	bool    m_pack;	// Archive packed
	time_t  m_acces;   	// last of time acces to Archive file
	time_t  m_beg;     	// begin Archive file;
	time_t  m_end;     	// end Archive file;
	//- XML-mode parametrs -
	XMLNode *m_node;    // XML-file tree
	//- Cache parameters -
	struct CacheEl
	{
	    time_t tm;
	    long   off;
	};
	vector<CacheEl> cache;
	CacheEl cach_pr;
	//- Specific parameters -
	Res	 m_res;     	// resource to access;	    
	ModMArch *m_owner;
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

	time_t begin();
	time_t end();
	void put( vector<TMess::SRec> &mess );
	void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category = "", char level = 0 );
	void start();
	void stop();
	    
	int  size();

	bool useXML()		{ return m_use_xml; }
	int  maxSize()		{ return m_max_size; }
	int  numbFiles()	{ return m_numb_files; }
	int  timeSize()		{ return m_time_size; }
	int  checkTm()		{ return m_chk_tm; }
	int  packTm()		{ return m_pack_tm; }
	    
	void checkArchivator( bool now = false );
	    
    private:	
	//Methods
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	   
	//Attributes
	bool	&m_use_xml;	// use XML for archive files
	int	&m_max_size;	// maximum size kb of Archives file
	int	&m_numb_files;	// number of Archive files
	int	&m_time_size;	// number days to one file
	int	&m_chk_tm;	// period of check archive files directory;
	int	&m_pack_tm;	// pack the archive files timeout
	
	Res    	m_res;     	// resource to access;	
	double 	tm_calc;        // Archiving time
	time_t 	m_lst_check;	// Last check directory time
	    
	deque<MFileArch *>  arh_s;
};

}

#endif //FS_MESS_H

