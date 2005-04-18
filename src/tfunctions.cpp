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


#include <tsys.h>
#include <tmessage.h>
#include "tfunctions.h"

//List of function libraries
TFunctionS::TFunctionS(TKernel *app) : m_owner(app)
{
    m_lb = grpAdd();
    nodeEn();
}

TFunctionS::~TFunctionS()
{

}

string TFunctionS::name()
{
    return Mess->I18N("Functions");
}    

void TFunctionS::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18N("Function subsystem"));
	ctrMkNode("area",opt,a_path.c_str(),"/lslib",Mess->I18N("Function's libraries"));
	ctrMkNode("list",opt,a_path.c_str(),"/lslib/lib",Mess->I18N("Libraries"),0444,0,0,"br")->
	    attr_("mode","att")->attr_("br_pref","_");	
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/lslib/lib" )
	{
	    vector<string> list_el;
	    list(list_el);
	    opt->childClean();
	    for( unsigned i_f=0; i_f < list_el.size(); i_f++ )
		ctrSetS( opt, list_el[i_f] );
      	}    	
	else throw TError("(%s) Branch %s error",__func__,a_path.c_str());
    }
}

AutoHD<TCntrNode> TFunctionS::ctrAt1( const string &br )
{
    if( br.substr(0,1) == "_" )	return at(br.substr(1));
    else throw TError("<{%s}> Branch %s error!",__func__,br.c_str());
}

//Function library abstract object
TLibFunc::TLibFunc( const string &iid ) : m_id(iid)
{
    m_fnc = grpAdd();
    nodeEn();
}

TLibFunc::~TLibFunc()
{

}

void TLibFunc::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/","Function's library: "+id());
	ctrMkNode("area",opt,a_path.c_str(),"/lib","Library");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/id","Id",0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/name","Name",0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/descr","Description",0444,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/lib/func","Functions",0444,0,0,"br")->
	    attr_("mode","att");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/lib/id" )		ctrSetS( opt, id() );
	else if( a_path == "/lib/name" )	ctrSetS( opt, name() );
	else if( a_path == "/lib/descr" )	ctrSetS( opt, descr() );
	else if( a_path == "/lib/func" )
	{
	    vector<string> list_el;
	    list(list_el);
	    opt->childClean();
	    for( unsigned i_f=0; i_f < list_el.size(); i_f++ )
		ctrSetS( opt, at(list_el[i_f]).at().name(), list_el[i_f].c_str() );
	}
	else throw TError("(%s) Branch %s error",__func__,a_path.c_str());
    }    
}

AutoHD<TCntrNode> TLibFunc::ctrAt1( const string &br )
{
    return at(pathLev(br,2));
}

//Function abstract object
TFunction::TFunction( const string &iid ) : m_id(iid), m_tval(NULL)
{

}

TFunction::~TFunction()
{
    for( int i_io = 0; i_io < m_io.size(); i_io++ )
	delete m_io[i_io];
}

TFunction::IO *TFunction::io( int id )
{    
    if( id >= m_io.size() || m_io[id] == NULL ) throw TError("Index broken!");
    return m_io[id];
}

char TFunction::ioType( int id ) 
{ 
    if( id >= m_io.size() || m_io[id] == NULL ) throw TError("Index broken!");
    return m_io[id]->type; 
}

int TFunction::ioId( const string &id )
{    
    for( int i_io = 0; i_io < m_io.size(); i_io++ )
	if( m_io[i_io]->id == id ) return i_io;
    return -1;	
}

void TFunction::ioList( vector<string> &list )
{
    for( int i_io = 0; i_io < m_io.size(); i_io++ )
	list.push_back( m_io[i_io]->id );
}

void TFunction::ioAdd( IO *io )
{
    m_io.push_back(io);
}

