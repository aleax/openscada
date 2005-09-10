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

#include <math.h>

#include <tsys.h>
#include <tmess.h>
#include "tfunctions.h"

//List of function libraries
TFunctionS::TFunctionS(TSYS *app) : TSubSYS(app,"Functions","Functions",false), run_st(false)
{
    m_lb = grpAdd();
}

TFunctionS::~TFunctionS()
{

}

void TFunctionS::subStart( )
{
    vector<string> lst;
    list(lst);
    for( int i_f = 0; i_f < lst.size(); i_f++ )
        at(lst[i_f]).at().start(true);
	    
    run_st = true;
}

void TFunctionS::subStop( )
{
    vector<string> lst;
    list(lst);
    for( int i_f = 0; i_f < lst.size(); i_f++ )
        at(lst[i_f]).at().start(false);
		    
    run_st = false;
}

void TFunctionS::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    switch(cmd)
    {
	case TCntrNode::Info:
	    TSubSYS::cntrCmd_( a_path, opt, cmd );       //Call parent
	
	    ctrInsNode("area",0,opt,a_path.c_str(),"/lslib",Mess->I18N("Function's libraries"));
	    ctrMkNode("list",opt,a_path.c_str(),"/lslib/lib",Mess->I18N("Libraries"),0444,0,0,"br")->
		attr_("idm","1")->attr_("mode","att")->attr_("br_pref","_");	
	    break;
	case TCntrNode::Get:
	    if( a_path == "/lslib/lib" )
	    {
		vector<string> list_el;
		list(list_el);
		opt->childClean();
		for( unsigned i_f=0; i_f < list_el.size(); i_f++ )
		    ctrSetS( opt, at(list_el[i_f]).at().name(), list_el[i_f].c_str() );
      	    }    	
	    else TSubSYS::cntrCmd_( a_path, opt, cmd );
	    break;
	case TCntrNode::Set:
	    TSubSYS::cntrCmd_( a_path, opt, cmd );
	    break;
    }
}

AutoHD<TCntrNode> TFunctionS::ctrAt( const string &br )
{
    if( br.substr(0,1) == "_" )	return at(br.substr(1));
    else throw TError("(Functions) Branch %s error!",br.c_str());
}

//Function library abstract object
TLibFunc::TLibFunc( const string &iid ) : m_id(iid), run_st(false)
{
    m_fnc = grpAdd();
}

TLibFunc::~TLibFunc()
{

}

void TLibFunc::start( bool val )  
{ 
    vector<string> lst;
    list(lst);
    for( int i_f = 0; i_f < lst.size(); i_f++ )
	at(lst[i_f]).at().start(val);
	
    run_st = val; 
}

void TLibFunc::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18N("Function's library: ")+id());
	ctrMkNode("area",opt,a_path.c_str(),"/lib",Mess->I18N("Library"));	
	ctrMkNode("area",opt,a_path.c_str(),"/lib/st",Mess->I18N("State"));
        ctrMkNode("fld",opt,a_path.c_str(),"/lib/st/st",Mess->I18N("Runing"),0664,0,0,"bool");
        ctrMkNode("area",opt,a_path.c_str(),"/lib/cfg",Mess->I18N("Config"));	
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/id",Mess->I18N("Id"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/name",Mess->I18N("Name"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/cfg/descr",Mess->I18N("Description"),0444,0,0,"str")->
	    attr_("cols","90")->attr_("rows","4");
	ctrMkNode("area",opt,a_path.c_str(),"/func",Mess->I18N("Functions"));	
	ctrMkNode("list",opt,a_path.c_str(),"/func/func",Mess->I18N("Functions"),0444,0,0,"br")->
	    attr_("idm","1")->attr_("mode","att")->attr_("br_pref","_");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/lib/st/st" )		ctrSetB( opt, run_st );
	else if( a_path == "/lib/cfg/id" )	ctrSetS( opt, id() );
	else if( a_path == "/lib/cfg/name" )	ctrSetS( opt, name() );
	else if( a_path == "/lib/cfg/descr" )	ctrSetS( opt, descr() );
	else if( a_path == "/func/func" )
	{
	    vector<string> list_el;
	    list(list_el);
	    opt->childClean();
	    for( unsigned i_f=0; i_f < list_el.size(); i_f++ )
		ctrSetS( opt, at(list_el[i_f]).at().name(), list_el[i_f].c_str() );
	}
	else throw TError("(LibFunc)Branch %s error",a_path.c_str());
    }    
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/lib/st/st" )	start(ctrGetB(opt));
	else throw TError("(LibFunc)Branch %s error",a_path.c_str());
    }
}

