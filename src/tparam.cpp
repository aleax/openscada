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

#include "resalloc.h"
#include "tsys.h"
#include "tparam.h"

TParam::TParam( const string &iid, TElem *cf_el ) : 
    m_en(false), p_el("w_attr"), prm_refl(NULL), TConfig(cf_el), m_wmode(TParam::Clear),
    m_id(cfg("SHIFR").getSd()), m_name(cfg("NAME").getSd()), m_descr(cfg("DESCR").getSd()), 
    m_aen(cfg("EN").getBd()), m_mode(cfg("MODE").getId()), m_prm(cfg("PRM").getSd())
{    
    m_id = m_name = iid;
}

TParam::~TParam(  )
{

}

void TParam::preDisable(int flag)
{
    vlDetElem(&p_el);
    vlDetElem(&SYS->controller().at().errE());
}

void TParam::postDisable(int flag)
{
    try
    {
	if( flag )
            SYS->db().at().dataDel(SYS->param().at().prmB(),SYS->param().at().nodePath()+"Prm/",*this);
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str()); }
}				    

void TParam::postEnable( )
{
    vlCfg(this);	//Mirrore config to atributes
    vlAttElem(&SYS->controller().at().errE());
    vlAttElem(&p_el);
}

void TParam::load( )
{
    SYS->db().at().dataGet(SYS->param().at().prmB(),SYS->param().at().nodePath()+"Prm/",*this);
    mode(mode(),m_prm);
    loadIO();    
}

void TParam::loadIO()
{
    //Load IO and init links
    if( mode() == TParam::Template )
    {
	TConfig cfg(&owner().prmIOE());
        cfg.cfg("PRM_ID").setS(id());
        TBDS::SName io_bd = SYS->param().at().prmB();
        io_bd.tbl += "_io";
	    
	AutoHD<TPrmTempl> w_tpl = owner().tplAt(m_wprm);
	
	for( int i_io = 0; i_io < tmpl->val.func()->ioSize(); i_io++ )
	{
	    int attr_id = w_tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
	    if( attr_id < 0 ) continue;
	    if( w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst ||
		w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkIn ||
		w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkOut ||
                (w_tpl.at().attr(attr_id).accs == TPrmTempl::Const &&
                 w_tpl.at().attr(attr_id).attr == TPrmTempl::FullAccess) )
	    try
	    {
		cfg.cfg("ID").setS(tmpl->val.func()->io(i_io)->id());
		SYS->db().at().dataGet(io_bd,SYS->param().at().nodePath()+"PrmIO/",cfg);
	    	if( w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst ||
			w_tpl.at().attr(attr_id).accs == TPrmTempl::Const )
	    	    tmpl->val.setS(i_io,cfg.cfg("VALUE").getS());
	    	else lnk(lnkId(i_io)).prm_attr = cfg.cfg("VALUE").getS();
	    }catch(TError err){ }
	}
	initTmplLnks();
    }
}

void TParam::save( )
{
    SYS->db().at().dataSet(SYS->param().at().prmB(),SYS->param().at().nodePath()+"Prm/",*this);
    saveIO();
}

void TParam::saveIO()
{
    //Save IO and init links
    if( mode() == TParam::Template )
    {
	TConfig cfg(&owner().prmIOE());
        cfg.cfg("PRM_ID").setS(id());
        TBDS::SName io_bd = SYS->param().at().prmB();
        io_bd.tbl += "_io";
	
	AutoHD<TPrmTempl> w_tpl = owner().tplAt(m_wprm);
	
	for( int i_io = 0; i_io < tmpl->val.func()->ioSize(); i_io++ )
	{
	    int attr_id = w_tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
	    if( attr_id < 0 ) continue;
    	    if( w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst ||
	        w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkIn ||
	        w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkOut ||
		(w_tpl.at().attr(attr_id).accs == TPrmTempl::Const && 
		 w_tpl.at().attr(attr_id).attr == TPrmTempl::FullAccess) )
	    {
		cfg.cfg("ID").setS(tmpl->val.func()->io(i_io)->id());
		if( w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst ||
			w_tpl.at().attr(attr_id).accs == TPrmTempl::Const )
                    cfg.cfg("VALUE").setS(tmpl->val.getS(i_io));
                else cfg.cfg("VALUE").setS(lnk(lnkId(i_io)).prm_attr);
		SYS->db().at().dataSet(io_bd,SYS->param().at().nodePath()+"PrmIO/",cfg);
	    }
	}
    }
}

