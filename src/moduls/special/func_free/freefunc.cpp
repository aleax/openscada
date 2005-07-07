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

#include <deque>

#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include "freefunclibs.h"
#include "freelib.h"
#include "freefunc.h"

using std::deque;

using namespace FreeFunc;

Func *FreeFunc::p_fnc;

//================== Function ========================
Func::Func( const char *id, Lib *own, const char *name ) : 
    TConfig(&own->owner().elFnc()), TFunction(id), m_owner(own), parse_res(own->owner().parseRes( )),
    m_name(cfg("NAME").getS()), m_descr(cfg("DESCR").getS()), prg_src(cfg("FORMULA").getS())
{
    cfg("ID").setS(id);    
    m_name = name;

    //Init named constant table
    m_const.push_back(NConst("pi",3.14159265358));
    m_const.push_back(NConst("e",2.71828182845));
}

Func::~Func( )
{
    start(false);
}
	
void Func::postDisable(int flag)
{
    if( flag )
	try{ del( ); }
	catch(TError err)
	{ owner().owner().mPut("FREE_FUNC",TMess::Error,"%s",err.what().c_str()); }
}

Func &Func::operator=(Func &func)
{
    //======== Set name ============
    *(TConfig *)this = (TConfig&)func;
    if( !m_id.size() ) 	m_id = cfg("ID").getS();
    else 		cfg("ID").setS(m_id);
    //======== Copy IO =============
    //Clear current IO
    while( ioSize() ) ioDel(0);
    //Make new IO
    for( int i_io = 0; i_io < func.ioSize(); i_io++ )
	ioAdd( new IO( func.io(i_io)->id().c_str(), func.io(i_io)->name().c_str(), 
		       func.io(i_io)->type(), func.io(i_io)->mode(), 
		       func.io(i_io)->def().c_str(), func.io(i_io)->hide(), 
		       func.io(i_io)->vector().c_str() ) );
}

Lib &Func::owner()
{
    return *m_owner;
}

void Func::chID( const char *id )
{
    if( owner().avoid(id) )
	throw TError("Function with id <%d> already allow.");
    del();
    //Set new ID
    m_id = id;
    cfg("ID").setS(m_id);
    //Save new function
    save();
}

void Func::load( )
{
    TBDS &bd = owner().owner().owner().owner().db();
    bd.open(owner().BD()).at().fieldGet(*this);
    bd.close(owner().BD());
    
    loadIO( );
}

void Func::loadIO( )
{
    TConfig cfg(&owner().owner().elFncIO());
    
    TBDS &bd = owner().owner().owner().owner().db();
    TBDS::SName io_bd = owner().BD();
    io_bd.tbl += "_io";
    
    AutoHD<TTable> tbl = bd.open(io_bd);
    if( tbl.freeStat() ) return;
    
    int fld_cnt=0;
    vector<int>	u_pos;
    while( tbl.at().fieldSeek(fld_cnt++,cfg) )
	if( cfg.cfg("F_ID").getS() == id() )
	{	
	    string sid = cfg.cfg("ID").getS();
	    //Calc insert position	    
	    int pos = cfg.cfg("POS").getI();
	    int i_ps;
	    for( i_ps = 0; i_ps < u_pos.size(); i_ps++ )
		if( u_pos[i_ps] > pos )	break;
	    u_pos.insert(u_pos.begin()+i_ps,pos);
	    
	    if( ioId(sid) < 0 )
		ioIns( new IO(sid.c_str(),"",IO::Real,IO::Input), i_ps );
		
	    int id = ioId(sid);		
	    //Set values
	    io(id)->name(cfg.cfg("NAME").getS());
	    io(id)->type((IO::Type)cfg.cfg("TYPE").getI());
	    io(id)->mode((IO::Mode)cfg.cfg("MODE").getI());
	    io(id)->def(cfg.cfg("DEF").getS());
	    io(id)->vector(cfg.cfg("VECT").getS());
	    io(id)->hide(cfg.cfg("HIDE").getB());	
	}
    tbl.free();
    bd.close(io_bd);
}

