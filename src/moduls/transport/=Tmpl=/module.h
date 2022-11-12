
//OpenSCADA module Transport.Tmpl file: module.h
/***************************************************************************
 *   Copyright (C) 2022 by MyName MyFamily, <my@email.org>                 *
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

// Preventing of the header file multi-including - change at the header file name changing
#ifndef MODULE_H
#define MODULE_H

// System includings - add need ones
#include <string>

// OpenSCADA API includings - add need ones
#include <ttransports.h>

// Definition of the individual module translation functions - do not change
#undef _
#define _(mess) mod->I18N(mess).c_str()
#undef trS
#define trS(mess) mod->I18N(mess,mess_PreSave)

using namespace OSCADA;

// All the module objects in own (individual) namespace - change for your module
namespace TrTmpl
{

// Definition the input transport object of the subsystem "Transports" - add methods and attributes at your need
//************************************************
//* TrTmpl::TTrIn				 *
//************************************************
class TTrIn: public TTransportIn
{
    public:
	TTrIn( string name, const string &idb, TElem *el );
	~TTrIn( );

	string getStatus( );

	unsigned keepAliveReqs( )	{ return mKeepAliveReqs; }
	unsigned keepAliveTm( )		{ return mKeepAliveTm; }
	int taskPrior( )		{ return mTaskPrior; }

	void setKeepAliveReqs( unsigned vl )	{ mKeepAliveReqs = vl; modif(); }
	void setKeepAliveTm( unsigned vl )	{ mKeepAliveTm = vl; modif(); }
	void setTaskPrior( int vl )		{ mTaskPrior = vmax(-1,vmin(199,vl)); modif(); }

	void start( );
	void stop( );

    protected:
	//Methods
	bool cfgChange( TCfg &co, const TVariant &pc );

	void load_( );
	void save_( );

    private:
	//Methods
	static void *Task( void * );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	bool	endrun;				// Command for stop task

	unsigned short	mKeepAliveReqs,		//KeepAlive requests
			mKeepAliveTm;		//KeepAlive timeout
	int		mTaskPrior;		//Requests processing task prioritet
	//???? Append additional configuration attributes
};

// Definition the output transport object of the subsystem "Transports" - add methods and attributes at your need
//************************************************
//* TrTmpl::TTrOut				 *
//************************************************
class TTrOut: public TTransportOut
{
    public:
	TTrOut( string name, const string &idb, TElem *el );
	~TTrOut( );

	string getStatus( );

	string timings( )		{ return mTimings; }
	unsigned short attempts( )	{ return mAttemts; }

	void setTimings( const string &vl, bool isDef = false );
	void setAttempts( unsigned short vl );

	void start( int time = 0 );
	void stop( );

	int messIO( const char *oBuf, int oLen, char *iBuf = NULL, int iLen = 0, int time = 0 );

    protected:
	//Methods
	bool cfgChange( TCfg &co, const TVariant &pc );

	void load_( );
	void save_( );

    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	//Attributes
	string		mTimings;
	unsigned short	mAttemts;
};

// Definition the root module object of the subsystem "Transports" - add methods and attributes at your need
//************************************************
//* TrTmpl::TTr				 *
//************************************************
class TTr: public TTypeTransport
{
    public:
	TTr( string name );
	~TTr( );

	TTransportIn  *In( const string &id, const string &stor );
	TTransportOut *Out( const string &id, const string &stor );

	string outAddrHelp( );

    private:
	//Methods
	void postEnable( int flag );
};

extern TTr *mod;	//The module root link

} //End the namespace BDTmpl

#endif //MODULE_H