void TParam::enable()
{
    vector<string> list;

    if( enableStat() )	return;
    m_en = true;
    mode((TParam::Mode)m_mode,m_prm);
    loadIO();
}

void TParam::disable()
{
    if( !enableStat() )	return;
    //saveIO();
    mode(TParam::Clear);    
    m_en = false;
}

void TParam::mode( TParam::Mode md, const string &prm )
{
    vector<string> list;
    
    if( !enableStat() )	return;

    try
    {
	//Free old mode
	if( md != mode() || prm != m_wprm )
	{
	    if( mode() == TParam::DirRefl )
	    {
        	while(p_el.fldSize())   p_el.fldDel(0);
        	delete prm_refl;
		prm_refl = NULL;
	    }
	    else if( mode() == TParam::Template )
	    {
		owner().prmCalc( id(), false );	//Get from process
		while(p_el.fldSize())   p_el.fldDel(0);
		delete tmpl;
		tmpl = NULL;
	    }
	}
    	
	//Init/update new mode
	if( md == TParam::DirRefl )
	{
	    if( !prm_refl ) prm_refl = new AutoHD<TParamContr>;
	    try
	    {
	    	prm_refl->free();
	    	*prm_refl = SYS->controller().at().at(TSYS::strSepParse(prm,0,'.')).at().at(TSYS::strSepParse(prm,1,'.')).at().at(TSYS::strSepParse(prm,2,'.'));
	    	prm_refl->at().vlList(list);
	    	for( int i_l = 0; i_l < list.size(); i_l++ )
		    if( !vlPresent(list[i_l]) )
		    	p_el.fldAdd( new TFld(list[i_l].c_str(),prm_refl->at().vlAt(list[i_l]).at().fld().descr().c_str(),
			    prm_refl->at().vlAt(list[i_l]).at().fld().type(),FLD_DWR|FLD_DRD|(prm_refl->at().vlAt(list[i_l]).at().fld().flg())&FLD_NWR) );
	    }catch(TError err)
	    { 
		m_wmode = Clear; 
		delete prm_refl;
	        prm_refl = NULL;
		throw;
	    }
	}
	else if( md == TParam::Template )
	{
	    bool to_make = false;
	    if( !tmpl )	tmpl = new STmpl;
	    try
	    {
	    	AutoHD<TPrmTempl> w_tpl = owner().tplAt(prm);
	    	if( !tmpl->val.func() )
	    	{
		    tmpl->val.func(&w_tpl.at().func().at());
		    to_make = true;
		}
		//Set mode
		m_wmode = md;
		//Init attrubutes
		for( int i_io = 0; i_io < tmpl->val.func()->ioSize(); i_io++ )
		{
		    int attr_id = w_tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
		    if( attr_id < 0 )	continue;
		    if( w_tpl.at().attr(attr_id).attr != TPrmTempl::NoAttr && 
			!vlPresent(tmpl->val.func()->io(i_io)->id()) )
		    {
			TFld::Type tp;
			unsigned char flg = FLD_DWR|FLD_DRD;
		    
			switch( tmpl->val.ioType(i_io) )
			{
			    case IO::String:	tp = TFld::String;      break;
			    case IO::Integer:	tp = TFld::Dec;         break;
			    case IO::Real: 	tp = TFld::Real;        break;
			    case IO::Boolean:	tp = TFld::Bool;        break;
			}
			if( w_tpl.at().attr(attr_id).attr == TPrmTempl::ReadOnly )
			    flg|=FLD_NWR;
			p_el.fldAdd( new TFld(tmpl->val.func()->io(i_io)->id().c_str(),tmpl->val.func()->io(i_io)->name().c_str(),tp,flg) );
		    }
		    if( (w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkIn || 
			    w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkOut) && lnkId(i_io) < 0 )
			tmpl->lnk.push_back(SLnk(i_io,w_tpl.at().attr(attr_id).accs));
		    if( to_make && (w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst || 
				    w_tpl.at().attr(attr_id).accs == TPrmTempl::Const ) )
			tmpl->val.setS(i_io,w_tpl.at().attr(attr_id).val);
		}
		//Init links
		initTmplLnks();
	    
		//Set to process
		owner().prmCalc(id(),true);
	    }catch(TError err)
	    { 
		m_wmode = Clear; 
		delete tmpl;
	        tmpl = NULL;
		throw;
	    }
	}
	m_wmode = md;
	m_wprm = prm;
    }catch(TError err){ Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str()); }
}

