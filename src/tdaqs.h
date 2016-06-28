
//OpenSCADA system file: tdaqs.h
/***************************************************************************
 *   Copyright (C) 2003-2010 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#ifndef TDAQS_H
#define TDAQS_H

#define SDAQ_VER	8	//ControllerS type modules version
#define SDAQ_ID		"DAQ"

#include <string>

#include "tprmtmpl.h"
#include "tsubsys.h"
#include "ttipdaq.h"

using std::string;

namespace OSCADA
{

//*************************************************
//* TDAQS                                         *
//*************************************************
class TController;
class TTipDAQ;

class TDAQS : public TSubSYS
{
    public:
	//Public methods
	TDAQS( );
	~TDAQS( );

	string objName( );

	int subVer( )	{ return SDAQ_VER; }
	void subStart(  );
	void subStop( );

	AutoHD<TTipDAQ> at( const string &name )		{ return modAt(name); }
	AutoHD<TCntrNode> daqAt( const string &path, char sep = 0, bool noex = false, bool waitForAttr = false );
	AutoHD<TValue> prmAt( const string &path, char sep = 0, bool noex = false );
	AutoHD<TVal> attrAt( const string &path, char sep = 0, bool noex = false );
	void ctrListPrmAttr( XMLNode *nd, const string &addrBase, bool toPrm = false, char sep = 0, const string &pref = "" );

	//> Parameter's templates library
	string tmplLibTable( )					{ return "ParamTemplLibs"; }
	void tmplLibList( vector<string> &list )		{ chldList(mTmpLib,list); }
	bool tmplLibPresent( const string &id )			{ return chldPresent(mTmpLib,id); }
	void tmplLibReg( TPrmTmplLib *lib )			{ chldAdd(mTmpLib,lib); }
	void tmplLibUnreg( const string &id, int flg = 0 )	{ chldDel(mTmpLib,id,-1,flg); }
	AutoHD<TPrmTmplLib> tmplLibAt( const string &id )	{ return chldAt(mTmpLib,id); }

	//> Redundancy
	bool rdActive( );
	int rdStLevel( )		{ return mRdStLevel; }
	void setRdStLevel( int vl )	{ mRdStLevel = vmin(255,vmax(0,vl)); modif(); }
	float rdTaskPer( )		{ return mRdTaskPer; }
	void setRdTaskPer( float vl )	{ mRdTaskPer = vmin(255,vmax(0.1,vl)); modif(); }
	int rdRestConnTm( )		{ return mRdRestConnTm; }
	void setRdRestConnTm( int vl )	{ mRdRestConnTm = vmin(255,vmax(0,vl)); modif(); }
	float rdRestDtTm( )		{ return mRdRestDtTm; }
	void setRdRestDtTm( float vl )	{ mRdRestDtTm = vmin(24,vmax(0.01,vl)); modif(); }
	void rdStList( vector<string> &ls );
	void rdActCntrList( vector<string> &ls, bool isRun = false );
	string rdStRequest( const string &cntr, XMLNode &req, const string &prevSt = "", bool toRun = true );

	TElem &elLib( )		{ return mElLib; }
	TElem &elTmpl( )	{ return mElTmpl; }
	TElem &elTmplIO( )	{ return mElTmplIO; }

	TElem &elErr( )		{ return mElErr; }	//Error atributes structure

    protected:
	void load_( );
	void save_( );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Private data
	class SStat {
	    public:
		SStat( ) : isLive(false), cnt(0), lev(0) { }

		bool	isLive;
		float	cnt;
		char	lev;
		map<string,bool> actCntr;
	};

	//Private methods
	string optDescr( );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	static void *RdTask( void *param );

	//Private attributes
	TElem	mElErr, mElLib, mElTmpl, mElTmplIO;
	int	mTmpLib;

	Res		mRdRes;
	unsigned char	mRdStLevel,		//Current station level
			mRdRestConnTm;		//Redundant restore connection to reserve stations timeout in seconds
	float		mRdTaskPer,		//Redundant task period in seconds
			mRdRestDtTm,		//Redundant history restore length time in hour
			mRdPrcTm;		//Redundant process time
	bool		prcStRd, endrunRd;
	map<string,SStat> mSt;
};

}

#endif // TDAQS_H
