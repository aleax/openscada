
//OpenSCADA system file: tdaqs.h
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

#ifndef TDAQS_H
#define TDAQS_H

#define SDAQ_VER	12	//ControllerS type modules version
#define SDAQ_ID		"DAQ"

#include <string>

#include "tprmtmpl.h"
#include "tsubsys.h"
#include "ttypedaq.h"

using std::string;

namespace OSCADA
{

//*************************************************
//* TDAQS                                         *
//*************************************************
class TController;
class TTypeDAQ;

class TDAQS : public TSubSYS
{
    public:
	//Public methods
	TDAQS( );
	~TDAQS( );

	string objName( );

	int subVer( )	{ return SDAQ_VER; }

	void unload( );

	void subStart( );
	void subStop( );

	AutoHD<TTypeDAQ> at( const string &name ) const		{ return modAt(name); }
	AutoHD<TCntrNode> daqAt( const string &path, char sep = 0, bool noex = false, bool waitForAttr = false ) const;
	AutoHD<TValue> prmAt( const string &path, char sep = 0, bool noex = false ) const;
	AutoHD<TVal> attrAt( const string &path, char sep = 0, bool noex = false ) const;
	void ctrListPrmAttr( XMLNode *nd, const string &addrBase, bool toPrm = false, char sep = 0, const string &pref = "" );

	// Parameter's templates library
	string tmplLibTable( )					{ return "ParamTemplLibs"; }
	void tmplLibList( vector<string> &list ) const		{ chldList(mTmpLib,list); }
	bool tmplLibPresent( const string &id ) const		{ return chldPresent(mTmpLib,id); }
	void tmplLibReg( TPrmTmplLib *lib )			{ chldAdd(mTmpLib,lib); }
	void tmplLibUnreg( const string &id, int flg = 0 )	{ chldDel(mTmpLib,id,-1,flg); }
	AutoHD<TPrmTmplLib> tmplLibAt( const string &id ) const	{ return chldAt(mTmpLib,id); }

	// Redundancy
	bool rdProcess( XMLNode *reqSt = NULL );
	float rdRestDtTm( )		{ return mRdRestDtTm; }
	void setRdRestDtTm( float vl )	{ mRdRestDtTm = vmin(24,vmax(0.01,vl)); modif(); }
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
	//Private methods
	string optDescr( );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Private attributes
	TElem	mElErr, mElLib, mElTmpl, mElTmplIO;
	int	mTmpLib;

	ResRW	mRdRes;
	float	mRdRestDtTm;			//Depth time of the redundant history restore in hours
	map<string, map<string,bool> > mRdCntr;
};

}

#endif // TDAQS_H
