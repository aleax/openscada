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
	
        string &name()   { return(m_name); }
        string &lName()  { return(m_lname); }			
	
	bool toStart() { return(m_start); }
	bool startStat(){ return(run_st); }

        void load( );
        void save( );			
        virtual void start(){ };
        virtual void stop(){ };		       	
	
        string &addr()   { return(m_addr); }
        int    &level()  { return(m_level); }
        void   categ( vector<string> &list );				

	virtual void put( vector<TMess::SRec> &mess ){ };
        virtual void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category = "", char level = 0 ) { };
	
	TTipArchive &owner() { return(*m_owner); }
	
    protected:
	string nodeName(){ return m_name; }
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	
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
	TTipArchive     *m_owner;
	
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

	string &name() { return(m_name); }
	
	TTipArchive &owner() { return(*m_owner); }
	
    protected:
	string nodeName(){ return m_name; }
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	
    protected:
	string  &m_name;
	string  &m_bd;
	
    private:
	TTipArchive *m_owner;
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
        bool messAvoid( const string &name )	{ return chldAvoid(m_mess,name); }
	void messAdd( const string &name );
	void messDel( const string &name )	{ chldDel(m_mess,name); }
	AutoHD<TArchiveMess> messAt( const string &name )
 	{ return chldAt(m_mess,name); }
	
	// Values
	void valList( vector<string> &list )	{ chldList(m_val,list); }
        bool valAvoid( const string &name )	{ return chldAvoid(m_val,name); }
	void valAdd( const string &name );
        void valDel( const string &name )	{ chldDel(m_val,name); }
	AutoHD<TArchiveVal> valAt( const string &name )
 	{ return chldAt(m_val,name); }
	
    protected:
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	AutoHD<TCntrNode> ctrAt1( const string &br );
	
    /** Private atributes:: */
    private:
	virtual TArchiveMess *AMess(const string &name )
	{ throw TError("(%s) Message arhiv no support!",__func__); }
	virtual TArchiveVal  *AVal(const string &name )
	{ throw TError("(%s) Value arhiv no support!",__func__); }
	
    /** Private atributes:: */
    private:
	int	m_mess, m_val;
};

//================================================================
//================ TArchiveS ======================================
//================================================================
class TArchiveS : public TGRPModule
{
    /** Public methods: */
    public:
	TArchiveS( TKernel *app );
	~TArchiveS(  );

	int gmdVer( ) { return(VER_ARH); }
    	// Init All transport's modules
	void gmdLoad( );
	void gmdStart( );
	void gmdStop( );	       	
	// Load/Reload all BD and update internal controllers structure!
	void loadBD( );
	// Update all BD from current to external BD.
	void saveBD( );
	
	TBDS::SName messB();
	TBDS::SName valB();
	
	TElem &messE(){ return(el_mess); }
	TElem &valE() { return(el_val); }
	
    /** Privates: */
    private:
	string optDescr(  );

	static void *MessArhTask(void *param);
	
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	
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
