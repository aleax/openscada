
//OpenSCADA system file: tarchives.h
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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

#include <time.h>

#include <string>
#include <vector>

#include "tbds.h"
#include "tarchval.h"

using std::string;
using std::vector;

#define BUF_ARCH_NM "<bufer>"

//================================================================
//===================== Message archivator =======================
//================================================================

//================ TMArchivator ==================================
class TTipArchivator;

class TMArchivator : public TCntrNode, public TConfig
{
    public:
	//Methods
	TMArchivator(const string &iid, const string &idb, TElem *cf_el );

        const string &id()	{ return m_id; }
	string workId();
        string name();
        string dscr()		{ return m_dscr; }
	bool toStart() 		{ return m_start; }
	bool startStat()	{ return run_st; }

	void name( const string &vl )  	{ m_name = vl; }
        void dscr( const string &vl )  	{ m_dscr = vl; }
	void toStart( bool vl )		{ m_start = vl; }

        virtual void load( );
        virtual void save( );
        virtual void start( )	{ };
        virtual void stop( )	{ };

        string &addr()	{ return m_addr; }
        int    &level()	{ return m_level; }
        void   categ( vector<string> &list );

	virtual void put( vector<TMess::SRec> &mess ){ };
        virtual void get( time_t b_tm, time_t e_tm, vector<TMess::SRec> &mess, const string &category = "", char level = 0, const string &arch = "" ) { };

	string BD();

	TTipArchivator &owner()	{ return *(TTipArchivator *)nodePrev(); }

    protected:
	//Methods
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	void postEnable( );
	void postDisable(int flag);     //Delete all DB if flag 1

	//- Check messages criteries -
	bool chkMessOK( const string &icateg, TMess::Type ilvl );

	//Attributes
	bool           run_st;

    private:
	//Methods
        string nodeName()       { return m_id; }

	//Attributes
	string	&m_id,		//Mess arch id
		&m_name,	//Mess arch name
		&m_dscr,	//Mess arch description
		&m_addr,	//Mess arch address
		&m_cat_o;	//Mess arch cetegory
	bool	&m_start;	//Mess arch starting flag
	int	&m_level;	//Mess arch level
	string  m_bd;

	//- Request mess params -
	time_t	m_beg, m_end;
	string	m_cat;
	int	m_lvl;
};

//================================================================
//=================== Archive subsystem ==========================
//================================================================

//=================== TTipArchivator =============================
class TArchiveS;

class TTipArchivator: public TModule
{
    public:
    	TTipArchivator( );
	virtual ~TTipArchivator();

	//- Messages -
	void messList( vector<string> &list )	{ chldList(m_mess,list); }
        bool messPresent( const string &iid )	{ return chldPresent(m_mess,iid); }
	void messAdd( const string &iid, const string &idb = "*.*" );
	void messDel( const string &iid, bool full = false )	{ chldDel(m_mess,iid,-1,full); }
	AutoHD<TMArchivator> messAt( const string &iid )
 	{ return chldAt(m_mess,iid); }

	//- Values -
	void valList( vector<string> &list )	{ chldList(m_val,list); }
        bool valPresent( const string &iid )	{ return chldPresent(m_val,iid); }
	void valAdd( const string &iid, const string &idb = "*.*" );
        void valDel( const string &iid, bool full = false )	{ chldDel(m_val,iid,-1,full); }
	AutoHD<TVArchivator> valAt( const string &iid )
 	{ return chldAt(m_val,iid); }

	TArchiveS &owner();

    protected:
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );

    private:
	//Methods
	virtual TMArchivator *AMess(const string &iid, const string &idb )
	{ throw TError(nodePath().c_str(),"Message arhiv no support!"); }
	virtual TVArchivator *AVal(const string &iid, const string &idb )
	{ throw TError(nodePath().c_str(),"Value arhiv no support!"); }

	//Attributes
	int	m_mess, m_val;
};

//===================== TArchiveS ================================
class TArchiveS : public TSubSYS
{
    public:
	TArchiveS( );
	~TArchiveS(  );

	int subVer( )	{ return VER_ARH; }
	
	int messPeriod(){ return m_mess_per; }
	int valPeriod()	{ return m_val_per; }
	
	void messPeriod( int ivl );
	void valPeriod( int ivl )	{ m_val_per = ivl; }
	
	void subLoad( );
	void subSave( );
	void subStart( );
	void subStop( );

	//- Value archives functions -
	void valList( vector<string> &list )	{ chldList(m_aval,list); }
        bool valPresent( const string &iid )  	{ return chldPresent(m_aval,iid); }
	void valAdd( const string &iid, const string &idb = "*.*" );
	void valDel( const string &iid, bool db = false )	{ chldDel(m_aval,iid,-1,db); }
	AutoHD<TVArchive> valAt( const string &iid )
        { return chldAt(m_aval,iid); }

	void setActValArch( const string &id, bool val );

	//- Archivators -
	AutoHD<TTipArchivator> at( const string &name )	{ return modAt(name); }

	//- Message archive function -
 	void messPut( time_t tm, const string &categ, TMess::Type level, const string &mess );	
	void messPut( const vector<TMess::SRec> &recs );
        void messGet( time_t b_tm, time_t e_tm, vector<TMess::SRec> & recs, const string &category = "", TMess::Type level = TMess::Debug, const string &arch = "" );

	TElem &messE()	{ return el_mess; }
	TElem &valE() 	{ return el_val; }
	TElem &aValE()	{ return el_aval; }

    private:
	//Methods
	string optDescr(  );

	static void ArhMessTask( union sigval obj );
	static void *ArhValTask( void *param );

	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );

	int messBufLen( )	{ return m_buf.size(); }
	void messBufLen(int len);

    private:
	//Attributes
	TElem  		el_mess, 	//Message archivator's DB elements
			el_val, 	//Value archivator's DB elements
			el_aval;	//Value archives DB elements

	//- Messages archiving -
	char	buf_err;		//Buffer error
	int   	m_mess_per;		//Message arhiving period
	timer_t	tmIdMess;   		//Messages timer
	bool   	prc_st_mess;		//Process messages flag
	//-- Messages buffer --
	int    	m_res;			//Mess access resource
	unsigned head_buf, 		//Head of messages buffer
		head_lstread;    	//Last read and archived head of messages buffer
	vector<TMess::SRec> m_buf; 	//Messages buffer
 	//-- Request mess params --
	time_t	m_beg, m_end;
	string	m_cat;
	int	m_lvl;
	string	m_archtor;
	
	//- Value archiving -
	int     m_val_per;             	//Value arhiving period
	int	m_val_prior;		//Value archive task priority
	pthread_t m_val_pthr;		//Value get realtime pthread	
	bool	prc_st_val;		//Process value flag
	bool    endrun_req_val;		//Endrun value request
	int	m_aval, v_res;
	vector< AutoHD<TVArchive> > act_up_src;
};

#endif // TArchiveS_H