void TParam::initTmplLnks()
{
    if( mode() != TParam::Template )	return;
    //Init links
    for( int i_l = 0; i_l < lnkSize(); i_l++ )
    {
	try
	{
	    lnk(i_l).prm.free();
	    lnk(i_l).prm = SYS->controller().at().at(TSYS::strSepParse(lnk(i_l).prm_attr,0,'.')).at().at(TSYS::strSepParse(lnk(i_l).prm_attr,1,'.')).at().at(TSYS::strSepParse(lnk(i_l).prm_attr,2,'.'));
	    if( lnk(i_l).prm.at().vlPresent(TSYS::strSepParse(lnk(i_l).prm_attr,3,'.')) )
	    {
		lnk(i_l).attr = TSYS::strSepParse(lnk(i_l).prm_attr,3,'.');
		//Init value
		tmpl->val.setS(lnk(i_l).io_id,lnk(i_l).prm.at().vlAt(lnk(i_l).attr).at().getS());
	    }			
    	    else lnk(i_l).prm.free();
	}catch(TError err){ }
    }
}

void TParam::vlGet( TVal &val )
{
    if( !enableStat() )
    {
	if( val.name() == "err" )
	    val.setB(true,NULL,true);
	else if( val.name() == "err_mess" )
	    val.setS(Mess->I18N("Parameter had disabled."),NULL,true);
	return;
    }    
    try
    {
	if( mode() == TParam::DirRefl )
	    switch(val.fld().type())
    	    {
	        case TFld::String:
	    	    val.setS(prm_refl->at().vlAt(val.name()).at().getS(),NULL,true);
                    break;
                case TFld::Dec:
            	    val.setI(prm_refl->at().vlAt(val.name()).at().getI(),NULL,true);
                    break;
                case TFld::Real:
                    val.setR(prm_refl->at().vlAt(val.name()).at().getR(),NULL,true);
                    break;
                case TFld::Bool:
                    val.setB(prm_refl->at().vlAt(val.name()).at().getB(),NULL,true);
                    break;
    	    }
	else if( mode() == TParam::Template )
	    switch(val.fld().type())
	    {
	        case TFld::String:
	            val.setS(tmpl->val.getS(tmpl->val.ioId(val.name())),NULL,true);
                    break;
                case TFld::Dec:
		    val.setI(tmpl->val.getI(tmpl->val.ioId(val.name())),NULL,true);
                    break;
                case TFld::Real:
                    val.setR(tmpl->val.getR(tmpl->val.ioId(val.name())),NULL,true);
                    break;
        	case TFld::Bool:
		    val.setB(tmpl->val.getB(tmpl->val.ioId(val.name())),NULL,true);
                    break;
	    }    
    }catch(TError err) 
    {  
	if( val.name() == "err" )
    	    val.setB(false,NULL,true);
        else if( val.name() == "err_mess" )
            val.setS(Mess->I18N("No errors."),NULL,true);    
    }
}

