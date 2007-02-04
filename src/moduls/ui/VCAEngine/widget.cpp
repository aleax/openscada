
//OpenSCADA system module UI.VCSEngine file: widget.cpp
/***************************************************************************
 *   Copyright (C) 2006 by Roman Savochenko
 *   rom_as@diyaorg.dp.ua
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#include <resalloc.h>
#include <tsys.h>

#include "vcaengine.h"
#include "widget.h"

using namespace VCA;


//====== Widget =====
Widget::Widget( const string &id, bool lib_loc, const string &isrcwdg, bool icontainer ) :
    TConfig(lib_loc?(&mod->elWdg()):(&mod->elInclWdg())), m_enable(false), m_lib_loc(lib_loc), m_container(icontainer),
    m_id(cfg("ID").getSd()), m_name(cfg("NAME").getSd()), m_descr(cfg("DESCR").getSd())
{
    attr_res = ResAlloc::resCreate();

    m_id = id;
    if(container())	inclWdg = grpAdd("wdg_");
    if(libLocated())
    {
	cfg("ORIGWDG").setS(original());
	user("user");
	grp("users");
	permit(0664);
    }
    else srcWdgNm(isrcwdg);
    
    attr_cfg = new TElem;    
    attr_cfg->valAtt(this);
}

Widget::~Widget()
{
    TAttrMap::iterator p;
    while( (p=attrs.begin())!=attrs.end() )
    {
        delete p->second;
        attrs.erase(p);
    }    
    attr_cfg->valDet(this);	
    delete attr_cfg;
    
    ResAlloc::resDelete(attr_res);
}

void Widget::postEnable()
{
    if( !libLocated() )	cfg("IDW").setS(ownWdg().id());

    attrProc(this,true);
}

void Widget::preDisable(int flag)
{
    if( enable( ) ) enable(false);
}

void Widget::postDisable(int flag)
{
    if( flag )
    {
     	if(libLocated())
	{
	    string fullDB = ownLib().fullDB();
            string tbl = ownLib().tbl();
	
	    //Remove from library table
	    SYS->db().at().dataDel(fullDB,mod->nodePath()+tbl,*this);
	    //Remove widget's IO from library IO table
	    TConfig c_el(&mod->elWdgIO());	    
	    c_el.cfg("IDW").setS(id());
	    c_el.cfg("ID").setS("");
	    SYS->db().at().dataDel(fullDB+"_io",mod->nodePath()+tbl+"_io",*this);
	}
	else
	{ 
	    string fullDB = ownWdg().ownLib().fullDB();
	    string tbl = ownWdg().ownLib().tbl();		
	
	    //Remove from library table
	    SYS->db().at().dataDel(fullDB+"_incl",mod->nodePath()+tbl+"_incl",*this);
	    //Remove widget's IO from library IO table
	    TConfig c_el(&mod->elWdgIO());
	    c_el.cfg("IDW").setS(ownWdg().id());
	    c_el.cfg("ID").setS("");
	    int fld_cnt=0;
	    while( SYS->db().at().dataSeek(fullDB+"_io",mod->nodePath()+tbl+"_io",fld_cnt++,c_el) )
	    {
		if(c_el.cfg("ID").getS().find(id()+"_")==0)
		{
		    SYS->db().at().dataDel(fullDB+"_io",mod->nodePath()+tbl+"_io",c_el);
		    fld_cnt--;
		}
		c_el.cfg("ID").setS("");
	    }
	}
    }
}

Widget &Widget::operator=(Widget &wdg)
{
    vector<string> als;
    attrList(als);
    for(int i_a = 0; i_a < als.size(); i_a++)
	if(wdg.attrPresent(als[i_a]))
	{
	    attrAt(als[i_a]).at().setS(wdg.attrAt(als[i_a]).at().getS());
	    attrAt(als[i_a]).at().modifVal(0);
	    attrAt(als[i_a]).at().modifCfg(0);
	}
    
    return *this;
}

LibWdg &Widget::ownLib()
{
    if(libLocated())
    {
	LibWdg *own = dynamic_cast<LibWdg *>(nodePrev());
	if(own)	return *own;
    }
    throw TError(nodePath().c_str(),_("Owner is not library!"));
}

Widget &Widget::ownWdg()
{
    if(!libLocated())
    {
	Widget *own = dynamic_cast<Widget *>(nodePrev());
	if(own)	return *own;
    }
    throw TError(nodePath().c_str(),_("Owner is not library!"));
}		

string Widget::name()
{
    return (m_name.size())?m_name:m_id;
}

string Widget::ico()
{
    if(libLocated()&&cfg("ICO").getS().size())	return cfg("ICO").getS();
    if(!srcWdg().freeStat())			return srcWdg().at().ico();    
    return icoOriginal();
}

void Widget::ico( const string &ico )
{
    if(libLocated()) cfg("ICO").setS(ico);
}

string Widget::icoOriginal()
{
    if( TUIS::icoPresent(mod->modId()+".wdg"+original()) )
	return TSYS::strEncode(TUIS::icoGet(mod->modId()+".wdg"+original()),TSYS::base64);
    else return "";
}

string Widget::user()  
{
    if(libLocated())
    {
	string m_user = cfg("USER").getS();
	return SYS->security().at().usrPresent(m_user)?m_user:"user"; 
    }
    else return ownWdg().user();
}

void Widget::user( const string &iuser )
{ 
    if(libLocated())
    {
	cfg("USER").setS(iuser);
	//Group update
	vector<string> gls;
	SYS->security().at().usrGrpList(user(),gls);
	grp(gls.size()?gls[0]:"users");
    }
}

string Widget::grp()
{ 
    if(libLocated())
    {
	string m_grp = cfg("GRP").getS();
	return SYS->security().at().grpPresent(m_grp)?m_grp:"users"; 
    }
    else return ownWdg().grp();
}	

void Widget::grp( const string &igrp )
{
    if(libLocated()) cfg("GRP").setS(igrp);
}

short Widget::permit()
{
    if(libLocated()) return cfg("PERMIT").getI();
    else return ownWdg().permit();
}

void Widget::permit( short iperm )
{
    if(libLocated()) cfg("PERMIT").setI(iperm);
}

string Widget::srcWdgNm()
{
    if(!libLocated())	return cfg("ORIGWDG").getS();
    return "";
}

void Widget::srcWdgNm( const string &isw )
{
    if( !libLocated() )
    {
	enable(false);	
	cfg("ORIGWDG").setS(isw);
    }
}


string Widget::progLang()
{
    if( !libLocated() )	return "";
    
    string iprg = cfg("PROC").getS();
    if(iprg.find("\n")==string::npos) 
    {
	iprg=iprg+"\n";
	cfg("PROC").setS(iprg);
    }
    return iprg.substr(0,iprg.find("\n"));
}
	
string Widget::prog()
{
    if( !libLocated() ) return "";

    string iprg = cfg("PROC").getS();
    int lng_end = iprg.find("\n");
    if(lng_end == string::npos) lng_end=0;
    else lng_end++;
    return iprg.substr(lng_end);
}

void Widget::progLang( const string &ilng )
{
    if( !libLocated() ) return;
    string iprg = cfg("PROC").getS();
    cfg("PROC").setS(iprg.replace(0,iprg.find("\n"),ilng));
}
	
void Widget::prog( const string &iprg )
{
    if( !libLocated() ) return;
    
    string tmp_prg = cfg("PROC").getS();	    
    int lng_end = tmp_prg.find("\n");
    if(lng_end == string::npos)
    {
        tmp_prg=tmp_prg+"\n";
        lng_end=tmp_prg.find("\n");
    }
    cfg("PROC").setS(tmp_prg.replace(lng_end+1,string::npos,iprg));
}

void Widget::load( )
{
    //Load generic widget's data
    string db  = libLocated() ? ownLib().DB() : ownWdg().ownLib().DB();
    string tbl = libLocated() ? ownLib().tbl() : ownWdg().ownLib().tbl()+"_incl";
    SYS->db().at().dataGet(db+"."+tbl,mod->nodePath()+tbl,*this);    
    
    //Load widget's attributes
    loadIO();
    
    //Load cotainer widgets
    if(container())
    {    
	TConfig c_el(&mod->elInclWdg());
	tbl=tbl+"_incl";
	c_el.cfg("IDW").setS(id());
	c_el.cfg("ID").setS("");
	int fld_cnt=0;
	while( SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fld_cnt++,c_el) )
	{
	    string sid  = c_el.cfg("ID").getS();
	    string orig = c_el.cfg("ORIGWDG").getS();
	    if( !wdgPresent(sid) ) wdgAdd(sid,orig);
	    wdgAt(sid).at().load();
	}
    }
}

void Widget::loadIO( )
{
    TConfig c_el(&mod->elWdgIO());

    string db  = libLocated() ? ownLib().DB() : ownWdg().ownLib().DB();
    string tbl = (libLocated() ? ownLib().tbl() : ownWdg().ownLib().tbl())+"_io";
    
    int fld_cnt=0;
    c_el.cfg("IDW").setS( libLocated() ? id() : ownWdg().id() );
    c_el.cfg("ID").setS("");
    while( SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fld_cnt++,c_el) )
    {
	string sid = c_el.cfg("ID").getS();
	unsigned flg = c_el.cfg("IO_TYPE").getI();
	c_el.cfg("ID").setS("");
	if(!libLocated())	
	    if(sid.find(id()+"_")==0) sid.erase(0,id().size()+1);
	    else continue;
	if(!attrPresent(sid))
	    if((flg>>4)&Attr::IsUser) attrAdd( new TFld(sid.c_str(),c_el.cfg("NAME").getS().c_str(),(TFld::Type)(flg&0x0f),flg>>4) );
	    else continue;
	if((flg>>4)&Attr::IsUser)
	{	
	    attrAt(sid).at().setS(TSYS::strSepParse(c_el.cfg("IO_VAL").getS(),0,'|'));
	    attrAt(sid).at().fld().values(TSYS::strSepParse(c_el.cfg("IO_VAL").getS(),1,'|'));
	    attrAt(sid).at().fld().selNames(TSYS::strSepParse(c_el.cfg("IO_VAL").getS(),2,'|'));	
	}
	else attrAt(sid).at().setS(c_el.cfg("IO_VAL").getS());
	
	attrAt(sid).at().flgSelf((Attr::SelfAttrFlgs)c_el.cfg("SELF_FLG").getI());
	attrAt(sid).at().lnkTempl(c_el.cfg("LNK_TMPL").getS());
	attrAt(sid).at().lnkVal(c_el.cfg("LNK_VAL").getS());
	
	attrAt(sid).at().modifVal(0);
	attrAt(sid).at().modifCfg(0);
    }
}

void Widget::save( )
{
    //Save generic widget's data
    string db  = libLocated() ? ownLib().DB() : ownWdg().ownLib().DB();
    string tbl = libLocated() ? ownLib().tbl() : ownWdg().ownLib().tbl()+"_incl"; 
    SYS->db().at().dataSet(db+"."+tbl,mod->nodePath()+tbl,*this);
    
    //Save widget's attributes
    saveIO();
    
    //Load cotainer widgets
    if(container())
    {
	vector<string> ls;
	wdgList(ls);
	for( int i_l = 0; i_l < ls.size(); i_l++ )
	    wdgAt(ls[i_l]).at().save();
    }
}

void Widget::saveIO( )
{
    TConfig c_el(&mod->elWdgIO());
    string db  = libLocated() ? ownLib().DB() : ownWdg().ownLib().DB();
    string tbl = (libLocated() ? ownLib().tbl() : ownWdg().ownLib().tbl())+"_io"; 
    
    //Save allow IO
    vector<string> atrr_ls;
    attrList(atrr_ls);    
    c_el.cfg("IDW").setS( libLocated() ? id() : ownWdg().id() );
    for( int i_io = 0; i_io < atrr_ls.size(); i_io++ )
	if( attrAt(atrr_ls[i_io]).at().modifVal() || attrAt(atrr_ls[i_io]).at().modifCfg() )
	{
	    c_el.cfg("ID").setS( libLocated() ? atrr_ls[i_io] : id()+"_"+atrr_ls[i_io] );
	    c_el.cfg("IO_TYPE").setI(0);
 	    if(attrAt(atrr_ls[i_io]).at().fld().flg()&Attr::IsUser)
	    {
		c_el.cfg("IO_VAL").setS(attrAt(atrr_ls[i_io]).at().getS()+"|"+
					attrAt(atrr_ls[i_io]).at().fld().values()+"|"+
					attrAt(atrr_ls[i_io]).at().fld().selNames());
		c_el.cfg("NAME").setS(attrAt(atrr_ls[i_io]).at().name());
		c_el.cfg("IO_TYPE").setI(attrAt(atrr_ls[i_io]).at().fld().type()+(attrAt(atrr_ls[i_io]).at().fld().flg()<<4));
    	    }
	    else c_el.cfg("IO_VAL").setS(attrAt(atrr_ls[i_io]).at().getS());
	    c_el.cfg("SELF_FLG").setI(attrAt(atrr_ls[i_io]).at().flgSelf());
	    c_el.cfg("LNK_TMPL").setS(attrAt(atrr_ls[i_io]).at().lnkTempl());
	    c_el.cfg("LNK_VAL").setS(attrAt(atrr_ls[i_io]).at().lnkVal());
	     
	    SYS->db().at().dataSet(db+"."+tbl,mod->nodePath()+tbl,c_el);
	    attrAt(atrr_ls[i_io]).at().modifVal(0);
	    attrAt(atrr_ls[i_io]).at().modifCfg(0);
	}
    //Clear no present IO
    int fld_cnt=0;
    c_el.cfg("ID").setS("");
    while( SYS->db().at().dataSeek(db+"."+tbl,mod->nodePath()+tbl,fld_cnt++,c_el) )
    {
        string sid = c_el.cfg("ID").getS();
	c_el.cfg("ID").setS("");
	if( container() && wdgPresent(TSYS::strSepParse(sid,0,'_')) )	continue;
	if(!libLocated())
	    if(sid.find(id()+"_")==0)	sid.erase(0,id().size()+1);
	    else continue;
	//Erase record from table
        if(!attrPresent(sid))
        {
            SYS->db().at().dataDel(db+"."+tbl,mod->nodePath()+tbl,c_el);
            fld_cnt--;
        }
    }
}

void Widget::enable( bool val )
{
    if( enable() == val ) return;
    
    if(!libLocated()&&original()=="Base")
    {
	if(val)	
	{
	    //- Connect to original widget -
	    m_srcWdg=mod->wlbAt(TSYS::strSepParse(srcWdgNm(),0,'.')).at().at(TSYS::strSepParse(srcWdgNm(),1,'.'));
	    
	    //- Free attributes -
	    ResAlloc res(attr_res,true);
	    TAttrMap::iterator p;
	    while( (p=attrs.begin())!=attrs.end() )
	    {
    		delete p->second;
    		attrs.erase(p);
	    }    
	    attr_cfg->valDet(this);
	    delete attr_cfg;
	    attr_cfg = srcWdg().at().attrCfg();
	    //- Attach to source -
	    attr_cfg->valAtt(this);
	    for(int i=0; i < attr_cfg->fldSize(); i++)
		attrs.insert( std::pair<string,Attr*>(attr_cfg->fldAt(i).name(),new Attr(attr_cfg->fldAt(i),this)) );
	    res.release();
	    //- Copy attribute's values from the original widget -
	    (*this) = srcWdg().at();
	    //- Load self values from DB -
	    loadIO();
	}
	else
	{
	    //- Free attributes -
            ResAlloc res(attr_res,true);
	    TAttrMap::iterator p;
	    while( (p=attrs.begin())!=attrs.end() )
	    {
    		delete p->second;
    		attrs.erase(p);
	    }    
	    attr_cfg->valDet(this);	    
	    m_srcWdg.free();
	    //- Add local attributes -
    	    attr_cfg = new TElem;
	    res.release();
	    attr_cfg->valAtt(this);    
	}
    }
    
    //- Enable process widgets into container -
    if(container())
    {
	vector<string>	ls;
	wdgList(ls);
	for(int i_l = 0; i_l < ls.size(); i_l++ )
	    wdgAt(ls[i_l]).at().enable(val);
    }
    
    m_enable = val;
}

void Widget::attrProc( Widget *uwdg, bool init )
{
    if(init)
    {
	attrAdd( new TFld("name",_("Name"),TFld::String,TFld::NoFlag,"","") );
	attrAdd( new TFld("en",_("Enabled"),TFld::Boolean,TFld::NoFlag,"","1") );
	attrAdd( new TFld("active",_("Active"),TFld::Boolean,TFld::NoFlag,"","0") );
	attrAdd( new TFld("geomX",_("Geometry:x"),TFld::Integer,TFld::NoFlag,"","0","0;10000") );
	attrAdd( new TFld("geomY",_("Geometry:y"),TFld::Integer,TFld::NoFlag,"","0","0;10000") );
	attrAdd( new TFld("geomW",_("Geometry:width"),TFld::Integer,TFld::NoFlag,"","10","0;10000") );
	attrAdd( new TFld("geomH",_("Geometry:height"),TFld::Integer,TFld::NoFlag,"","10","0;10000") );
	attrAdd( new TFld("geomZ",_("Geometry:level"),TFld::Integer,TFld::NoFlag,"","0","0;10") );
    }
}

void Widget::attrList( vector<string> &list )
{
    list.clear();
    ResAlloc res(attr_res,false);
    attr_cfg->fldList(list);
}

bool Widget::attrPresent( const string &n_val )
{
    ResAlloc res(attr_res,false);
    TAttrMap::iterator p=attrs.find(n_val);
    if(p==attrs.end())	return false;
    return true;
}

void Widget::attrAdd( TFld *attr )
{
    if(attrPresent(attr->name()))
    {
	delete attr;
        throw TError(nodePath().c_str(),_("Attribut %s already present."),attr->name().c_str());
    }
    attr_cfg->fldAdd(attr);
}
		    
void Widget::attrDel( const string &attr )
{
    if(attrPresent(attr))
        attr_cfg->fldDel(attr_cfg->fldId(attr));
}

AutoHD<Attr> Widget::attrAt( const string &n_val )
{
    ResAlloc res(attr_res,false);
    TAttrMap::iterator p=attrs.find(n_val);
    if(p==attrs.end())	throw TError(nodePath().c_str(),_("Attribute %s no present!"),n_val.c_str());
    return  AutoHD<Attr>(p->second);
}

bool Widget::attrChange( Attr &cfg )
{
    return true;
}

void Widget::wdgList( vector<string> &list )
{
    if( !container() )	throw TError(nodePath().c_str(),_("No container widget!")); 
    chldList(inclWdg,list);
}

bool Widget::wdgPresent( const string &wdg )
{
    if( !container() )  throw TError(nodePath().c_str(),_("No container widget!")); 
    return chldPresent(inclWdg,wdg); 
}

void Widget::wdgAdd( const string &wid, const string &path )
{
    if( !container() )  throw TError(nodePath().c_str(),_("No container widget!"));
    if( wdgPresent(wid) ) return;
	
    if( mod->origPresent(path) ) chldAdd(inclWdg,mod->origGet(path,wid,false));
    else chldAdd(inclWdg,new Widget(wid,false,path));    
}

void Widget::wdgDel( const string &wid, bool full )
{ 
    if( !container() )  throw TError(nodePath().c_str(),_("No container widget!"));
    chldDel(inclWdg,wid,-1,full); 
}

AutoHD<Widget> Widget::wdgAt( const string &wdg )
{ 
    if( !container() )  throw TError(nodePath().c_str(),_("No container widget!"));
    return chldAt(inclWdg,wdg); 
}

void Widget::addFld( TElem *el, unsigned id )
{
    if( el == attr_cfg )
    {
	ResAlloc res(attr_res,true);
	attrs.insert( std::pair<string,Attr*>(el->fldAt(id).name(),new Attr(el->fldAt(id),this)) );
    }
    else TConfig::addFld(el,id);
}

void Widget::delFld( TElem *el, unsigned id )
{
    if( el == attr_cfg )
    {
	ResAlloc res(attr_res,true);
	TAttrMap::iterator p=attrs.find(el->fldAt(id).name());
	if(p==attrs.end())  return;
        delete p->second;
	attrs.erase(p);
    }
    else TConfig::delFld(el,id);
}

void Widget::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        ctrMkNode("oscada_cntr",opt,-1,"/",_("Widget: ")+id());
	if(container() && ctrMkNode("branches",opt,-1,"/br","",R_R_R_))
            ctrMkNode("grp",opt,-1,"/br/wdg_",_("Widget"),R_R_R_,"root","root",1,"list","/inclwdg/wdg");
	if(ico().size()) ctrMkNode("img",opt,-1,"/ico","",0444);
        if(ctrMkNode("area",opt,-1,"/wdg",_("Widget")))
	{
	    if(ctrMkNode("area",opt,-1,"/wdg/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/wdg/st/en",_("Enable"),RWRWR_,user().c_str(),grp().c_str(),1,"tp","bool");
		if(original()=="Base")
		{
		    ctrMkNode("fld",opt,-1,"/wdg/st/orig",_("Original"),permit(),user().c_str(),grp().c_str(),3,"tp","str","dest","sel_ed","select","/wdg/w_lst");
		    ctrMkNode("comm",opt,-1,"/wdg/st/goorig",_("Go to original"),permit(),user().c_str(),grp().c_str(),1,"tp","lnk");
		}
		else ctrMkNode("fld",opt,-1,"/wdg/st/orig",_("Original"),R_R_R_,"root","root",1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/wdg/cfg",_("Config")))
	    {
		ctrMkNode("fld",opt,-1,"/wdg/cfg/id",_("Id"),R_R_R_,user().c_str(),grp().c_str(),1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/name",_("Name"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
		ctrMkNode("fld",opt,-1,"/wdg/cfg/descr",_("Description"),permit(),user().c_str(),grp().c_str(),3,"tp","str","cols","50","rows","3");
		if(libLocated())
		{
		    ctrMkNode("img",opt,-1,"/wdg/cfg/ico",_("Icon"),permit(),user().c_str(),grp().c_str(),2,"v_sz","64","h_sz","64");    
		    ctrMkNode("fld",opt,-1,"/wdg/cfg/user",_("User and group"),RWRWR_,"root","root",3,"tp","str","dest","select","select","/wdg/u_lst");
		    ctrMkNode("fld",opt,-1,"/wdg/cfg/grp","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","str","dest","select","select","/wdg/g_lst");
		    ctrMkNode("fld",opt,-1,"/wdg/cfg/u_a",_("Access: user-grp-other"),RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/wdg/a_lst");
		    ctrMkNode("fld",opt,-1,"/wdg/cfg/g_a","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/wdg/a_lst");
		    ctrMkNode("fld",opt,-1,"/wdg/cfg/o_a","",RWR_R_,user().c_str(),grp().c_str(),3,"tp","dec","dest","select","select","/wdg/a_lst");
		}
		ctrMkNode("comm",opt,-1,"/wdg/cfg/load",_("Load"),permit(),user().c_str(),grp().c_str());
		ctrMkNode("comm",opt,-1,"/wdg/cfg/save",_("Save"),permit(),user().c_str(),grp().c_str());
	    }
	}
    	if(ctrMkNode("area",opt,-1,"/attr",_("Attributes")))
	{
	    //Properties form create
	    vector<string> list_a;
	    attrList(list_a);
	    for( unsigned i_el = 0; i_el < list_a.size(); i_el++ )
		attrAt(list_a[i_el]).at().fld().cntrCmdMake(opt,"/attr",-1,user().c_str(),grp().c_str(),permit());
	}
	if(ctrMkNode("area",opt,-1,"/links",_("Links")))
	{
	    ctrMkNode("fld",opt,-1,"/links/onlyAttr",_("Only atributes show"),permit(),user().c_str(),grp().c_str(),1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/links/lnk",_("Links")))
	    {
		vector<string> alist, list;
		attrList(alist);
		for( int i_a = 0; i_a < alist.size(); i_a++ )
                {
		    if( !(attrAt(alist[i_a]).at().flgSelf()&(Attr::LnkIn|Attr::LnkOut)) ) continue;
                    //Check select param
                    if( attrAt(alist[i_a]).at().lnkTempl().size() && !atoi(TBDS::genDBGet(mod->nodePath()+"onlAttr","0",opt->attr("user")).c_str()) )
                    {
                        string nprm = TSYS::strSepParse(attrAt(alist[i_a]).at().lnkTempl(),0,'|');
	                //Check already to present parameters
	                bool f_ok = false;
	                for( int i_l = 0; i_l < list.size(); i_l++ ) 
			    if( list[i_l] == nprm ) { f_ok = true; break; }
                        if(!f_ok)
                        {
                    	    ctrMkNode("fld",opt,-1,(string("/links/lnk/pr_")+alist[i_a]).c_str(),nprm,permit(),user().c_str(),grp().c_str(),
                                3,"tp","str","dest","sel_ed","select",(string("/links/lnk/pl_")+alist[i_a]).c_str());
                    	    list.push_back(nprm);
                        }
		    }
		    else ctrMkNode("fld",opt,-1,(string("/links/lnk/el_")+alist[i_a]).c_str(),
				    attrAt(alist[i_a]).at().fld().name(),permit(),user().c_str(),grp().c_str(),
				    3,"tp","str","dest","sel_ed","select",(string("/links/lnk/ls_")+alist[i_a]).c_str());
		}        	
	    }
	}
	if(libLocated()&&ctrMkNode("area",opt,-1,"/attrcfg",_("Attributes configuration")))
	{
            if(ctrMkNode("table",opt,-1,"/attrcfg/attr",_("Attributes"),permit(),user().c_str(),grp().c_str(),2,"s_com","add,del","key","id"))
            {
        	ctrMkNode("list",opt,-1,"/attrcfg/attr/id",_("Id"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
	        ctrMkNode("list",opt,-1,"/attrcfg/attr/name",_("Name"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
        	ctrMkNode("list",opt,-1,"/attrcfg/attr/type",_("Type"),permit(),user().c_str(),grp().c_str(),4,"tp","dec","idm","1","dest","select","select","/attrcfg/tp_ls");
		ctrMkNode("list",opt,-1,"/attrcfg/attr/wa",_("Work area"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
		ctrMkNode("list",opt,-1,"/attrcfg/attr/calc",_("Calcul"),permit(),user().c_str(),grp().c_str(),1,"tp","bool");
		ctrMkNode("list",opt,-1,"/attrcfg/attr/lnk",_("Link"),permit(),user().c_str(),grp().c_str(),4,"tp","dec","idm","1","dest","select","select","/attrcfg/lnk_ls");
		ctrMkNode("list",opt,-1,"/attrcfg/attr/lnktmpl",_("Link template"),permit(),user().c_str(),grp().c_str(),1,"tp","str");
	    }
	    if(ctrMkNode("area",opt,-1,"/attrcfg/calc",_("Calculate")))
	    {
		ctrMkNode("fld",opt,-1,"/attrcfg/calc/progLng",_("Programm language"),permit(),user().c_str(),grp().c_str(),3,"tp","str","dest","sel_ed","select","/attrcfg/calc/plangLs");
		ctrMkNode("fld",opt,-1,"/attrcfg/calc/prog",_("Programm"),permit(),user().c_str(),grp().c_str(),3,"tp","str","cols","90","rows","8");
	    }
	}
        if(container()&&ctrMkNode("area",opt,-1,"/inclwdg",_("Include widgets")))
	{
	    ctrMkNode("fld",opt,-1,"/inclwdg/iwdg",_("Include widget"),permit(),user().c_str(),grp().c_str(),3,"tp","str","dest","sel_ed","select","/inclwdg/iw_lst");
	    ctrMkNode("list",opt,-1,"/inclwdg/wdg",_("Widgets"),permit(),user().c_str(),grp().c_str(),4,"tp","br","idm","1","s_com","add,del","br_pref","wdg_");
	}	
        return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/wdg/st/en" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,user().c_str(),grp().c_str(),SEQ_RD) ) opt->text(TSYS::int2str(enable()));
        if( ctrChkNode(opt,"set",RWRWR_,user().c_str(),grp().c_str(),SEQ_WR) ) enable(atoi(opt->text().c_str()));
    }
    else if( a_path == "/wdg/st/orig" )
    {
	if(original()!="Base")
	{	
	    if( ctrChkNode(opt,"get",RWRWR_,"root","root",SEQ_RD) )	opt->text(original());
	}
	else
	{
	    if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )	opt->text(srcWdgNm());
	    if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )	srcWdgNm(opt->text());
	}
    }
    else if( a_path == "/wdg/st/goorig" && ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
        opt->text("/UI/mod_"+mod->modId()+"/wlb_"+TSYS::strSepParse(srcWdgNm(),0,'.')+"/wdg_"+TSYS::strSepParse(srcWdgNm(),1,'.'));
    else if( a_path == "/wdg/cfg/user" )
    {
	if( ctrChkNode(opt,"get",RWRWR_,"root","root",SEQ_RD) )	opt->text(user());
	if( ctrChkNode(opt,"set",RWRWR_,"root","root",SEQ_WR) )	user(opt->text());
    }
    else if( a_path == "/wdg/cfg/grp" )
    {
        if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),grp().c_str(),SEQ_RD) )	opt->text(grp());
        if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),grp().c_str(),SEQ_WR) )  	grp(opt->text());
    }
    else if( a_path == "/wdg/cfg/u_a" || a_path == "/wdg/cfg/g_a" || a_path == "/wdg/cfg/o_a" )
    {
        if( ctrChkNode(opt,"get",RWR_R_,user().c_str(),grp().c_str(),SEQ_RD) )
        {
            if( a_path == "/wdg/cfg/u_a" )       opt->text(TSYS::int2str((permit()>>6)&0x7));
            if( a_path == "/wdg/cfg/g_a" )       opt->text(TSYS::int2str((permit()>>3)&0x7));
            if( a_path == "/wdg/cfg/o_a" )       opt->text(TSYS::int2str(permit()&0x7));
        }
        if( ctrChkNode(opt,"set",RWR_R_,user().c_str(),grp().c_str(),SEQ_WR) )
        {
            if( a_path == "/wdg/cfg/u_a" )       permit( (permit()&(~(0x07<<6)))|(atoi(opt->text().c_str())<<6) );
            if( a_path == "/wdg/cfg/g_a" )       permit( (permit()&(~(0x07<<3)))|(atoi(opt->text().c_str())<<3) );
            if( a_path == "/wdg/cfg/o_a" )       permit( (permit()&(~0x07))|atoi(opt->text().c_str()) );
        }
    }
    else if( a_path == "/wdg/cfg/ico" || a_path == "/ico" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )   opt->text(ico());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )   ico(opt->text());
    }
    else if( a_path == "/wdg/cfg/id" && ctrChkNode(opt,"get",R_R_R_,user().c_str(),grp().c_str()) )	opt->text(id());
    else if( a_path == "/wdg/cfg/name" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )   opt->text(name());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )   name(opt->text());
    }
    else if( a_path == "/wdg/cfg/descr" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )   opt->text(descr());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )   descr(opt->text());
    }
    else if( a_path == "/wdg/cfg/load" && ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )	load();
    else if( a_path == "/wdg/cfg/save" && ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )	save();
    else if( a_path == "/wdg/u_lst" && ctrChkNode(opt) )
    {
        vector<string> ls;
        SYS->security().at().usrList(ls);
        for(int i_l = 0; i_l < ls.size(); i_l++)
        opt->childAdd("el")->text(ls[i_l]);
    }
    else if( a_path == "/wdg/g_lst" && ctrChkNode(opt) )
    {
        vector<string> ls;
        SYS->security().at().usrGrpList(user(), ls );
        for(int i_l = 0; i_l < ls.size(); i_l++)
        opt->childAdd("el")->text(ls[i_l]);
    }
    else if( a_path == "/wdg/a_lst" && ctrChkNode(opt) )
    {
        opt->childAdd("el")->attr("id","0")->text(_("No access"));
        opt->childAdd("el")->attr("id","4")->text(_("Use(open)"));
        opt->childAdd("el")->attr("id","2")->text(_("Modify"));
        opt->childAdd("el")->attr("id","6")->text(_("Full"));
    }
    else if( a_path.substr(0,5) == "/attr" && TSYS::pathLev(a_path,1).size() > 4 && TSYS::pathLev(a_path,1).substr(0,4) == "sel_" && TCntrNode::ctrChkNode(opt) )
    {
        AutoHD<Attr> attr = attrAt(TSYS::pathLev(a_path,1).substr(4));
        for( int i_a=0; i_a < attr.at().fld().selNm().size(); i_a++ )
	    opt->childAdd("el")->text(attr.at().fld().selNm()[i_a]);
    }
    else if( a_path.substr(0,6) == "/attr/" )
    {
	AutoHD<Attr> attr = attrAt(TSYS::pathLev(a_path,1));
	if( ctrChkNode(opt,"get",(attr.at().fld().flg()&TFld::NoWrite)?(permit()&~0222):permit(),user().c_str(),grp().c_str(),SEQ_RD) )
        {
	    if( attr.at().fld().flg()&TFld::Selected )	opt->text(attr.at().getSEL());
            else                                    	opt->text(attr.at().getS());
	}
	if( ctrChkNode(opt,"set",(attr.at().fld().flg()&TFld::NoWrite)?(permit()&~0222):permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{
	    if( attr.at().fld().flg()&TFld::Selected )	attr.at().setSEL(opt->text());
            else                                    	attr.at().setS(opt->text());
	}
    }
    else if( a_path == "/links/onlyAttr" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	    opt->text(TBDS::genDBGet(mod->nodePath()+"onlAttr","0",opt->attr("user")));
	if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	    TBDS::genDBSet(mod->nodePath()+"onlAttr",opt->text(),opt->attr("user"));
    }
    else if( a_path.substr(0,14) == "/links/lnk/pr_" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	{
	    string lnk_val = attrAt(a_path.substr(14)).at().lnkVal();	    
	    int c_lvl = 0;
	    while(TSYS::strSepParse(lnk_val,c_lvl,'.').size())	c_lvl++;
	    if( (TSYS::strSepParse(lnk_val,0,'.') == "P" && c_lvl==5) || 
		    (TSYS::strSepParse(lnk_val,0,'.') == "W" && c_lvl==3) )
		opt->text(lnk_val.substr(0,lnk_val.rfind(".")));
	    else opt->text(lnk_val);
	}
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{	
	    vector<string> a_ls;
	    string no_set;
	    bool noonly_no_set = true;
	    string lnk_val = opt->text();
	    string p_nm = TSYS::strSepParse(attrAt(a_path.substr(14)).at().lnkTempl(),0,'|');
	    int c_lvl = 0;
	    while(TSYS::strSepParse(lnk_val,c_lvl,'.').size())  c_lvl++;
	    
	    attrList(a_ls);
	    
	    AutoHD<TValue> prm;
	    if(TSYS::strSepParse(lnk_val,0,'.') == "P" && c_lvl==4)
		prm = SYS->daq().at().at(TSYS::strSepParse(lnk_val,1,'.')).at().at(TSYS::strSepParse(lnk_val,2,'.')).at().
			at(TSYS::strSepParse(lnk_val,3,'.'));
	    AutoHD<Widget> wdg;
	    if(TSYS::strSepParse(lnk_val,0,'.') == "W" && c_lvl==2)
	    {
		string nwdg = TSYS::strSepParse(lnk_val,1,'.');
		if(nwdg=="base") wdg = libLocated() ? this : &ownWdg();
		else		 wdg = libLocated() ? wdgAt(nwdg) : ownWdg().wdgAt(nwdg);
	    }	
	    
	    for( int i_a = 0; i_a < a_ls.size(); i_a++ )
		if( p_nm == TSYS::strSepParse(attrAt(a_ls[i_a]).at().lnkTempl(),0,'|') )
		{
		    attrAt(a_ls[i_a]).at().lnkVal(lnk_val);
		    string p_attr = TSYS::strSepParse(attrAt(a_ls[i_a]).at().lnkTempl(),1,'|');
		    if(!prm.freeStat() || !wdg.freeStat())
		    {
			if( (!prm.freeStat() && prm.at().vlPresent(p_attr)) || 
			    (!wdg.freeStat() && wdg.at().attrPresent(p_attr)) )
		    	{
			    attrAt(a_ls[i_a]).at().lnkVal(lnk_val+"."+p_attr);
			    noonly_no_set = false;
			}
			else no_set+=p_attr+",";
		    }
		}
	    if(!prm.freeStat() || !wdg.freeStat())
	    {
		if( noonly_no_set )     throw TError(nodePath().c_str(),_("Parameter have not only atributes!"));
                else if( no_set.size() )throw TError(nodePath().c_str(),_("Parameter have not atributes: %s !"),no_set.c_str());
	    }
	}
    }
    else if( (a_path.substr(0,14) == "/links/lnk/pl_" || a_path.substr(0,14) == "/links/lnk/ls_") 
	    && ctrChkNode(opt) )
    {
	string m_prm = attrAt(a_path.substr(14)).at().lnkVal();
	bool is_pl = (a_path.substr(0,14) == "/links/lnk/pl_");
        int c_lv;
	string c_path = "";
	opt->childAdd("el")->text(c_path);
	string obj_tp = TSYS::strSepParse(m_prm,0,'.');
	for( c_lv = 0; TSYS::strSepParse(m_prm,c_lv,'.').size(); c_lv++ )
	{
	    if( is_pl && ((obj_tp=="P" && c_lv>3) || (obj_tp=="W" && c_lv)) )	break;
	    if( c_lv ) c_path+=".";
	    c_path = c_path+TSYS::strSepParse(m_prm,c_lv,'.');
	    opt->childAdd("el")->text(c_path);
	}
	if(c_lv) c_path+=".";
	string prm1 = TSYS::strSepParse(m_prm,1,'.');
	string prm2 = TSYS::strSepParse(m_prm,2,'.');
	string prm3 = TSYS::strSepParse(m_prm,3,'.');
	vector<string>	ls;
	switch(c_lv)
	{
	    case 0:	ls.push_back("P"); ls.push_back("W"); break;
	    case 1:
		if(obj_tp=="P")	SYS->daq().at().modList(ls);
		if(obj_tp=="W")
		{
		    if(libLocated()) wdgList(ls); 
		    else ownWdg().wdgList(ls);
		    ls.push_back("base");
		}
		break;
	    case 2:	
		if(obj_tp=="P" && SYS->daq().at().modPresent(prm1))
		    SYS->daq().at().at(prm1).at().list(ls);
		if(!is_pl && obj_tp=="W")
		{
		    AutoHD<Widget> wdg;
		    if(prm1=="base") wdg = libLocated() ? this : &ownWdg();
                    else             wdg = libLocated() ? wdgAt(prm1) : ownWdg().wdgAt(prm1);
		    if(!wdg.freeStat())	wdg.at().attrList(ls);
		}
		break;
	    case 3:
		if(obj_tp=="P" && SYS->daq().at().modPresent(prm1) 
			&& SYS->daq().at().at(prm1).at().present(prm2))
		    SYS->daq().at().at(prm1).at().at(prm2).at().list(ls);
		break;
	    case 4:
 		if(!is_pl && obj_tp=="P" && SYS->daq().at().modPresent(prm1) 
			&& SYS->daq().at().at(prm1).at().present(prm2) 
			&& SYS->daq().at().at(prm1).at().at(prm2).at().present(prm3) )
		    SYS->daq().at().at(prm1).at().at(prm2).at().at(prm3).at().vlList(ls);
		break;        	
	}
	for(int i_l = 0; i_l < ls.size(); i_l++)
	    opt->childAdd("el")->text(c_path+ls[i_l]);
    }
    else if( a_path.substr(0,14) == "/links/lnk/el_" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) ) 
	    opt->text(attrAt(a_path.substr(14)).at().lnkVal());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) 
	    attrAt(a_path.substr(14)).at().lnkVal(opt->text());
    }
    else if( libLocated() && a_path == "/attrcfg/attr" )
    {
	if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
	{
    	    XMLNode *n_id      = ctrMkNode("list",opt,-1,"/attrcfg/attr/id","");
	    XMLNode *n_name    = ctrMkNode("list",opt,-1,"/attrcfg/attr/name","");
	    XMLNode *n_type    = ctrMkNode("list",opt,-1,"/attrcfg/attr/type","");
	    XMLNode *n_wa      = ctrMkNode("list",opt,-1,"/attrcfg/attr/wa","");
	    XMLNode *n_calc    = ctrMkNode("list",opt,-1,"/attrcfg/attr/calc","");
	    XMLNode *n_lnk     = ctrMkNode("list",opt,-1,"/attrcfg/attr/lnk","");
	    XMLNode *n_lnktmpl = ctrMkNode("list",opt,-1,"/attrcfg/attr/lnktmpl","");
           
	    vector<string> lst;
	    //Self widget's attributes
	    attrList(lst);
	    for(int i_el = 0; i_el < lst.size(); i_el++)
	    {
		if( n_id )	n_id->childAdd("el")->text(lst[i_el]);
		if( n_name )    n_name->childAdd("el")->text(attrAt(lst[i_el]).at().name());
		if( n_type )	n_type->childAdd("el")->text(TSYS::int2str(attrAt(lst[i_el]).at().fld().type()+
				    ((attrAt(lst[i_el]).at().fld().flg()&(TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address))<<4)));
		if( n_wa )	n_wa->childAdd("el")->text( attrAt(lst[i_el]).at().fld().values()+"|"+
							    attrAt(lst[i_el]).at().fld().selNames());
		if( n_calc )	n_calc->childAdd("el")->text(TSYS::int2str(attrAt(lst[i_el]).at().flgSelf()&Attr::AttrCalc));
		if( n_lnk )	n_lnk->childAdd("el")->text(TSYS::int2str(attrAt(lst[i_el]).at().flgSelf()&(Attr::LnkIn|Attr::LnkOut|Attr::LnkGlob)));
		if( n_lnktmpl )	n_lnktmpl->childAdd("el")->text(attrAt(lst[i_el]).at().lnkTempl());		
	    }
	    //Include widget's attributes
	    if(container())
	    {
		wdgList(lst);
		for(int i_aw = 0; i_aw < lst.size(); i_aw++)
		{
		    vector<string> alst;
		    wdgAt(lst[i_aw]).at().attrList(alst);
		    for(int i_ai = 0; i_ai < alst.size(); i_ai++)
		    {
			if( n_id )	n_id->childAdd("el")->text(lst[i_aw]+"_"+alst[i_ai]);
			if( n_name )	n_name->childAdd("el")->text(wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().name());
			if( n_type )    n_type->childAdd("el")->text(TSYS::int2str(wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().fld().type()+
                                    	    ((wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().fld().flg()&(TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address))<<4)));
			if( n_wa )	n_wa->childAdd("el")->text( wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().fld().values()+"|"+
								    wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().fld().selNames());
			if( n_calc )	n_calc->childAdd("el")->text(TSYS::int2str(wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().flgSelf()&Attr::AttrCalc));
			if( n_lnk )	n_lnk->childAdd("el")->text(TSYS::int2str(wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().flgSelf()&(Attr::LnkIn|Attr::LnkOut|Attr::LnkGlob)));
			if( n_lnktmpl )	n_lnktmpl->childAdd("el")->text(wdgAt(lst[i_aw]).at().attrAt(alst[i_ai]).at().lnkTempl());
		    }
		}
	    }
	}
        if( ctrChkNode(opt,"add",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
        {
            attrAdd(new TFld("newAttr",_("New attribute"),TFld::String,Attr::IsUser));
            attrAt("newAttr").at().modifCfg(1);
        }
        if( ctrChkNode(opt,"del",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
        {
            if(!attrPresent(opt->attr("key_id")))
                throw TError(nodePath().c_str(),_("Delete a include widget's element is error."));
            if(attrAt(opt->attr("key_id")).at().fld().flg()&Attr::IsUser) attrDel(opt->attr("key_id"));
            else throw TError(nodePath().c_str(),_("Delete a not user element is error."));
        }	
	if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
        {
            //Request data
            string idattr = opt->attr("key_id");
            string idcol  = opt->attr("col");	    
	    
	    if( idcol == "id" || idcol == "name" || idcol == "type" || idcol == "wa" )
	    {
                string          tnm     = attrAt(idattr).at().name();
                TFld::Type      ttp     = attrAt(idattr).at().fld().type();
                unsigned        tflg    = attrAt(idattr).at().fld().flg();
			    
                if(!attrPresent(idattr) || !(attrAt(idattr).at().fld().flg()&Attr::IsUser) )
            	    throw TError(nodePath().c_str(),_("Change a no user attribute is no permit"));
                if( idcol == "id" )
                {
                    attrAdd( new TFld(opt->text().c_str(),tnm.c_str(),ttp,tflg|Attr::IsUser) );
            	    attrAt(opt->text().c_str()).at().modifCfg(1);
                    attrDel(idattr);
            	}
            	else if( idcol == "name" )      attrAt(idattr).at().fld().descr(opt->text().c_str());
                else if( idcol == "type" )
                {
                    TFld::Type  tp  = (TFld::Type)(atoi(opt->text().c_str())&0x0f);
            	    unsigned    flg = (atoi(opt->text().c_str())>>4)|Attr::IsUser;
	            if(tp!=ttp || (tflg^flg)&TFld::Selected )
	            {
	                attrDel(idattr);
	                attrAdd( new TFld(idattr.c_str(),tnm.c_str(),tp,tflg^((tflg^flg)&(TFld::Selected|Attr::Color|Attr::Image|Attr::Font|Attr::Address|Attr::IsUser))) );
	                attrAt(idattr.c_str()).at().modifCfg(1);
	            }
	            else if( (tflg^flg)&(Attr::Color|Attr::Image|Attr::Font|Attr::Address) )
	                attrAt(idattr).at().fld().flg(attrAt(idattr).at().fld().flg()^((tflg^flg)&(Attr::Color|Attr::Image|Attr::Font|Attr::Address)));
	        }
		else if( idcol == "wa" )
		{
		    attrAt(idattr).at().fld().values(TSYS::strSepParse(opt->text().c_str(),0,'|'));
		    attrAt(idattr).at().fld().selNames(TSYS::strSepParse(opt->text().c_str(),1,'|'));
		}
	    }
	    else
	    {	    
		AutoHD<Attr> attr;
		if(attrPresent(idattr) || !container() )
		    attr = attrAt(idattr);
		else attr = wdgAt(TSYS::strSepParse(idattr,0,'_')).at().attrAt(TSYS::strSepParse(idattr,1,'_'));
	    
		Attr::SelfAttrFlgs sflg = attr.at().flgSelf();
	    
		if( idcol == "calc" )
		{
		    Attr::SelfAttrFlgs stflg = atoi(opt->text().c_str())?Attr::AttrCalc:(Attr::SelfAttrFlgs)0;
		    attr.at().flgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&Attr::AttrCalc)) );
		}
		else if( idcol == "lnk" )
        	{
		    Attr::SelfAttrFlgs stflg = (Attr::SelfAttrFlgs)atoi(opt->text().c_str());
		    if( (sflg^stflg)&(Attr::LnkIn|Attr::LnkOut|Attr::LnkGlob) )
			attr.at().flgSelf( (Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&(Attr::LnkIn|Attr::LnkOut|Attr::LnkGlob))) );
		}
		else if( idcol == "lnktmpl" )	attr.at().lnkTempl(opt->text());
	    }
	}
    }
    else if( a_path == "/attrcfg/calc/progLng" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )	opt->text(progLang());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )	progLang(opt->text());
    }
    else if( a_path == "/attrcfg/calc/prog" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )	opt->text(prog());
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )  	 prog(opt->text());
    }
    else if( a_path == "/attrcfg/calc/plangLs" && ctrChkNode(opt) )
    {
	string tplng = progLang();
        int c_lv = 0;
        string c_path = "";
	opt->childAdd("el")->text(c_path);
        while(TSYS::strSepParse(tplng,c_lv,'.').size())
        {
            if( c_lv ) c_path+=".";
            c_path = c_path+TSYS::strSepParse(tplng,c_lv,'.');
            opt->childAdd("el")->text(c_path);
            c_lv++;
        }
        if(c_lv) c_path+=".";
        vector<string>  ls;
        switch(c_lv)
        {
            case 0:     SYS->daq().at().modList(ls);    break;
            case 1:
                if(SYS->daq().at().modPresent(TSYS::strSepParse(tplng,0,'.')))
                    SYS->daq().at().at(TSYS::strSepParse(tplng,0,'.')).at().compileFuncLangs(ls);
                break;
        }
        for(int i_l = 0; i_l < ls.size(); i_l++)
            opt->childAdd("el")->text(c_path+ls[i_l]);
    }
    else if( a_path == "/attrcfg/tp_ls" && ctrChkNode(opt) )
    {
        opt->childAdd("el")->attr("id",TSYS::int2str(TFld::Boolean))->text(_("Boolean"));
        opt->childAdd("el")->attr("id",TSYS::int2str(TFld::Integer))->text(_("Integer"));
        opt->childAdd("el")->attr("id",TSYS::int2str(TFld::Real))->text(_("Real"));
        opt->childAdd("el")->attr("id",TSYS::int2str(TFld::String))->text(_("String"));
        opt->childAdd("el")->attr("id",TSYS::int2str(TFld::Integer+(TFld::Selected<<4)))->text(_("Select integer"));
        opt->childAdd("el")->attr("id",TSYS::int2str(TFld::Real+(TFld::Selected<<4)))->text(_("Select real"));
        opt->childAdd("el")->attr("id",TSYS::int2str(TFld::String+(TFld::Selected<<4)))->text(_("Select string"));
	opt->childAdd("el")->attr("id",TSYS::int2str(TFld::String+(Attr::Color<<4)))->text(_("Color"));
	opt->childAdd("el")->attr("id",TSYS::int2str(TFld::String+(Attr::Image<<4)))->text(_("Image"));
        opt->childAdd("el")->attr("id",TSYS::int2str(TFld::String+(Attr::Font<<4)))->text(_("Font"));
        opt->childAdd("el")->attr("id",TSYS::int2str(TFld::String+(Attr::Address<<4)))->text(_("Address"));
    }
    else if( a_path == "/attrcfg/lnk_ls" && ctrChkNode(opt) )
    {
        opt->childAdd("el")->attr("id",TSYS::int2str(0))->text(_("No link"));
	opt->childAdd("el")->attr("id",TSYS::int2str(Attr::LnkIn))->text(_("Local input link"));
	opt->childAdd("el")->attr("id",TSYS::int2str(Attr::LnkOut))->text(_("Local output link"));
	opt->childAdd("el")->attr("id",TSYS::int2str(Attr::LnkIn|Attr::LnkOut))->text(_("Local full link"));
	opt->childAdd("el")->attr("id",TSYS::int2str(Attr::LnkIn|Attr::LnkGlob))->text(_("Global input link"));
	opt->childAdd("el")->attr("id",TSYS::int2str(Attr::LnkOut|Attr::LnkGlob))->text(_("Global output link"));
	opt->childAdd("el")->attr("id",TSYS::int2str(Attr::LnkIn|Attr::LnkOut|Attr::LnkGlob))->text(_("Global full link"));	
    }
    else if( container() && a_path == "/inclwdg/iwdg" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
            opt->text(TBDS::genDBGet(mod->nodePath()+"inclWdg","ElFigure",opt->attr("user")));
        if( ctrChkNode(opt,"set",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
            TBDS::genDBSet(mod->nodePath()+"inclWdg",opt->text(),opt->attr("user"));
    }
    else if( container() && a_path == "/inclwdg/wdg" )
    {
        if( ctrChkNode(opt,"get",permit(),user().c_str(),grp().c_str(),SEQ_RD) )
        {
            vector<string>  lst;
    	    wdgList(lst);
            for( unsigned i_f=0; i_f < lst.size(); i_f++ )
                opt->childAdd("el")->attr("id",lst[i_f])->text(wdgAt(lst[i_f]).at().name());
        }
        if( ctrChkNode(opt,"add",permit(),user().c_str(),grp().c_str(),SEQ_WR) )
	{
            wdgAdd(opt->attr("id").c_str(),TBDS::genDBGet(mod->nodePath()+"inclWdg","ElFigure",opt->attr("user")));
            wdgAt(opt->attr("id").c_str()).at().name(opt->text().c_str());
        }
	if( ctrChkNode(opt,"del",permit(),user().c_str(),grp().c_str(),SEQ_WR) ) wdgDel(opt->attr("id").c_str(),true);
    }
    else if( (a_path == "/wdg/w_lst" || (container()&&a_path == "/inclwdg/iw_lst")) && ctrChkNode(opt) )
    {
        int c_lv = 0;
        string c_path = "";
        string lnk = (a_path=="/inclwdg/iw_lst") ? TBDS::genDBGet(mod->nodePath()+"inclWdg","",opt->attr("user")) : srcWdgNm();
				    
	opt->childAdd("el")->text(c_path);
	while(TSYS::strSepParse(lnk,c_lv,'.').size())
	{
	    if( c_lv ) c_path+=".";
	    c_path = c_path+TSYS::strSepParse(lnk,c_lv,'.');
	    opt->childAdd("el")->text(c_path);
	    c_lv++;
	}
        if(c_lv) c_path+=".";
        vector<string>  ls;
        switch(c_lv)
        {
            case 0:
		if(a_path == "/inclwdg/iw_lst")
		{
            	    mod->origList(ls);
	    	    for(int i_l = 0; i_l < ls.size(); i_l++)
		        opt->childAdd("el")->text(c_path+ls[i_l]);
		}
		mod->wlbList(ls);
		break;
            case 1:     
	        if( !mod->origPresent(lnk) && mod->wlbPresent(TSYS::strSepParse(lnk,0,'.')) )
                    mod->wlbAt(TSYS::strSepParse(lnk,0,'.')).at().list(ls);
                break;
        }
        for(int i_l = 0; i_l < ls.size(); i_l++)
            opt->childAdd("el")->text(c_path+ls[i_l]);
    }
}

//===== Widget's atributes =====
Attr::Attr( TFld &ifld, Widget *owner ) : m_owner(owner), vl_modif(0), cfg_modif(0), self_flg((SelfAttrFlgs)0)
{
    //Chek for self field for dinamic elements
    if( ifld.flg()&TFld::SelfFld )
    {
        m_fld = new TFld();
        *m_fld = ifld;
    }
    else m_fld = &ifld;
			    
    switch(fld().type())
    {
        case TFld::String:
    	    m_val.s_val    = new string;
            *(m_val.s_val) = fld().def();
            break;
        case TFld::Integer:	m_val.i_val = atoi(m_fld->def().c_str());       break;
        case TFld::Real:        m_val.r_val = atof(m_fld->def().c_str());       break;
        case TFld::Boolean:	m_val.b_val = atoi(m_fld->def().c_str());       break;
    }
}

Attr::~Attr(  )
{
    if( fld().type() == TFld::String )	delete m_val.s_val;
    if( fld().flg()&TFld::SelfFld )	delete m_fld;
}

string Attr::id()
{
    return fld().name();
}

string Attr::name()
{
    return fld().descr();
}

int Attr::flgGlob()
{
    return fld().flg();
}

void Attr::flgSelf( SelfAttrFlgs flg )
{ 
    if(self_flg == flg)	return;
    self_flg = flg; 
    cfg_modif++;
}

string Attr::getSEL( )
{
    if( !(fld().flg()&TFld::Selected) )
	throw TError("Cfg",_("Element type no select!"));
    switch( fld().type() )
    {
        case TFld::String:      return fld().selVl2Nm(*m_val.s_val);
        case TFld::Integer:	return fld().selVl2Nm(m_val.i_val);
        case TFld::Real:        return fld().selVl2Nm(m_val.r_val);
        case TFld::Boolean:	return fld().selVl2Nm(m_val.b_val);
    }
}

string Attr::getS( )
{
    switch(fld().type())
    {
	case TFld::String:      return *m_val.s_val;
        case TFld::Integer:	return TSYS::int2str(m_val.i_val);
        case TFld::Real:        return TSYS::real2str(m_val.r_val);
        case TFld::Boolean:	return TSYS::int2str(m_val.b_val);
    }
}
									    
double Attr::getR( )
{
    switch(fld().type())
    {
        case TFld::String:      return atof(m_val.s_val->c_str());
        case TFld::Integer:	return m_val.i_val;
        case TFld::Real:        return m_val.r_val;
        case TFld::Boolean:	return m_val.b_val;
    }
}

int Attr::getI( )
{
    switch(fld().type())
    {
	case TFld::String:      return atoi(m_val.s_val->c_str());
        case TFld::Integer:	return m_val.i_val;
        case TFld::Real:        return (int)m_val.r_val;
        case TFld::Boolean:	return m_val.b_val;
    }
}
								    
bool Attr::getB( )
{
    switch(fld().type())
    {
        case TFld::String:      return atoi(m_val.s_val->c_str());
        case TFld::Integer:	return m_val.i_val;
        case TFld::Real:        return (int)m_val.r_val;
        case TFld::Boolean:	return m_val.b_val;
    }
}

void Attr::setSEL( const string &val )
{
    if( !(fld().flg()&TFld::Selected) )
        throw TError("Cfg",_("Element type no select!"));
    switch( fld().type() )
    {
	case TFld::String:      setS( fld().selNm2VlS(val) );  break;
        case TFld::Integer:	setI( fld().selNm2VlI(val) );  break;
        case TFld::Real:        setR( fld().selNm2VlR(val) );  break;
        case TFld::Boolean:	setB( fld().selNm2VlB(val) );  break;
    }
}
											
void Attr::setS( const string &val )
{
    switch( fld().type() )
    {
	case TFld::String:
	    if( *(m_val.s_val) == val )	break;
            if( fld().flg()&Attr::Prevent )
            {
                string t_str = *(m_val.s_val);
        	*(m_val.s_val) = val;
                if( !m_owner->attrChange(*this) )
            	    *(m_val.s_val) = t_str;
            }
    	    else *(m_val.s_val) = val;
	    vl_modif++;
            break;
        case TFld::Integer:	setI( atoi(val.c_str()) );      break;
	case TFld::Real:	setR( atof(val.c_str()) );      break;
        case TFld::Boolean:	setB( atoi(val.c_str()) );      break;
    }
}

void Attr::setR( double val )
{
    switch( fld().type() )
    {
        case TFld::String:      setS(TSYS::real2str(val));      break;
        case TFld::Integer:	setI( (int)val );       break;
        case TFld::Real:
    	    if( !(fld().flg()&TFld::Selected) && fld().selValR()[0] < fld().selValR()[1] )
    	    {
                val = vmax(val,fld().selValR()[0]);
                val = vmin(val,fld().selValR()[1]);
            }
	    if( m_val.r_val == val )	break;
            if( fld().flg()&Attr::Prevent )
            {
                double t_val = m_val.r_val;
                m_val.r_val = val;
                if( !m_owner->attrChange(*this) )
            	    m_val.r_val = t_val;
	    }
            else m_val.r_val = val;
	    vl_modif++;
            break;
	case TFld::Boolean:	setB( val );    break;
    }
}

void Attr::setI( int val )
{
    switch( fld().type() )
    {
        case TFld::String:      setS(TSYS::int2str(val));       break;
        case TFld::Integer:
            if( !(fld().flg()&TFld::Selected) && fld().selValI()[0] < fld().selValI()[1] )
            {
    		val = vmax(val,m_fld->selValI()[0]);
		val = vmin(val,m_fld->selValI()[1]);
            }
	    if( m_val.i_val == val )	break;
            if( fld().flg()&Attr::Prevent )
            {
                int t_val = m_val.i_val;
                m_val.i_val = val;
                if( !m_owner->attrChange(*this) )
            	    m_val.i_val = t_val;
	    }
            else m_val.i_val = val;
	    vl_modif++;
            break;
        case TFld::Real:        setR( val );    break;
        case TFld::Boolean:	setB( val );    break;
    }
}

void Attr::setB( bool val )
{
    switch( fld().type() )
    {
        case TFld::String:      setS(TSYS::int2str(val));       break;
        case TFld::Integer:    	setI( val );    break;
        case TFld::Real:        setR( val );    break;
        case TFld::Boolean:
	    if( m_val.b_val == val )	break;
    	    if( fld().flg()&Attr::Prevent )
            {
                bool t_val = m_val.b_val;
                m_val.b_val = val;
                if( !m_owner->attrChange(*this) )
                    m_val.b_val = t_val;
            }
    	    else m_val.b_val = val;
	    vl_modif++;
        break;
    }
}

void Attr::lnkTempl(const string &vl)
{
    if(lnk_tmpl == vl) return;
    lnk_tmpl = vl;
    cfg_modif++;
}

void Attr::lnkVal( const string &vl )
{
    if(lnk_val == vl) return;
    lnk_val = vl; 
    cfg_modif++;
}

void Attr::AHDConnect()
{
    ResAlloc::resRequestR(m_owner->attr_res);
}

void Attr::AHDDisConnect()
{
    ResAlloc::resReleaseR(m_owner->attr_res);
}
																									    