AutoHD<TCntrNode> TLibFunc::ctrAt( const string &br )
{
    if( br.substr(0,1) == "_" )	return at(TSYS::strEncode(br.substr(1),TSYS::PathEl));
    else throw TError("(LibFunc)Branch %s error",br.c_str());
}

//Function abstract object
TFunction::TFunction( const string &iid ) : m_id(iid), m_tval(NULL), run_st(false)
{

}

TFunction::~TFunction()
{
    for( int i_io = 0; i_io < m_io.size(); i_io++ )
	delete m_io[i_io];
}

void TFunction::preDisable(int flag)
{
    if( m_tval ) { delete m_tval; m_tval = NULL; }
    if( used.size() )
    {
	string mess("Function used by: ");
	for( int i=0; i < used.size(); i++ )
	    mess+=used[i]->name()+", ";
	throw TError(nodePath().c_str(),mess.c_str());
    }
}

int TFunction::ioSize()
{
    return m_io.size();
}

IO *TFunction::io( int iid )
{    
    if( iid >= m_io.size() ) throw TError(id().c_str(),"Index %d broken!",iid);
    return m_io[iid];
}

int TFunction::ioId( const string &id )
{    
    for( int i_io = 0; i_io < m_io.size(); i_io++ )
	if( m_io[i_io]->id() == id ) return i_io;
    return -1;	
}

void TFunction::ioList( vector<string> &list )
{
    for( int i_io = 0; i_io < m_io.size(); i_io++ )
	list.push_back( m_io[i_io]->id() );
}

void TFunction::ioAdd( IO *io )
{
    preIOCfgChange();    
    m_io.push_back(io);
    io->owner = this;    
    postIOCfgChange();
}

void TFunction::ioIns( IO *io, int pos )
{
    if( pos < 0 || pos > m_io.size() )	
	pos = m_io.size();
	
    preIOCfgChange();	
    m_io.insert(m_io.begin()+pos,io);
    io->owner = this;    
    postIOCfgChange();
}

void TFunction::ioDel( int pos )
{
    if( pos < 0 || pos >= m_io.size() )
        throw TError(id().c_str(),"Delete position <%d> error.",pos);
	
    preIOCfgChange();    	
    m_io.erase(m_io.begin()+pos);
    postIOCfgChange();
}

void TFunction::ioMove( int pos, int to )
{
    if( pos < 0 || pos >= m_io.size() || to < 0 || to >= m_io.size() )
	throw TError(id().c_str(),"Move parameters <%d:%d> error.",pos,to);
	
    preIOCfgChange();    	
    IO *io = m_io[to];
    m_io[to] = m_io[pos];
    m_io[pos] = io;  
    postIOCfgChange();  	
}    

void TFunction::preIOCfgChange()
{
    for(unsigned i=0; i < used.size(); i++)
	used[i]->preIOCfgChange();
}

void TFunction::postIOCfgChange()
{
    for(unsigned i=0; i < used.size(); i++)
        used[i]->postIOCfgChange();
}

void TFunction::valAtt( TValFunc *vfnc )
{
    for(unsigned i=0; i < used.size() ;i++)
	if(used[i] == vfnc) 
	    throw TError(nodePath().c_str(),"Value <%s> already attached!",vfnc->name().c_str());
    used.push_back(vfnc);
}

void TFunction::valDet( TValFunc *vfnc )
{
    for(unsigned i=0; i < used.size() ;i++)
	if(used[i] == vfnc)
        {
            used.erase(used.begin()+i);
    	    break;
        }
}

