
//OpenSCADA system file: tdaqs.h
/***************************************************************************
 *   Copyright (C) 2003-2016 by Roman Savochenko, <rom_as@oscada.org>      *
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

#define SDAQ_VER	10	//ControllerS type modules version
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
	void subStart( );
	void subStop( );

	AutoHD<TTypeDAQ> at( const string &name )		{ return modAt(name); }
	AutoHD<TCntrNode> daqAt( const string &path, char sep = 0, bool noex = false, bool waitForAttr = false );
	AutoHD<TValue> prmAt( const string &path, char sep = 0, bool noex = false );
	AutoHD<TVal> attrAt( const string &path, char sep = 0, bool noex = false );
	void ctrListPrmAttr( XMLNode *nd, const string &addrBase, bool toPrm = false, char sep = 0, const string &pref = "" );

	// Parameter's templates library
	string tmplLibTable( )					{ return "ParamTemplLibs"; }
	void tmplLibList( vector<string> &list )		{ chldList(mTmplib,list); }
	bool tmplLibPresent( const string &id )			{ return chldPresent(mTmplib,id); }
	void tmplLibReg( TPrmTmplLib *lib )			{ chldAdd(mTmplib,lib); }
	void tmplLibUnreg( const string &id, int flg = 0 )	{ chldDel(mTmplib,id,-1,flg); }
	AutoHD<TPrmTmplLib> tmplLibAt( const string &id )	{ return chldAt(mTmplib,id); }

	// Redundancy
	bool rdProcess( XMLNode *reqSt = NULL );
	float rdRestDtTm( )		{ return mRdRestDtTm; }
	void setRdRestDtTm( float vl )	{ mRdRestDtTm = vmin(12,vmax(0.01,vl)); modif(); }
	void rdActCntrList( vector<string> &ls, bool isRun = false );
	string rdStRequest( const string &cntr, XMLNode &req, const string &prevSt = "", bool toRun = true );

	TElem &elLib( )	{ return lb_el; }
	TElem &tplE( )	{ return el_tmpl; }
	TElem &tplIOE( ){ return el_tmpl_io; }

	TElem &errE( )	{ return el_err; }	//Error atributes structure

    protected:
	void load_( );
	void save_( );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Private methods
	string optDescr( );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Private attributes
	TElem	el_err, lb_el, el_tmpl, el_tmpl_io;
	int	mTmplib;

	Res		mRdRes;
	float		mRdRestDtTm;		//Redundant history restore length time in hour
	map<string, map<string,bool> > mRdCntr;
};

}

#endif // TDAQS_H