void Func::save( )
{
    TBDS &bd = owner().owner().owner().owner().db();
    bd.open(owner().BD(),true).at().fieldSet(*this);
    bd.close(owner().BD());

    //Save io config
    saveIO();
}

void Func::saveIO( )
{
    TConfig cfg(&owner().owner().elFncIO());
    
    TBDS &bd = owner().owner().owner().owner().db();
    TBDS::SName io_bd = owner().BD();
    io_bd.tbl += "_io";    

    AutoHD<TTable> tbl = bd.open(io_bd,true);    
    if( tbl.freeStat() ) return;    
    //Save allow IO
    cfg.cfg("F_ID").setS(id());    
    for( int i_io = 0; i_io < ioSize(); i_io++ )
    {
	cfg.cfg("ID").setS(io(i_io)->id());
	cfg.cfg("NAME").setS(io(i_io)->name());
	cfg.cfg("TYPE").setI(io(i_io)->type());
	cfg.cfg("MODE").setI(io(i_io)->mode());
	cfg.cfg("DEF").setS(io(i_io)->def());
	cfg.cfg("VECT").setS(io(i_io)->vector());
	cfg.cfg("HIDE").setB(io(i_io)->hide());
	cfg.cfg("POS").setI(i_io);
	tbl.at().fieldSet(cfg);
    }    
    //Clear IO    
    int fld_cnt=0;
    while( tbl.at().fieldSeek(fld_cnt++,cfg) )
	if( cfg.cfg("F_ID").getS() == id() && ioId(cfg.cfg("ID").getS()) < 0 )
	{ tbl.at().fieldDel(cfg); fld_cnt--; }
    
    tbl.free();
    bd.close(io_bd);
}

void Func::del( )
{
    TBDS &bd = owner().owner().owner().owner().db();
    bd.open(owner().BD()).at().fieldDel(*this);
    bd.close(owner().BD());
	    
    //Delete io from DB
    delIO();
}

void Func::delIO( )
{
    TBDS &bd = owner().owner().owner().owner().db();
    TConfig cfg(&owner().owner().elFncIO());
    int fld_cnt=0;
    TBDS::SName io_bd = owner().BD();
    io_bd.tbl += "_io";
    AutoHD<TTable> tbl = bd.open(io_bd);
    
    while( tbl.at().fieldSeek(fld_cnt++,cfg) )
	if( cfg.cfg("F_ID").getS() == id() )
    	{ tbl.at().fieldDel(cfg); fld_cnt--; }
	
    tbl.free();
    bd.close(io_bd);
}

void Func::start( bool val )
{
    if( run_st == val )	return;
    
    if( val )
    {	
	try{ parseProg( ); }
	catch(TError err)
	{ Mess->put_s("FREE_FUNC",TMess::Error,err.what()); }
    }
    else
    {
	la_pos = 0;
	prg = "";
        symbClean();
	run_st = false;
    }
}

void Func::parseProg()
{
    ResAlloc res(parse_res,true);
    
    p_fnc  = this;	//Parse func
    p_err  = "";	//Clear error messages
    la_pos = 0;   	//LA position
    run_st = false;	//Stop function
    prg = "";		//Clear programm
    symbClean();	//Clear symbol
    
    if( yyparse( ) ) 
    {
	prg = "";
	symbClean();
	throw TError(p_err);
    }
    run_st = true;
    //List symbol table
    //for( int i_smb = 0; i_smb < symbSize(); i_smb++ )
    //    printf("Symbol %s, type %d\n",symbAt(i_smb)->name().c_str(), symbAt(i_smb)->type());
}

int Func::symbGet( const char *nm )
{
    //Check avoid symbols
    for( int i_smb = 0; i_smb < m_smb.size(); i_smb++ )
	if( m_smb[i_smb]->name() == nm )
	    return i_smb;
    
    //Make new symbol
    m_smb.push_back(new Symbol(nm,this));
    return m_smb.size()-1;
}
	
void Func::symbClean()
{
    for( int i_smb = 0; i_smb < m_smb.size(); i_smb++ )
	delete m_smb[i_smb];
    m_smb.clear();
}