void TFunction::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    if( cmd==TCntrNode::Info )
    {
	ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18N("Function: ")+name());
	ctrMkNode("area",opt,a_path.c_str(),"/func",Mess->I18N("Function"));
	ctrMkNode("area",opt,a_path.c_str(),"/func/st",Mess->I18N("State"));
        ctrMkNode("fld",opt,a_path.c_str(),"/func/st/st",Mess->I18N("Runing"),0664,0,0,"bool");
        ctrMkNode("area",opt,a_path.c_str(),"/func/cfg",Mess->I18N("Config"));	
	ctrMkNode("fld",opt,a_path.c_str(),"/func/cfg/id",Mess->I18N("Id"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/func/cfg/name",Mess->I18N("Name"),0444,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/func/cfg/descr",Mess->I18N("Description"),0444,0,0,"str")->
	    attr_("cols","90")->attr_("rows","4");
	ctrMkNode("area",opt,a_path.c_str(),"/io",Mess->I18N("IO"));	
	ctrMkNode("table",opt,a_path.c_str(),"/io/io",Mess->I18N("IO"),0440,0,0);
	ctrMkNode("list",opt,a_path.c_str(),"/io/io/0",Mess->I18N("Id"),0440,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/io/io/1",Mess->I18N("Name"),0440,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/io/io/2",Mess->I18N("Type"),0440,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/io/io/3",Mess->I18N("Mode"),0440,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/io/io/4",Mess->I18N("Hide"),0440,0,0,"bool");
	ctrMkNode("list",opt,a_path.c_str(),"/io/io/5",Mess->I18N("Default"),0440,0,0,"str");
	//ctrMkNode("list",opt,a_path.c_str(),"/io/io/6",Mess->I18N("Vector"),0440,0,0,"str");
	ctrMkNode("area",opt,a_path.c_str(),"/test",Mess->I18N("Test"));
	ctrMkNode("fld",opt,a_path.c_str(),"/test/en",Mess->I18N("Enable"),0660,0,0,"bool");
	//Add test form
	if( m_tval )
	{
	    ctrMkNode("area",opt,a_path.c_str(),"/test/io",Mess->I18N("IO"));
    	    //Put io
    	    for( int i_io = 0; i_io < ioSize(); i_io++ )
    	    {
		if( m_io[i_io]->hide() ) continue;
	    
		char *tp = "";
		if(io(i_io)->type() == IO::String)	tp = "str";
		else if(io(i_io)->type() == IO::Integer)tp = "dec";
		else if(io(i_io)->type() == IO::Real)	tp = "real";
		else if(io(i_io)->type() == IO::Boolean)tp = "bool";
		
		ctrMkNode("fld",opt,a_path.c_str(),("/test/io/"+io(i_io)->id()).c_str(),io(i_io)->name(),0664,0,0,tp);
	    }
	    //Add Calc button and Calc time
	    ctrMkNode("fld",opt,a_path.c_str(),"/test/tm",Mess->I18N("Calc time (mks)"),0444,0,0,"real");
	    ctrMkNode("comm",opt,a_path.c_str(),"/test/calc",Mess->I18N("Calc"));
	}
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/func/st/st" )		ctrSetB( opt, run_st );
	else if( a_path == "/func/cfg/id" )	ctrSetS( opt, id() );
	else if( a_path == "/func/cfg/name" )	ctrSetS( opt, name() );
	else if( a_path == "/func/cfg/descr" )	ctrSetS( opt, descr() );
	else if( a_path == "/io/io" )
	{
	    XMLNode *n_id	= ctrId(opt,"0");
	    XMLNode *n_nm  	= ctrId(opt,"1");
	    XMLNode *n_type 	= ctrId(opt,"2");
	    XMLNode *n_mode 	= ctrId(opt,"3");
	    XMLNode *n_hide 	= ctrId(opt,"4");
	    XMLNode *n_def 	= ctrId(opt,"5");
	    //XMLNode *n_vect	= ctrId(opt,"6");
	    for( int i_io = 0; i_io < ioSize(); i_io++ )
	    { 
	      	ctrSetS(n_id,io(i_io)->id());
		ctrSetS(n_nm,io(i_io)->name());
		//Make type
		if( io(i_io)->type() == IO::String )		ctrSetS(n_type,Mess->I18N("String"));
		else if( io(i_io)->type() == IO::Integer )	ctrSetS(n_type,Mess->I18N("Integer"));
		else if( io(i_io)->type() == IO::Real )		ctrSetS(n_type,Mess->I18N("Real"));
		else if( io(i_io)->type() == IO::Boolean )	ctrSetS(n_type,Mess->I18N("Bool"));
		else if( io(i_io)->type() == IO::Vector )	ctrSetS(n_type,Mess->I18N("Vector"));
		//Make mode
		if( io(i_io)->mode() == IO::Output )		ctrSetS(n_mode,Mess->I18N("Output"));
		else if( io(i_io)->mode() == IO::Return )	ctrSetS(n_mode,Mess->I18N("Return"));
		else if( io(i_io)->mode() == IO::Input )	ctrSetS(n_mode,Mess->I18N("Input"));
		
		if( io(i_io)->hide() )	ctrSetB(n_hide,true);
		else		        ctrSetB(n_hide,false);
		
		ctrSetS(n_def,io(i_io)->def());
		//ctrSetS(n_vect,io(i_io)->vector());
	    }	
    	}
	else if( a_path == "/test/en" )	ctrSetB( opt, m_tval?true:false );    
	else if( m_tval && a_path == "/test/tm" )	ctrSetR( opt, m_tval->calcTm() );
	else if( m_tval && a_path.substr(0,8) == "/test/io" )
	{
	    for( int i_io = 0; i_io < m_io.size(); i_io++ )
		if( TSYS::pathLev(a_path,2) == io(i_io)->id() )
		{
		    if(io(i_io)->type() == IO::String) 	ctrSetS( opt, m_tval->getS(i_io) );
		    else if(io(i_io)->type() == IO::Integer)	ctrSetI( opt, m_tval->getI(i_io) );
		    else if(io(i_io)->type() == IO::Real)	ctrSetR( opt, m_tval->getR(i_io) );
		    else if(io(i_io)->type() == IO::Boolean)  	ctrSetB( opt, m_tval->getB(i_io) );
		}    
	}
	else throw TError("(Function)Branch %s error",a_path.c_str());	
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/func/st/st" )	start(ctrGetB( opt ));
	else if( a_path == "/test/en" )
	{
	    if( ctrGetB( opt ) && !m_tval ) 
	    { 
		m_tval = new TValFunc(id()+"_test",this);
		m_tval->dimens(true); 
	    }
	    if( !ctrGetB( opt ) && m_tval ) { delete m_tval; m_tval = NULL; }
	}	
	else if( m_tval && a_path.substr(0,8) == "/test/io" )
	{
	    for( int i_io = 0; i_io < ioSize(); i_io++ )
		if( TSYS::pathLev(a_path,2) == io(i_io)->id() )
		{
		    if(io(i_io)->type() == IO::String)	m_tval->setS(i_io, ctrGetS( opt ));
		    else if(io(i_io)->type() == IO::Integer)	m_tval->setI(i_io, ctrGetI( opt ));
		    else if(io(i_io)->type() == IO::Real)       m_tval->setR(i_io, ctrGetR( opt ));
		    else if(io(i_io)->type() == IO::Boolean)	m_tval->setB(i_io, ctrGetB( opt ));
		}
	}
	else if( m_tval && a_path == "/test/calc" )	m_tval->calc();
	else throw TError("(Function)Branch %s error",a_path.c_str());    
    }                   
}

