/***************************************************************************
 *   Copyright (C) 2005 by Roman Savochenko                                *
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
    m_enable(false), m_process(false), m_sw_mode(0), m_sw_hide(false),
    m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), m_descr(cfg("DESCR").getSd()),
    m_func(cfg("FUNC").getSd()), m_to_en(cfg("EN").getBd()), m_to_prc(cfg("PROC").getBd())
{
    m_id = iid;
    lnk_res = ResAlloc::resCreate();
}

Block::~Block( )
{
    if( enable() ) enable(false);
    
    ResAlloc::resDelete(lnk_res);
}

void Block::postDisable(int flag)
{
    try
    {
        if( flag )
        {
	    //Delete block from BD
            TBDS::SName tbl = owner().BD();
            tbl.tbl = owner().cfg("BLOCK_SH").getS();
	    SYS->db().at().dataDel(tbl,mod->nodePath()+tbl.tbl,*this);
	    
	    //Delete block's IO from BD
	    TConfig cfg(&((TipContr &)owner().owner()).blockIOE());
	    tbl.tbl = owner().cfg("BLOCK_SH").getS()+"_io";
	    cfg.cfg("BLK_ID").setS(id());   //Delete all block id records
            cfg.cfg("ID").setS("");			    
	    SYS->db().at().dataDel(tbl,mod->nodePath()+tbl.tbl,cfg);
        }	
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}

void Block::load(  )
{   
    TBDS::SName bd = owner().BD();
    bd.tbl = owner().cfg("BLOCK_SH").getS();
    SYS->db().at().dataGet(bd,mod->nodePath()+bd.tbl,*this);
     
    //Load IO config
    loadIO();
}

void Block::save( )
{
    TBDS::SName bd = owner().BD();
    bd.tbl = owner().cfg("BLOCK_SH").getS();
    SYS->db().at().dataSet(bd,mod->nodePath()+bd.tbl,*this);
    
    //Save IO config
    saveIO();
}
		
void Block::loadIO( )
{    
    if( !func() ) return;
    
    TConfig cfg(&mod->blockIOE());
    cfg.cfg("BLK_ID").setS(id());	
    TBDS::SName bd = owner().BD();
    bd.tbl = owner().cfg("BLOCK_SH").getS()+"_io";	
    
    for( int i_ln = 0; i_ln < m_val.size(); i_ln++ )
    {    
	if( i_ln >= m_lnk.size() )
	{  
	    m_lnk.push_back( SLnk() );
	    m_lnk[i_ln].tp = FREE;
	}	    
    
	try
	{
	    cfg.cfg("ID").setS(func()->io(i_ln)->id());    
	    SYS->db().at().dataGet(bd,mod->nodePath()+bd.tbl,cfg);
    	    //Value
	    setS(i_ln,cfg.cfg("VAL").getS());
	    //Config of link
	    link(i_ln,SET,(LnkT)cfg.cfg("TLNK").getI(),cfg.cfg("LNK").getS());
	}
	catch(TError err){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
    }
}

void Block::saveIO( )
{
    if( !enable() )      return;
	
    TConfig cfg(&mod->blockIOE());
    cfg.cfg("BLK_ID").setS(id());
    TBDS::SName bd = owner().BD();
    bd.tbl = owner().cfg("BLOCK_SH").getS()+"_io";
    
    for( int i_ln = 0; i_ln < m_lnk.size(); i_ln++ )
        try
	{ 	
	    cfg.cfg("ID").setS(func()->io(i_ln)->id());		    
	    cfg.cfg("TLNK").setI(m_lnk[i_ln].tp);	//Type link
	    cfg.cfg("LNK").setS((m_lnk[i_ln].tp == FREE)?"":m_lnk[i_ln].lnk);	//Link
	    cfg.cfg("VAL").setS(getS(i_ln));		//Value	    
	    
	    SYS->db().at().dataSet(bd,mod->nodePath()+bd.tbl,cfg);	
	}
        catch(TError err){ Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}

void Block::enable( bool val )
{
    //Enable
    if( val && !m_enable )
    {
	if( !func() && dynamic_cast<TFunction *>(&SYS->nodeAt(m_func,0,'.').at()) )
	    func( (TFunction *)&SYS->nodeAt(m_func,0,'.').at() );
	//Init links
	loadIO( );
    }
    //Disable
    else if( !val && m_enable )
    {
	if( process() ) process(false);
	//Save IO config
        //saveIO();
	
	//Clean IO
	for( unsigned i_ln = 0; i_ln < m_lnk.size(); i_ln++ )
	    link(i_ln,SET,FREE);
	m_lnk.clear();
	
	//Free func
	func(NULL);
    }
    m_enable = val;
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
    if( iid >= m_lnk.size() )	
	throw TError(nodePath().c_str(),mod->I18N("Link %d error!"),iid);
    return m_lnk[iid].tp;
}
                                   				   
void Block::link( unsigned iid, LnkCmd cmd, LnkT lnk, const string &vlnk )
{
    ResAlloc res(lnk_res,true);
    if( iid >= m_lnk.size() )	
	throw TError(nodePath().c_str(),mod->I18N("Link %d error!"),iid);

    //Change type link
    if( cmd == SET )
    {   
	if( lnk != m_lnk[iid].tp )
	{
	    //Free old structures
	    switch(m_lnk[iid].tp)
	    {
		case I_LOC: case I_GLB:	
		    delete m_lnk[iid].iblk;	break;
		case I_PRM: case O_PRM:
		    delete m_lnk[iid].prm;	break;
	    }
    
	    //Make new structures
	    switch(lnk)
	    {
		case I_LOC: case I_GLB:	
		    m_lnk[iid].iblk = new SLIBlk;	break;
		case I_PRM: case O_PRM:
		    m_lnk[iid].prm  = new SLPrm;	break;
	    }	    
	    m_lnk[iid].tp = lnk;
	}
	m_lnk[iid].lnk = vlnk;
    }
    //Connect new link and init
    if( cmd == INIT || process() )
    {
	string	lo1 = TSYS::strSepParse(m_lnk[iid].lnk,0,'.');
	string	lo2 = TSYS::strSepParse(m_lnk[iid].lnk,1,'.');
	string	lo3 = TSYS::strSepParse(m_lnk[iid].lnk,2,'.');
	string  lo4 = TSYS::strSepParse(m_lnk[iid].lnk,3,'.');
	
	switch(m_lnk[iid].tp)
	{
	    case I_LOC:
		if( owner().blkPresent(lo1) && owner().blkAt(lo1).at().ioId(lo2) >= 0 )
		{
		    m_lnk[iid].iblk->w_bl = owner().blkAt(lo1);
		    m_lnk[iid].iblk->w_id = m_lnk[iid].iblk->w_bl.at().ioId(lo2);
		}
		break;
	    case I_GLB:
		if( owner().owner().present(lo1) && 
		    ((Contr &)owner().owner().at(lo1).at()).blkPresent(lo2) && 
		    ((Contr &)owner().owner().at(lo1).at()).blkAt(lo2).at().ioId(lo3) >= 0 )
		{
		    m_lnk[iid].iblk->w_bl = ((Contr &)owner().owner().at(lo1).at()).blkAt(lo2);
		    m_lnk[iid].iblk->w_id = m_lnk[iid].iblk->w_bl.at().ioId(lo3);
		}
		break;
	    case I_PRM: case O_PRM:
		if( dynamic_cast<TVal *>(&SYS->nodeAt(m_lnk[iid].lnk,0,'.').at()) )
		{
		    m_lnk[iid].prm->w_prm = dynamic_cast<TValue *>(SYS->nodeAt(m_lnk[iid].lnk,0,'.').at().nodePrev());
		    m_lnk[iid].prm->w_atr = SYS->nodeAt(m_lnk[iid].lnk,0,'.').at().nodeName();
		}
		break;
	}				
    }
    //Disconnect
    if( cmd == DEINIT )
	switch(m_lnk[iid].tp)
	{
	    case I_LOC: case I_GLB:	
		m_lnk[iid].iblk->w_bl.free();	break;
	    case I_PRM: case O_PRM:
		m_lnk[iid].prm->w_prm.free();	break;
	}
}

void Block::calc( )
{
    //Get values from input links
    ResAlloc::resRequestR(lnk_res);
    try
    {
	for( unsigned i_ln=0; i_ln < m_lnk.size(); i_ln++ )
    	    switch( m_lnk[i_ln].tp )
    	    {
    		case I_LOC: case I_GLB:
		{
		    SLIBlk *lc = m_lnk[i_ln].iblk;
		    if( lc->w_bl.freeStat() || !lc->w_bl.at().enable() )	continue;
		    switch(ioType(i_ln))
		    {
			case IO::String:	setS(i_ln,lc->w_bl.at().getS(lc->w_id));	break;
			case IO::Integer:	setI(i_ln,lc->w_bl.at().getI(lc->w_id));	break;
			case IO::Real:		setR(i_ln,lc->w_bl.at().getR(lc->w_id));	break;
			case IO::Boolean:	setB(i_ln,lc->w_bl.at().getB(lc->w_id));	break;	    
		    }		
		    break;
	        }
		case I_PRM:
		{
		    if( m_lnk[i_ln].prm->w_prm.freeStat() )	continue;
		    AutoHD<TVal> pvl = m_lnk[i_ln].prm->w_prm.at().vlAt(m_lnk[i_ln].prm->w_atr);
		    switch(ioType(i_ln))
		    {
			case IO::String:	setS(i_ln,pvl.at().getS());	break;
			case IO::Integer:	setI(i_ln,pvl.at().getI());	break;
			case IO::Real:		setR(i_ln,pvl.at().getR());	break;
			case IO::Boolean:	setB(i_ln,pvl.at().getB());	break;
		    }
		    break;
		}
	    }
    }catch(TError err)
    {
	err_cnt++;
	ResAlloc::resReleaseR(lnk_res);
        throw TError(nodePath().c_str(),mod->I18N("Error read block's <%s> links."),id().c_str());
    }	
    ResAlloc::resReleaseR(lnk_res);
    	
    //Calc function
    try{ TValFunc::calc( ); }
    catch(TError err)
    { 	
	err_cnt++;
	throw TError(nodePath().c_str(),mod->I18N("Error calc block <%s>."),id().c_str());
    }
    
    //Put values to output links
    ResAlloc::resRequestR(lnk_res);
    try
    {
        for( unsigned i_ln=0; i_ln < m_lnk.size(); i_ln++ )
	    if( m_lnk[i_ln].tp == O_PRM && !m_lnk[i_ln].prm->w_prm.freeStat() )
	    {
		AutoHD<TVal> pvl = m_lnk[i_ln].prm->w_prm.at().vlAt(m_lnk[i_ln].prm->w_atr);
		switch(ioType(i_ln))
		{
		    case IO::String:	pvl.at().setS(getS(i_ln));	break;
		    case IO::Integer:	pvl.at().setI(getI(i_ln));	break;
		    case IO::Real:	pvl.at().setR(getR(i_ln));	break;
	    	    case IO::Boolean:	pvl.at().setB(getB(i_ln));	break;
		}
	    }
    }catch(TError err)
    {
	err_cnt++;
	ResAlloc::resReleaseR(lnk_res);
	throw TError(nodePath().c_str(),mod->I18N("Error write block's <%s> links."),id().c_str());
    }    
    ResAlloc::resReleaseR(lnk_res);
    err_cnt=0;
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
	    ctrMkNode("fld",opt,a_path.c_str(),"/blck/cfg/func",mod->I18N("Function"),(!func())?0664:0444,0,0,"str")->
		attr_("dest","sel_ed")->attr("select","/blck/cfg/fncs");
	    ctrMkNode("comm",opt,a_path.c_str(),"/blck/cfg/func_lnk",mod->I18N("Go to function"),0550,0,0,"lnk");
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
    			    func()->io(id)->name().c_str(),0664,0,0,"dec")->attr_("dest","select")->
    			    attr_("select",(ioMode(id) != IO::Input)?"/lio/otp":"/lio/itp");
			if( m_lnk[id].tp != FREE )
			    ctrMkNode("fld",opt,a_path.c_str(),(string("/lio/io/2|")+list[i_io]).c_str(),"",0664,0,0,"str")->
		    		attr_("dest","sel_ed")->attr_("select",(string("/lio/io/3|")+list[i_io]).c_str());
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
	    else if( a_path == "/blck/cfg/func" )	ctrSetS( opt, m_func );
	    else if( a_path == "/blck/cfg/func_lnk" )
	    {	
		int c_lv = 0;
		string path = "/";
		while(TSYS::strSepParse(m_func,c_lv,'.').size())
		    path+=TSYS::strSepParse(m_func,c_lv++,'.')+"/";
		opt->text(path);
	    }
	    else if( a_path == "/blck/cfg/fncs" )
	    {
		opt->childClean();
		int c_lv = 0;
                string c_path = "";
		ctrSetS( opt, c_path );
                while(TSYS::strSepParse(m_func,c_lv,'.').size())
                {
                    if( c_lv ) c_path+=".";
                    c_path = c_path+TSYS::strSepParse(m_func,c_lv,'.');
		    ctrSetS( opt, c_path );
                    c_lv++;
                }
		if(c_lv) c_path += ".";
		if( !dynamic_cast<TFunction *>(&SYS->nodeAt(c_path,0,'.').at()) )
		    SYS->nodeAt(c_path,0,'.').at().nodeList(list);
                for( unsigned i_a=0; i_a < list.size(); i_a++ )
            	    ctrSetS( opt, c_path+list[i_a]);
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
		    string lnk = m_lnk[id].lnk;
		
		    if( lev == '0' )
			switch(ioType(id))
			{
			    case IO::String:	ctrSetS( opt, getS(id) );	break;
			    case IO::Integer:	ctrSetI( opt, getI(id) );	break;
			    case IO::Real:    	ctrSetR( opt, getR(id) );	break;
			    case IO::Boolean: 	ctrSetB( opt, getB(id) );	break;
			}
		    else if( lev == '1' )	ctrSetI(opt,m_lnk[id].tp);
		    else if( lev == '2' )	ctrSetS(opt,lnk);
		    else if( lev == '3' )
		    {	
			opt->childClean();
			int c_lv = 0;
                        string c_path = "";
			
			ctrSetS( opt, c_path );    
            		while(TSYS::strSepParse(lnk,c_lv,'.').size())
            		{
                	    if( c_lv ) c_path+=".";
                	    c_path = c_path+TSYS::strSepParse(lnk,c_lv,'.');
			    ctrSetS( opt, c_path );
                	    c_lv++;
            		}
                	if(c_lv) c_path+=".";
			switch(m_lnk[id].tp)
			{
			    case I_LOC:
				switch(c_lv)
            			{
                		    case 0: owner().blkList(list); break;
				    case 1:
					if( owner().blkPresent(TSYS::strSepParse(lnk,0,'.'))
						&& owner().blkAt(TSYS::strSepParse(lnk,0,'.')).at().func() )
					    owner().blkAt(TSYS::strSepParse(lnk,0,'.')).at().ioList(list);
					break;
				}
				break;
			    case I_GLB:
				switch(c_lv)
				{
				    case 0: owner().owner().list(list); break;
				    case 1: 
					if( owner().owner().present(TSYS::strSepParse(lnk,0,'.')) )
					    ((Contr &)owner().owner().at(TSYS::strSepParse(lnk,0,'.')).at()).blkList(list);
					break;
				    case 2:
					if( owner().owner().present(TSYS::strSepParse(lnk,0,'.')) &&
						((Contr &)owner().owner().at(TSYS::strSepParse(lnk,0,'.')).at()).blkPresent(TSYS::strSepParse(lnk,1,'.')) )
					    ((Contr &)owner().owner().at(TSYS::strSepParse(lnk,0,'.')).at()).blkAt(TSYS::strSepParse(lnk,1,'.')).at().ioList(list);
					break;
				}
				break;
			    case I_PRM: case O_PRM:
				if( !dynamic_cast<TVal *>(&SYS->nodeAt(lnk,0,'.').at()) )
				    SYS->nodeAt(lnk,0,'.').at().nodeList(list);
				break;
			}
            		for( unsigned i_a=0; i_a < list.size(); i_a++ )
            		    ctrSetS( opt, c_path+list[i_a]);
		    }		    
		}
		else if( a_path == "/lio/itp" )
		{	
		    opt->childClean();
		    ctrSetS( opt, mod->I18N("Free"), 	TSYS::int2str(Block::FREE).c_str() );
		    ctrSetS( opt, mod->I18N("Local"), 	TSYS::int2str(Block::I_LOC).c_str() );
		    ctrSetS( opt, mod->I18N("Global"), 	TSYS::int2str(Block::I_GLB).c_str() );
		    ctrSetS( opt, mod->I18N("Parameter"), 	TSYS::int2str(Block::I_PRM).c_str() );
		}
		else if( a_path == "/lio/otp" )
		{
		    opt->childClean();
		    ctrSetS( opt, mod->I18N("Free"),	TSYS::int2str(Block::FREE).c_str() );
		    ctrSetS( opt, mod->I18N("Parameter"),	TSYS::int2str(Block::O_PRM).c_str() );
		}   
		else throw TError(nodePath().c_str(),mod->I18N("Branch <%s> error!"),a_path.c_str());
	    }	    
	    else throw TError(nodePath().c_str(),mod->I18N("Branch <%s> error!"),a_path.c_str());
	    break;
	case TCntrNode::Set:
	    if( a_path == "/blck/st/en" )       	enable(ctrGetB(opt));
	    else if( a_path == "/blck/st/prc" )		process(ctrGetB(opt));
	    else if( a_path == "/blck/cfg/name" )	name(ctrGetS(opt));
	    else if( a_path == "/blck/cfg/descr" )	descr(ctrGetS(opt));
	    else if( a_path == "/blck/cfg/toen" )       m_to_en = ctrGetB(opt);
	    else if( a_path == "/blck/cfg/toprc" )      m_to_prc = ctrGetB(opt);	    
	    else if( a_path == "/blck/cfg/func" )  	m_func = ctrGetS( opt );
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
			else if(ioType(id) == IO::Real)		setR(id, ctrGetR( opt ));
			else if(ioType(id) == IO::Boolean)	setB(id, ctrGetB( opt ));	
		    }	
		    else if( lev == '1' )	link(id,SET,(Block::LnkT)ctrGetI(opt) );
		    else if( lev == '2' )	link(id,SET,m_lnk[id].tp,ctrGetS(opt));
		}
		else throw TError(nodePath().c_str(),mod->I18N("Branch <%s> error!"),a_path.c_str());
	    }
	    else throw TError(nodePath().c_str(),mod->I18N("Branch <%s> error!"),a_path.c_str());
	    break;
    }    
}