void TFunction::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/","Function: "+id());
	ctrMkNode("area",opt,a_path.c_str(),"/func","Function");
	ctrMkNode("fld",opt,a_path.c_str(),"/func/id","Id",0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/func/name","Name",0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/func/descr","Description",0444,0,0,"str")->
	    attr_("cols","90")->attr_("rows","3");
	ctrMkNode("table",opt,a_path.c_str(),"/func/io","IO",0440,0,0);
	ctrMkNode("list",opt,a_path.c_str(),"/func/io/0","Id",0440,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/func/io/1","Name",0440,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/func/io/2","Options",0440,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/func/io/3","Default",0440,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/func/io/4","Vector",0440,0,0,"str");
	ctrMkNode("area",opt,a_path.c_str(),"/test","Test");
	ctrMkNode("fld",opt,a_path.c_str(),"/test/en","Enable",0660,0,0,"bool");
	//Add test form
	if( m_tval )
	{
	    ctrMkNode("area",opt,a_path.c_str(),"/test/in","Inputs");
	    ctrMkNode("area",opt,a_path.c_str(),"/test/out","Outputs");	    
    	    //Put io
    	    for( int i_io = 0; i_io < m_io.size(); i_io++ )
    	    {
		if( m_io[i_io]->type&IO_HIDE ) continue;
	    
		string io_area = "/test/in";
    		if( m_io[i_io]->type&(IO_OUT|IO_RET) )
		    io_area = "/test/out";
		char *tp = "";
		if(ioType(i_io)&IO_STR)		tp = "str";
		else if(ioType(i_io)&IO_INT)	tp = "dec";
		else if(ioType(i_io)&IO_REAL)	tp = "real";
		else if(ioType(i_io)&IO_BOOL)	tp = "bool";
		
		ctrMkNode("fld",opt,a_path.c_str(),(io_area+"/"+m_io[i_io]->id).c_str(),m_io[i_io]->name,0664,0,0,tp);
	    }
	    //Add Calc button and Calc time
	    ctrMkNode("fld",opt,a_path.c_str(),"/test/tm","Calc time (mks)",0444,0,0,"real");
	    ctrMkNode("comm",opt,a_path.c_str(),"/test/calc","Calc");
	}
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/func/id" )		ctrSetS( opt, id() );
	else if( a_path == "/func/name" )	ctrSetS( opt, name() );
	else if( a_path == "/func/descr" )	ctrSetS( opt, descr() );
	else if( a_path == "/func/io" )
	{
	    XMLNode *n_id	= ctrId(opt,"0");
	    XMLNode *n_nm  	= ctrId(opt,"1");
	    XMLNode *n_opt 	= ctrId(opt,"2");
	    XMLNode *n_def 	= ctrId(opt,"3");
	    XMLNode *n_vect	= ctrId(opt,"4");
	    for( int i_io = 0; i_io < m_io.size(); i_io++ )
	    { 
	      	ctrSetS(n_id,m_io[i_io]->id);
		ctrSetS(n_nm,m_io[i_io]->name);
		//Make option string
		string v_opt;
		if( m_io[i_io]->type&IO_STR ) 		v_opt = "STRING ";
		else if( m_io[i_io]->type&IO_INT ) 	v_opt = "INTEGER ";
		else if( m_io[i_io]->type&IO_REAL ) 	v_opt = "REAL ";
		else if( m_io[i_io]->type&IO_BOOL ) 	v_opt = "BOOL ";
		else if( m_io[i_io]->type&IO_VECT ) 	v_opt += "Vector ";
		if( m_io[i_io]->type&IO_OUT ) 		v_opt += "| Output";
		else if( m_io[i_io]->type&IO_RET )	v_opt += "| Return";
		else 					v_opt += "| Input";
		if( m_io[i_io]->type&IO_HIDE ) 		v_opt += " | Hide";
		ctrSetS(n_opt,v_opt);
		ctrSetS(n_def,m_io[i_io]->def);
		ctrSetS(n_vect,m_io[i_io]->vector);
	    }	
    	}
	else if( a_path == "/test/en" )	ctrSetB( opt, m_tval?true:false );    
	else if( m_tval && a_path == "/test/tm" )	ctrSetR( opt, m_tval->calcTm() );
	else if( m_tval && (a_path.substr(0,8) == "/test/in" || a_path.substr(0,9) == "/test/out") )
	{
	    for( int i_io = 0; i_io < m_io.size(); i_io++ )
		if( pathLev(a_path,2) == m_io[i_io]->id )
		{
		    if(m_io[i_io]->type&IO_STR)	 	ctrSetS( opt, m_tval->getS(i_io) );
		    else if(m_io[i_io]->type&IO_INT)	ctrSetI( opt, m_tval->getI(i_io) );
		    else if(m_io[i_io]->type&IO_REAL)       ctrSetR( opt, m_tval->getR(i_io) );
		    else if(m_io[i_io]->type&IO_BOOL)       ctrSetB( opt, m_tval->getB(i_io) );
		}    
	}
	else throw TError("(%s) Branch %s error",__func__,a_path.c_str());	
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/test/en" )
	{
	    if( ctrGetB( opt ) && !m_tval ) 
	    { 
		m_tval = new TValFunc(this); 
		m_tval->dimens(true); 
	    }
	    if( !ctrGetB( opt ) && m_tval ) { delete m_tval; m_tval = NULL; }
	}	
	else if( m_tval && (a_path.substr(0,8) == "/test/in" || a_path.substr(0,9) == "/test/out") )
	{
	    for( int i_io = 0; i_io < m_io.size(); i_io++ )
		if( pathLev(a_path,2) == m_io[i_io]->id )
		{
		    if(m_io[i_io]->type&IO_STR)	        m_tval->setS(i_io, ctrGetS( opt ));
		    else if(m_io[i_io]->type&IO_INT)	m_tval->setI(i_io, ctrGetI( opt ));
		    else if(m_io[i_io]->type&IO_REAL)       m_tval->setR(i_io, ctrGetR( opt ));
		    else if(m_io[i_io]->type&IO_BOOL)       m_tval->setB(i_io, ctrGetB( opt ));
		}
	}
	else if( m_tval && a_path == "/test/calc" )	m_tval->calc();
	else throw TError("(%s) Branch %s error",__func__,a_path.c_str());    
    }
}

