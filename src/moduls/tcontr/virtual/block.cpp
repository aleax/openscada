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
#include <xml.h>
#include <tmessage.h>
#include <tparams.h>
#include "virtual.h"
#include "block.h"

using namespace Virtual;

//Function block
Block::Block( const string &iid, Contr *iown ) : 
    TConfig( &((TipContr &)iown->owner()).blockE() ), TValFunc(NULL), m_owner(iown), 
    m_process(false), m_sw_lnk(false), m_sw_hide(false),
    m_id(cfg("ID").getS()), m_name(cfg("NAME").getS()), m_descr(cfg("DESCR").getS()),
    m_lib(cfg("FUNC_LIB").getS()), m_func(cfg("FUNC_ID").getS()),
    m_to_en(cfg("EN").getB()), m_to_prc(cfg("PROC").getB())
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
	    TBDS &bds = owner().owner().owner().owner().BD();
	    
	    //Delete block from BD
            TBDS::SName tbl = owner().BD();
            tbl.tbl = owner().cfg("BLOCK_SH").getS();			    
            bds.open(tbl).at().fieldDel(*this);
            bds.close(tbl);
	    
	    //Delete block's IO from BD
	    TConfig cfg(&((TipContr &)owner().owner()).blockIOE());
	    tbl.tbl = owner().cfg("BLOCK_SH").getS()+"_io";
	    AutoHD<TTable> dbt = bds.open(tbl);
	    int fld_cnt = 0;
	    while( dbt.at().fieldSeek(fld_cnt++,cfg) )
		if( id() == cfg.cfg("BLK_ID").getS() )
		{
		    dbt.at().fieldDel(cfg);
		    fld_cnt--;
		}
        }	
    }catch(TError err)
    { owner().owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
}

