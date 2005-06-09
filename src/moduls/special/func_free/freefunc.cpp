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
#include <tkernel.h>
#include <tmessage.h>
#include "freefunc.h"

//============ Modul info! =====================================================
#define MOD_ID      "freefunc"
#define MOD_NAME    "Free functions"
#define MOD_TYPE    "Special"
#define VER_TYPE    VER_SPC
#define SUB_TYPE    "LIB"
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow free function libraries. Based at Complex2 free formules engine."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
    	else
	    AtMod.id	= "";

	return( AtMod );
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	FreeFunc::Libs *self_addr = NULL;

    	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new FreeFunc::Libs( source );

	return ( self_addr );
    }
}

using namespace FreeFunc;

//===================== Complex1 functions library =========================
Libs::Libs( string src ) : m_bd("","","vLibFunc")
{
    mId 	= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= src;
}

Libs::~Libs()
{
    stop();
}

void Libs::start( )
{
    if( !run_st )
    {
	//Load BD
        loadBD();    
	run_st = true;
    }
}

void Libs::stop( )
{
    if( run_st )
    {
	while( free_libs.size() )
	{
            owner().owner().func().unreg(free_libs[0]);
            free_libs.erase(free_libs.begin());
        }	
	run_st = false;
    }
}

void Libs::modConnect( )
{
    TModule::modConnect( );

    //Lib's db structure
    lb_el.fldAdd( new TFld("ID",I18N("ID"),T_STRING|F_KEY,"10") );
    lb_el.fldAdd( new TFld("NAME",I18N("Name"),T_STRING,"20") );
    lb_el.fldAdd( new TFld("DESCR",I18N("Description"),T_STRING,"50") );
    lb_el.fldAdd( new TFld("BD_TP",I18N("Data base type"),T_STRING,"20") );
    lb_el.fldAdd( new TFld("BD_NM",I18N("Data base name"),T_STRING,"20") );
    lb_el.fldAdd( new TFld("BD_TBL",I18N("Data base table"),T_STRING,"20") );

    //Function's structure
    fnc_el.fldAdd( new TFld("ID",I18N("ID"),T_STRING|F_KEY,"10") );
    fnc_el.fldAdd( new TFld("NAME",I18N("Name"),T_STRING,"20") );
    fnc_el.fldAdd( new TFld("DESCR",I18N("Description"),T_STRING,"50") );
    fnc_el.fldAdd( new TFld("FORMULA",I18N("Formula"),T_STRING,"255") );

    //Function's structure
    fncio_el.fldAdd( new TFld("F_ID",I18N("Function ID"),T_STRING|F_KEY,"10") );
    fncio_el.fldAdd( new TFld("ID",I18N("ID"),T_STRING|F_KEY,"10") );
    fncio_el.fldAdd( new TFld("NAME",I18N("Name"),T_STRING,"20") );
    fncio_el.fldAdd( new TFld("TYPE",I18N("Type"),T_DEC,"1") );
    fncio_el.fldAdd( new TFld("MODE",I18N("Mode"),T_DEC,"1") );
    fncio_el.fldAdd( new TFld("DEF",I18N("Default value"),T_STRING,"20") );
    fncio_el.fldAdd( new TFld("VECT",I18N("Vector"),T_STRING,"10") );
    fncio_el.fldAdd( new TFld("HIDE",I18N("Hide"),T_BOOL,"1") );
    fncio_el.fldAdd( new TFld("POS",I18N("Position"),T_DEC,"3") );
}

TBDS::SName Libs::BD()
{
    return owner().owner().nameDBPrep(m_bd);
}

void Libs::loadBD()
{
    try
    {
	TConfig c_el(&elLib());
	AutoHD<TTable> tbl = owner().owner().BD().open(BD());
	int fld_cnt = 0;
	while( owner().owner().BD().dataSeek(tbl,cfgNodeName()+"lib/", fld_cnt++,c_el) )
        {
	    string l_id = c_el.cfg("ID").getS();
	    
	    int f_lb;
	    for( f_lb = 0; f_lb < free_libs.size(); f_lb++ )		
		if( free_libs[f_lb] == l_id )	break;		
	    if( f_lb >= free_libs.size() )
	    {
		Lib *lb = new Lib(l_id.c_str(),this);
		//*(TConfig *)lb = c_el;
    		owner().owner().func().reg(lb);
		free_libs.push_back(l_id);
	    }
	    ((Lib &)owner().owner().func().at(l_id).at()).load();
	}
	if(!tbl.freeStat())
	{
	    tbl.free();
	    owner().owner().BD().close(BD());
	}	
    }catch( TError err ){ mPutS("SYS",MESS_ERR,err.what()); }
}

void Libs::saveBD()
{   
    for( int l_id = 0; l_id < free_libs.size(); l_id++ )
	((Lib &)owner().owner().func().at(free_libs[l_id]).at()).save();
}  