TFunction::IO::IO( const char *iid, const char *iname, char itype, const char *idef, const char *ivect )
{
    id   = iid;
    name = iname;
    type = itype;
    def  = idef;
    vector = ivect;
}	

//TValFunc
TValFunc::TValFunc( TFunction *ifunc ) : m_func(NULL), m_dimens(false), tm_calc(0.0)
{   
    func(ifunc);
}

TValFunc::~TValFunc( )
{
    if( m_func ) funcDisConnect();
}

void TValFunc::func( TFunction *ifunc )
{
    if( m_func ) funcDisConnect();
    if( ifunc ) 
    {
	m_func = ifunc;
	for( int i_vl = 0; i_vl < m_func->m_io.size(); i_vl++ )
	{
	    SVl val;
	    //If free index
	    if( m_func->m_io[i_vl] == NULL ) 
	    {
		val.tp = 0;
		m_val.push_back(val);
		continue;
	    }
	    val.tp = m_func->m_io[i_vl]->type;
	    if( val.tp&IO_STR ) 	val.vl = new string(m_func->m_io[i_vl]->def);
	    else if( val.tp&IO_INT )	val.vl = new int(atoi(m_func->m_io[i_vl]->def.c_str()));
	    else if( val.tp&IO_REAL ) 	val.vl = new double(atof(m_func->m_io[i_vl]->def.c_str()));
	    else if( val.tp&IO_BOOL )	val.vl = new bool(atoi(m_func->m_io[i_vl]->def.c_str()));
	    m_val.push_back(val);
	}
    }
}

void TValFunc::funcDisConnect( )
{
    if( m_func )
    {
	for( int i_vl = 0; i_vl < m_val.size(); i_vl++ )
	    if( m_val[i_vl].tp&IO_STR )		delete (string *)m_val[i_vl].vl;
	    else if( m_val[i_vl].tp&IO_INT )	delete (int *)m_val[i_vl].vl;
	    else if( m_val[i_vl].tp&IO_REAL )	delete (double *)m_val[i_vl].vl;
	    else if( m_val[i_vl].tp&IO_BOOL )	delete (bool *)m_val[i_vl].vl;
	m_val.clear();    
	m_func = NULL;
    }
}

int TValFunc::ioId( const string &id )
{
    if( !m_func )	throw TError("Function no attached!");
    return m_func->ioId(id);
}

void TValFunc::ioList( vector<string> &list )
{
    if( !m_func )       throw TError("Function no attached!");
    return m_func->ioList(list);
}

void TValFunc::calc( )
{ 
    if( !m_func ) return;    
    if( !m_dimens ) m_func->calc(this);
    else
    {
	unsigned long long t_cnt = SYS->shrtCnt();
	m_func->calc(this); 
	tm_calc = 1.0e6*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
    }
}