void Block::load( bool self )
{    
    if( self )
    {
	TBDS &bd = owner().owner().owner().owner().BD();
	TBDS::SName tbl = owner().BD();
	tbl.tbl = owner().cfg("BLOCK_SH").getS();
    
	bd.open(tbl).at().fieldGet(*this);
	bd.close(tbl);
    }
    
    //Load io config
    if( enable() )
    {
	for( int i_ln = 0; i_ln < m_lnk.size(); i_ln++ )
    	    if( m_lnk[i_ln].tp != 0 ) 
	    try{ loadIO(i_ln); } 
	    catch(TError err){ owner().owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
    }
}

void Block::save( )
{
    TBDS &bd = owner().owner().owner().owner().BD();
    TBDS::SName tbl = owner().BD();
    tbl.tbl = owner().cfg("BLOCK_SH").getS();
    
    bd.open(tbl,true).at().fieldSet(*this);
    bd.close(tbl);
    
    //Save io config
    if( enable() )
    {
        for( int i_ln = 0; i_ln < m_lnk.size(); i_ln++ )
            if( m_lnk[i_ln].tp != 0 ) 
		try{ saveIO(i_ln); } 
		catch(TError err){ owner().owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
    }
}
		
void Block::loadIO( unsigned i_ln )
{    
    if( !enable() || m_lnk[i_ln].tp == 0 )	return;
    
    TConfig cfg(&((TipContr &)owner().owner()).blockIOE());
    cfg.cfg("BLK_ID").setS(id());
    cfg.cfg("ID").setS(func()->io(i_ln)->id);
    
    TBDS &bd = owner().owner().owner().owner().BD();
    TBDS::SName tbl = owner().BD();
    tbl.tbl = owner().cfg("BLOCK_SH").getS()+"_io";
    
    bd.open(tbl).at().fieldGet(cfg);
    bd.close(tbl);

    //Link type
    LnkT tp = (LnkT)cfg.cfg("TLNK").getI();
    //link( i_ln, (LnkT)cfg.cfg("TLNK").getI() );
    
    //Value
    string val = cfg.cfg("VAL").getS();
    if(type(i_ln)&IO_STR)       setS(i_ln,val);
    else if(type(i_ln)&IO_INT)  setI(i_ln,atoi(val.c_str()));
    else if(type(i_ln)&IO_REAL) setR(i_ln,atof(val.c_str()));
    else if(type(i_ln)&IO_BOOL) setB(i_ln,atoi(val.c_str()));
    
    //Config of link
    link(i_ln,SET,tp,cfg.cfg("O1").getS(),cfg.cfg("O2").getS(),cfg.cfg("O3").getS());
}

void Block::saveIO( unsigned i_ln )
{
    if( !enable() || m_lnk[i_ln].tp == 0 )      return;
	
    TConfig cfg(&((TipContr &)owner().owner()).blockIOE());
    cfg.cfg("BLK_ID").setS(id());
    cfg.cfg("ID").setS(func()->io(i_ln)->id);
		    
    TBDS &bd = owner().owner().owner().owner().BD();
    TBDS::SName tbl = owner().BD();
    tbl.tbl = owner().cfg("BLOCK_SH").getS()+"_io";

    //Link type
    cfg.cfg("TLNK").setI(m_lnk[i_ln].tp);
    
    //Value
    string val;
    if(type(i_ln)&IO_STR)       val = getS(i_ln);
    else if(type(i_ln)&IO_INT)  val = TSYS::int2str(getI(i_ln));
    else if(type(i_ln)&IO_REAL) val = TSYS::real2str(getR(i_ln));
    else if(type(i_ln)&IO_BOOL) val = TSYS::int2str(getB(i_ln));
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
    bd.open(tbl,true).at().fieldSet(cfg);
    bd.close(tbl);	    
}

void Block::enable( bool val )
{
    if( process() ) process(false);
    //Enable
    if( val && u_func.freeStat() )
    {
	u_func = owner().owner().owner().owner().func().at(m_lib).at().at(m_func);
	func( &u_func.at() );
	
	//Init links
	for( int i_ln = 0; i_ln < m_val.size(); i_ln++ )
	{
	    m_lnk.push_back( SLnk() );
	    if( m_val[i_ln].tp )
	    {
	    	m_lnk[i_ln].tp = FREE;
		//Load IO
		try{ loadIO(i_ln); }
        	catch(TError err){ owner().owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }			    
	    }
	    else m_lnk[i_ln].tp = DIS;	    
	}
    }
    //Disable
    else if( !val && !u_func.freeStat() )
    {
	for( unsigned i_ln = 0; i_ln < m_lnk.size(); i_ln++ )
	    link(i_ln,SET,FREE);
	m_lnk.clear();
	
	//Free func
	func(NULL);	
	u_func.free();	
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

Block::LnkT Block::link( unsigned id )
{    
    if( id >= m_lnk.size() || m_lnk[id].tp == DIS )	throw TError("Link %d error!",id);
    return m_lnk[id].tp;
}
                                   				   
void Block::link( unsigned id, LnkCmd cmd, LnkT lnk, const string &o1, const string &o2, const string &o3 )
{
    string lo1, lo2, lo3;

    ResAlloc res(hd_res,true);
    if( id >= m_lnk.size() || m_lnk[id].tp == DIS )	throw TError("Link %d error!",id);
    
    //Change type link
    if( cmd == SET && lnk != m_lnk[id].tp )
    {    
	//Free old structures
	if( m_lnk[id].tp == I_LOC || m_lnk[id].tp == I_GLB )		delete m_lnk[id].iblk;
	else if( m_lnk[id].tp == I_PRM || m_lnk[id].tp == O_PRM )	delete m_lnk[id].prm;
    
	//Make new structures
	if( lnk == I_LOC || lnk == I_GLB )	m_lnk[id].iblk = new SLIBlk;
	else if( lnk == I_PRM || lnk == O_PRM )	m_lnk[id].prm  = new SLPrm;
	
	m_lnk[id].tp = lnk;
    }
    
    //Connect new link and init
    if( cmd == INIT || cmd == SET )
    {
	//Init local link
	if( m_lnk[id].tp == I_LOC )
	{
	    if( cmd == SET )
	    {
		m_lnk[id].iblk->blk = o1;
		m_lnk[id].iblk->id  = o2;
	    }
	    if( cmd == INIT || process() )
	    {
		lo1 = m_lnk[id].iblk->blk;
		lo2 = m_lnk[id].iblk->id;
		if( owner().blkAvoid(lo1) && owner().blkAt(lo1).at().ioId(lo2) >= 0 )
		{
		    m_lnk[id].iblk->w_bl = owner().blkAt(lo1);
        	    m_lnk[id].iblk->w_id = m_lnk[id].iblk->w_bl.at().ioId(lo2);
		}
	    }
	}
	//Init global link
	if( m_lnk[id].tp == I_GLB )
	{
	    if( cmd == SET )
	    {
		m_lnk[id].iblk->cnt = o1;
		m_lnk[id].iblk->blk = o2;
		m_lnk[id].iblk->id  = o3;
	    }
	    if( cmd == INIT || process() )
	    {
		lo1 = m_lnk[id].iblk->cnt;
		lo2 = m_lnk[id].iblk->blk;
		lo3 = m_lnk[id].iblk->id;
		if( owner().owner().avoid(lo1) && 
			((Contr &)owner().owner().at(lo1).at()).blkAvoid(lo2) && 
			((Contr &)owner().owner().at(lo1).at()).blkAt(lo2).at().ioId(lo3) >= 0 )
		{
		    m_lnk[id].iblk->w_bl = ((Contr &)owner().owner().at(lo1).at()).blkAt(lo2);
        	    m_lnk[id].iblk->w_id = m_lnk[id].iblk->w_bl.at().ioId(lo3);
		}
	    }
	}	
	//Init parameter's link
	if( m_lnk[id].tp == I_PRM || m_lnk[id].tp == O_PRM )
	{
	    if( cmd == SET )
	    {
		m_lnk[id].prm->prm = o1;
		m_lnk[id].prm->atr = o2;
	    }
	    if( cmd == INIT || process() )
	    {
                TParamS &prms = owner().owner().owner().owner().Param();
		
		lo1 = m_lnk[id].prm->prm;
		lo2 = m_lnk[id].prm->atr;
		if( prms.avoid(lo1) && prms.at(lo1).at().at().vlAvoid(lo2) )
		    m_lnk[id].prm->w_prm = prms.at(lo1);
	    }
	}	
    }
    
    //Disconnect
    if( cmd == DEINIT )
    {
        if( m_lnk[id].tp == I_LOC || m_lnk[id].tp == I_GLB )
	    m_lnk[id].iblk->w_bl.free();
    	if( m_lnk[id].tp == I_PRM || m_lnk[id].tp == O_PRM )
	    m_lnk[id].prm->w_prm.free();	
    }
}

void Block::calc( )
{
    //Get values from input links
    ResAlloc::resRequestR(hd_res);
    for( unsigned i_ln=0; i_ln < m_lnk.size(); i_ln++ )
    {
	char tp = type(i_ln);
	
	if( m_lnk[i_ln].tp == I_LOC || m_lnk[i_ln].tp == I_GLB )
	{	    
	    SLIBlk *lc = m_lnk[i_ln].iblk;
	    if( lc->w_bl.freeStat() )	continue;
	    char tpp = lc->w_bl.at().type(lc->w_id);
	    
            if( tp&IO_STR )
	    {	
		if( tpp&IO_STR )	setS(i_ln,lc->w_bl.at().getS(lc->w_id));
		else if( tpp&IO_INT )	setS(i_ln,TSYS::int2str(lc->w_bl.at().getI(lc->w_id)));
		else if( tpp&IO_REAL )	setS(i_ln,TSYS::real2str(lc->w_bl.at().getR(lc->w_id)));
		else if( tpp&IO_BOOL )	setS(i_ln,TSYS::int2str(lc->w_bl.at().getB(lc->w_id)));
	    }
	    else if( tp&IO_INT )
	    {	
		if( tpp&IO_STR )	setI(i_ln,atoi(lc->w_bl.at().getS(lc->w_id).c_str()));
		else if( tpp&IO_INT )	setI(i_ln,lc->w_bl.at().getI(lc->w_id));
		else if( tpp&IO_REAL )	setI(i_ln,lc->w_bl.at().getR(lc->w_id));
		else if( tpp&IO_BOOL )	setI(i_ln,lc->w_bl.at().getB(lc->w_id));
	    }
	    else if( tp&IO_REAL )
	    {	
		if( tpp&IO_STR )	setR(i_ln,atof(lc->w_bl.at().getS(lc->w_id).c_str()));
		else if( tpp&IO_INT )	setR(i_ln,lc->w_bl.at().getI(lc->w_id));
		else if( tpp&IO_REAL )	setR(i_ln,lc->w_bl.at().getR(lc->w_id));
		else if( tpp&IO_BOOL )	setR(i_ln,lc->w_bl.at().getB(lc->w_id));
	    }
	    else if( tp&IO_BOOL )
	    {	
		if( tpp&IO_STR )	setB(i_ln,lc->w_bl.at().getS(lc->w_id).size());
		else if( tpp&IO_INT )	setB(i_ln,lc->w_bl.at().getI(lc->w_id));
		else if( tpp&IO_REAL )	setB(i_ln,lc->w_bl.at().getR(lc->w_id));
		else if( tpp&IO_BOOL )	setB(i_ln,lc->w_bl.at().getB(lc->w_id));
	    }
	}
    	else if( m_lnk[i_ln].tp == I_PRM )
	{
	    if( m_lnk[i_ln].prm->w_prm.freeStat() )	continue;
	    AutoHD<TVal> pvl = m_lnk[i_ln].prm->w_prm.at().at().vlAt(m_lnk[i_ln].prm->atr);
	    unsigned tpp = pvl.at().fld().type();
	    
            if( tp&IO_STR )
	    {	
		if( tpp&T_STRING )	setS(i_ln,pvl.at().getS());
		else if( tpp&(T_DEC|T_OCT|T_HEX) )	setS(i_ln,TSYS::int2str(pvl.at().getI()));
		else if( tpp&T_REAL )	setS(i_ln,TSYS::real2str(pvl.at().getR()));
		else if( tpp&T_BOOL )	setS(i_ln,TSYS::int2str(pvl.at().getB()));
	    }
	    else if( tp&IO_INT )
	    {
		if( tpp&T_STRING )	setI(i_ln,atoi(pvl.at().getS().c_str()));
		else if( tpp&(T_DEC|T_OCT|T_HEX) )	setI(i_ln,pvl.at().getI());
		else if( tpp&T_REAL )	setI(i_ln,pvl.at().getR());
		else if( tpp&T_BOOL )	setI(i_ln,pvl.at().getB());
	    }
	    else if( tp&IO_REAL )
	    {
		if( tpp&T_STRING )	setR(i_ln,atof(pvl.at().getS().c_str()));
		else if( tpp&(T_DEC|T_OCT|T_HEX) )	setR(i_ln,pvl.at().getI());
		else if( tpp&T_REAL )	setR(i_ln,pvl.at().getR());
		else if( tpp&T_BOOL )	setR(i_ln,pvl.at().getB());
	    }	    
	    else if( tp&IO_BOOL )
	    {
		if( tpp&T_STRING )	setB(i_ln,pvl.at().getS().size());
		else if( tpp&(T_DEC|T_OCT|T_HEX) )	setB(i_ln,pvl.at().getI());
		else if( tpp&T_REAL )	setB(i_ln,pvl.at().getR());
		else if( tpp&T_BOOL )	setB(i_ln,pvl.at().getB());
	    }	    
	}
    }    
    ResAlloc::resReleaseR(hd_res);
    	
    //Calc function
    try{ TValFunc::calc( ); }
    catch(TError err){ owner().owner().mPut("SYS",MESS_ERR,"%s",err.what().c_str()); }
    
    //Put values to output links
    ResAlloc::resRequestR(hd_res);
    for( unsigned i_ln=0; i_ln < m_lnk.size(); i_ln++ )
    {
	char tp = type(i_ln);
    	if( m_lnk[i_ln].tp == O_PRM )
	{
	    if( m_lnk[i_ln].prm->w_prm.freeStat() )	continue;
	    AutoHD<TVal> pvl = m_lnk[i_ln].prm->w_prm.at().at().vlAt(m_lnk[i_ln].prm->atr);
	    unsigned tpp = pvl.at().fld().type();
	    
            if( tp&IO_STR )
	    {	
		if( tpp&T_STRING )	pvl.at().setS(getS(i_ln));
		else if( tpp&(T_DEC|T_OCT|T_HEX) ) pvl.at().setI(atoi(getS(i_ln).c_str()));
		else if( tpp&T_REAL )   pvl.at().setR(atof(getS(i_ln).c_str()));
		else if( tpp&T_BOOL )   pvl.at().setB(getS(i_ln).size());
	    }
	    else if( tp&IO_INT )
	    {
		if( tpp&T_STRING )	pvl.at().setS(TSYS::int2str(getI(i_ln)));
		else if( tpp&(T_DEC|T_OCT|T_HEX) ) pvl.at().setI(getI(i_ln));
		else if( tpp&T_REAL )   pvl.at().setR(getI(i_ln));
		else if( tpp&T_BOOL )   pvl.at().setB(getI(i_ln));
	    }
	    else if( tp&IO_REAL )
	    {
		if( tpp&T_STRING )	pvl.at().setS(TSYS::real2str(getR(i_ln)));
		else if( tpp&(T_DEC|T_OCT|T_HEX) ) pvl.at().setI(getR(i_ln));
		else if( tpp&T_REAL )   pvl.at().setR(getR(i_ln));
		else if( tpp&T_BOOL )   pvl.at().setB(getR(i_ln));
	    }	    
	    else if( tp&IO_BOOL )
	    {
		if( tpp&T_STRING )	pvl.at().setS(TSYS::int2str(getB(i_ln)));
		else if( tpp&(T_DEC|T_OCT|T_HEX) ) pvl.at().setI(getB(i_ln));
		else if( tpp&T_REAL )   pvl.at().setR(getB(i_ln));
		else if( tpp&T_BOOL )   pvl.at().setB(getB(i_ln));
	    }	    
	}
    }    
    ResAlloc::resReleaseR(hd_res);
}

void Block::ctrStat_( XMLNode *inf )
{
    char *dscr = "dscr";
    char *i_cntr =
	"<oscada_cntr>"
	 "<area id='blck'>"
	  "<area id='st'>"
	   "<fld id='en' acs='0664' tp='bool'/>"
	   "<fld id='prc' acs='0664' tp='bool'/>"
          "</area>"			  
	  "<area id='cfg'>"
	   "<fld id='id' acs='0444' tp='str'/>"
	   "<fld id='name' acs='0664' tp='str'/>"
	   "<fld id='descr' acs='0664' tp='str'/>"
	   "<fld id='toen' acs='0664' tp='bool'/>"
	   "<fld id='toprc' acs='0664' tp='bool'/>"
	   "<area id='func'>"	   
	    "<fld id='lib' acs='0444' tp='str' dest='select' select='/blck/libs'/>"
	    "<fld id='id' acs='0444' tp='str' dest='select' select='/blck/fncs'/>"
	    "<fld id='nm' acs='0444' tp='str'/>"
	    "<fld id='dscr' acs='0444' tp='str' cols='90' rows='3'/>"
	   "</area>"
           "<comm id='load' acs='0550'/>"
	   "<comm id='save' acs='0550'/>"				 
          "</area>"			  
	 "</area>"
	"</oscada_cntr>";
	
    inf->load( i_cntr );
    inf->text( "Block: "+id() );
    //Section "Block"
    XMLNode *t_cntr = inf->childGet(0);
    t_cntr->attr(dscr,"Block");    
    t_cntr->childGet(0)->attr(dscr,"State");
    t_cntr->childGet(0)->childGet(0)->attr(dscr,"Enable");
    if( !owner().startStat() ) t_cntr->childGet(0)->childDel(1);
    else t_cntr->childGet(0)->childGet(1)->attr(dscr,"Process");
    t_cntr->childGet(1)->attr(dscr,"Config");
    t_cntr = t_cntr->childGet(1);
    t_cntr->childGet(0)->attr(dscr,"Id");
    t_cntr->childGet(1)->attr(dscr,"Name");
    t_cntr->childGet(2)->attr(dscr,"Description");
    t_cntr->childGet(3)->attr(dscr,"To enable");
    t_cntr->childGet(4)->attr(dscr,"To process");
    t_cntr->childGet(6)->attr(dscr,"Load from BD");
    t_cntr->childGet(7)->attr(dscr,"Save to BD");	    
    t_cntr = t_cntr->childGet(5);
    t_cntr->attr(dscr,"Function");
    t_cntr->childGet(0)->attr(dscr,"Library");
    t_cntr->childGet(1)->attr(dscr,"Id");
    t_cntr->childGet(2)->attr(dscr,"Name");
    t_cntr->childGet(3)->attr(dscr,"Description");
    if( u_func.freeStat() )
    {
	t_cntr->childGet(0)->attr("acs","0664");
	t_cntr->childGet(1)->attr("acs","0664");
    }
    
    //Add Links and values
    if( enable() )
    {	
	char *l_io = 
	    "<area id='lio'>"
	     "<area id='show'>"
	      "<fld id='lnk' acs='0664' tp='bool'/>"
	      "<fld id='hide' acs='0664' tp='bool'/>"
	     "</area>"
	     "<area id='in'/>"
	     "<area id='out'/>"
	    "</area>";
	    
	//Section "IO"    
	t_cntr = inf->childAdd();
	t_cntr->load(l_io);	
	t_cntr->attr(dscr,"IO");
	t_cntr->childGet(0)->attr(dscr,"Show");
	t_cntr->childGet(0)->childGet(0)->attr(dscr,"Links");
	t_cntr->childGet(0)->childGet(1)->attr(dscr,"Hiden");
	t_cntr->childGet(1)->attr(dscr,"Inputs");
	t_cntr->childGet(2)->attr(dscr,"Outputs");
	//Put IO links
        vector<string> list;
        ioList(list);
        for( int i_io = 0; i_io < list.size(); i_io++ )
        {
	    int id = ioId(list[i_io]);
	    
	    if( type(id)&IO_HIDE && !m_sw_hide ) continue;
	
	    XMLNode *io_area, *io_el;
	    if( type(id)&(IO_OUT|IO_RET) )	io_area = t_cntr->childGet(2);
	    else				io_area = t_cntr->childGet(1);
	
	    //Add values
	    if( !m_sw_lnk )
	    {
		io_el = io_area->childAdd("fld");
		io_el->attr("id","0|"+list[i_io]);  //Type
		if( m_lnk[id].tp != FREE )	io_el->attr("acs","0444");
		io_el->attr("dscr",func()->io(id)->name);
        	if(type(id)&IO_STR)         	io_el->attr("tp","str");
		else if(type(id)&IO_INT)    	io_el->attr("tp","dec");
		else if(type(id)&IO_REAL)   	io_el->attr("tp","real");
		else if(type(id)&IO_BOOL)   	io_el->attr("tp","bool");
		continue;
	    }
	    
	    //Add link's type	
	    io_el = io_area->childAdd("fld");
	    io_el->attr("id","1|"+list[i_io]);	//Type
            io_el->attr("dscr",func()->io(id)->name);
	    io_el->attr("tp","str");
	    io_el->attr("dest","select");
	    if( type(id)&(IO_OUT|IO_RET) ) 	io_el->attr("select","/lio/otp");
	    else 				io_el->attr("select","/lio/itp");
	    //Check type
            if( m_lnk[id].tp == I_LOC )
	    {
		//Local block
		io_el = io_area->childAdd("fld");
		io_el->attr("id","2|"+list[i_io]);
		io_el->attr("tp","str");
		io_el->attr("dest","select");
		io_el->attr("select","/lio/blk");
		
		//Local block io
		io_el = io_area->childAdd("fld");
		io_el->attr("id","3|"+list[i_io]);
                io_el->attr("tp","str");
		io_el->attr("dest","select");
                io_el->attr("select","/lio/in/4|"+list[i_io]);
	    }
            if( m_lnk[id].tp == I_GLB )
	    {
		//Virtual controller
		io_el = io_area->childAdd("fld");
		io_el->attr("id","2|"+list[i_io]);
		io_el->attr("tp","str");
		io_el->attr("dest","select");
		io_el->attr("select","/lio/vcntr");

		//Virtual controller block
                io_el = io_area->childAdd("fld");
                io_el->attr("id","3|"+list[i_io]);
               	io_el->attr("tp","str");
               	io_el->attr("dest","select");
		io_el->attr("select","/lio/in/5|"+list[i_io]);

		//Block's io	    
		io_el = io_area->childAdd("fld");
		io_el->attr("id","4|"+list[i_io]);
                io_el->attr("tp","str");
		io_el->attr("dest","select");
                io_el->attr("select","/lio/in/6|"+list[i_io]);
	    }
            if( m_lnk[id].tp == I_PRM || m_lnk[id].tp == O_PRM )
	    {
		//Parameter
		io_el = io_area->childAdd("fld");
		io_el->attr("id","2|"+list[i_io]);
		io_el->attr("tp","str");
		io_el->attr("dest","select");
		io_el->attr("select","/lio/prms");

		// Parameter's atribut
                io_el = io_area->childAdd("fld");
                io_el->attr("id","3|"+list[i_io]);
               	io_el->attr("tp","str");
               	io_el->attr("dest","select");
		io_el->attr("select","/lio/in/4|"+list[i_io]);
	    }
	}	
    }										
}			

void Block::ctrDinGet_( const string &a_path, XMLNode *opt )
{
    vector<string> list;		    
    if( a_path == "/blck/st/en" )     		ctrSetB( opt, enable() );
    else if( a_path == "/blck/st/prc" )		ctrSetB( opt, process() );
    else if( a_path == "/blck/cfg/id" )		ctrSetS( opt, id() );
    else if( a_path == "/blck/cfg/name" )	ctrSetS( opt, name() );
    else if( a_path == "/blck/cfg/descr" )	ctrSetS( opt, descr() );
    else if( a_path == "/blck/cfg/toen" )	ctrSetB( opt, m_to_en );
    else if( a_path == "/blck/cfg/toprc" )      ctrSetB( opt, m_to_prc );
    else if( a_path == "/blck/cfg/func/lib" )	ctrSetS( opt, m_lib );
    else if( a_path == "/blck/cfg/func/id" )	ctrSetS( opt, m_func );
    else if( a_path == "/blck/libs" )
    {
	owner().owner().owner().owner().func().list(list);
        opt->childClean();
        for( unsigned i_a=0; i_a < list.size(); i_a++ )
    	    ctrSetS( opt, list[i_a] );
    }
    else if( a_path == "/blck/fncs" )
    {	
	if( owner().owner().owner().owner().func().avoid(m_lib) )
	{
    	    owner().owner().owner().owner().func().at(m_lib).at().list(list);
    	    opt->childClean();
    	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
    		ctrSetS( opt, list[i_a] );    
	}
    }
    else if( enable() )
    {
	if( a_path == "/blck/cfg/func/nm" )	ctrSetS( opt, u_func.at().name() );
	else if( a_path == "/blck/cfg/func/dscr" )	ctrSetS( opt, u_func.at().descr() );    
        else if( a_path == "/lio/show/lnk" )	ctrSetB( opt, m_sw_lnk );
	else if( a_path == "/lio/show/hide" )	ctrSetB( opt, m_sw_hide );
	else if( a_path.substr(0,7) == "/lio/in" || a_path.substr(0,8) == "/lio/out" )
	{
	    char lev = pathLev(a_path,2)[0];
	    int id   = ioId(pathLev(a_path,2).substr(2));
	
	    if( lev == '0' )
    	    {
    		if(type(id)&IO_STR)             ctrSetS( opt, getS(id) );
        	else if(type(id)&IO_INT)        ctrSetI( opt, getI(id) );
		else if(type(id)&IO_REAL)       ctrSetR( opt, getR(id) );
		else if(type(id)&IO_BOOL)       ctrSetB( opt, getB(id) );	
	    }	
	    else if( lev == '1' )
	    {
		if( m_lnk[id].tp == FREE )	ctrSetS(opt,"Free");
		else if( m_lnk[id].tp == I_LOC )ctrSetS(opt,"Local");
		else if( m_lnk[id].tp == I_GLB )ctrSetS(opt,"Global");
		else if( m_lnk[id].tp == I_PRM || m_lnk[id].tp == O_PRM )
		    ctrSetS(opt,"Param");
	    }
	    else if( m_lnk[id].tp == I_LOC )
	    {
		if( lev == '2' )		ctrSetS(opt,m_lnk[id].iblk->blk);
		else if( lev == '3' )  		ctrSetS(opt,m_lnk[id].iblk->id);
		else if( lev == '4' )
		{		 
		    if( owner().blkAvoid(m_lnk[id].iblk->blk) )
		    {
		        AutoHD<Block> blk = owner().blkAt(m_lnk[id].iblk->blk);
			blk.at().ioList(list);
	    		for( unsigned i_a=0; i_a < list.size(); i_a++ )
			    if( blk.at().type(blk.at().ioId(list[i_a]))&(IO_OUT|IO_RET) )
	        		ctrSetS( opt, list[i_a] );
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
		    if( owner().owner().avoid(m_lnk[id].iblk->cnt) )
		    {
			((Contr &)owner().owner().at(m_lnk[id].iblk->cnt).at()).blkList(list);
			for( unsigned i_a=0; i_a < list.size(); i_a++ )
		    	    ctrSetS( opt, list[i_a] );
	    	    }
		}
		else if( lev == '6' )
		{		 
		    if( owner().owner().avoid(m_lnk[id].iblk->cnt) && 
			    ((Contr &)owner().owner().at(m_lnk[id].iblk->cnt).at()).blkAvoid(m_lnk[id].iblk->blk) )
		    {
			AutoHD<Block> blk = ((Contr &)owner().owner().at(m_lnk[id].iblk->cnt).at()).blkAt(m_lnk[id].iblk->blk);
			blk.at().ioList(list);
	    		for( unsigned i_a=0; i_a < list.size(); i_a++ )
			    if( blk.at().type(blk.at().ioId(list[i_a]))&(IO_OUT|IO_RET) )
	        		ctrSetS( opt, list[i_a] );
		    }
		}
	    }
	    else if( m_lnk[id].tp == I_PRM || m_lnk[id].tp == O_PRM )
	    {
		if( lev == '2' )		ctrSetS(opt,m_lnk[id].prm->prm);
		else if( lev == '3' )	ctrSetS(opt,m_lnk[id].prm->atr);
		else if( lev == '4' )
		{	
		    TParamS &prms = owner().owner().owner().owner().Param();
		    if( prms.avoid(m_lnk[id].prm->prm) )
		    {
			prms.at(m_lnk[id].prm->prm).at().at().vlList(list);
			for( unsigned i_a=0; i_a < list.size(); i_a++ )
			    if( m_lnk[id].tp == I_PRM || 
				    !(prms.at(m_lnk[id].prm->prm).at().at().vlAt(list[i_a]).at().fld().type()&F_NWR) )
				ctrSetS( opt, list[i_a] );
		    }
		}
	    }
	}
	else if( a_path == "/lio/itp" )
	{	
	    opt->childClean();
	    ctrSetS( opt, "Free" );
	    ctrSetS( opt, "Local" );
	    ctrSetS( opt, "Global" );
	    ctrSetS( opt, "Param" );
	}
	else if( a_path == "/lio/otp" )
	{
	    opt->childClean();
    	    ctrSetS( opt, "Free" );
	    ctrSetS( opt, "Param" );
	}   
	//Local blocks
	else if( a_path == "/lio/blk" )
	{
	    owner().blkList(list);
    	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, list[i_a] );
	}				
	//Virtual controllers
	else if( a_path == "/lio/vcntr" )
	{
    	    owner().owner().list(list);
    	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, list[i_a] );
	}				
	//Parameters
	else if( a_path == "/lio/prms" )
	{
	    owner().owner().owner().owner().Param().list(list);
    	    opt->childClean();
	    for( unsigned i_a=0; i_a < list.size(); i_a++ )
		ctrSetS( opt, list[i_a] );
	}
    }
}
	    
void Block::ctrDinSet_( const string &a_path, XMLNode *opt )
{
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
        if( a_path == "/lio/show/lnk" )		m_sw_lnk  = ctrGetB( opt );
	else if( a_path == "/lio/show/hide" )	m_sw_hide = ctrGetB( opt );
	else if( a_path.substr(0,7) == "/lio/in" || a_path.substr(0,8) == "/lio/out" )	    
	{
    	    char lev = pathLev(a_path,2)[0];
    	    int id = ioId(pathLev(a_path,2).substr(2));
	
    	    if( lev == '0' )
	    {
	    	if(type(id)&IO_STR)             setS(id, ctrGetS( opt ));
		else if(type(id)&IO_INT)        setI(id, ctrGetI( opt ));
		else if(type(id)&IO_REAL)       setR(id, ctrGetR( opt ));
		else if(type(id)&IO_BOOL)       setB(id, ctrGetB( opt ));	
	    }	
	    else if( lev == '1' )
	    {
		if( ctrGetS(opt) == "Free" )    	link(id,SET,FREE);
		else if( ctrGetS(opt) == "Local" )	link(id,SET,I_LOC);
		else if( ctrGetS(opt) == "Global" )	link(id,SET,I_GLB);
		else if( ctrGetS(opt) == "Param" )	
		{
		    if( type(id)&(IO_OUT|IO_RET) )	link(id,SET,O_PRM);
		    else 				link(id,SET,I_PRM);
		}
	    }
	    else if( m_lnk[id].tp == I_LOC )
	    {
		if( lev == '2' )		link(id,SET,m_lnk[id].tp,ctrGetS(opt),m_lnk[id].iblk->id);	    
		else if( lev == '3' )  	link(id,SET,m_lnk[id].tp,m_lnk[id].iblk->blk,ctrGetS(opt));
	    }
	    else if( m_lnk[id].tp == I_GLB )
	    {
		if( lev == '2' )		link(id,SET,m_lnk[id].tp,ctrGetS(opt),m_lnk[id].iblk->blk,m_lnk[id].iblk->id);
		else if( lev == '3' )	link(id,SET,m_lnk[id].tp,m_lnk[id].iblk->cnt,ctrGetS(opt),m_lnk[id].iblk->id);
		else if( lev == '4' )  	link(id,SET,m_lnk[id].tp,m_lnk[id].iblk->cnt,m_lnk[id].iblk->blk,ctrGetS(opt));
	    }
	    else if( m_lnk[id].tp == I_PRM || m_lnk[id].tp == O_PRM )
	    {
		if( lev == '2' )		link(id,SET,m_lnk[id].tp,ctrGetS(opt),m_lnk[id].prm->atr);
		else if( lev == '3' )	link(id,SET,m_lnk[id].tp,m_lnk[id].prm->prm,ctrGetS(opt));
	    }
	}
    }
}

