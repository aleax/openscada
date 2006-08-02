
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
#include <math.h>

#include "virtual.h"
#include "block.h"

using namespace Virtual;

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

void Block::preDisable(int flag)
{
    if( process() ) process(false);
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
    { Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str()); }
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
        catch(TError err)
	{ 
	    Mess->put(err.cat.c_str(),TMess::Error,"%s",err.mess.c_str());
	    Mess->put(nodePath().c_str(),TMess::Error,mod->I18N("Block link <%s> save error."),func()->io(i_ln)->id().c_str());
	}
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

void Block::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
	ctrMkNode("oscada_cntr",opt,-1,"/",mod->I18N("Block: ")+id());
	ctrMkNode("area",opt,-1,"/blck",mod->I18N("Block"));
	ctrMkNode("area",opt,-1,"/blck/st",mod->I18N("State"));
	ctrMkNode("fld",opt,-1,"/blck/st/en",mod->I18N("Enable"),0664,"root","root",1,"tp","bool");
	if( owner().startStat() )
	    ctrMkNode("fld",opt,-1,"/blck/st/prc",mod->I18N("Process"),0664,"root","root",1,"tp","bool");
	ctrMkNode("area",opt,-1,"/blck/cfg",mod->I18N("Config"));
	ctrMkNode("fld",opt,-1,"/blck/cfg/id",mod->I18N("Id"),0444,"root","root",1,"tp","str");
	ctrMkNode("fld",opt,-1,"/blck/cfg/name",mod->I18N("Name"),0664,"root","root",1,"tp","str");
	ctrMkNode("fld",opt,-1,"/blck/cfg/descr",mod->I18N("Description"),0664,"root","root",3,"tp","str","cols","90","rows","4");
	ctrMkNode("fld",opt,-1,"/blck/cfg/toen",mod->I18N("To enable"),0664,"root","root",1,"tp","bool");
	ctrMkNode("fld",opt,-1,"/blck/cfg/toprc",mod->I18N("To process"),0664,"root","root",1,"tp","bool");
	ctrMkNode("fld",opt,-1,"/blck/cfg/func",mod->I18N("Function"),(!func())?0664:0444,"root","root",3,"tp","str","dest","sel_ed","select","/blck/cfg/fncs");
	ctrMkNode("comm",opt,-1,"/blck/cfg/func_lnk",mod->I18N("Go to function"),0440,"root","root",1,"tp","lnk");
	ctrMkNode("comm",opt,-1,"/blck/cfg/load",mod->I18N("Load"),0440);
	ctrMkNode("comm",opt,-1,"/blck/cfg/save",mod->I18N("Save"),0440);
	if( enable() )
	{
	    ctrMkNode("area",opt,-1,"/lio",mod->I18N("IO"));
	    ctrMkNode("area",opt,-1,"/lio/show",mod->I18N("Show"));
	    ctrMkNode("fld",opt,-1,"/lio/show/hide",mod->I18N("Hiden"),0664,"root","root",1,"tp","bool");
	    ctrMkNode("area",opt,-1,"/lio/io",mod->I18N("IO"));
	    vector<string> list;
	    ioList(list);
	    for( int i_io = 0; i_io < list.size(); i_io++ )
	    {
	        int id = ioId(list[i_io]);
	        if( ioHide(id) && !atoi(TBDS::genDBGet(nodePath()+"showHide","0",opt->attr("user")).c_str()) ) continue;
	        char *tip;
	        switch(ioType(id))
	        {
	            case IO::String:    tip = "str";    break;
	            case IO::Integer:   tip = "dec";    break;
	            case IO::Real:      tip = "real";   break;
	            case IO::Boolean:   tip = "bool";   break;
	        }
	        ctrMkNode("fld",opt,-1,(string("/lio/io/")+list[i_io]).c_str(),
                        func()->io(id)->name().c_str(),(m_lnk[id].tp != FREE)?0444:0664,"root","root",1,"tp",tip);
            }
	    ctrMkNode("area",opt,-1,"/lnk",mod->I18N("Links"));
	    ctrMkNode("area",opt,-1,"/lnk/show",mod->I18N("Show"));
	    ctrMkNode("fld",opt,-1,"/lnk/show/hide",mod->I18N("Hiden"),0664,"root","root",1,"tp","bool");
	    ctrMkNode("area",opt,-1,"/lnk/io",mod->I18N("IO"));
		
	    //Put IO links
	    for( int i_io = 0; i_io < list.size(); i_io++ )
	    {
	        int id = ioId(list[i_io]);
	    
	        if( ioHide(id) && !atoi(TBDS::genDBGet(nodePath()+"showHide","0",opt->attr("user")).c_str()) ) continue;		
		
    	        //Add link's type	
    	        ctrMkNode("fld",opt,-1,(string("/lnk/io/1|")+list[i_io]).c_str(),
    	    	    func()->io(id)->name().c_str(),0664,"root","root",3,"tp","dec","dest","select","select",(ioMode(id) != IO::Input)?"/lnk/otp":"/lnk/itp");
		if( m_lnk[id].tp != FREE )
		    ctrMkNode("fld",opt,-1,(string("/lnk/io/2|")+list[i_io]).c_str(),"",0664,"root","root",3,"tp","str","dest","sel_ed","select",(string("/lnk/io/3|")+list[i_io]).c_str());
	    }
	}
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/blck/st/en" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(enable()?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	enable(atoi(opt->text().c_str()));
    }
    else if( a_path == "/blck/st/prc" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(process()?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	process(atoi(opt->text().c_str()));
    }
    else if( a_path == "/blck/cfg/id" && ctrChkNode(opt) )	opt->text(id());
    else if( a_path == "/blck/cfg/name" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(name());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	name(opt->text());
    }
    else if( a_path == "/blck/cfg/descr" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(descr());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	descr(opt->text());
    }
    else if( a_path == "/blck/cfg/toen" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(m_to_en?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_to_en = atoi(opt->text().c_str());
    }
    else if( a_path == "/blck/cfg/toprc" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(m_to_prc?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_to_prc = atoi(opt->text().c_str());
    }
    else if( a_path == "/blck/cfg/func" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(m_func);
	if( !func() && ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	m_func = opt->text();
    }
    else if( a_path == "/blck/cfg/func_lnk" && ctrChkNode(opt,"get",0440,"root","root",SEQ_RD) )
    {	
	int c_lv = 0;
	string path = "/";
	while(TSYS::strSepParse(m_func,c_lv,'.').size())
	    path+=TSYS::strSepParse(m_func,c_lv++,'.')+"/";
	opt->text(path);
    }
    else if( a_path == "/blck/cfg/fncs" && ctrChkNode(opt) )
    {
	vector<string> list;
        int c_lv = 0;
        string c_path = "";
        while(TSYS::strSepParse(m_func,c_lv,'.').size())
        {
            opt->childAdd("el")->text(c_path);
            if( c_lv ) c_path+=".";
            c_path = c_path+TSYS::strSepParse(m_func,c_lv,'.');
            c_lv++;
        }
        opt->childAdd("el")->text(c_path);
        if( c_lv != 0 ) c_path += ".";
    	    SYS->nodeAt(c_path,0,'.').at().nodeList(list);
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    opt->childAdd("el")->text(c_path+list[i_a]);
    }
    else if( (a_path == "/lio/show/hide" || a_path == "/lnk/show/hide") && enable() )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(TBDS::genDBGet(nodePath()+"showHide","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	TBDS::genDBSet(nodePath()+"showHide",opt->text(),opt->attr("user"));
    }
    else if( a_path.substr(0,7) == "/lio/io" && enable() )
    {
	int id = ioId(TSYS::pathLev(a_path,2));
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->text(getS(id));
	if( m_lnk[id].tp == FREE && ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setS(id,opt->text());
    }
    else if( a_path.substr(0,7) == "/lnk/io" && enable() )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    vector<string> list;
	    char lev = TSYS::pathLev(a_path,2)[0];
	    int id   = ioId(TSYS::pathLev(a_path,2).substr(2));
	    string lnk = m_lnk[id].lnk;
		
	    if( lev == '1' )		opt->text(TSYS::int2str(m_lnk[id].tp));
	    else if( lev == '2' )	opt->text(lnk);
	    else if( lev == '3' )
	    {	
		int c_lv = 0;
                string c_path = "";
			
		opt->childAdd("el")->text(c_path);    
            	while(TSYS::strSepParse(lnk,c_lv,'.').size())
            	{
                    if( c_lv ) c_path+=".";
                    c_path = c_path+TSYS::strSepParse(lnk,c_lv,'.');
		    opt->childAdd("el")->text(c_path);
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
		        SYS->nodeAt(lnk,0,'.').at().nodeList(list);
			break;
		}
            	for( unsigned i_a=0; i_a < list.size(); i_a++ )
            	    opt->childAdd("el")->text(c_path+list[i_a]);
	    }
	}
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )    
	{
	    char lev = TSYS::pathLev(a_path,2)[0];
            int id = ioId(TSYS::pathLev(a_path,2).substr(2));
            if( lev == '1' )            link(id,SET,(Block::LnkT)atoi(opt->text().c_str()));
	    else if( lev == '2' )	link(id,SET,m_lnk[id].tp,opt->text());
	}	    		    
    }
    else if( a_path == "/lnk/itp" && enable() && ctrChkNode(opt) )
    {
	opt->childAdd("el")->attr("id",TSYS::int2str(Block::FREE))->text(mod->I18N("Free"));
	opt->childAdd("el")->attr("id",TSYS::int2str(Block::I_LOC))->text(mod->I18N("Local"));
	opt->childAdd("el")->attr("id",TSYS::int2str(Block::I_GLB))->text(mod->I18N("Global"));
	opt->childAdd("el")->attr("id",TSYS::int2str(Block::I_PRM))->text(mod->I18N("Parameter"));
    }
    else if( a_path == "/lnk/otp" && enable() && ctrChkNode(opt) )
    {
	opt->childAdd("el")->attr("id",TSYS::int2str(Block::FREE))->text(mod->I18N("Free"));
	opt->childAdd("el")->attr("id",TSYS::int2str(Block::O_PRM))->text(mod->I18N("Parameter"));
    }   
}