IO::IO( const char *iid, const char *iname, IO::Type itype, IO::Mode imode, const char *idef, bool ihide, const char *ivect )
{
    m_id = iid;
    m_name = iname;
    m_type = itype;
    m_mode = imode;
    m_hide = ihide;
    m_def  = idef;
    m_vect = ivect;
}	
	
void IO::id( const string &val )
{ 
    owner->preIOCfgChange();
    m_id = val; 
    owner->postIOCfgChange();
}

void IO::name( const string &val ) 	
{ 
    owner->preIOCfgChange();
    m_name = val; 
    owner->postIOCfgChange();
}

void IO::type( Type val ) 	
{
    owner->preIOCfgChange();
    m_type = val;
    owner->postIOCfgChange();
}

void IO::mode( Mode val ) 	
{ 
    owner->preIOCfgChange();
    m_mode = val; 
    owner->postIOCfgChange();
}

void IO::def( const string &val )
{ 
    owner->preIOCfgChange();
    m_def = val; 
    owner->postIOCfgChange();
}

void IO::vector( const string &val )
{ 
    owner->preIOCfgChange();
    m_vect = val; 
    owner->postIOCfgChange();
}

void IO::hide( bool val )	
{ 
    owner->preIOCfgChange();
    m_hide = val; 
    owner->postIOCfgChange();
}

