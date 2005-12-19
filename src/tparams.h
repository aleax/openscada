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

#ifndef TPARAMS_H
#define TPARAMS_H

#include <string>
#include <vector>

#include "tparam.h"
#include "tprmtmpl.h"
#include "tcontrollers.h"

using std::string;
using std::vector;

class TParam;
class TParamContr;

class TParamS : public TSubSYS
{
    friend class TParam;
    /** Public methods: */
    public:
	TParamS( );    
	~TParamS( );
	
        void subLoad( );
	void subSave( );
	void subStart( );
	void subStop( );				       

	//Parameters
	void list( vector<string> &list )	{ chldList(m_prm,list); }
	bool present( const string &param )	{ return chldPresent(m_prm,param); }
	void add( const string &id );
	void del( const string &id )		{ chldDel(m_prm,id); }
	AutoHD<TParam> at( const string &name, const string &who = "" )
	{ return chldAt(m_prm,name); }	    
	
	//Param's templates	
	void tplList( vector<string> &list )	{ chldList(m_tpl,list); }
	bool tplPresent( const string &tpl )	{ return chldPresent(m_tpl,tpl); }
	void tplAdd( const string &tpl );
	void tplDel( const string &tpl )	{ chldDel(m_tpl,tpl); }
        AutoHD<TPrmTempl> tplAt( const string &tpl, const string &who = "" )
	{ return chldAt(m_tpl,tpl); }
	
	TBDS::SName prmB();
        TBDS::SName tmplB();
	
	TElem	&prmE()		{ return el_prm; }
	TElem   &prmIOE() 	{ return el_prm_io; }
	TElem   &tplE()		{ return el_tmpl; }
	TElem   &tplIOE()	{ return el_tmpl_io; }
	
    /** Private methods: */
    private:
	string optDescr(  );
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	AutoHD<TCntrNode> TParamS::ctrAt( const string &a_path );
	
	void loadParams();
	void loadTemplates();
	void saveParams();	
	void saveTemplates();
	
	void prmCalc( const string & id, bool val );
	
	static void *Task(void *);
	
    /**Attributes: */
    private:    
	int	clc_res,
		m_prm,	//Params conteiner header
		m_tpl,	//Templates conteiner header
		m_per;	//Calc parameter template's algoritms (ms)
	double	tm_calc;//Calc time
	bool    run_st, endrun;
	pthread_t pthr_tsk;	//Calc pthread header
	vector< AutoHD<TParam> > clc_prm;	
	TBDS::SName	m_bd_prm, m_bd_tmpl;
	TElem	el_prm, el_prm_io, el_tmpl, el_tmpl_io;
};

#endif // TPARAMS_H