float Func::getValR( TValFunc *io, StkEl stkel )
{
    if( stkel.type == StkEl::Const ) return stkel.var.v_r;
    else if( stkel.var.symb->type() == Symbol::Var )
    {
	Symbol *symb = stkel.var.symb;
	if( symb->val().var->io_id < 0 ) return symb->val().var->val;
	else return io->getR( symb->val().var->io_id );
    }
    return 0;
}
void  Func::setValR( TValFunc *io, StkEl stkel, float val )
{
    if( stkel.type == StkEl::Const ) stkel.var.v_r = val;
    else if( stkel.var.symb->type() == Symbol::Var )
    {
	Symbol *symb = stkel.var.symb;
	if( symb->val().var->io_id < 0 ) symb->val().var->val = val;
	else io->setR( symb->val().var->io_id, val );
    }
}

void Func::calc( TValFunc *val )
{
    vector<StkEl> stk;
    const BYTE *cprg = (const BYTE *)prg.c_str();
    exec(val,stk,cprg,cprg);
}

void Func::exec( TValFunc *val, vector<StkEl> &stk, const BYTE *stprg, const BYTE *cprg )
{
    float o;
    
    while( *cprg != StkEl::End )
    {			
	//Variable
	if( *cprg == StkEl::PushV )
	{
	    stk.push_back( symbAt(*(WORD *)(cprg+1)) );
	    cprg+=sizeof(WORD) ;
	}
	//Constant
	else if( *cprg == StkEl::PushR )
	{
	    stk.push_back( *(float *)(cprg+1) );
	    cprg+=sizeof(float);
	}	
	//Operations
	else if( *cprg == StkEl::OpAdd )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) + o;
	}	
	else if( *cprg == StkEl::OpSub )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) - o;
	}
	else if( *cprg == StkEl::OpDiv )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) / o;
	}	
	else if( *cprg == StkEl::OpMul )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) * o;
	}
	else if( *cprg == StkEl::OpPow )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=pow(getValR(val,stk.back()),o);
	}	
	else if( *cprg == StkEl::OpOR )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) || o;
	}	
	else if( *cprg == StkEl::OpAND )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) && o;
	}	
	else if( *cprg == StkEl::OpGT )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) > o;
	}	
	else if( *cprg == StkEl::OpLT )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) < o;
	}	
	else if( *cprg == StkEl::OpGE )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) >= o;
	}	
	else if( *cprg == StkEl::OpLE )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) <= o;		
	}	
	else if( *cprg == StkEl::OpEQ )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) == o;
	}	
	else if( *cprg == StkEl::OpNE )
	{
	    o = getValR(val,stk.back()); stk.pop_back();
	    stk.back()=getValR(val,stk.back()) != o;
	}	
	else if( *cprg == StkEl::OpNot )
	    stk.back()= !getValR(val,stk.back());
	//Negative
	else if( *cprg == StkEl::OpNeg )
	    stk.back() = -getValR(val,stk.back());
	//Assign
	else if( *cprg == StkEl::Assign )
	{
	    o = getValR(val,stk.back());
	    stk.back() = symbAt(*(WORD *)(cprg+1));
	    setValR(val,stk.back(),o);
	    cprg+=sizeof(WORD);
	}
	else if( *cprg == StkEl::AddEq )
	{
	    StkEl var(symbAt(*(WORD *)(cprg+1)));	    
	    setValR(val,var,getValR(val,var)+getValR(val,stk.back()));
	    stk.pop_back();
	    cprg+=sizeof(WORD);
	}
	else if( *cprg == StkEl::SubEq )
        {
            StkEl var(symbAt(*(WORD *)(cprg+1)));
            setValR(val,var,getValR(val,var)-getValR(val,stk.back()));
            stk.pop_back();
	    cprg+=sizeof(WORD);
        }
	else if( *cprg == StkEl::MulEq )
        {
            StkEl var(symbAt(*(WORD *)(cprg+1)));
            setValR(val,var,getValR(val,var)*getValR(val,stk.back()));
            stk.pop_back();
	    cprg+=sizeof(WORD);
        }
	else if( *cprg == StkEl::DivEq )
        {
            StkEl var(symbAt(*(WORD *)(cprg+1)));
            setValR(val,var,getValR(val,var)/getValR(val,stk.back()));
            stk.pop_back();
	    cprg+=sizeof(WORD);
        }
	//IF construction
	else if( *cprg == StkEl::If )
	{
	    const BYTE *c_cond = cprg+7;
	    const BYTE *c_then = stprg + *(WORD *)(cprg+1);
	    const BYTE *c_else = stprg + *(WORD *)(cprg+3);
	    cprg = stprg + *(WORD *)(cprg+5);
	    //Calc condition
	    exec(val,stk,stprg,c_cond);
	    //Check condition
	    o = getValR(val,stk.back());
	    stk.pop_back();
	    if(o) 
		exec(val,stk,stprg,c_then);
	    else if( c_then != c_else )
		exec(val,stk,stprg,c_else);
	    continue;
	}
	//IF expresion construction
	else if( *cprg == StkEl::IfExpr )
	{
	    const BYTE *c_then = cprg+5;
	    const BYTE *c_else = stprg + *(WORD *)(cprg+1);
	    cprg = stprg + *(WORD *)(cprg+3);
	    //Check condition
	    o = getValR(val,stk.back());
	    stk.pop_back();
	    if(o)	exec(val,stk,stprg,c_then);
	    else 	exec(val,stk,stprg,c_else);
	    continue;
	}
	//Call functions and procedures
	else if( *cprg == StkEl::Proc || *cprg == StkEl::Func )
        {
	    TValFunc &vfnc = symbAt(*(WORD *)(cprg+1))->val().fnc->fval;
	    int prm = *(BYTE *)(cprg+3);
	    
	    //Get return position
	    int r_pos;
	    for( r_pos = 0; r_pos < vfnc.func()->ioSize(); r_pos++ )
		if( vfnc.ioMode(r_pos) == IO::Return ) break;		
	    //Process parameters
	    vector<StkEl>::iterator p_it = stk.end()-1;
	    for( int i_p = prm-1; i_p >= 0; i_p--, p_it-- )
	    {
		int p_p = (i_p>=r_pos)?i_p+1:i_p;
		vfnc.setR(p_p,getValR(val,*p_it));
	    }
	    //Make calc
            vfnc.calc();
	    //Process outputs
	    for( int i_p = prm-1; i_p >= 0; i_p-- )
            {
                int p_p = (i_p>=r_pos)?i_p+1:i_p;
                if( vfnc.ioMode(p_p) == IO::Output )
                    setValR(val,stk.back(),vfnc.getR(p_p));
		stk.pop_back();
	    }	    	
	    //Push result
	    if( *cprg == StkEl::Func )
                stk.push_back(vfnc.getR(r_pos));
	    cprg+=sizeof(WORD)+sizeof(BYTE);
    	}
	//Pop from stack
	else if( *cprg == StkEl::PopEl ) stk.pop_back();
	else
	{ 
	    start(false);
	    throw TError("Operation %c(%xh) error. Function <%s> stoped.",*cprg,*cprg,id().c_str());	    
	}
	cprg++;
    }
    //if( stk.size() )
    //	Mess->put("FREE_FUNC",TMess::Warning,"Function %s. Stack not free past calc.",name().c_str());
}