//===================================================
//========== TValFunc ===============================
//===================================================
TValFunc::TValFunc( const string &iname, TFunction *ifunc ) : m_name(iname), m_func(NULL), m_dimens(false), tm_calc(0.0)
{   
    func(ifunc);    
}

TValFunc::~TValFunc( )
{
    if( m_func ) funcDisConnect();
}

void TValFunc::func( TFunction *ifunc, bool att_det )
{
    if( m_func ) funcDisConnect(att_det);
    if( ifunc ) 
    {
	m_func = ifunc;
	if(att_det) m_func->valAtt(this);
	for( int i_vl = 0; i_vl < m_func->ioSize(); i_vl++ )
	{
	    SVl val;
	    val.tp = m_func->io(i_vl)->type();
	    if( val.tp == IO::String ) 	val.vl = new string(m_func->io(i_vl)->def());
	    else if( val.tp == IO::Integer )	val.vl = new int(atoi(m_func->io(i_vl)->def().c_str()));
	    else if( val.tp == IO::Real ) 	val.vl = new double(atof(m_func->io(i_vl)->def().c_str()));
	    else if( val.tp == IO::Boolean )	val.vl = new bool(atoi(m_func->io(i_vl)->def().c_str()));
	    m_val.push_back(val);
	}
    }
}

void TValFunc::funcDisConnect( bool det )
{
    if( m_func )
    {
	for( int i_vl = 0; i_vl < m_val.size(); i_vl++ )
	    if( m_val[i_vl].tp == IO::String )	delete (string *)m_val[i_vl].vl;
	    else if( m_val[i_vl].tp == IO::Integer )	delete (int *)m_val[i_vl].vl;
	    else if( m_val[i_vl].tp == IO::Real )	delete (double *)m_val[i_vl].vl;
	    else if( m_val[i_vl].tp == IO::Boolean )	delete (bool *)m_val[i_vl].vl;
	m_val.clear();    
	if(det)
	{ 
	    m_func->valDet(this);
	    m_func = NULL;
	}
    }
}

int TValFunc::ioId( const string &iid )
{
    if( !m_func )	throw TError("ValFnc","Function <%s> no attached!",iid.c_str());
    return m_func->ioId(iid);
}

void TValFunc::ioList( vector<string> &list )
{
    if( !m_func )       throw TError("ValFnc","Function no attached!");
    return m_func->ioList(list);
}

int TValFunc::ioSize( )
{
    if( !m_func )       throw TError("ValFnc","Function no attached!");
    return m_func->ioSize();
}

string TValFunc::getS( unsigned id )
{
    if( id >= m_val.size() )    throw TError("ValFnc","Id or IO %d error!",id);
    switch(m_val[id].tp)
    {
	case IO::String:	return *(string *)m_val[id].vl;
	case IO::Integer:	return TSYS::int2str(*(int *)m_val[id].vl);
	case IO::Real:		return TSYS::real2str(*(double *)m_val[id].vl);
	case IO::Boolean:	return TSYS::int2str(*(bool *)m_val[id].vl);
    }
    return "";
}	    
	
