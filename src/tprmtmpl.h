
//OpenSCADA system file: tprmtmpl.h
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
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

#ifndef TPRMTMPL_H
#define TPRMTMPL_H

#include <string>
#include <vector>

#include "tconfig.h"
#include "tfunction.h"

using std::string;
using std::vector;

//*************************************************
//* TPrmTempl                                     *
//*************************************************
class TPrmTmplLib;

class TPrmTempl: public TFunction, public TConfig
{
    public:
	//Data
	//- Addition flags for IO -
	enum IOTmplFlgs
	{
	    AttrRead 	= 0x010,	//Attribute only for read
	    AttrFull 	= 0x020,	//Attribute for full access
	    CfgPublConst= 0x040,	//Configure as public constant
	    CfgLink	= 0x080,	//Configure as link
	    LockAttr	= 0x100		//Lock attribute
	};
    
	//Methods
	TPrmTempl( const char *id, const char *name = "" );
	~TPrmTempl( );
	
	const string &id( )	{ return m_id; }
        string name( );
        string descr( )		{ return m_descr; }
	string progLang( );
	string prog( );
	
	void setName( const string &inm )  	{ m_name = inm; }
        void setDescr( const string &idsc )	{ m_descr = idsc; }
	void setProgLang( const string &ilng );
	void setProg( const string &iprg );
	void setStart( bool val );		

	void load( );
        void save( );

	AutoHD<TFunction>	func( );	//Programming language attached function
	
	TPrmTmplLib &owner( )	{ return *(TPrmTmplLib*)nodePrev(); }

    protected:
	void preIOCfgChange( );
	
    private:
	//Methods
	string nodeName( )	{ return m_id; }
	void postEnable( int flag );
	void postDisable( int flag );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	//Attributes
	string 	&m_id, &m_name, &m_descr, &m_prog, work_prog;
};

//*************************************************
//* TPrmTmplLib                                   *
//*************************************************
class TDAQS;

class TPrmTmplLib : public TCntrNode, public TConfig
{
    public:
        TPrmTmplLib( const char *id, const char *name, const string &lib_db );
	~TPrmTmplLib( );
	
        const string &id( )	{ return m_id; }
        string name( );
        string descr( )  	{ return m_descr; }
	
	string DB( )            { return work_lib_db; }
	string tbl( )    	{ return m_db; }
        string fullDB( )        { return DB()+'.'+tbl(); }
					
        bool startStat( )       { return run_st; }
        void start( bool val );
							
        void load( );
        void save( );
								
        void list( vector<string> &ls ) 	{ chldList(m_ptmpl,ls); }
        bool present( const string &id )	{ return chldPresent(m_ptmpl,id); }
        AutoHD<TPrmTempl> at( const string &id ){ return chldAt(m_ptmpl,id); }
        void add( const char *id, const char *name = "" );
        void del( const char *id, bool full_del = false )	{ chldDel(m_ptmpl,id,-1,full_del); }
	
	TDAQS &owner( )    			{ return *(TDAQS*)nodePrev(); }
		
    protected:
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
				    
        void preDisable( int flag );
        void postDisable( int flag );
					    
    private:
	//Methods
        string nodeName( )	{ return m_id; }
	
	//Attributes
        bool    run_st;
        int     m_ptmpl;
        string  &m_id, &m_name, &m_descr, &m_db, work_lib_db;
};	
		    

#endif // TPRMTMPL_H
