/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#ifndef TArchiveS_H
#define TArchiveS_H

#define  VER_ARH 1    //ArchiveS type modules version

#include <string>
#include <vector>

#include "tbds.h"

using std::string;
using std::vector;

//================================================================
//=========== TArchiveMess ========================================
//================================================================
class TTipArchive;

class TArchiveMess : public TCntrNode, public TConfig
{
    public:
	TArchiveMess(const string &name, TTipArchive *owner );
	virtual ~TArchiveMess();	
	
        string &name()	{ return(m_name); }
        string &lName()	{ return(m_lname); }			
	
	bool toStart() 	{ return(m_start); }
	bool startStat(){ return(run_st); }

        void load( );
        void save( );			
        virtual void start()	{ };
        virtual void stop()	{ };		       	
	
        string &addr()	{ return(m_addr); }
        int    &level()	{ return(m_level); }
        void   categ( vector<string> &list );				

	virtual void put( vector<TMess::SRec> &mess ){ };
        virtual void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category = "", char level = 0 ) { };

	TTipArchive &owner()	{ return *(TTipArchive *)nodePrev(); }
	
    protected:
	string nodePref()	{ return "mess_"; }
	string nodeName()	{ return m_name; }
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
	void postDisable(int flag);     //Delete all DB if flag 1
	
    protected:
	string         &m_name;
	string         &m_lname;
	bool           &m_start;
	string         &m_addr;
	string         &m_cat_o;
	int            &m_level;

	bool           run_st;
	
    private:
	//Request mess params
	time_t	m_beg, m_end;
	string	m_cat;
	int	m_lvl;
};

//================================================================
//=========== TArchiveVal =========================================
//================================================================
class TArchiveVal : public TCntrNode, public TConfig
{
    public:
	TArchiveVal( const string &name, TTipArchive *owner );
	virtual ~TArchiveVal();

	string &name() 		{ return m_name; }	
	TTipArchive &owner() 	{ return *(TTipArchive *)nodePrev(); }
	
    protected:
	string nodePref()	{ return "val_"; }
	string nodeName()	{ return m_name; }
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
    protected:
	string  &m_name;
	string  &m_bd;
};

//================================================================
//=========== TTipArchive =========================================
//================================================================

class TTipArchive: public TModule
{
    /** Public methods: */
    public:
    	TTipArchive( );
	virtual ~TTipArchive();
		
	//Messages
	void messList( vector<string> &list )	{ chldList(m_mess,list); }
        bool messPresent( const string &name )	{ return chldPresent(m_mess,name); }
	void messAdd( const string &name );
	void messDel( const string &name )	{ chldDel(m_mess,name); }
	AutoHD<TArchiveMess> messAt( const string &name )
 	{ return chldAt(m_mess,name); }
	
	// Values
	void valList( vector<string> &list )	{ chldList(m_val,list); }
        bool valPresent( const string &name )	{ return chldPresent(m_val,name); }
	void valAdd( const string &name );
        void valDel( const string &name )	{ chldDel(m_val,name); }
	AutoHD<TArchiveVal> valAt( const string &name )
 	{ return chldAt(m_val,name); }
	
    protected:
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	AutoHD<TCntrNode> ctrAt( const string &br );
	
    /** Private atributes:: */
    private:
	virtual TArchiveMess *AMess(const string &name )
	{ throw TError(nodePath().c_str(),"Message arhiv no support!"); }
	virtual TArchiveVal  *AVal(const string &name )
	{ throw TError(nodePath().c_str(),"Value arhiv no support!"); }
	
    /** Private atributes:: */
    private:
	int	m_mess, m_val;
};

//================================================================
//================ TArchiveS ======================================
//================================================================
class TArchiveS : public TSubSYS
{
    /** Public methods: */
    public:
	TArchiveS( TSYS *app );
	~TArchiveS(  );

	int subVer( )	{ return(VER_ARH); }
    	// Init All transport's modules
	void subLoad( );
	void subSave( );
	void subStart( );
	void subStop( );	       	
	
	TBDS::SName messB();
	TBDS::SName valB();
	
	TElem &messE()	{ return(el_mess); }
	TElem &valE() 	{ return(el_val); }
	
    /** Privates: */
    private:
	string optDescr(  );

	static void *MessArhTask(void *param);
	
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
    /** Private atributes: */
    private:	
	TBDS::SName	m_bd_mess, m_bd_val;	
	TElem  		el_mess, el_val;

	int       m_mess_per;       //Mmessage arhiving period
	pthread_t m_mess_pthr;
	bool      m_mess_r_stat;
	bool      m_mess_r_endrun;
};

#endif // TArchiveS_H