void Libs::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TSpecial::cntrCmd_( a_path, opt, cmd );       //Call parent
	
	if( run_st )
	{    
	    ctrInsNode("area",1,opt,a_path.c_str(),"/mod",I18N(MOD_NAME));
	    ctrMkNode("area",opt,a_path.c_str(),"/mod/libs",I18N("Libraries"));
	    ctrMkNode("list",opt,a_path.c_str(),"/mod/libs/lb",I18N("Libraries"),0664,0,0,"br")->
		attr_("idm","1")->attr_("s_com","add,del")->attr_("mode","att")->attr_("br_pref","_lb_");
	    ctrMkNode("comm",opt,a_path.c_str(),"/mod/load",Mess->I18N("Load from BD"),0550);
            ctrMkNode("comm",opt,a_path.c_str(),"/mod/save",Mess->I18N("Save to BD"),0550);		    	
	}
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/mod/libs/lb" )
	{
	    opt->childClean();
	    for( unsigned i_a=0; i_a < free_libs.size(); i_a++ )
		ctrSetS( opt, owner().owner().func().at(free_libs[i_a]).at().name(), free_libs[i_a].c_str() );
	}
	else TSpecial::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/mod/libs/lb" )
	{
	    if( opt->name() == "add" )
	    {
		owner().owner().func().reg(new Lib(opt->attr("id").c_str(),this,opt->text().c_str()));
		free_libs.push_back(opt->attr("id"));
	    }
	    else if( opt->name() == "del" )
	    {
		owner().owner().func().unreg(opt->attr("id"));
		for(int i_el = 0; i_el < free_libs.size(); i_el++)
		    if( free_libs[i_el] == opt->attr("id") )
		    {
			free_libs.erase(free_libs.begin()+i_el);
			break;
		    }
	    }
	}
	else if( a_path == "/mod/load" )	loadBD();
	else if( a_path == "/mod/save" )	saveBD();
	else TSpecial::cntrCmd_( a_path, opt, cmd );
    }
}

AutoHD<TCntrNode> Libs::ctrAt1( const string &a_path )
{
    if( a_path.substr(0,4) == "_lb_" )	return owner().owner().func().at(pathEncode(a_path.substr(4),true));
    else return TSpecial::ctrAt1(a_path);
}


//================ Complex2 functions library ==================
Lib::Lib( const char *id, Libs *own, const char *name ) : 
    TConfig(&own->elLib()), TLibFunc(id), m_owner(own), 
    m_name(cfg("NAME").getS()), m_descr(cfg("DESCR").getS()), 
    m_bd_tp(cfg("BD_TP").getS()), m_bd_nm(cfg("BD_NM").getS()), m_bd_tbl(cfg("BD_TBL").getS())
{
    cfg("ID").setS(id);
    m_name = name;
}

Lib::~Lib()
{
    
}

void Lib::load( )
{
    TBDS &bd = owner().owner().owner().BD();
    bd.open(owner().BD()).at().fieldGet(*this);
    bd.close(owner().BD());

    //Load functions
    TConfig c_el(&owner().elFnc());
    AutoHD<TTable> tbl = bd.open(BD());

    int fld_cnt = 0;
    while( bd.dataSeek(tbl,owner().cfgNodeName()+id()+"_fnc/", fld_cnt++,c_el) )
    {
	string f_id = c_el.cfg("ID").getS();
        
	if( !avoid(f_id) )
        {
	    Func *n_fnc = new Func(f_id.c_str(),this);
	    //*(TConfig *)n_fnc = c_el;
	    reg(n_fnc);
	}
        ((Func &)at(f_id).at()).load();
    }
    if(!tbl.freeStat())
    {
	tbl.free();
	bd.close(BD());
    }    
}

void Lib::save( )
{    
    TBDS &bd = owner().owner().owner().BD();
    bd.open(owner().BD(),true).at().fieldSet(*this);
    bd.close(owner().BD());

    //Save functions
    vector<string> f_lst;
    list(f_lst);
    for( int i_ls = 0; i_ls < f_lst.size(); i_ls++ )
	((Func &)at(f_lst[i_ls]).at()).save();    
}

TBDS::SName Lib::BD()
{
    return owner().owner().owner().nameDBPrep(TBDS::SName(m_bd_tp.c_str(),m_bd_nm.c_str(),m_bd_tbl.c_str()));
}

Libs &Lib::owner()
{
    return *m_owner;
}