int TValFunc::getI( unsigned id )
{
    if( id >= m_val.size() )    throw TError("ValFnc","Id or IO %d error!",id);
    switch(m_val[id].tp)
    {
	case IO::String:	return atoi(((string *)m_val[id].vl)->c_str());
	case IO::Integer:	return *(int *)m_val[id].vl;
	case IO::Real:		return (int)(*(double *)m_val[id].vl);
	case IO::Boolean:	return *(bool *)m_val[id].vl;
    }
    return 0;
}	
	
double TValFunc::getR( unsigned id )
{
    if( id >= m_val.size() )    throw TError("ValFnc","Id or IO %d error!",id);
    switch(m_val[id].tp)
    {
	case IO::String:	return atof(((string *)m_val[id].vl)->c_str());
	case IO::Integer:	return *(int *)m_val[id].vl;
	case IO::Real:		return *(double *)m_val[id].vl;
	case IO::Boolean:	return *(bool *)m_val[id].vl;
    }
    return 0.0;
}
	
bool TValFunc::getB( unsigned id )
{
    if( id >= m_val.size() )    throw TError("ValFnc","Id or IO %d error!",id);
    switch(m_val[id].tp)
    {
	case IO::String:	return atoi(((string *)m_val[id].vl)->c_str());
	case IO::Integer:	return *(int *)m_val[id].vl;
	case IO::Real:		return *(double *)m_val[id].vl;
	case IO::Boolean:	return *(bool *)m_val[id].vl;
    }
    return false;
}
	
void TValFunc::setS( unsigned id, const string &val )
{
    if( id >= m_val.size() )    throw TError("ValFnc","Id or IO %d error!",id);
    switch(m_val[id].tp)
    {
	case IO::String:	*(string *)m_val[id].vl = val;	break;					
	case IO::Integer:	*(int *)m_val[id].vl = atoi(val.c_str());	break;
	case IO::Real:		*(double *)m_val[id].vl = atof(val.c_str());	break;
	case IO::Boolean:	*(bool *)m_val[id].vl = atoi(val.c_str());	break;
    }
}
	
void TValFunc::setI( unsigned id, int val )
{
    if( id >= m_val.size() )    throw TError("ValFnc","Id or IO %d error!",id);
    switch(m_val[id].tp)
    {
	case IO::String:	*(string *)m_val[id].vl = TSYS::int2str(val);	break;
	case IO::Integer:	*(int *)m_val[id].vl = val;	break;
	case IO::Real:		*(double *)m_val[id].vl = val;	break;
	case IO::Boolean:	*(bool *)m_val[id].vl = val;	break;
    }
}
	
void TValFunc::setR( unsigned id, double val )
{
    if( id >= m_val.size() )    throw TError("ValFnc","Id or IO %d error!",id);
    if( isnan(val) ) val = 0.;	//Check for 'Not a Number'
    switch(m_val[id].tp)
    {
	case IO::String:	*(string *)m_val[id].vl = TSYS::real2str(val);	break;
	case IO::Integer:	*(int *)m_val[id].vl = (int)val;break;
	case IO::Real:		*(double *)m_val[id].vl = val;	break;
	case IO::Boolean:	*(bool *)m_val[id].vl = val;	break;
    }
}	
	
void TValFunc::setB( unsigned id, bool val )
{
    if( id >= m_val.size() )    throw TError("ValFnc","Id or IO %d error!",id);
    switch(m_val[id].tp)
    {
	case IO::String:	*(string *)m_val[id].vl = TSYS::int2str(val);	break;
	case IO::Integer:	*(int *)m_val[id].vl = val;	break;
	case IO::Real:		*(double *)m_val[id].vl = val;	break;
	case IO::Boolean:	*(bool *)m_val[id].vl = val;	break;
    }
}

void TValFunc::calc( )
{ 
    if( !m_func || !m_func->startStat() ) return;    
    if( !m_dimens ) m_func->calc(this);
    else
    {
	unsigned long long t_cnt = SYS->shrtCnt();
	m_func->calc(this); 
	tm_calc = 1.0e6*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());
    }
}

void TValFunc::preIOCfgChange()
{    
    func( NULL, false );
}

void TValFunc::postIOCfgChange()
{
    func( m_func, false );
}