NConst *Func::constGet( const char *nm )
{ 
    for( int i_cst = 0; i_cst < m_const.size(); i_cst++)
	if( m_const[i_cst].name == nm ) return &m_const[i_cst];
    return NULL; 
}

void Func::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TFunction::cntrCmd_( a_path, opt, cmd );       //Call parent
	
        ctrMkNode("fld",opt,a_path.c_str(),"/func/cfg/name",Mess->I18N("Name"),0664,0,0,"str");
        ctrMkNode("fld",opt,a_path.c_str(),"/func/cfg/descr",Mess->I18N("Description"),0664,0,0,"str")->
	    attr_("cols","90")->attr_("rows","3");
	ctrMkNode("comm",opt,a_path.c_str(),"/func/cfg/load",Mess->I18N("Load from BD"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/func/cfg/save",Mess->I18N("Save to BD"),0550);
	ctrMkNode("table",opt,a_path.c_str(),"/io/io",Mess->I18N("IO"),0664,0,0)->attr_("s_com","add,del,ins,move");
	ctrMkNode("list",opt,a_path.c_str(),"/io/io/0",Mess->I18N("Id"),0664,0,0,"str");
	ctrMkNode("list",opt,a_path.c_str(),"/io/io/1",Mess->I18N("Name"),0664,0,0,"str");	
	ctrMkNode("list",opt,a_path.c_str(),"/io/io/2",Mess->I18N("Type"),0664,0,0,"str")->
    	    attr_("idm","1")->attr_("dest","select")->attr_("select","/io/tp");
	ctrMkNode("list",opt,a_path.c_str(),"/io/io/3",Mess->I18N("Mode"),0664,0,0,"str")->
	    attr_("idm","1")->attr_("dest","select")->attr_("select","/io/md");	    
	ctrMkNode("list",opt,a_path.c_str(),"/io/io/4",Mess->I18N("Hide"),0664,0,0,"bool");
        ctrMkNode("list",opt,a_path.c_str(),"/io/io/5",Mess->I18N("Default"),0664,0,0,"str");
    	ctrMkNode("list",opt,a_path.c_str(),"/io/io/6",Mess->I18N("Vector"),0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/io/prog",Mess->I18N("Programm"),0664,0,0,"str")->
	    attr_("cols","90")->attr_("rows","10");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/io/io" )
	{
	    XMLNode *n_id       = ctrId(opt,"0");
	    XMLNode *n_nm       = ctrId(opt,"1");
	    XMLNode *n_type     = ctrId(opt,"2");
	    XMLNode *n_mode     = ctrId(opt,"3");
	    XMLNode *n_hide     = ctrId(opt,"4");
	    XMLNode *n_def      = ctrId(opt,"5");
    	    XMLNode *n_vect     = ctrId(opt,"6");
	    for( int id = 0; id < ioSize(); id++ )		
	    {		
		ctrSetS(n_id,io(id)->id());
		ctrSetS(n_nm,io(id)->name());
		//Make type
		if( io(id)->type() == IO::Integer )    	ctrSetS(n_type,"int");
		else if( io(id)->type() == IO::Real )	ctrSetS(n_type,"real");
		else if( io(id)->type() == IO::Boolean )ctrSetS(n_type,"bool");
		//Make mode
		if( io(id)->mode() == IO::Output )    	ctrSetS(n_mode,"out");
		else if( io(id)->mode() == IO::Return )	ctrSetS(n_mode,"ret");
		else if( io(id)->mode() == IO::Input )	ctrSetS(n_mode,"in");

		(io(id)->hide()) ? ctrSetB(n_hide,true) : ctrSetB(n_hide,false);

		ctrSetS(n_def,io(id)->def());
		ctrSetS(n_vect,io(id)->vector());
	    }	
	}	
	else if( a_path == "/io/tp" )
	{
	    ctrSetS( opt, Mess->I18N("Real"), "real" );
	    ctrSetS( opt, Mess->I18N("Integer"), "int" );
	    ctrSetS( opt, Mess->I18N("Boolean"), "bool" );
	}
	else if( a_path == "/io/md" )
	{
	    ctrSetS( opt, Mess->I18N("Input"), "in" );
	    ctrSetS( opt, Mess->I18N("Output"), "out" );
	    ctrSetS( opt, Mess->I18N("Return"), "ret" );
	}
	else if( a_path == "/io/prog" )	ctrSetS( opt, prg_src );
	else TFunction::cntrCmd_( a_path, opt, cmd );       //Call parent
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/func/cfg/name" )	m_name 	= ctrGetS(opt);
	else if( a_path == "/func/cfg/descr" )	m_descr = ctrGetS(opt);
	else if( a_path == "/io/io" )
	{
	    if( opt->name() == "add" )		ioAdd( new IO("new","New IO",IO::Real,IO::Input) );
	    else if( opt->name() == "ins" )	ioIns( new IO("new","New IO",IO::Real,IO::Input), atoi(opt->attr("row").c_str()) );
	    else if( opt->name() == "del" )	ioDel( atoi(opt->attr("row").c_str()) );
	    else if( opt->name() == "move" )	ioMove( atoi(opt->attr("row").c_str()), atoi(opt->attr("to").c_str()) );	    
	    else if( opt->name() == "set" )	
	    {
		int row = atoi(opt->attr("row").c_str());
		int col = atoi(opt->attr("col").c_str());
		if( (col == 0 || col == 1) && !opt->text().size() )
		    throw TError("Empty value no valid.");		    
		if( col == 0 )		io(row)->id(ctrGetS(opt));
		else if( col == 1 )	io(row)->name(ctrGetS(opt));
		else if( col == 2 )	
		{
		    if( ctrGetS(opt) == "real" )	io(row)->type(IO::Real);
		    else if( ctrGetS(opt) == "int" )	io(row)->type(IO::Integer);
		    else if( ctrGetS(opt) == "bool" )	io(row)->type(IO::Boolean);
		}
		else if( col == 3 )	
		{
		    if( ctrGetS(opt) == "in" )		io(row)->mode(IO::Input);
		    else if( ctrGetS(opt) == "out" )	io(row)->mode(IO::Output);
		    else if( ctrGetS(opt) == "ret" )	io(row)->mode(IO::Return);
		}
		else if( col == 4 )    	io(row)->hide(ctrGetB(opt));
		else if( col == 5 )    	io(row)->def(ctrGetS(opt));
		else if( col == 6 )    	io(row)->vector(ctrGetS(opt));
	    }
	}	
	else if( a_path == "/io/prog" )
	{
	    prg_src = ctrGetS( opt );
	    parseProg();
	}
	else if( a_path == "/func/cfg/load" )	load();
	else if( a_path == "/func/cfg/save" )	save();
	else TFunction::cntrCmd_( a_path, opt, cmd );       //Call parent
    }
}

//================== Symbol ========================
Symbol::Symbol( const char *name, FreeFunc::Func *afnc ) : m_name(name), m_tp(Undef), fnc(afnc)
{
    type( Check );
}
	
Symbol::~Symbol( )
{
    type( Undef );
}
	
void Symbol::type( Type tp )
{
    if( tp == m_tp )	return;
    //Free old type
    if( m_tp == Func )
    {
	m_val.fnc->fval.func(NULL);
	m_val.fnc->func.free();
	delete m_val.fnc;
    }
    else if( m_tp == Var )	
	delete m_val.var;
    m_tp = Undef;
    
    //Set new type
    if( tp == Var || tp == Check )
    {
	m_val.var = new SVar;
	//Set internal
	m_val.var->io_id = -1;
	m_val.var->val   = 0;	
	//Check IO	
	for( int i_io = 0; i_io < fnc->ioSize(); i_io++ )
    	    if( fnc->io(i_io)->id() == name() ) 
	    { 
		m_val.var->io_id = i_io;		
		tp = Var; 
		break;
	    }
	if( tp == Check ) delete m_val.var;
    }
    if( tp == Func || tp == Check )	
    {
	string lib_n = fnc->owner().id();
	string fnc_n = name();
        if( SYS->strSepParse(name(),1,'.').size() )
	{
	    lib_n = SYS->strSepParse(name(),0,'.');
	    fnc_n = SYS->strSepParse(name(),1,'.');
	}
	if( fnc->owner().owner().owner().owner().func().avoid(lib_n) && 
		fnc->owner().owner().owner().owner().func().at(lib_n).at().avoid(fnc_n) )
	{
	    m_val.fnc = new SFunc;
	    m_val.fnc->func = fnc->owner().owner().owner().owner().func().at(lib_n).at().at(fnc_n);
	    m_val.fnc->fval.func(&m_val.fnc->func.at());
	    tp = Func;
	}
    }
    if( tp == Check ) tp = Undef;

    m_tp = tp;
}