void Lib::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    vector<string> list;
    
    if( cmd==TCntrNode::Info )
    {
	TLibFunc::cntrCmd_( a_path, opt, cmd );       //Call parent
	
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/name","Name",0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/descr","Description",0664,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/bd_tp",Mess->I18N("Library BD (module:bd:table)"),0660,0,0,"str")->
    	    attr_("idm","1")->attr_("dest","select")->attr_("select","/lib/b_mod");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/bd_nm","",0660,0,0,"str");
	ctrMkNode("fld",opt,a_path.c_str(),"/lib/bd_tbl","",0660,0,0,"str");
	ctrMkNode("comm",opt,a_path.c_str(),"/lib/load",Mess->I18N("Load from BD"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/lib/save",Mess->I18N("Save to BD"),0550);
	ctrMkNode("list",opt,a_path.c_str(),"/func/func","Functions",0664,0,0,"br")->
	    attr_("idm","1")->attr_("s_com","add,del")->attr_("mode","att")->attr_("br_pref","_");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/lib/bd_tp" )		ctrSetS(opt,m_bd_tp);
	else if( a_path == "/lib/bd_nm" )	ctrSetS(opt,m_bd_nm);
	else if( a_path == "/lib/bd_tbl" )	ctrSetS(opt,m_bd_tbl);
	else if( a_path == "/lib/b_mod" )
	{
	    opt->childClean();
	    owner().owner().owner().BD().gmdList(list);
	    ctrSetS( opt, "" ); //Default DB
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
    		ctrSetS( opt, list[i_a] );
	}
	else TLibFunc::cntrCmd_( a_path, opt, cmd );       //Call parent
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/lib/name" )		m_name 	= ctrGetS(opt);
	else if( a_path == "/lib/descr" )	m_descr = ctrGetS(opt);
	else if( a_path == "/lib/bd_tp" )	m_bd_tp	= ctrGetS(opt);
	else if( a_path == "/lib/bd_nm" )	m_bd_nm	= ctrGetS(opt);
	else if( a_path == "/lib/bd_tbl" )	m_bd_tbl= ctrGetS(opt);
	else if( a_path == "/func/func" )
	{
	    if( opt->name() == "add" )		reg( new Func(opt->attr("id").c_str(),this,opt->text().c_str()) );
	    else if( opt->name() == "del" )	unreg( opt->attr("id") );
	}
	else if( a_path == "/lib/load" )	load();
	else if( a_path == "/lib/save" )	save();
	else TLibFunc::cntrCmd_( a_path, opt, cmd );       //Call parent
    }
}
    																									    
//================== Complex2 function ========================
Func::Func( const char *id, Lib *own, const char *name ) : 
    TConfig(&own->owner().elFnc()), TFunction(id), m_owner(own),
    m_name(cfg("NAME").getS()), m_descr(cfg("DESCR").getS()), m_formula(cfg("FORMULA").getS()) 
{
    cfg("ID").setS(id);    
    m_name = name;
}

Func::~Func( )
{

}
	
Lib &Func::owner()
{
    return *m_owner;
}

void Func::load( )
{
    TBDS &bd = owner().owner().owner().owner().BD();
    bd.open(owner().BD()).at().fieldGet(*this);
    bd.close(owner().BD());
    
    loadIO( );
}

void Func::loadIO( )
{
    TConfig cfg(&owner().owner().elFncIO());
    
    TBDS &bd = owner().owner().owner().owner().BD();
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
    TBDS &bd = owner().owner().owner().owner().BD();
    bd.open(owner().BD(),true).at().fieldSet(*this);
    bd.close(owner().BD());

    //Save io config
    saveIO();
}

void Func::saveIO( )
{
    TConfig cfg(&owner().owner().elFncIO());
    
    TBDS &bd = owner().owner().owner().owner().BD();
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
	    tbl.at().fieldDel(cfg);
    
    tbl.free();
    bd.close(io_bd);
}

void Func::calc( TValFunc *val )
{

}

void Func::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {
	TFunction::cntrCmd_( a_path, opt, cmd );       //Call parent
	
        ctrMkNode("fld",opt,a_path.c_str(),"/func/name",Mess->I18N("Name"),0664,0,0,"str");
        ctrMkNode("fld",opt,a_path.c_str(),"/func/descr",Mess->I18N("Description"),0664,0,0,"str")->
	    attr_("cols","90")->attr_("rows","3");
	ctrMkNode("comm",opt,a_path.c_str(),"/func/load",Mess->I18N("Load from BD"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/func/save",Mess->I18N("Save to BD"),0550);
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
	ctrMkNode("fld",opt,a_path.c_str(),"/io/form",Mess->I18N("Formula"),0664,0,0,"str")->
	    attr_("cols","90")->attr_("rows","5");
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
	else if( a_path == "/io/form" )	ctrSetS( opt, m_formula );
	else TFunction::cntrCmd_( a_path, opt, cmd );       //Call parent
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/func/name" )		m_name 	= ctrGetS(opt);
	else if( a_path == "/func/descr" )	m_descr = ctrGetS(opt);
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
	else if( a_path == "/io/form" )		m_formula = ctrGetS( opt );
	else if( a_path == "/func/load" )	load();
	else if( a_path == "/func/save" )	save();
	else TFunction::cntrCmd_( a_path, opt, cmd );       //Call parent
    }
}

