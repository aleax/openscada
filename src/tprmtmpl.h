
//OpenSCADA system file: tprmtmpl.h
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef TPRMTMPL_H
#define TPRMTMPL_H

#include <string>
#include <vector>

#include "tconfig.h"
#include "tfunction.h"

using std::string;
using std::vector;

namespace OSCADA
{

//*************************************************
//* TPrmTempl                                     *
//*************************************************
class TPrmTmplLib;

class TPrmTempl: public TFunction, public TConfig
{
    public:
	//Data
	// Addition flags for IO
	enum IOTmplFlgs
	{
	    AttrRead	= 0x010,	//Attribute only for read
	    AttrFull	= 0x020,	//Attribute for full access
	    CfgPublConst= 0x040,	//Configure as public constant
	    CfgLink	= 0x080,	//Configure as link
	    LockAttr	= 0x100		//Lock attribute
	};

	//Methods
	TPrmTempl( const string &id, const string &name = "" );
	~TPrmTempl( );

	TCntrNode &operator=( TCntrNode &node );

	string	id( )		{ return mId; }
	string	name( );
	string	descr( );
	int	maxCalcTm( );
	string	progLang( );
	string	prog( );
	int64_t	timeStamp( )	{ return mTimeStamp; }

	void setName( const string &inm );
	void setDescr( const string &idsc );
	void setMaxCalcTm( int vl );
	void setProgLang( const string &ilng );
	void setProg( const string &iprg );
	void setStart( bool val );

	AutoHD<TFunction>	func( );	//Programming language attached function

	TPrmTmplLib &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );

	void postEnable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &cfg )     { modif(); return true; }
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Methods
	const char *nodeName( )	{ return mId.getSd(); }

	//Attributes
	TCfg	&mId;
	int64_t	&mTimeStamp;
	string	work_prog;
};

//*************************************************
//* TPrmTmplLib                                   *
//*************************************************
class TDAQS;

class TPrmTmplLib : public TCntrNode, public TConfig
{
    public:
	TPrmTmplLib( const string &id, const string &name, const string &lib_db );
	~TPrmTmplLib( );

	TCntrNode &operator=( TCntrNode &node );

	string	id( )		{ return mId; }
	string	name( );
	string	descr( );

	string	DB( )		{ return work_lib_db; }
	string	tbl( )		{ return cfg("DB").getS(); }
	string	fullDB( )	{ return DB()+'.'+tbl(); }

	bool startStat( )	{ return run_st; }
	void start( bool val );

	void setName( const string &vl );
	void setDescr( const string &vl );
	void setFullDB( const string &vl );

	void list( vector<string> &ls )		{ chldList(m_ptmpl,ls); }
	bool present( const string &id )	{ return chldPresent(m_ptmpl,id); }
	AutoHD<TPrmTempl> at( const string &id ){ return chldAt(m_ptmpl,id); }
	void add( const string &id, const string &name = "" );
	void del( const string &id, bool full_del = false )	{ chldDel(m_ptmpl,id,-1,full_del); }

	TDAQS &owner( );

    protected:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void preDisable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &cfg )	{ modif(); return true; }

	void load_( );
	void save_( );

	TVariant objFuncCall( const string &id, vector<TVariant> &prms, const string &user );

    private:
	//Methods
	const char *nodeName( )	{ return mId.getSd(); }

	//Attributes
	bool	run_st;
	int	m_ptmpl;
	TCfg	&mId;
	ResString work_lib_db;
};

}

#endif // TPRMTMPL_H
