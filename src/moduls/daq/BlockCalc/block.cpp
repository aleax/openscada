
//OpenSCADA system module DAQ.BlockCalc file: block.cpp
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
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

bool Block::m_sw_hide = false;

//Function block
Block::Block( const string &iid, Contr *iown ) : 
    TCntrNode(iown), TConfig( &((TipContr &)iown->owner()).blockE() ), TValFunc(iid+"_block",NULL), 
    m_enable(false), m_process(false),
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

Block &Block::operator=(Block &blk)
{
    //- Copy parameters -
    string prev_id = m_id;
    *(TConfig *)this = (TConfig&)blk;
    if( !prev_id.empty() ) m_id = prev_id;
    //- Copy IO and links -
    if( blk.enable() )
    {
	enable(true);
	loadIO(blk.owner().cfg("BLOCK_SH").getS(),blk.id());    
    }    
}

void Block::postDisable(int flag)
{
    try
    {
        if( flag )
        {
	    //Delete block from BD
            string tbl = owner().genBD()+"."+owner().cfg("BLOCK_SH").getS();
	    SYS->db().at().dataDel(tbl,mod->nodePath()+owner().cfg("BLOCK_SH").getS(),*this);
	    
	    //Delete block's IO from BD
	    TConfig cfg(&owner().owner().blockIOE());
	    tbl=tbl+"_io";
	    cfg.cfg("BLK_ID").setS(id());   //Delete all block id records
            cfg.cfg("ID").setS("");			    
	    SYS->db().at().dataDel(tbl,mod->nodePath()+owner().cfg("BLOCK_SH").getS()+"_io",cfg);
        }	
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Error,err.mess.c_str()); }
}

string Block::name()
{
    return m_name.size()?m_name:id();
}

void Block::load( )
{   
    string bd = owner().genBD()+"."+owner().cfg("BLOCK_SH").getS();
    SYS->db().at().dataGet(bd,mod->nodePath()+owner().cfg("BLOCK_SH").getS(),*this);
     
    //Load IO config
    loadIO();
}

void Block::save( )
{
    string bd = owner().genBD()+"."+owner().cfg("BLOCK_SH").getS();
    SYS->db().at().dataSet(bd,mod->nodePath()+owner().cfg("BLOCK_SH").getS(),*this);
    
    //Save IO config
    saveIO();
}
		
void Block::loadIO( const string &blk_sh, const string &blk_id )
{    
    if( !func() ) return;
    
    TConfig cfg(&mod->blockIOE());
    cfg.cfg("BLK_ID").setS((blk_id.size())?blk_id:id());
    string bd_tbl = ((blk_sh.size())?blk_sh:owner().cfg("BLOCK_SH").getS())+"_io";
    string bd = owner().genBD()+"."+bd_tbl;
    
    for( int i_ln = 0; i_ln < m_val.size(); i_ln++ )
    {    
	if( i_ln >= m_lnk.size() )
	{  
	    m_lnk.push_back( SLnk() );
	    m_lnk[i_ln].tp = FREE;
	}	    
    
	cfg.cfg("ID").setS(func()->io(i_ln)->id());    
	if(!SYS->db().at().dataGet(bd,mod->nodePath()+bd_tbl,cfg))
	    continue;
    	//Value
	setS(i_ln,cfg.cfg("VAL").getS());
	//Config of link
	link(i_ln,SET,(LnkT)cfg.cfg("TLNK").getI(),cfg.cfg("LNK").getS());
    }
}

