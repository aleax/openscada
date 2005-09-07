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
#include <resalloc.h>
#include <tmess.h>
#include <tparams.h>
#include <math.h>

#include "virtual.h"
#include "block.h"

using namespace Virtual;

//Function block
Block::Block( const string &iid, Contr *iown ) : 
    TCntrNode(iown), TConfig( &((TipContr &)iown->owner()).blockE() ), TValFunc(iid+"_block",NULL), 
    m_process(false), m_sw_mode(0), m_sw_hide(false),
    m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), m_descr(cfg("DESCR").getSd()),
    m_lib(cfg("FUNC_LIB").getSd()), m_func(cfg("FUNC_ID").getSd()),
    m_to_en(cfg("EN").getBd()), m_to_prc(cfg("PROC").getBd())
{
    m_id = iid;
    hd_res = ResAlloc::resCreate();
}

Block::~Block( )
{
    if( enable() ) enable(false);
    ResAlloc::resDelete(hd_res);
}

void Block::postDisable(int flag)
{
    try
    {
        if( flag )
        {
	    AutoHD<TBDS> bds = owner().owner().owner().owner().db();
	    
	    //Delete block from BD
            TBDS::SName tbl = owner().BD();
            tbl.tbl = owner().cfg("BLOCK_SH").getS();			    
            bds.at().open(tbl).at().fieldDel(*this);
            bds.at().close(tbl);
	    
	    //Delete block's IO from BD
	    TConfig cfg(&((TipContr &)owner().owner()).blockIOE());
	    tbl.tbl = owner().cfg("BLOCK_SH").getS()+"_io";
	    AutoHD<TTable> dbt = bds.at().open(tbl);
	    int fld_cnt = 0;
	    while( dbt.at().fieldSeek(fld_cnt++,cfg) )
		if( id() == cfg.cfg("BLK_ID").getS() )
		{
		    dbt.at().fieldDel(cfg);
		    fld_cnt--;
		}
        }	
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}

void Block::load(  )
{    
    AutoHD<TBDS> bd = owner().owner().owner().owner().db();
    TBDS::SName tbl = owner().BD();
    tbl.tbl = owner().cfg("BLOCK_SH").getS();
    
    bd.at().open(tbl).at().fieldGet(*this);
    bd.at().close(tbl);
    
    //Load io config
    if( enable() )
    {
	for( int i_ln = 0; i_ln < m_lnk.size(); i_ln++ )
    	    if( m_lnk[i_ln].tp != 0 ) 
	    try{ loadIO(i_ln); } 
	    catch(TError err){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    }
}

void Block::save( )
{
    AutoHD<TBDS> bd = owner().owner().owner().owner().db();
    TBDS::SName tbl = owner().BD();
    tbl.tbl = owner().cfg("BLOCK_SH").getS();
    
    bd.at().open(tbl,true).at().fieldSet(*this);
    bd.at().close(tbl);
    
    //Save io config
    if( enable() )
    {
        for( int i_ln = 0; i_ln < m_lnk.size(); i_ln++ )
            if( m_lnk[i_ln].tp != 0 ) 
		try{ saveIO(i_ln); } 
		catch(TError err){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    }
}
		
void Block::loadIO( unsigned i_ln )
{    
    if( !enable() || m_lnk[i_ln].tp == 0 )	return;
    
    TConfig cfg(&((TipContr &)owner().owner()).blockIOE());
    cfg.cfg("BLK_ID").setS(id());
    cfg.cfg("ID").setS(func()->io(i_ln)->id());
    
    AutoHD<TBDS> bd = owner().owner().owner().owner().db();
    TBDS::SName tbl = owner().BD();
    tbl.tbl = owner().cfg("BLOCK_SH").getS()+"_io";
    
    bd.at().open(tbl).at().fieldGet(cfg);
    bd.at().close(tbl);

    //Link type
    LnkT tp = (LnkT)cfg.cfg("TLNK").getI();
    //link( i_ln, (LnkT)cfg.cfg("TLNK").getI() );
    
    //Value
    string val = cfg.cfg("VAL").getS();
    if(ioType(i_ln) == IO::String )       setS(i_ln,val);
    else if(ioType(i_ln) == IO::Integer )	setI(i_ln,atoi(val.c_str()));
    else if(ioType(i_ln) == IO::Real ) 	setR(i_ln,atof(val.c_str()));
    else if(ioType(i_ln) == IO::Boolean ) setB(i_ln,atoi(val.c_str()));
    
    //Config of link
    link(i_ln,SET,tp,cfg.cfg("O1").getS(),cfg.cfg("O2").getS(),cfg.cfg("O3").getS());
}

void Block::saveIO( unsigned i_ln )
{
    if( !enable() || m_lnk[i_ln].tp == 0 )      return;
	
    TConfig cfg(&((TipContr &)owner().owner()).blockIOE());
    cfg.cfg("BLK_ID").setS(id());
    cfg.cfg("ID").setS(func()->io(i_ln)->id());
		    
    AutoHD<TBDS> bd = owner().owner().owner().owner().db();
    TBDS::SName tbl = owner().BD();
    tbl.tbl = owner().cfg("BLOCK_SH").getS()+"_io";

    //Link type
    cfg.cfg("TLNK").setI(m_lnk[i_ln].tp);
    
    //Value
    string val;
    if(ioType(i_ln) == IO::String )		val = getS(i_ln);
    else if(ioType(i_ln) == IO::Integer )	val = TSYS::int2str(getI(i_ln));
    else if(ioType(i_ln) == IO::Real )		val = TSYS::real2str(getR(i_ln));
    else if(ioType(i_ln) == IO::Boolean ) 	val = TSYS::int2str(getB(i_ln));
    cfg.cfg("VAL").setS(val);    
    
    //Config of link
    switch(m_lnk[i_ln].tp)
    {
	case I_LOC:
	    cfg.cfg("O1").setS(m_lnk[i_ln].iblk->blk);
	    cfg.cfg("O2").setS(m_lnk[i_ln].iblk->id);	    	    
            break;
        case I_GLB:
            cfg.cfg("O1").setS(m_lnk[i_ln].iblk->cnt);
            cfg.cfg("O2").setS(m_lnk[i_ln].iblk->blk);
	    cfg.cfg("O3").setS(m_lnk[i_ln].iblk->id);
            break;
	case I_PRM:
	case O_PRM:
	    cfg.cfg("O1").setS(m_lnk[i_ln].prm->prm);
            cfg.cfg("O2").setS(m_lnk[i_ln].prm->atr);
	    break;
    }
    bd.at().open(tbl,true).at().fieldSet(cfg);
    bd.at().close(tbl);	    
}

void Block::enable( bool val, bool dis_fnc )
{
    if( process() ) process(false);
    //Enable
    if( val && !func() )
    {
	func( &owner().owner().owner().owner().func().at().at(m_lib).at().at(m_func).at() );
	
	//Init links
	for( int i_ln = 0; i_ln < m_val.size(); i_ln++ )
	{
	    m_lnk.push_back( SLnk() );
	    if( m_val[i_ln].tp )
	    {
	    	m_lnk[i_ln].tp = FREE;
		//Load IO
		try{ loadIO(i_ln); }
        	catch(TError err){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }			    
	    }
	    else m_lnk[i_ln].tp = DIS;	    
	}
    }
    //Disable
    else if( !val && func() )
    {
	for( unsigned i_ln = 0; i_ln < m_lnk.size(); i_ln++ )
	    link(i_ln,SET,FREE);
	m_lnk.clear();
	
	//Free func
	if( dis_fnc ) func(NULL);
    }
}

void Block::process( bool val )
{
    if( val && !enable() ) enable(true);
    
    //Connect links
    if( val && !process() )
    {
        for( int i_ln = 0; i_ln < m_lnk.size(); i_ln++ )
	    link( i_ln, INIT );
	owner().blkProc( id(), val );
    }
    //Disconnect links
    if( !val && process() )
    {
	for( int i_ln = 0; i_ln < m_lnk.size(); i_ln++ )
	    link( i_ln, DEINIT );
	owner().blkProc( id(), val );
    }
    m_process = val;
}

Block::LnkT Block::link( unsigned iid )
{    
    if( iid >= m_lnk.size() || m_lnk[iid].tp == DIS )	
	throw TError(nodePath().c_str(),"Link %d error!",iid);
    return m_lnk[iid].tp;
}
                                   				   
void Block::link( unsigned iid, LnkCmd cmd, LnkT lnk, const string &o1, const string &o2, const string &o3 )
{
    string lo1, lo2, lo3;
    
    ResAlloc res(hd_res,true);
    if( iid >= m_lnk.size() || m_lnk[iid].tp == DIS )	
	throw TError(nodePath().c_str(),"Link %d error!",iid);

    //Change type link
    if( cmd == SET && lnk != m_lnk[iid].tp )
    {    
	//Free old structures
	switch(m_lnk[iid].tp)
	{
	    case I_LOC: case I_GLB:	delete m_lnk[iid].iblk;	break;
	    case I_PRM: case O_PRM:	delete m_lnk[iid].prm;	break;
	}
    
	//Make new structures
	switch(lnk)
	{
	    case I_LOC: case I_GLB:	m_lnk[iid].iblk = new SLIBlk;	break;
	    case I_PRM: case O_PRM:	m_lnk[iid].prm  = new SLPrm;	break;
	}	
	m_lnk[iid].tp = lnk;
    }
    //Connect new link and init
    if( cmd == INIT || cmd == SET )
	switch(m_lnk[iid].tp)
	{
	    case I_LOC:
		if( cmd == SET )
		{
		    m_lnk[iid].iblk->blk = o1;
		    m_lnk[iid].iblk->id  = o2;
		}
		if( cmd == INIT || process() )
		{
		    lo1 = m_lnk[iid].iblk->blk;
		    lo2 = m_lnk[iid].iblk->id;
		    if( owner().blkPresent(lo1) && owner().blkAt(lo1).at().ioId(lo2) >= 0 )
		    {
			m_lnk[iid].iblk->w_bl = owner().blkAt(lo1);
			m_lnk[iid].iblk->w_id = m_lnk[iid].iblk->w_bl.at().ioId(lo2);
		    }
		}
		break;
	    case I_GLB:
		if( cmd == SET )
		{
		    m_lnk[iid].iblk->cnt = o1;
		    m_lnk[iid].iblk->blk = o2;
		    m_lnk[iid].iblk->id  = o3;
		}
		if( cmd == INIT || process() )
		{
		    lo1 = m_lnk[iid].iblk->cnt;
		    lo2 = m_lnk[iid].iblk->blk;
		    lo3 = m_lnk[iid].iblk->id;
		    if( owner().owner().present(lo1) && 
			    ((Contr &)owner().owner().at(lo1).at()).blkPresent(lo2) && 
			    ((Contr &)owner().owner().at(lo1).at()).blkAt(lo2).at().ioId(lo3) >= 0 )
		    {
			m_lnk[iid].iblk->w_bl = ((Contr &)owner().owner().at(lo1).at()).blkAt(lo2);
			m_lnk[iid].iblk->w_id = m_lnk[iid].iblk->w_bl.at().ioId(lo3);
		    }
		}
		break;
	    case I_PRM: case O_PRM:
		if( cmd == SET )
		{
		    m_lnk[iid].prm->prm = o1;
		    m_lnk[iid].prm->atr = o2;
		}
		if( cmd == INIT || process() )
		{
		    AutoHD<TParamS> prms = owner().owner().owner().owner().param();
		    
		    lo1 = m_lnk[iid].prm->prm;
		    lo2 = m_lnk[iid].prm->atr;
		    if( prms.at().present(lo1) && prms.at().at(lo1).at().at().vlPresent(lo2) )
			m_lnk[iid].prm->w_prm = prms.at().at(lo1);
		}
		break;
	}
    //Disconnect
    if( cmd == DEINIT )
	switch(m_lnk[iid].tp)
	{
	    case I_LOC: case I_GLB:	m_lnk[iid].iblk->w_bl.free();	break;
	    case I_PRM: case O_PRM:	m_lnk[iid].prm->w_prm.free();	break;
	}
}

void Block::calc( )
{
    //Get values from input links
    ResAlloc::resRequestR(hd_res);
    for( unsigned i_ln=0; i_ln < m_lnk.size(); i_ln++ )
	switch( m_lnk[i_ln].tp )
	{
	    case I_LOC: case I_GLB:
	    {
		if( m_lnk[i_ln].iblk->w_bl.freeStat() )	continue;
		SLIBlk *lc = m_lnk[i_ln].iblk;
		switch(ioType(i_ln))
		{
		    case IO::String:	setS(i_ln,lc->w_bl.at().getS(lc->w_id));	break;
		    case IO::Integer:	setI(i_ln,lc->w_bl.at().getI(lc->w_id));	break;
		    case IO::Real:	setR(i_ln,lc->w_bl.at().getR(lc->w_id));	break;
		    case IO::Boolean:	setB(i_ln,lc->w_bl.at().getB(lc->w_id));	break;	    
		}		
		break;
	    }
	    case I_PRM:
	    {
		if( m_lnk[i_ln].prm->w_prm.freeStat() )	continue;
		AutoHD<TVal> pvl = m_lnk[i_ln].prm->w_prm.at().at().vlAt(m_lnk[i_ln].prm->atr);
		switch(ioType(i_ln))
		{
		    case IO::String:	setS(i_ln,pvl.at().getS());	break;
		    case IO::Integer:	setI(i_ln,pvl.at().getI());	break;
		    case IO::Real:	setR(i_ln,pvl.at().getR());	break;
		    case IO::Boolean:	setB(i_ln,pvl.at().getB());	break;
		}
		break;
	    }
	}
    ResAlloc::resReleaseR(hd_res);
    	
    //Calc function
    try{ TValFunc::calc( ); }
    catch(TError err){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    
    //Check real values borders
    for( unsigned i_io=0; i_io < ioSize(); i_io++ )
	if( ioType(i_io) == IO::Real )
	    switch(isinf(getR(i_io)))		
	    {
		case 1:  setR(i_io, 1e300);  break;
		case -1: setR(i_io, -1e300); break;
	    }
    
    //Put values to output links
    ResAlloc::resRequestR(hd_res);
    for( unsigned i_ln=0; i_ln < m_lnk.size(); i_ln++ )
    	if( m_lnk[i_ln].tp == O_PRM && !m_lnk[i_ln].prm->w_prm.freeStat() )
	{
	    AutoHD<TVal> pvl = m_lnk[i_ln].prm->w_prm.at().at().vlAt(m_lnk[i_ln].prm->atr);	    
	    switch(ioType(i_ln))
	    {
		case IO::String:	pvl.at().setS(getS(i_ln));	break;
		case IO::Integer:	pvl.at().setI(getI(i_ln));	break;
		case IO::Real:		pvl.at().setR(getR(i_ln));	break;
	    	case IO::Boolean:       pvl.at().setB(getB(i_ln));	break;
	    }
	}
    ResAlloc::resReleaseR(hd_res);
}

void Block::preIOCfgChange()
{
    if( enable() ) enable(false,false);
    TValFunc::preIOCfgChange();
}

void Block::postIOCfgChange()
{
    TValFunc::postIOCfgChange();
    if(toEnable())	enable(true);
    if(toProcess()) 	process(true);
}

void Block::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> list;		    
    
    switch(cmd)
    {
	case TCntrNode::Info:
	    ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",mod->I18N("Block: ")+id());
	    ctrMkNode("area",opt,a_path.c_str(),"/blck",mod->I18N("Block"));
	    ctrMkNode("area",opt,a_path.c_str(),"/blck/st",mod->I18N("State"));
	    ctrMkNode("fld",opt,a_path.c_str(),"/blck/st/en",mod->I18N("Enable"),0664,0,0,"bool");
	    if( owner().startStat() )
		ctrMkNode("fld",opt,a_path.c_str(),"/blck/st/prc",mod->I18N("Process"),0664,0,0,"bool");
	    ctrMkNode("area",opt,a_path.c_str(),"/blck/cfg",mod->I18N("Config"));
	    ctrMkNode("fld",opt,a_path.c_str(),"/blck/cfg/id",mod->I18N("Id"),0444,0,0,"str");
	    ctrMkNode("fld",opt,a_path.c_str(),"/blck/cfg/name",mod->I18N("Name"),0664,0,0,"str");
	    ctrMkNode("fld",opt,a_path.c_str(),"/blck/cfg/descr",mod->I18N("Description"),0664,0,0,"str")->
		attr_("cols","90")->attr_("rows","4");
	    ctrMkNode("fld",opt,a_path.c_str(),"/blck/cfg/toen",mod->I18N("To enable"),0664,0,0,"bool");
	    ctrMkNode("fld",opt,a_path.c_str(),"/blck/cfg/toprc",mod->I18N("To process"),0664,0,0,"bool");
	    ctrMkNode("area",opt,a_path.c_str(),"/blck/cfg/func",mod->I18N("Function"));
	    ctrMkNode("fld",opt,a_path.c_str(),"/blck/cfg/func/lib",mod->I18N("Library"),(!func())?0664:0444,0,0,"str")->
		attr_("dest","select")->attr("select","/blck/libs");
	    ctrMkNode("fld",opt,a_path.c_str(),"/blck/cfg/func/id",mod->I18N("Function"),(!func())?0664:0444,0,0,"str")->
		attr_("dest","select")->attr("select","/blck/fncs");
	    ctrMkNode("comm",opt,a_path.c_str(),"/blck/cfg/func/lnk",mod->I18N("View function"),0550,0,0,"lnk");
	    ctrMkNode("comm",opt,a_path.c_str(),"/blck/cfg/load",mod->I18N("Load"),0550);
	    ctrMkNode("comm",opt,a_path.c_str(),"/blck/cfg/save",mod->I18N("Save"),0550);
	    if( enable() )
	    {
		ctrMkNode("area",opt,a_path.c_str(),"/lio",mod->I18N("IO"));
		ctrMkNode("area",opt,a_path.c_str(),"/lio/show",mod->I18N("Show"));
		ctrMkNode("fld",opt,a_path.c_str(),"/lio/show/mode",mod->I18N("Show"),0664,0,0,"dec")->
		    attr_("dest","select")->attr("select","/lio/show/mode_lst");
		ctrMkNode("fld",opt,a_path.c_str(),"/lio/show/hide",mod->I18N("Hiden"),0664,0,0,"bool");
		ctrMkNode("area",opt,a_path.c_str(),"/lio/io",mod->I18N("IO"));
		
		//Put IO links
		vector<string> list;
		ioList(list);
		for( int i_io = 0; i_io < list.size(); i_io++ )
		{
		    int id = ioId(list[i_io]);
	    
		    if( ioHide(id) && !m_sw_hide ) continue;		
		
		    //Add values
		    if( m_sw_mode == 0 )
		    {
			char *tip;
			switch(ioType(id))
			{
			    case IO::String:	tip = "str";	break;
			    case IO::Integer:	tip = "dec";	break;
			    case IO::Real:   	tip = "real";	break;
			    case IO::Boolean: 	tip = "bool";	break;
			}
			ctrMkNode("fld",opt,a_path.c_str(),(string("/lio/io/0|")+list[i_io]).c_str(),
				func()->io(id)->name().c_str(),(m_lnk[id].tp != FREE)?0444:0664,0,0,tip);
    			    continue;
		    }
		    else if( m_sw_mode == 1 )
		    {		
    			//Add link's type	
    			ctrMkNode("fld",opt,a_path.c_str(),(string("/lio/io/1|")+list[i_io]).c_str(),
    				func()->io(id)->name().c_str(),0664,0,0,"str")->attr_("dest","select")->
    				attr_("select",(ioMode(id) != IO::Input)?"/lio/otp":"/lio/itp");
    			switch(m_lnk[id].tp)
    			{
    			    case I_LOC:
    				//Local block
    				ctrMkNode("fld",opt,a_path.c_str(),(string("/lio/io/2|")+list[i_io]).c_str(),"",0664,0,0,"str")->
    				    attr_("dest","select")->attr_("select","/lio/blk");
    				//Local block io
    				ctrMkNode("fld",opt,a_path.c_str(),(string("/lio/io/3|")+list[i_io]).c_str(),"",0664,0,0,"str")->
    				    attr_("dest","select")->attr_("select",(string("/lio/io/4|")+list[i_io]).c_str());
    				break;
    			    case I_GLB:
    				//Virtual controller
    				ctrMkNode("fld",opt,a_path.c_str(),(string("/lio/io/2|")+list[i_io]).c_str(),"",0664,0,0,"str")->
    				    attr_("dest","select")->attr_("select","/lio/vcntr");
    				//Virtual controller block
    				ctrMkNode("fld",opt,a_path.c_str(),(string("/lio/io/3|")+list[i_io]).c_str(),"",0664,0,0,"str")->
    				    attr_("dest","select")->attr_("select",(string("/lio/io/5|")+list[i_io]).c_str());		    
    				//Block's io
    				ctrMkNode("fld",opt,a_path.c_str(),(string("/lio/io/4|")+list[i_io]).c_str(),"",0664,0,0,"str")->
    				    attr_("dest","select")->attr_("select",(string("/lio/io/6|")+list[i_io]).c_str());
    				break;
    			    case I_PRM: case O_PRM:
    				//Parameter
    				ctrMkNode("fld",opt,a_path.c_str(),(string("/lio/io/2|")+list[i_io]).c_str(),"",0664,0,0,"str")->
    				    attr_("dest","select")->attr_("select","/lio/prms");
		    		// Parameter's atribut
		    		ctrMkNode("fld",opt,a_path.c_str(),(string("/lio/io/3|")+list[i_io]).c_str(),"",0664,0,0,"str")->
	    			    attr_("dest","select")->attr_("select",(string("/lio/io/4|")+list[i_io]).c_str());
		    		break;
		    	}
		    }
		}
	    }
	    break;
	case TCntrNode::Get:
	    if( a_path == "/blck/st/en" )     	ctrSetB( opt, enable() );
	    else if( a_path == "/blck/st/prc" )	ctrSetB( opt, process() );
	    else if( a_path == "/blck/cfg/id" )	ctrSetS( opt, id() );
	    else if( a_path == "/blck/cfg/name" )	ctrSetS( opt, name() );
	    else if( a_path == "/blck/cfg/descr" )	ctrSetS( opt, descr() );
	    else if( a_path == "/blck/cfg/toen" )	ctrSetB( opt, m_to_en );
	    else if( a_path == "/blck/cfg/toprc" )	ctrSetB( opt, m_to_prc );
	    else if( a_path == "/blck/cfg/func/lib" )	ctrSetS( opt, m_lib );
	    else if( a_path == "/blck/cfg/func/id" )	ctrSetS( opt, m_func );
	    else if( a_path == "/blck/cfg/func/lnk" )	opt->text("d_Functions/d_"+m_lib+"/d_"+m_func );
	    else if( a_path == "/blck/libs" )
	    {
		owner().owner().owner().owner().func().at().list(list);
		opt->childClean();
		for( unsigned i_a=0; i_a < list.size(); i_a++ )
		    ctrSetS( opt, owner().owner().owner().owner().func().at().at(list[i_a]).at().name(),list[i_a].c_str() );
	    }
	    else if( a_path == "/blck/fncs" )
	    {	
		if( owner().owner().owner().owner().func().at().present(m_lib) )
		{
		    owner().owner().owner().owner().func().at().at(m_lib).at().list(list);
		    opt->childClean();
		    for( unsigned i_a=0; i_a < list.size(); i_a++ )
			ctrSetS( opt, owner().owner().owner().owner().func().at().at(m_lib).at().at(list[i_a]).at().name(), list[i_a].c_str() );    
		}
	    }
	    else if( enable() )
	    {
		if( a_path == "/lio/show/mode" )	ctrSetI( opt, m_sw_mode );
		else if( a_path == "/lio/show/mode_lst" )
		{
		    ctrSetS( opt, mod->I18N("Values"), "0" );
		    ctrSetS( opt, mod->I18N("Links") , "1" );
		}
		else if( a_path == "/lio/show/hide" )	ctrSetB( opt, m_sw_hide );
		else if( a_path.substr(0,7) == "/lio/io" )
		{
		    char lev = TSYS::pathLev(a_path,2)[0];
		    int id   = ioId(TSYS::pathLev(a_path,2).substr(2));
		
		    if( lev == '0' )
			switch(ioType(id))
			{
			    case IO::String:	ctrSetS( opt, getS(id) );	break;
			    case IO::Integer:	ctrSetI( opt, getI(id) );	break;
			    case IO::Real:    	
				printf("TEST 00: %d\n", isnan(getR(id)));
				ctrSetR( opt, isnan(getR(id))?0:getR(id) );	
				break;
			    case IO::Boolean: 	ctrSetB( opt, getB(id) );	break;
			}
		    else if( lev == '1' )
			switch(m_lnk[id].tp)			    
			{
			    case FREE:	ctrSetS(opt,"free");	break;
			    case I_LOC:	ctrSetS(opt,"loc");	break;
			    case I_GLB:	ctrSetS(opt,"glob");	break;
			    case I_PRM: 
			    case O_PRM:	ctrSetS(opt,"prm");	break;
			}
		    else if( m_lnk[id].tp == I_LOC )
		    {
			if( lev == '2' )	ctrSetS(opt,m_lnk[id].iblk->blk);
			else if( lev == '3' )	ctrSetS(opt,m_lnk[id].iblk->id);
			else if( lev == '4' )
			{		 
			    if( owner().blkPresent(m_lnk[id].iblk->blk) )
			    {
				AutoHD<Block> blk = owner().blkAt(m_lnk[id].iblk->blk);
				blk.at().ioList(list);
				for( unsigned i_a=0; i_a < list.size(); i_a++ )
				    ctrSetS( opt, blk.at().func()->io(i_a)->name(), list[i_a].c_str() );
			    }
			}
		    }
		    else if( m_lnk[id].tp == I_GLB )
		    {
			if( lev == '2' )	ctrSetS(opt,m_lnk[id].iblk->cnt);
			else if( lev == '3' )	ctrSetS(opt,m_lnk[id].iblk->blk);
			else if( lev == '4' )  	ctrSetS(opt,m_lnk[id].iblk->id);
			else if( lev == '5' )
			{	
			    if( owner().owner().present(m_lnk[id].iblk->cnt) )
			    {
				((Contr &)owner().owner().at(m_lnk[id].iblk->cnt).at()).blkList(list);
				for( unsigned i_a=0; i_a < list.size(); i_a++ )
				    ctrSetS( opt, ((Contr &)owner().owner().at(m_lnk[id].iblk->cnt).at()).blkAt(list[i_a]).at().name(), list[i_a].c_str() );
			    }
			}
			else if( lev == '6' )
			{		 
			    if( owner().owner().present(m_lnk[id].iblk->cnt) && 
				    ((Contr &)owner().owner().at(m_lnk[id].iblk->cnt).at()).blkPresent(m_lnk[id].iblk->blk) )
			    {
				AutoHD<Block> blk = ((Contr &)owner().owner().at(m_lnk[id].iblk->cnt).at()).blkAt(m_lnk[id].iblk->blk);
				blk.at().ioList(list);
				for( unsigned i_a=0; i_a < list.size(); i_a++ )
				    ctrSetS( opt, blk.at().func()->io(i_a)->name(), list[i_a].c_str() );
			    }
			}
		    }
		    else if( m_lnk[id].tp == I_PRM || m_lnk[id].tp == O_PRM )
		    {
			if( lev == '2' )		ctrSetS(opt,m_lnk[id].prm->prm);
			else if( lev == '3' )	ctrSetS(opt,m_lnk[id].prm->atr);
			else if( lev == '4' )
			{	
			    AutoHD<TParamS> prms = owner().owner().owner().owner().param();
			    if( prms.at().present(m_lnk[id].prm->prm) )
			    {
				prms.at().at(m_lnk[id].prm->prm).at().at().vlList(list);
				for( unsigned i_a=0; i_a < list.size(); i_a++ )
				    if( m_lnk[id].tp == I_PRM || 
					    !(prms.at().at(m_lnk[id].prm->prm).at().at().vlAt(list[i_a]).at().fld().flg()&FLD_NWR) )
					ctrSetS( opt, list[i_a] );
			    }
			}
		    }
		}
		else if( a_path == "/lio/itp" )
		{	
		    opt->childClean();
		    ctrSetS( opt, mod->I18N("Free"), "free" );
		    ctrSetS( opt, mod->I18N("Local"), "loc" );
		    ctrSetS( opt, mod->I18N("Global"), "glob" );
		    ctrSetS( opt, mod->I18N("Parameter"), "prm" );
		}
		else if( a_path == "/lio/otp" )
		{
		    opt->childClean();
		    ctrSetS( opt, mod->I18N("Free")	,"free" );
		    ctrSetS( opt, mod->I18N("Parameter")	,"prm" );
		}   
		//Local blocks
		else if( a_path == "/lio/blk" )
		{
		    owner().blkList(list);
		    opt->childClean();
		    for( unsigned i_a=0; i_a < list.size(); i_a++ )
			ctrSetS( opt, owner().blkAt(list[i_a]).at().name(), list[i_a].c_str() );
		}				
		//Virtual controllers
		else if( a_path == "/lio/vcntr" )
		{
		    owner().owner().list(list);
		    opt->childClean();
		    for( unsigned i_a=0; i_a < list.size(); i_a++ )
			ctrSetS( opt, owner().owner().at(list[i_a]).at().lName(), list[i_a].c_str() );
		}				
		//Parameters
		else if( a_path == "/lio/prms" )
		{
		    owner().owner().owner().owner().param().at().list(list);
		    opt->childClean();
		    for( unsigned i_a=0; i_a < list.size(); i_a++ )
			ctrSetS( opt, list[i_a] );
		}
		else throw TError(nodePath().c_str(),"Branch <%s> error!",a_path.c_str());
	    }	    
	    else throw TError(nodePath().c_str(),"Branch <%s> error!",a_path.c_str());
	    break;
	case TCntrNode::Set:
	    if( a_path == "/blck/st/en" )       	enable(ctrGetB(opt));
	    else if( a_path == "/blck/st/prc" )		process(ctrGetB(opt));
	    else if( a_path == "/blck/cfg/name" )	name(ctrGetS(opt));
	    else if( a_path == "/blck/cfg/descr" )	descr(ctrGetS(opt));
	    else if( a_path == "/blck/cfg/toen" )       m_to_en = ctrGetB(opt);
	    else if( a_path == "/blck/cfg/toprc" )      m_to_prc = ctrGetB(opt);	    
	    else if( a_path == "/blck/cfg/func/lib" )  	m_lib = ctrGetS( opt );
	    else if( a_path == "/blck/cfg/func/id" )   	m_func = ctrGetS( opt );    
	    else if( a_path == "/blck/cfg/load" )       load();
	    else if( a_path == "/blck/cfg/save" )       save();	
	    else if( enable() )
	    {
		if( a_path == "/lio/show/mode" )        m_sw_mode = ctrGetI( opt );
		else if( a_path == "/lio/show/hide" )	m_sw_hide = ctrGetB( opt );
		else if( a_path.substr(0,7) == "/lio/io" )	    
		{
		    char lev = TSYS::pathLev(a_path,2)[0];
		    int id = ioId(TSYS::pathLev(a_path,2).substr(2));
		
		    if( lev == '0' )
		    {
			if(ioType(id) == IO::String)     	setS(id, ctrGetS( opt ));
			else if(ioType(id) == IO::Integer)	setI(id, ctrGetI( opt ));
			else if(ioType(id) == IO::Real)	setR(id, ctrGetR( opt ));
			else if(ioType(id) == IO::Boolean)	setB(id, ctrGetB( opt ));	
		    }	
		    else if( lev == '1' )
		    {
			if( ctrGetS(opt) == "free" )    	link(id,SET,FREE);
			else if( ctrGetS(opt) == "loc" )	link(id,SET,I_LOC);
			else if( ctrGetS(opt) == "glob" )	link(id,SET,I_GLB);
			else if( ctrGetS(opt) == "prm" )	
			{
			    if( ioMode(id) != IO::Input )	link(id,SET,O_PRM);
			    else 				link(id,SET,I_PRM);
			}
		    }
		    else if( m_lnk[id].tp == I_LOC )
		    {
			if( lev == '2' )	link(id,SET,m_lnk[id].tp,ctrGetS(opt),m_lnk[id].iblk->id);	    
			else if( lev == '3' )  	link(id,SET,m_lnk[id].tp,m_lnk[id].iblk->blk,ctrGetS(opt));
		    }
		    else if( m_lnk[id].tp == I_GLB )
		    {
			if( lev == '2' )	link(id,SET,m_lnk[id].tp,ctrGetS(opt),m_lnk[id].iblk->blk,m_lnk[id].iblk->id);
			else if( lev == '3' )	link(id,SET,m_lnk[id].tp,m_lnk[id].iblk->cnt,ctrGetS(opt),m_lnk[id].iblk->id);
			else if( lev == '4' )  	link(id,SET,m_lnk[id].tp,m_lnk[id].iblk->cnt,m_lnk[id].iblk->blk,ctrGetS(opt));
		    }
		    else if( m_lnk[id].tp == I_PRM || m_lnk[id].tp == O_PRM )
		    {
			if( lev == '2' )	link(id,SET,m_lnk[id].tp,ctrGetS(opt),m_lnk[id].prm->atr);
			else if( lev == '3' )	link(id,SET,m_lnk[id].tp,m_lnk[id].prm->prm,ctrGetS(opt));
		    }
		}
		else throw TError(nodePath().c_str(),"Branch <%s> error!",a_path.c_str());
	    }
	    else throw TError(nodePath().c_str(),"Branch <%s> error!",a_path.c_str());
	    break;
    }    
}