void TParam::vlSet( TVal &val )
{
    try
    {
	if( mode() == TParam::DirRefl )
    	    switch(val.fld().type())
    	    {
        	case TFld::String:
                    prm_refl->at().vlAt(val.name()).at().setS(val.getS(NULL,true));
            	    break;
                case TFld::Dec:
                    prm_refl->at().vlAt(val.name()).at().setI(val.getI(NULL,true));
        	    break;
                case TFld::Real:
                    prm_refl->at().vlAt(val.name()).at().setR(val.getR(NULL,true));
                    break;
                case TFld::Bool:
                    prm_refl->at().vlAt(val.name()).at().setB(val.getB(NULL,true));
            	    break;
            }
	else if( mode() == TParam::Template )
	    switch(val.fld().type())
	    {
	        case TFld::String:
	            tmpl->val.setS(tmpl->val.ioId(val.name()),val.getS(NULL,true));
                    break;
                case TFld::Dec:
		    tmpl->val.setI(tmpl->val.ioId(val.name()),val.getI(NULL,true));
                    break;
                case TFld::Real:
		    tmpl->val.setR(tmpl->val.ioId(val.name()),val.getR(NULL,true));
                    break;
        	case TFld::Bool:
		    tmpl->val.setB(tmpl->val.ioId(val.name()),val.getB(NULL,true));
                    break;
	    }    
    }catch(TError err) {  }
}

int TParam::lnkSize()   
{ 
    if( mode() != TParam::Template )	
	throw TError(nodePath().c_str(),Mess->I18N("Parameter disabled or no template based.")); 
    return tmpl->lnk.size(); 
}

int TParam::lnkId( int id )
{
    if( mode() != TParam::Template )	
	throw TError(nodePath().c_str(),Mess->I18N("Parameter disabled or no template based.")); 
    for( int i_l = 0; i_l < tmpl->lnk.size(); i_l++ )
	if( tmpl->lnk[i_l].io_id == id )
	    return i_l;
    return -1;	    
}

TParam::SLnk &TParam::lnk( int num )
{
    if( mode() != TParam::Template )	
	throw TError(nodePath().c_str(),Mess->I18N("Parameter disabled or no template based."));
    if( num < 0 || num >= tmpl->lnk.size() )
	throw TError(nodePath().c_str(),Mess->I18N("Parameter id error."));
    return tmpl->lnk[num];
}

void TParam::calc()
{
    if( mode() != TParam::Template )	return;
    try
    {
	//Get input links
	for( int i_l = 0; i_l < lnkSize(); i_l++ )
	{
	    if( lnk(i_l).prm.freeStat() || lnk(i_l).mode != TPrmTempl::LinkIn ) 
		continue;
		
	    switch(tmpl->val.ioType(lnk(i_l).io_id))
	    {
		case IO::String:	
		    tmpl->val.setS(lnk(i_l).io_id,lnk(i_l).prm.at().vlAt(lnk(i_l).attr).at().getS());
		    break;
		case IO::Integer:
		    tmpl->val.setI(lnk(i_l).io_id,lnk(i_l).prm.at().vlAt(lnk(i_l).attr).at().getI());
		    break;
		case IO::Real:
		    tmpl->val.setR(lnk(i_l).io_id,lnk(i_l).prm.at().vlAt(lnk(i_l).attr).at().getR());
		    break;
		case IO::Boolean:
		    tmpl->val.setB(lnk(i_l).io_id,lnk(i_l).prm.at().vlAt(lnk(i_l).attr).at().getB());
		    break;
	    }
	}
	//Calc template
	tmpl->val.calc();
	//Put output links
	for( int i_l = 0; i_l < lnkSize(); i_l++ )
        {
	    if( lnk(i_l).prm.freeStat() || lnk(i_l).mode != TPrmTempl::LinkOut )
		continue;
	    switch(tmpl->val.ioType(lnk(i_l).io_id))
            {
		case IO::String:
            	    lnk(i_l).prm.at().vlAt(lnk(i_l).attr).at().setS(tmpl->val.getS(lnk(i_l).io_id));
                    break;
		case IO::Integer:
		    lnk(i_l).prm.at().vlAt(lnk(i_l).attr).at().setI(tmpl->val.getI(lnk(i_l).io_id));
                    break;
		case IO::Real:
		    lnk(i_l).prm.at().vlAt(lnk(i_l).attr).at().setR(tmpl->val.getR(lnk(i_l).io_id));
		    break;
		case IO::Boolean:
		    lnk(i_l).prm.at().vlAt(lnk(i_l).attr).at().setB(tmpl->val.getB(lnk(i_l).io_id));
		    break;
	    }
	}	
    }catch(TError err)
    { 
	Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str());
	Mess->put(nodePath().c_str(),TMess::Warning,Mess->I18N("Error calc template."));
    }
}

