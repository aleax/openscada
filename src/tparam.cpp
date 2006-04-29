
//OpenSCADA system file: tparam.cpp
/***************************************************************************
 *   Copyright (C) 2003-2006 by Roman Savochenko                           *
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

#include <typeinfo>

#include "resalloc.h"
#include "tsys.h"
#include "tparam.h"

TParam::TParam( const string &iid, const string &idb, TElem *cf_el ) : 
    m_en(false), m_bd(idb), p_el("w_attr"), prm_refl(NULL), TConfig(cf_el), m_wmode(TParam::Clear), m_onl_attr(false),
    m_id(cfg("SHIFR").getSd()), m_name(cfg("NAME").getSd()), m_descr(cfg("DESCR").getSd()), 
    m_aen(cfg("EN").getBd()), m_mode(cfg("MODE").getId()), m_prm(cfg("PRM").getSd())
{    
    m_id = iid;    
}

TParam::~TParam(  )
{

}

void TParam::preDisable(int flag)
{
    vlDetElem(&p_el);
    vlDetElem(&SYS->daq().at().errE());
}

void TParam::postDisable(int flag)
{
    try
    {
	if( flag ) SYS->db().at().dataDel(BD(),SYS->param().at().nodePath()+"Prm/",*this);
    }catch(TError err)
    { Mess->put(err.cat.c_str(),TMess::Warning,err.mess.c_str()); }
}

string TParam::BD()
{
    return m_bd+"."+owner().subId();
}

void TParam::postEnable( )
{
    vlCfg(this);	//Mirrore config to atributes
    vlAttElem(&SYS->daq().at().errE());
    vlAttElem(&p_el);
}

string TParam::name()   
{ 
    return (m_name.size())?m_name:m_id;
}

void TParam::load( )
{
    try
    {
	SYS->db().at().dataGet(BD(),SYS->param().at().nodePath()+"Prm/",*this);
	mode(mode(),m_prm);
	loadIO();
    }catch(...){ }
}

void TParam::loadIO()
{
    //Load IO and init links
    if( mode() == TParam::Template )
    {
	TConfig cfg(&owner().prmIOE());
        cfg.cfg("PRM_ID").setS(id());
        string io_bd = BD()+"_io";
	    
	AutoHD<TPrmTempl> w_tpl = owner().tplAt(m_wprm);
	
	for( int i_io = 0; i_io < tmpl->val.ioSize(); i_io++ )
	{
	    int attr_id = w_tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
	    if( attr_id < 0 ) continue;
	    
	    cfg.cfg("ID").setS(tmpl->val.func()->io(i_io)->id());
	    if(!SYS->db().at().dataGet(io_bd,SYS->param().at().nodePath()+"PrmIO/",cfg))
	        continue;
	    if( w_tpl.at().attr(attr_id).accs == TPrmTempl::Link )
	        lnk(lnkId(i_io)).prm_attr = cfg.cfg("VALUE").getS();
	    else tmpl->val.setS(i_io,cfg.cfg("VALUE").getS());
	}
	initTmplLnks();
    }
}

void TParam::save( )
{
    SYS->db().at().dataSet(BD(),SYS->param().at().nodePath()+"Prm/",*this);
    
    saveIO();
    
    //Save archives
    vector<string> a_ls;
    vlList(a_ls);
    for(int i_a = 0; i_a < a_ls.size(); i_a++)
        if( !vlAt(a_ls[i_a]).at().arch().freeStat() )
            vlAt(a_ls[i_a]).at().arch().at().save();				    
}

void TParam::saveIO()
{
    //Save IO and init links
    if( mode() == TParam::Template )
    {
	TConfig cfg(&owner().prmIOE());
        cfg.cfg("PRM_ID").setS(id());
        string io_bd = BD()+"_io";
	
	AutoHD<TPrmTempl> w_tpl = owner().tplAt(m_wprm);
	
	for( int i_io = 0; i_io < tmpl->val.func()->ioSize(); i_io++ )
	{
	    int attr_id = w_tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
	    if( attr_id < 0 ) continue;
	    cfg.cfg("ID").setS(tmpl->val.func()->io(i_io)->id());
	    if( w_tpl.at().attr(attr_id).accs == TPrmTempl::Link )
		cfg.cfg("VALUE").setS(lnk(lnkId(i_io)).prm_attr);
            else cfg.cfg("VALUE").setS(tmpl->val.getS(i_io));
	    SYS->db().at().dataSet(io_bd,SYS->param().at().nodePath()+"PrmIO/",cfg);
	}
    }
}

void TParam::enable()
{
    if( enableStat() )	return;
    m_en = true;
    try
    { 
	mode((TParam::Mode)m_mode,m_prm); 
	loadIO();
    }
    catch(...){ m_en = false; throw; }    
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
    	    owner().setPrmCalc( id(), false );	//Get from process
    	    while(p_el.fldSize())   p_el.fldDel(0);
    	    delete tmpl;
    	    tmpl = NULL;
        }
    }
	
    //Init/update new mode
    if( md == TParam::DirRefl )
    {
        if( !prm_refl ) prm_refl = new AutoHD<TValue>;
        try
        {
    	    prm_refl->free();
	    if( !dynamic_cast<TValue*>(&SYS->nodeAt(prm,0,'.').at()) )
		throw TError(nodePath().c_str(),Mess->I18N("Do not select parameter."));
	    *prm_refl = SYS->nodeAt(prm,0,'.');
    	    prm_refl->at().vlList(list);
 	    for( int i_l = 0; i_l < list.size(); i_l++ )
		if( !vlPresent(list[i_l]) )
		    p_el.fldAdd( new TFld(list[i_l].c_str(),prm_refl->at().vlAt(list[i_l]).at().fld().descr().c_str(),
			prm_refl->at().vlAt(list[i_l]).at().fld().type(),FLD_DWR|FLD_DRD|(prm_refl->at().vlAt(list[i_l]).at().fld().flg())&FLD_NWR) );
        }catch(...)
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
 		if( w_tpl.at().attr(attr_id).accs == TPrmTempl::Link && lnkId(i_io) < 0 )
		    tmpl->lnk.push_back(SLnk(i_io,w_tpl.at().attr(attr_id).accs));
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
		    p_el.fldAdd( new TFld(tmpl->val.func()->io(i_io)->id().c_str(),
				tmpl->val.func()->io(i_io)->name().c_str(),tp,flg) );
		}
		if( to_make && (w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst || 
			w_tpl.at().attr(attr_id).accs == TPrmTempl::Const ) )
		    tmpl->val.setS(i_io,w_tpl.at().attr(attr_id).val);
	    }
	    //Init links
	    initTmplLnks();
        
	    //Set to process
	    owner().setPrmCalc(id(),true);
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
}

void TParam::initTmplLnks()
{
    if( mode() != TParam::Template )	return;
    //Init links
    for( int i_l = 0; i_l < lnkSize(); i_l++ )
    {
	try
	{
	    lnk(i_l).aprm.free();
	    if( dynamic_cast<TVal *>(&SYS->nodeAt(lnk(i_l).prm_attr,0,'.').at()) )
	    {
		lnk(i_l).aprm = SYS->nodeAt(lnk(i_l).prm_attr,0,'.');
		//Init value
		tmpl->val.setS(lnk(i_l).io_id,lnk(i_l).aprm.at().getS());
	    }
	}catch(TError err){ }
    }
}

void TParam::vlGet( TVal &val )
{
    if( !enableStat() )
    {
	if( val.name() == "err" ) val.setS(Mess->I18N("1:Parameter had disabled."),0,true);
	return;
    }    
    try
    {
	if( mode() == TParam::DirRefl )
	    switch(val.fld().type())
    	    {
	        case TFld::String:
	    	    val.setS(prm_refl->at().vlAt(val.name()).at().getS(),0,true);
                    break;
                case TFld::Dec:
            	    val.setI(prm_refl->at().vlAt(val.name()).at().getI(),0,true);
                    break;
                case TFld::Real:
                    val.setR(prm_refl->at().vlAt(val.name()).at().getR(),0,true);
                    break;
                case TFld::Bool:
                    val.setB(prm_refl->at().vlAt(val.name()).at().getB(),0,true);
                    break;
    	    }
	else if( mode() == TParam::Template )
	{
	    int id_lnk = lnkId(val.name());
	    if( id_lnk >= 0 && lnk(id_lnk).aprm.freeStat() ) 
		id_lnk=-1;
	    switch(val.fld().type())
	    {
	        case TFld::String:
		    if( id_lnk < 0 ) val.setS(tmpl->val.getS(tmpl->val.ioId(val.name())),0,true);
		    else val.setS(lnk(id_lnk).aprm.at().getS(),0,true);
            	    break;
            	case TFld::Dec:
		    if( id_lnk < 0 ) val.setI(tmpl->val.getI(tmpl->val.ioId(val.name())),0,true);
		    else val.setI(lnk(id_lnk).aprm.at().getI(),0,true);
            	    break;
            	case TFld::Real:
		    if( id_lnk < 0 ) val.setR(tmpl->val.getR(tmpl->val.ioId(val.name())),0,true);
		    else val.setR(lnk(id_lnk).aprm.at().getR(),0,true);
            	    break;
        	case TFld::Bool:
		    if( id_lnk < 0 ) val.setB(tmpl->val.getB(tmpl->val.ioId(val.name())),0,true);
		    else val.setB(lnk(id_lnk).aprm.at().getB(),0,true);
            	    break;
	    }		
	}	
    }catch(TError err) 
    { if( val.name() == "err" ) val.setS("0",0,true); }
}

void TParam::vlSet( TVal &val )
{
    try
    {
	if( mode() == TParam::DirRefl )
    	    switch(val.fld().type())
    	    {
        	case TFld::String:
                    prm_refl->at().vlAt(val.name()).at().setS(val.getS(0,true));
            	    break;
                case TFld::Dec:
                    prm_refl->at().vlAt(val.name()).at().setI(val.getI(0,true));
        	    break;
                case TFld::Real:
                    prm_refl->at().vlAt(val.name()).at().setR(val.getR(0,true));
                    break;
                case TFld::Bool:
                    prm_refl->at().vlAt(val.name()).at().setB(val.getB(0,true));
            	    break;
            }
	else if( mode() == TParam::Template )
	{
	    int id_lnk = lnkId(val.name());
	    if( id_lnk >= 0 && lnk(id_lnk).aprm.freeStat()) 
		id_lnk=-1;
	    switch(val.fld().type())
	    {
	        case TFld::String:
		    if( id_lnk < 0 ) tmpl->val.setS(tmpl->val.ioId(val.name()),val.getS(0,true));
		    else lnk(id_lnk).aprm.at().setS(val.getS(0,true));
                    break;
                case TFld::Dec:
		    if( id_lnk < 0 ) tmpl->val.setI(tmpl->val.ioId(val.name()),val.getI(0,true));
		    else lnk(id_lnk).aprm.at().setI(val.getI(0,true));
                    break;
                case TFld::Real:
		    if( id_lnk < 0 ) tmpl->val.setR(tmpl->val.ioId(val.name()),val.getR(0,true));
		    else lnk(id_lnk).aprm.at().setR(val.getR(0,true));
                    break;
        	case TFld::Bool:
		    if( id_lnk < 0 ) tmpl->val.setB(tmpl->val.ioId(val.name()),val.getB(0,true));
		    else lnk(id_lnk).aprm.at().setB(val.getB(0,true));
                    break;
	    }    
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
    for( int i_l = 0; i_l < lnkSize(); i_l++ )
	if( lnk(i_l).io_id == id )
	    return i_l;
    return -1;	    
}

int TParam::lnkId( const string &id )
{
    if( mode() != TParam::Template )
	throw TError(nodePath().c_str(),Mess->I18N("Parameter disabled or no template based."));
    for( int i_l = 0; i_l < lnkSize(); i_l++ )
        if( tmpl->val.func()->io(lnk(i_l).io_id)->id() == id )
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
	    if( lnk(i_l).aprm.freeStat() || lnk(i_l).mode != TPrmTempl::Link )
		continue;
		
	    switch(tmpl->val.ioType(lnk(i_l).io_id))
	    {
		case IO::String:	
		    tmpl->val.setS(lnk(i_l).io_id,lnk(i_l).aprm.at().getS());
		    break;
		case IO::Integer:
		    tmpl->val.setI(lnk(i_l).io_id,lnk(i_l).aprm.at().getI());
		    break;
		case IO::Real:
		    tmpl->val.setR(lnk(i_l).io_id,lnk(i_l).aprm.at().getR());
		    break;
		case IO::Boolean:
		    tmpl->val.setB(lnk(i_l).io_id,lnk(i_l).aprm.at().getB());
		    break;
	    }
	}
	//Calc template
	tmpl->val.calc();
	//Put output links
	for( int i_l = 0; i_l < lnkSize(); i_l++ )
        {
	    if( lnk(i_l).aprm.freeStat() || lnk(i_l).mode != TPrmTempl::Link || 
		    lnk(i_l).aprm.at().fld().flg()&FLD_NWR )
		continue;
	    switch(tmpl->val.ioType(lnk(i_l).io_id))
            {
		case IO::String:
            	    lnk(i_l).aprm.at().setS(tmpl->val.getS(lnk(i_l).io_id));
                    break;
		case IO::Integer:
		    lnk(i_l).aprm.at().setI(tmpl->val.getI(lnk(i_l).io_id));
                    break;
		case IO::Real:
		    lnk(i_l).aprm.at().setR(tmpl->val.getR(lnk(i_l).io_id));
		    break;
		case IO::Boolean:
		    lnk(i_l).aprm.at().setB(tmpl->val.getB(lnk(i_l).io_id));
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
	TValue::cntrCmd_(a_path,opt,cmd);
    
        ctrMkNode("oscada_cntr",opt,a_path.c_str(),"/",Mess->I18N("Parameter: ")+name());
        ctrInsNode("area",0,opt,a_path.c_str(),"/prm",Mess->I18N("Parameter"));
	ctrMkNode("area",opt,a_path.c_str(),"/prm/st",Mess->I18N("State"));
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/en",Mess->I18N("Enable"),0664,0,0,"bool");
	ctrMkNode("fld",opt,a_path.c_str(),"/prm/st/bd",Mess->I18N("Parameter DB (module.db)"),0660,0,0,"str");
        ctrMkNode("area",opt,a_path.c_str(),"/prm/cfg",Mess->I18N("Config"));
	TConfig::cntrMake(opt,a_path.c_str(),"/prm/cfg",0);
	ctrId(opt,"/prm/cfg/MODE")->attr_("dest","select")->attr_("select","/cfg/mode_lst");
	ctrId(opt,"/prm/cfg/PRM")->attr_("dest","sel_ed")->attr_("select","/cfg/prmp_lst");
	ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/load",Mess->I18N("Load"),0550);
        ctrMkNode("comm",opt,a_path.c_str(),"/prm/cfg/save",Mess->I18N("Save"),0550);
	if( enableStat() && mode() == TParam::Template )
	{
	    ctrMkNode("area",opt,a_path.c_str(),"/cfg",Mess->I18N("Template config"));
	    ctrMkNode("fld",opt,a_path.c_str(),"/cfg/attr_only",Mess->I18N("Only atributes show"),0664,0,0,"bool");
	    ctrMkNode("area",opt,a_path.c_str(),"/cfg/prm",Mess->I18N("Parameters"));
	    AutoHD<TPrmTempl> w_tpl = owner().tplAt(m_wprm);
	    list.clear();	    
	    for( int i_io = 0; i_io < tmpl->val.ioSize(); i_io++ )
            {
		int attr_id = w_tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
                if( attr_id < 0 )   continue;				    
		//Check select param
		if( !m_onl_attr && w_tpl.at().attr(attr_id).accs == TPrmTempl::Link && w_tpl.at().attr(attr_id).val.size() )
		{
		    string nprm = TSYS::strSepParse(w_tpl.at().attr(attr_id).val,0,'|');
		    //Check already to present parameters
		    bool f_ok = false;
		    for( int i_l = 0; i_l < list.size(); i_l++ )
			if( list[i_l] == nprm )
			{ f_ok = true; break; }
		    if(!f_ok)
		    {
			ctrMkNode("fld",opt,a_path.c_str(),(string("/cfg/prm/pr_")+TSYS::int2str(i_io)).c_str(),
				nprm,0664,0,0,"str")->attr_("dest","sel_ed")->attr("select",string("/cfg/prm/pl_")+TSYS::int2str(i_io));
			list.push_back(nprm);
		    }
		}		
		else
		{	    
		    char *tip = "str";
		    if( w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst )     
	    		switch(tmpl->val.ioType(i_io))
	    		{
	        	    case IO::String:    tip = "str";    break;
	        	    case IO::Integer:   tip = "dec";    break;
	        	    case IO::Real:      tip = "real";   break;
	    		    case IO::Boolean:   tip = "bool";   break;
            		}
		    if( w_tpl.at().attr(attr_id).accs == TPrmTempl::Link ||
			w_tpl.at().attr(attr_id).accs == TPrmTempl::PublConst )
		    {
			ctrMkNode("fld",
			    opt,a_path.c_str(),
			    (string("/cfg/prm/el_")+TSYS::int2str(i_io)).c_str(),
			    tmpl->val.func()->io(i_io)->name(),0664,0,0,tip);
			if( w_tpl.at().attr(attr_id).accs == TPrmTempl::Link )
		    	    ctrId(opt,string("/cfg/prm/el_")+TSYS::int2str(i_io))->
		    		attr_("dest","sel_ed")->
				attr("select",string("/cfg/prm/ls_")+TSYS::int2str(i_io));
		    }    
		}
	    }			    
	}    
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/prm/st/en" )       	ctrSetB( opt, enableStat() );
	else if( a_path == "/prm/st/bd" )       ctrSetS( opt, m_bd );
	else if( a_path.substr(0,8) == "/prm/cfg" )	TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Get);
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
		ctrSetS( opt, c_path );
		while(TSYS::strSepParse(m_prm,c_lv,'.').size())
		{
		    if( c_lv ) c_path+=".";
		    c_path = c_path+TSYS::strSepParse(m_prm,c_lv,'.');
		    ctrSetS( opt, c_path );
		    c_lv++;		    
		}
		if(c_lv) c_path+=".";
		if( !dynamic_cast<TValue *>(&SYS->nodeAt(c_path,0,'.').at()) )
		    SYS->nodeAt(c_path,0,'.').at().nodeList(list);
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
	    if( a_path == "/cfg/attr_only" )   ctrSetB( opt, m_onl_attr);
	    else if( a_path.substr(0,12) == "/cfg/prm/pr_" )
	    {
		int i_lnk = lnkId(atoi(a_path.substr(12).c_str()));
		int i_cnt = 0;
		string e_vl;
		if( dynamic_cast<TVal *>(&SYS->nodeAt(lnk(i_lnk).prm_attr,0,'.').at()) )
		    while(TSYS::strSepParse(lnk(i_lnk).prm_attr,i_cnt+1,'.').size())
		    {
			if(i_cnt)e_vl+=".";
			e_vl+=TSYS::strSepParse(lnk(i_lnk).prm_attr,i_cnt++,'.');
		    }
		else e_vl = lnk(i_lnk).prm_attr;
		ctrSetS(opt,e_vl);
	    }
	    else if( a_path.substr(0,12) == "/cfg/prm/pl_" )
	    {
		opt->childClean();
                string l_prm = lnk(lnkId(atoi(a_path.substr(12).c_str()))).prm_attr;
		int c_lv = 0;
	        string c_path = "";
	        ctrSetS( opt, c_path );
	        while(TSYS::strSepParse(l_prm,c_lv,'.').size())
                {
                    if( c_lv ) c_path+=".";
		    c_path = c_path+TSYS::strSepParse(l_prm,c_lv,'.');
		    if( !dynamic_cast<TVal *>(&SYS->nodeAt(c_path,0,'.').at()) )
			ctrSetS( opt, c_path );
		    c_lv++;
                }
                if(c_lv) c_path+=".";
                if( !dynamic_cast<TValue *>(&SYS->nodeAt(c_path,0,'.').at()) )
                    SYS->nodeAt(c_path,0,'.').at().nodeList(list);
		for( unsigned i_a=0; i_a < list.size(); i_a++ )
                    ctrSetS( opt, c_path+list[i_a]);
	    }
	    else if( a_path.substr(0,12) == "/cfg/prm/el_" )
	    {
		AutoHD<TPrmTempl> w_tpl = owner().tplAt(m_wprm);
		int i_io = atoi(a_path.substr(12).c_str());
		int attr_id = w_tpl.at().attrId(tmpl->val.func()->io(i_io)->id());
		if( w_tpl.at().attr(attr_id).accs == TPrmTempl::Link )
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
	    else if( a_path.substr(0,12) == "/cfg/prm/ls_" )
	    {
		opt->childClean();
		string l_prm = lnk(lnkId(atoi(a_path.substr(12).c_str()))).prm_attr;
		
	        int c_lv = 0;
		string c_path = "";
		ctrSetS( opt, c_path );
		while(TSYS::strSepParse(l_prm,c_lv,'.').size())
		{
		    if( c_lv ) c_path+=".";		    
		    c_path = c_path+TSYS::strSepParse(l_prm,c_lv,'.');
		    ctrSetS( opt, c_path );
		    c_lv++;		    
		}
		if(c_lv) c_path+=".";
		if( !dynamic_cast<TVal *>(&SYS->nodeAt(l_prm,0,'.').at()) )
            	    SYS->nodeAt(l_prm,0,'.').at().nodeList(list);
	        for( unsigned i_a=0; i_a < list.size(); i_a++ )
	            ctrSetS( opt, c_path+list[i_a]);
	    }
	    else TValue::cntrCmd_(a_path,opt,cmd);
	}    
	else TValue::cntrCmd_(a_path,opt,cmd);
    }			
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/prm/st/en" )            ctrGetB(opt)?enable():disable();
	else if( a_path == "/prm/st/bd" )       m_bd = ctrGetS(opt);
	else if( a_path == "/prm/cfg/load" )    load();
        else if( a_path == "/prm/cfg/save" )    save();	
	else if( a_path.substr(0,8) == "/prm/cfg" )
	{
	    TConfig::cntrCmd(TSYS::pathLev(a_path,2), opt, TCntrNode::Set);
	    if( a_path == "/prm/cfg/MODE" )	
		try{ mode( (TParam::Mode)m_mode, m_prm = "" ); } catch(...) { disable(); throw; }
	    else if( a_path == "/prm/cfg/PRM" ) 
		try{ mode( (TParam::Mode)m_mode, m_prm ); } catch(...){ disable(); throw; }
	}
	else if( mode() == TParam::Template )
	{
	    if( a_path == "/cfg/attr_only" )   	m_onl_attr = ctrGetB(opt);
	    else if( a_path.substr(0,12) == "/cfg/prm/pr_" )
	    {
		bool only_no_set = true;
		string no_set;
		AutoHD<TPrmTempl> w_tpl = owner().tplAt(m_wprm);
		string p_nm = TSYS::strSepParse(w_tpl.at().attr(lnk(lnkId(atoi(a_path.substr(12).c_str()))).io_id).val,0,'|');
		string p_vl = ctrGetS(opt);
		bool it_val = dynamic_cast<TValue *>(&SYS->nodeAt(p_vl,0,'.').at());
		for( int i_l = 0; i_l < lnkSize(); i_l++ )
		    if( p_nm == TSYS::strSepParse(w_tpl.at().attr(lnk(i_l).io_id).val,0,'|') )
		    {
			lnk(i_l).prm_attr = p_vl;
			if( it_val )
			{
			    string p_attr = TSYS::strSepParse(w_tpl.at().attr(lnk(i_l).io_id).val,1,'|');
			    if( dynamic_cast<TValue &>(SYS->nodeAt(p_vl,0,'.').at()).vlPresent(p_attr) )
			    { lnk(i_l).prm_attr+= "."+p_attr; only_no_set = false; }
			    else no_set+=p_attr+",";
			}
		    }
		if( it_val )
		{    
		    if( only_no_set )	
			throw TError(nodePath().c_str(),Mess->I18N("Parameter have not only atributes!"));
		    else if( no_set.size() ) 	
			throw TError(nodePath().c_str(),Mess->I18N("Parameter have not atributes: %s !"),no_set.c_str());
		}
	    }	    
	    else if( a_path.substr(0,12) == "/cfg/prm/el_" )
	    {		
		AutoHD<TPrmTempl> w_tpl = owner().tplAt(m_wprm);
		int i_io = atoi(a_path.substr(12).c_str());
		int attr_id = w_tpl.at().attrId(tmpl->val.func()->io(i_io)->id());		
		if( w_tpl.at().attr(attr_id).accs == TPrmTempl::Link )
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
	    else TValue::cntrCmd_(a_path,opt,cmd);
	}	
        else TValue::cntrCmd_(a_path,opt,cmd);
    }
}
