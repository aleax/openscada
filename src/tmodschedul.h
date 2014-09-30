
//OpenSCADA system file: tmodschedul.h
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

#ifndef TMODSCHEDUL_H
#define TMODSCHEDUL_H

#define SMSH_ID		"ModSched"

#include <pthread.h>

#include "tsubsys.h"

namespace OSCADA
{

//*************************************************
//* TModSchedul                                   *
//*************************************************
class TModSchedul : public TSubSYS
{
    public:
	//Data
	class SHD {
	    public:
		//Methods
		SHD( ) : hd(NULL), tm(0)	{ }
		SHD( void *ihd, time_t itm, const string &iname ) : hd(ihd), tm(itm), name(iname)	{ }

		//Attributes
		void		*hd;		// NULL - share lib present but no attached
		vector<string>	use;		// if share lib attached to show who modules used
		time_t		tm;		// data modify of share lib for automatic update
		string		name;		// share lib path
		string		err;		// Error message for broken modules
	};

	//Methods
	TModSchedul( );

	string allowList( )	{ return mAllow; }
	string denyList( )	{ return mDeny; }
	int chkPer( )		{ return mPer; }

	void setAllowList( const string &vl )	{ mAllow = vl; modif(); }
	void setDenyList( const string &vl )	{ mDeny = vl; modif(); }
	void setChkPer( int per );

	int loadLibS( );				// Load/Init/Start all share libs and registry moduls into TSubSYS

	SHD lib( const string &name );			// Get stat share lib <name>
	void libList( vector<string> &list );		// List present share libs
	int libLoad( const string &path, bool full );	// Load share libs for <dest> from <path> whith call gmdInit if set <full>
	void libAtt( const string &name, bool full = false);	// Attach share libs
	void libDet( const string &name );		// Detach share libs

	Res &nodeRes( )		{ return nRes; }

    protected:
	//Methods
	void load_( );
	void save_( );
	void preDisable( int flag );

    private:
	//Methods
	string optDescr( );				// Description of config help
	void ScanDir( const string &Paths, vector<string> &files );//Scan directory for OpenSCADA share libs
	bool CheckFile( const string &name );		//Check file to OpenSCADA share libs
	int  libReg( const string &name );		//Registre present share lib
	void libUnreg( const string &name );		//Unreg deleted share lib
	bool chkAllowMod( const string &name );		//Check for allow file to attaching

	void cntrCmdProc( XMLNode *opt );		//Control interface command process

	static void SchedTask( union sigval obj );

	//Attributes
	string	mAllow, mDeny;

	int		mPer;				//Check to new modules period
	vector<SHD>	SchHD;

	Res		nRes;
};

}

#endif // TMODSCHEDUL_H