void TParam::cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd )
{
    vector<string> list;
    
    if( cmd==TCntrNode::Info )
    {
        ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18N("Parameter: ")+name());
        ctrMkNode("area",opt,a_path.c_str(),"/prm",Mess->I18N("Parameter"));
	ctrMkNode("area",opt,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/en",Mess->I18N("Enable"),0664,0,0,"bool");
        ctrMkNode("area",opt,a_path.c_str(),"/prm/cfg",Mess->I18N("Config"));
	TConfig::cntrMake(opt,a_path.c_str(),"/prm/cfg",0);
	ctrId(opt,"/prm/cfg/MODE")->attr_("dest","select")->attr_("select","/cfg/mode_lst");
	ctrId(opt,"/prm/cfg/PRM")->attr_("dest","sel_ed")->attr_("select","/cfg/prmp_lst");
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/load",Mess->I18N("Load"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/save",Mess->I18N("Save"),0550);
        ctrMkNode("area",opt,a_path.c_str(),"/val",Mess->I18N("Atributes"));
        TValue::cntrMake(opt,a_path.c_str(),"/val",-1);
	if( enableStat() && mode() == TParam::Template )
	{
	    ctrMkNode("area",opt,a_path.c_str(),"/cfg",Mess->I18N("Template config"));
	    ctrMkNode("area",opt,a_path.c_str(),"/cfg/prm",Mess->I18N("Parameters"));
	    AutoHD<TPrmTempl> w_tpl = owner().tplAt(m_wprm);
	    for( int i_io = 0; i_io < tmpl->val.func()->ioSize(); i_io++ )
            {
		char *tip = "str";
		int attr_id = w_tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
                if( attr_id < 0 )   continue;
		if( w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst )     
	    	    switch(tmpl->val.ioType(i_io))
	    	    {
	        	case IO::String:    tip = "str";    break;
	        	case IO::Integer:   tip = "dec";    break;
	        	case IO::Real:      tip = "real";   break;
	    		case IO::Boolean:   tip = "bool";   break;
            	    }
		if( w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkIn || 
			w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkOut ||
			w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst )
		{
		    ctrMkNode("fld",
			  opt,a_path.c_str(),
			  (string("/cfg/prm/el_")+TSYS::int2str(i_io)).c_str(),
			  tmpl->val.func()->io(i_io)->name(),0664,0,0,tip);
		    if( w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkIn || 
			    w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkOut )
		        ctrId(opt,string("/cfg/prm/el_")+TSYS::int2str(i_io))->
		    	    attr_("dest","sel_ed")->
			    attr("select",string("/cfg/prm/ls_")+TSYS::int2str(i_io));
		}    
	    }			    
	}    
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/st/en" )       	ctrSetB( opt, enableStat() );
	else if( a_path.substr(0,8) == "/prm/cfg" )	TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Get);
	else if( a_path.substr(0,4) == "/val" )	TValue::cntrCmd(TSYS::pathLev(a_path,1), opt, TCntrNode::Get);
        else if( a_path == "/cfg/mode_lst" )
        {
	    opt->childClean();
	    ctrSetS( opt, Mess->I18N("Clear parametr"), TSYS::int2str(TParam::Clear).c_str() );
    	    ctrSetS( opt, Mess->I18N("Direct reflection"), TSYS::int2str(TParam::DirRefl).c_str() );
	    ctrSetS( opt, Mess->I18N("Template"), TSYS::int2str(TParam::Template).c_str() );
	}	
	else if( a_path == "/cfg/prmp_lst" )
	{
	    opt->childClean();
	    if( m_mode == TParam::DirRefl )
	    {		
	        int c_lv = 0;
		string c_path = "";
		while(TSYS::strSepParse(m_prm,c_lv,'.').size())
		{
		    ctrSetS( opt, c_path );
		    if( c_lv ) c_path+=".";
		    c_path = c_path+TSYS::strSepParse(m_prm,c_lv,'.');
		    c_lv++;		    
		}
		if( c_lv != 3 )
		{
		    ctrSetS( opt, c_path );
		    if(c_lv) c_path+=".";
		}
		switch(c_lv)
		{
		    case 0:	
			SYS->controller().at().modList(list);
			break;
		    case 1:
			if( SYS->controller().at().modPresent(TSYS::strSepParse(m_prm,0,'.')) )
			    SYS->controller().at().at(TSYS::strSepParse(m_prm,0,'.')).at().list(list);
			break;
		    case 2: case 3:
			if( SYS->controller().at().modPresent(TSYS::strSepParse(m_prm,0,'.')) && 
				SYS->controller().at().at(TSYS::strSepParse(m_prm,0,'.')).at().present(TSYS::strSepParse(m_prm,1,'.')) )
			    SYS->controller().at().at(TSYS::strSepParse(m_prm,0,'.')).at().at(TSYS::strSepParse(m_prm,1,'.')).at().list(list);
			break;
		}
		if(c_lv==3)	
		    c_path=TSYS::strSepParse(m_prm,0,'.')+"."+TSYS::strSepParse(m_prm,1,'.')+".";
		for( unsigned i_a=0; i_a < list.size(); i_a++ )
		    ctrSetS( opt, c_path+list[i_a]);
	    }
	    else if( m_mode == TParam::Template )
            {
		owner().tplList(list);
		for( unsigned i_a=0; i_a < list.size(); i_a++ )
                    ctrSetS( opt, list[i_a]);
	    }
	}
	else if( mode() == TParam::Template )
	{	    
	    if( a_path.substr(0,12) == "/cfg/prm/el_" )
	    {
		AutoHD<TPrmTempl> w_tpl = owner().tplAt(m_wprm);
		int i_io = atoi(a_path.substr(12).c_str());
		int attr_id = w_tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
		if( w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkIn || 
			w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkOut )
		    ctrSetS( opt, lnk(lnkId(i_io)).prm_attr );
		else if( w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst )
		    switch(tmpl->val.ioType(i_io))
                    {
			case IO::String:	ctrSetS( opt, tmpl->val.getS(i_io) );	break;
			case IO::Integer:	ctrSetI( opt, tmpl->val.getI(i_io) );   break;
			case IO::Real:		ctrSetR( opt, tmpl->val.getR(i_io) );   break;
			case IO::Boolean:	ctrSetB( opt, tmpl->val.getB(i_io) );	break;
		    }
	    }
	    if( a_path.substr(0,12) == "/cfg/prm/ls_" )
	    {
		opt->childClean();
		string l_prm = lnk(lnkId(atoi(a_path.substr(12).c_str()))).prm_attr;
		
	        int c_lv = 0;
		string c_path = "";
		while(TSYS::strSepParse(l_prm,c_lv,'.').size())
		{
		    ctrSetS( opt, c_path );
		    if( c_lv ) c_path+=".";
		    c_path = c_path+TSYS::strSepParse(l_prm,c_lv,'.');
		    c_lv++;		    
		}
		if( c_lv != 4 )
		{
		    ctrSetS( opt, c_path );
		    if(c_lv) c_path+=".";
		}
		switch(c_lv)
		{
		    case 0:	
			SYS->controller().at().modList(list);
			break;
		    case 1:
			if( SYS->controller().at().modPresent(TSYS::strSepParse(l_prm,0,'.')) )
			    SYS->controller().at().at(TSYS::strSepParse(l_prm,0,'.')).at().list(list);
			break;
		    case 2: 
			if( SYS->controller().at().modPresent(TSYS::strSepParse(l_prm,0,'.')) && 
				SYS->controller().at().at(TSYS::strSepParse(l_prm,0,'.')).at().present(TSYS::strSepParse(l_prm,1,'.')) )
			    SYS->controller().at().at(TSYS::strSepParse(l_prm,0,'.')).at().at(TSYS::strSepParse(l_prm,1,'.')).at().list(list);
		    case 3: case 4:
			if( SYS->controller().at().modPresent(TSYS::strSepParse(l_prm,0,'.')) && 
				SYS->controller().at().at(TSYS::strSepParse(l_prm,0,'.')).at().present(TSYS::strSepParse(l_prm,1,'.')) &&
				SYS->controller().at().at(TSYS::strSepParse(l_prm,0,'.')).at().at(TSYS::strSepParse(l_prm,1,'.')).at().present(TSYS::strSepParse(l_prm,2,'.')) )
			    SYS->controller().at().at(TSYS::strSepParse(l_prm,0,'.')).at().at(TSYS::strSepParse(l_prm,1,'.')).at().at(TSYS::strSepParse(l_prm,2,'.')).at().vlList(list);
			break;
		}
		if(c_lv==4)	
		    c_path=TSYS::strSepParse(l_prm,0,'.')+"."+TSYS::strSepParse(l_prm,1,'.')+"."+TSYS::strSepParse(l_prm,2,'.')+".";
		for( unsigned i_a=0; i_a < list.size(); i_a++ )
		    ctrSetS( opt, c_path+list[i_a]);	    
	    }
	}    
	else throw TError(nodePath().c_str(),Mess->I18N("Branch <%s> error!"),a_path.c_str());
    }			
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/en" )            ctrGetB(opt)?enable():disable();
	else if( a_path == "/prm/cfg/load" )    load();
        else if( a_path == "/prm/cfg/save" )    save();	
	else if( a_path.substr(0,8) == "/prm/cfg" )
	{
	    TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Set);
	    if( a_path == "/prm/cfg/MODE" )	mode( (TParam::Mode)m_mode, m_prm = "" ); 
	    else if( a_path == "/prm/cfg/PRM" ) mode( (TParam::Mode)m_mode, m_prm );
	}
	else if( a_path.substr(0,4) == "/val" )	TValue::cntrCmd(TSYS::pathLev(a_path,1), opt, TCntrNode::Set);
	else if( mode() == TParam::Template )
	{	    
	    if( a_path.substr(0,12) == "/cfg/prm/el_" )
	    {		
		AutoHD<TPrmTempl> w_tpl = owner().tplAt(m_wprm);
		int i_io = atoi(a_path.substr(12).c_str());
		int attr_id = w_tpl.at().attrId(tmpl->val.func()->io(i_io)->id());		
		if( w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkIn || 
		    w_tpl.at().attr(attr_id).accs == TPrmTempl::LinkOut )
		{
		    lnk(lnkId(i_io)).prm_attr = ctrGetS(opt);
		    mode( (TParam::Mode)m_mode, m_prm );
		}
		else if( w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst )
		    switch(tmpl->val.ioType(i_io))
	            {
	        	case IO::String:	tmpl->val.setS(i_io,ctrGetS(opt));break;
		        case IO::Integer:       tmpl->val.setI(i_io,ctrGetI(opt));break;
		        case IO::Real:          tmpl->val.setR(i_io,ctrGetR(opt));break;
		        case IO::Boolean:       tmpl->val.setB(i_io,ctrGetB(opt));break;
		    }		    
	    }
	}	
        else throw TError(nodePath().c_str(),Mess->I18N("Branch <%s> error!"),a_path.c_str());
    }
}
                                                                           