void Block::saveIO( )
{
    if( !enable() )      return;
	
    TConfig cfg(&mod->blockIOE());
    cfg.cfg("BLK_ID").setS(id());
    string bd_tbl = owner().cfg("BLOCK_SH").getS()+"_io";
    string bd = owner().genBD()+"."+bd_tbl;
    
    for( int i_ln = 0; i_ln < m_lnk.size(); i_ln++ )
        try
	{ 	
	    cfg.cfg("ID").setS(func()->io(i_ln)->id());		    
	    cfg.cfg("TLNK").setI(m_lnk[i_ln].tp);	//Type link
	    cfg.cfg("LNK").setS((m_lnk[i_ln].tp == FREE)?"":m_lnk[i_ln].lnk);	//Link
	    cfg.cfg("VAL").setS(getS(i_ln));		//Value	    
	    
	    SYS->db().at().dataSet(bd,mod->nodePath()+bd_tbl,cfg);
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
		    delete m_lnk[iid].aprm;	break;
	    }
    
	    //Make new structures
	    switch(lnk)
	    {
		case I_LOC: case I_GLB:	
		    m_lnk[iid].iblk = new SLIBlk;	break;
		case I_PRM: case O_PRM:
		    m_lnk[iid].aprm = new AutoHD<TVal>;	break;
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
		    *m_lnk[iid].aprm = SYS->nodeAt(m_lnk[iid].lnk,0,'.');
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
		m_lnk[iid].aprm->free();	break;
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
		    if( !m_lnk[i_ln].iblk->w_bl.freeStat() && m_lnk[i_ln].iblk->w_bl.at().enable() )
			switch(ioType(i_ln))
			{
			    case IO::String:	setS(i_ln,m_lnk[i_ln].iblk->w_bl.at().getS(m_lnk[i_ln].iblk->w_id));	break;
			    case IO::Integer:	setI(i_ln,m_lnk[i_ln].iblk->w_bl.at().getI(m_lnk[i_ln].iblk->w_id));	break;
			    case IO::Real:	setR(i_ln,m_lnk[i_ln].iblk->w_bl.at().getR(m_lnk[i_ln].iblk->w_id));	break;
			    case IO::Boolean:	setB(i_ln,m_lnk[i_ln].iblk->w_bl.at().getB(m_lnk[i_ln].iblk->w_id));	break;
			}		
		    break;
		case I_PRM:
		    if( !m_lnk[i_ln].aprm->freeStat() )
			switch(ioType(i_ln))
			{
			    case IO::String:	setS(i_ln,m_lnk[i_ln].aprm->at().getS());	break;
			    case IO::Integer:	setI(i_ln,m_lnk[i_ln].aprm->at().getI());	break;
			    case IO::Real:	setR(i_ln,m_lnk[i_ln].aprm->at().getR());	break;
			    case IO::Boolean:	setB(i_ln,m_lnk[i_ln].aprm->at().getB());	break;
			}
		    break;
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
	    if( m_lnk[i_ln].tp == O_PRM && !m_lnk[i_ln].aprm->freeStat() )
		switch(ioType(i_ln))
		{
		    case IO::String:	m_lnk[i_ln].aprm->at().setS(getS(i_ln));	break;
		    case IO::Integer:	m_lnk[i_ln].aprm->at().setI(getI(i_ln));	break;
		    case IO::Real:	m_lnk[i_ln].aprm->at().setR(getR(i_ln));	break;
	    	    case IO::Boolean:	m_lnk[i_ln].aprm->at().setB(getB(i_ln));	break;
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
	    TCntrNode::cntrCmd_(a_path,opt,cmd);
	
	    ctrMkNode("oscada_cntr",opt,-1,a_path.c_str(),"/",mod->I18N("Block: ")+id());
	    ctrMkNode("area",opt,-1,a_path.c_str(),"/blck",mod->I18N("Block"));
	    ctrMkNode("area",opt,-1,a_path.c_str(),"/blck/st",mod->I18N("State"));
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/blck/st/en",mod->I18N("Enable"),0664,0,0,1,"tp","bool");
	    if( owner().startStat() )
		ctrMkNode("fld",opt,-1,a_path.c_str(),"/blck/st/prc",mod->I18N("Process"),0664,0,0,1,"tp","bool");
	    ctrMkNode("area",opt,-1,a_path.c_str(),"/blck/cfg",mod->I18N("Config"));
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/blck/cfg/id",mod->I18N("Id"),0444,0,0,1,"tp","str");
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/blck/cfg/name",mod->I18N("Name"),0664,0,0,1,"tp","str");
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/blck/cfg/descr",mod->I18N("Description"),0664,0,0,3,"tp","str","cols","90","rows","4");
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/blck/cfg/toen",mod->I18N("To enable"),0664,0,0,1,"tp","bool");
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/blck/cfg/toprc",mod->I18N("To process"),0664,0,0,1,"tp","bool");
	    ctrMkNode("fld",opt,-1,a_path.c_str(),"/blck/cfg/func",mod->I18N("Function"),(!func())?0664:0444,0,0,3,"tp","str","dest","sel_ed","select","/blck/cfg/fncs");
	    ctrMkNode("comm",opt,-1,a_path.c_str(),"/blck/cfg/func_lnk",mod->I18N("Go to function"),0550,0,0,1,"tp","lnk");
	    ctrMkNode("comm",opt,-1,a_path.c_str(),"/blck/cfg/load",mod->I18N("Load"),0550);
	    ctrMkNode("comm",opt,-1,a_path.c_str(),"/blck/cfg/save",mod->I18N("Save"),0550);
	    if( enable() )
	    {
		ctrMkNode("area",opt,-1,a_path.c_str(),"/lio",mod->I18N("IO"));
		ctrMkNode("area",opt,-1,a_path.c_str(),"/lio/show",mod->I18N("Show"));
		ctrMkNode("fld",opt,-1,a_path.c_str(),"/lio/show/hide",mod->I18N("Hiden"),0664,0,0,1,"tp","bool");
		ctrMkNode("area",opt,-1,a_path.c_str(),"/lio/io",mod->I18N("IO"));
		vector<string> list;
	        ioList(list);
	        for( int i_io = 0; i_io < list.size(); i_io++ )
	        {
	            int id = ioId(list[i_io]);
	            if( ioHide(id) && !m_sw_hide ) continue;
		    char *tip;
		    switch(ioType(id))
		    {
		        case IO::String:    tip = "str";    break;
		        case IO::Integer:   tip = "dec";    break;
		        case IO::Real:      tip = "real";   break;
		        case IO::Boolean:   tip = "bool";   break;
		    }
		    ctrMkNode("fld",opt,-1,a_path.c_str(),(string("/lio/io/")+list[i_io]).c_str(),
                                    func()->io(id)->name().c_str(),(m_lnk[id].tp != FREE)?0444:0664,0,0,1,"tp",tip);
                }
		ctrMkNode("area",opt,-1,a_path.c_str(),"/lnk",mod->I18N("Links"));
		ctrMkNode("area",opt,-1,a_path.c_str(),"/lnk/show",mod->I18N("Show"));
		ctrMkNode("fld",opt,-1,a_path.c_str(),"/lnk/show/hide",mod->I18N("Hiden"),0664,0,0,1,"tp","bool");
		ctrMkNode("area",opt,-1,a_path.c_str(),"/lnk/io",mod->I18N("IO"));
		
		//Put IO links
		for( int i_io = 0; i_io < list.size(); i_io++ )
		{
		    int id = ioId(list[i_io]);
	    
		    if( ioHide(id) && !m_sw_hide ) continue;		
		
    		    //Add link's type	
    		    ctrMkNode("fld",opt,-1,a_path.c_str(),(string("/lnk/io/1|")+list[i_io]).c_str(),
    			func()->io(id)->name().c_str(),0664,0,0,3,"tp","dec","dest","select","select",(ioMode(id) != IO::Input)?"/lnk/otp":"/lnk/itp");
		    if( m_lnk[id].tp != FREE )
		        ctrMkNode("fld",opt,-1,a_path.c_str(),(string("/lnk/io/2|")+list[i_io]).c_str(),"",0664,0,0,3,"tp","str","dest","sel_ed","select",(string("/lnk/io/3|")+list[i_io]).c_str());
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
		if( a_path == "/lio/show/hide" || a_path == "/lnk/show/hide" )	ctrSetB( opt, m_sw_hide );
		else if( a_path.substr(0,7) == "/lio/io" )
		{
		    int id = ioId(TSYS::pathLev(a_path,2));
		    switch(ioType(id))
	            {
	                case IO::String:    ctrSetS( opt, getS(id) );       break;
	        	case IO::Integer:   ctrSetI( opt, getI(id) );       break;
			case IO::Real:      ctrSetR( opt, getR(id) );       break;
			case IO::Boolean:   ctrSetB( opt, getB(id) );       break;
		    }
		}		
		else if( a_path.substr(0,7) == "/lnk/io" )
		{
		    char lev = TSYS::pathLev(a_path,2)[0];
		    int id   = ioId(TSYS::pathLev(a_path,2).substr(2));
		    string lnk = m_lnk[id].lnk;
		
		    if( lev == '1' )		ctrSetI(opt,m_lnk[id].tp);
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
		else if( a_path == "/lnk/itp" )
		{	
		    opt->childClean();
		    ctrSetS( opt, mod->I18N("Free"), 	TSYS::int2str(Block::FREE).c_str() );
		    ctrSetS( opt, mod->I18N("Local"), 	TSYS::int2str(Block::I_LOC).c_str() );
		    ctrSetS( opt, mod->I18N("Global"), 	TSYS::int2str(Block::I_GLB).c_str() );
		    ctrSetS( opt, mod->I18N("Parameter"),TSYS::int2str(Block::I_PRM).c_str() );
		}
		else if( a_path == "/lnk/otp" )
		{
		    opt->childClean();
		    ctrSetS( opt, mod->I18N("Free"),	TSYS::int2str(Block::FREE).c_str() );
		    ctrSetS( opt, mod->I18N("Parameter"),TSYS::int2str(Block::O_PRM).c_str() );
		}   
		else TCntrNode::cntrCmd_(a_path,opt,cmd);
	    }	    
	    else TCntrNode::cntrCmd_(a_path,opt,cmd);
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
		if( a_path == "/lio/show/hide" || a_path == "/lnk/show/hide" )	m_sw_hide = ctrGetB( opt );
		else if( a_path.substr(0,7) == "/lio/io" )
		{
		    int id = ioId(TSYS::pathLev(a_path,2));
		    switch(ioType(id))
		    {
			case IO::String:	setS(id, ctrGetS( opt )); break;
			case IO::Integer:	setI(id, ctrGetI( opt )); break;
			case IO::Real:		setR(id, ctrGetR( opt )); break;
			case IO::Boolean:	setB(id, ctrGetB( opt )); break;
		    }
		}
		else if( a_path.substr(0,7) == "/lnk/io" )	    
		{
		    char lev = TSYS::pathLev(a_path,2)[0];
		    int id = ioId(TSYS::pathLev(a_path,2).substr(2));
		    if( lev == '1' )		link(id,SET,(Block::LnkT)ctrGetI(opt));
		    else if( lev == '2' )	link(id,SET,m_lnk[id].tp,ctrGetS(opt));
		}
		else TCntrNode::cntrCmd_(a_path,opt,cmd);
	    }
	    else TCntrNode::cntrCmd_(a_path,opt,cmd);
	    break;
    }    
}
