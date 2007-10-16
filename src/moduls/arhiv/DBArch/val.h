
//OpenSCADA system module Archive.DBArch file: val.h
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
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

#ifndef DB_VAL_H
#define DB_VAL_H

#include <string>

#include <tarchives.h>

using std::string;

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
		    
	long long begin( )	{ return m_beg;	}
	long long end( )	{ return m_end; }
	long long period( )	{ return m_per; }

	void getVal( TValBuf &buf, long long beg = 0, long long end = 0 );
	string getS( long long *tm, bool up_ord );
	double getR( long long *tm, bool up_ord );
	int    getI( long long *tm, bool up_ord );
	char   getB( long long *tm, bool up_ord );

	void setVal( TValBuf &buf, long long beg = 0, long long end = 0 );
	    
	ModVArch &archivator()	{ return (ModVArch&)TVArchEl::archivator(); }
	
    private:
	//Attributes
	long long m_beg, m_end, m_per;
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

	double maxSize( )	{ return m_max_size; }

	void setValPeriod( double iper );

	void load( );	    
	void start( );
	void stop( );
	    
    private:
	//Methods
	TVArchEl *getArchEl( TVArchive &arch );
	void cntrCmdProc( XMLNode *opt );
	
	//Attributes
	double  &m_max_size;                    //Maximum archive size (hours)
};

}

#endif //DB_VAL_H

