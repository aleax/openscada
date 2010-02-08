
//OpenSCADA system file: tvalue.cpp
/***************************************************************************
 *   Copyright (C) 2003-2009 by Roman Savochenko                           *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#include "tsys.h"
#include "tmess.h"
#include "tarchval.h"
#include "tparamcontr.h"
#include "tcontroller.h"
#include "tvalue.h"

//*************************************************
//* TValue                                        *
//*************************************************
TValue::TValue( ) : l_cfg(0), mCfg(NULL)
{
    m_vl = grpAdd("a_",true);
}

TValue::~TValue()
{
    while( elem.size() ) vlElemDet(elem[0]);
}

TVal* TValue::vlNew( )
{
    return new TVal( );
}

void TValue::detElem( TElem *el )
{
    vlElemDet(el);
}

void TValue::addFld( TElem *el, unsigned id_val )
{
    int i_off = l_cfg;
    for( unsigned i_e = 0; i_e < elem.size(); i_e++ )
	if( elem[i_e]->elName() == el->elName() ) break;
	else i_off += elem[i_e]->fldSize();

    TVal *vl = vlNew();
    vl->setFld(el->fldAt(id_val));
    chldAdd(m_vl,vl,i_off+id_val);
}

void TValue::delFld( TElem *el, unsigned id_val )
{
    if( nodeMode() == TCntrNode::Enable && chldPresent(m_vl,el->fldAt(id_val).name()) ) 
	chldDel(m_vl,el->fldAt(id_val).name());
}

void TValue::setVlCfg( TConfig *cfg )
{
    vector<string> list;
    //> Detach old configs
    if( mCfg )
    {
	mCfg->cfgList( list );
	for( unsigned i_cf = 0; i_cf < list.size(); i_cf++ )
	    if( !(mCfg->cfg(list[i_cf]).fld().flg()&TCfg::NoVal) && vlPresent(list[i_cf]) )
	    {
		chldDel(m_vl,list[i_cf]);
		l_cfg--;
	    }
	mCfg = NULL;
    }
    //> Attach new config
    if( cfg )
    {
	cfg->cfgList( list );
	for( unsigned i_cf = 0; i_cf < list.size(); i_cf++ )
	    if( !(cfg->cfg(list[i_cf]).fld().flg()&TCfg::NoVal) && !vlPresent(list[i_cf]) )
		//chldAdd(m_vl, new TVal(cfg->cfg(list[i_cf]),this),l_cfg++);
	    {
		TVal *vl = vlNew();
		vl->setCfg(cfg->cfg(list[i_cf]));
		chldAdd(m_vl,vl,l_cfg);
		l_cfg++;
	    }
	mCfg = cfg;
    }
}

bool TValue::vlElemPresent( TElem *ValEl )
{
    for(int i_el = 0; i_el < elem.size(); i_el++)
	if( elem[i_el] == ValEl ) return true;
    return false;
}

void TValue::vlElemAtt( TElem *ValEl )
{
    ValEl->valAtt(this);
    for( unsigned i_elem = 0; i_elem < ValEl->fldSize(); i_elem++ )
	addFld(ValEl,i_elem);
    elem.push_back(ValEl);
}

void TValue::vlElemDet( TElem *ValEl )
{
    for(unsigned i_e = 0; i_e < elem.size(); i_e++)
	if(elem[i_e] == ValEl )
	{
	    for(unsigned i_elem = 0; i_elem < elem[i_e]->fldSize(); i_elem++)
		delFld(elem[i_e],i_elem);
	    elem[i_e]->valDet(this);
	    elem.erase(elem.begin()+i_e);
	    return;
	}
    //throw TError(nodePath().c_str(),"Element <%s> no present!",ValEl->elName().c_str());
}

TElem &TValue::vlElem( const string &name )
{
    for(unsigned i_e = 0; i_e < elem.size(); i_e++)
	if( elem[i_e]->elName() == name )
	    return *elem[i_e];
    throw TError(nodePath().c_str(),"Element <%s> is not present!",name.c_str());
}

void TValue::cntrCmdProc( XMLNode *opt )
{
    vector<string> list_c;
    string a_path = opt->attr("path");
    //> Service commands process
    if( a_path == "/serv/attr"  )		//Attributes access
    {
	vlList(list_c);
	if( ctrChkNode(opt,"list",RWRWRW,"root","root",SEQ_RD) )	//Full info attributes list
	{
	    AutoHD<TVal> attr;
	    for( int i_el = 0; i_el < list_c.size(); i_el++ )
	    {
		attr = vlAt(list_c[i_el]);
		opt->childAdd("el")->
		    setAttr("id",list_c[i_el])->
		    setAttr("nm",attr.at().fld().descr())->
		    setAttr("flg",TSYS::int2str(attr.at().fld().flg()))->
		    setAttr("tp",TSYS::int2str(attr.at().fld().type()))->
		    setAttr("vals",attr.at().fld().values())->
		    setAttr("names",attr.at().fld().selNames());
	    }
	}
	if( ctrChkNode(opt,"get",RWRWRW,"root","root",SEQ_RD) )		//All attributes values
	{
	    long long vtm;
	    string svl;
	    AutoHD<TVal> vl;
	    XMLNode *aNd;

	    //> Put last attribute's values
	    bool sepReq = atoi(opt->attr("sepReq").c_str());
	    bool hostTm = atoi(opt->attr("hostTm").c_str());
	    if( !sepReq )
		for( int i_el = 0; i_el < list_c.size(); i_el++ )
		{
		    vl = vlAt(list_c[i_el]);
		    vtm = 0; svl = vl.at().getS(&vtm);
		    aNd = opt->childAdd("el")->setAttr("id",list_c[i_el])->setText(svl);
		    if( !hostTm ) aNd->setAttr("tm",TSYS::ll2str(vtm));
		}

	    //> Archives requests process
	    for( int i_a = 0; i_a < opt->childSize(); i_a++ )
	    {
		aNd = opt->childGet(i_a);
		if( !sepReq && aNd->name() != "ael" ) break;
		if( (aNd->name() != "el" && aNd->name() != "ael") || !vlPresent(aNd->attr("id")) ) { opt->childDel(aNd); i_a--; continue; }
		vl = vlAt(aNd->attr("id"));

		//>> Separated element request
		if( aNd->name() == "el" )
		{
		    vtm = 0; svl = vl.at().getS(&vtm);
		    aNd->setText(svl);
		    if( !hostTm ) aNd->setAttr("tm",TSYS::ll2str(vtm));
		    continue;
		}

		//>> To archive of element request
		if( vl.at().arch().freeStat() ) { opt->childDel(aNd); i_a--; continue; }

		AutoHD<TVArchive> arch = vl.at().arch();
		long long vper = arch.at().period(BUF_ARCH_NM);
		long long reqBeg = (atoll(aNd->attr("tm").c_str())/vper+1)*vper;
		long long vbeg = vmax(reqBeg,arch.at().begin(BUF_ARCH_NM));
		long long vend = arch.at().end(BUF_ARCH_NM);

		//>>> Longing to equivalent archivators
		if( vbeg == arch.at().begin(BUF_ARCH_NM) )
		{
		    vector<string> archLs;
		    arch.at().archivatorList(archLs);
		    for( int i_a = 0; i_a < archLs.size(); i_a++ )
			if( arch.at().period(archLs[i_a]) == vper )
			    vbeg = vmax(reqBeg,arch.at().begin(archLs[i_a]));
		}
		aNd->setAttr("tm",TSYS::ll2str(vbeg))->setAttr("per",TSYS::ll2str(vper));

		TValBuf buf(arch.at().valType(),0,0,false,true);
		arch.at().getVals( buf, vbeg, vend, "", (vend-vbeg)/vper, true );

		bool firstVal = true;
		string vl;
		for( vbeg = buf.begin(); vbeg <= buf.end(); vbeg++ )
		{
		    vl = buf.getS(&vbeg,true);
		    if( firstVal && vl == EVAL_STR ) continue;
		    if( firstVal && vl != EVAL_STR ) { aNd->setAttr("tm",TSYS::ll2str(vbeg)); firstVal = false; }
		    aNd->childAdd("v")->setText(vl);
		}

		if( !aNd->childSize() ) { opt->childDel(aNd); i_a--; }
	    }
	}
	if( ctrChkNode(opt,"set",RWRWRW,"root","root",SEQ_WR) )		//Multi attributes set
	    for( int i_el = 0; i_el < opt->childSize(); i_el++ )
		vlAt(opt->childGet(i_el)->attr("id")).at().setS(opt->childGet(i_el)->text());
	return;
    }

    //> Interface comands process
    //>> Info command process
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Parameter: ")+nodeName(),0664,"root","root");
	if(ctrMkNode("area",opt,-1,"/val",_("Atributes")))
	{
	    //>>> Add atributes list
	    vlList(list_c);
	    for( int i_el = 0; i_el < list_c.size(); i_el++ )
		vlAt(list_c[i_el]).at().fld().cntrCmdMake(opt,"/val",-1);
	}
	if(ctrMkNode("area",opt,-1,"/arch",_("Archiving")))
	{
	    //>>> Archiving
	    if(ctrMkNode("table",opt,-1,"/arch/arch",_("Archiving"),0664,"root","root",1,"key","atr"))
	    {
		vector<string> list_c2;
		ctrMkNode("list",opt,-1,"/arch/arch/atr",_("Atribute"),0444,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/arch/arch/prc",_("Archiving"),0664,"root","root",1,"tp","bool");
		SYS->archive().at().modList(list_c);
		for( int i_ta = 0; i_ta < list_c.size(); i_ta++ )
		{
		    SYS->archive().at().at(list_c[i_ta]).at().valList(list_c2);
		    for( int i_a = 0; i_a < list_c2.size(); i_a++ )
		    {
			string a_id = SYS->archive().at().at(list_c[i_ta]).at().valAt(list_c2[i_a]).at().workId();
			ctrMkNode("list",opt,-1,("/arch/arch/"+a_id).c_str(),a_id,0664,"root","root",1,"tp","bool");
		    }
		}
	    }
	}
	return;
    }
    //>> Process command to page
    if( a_path.substr(0,4) == "/val" )
    {
	if( a_path.size() > 9 && a_path.substr(0,9) == "/val/sel_" && ctrChkNode(opt) )
	{
	    AutoHD<TVal> vl = vlAt(TSYS::pathLev(a_path,1).substr(4));
	    for( int i_a=0; i_a < vl.at().fld().selNm().size(); i_a++ )
		opt->childAdd("el")->setText(vl.at().fld().selNm()[i_a]);
	    return;
	}
	AutoHD<TVal> vl = vlAt(TSYS::pathLev(a_path,1));
	if( ctrChkNode(opt,"get",(vl.at().fld().flg()&TFld::NoWrite)?0440:0660,"root","root",SEQ_RD) )
	{
	    if( vl.at().fld().flg()&TFld::Selected )	opt->setText(vl.at().getSEL());
	    else opt->setText( (vl.at().fld().type()==TFld::Real) ?
		    ((vl.at().getR()==EVAL_REAL) ? EVAL_STR : TSYS::real2str(vl.at().getR(),6)) : 
		    vl.at().getS() );
	}
	if( ctrChkNode(opt,"set",(vl.at().fld().flg()&TFld::NoWrite)?0440:0660,"root","root",SEQ_WR) )
	{
	    if( vl.at().fld().flg()&TFld::Selected )	vl.at().setSEL(opt->text());
	    else					vl.at().setS(opt->text());
	}
    }
    else if( a_path == "/arch/arch" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    vector<string> list_c2;
	    //>>> Prepare headers
	    ctrMkNode("list",opt,-1,"/arch/arch/atr","",0444);
	    ctrMkNode("list",opt,-1,"/arch/arch/prc","",0664);
	    SYS->archive().at().modList(list_c);
	    for( int i_ta = 0; i_ta < list_c.size(); i_ta++ )
	    {
		SYS->archive().at().at(list_c[i_ta]).at().valList(list_c2);
		for( int i_a = 0; i_a < list_c2.size(); i_a++ )
		    ctrMkNode("list",opt,-1,("/arch/arch/"+SYS->archive().at().at(list_c[i_ta]).at().valAt(list_c2[i_a]).at().workId()).c_str(),"",0664);
	    }
	    //>>> Fill table
	    vlList(list_c);
	    for( int i_v = 0; i_v < list_c.size(); i_v++ )
		for( int i_a = 0; i_a < opt->childSize(); i_a++ )
		{
		    XMLNode *chld = opt->childGet(i_a);
		    string c_id = chld->attr("id");
		    if(c_id=="atr")		chld->childAdd("el")->setText(list_c[i_v]);
		    else if(c_id=="prc")	chld->childAdd("el")->setText(vlAt(list_c[i_v]).at().arch().freeStat()?"0":"1");
		    else chld->childAdd("el")->
			setText(vlAt(list_c[i_v]).at().arch().freeStat()?"0":(vlAt(list_c[i_v]).at().arch().at().archivatorPresent(c_id)?"1":"0"));
		}
	}
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	{
	    bool create = false;	//Create archive
	    bool v_get = atoi(opt->text().c_str());
	    string attr = opt->attr("key_atr");
	    string col  = opt->attr("col");

	    //>>> Check for create new
	    if( v_get && vlAt(attr).at().arch().freeStat() )
	    {
		//>>>> Make archive name
		string a_nm = nodeName()+"_"+attr;
		if( a_nm.size() > 20 ) a_nm = (nodeName().substr(0,nodeName().size()-nodeName().size()*(a_nm.size()-19)/21)+"_"+attr).substr(0,20);
		string rez_nm = a_nm;
		for( int p_cnt = 0; SYS->archive().at().valPresent(rez_nm); p_cnt++ )
		{
		    rez_nm = a_nm+TSYS::int2str(p_cnt);
		    if( rez_nm.size() > 20 ) rez_nm = a_nm.substr(0,a_nm.size()-(rez_nm.size()-20))+TSYS::int2str(p_cnt);
		}
		//>>>> Create new archive
		SYS->archive().at().valAdd(rez_nm);
		SYS->archive().at().valAt(rez_nm).at().setValType(vlAt(attr).at().fld().type());
		SYS->archive().at().valAt(rez_nm).at().setSrcMode(TVArchive::PassiveAttr,
		    vlAt(attr).at().nodePath('.').substr(SYS->id().size()+1));
		SYS->archive().at().valAt(rez_nm).at().setToStart(true);
		SYS->archive().at().valAt(rez_nm).at().start();
		vlArchMake(vlAt(attr).at());
		modif();
	    }
	    //>>> Check for delete archive
	    if( col == "prc" && !v_get && !vlAt(attr).at().arch().freeStat() )
	    {
		string a_id = vlAt(attr).at().arch().at().id();
		SYS->archive().at().valDel(a_id,true);
		modif();
	    }
	    //>>> Change archivator status
	    if( col != "prc" && !vlAt(attr).at().arch().freeStat() )
	    {
		if( v_get )	vlAt(attr).at().arch().at().archivatorAttach(col);
		else		vlAt(attr).at().arch().at().archivatorDetach(col,true);
		modif();
		vlAt(attr).at().arch().at().modif();
	    }

	}
    }
    else TCntrNode::cntrCmdProc(opt);
}

//*************************************************
//* TVal                                          *
//*************************************************
TVal::TVal( ) : mCfg(false), mTime(0), mResB1(false), mResB2(false), mReqFlg(false)
{
    src.fld = NULL;
}

TVal::TVal( TFld &fld ) : mCfg(false), mTime(0)
{
    src.fld = NULL;
    modifClr();

    setFld(fld);
}

TVal::TVal( TCfg &cfg ) : mCfg(false), mTime(0)
{
    src.fld = NULL;

    setCfg(cfg);
}

TVal::~TVal( )
{
    if( !mCfg && src.fld->type() == TFld::String )	delete val.val_s;
    if( !mCfg && src.fld->flg()&TFld::SelfFld )		delete src.fld;
}

TValue &TVal::owner( )	{ return *(TValue*)nodePrev(); }

void TVal::setFld( TFld &fld )
{
    //> Delete previous
    if( !mCfg && src.fld && src.fld->type() == TFld::String )	delete val.val_s;
    if( !mCfg && src.fld && src.fld->flg()&TFld::SelfFld )	delete src.fld;

    //> Chek for self field for dynamic elements
    if( fld.flg()&TFld::SelfFld )
    {
	src.fld = new TFld();
	*(src.fld) = fld;
    }
    else src.fld = &fld;

    switch(src.fld->type())
    {
	case TFld::String:
	    val.val_s = new ResString();
	    if( src.fld->def().empty() ) val.val_s->setVal(EVAL_STR);
	    else val.val_s->setVal(src.fld->def());
	    break;
	case TFld::Integer:
	    val.val_i = src.fld->def().empty() ? EVAL_INT : atoi(src.fld->def().c_str());	break;
	case TFld::Real:
	    val.val_r = src.fld->def().empty() ? EVAL_REAL : atof(src.fld->def().c_str());	break;
	case TFld::Boolean:
	    val.val_b = src.fld->def().empty() ? EVAL_BOOL : atoi(src.fld->def().c_str());	break;
    }

    mCfg = false;
}

void TVal::setCfg( TCfg &cfg )
{
    //> Delete previous
    if( !mCfg && src.fld && src.fld->type() == TFld::String )	delete val.val_s;
    if( !mCfg && src.fld && src.fld->flg()&TFld::SelfFld )	delete src.fld;

    //> Set cfg
    src.cfg = &cfg;
    mCfg = true;
}

const string &TVal::name()
{
    if( mCfg )	return( src.cfg->name() );
    else	return( src.fld->name() );
}

TFld &TVal::fld()
{
    if( mCfg )	return( src.cfg->fld() );
    return( *src.fld );
}

AutoHD<TVArchive> TVal::arch()	{ return mArch; }

void TVal::setArch( const AutoHD<TVArchive> &vl )	{ mArch = vl; }

string TVal::getSEL( long long *tm, bool sys )
{
    if( !(fld().flg()&TFld::Selected) )	throw TError("Val","Not select type!");
    switch( fld().type() )
    {
	case TFld::String:	return fld().selVl2Nm(getS(tm,sys));
	case TFld::Integer:	return fld().selVl2Nm(getI(tm,sys));
	case TFld::Real:	return fld().selVl2Nm(getR(tm,sys));
	case TFld::Boolean:	return fld().selVl2Nm(getB(tm,sys));
    }
}

string TVal::getS( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::Integer:
	{ int vl = getI(tm,sys);	return (vl!=EVAL_INT)  ? TSYS::int2str(vl) : EVAL_STR; }
	case TFld::Real:
	{ double vl = getR(tm,sys);	return (vl!=EVAL_REAL) ? TSYS::real2str(vl) : EVAL_STR; }
	case TFld::Boolean:
	{ char vl = getB(tm,sys);	return (vl!=EVAL_BOOL) ? TSYS::int2str((bool)vl) : EVAL_STR; }
	case TFld::String:
	    setReqFlg(true);
	    //> Get from archive
	    if( tm && (*tm) && !mArch.freeStat() && *tm/mArch.at().period() < time()/mArch.at().period() )
		return mArch.at().getS(tm);
	    //> Get value from config
	    if( mCfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getS( );
	    }
	    //> Get current value
	    if( fld().flg()&TVal::DirRead && !sys )	owner().vlGet( *this );
	    if( tm ) *tm = time();
	    return val.val_s->getVal();
    }
}

int TVal::getI( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	{ string vl = getS(tm,sys);	return (vl!=EVAL_STR) ? atoi(vl.c_str()) : EVAL_INT; }
	case TFld::Real:
	{ double vl = getR(tm,sys);	return (vl!=EVAL_REAL) ? (int)vl : EVAL_INT; }
	case TFld::Boolean:
	{ char vl = getB(tm,sys);	return (vl!=EVAL_BOOL) ? (bool)vl : EVAL_INT; }
	case TFld::Integer:
	    setReqFlg(true);
	    //> Get from archive
	    if( tm && (*tm) && !mArch.freeStat() && *tm/mArch.at().period() < time()/mArch.at().period() ) 
		return mArch.at().getI(tm);
	    //> Get value from config
	    if( mCfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getI( );
	    }
	    //> Get current value
	    if( fld().flg()&TVal::DirRead && !sys )	owner().vlGet( *this );
	    if( tm ) *tm = time();
	    return val.val_i;
    }
}

double TVal::getR( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	{ string vl = getS(tm,sys);	return (vl!=EVAL_STR) ? atof(vl.c_str()) : EVAL_REAL; }
	case TFld::Integer:
	{ int vl = getI(tm,sys);	return (vl!=EVAL_INT) ? vl : EVAL_REAL; }
	case TFld::Boolean:
	{ char vl = getB(tm,sys);	return (vl!=EVAL_BOOL) ? (bool)vl : EVAL_REAL; }
	case TFld::Real:
	    setReqFlg(true);
	    //> Get from archive
	    if( tm && (*tm) && !mArch.freeStat() && *tm/mArch.at().period() < time()/mArch.at().period() )
		return mArch.at().getR(tm);
	    //> Get value from config
	    if( mCfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getR( );
	    }
	    //> Get current value
	    if( fld().flg()&TVal::DirRead && !sys )	owner().vlGet( *this );
	    if( tm ) *tm = time();
	    return val.val_r;
    }
}

char TVal::getB( long long *tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	{ string vl = getS(tm,sys);	return (vl!=EVAL_STR) ? (bool)atoi(vl.c_str()) : EVAL_BOOL; }
	case TFld::Integer:
	{ int vl = getI(tm,sys);	return (vl!=EVAL_INT) ? (bool)vl : EVAL_BOOL; }
	case TFld::Real:
	{ double vl = getR(tm,sys);	return (vl!=EVAL_REAL) ? (bool)vl : EVAL_BOOL; }
	case TFld::Boolean:
	    setReqFlg(true);
	    //> Get from archive
	    if( tm && (*tm) && !mArch.freeStat() && *tm/mArch.at().period() < time()/mArch.at().period() )
		return mArch.at().getB(tm);
	    //> Get value from config
	    if( mCfg )
	    {
		if(tm) *tm = TSYS::curTime();
		return src.cfg->getB( );
	    }
	    //> Get current value
	    if( fld().flg()&TVal::DirRead && !sys )	owner().vlGet( *this );
	    if( tm ) *tm = time();
	    return val.val_b;
    }
}

void TVal::setSEL( const string &value, long long tm, bool sys )
{
    if( !(fld().flg()&TFld::Selected) )	throw TError("Val","Not select type!");
    switch( fld().type() )
    {
	case TFld::String:	setS(fld().selNm2VlS(value),tm,sys);	break;
	case TFld::Integer:	setI(fld().selNm2VlI(value),tm,sys);	break;
	case TFld::Real:	setR(fld().selNm2VlR(value),tm,sys);	break;
	case TFld::Boolean:	setB(fld().selNm2VlB(value),tm,sys);	break;
    }
}

void TVal::setS( const string &value, long long tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::Integer:
	    setI( (value!=EVAL_STR) ? atoi(value.c_str()) : EVAL_INT, tm, sys );	break;
	case TFld::Real:
	    setR( (value!=EVAL_STR) ? atof(value.c_str()) : EVAL_REAL, tm, sys );	break;
	case TFld::Boolean:
	    setB( (value!=EVAL_STR) ? (bool)atoi(value.c_str()) : EVAL_BOOL, tm, sys );	break;
	case TFld::String:
	    //> Set value to config
	    if( mCfg )	{ src.cfg->setS( value ); return; }
	    //> Check to write
	    if( !sys && fld().flg()&TFld::NoWrite )	throw TError("Val","Write access is denied!");
	    //> Set current value and time
	    string pvl = val.val_s->getVal(); val.val_s->setVal(value);
	    mTime = tm;
	    if( !mTime ) mTime = TSYS::curTime();
	    if( fld().flg()&TVal::DirWrite && !sys )	owner().vlSet( *this, pvl );
	    //> Set to archive
	    if( !mArch.freeStat() && mArch.at().srcMode() == TVArchive::PassiveAttr )
		try{ mArch.at().setS(value,time()); }
		catch(TError err){ mess_err(nodePath().c_str(),_("Write value to archive error: %s"),err.mess.c_str()); }
    }
}

void TVal::setI( int value, long long tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	    setS( (value!=EVAL_INT) ? TSYS::int2str(value) : EVAL_STR, tm, sys );	break;
	case TFld::Real:
	    setR( (value!=EVAL_INT) ? value : EVAL_REAL, tm, sys );		break;
	case TFld::Boolean:
	    setB( (value!=EVAL_INT) ? (bool)value : EVAL_BOOL, tm, sys );	break;
	case TFld::Integer:
	{
	    //> Set value to config
	    if( mCfg )	{ src.cfg->setI( value ); return; }
	    //> Check to write
            if( !sys && fld().flg()&TFld::NoWrite )	throw TError("Val","Write access is denied!");
	    //> Set current value and time
	    if( !(fld().flg()&TFld::Selected) && fld().selValI()[1] > fld().selValI()[0] && value != EVAL_INT )
		value = vmin(fld().selValI()[1],vmax(fld().selValI()[0],value));
	    int pvl = val.val_i; val.val_i = value;
	    mTime = tm;
	    if( !mTime ) mTime = TSYS::curTime();
	    if( fld().flg()&TVal::DirWrite && !sys )	owner().vlSet( *this, pvl );
	    //> Set to archive
	    if( !mArch.freeStat() && mArch.at().srcMode() == TVArchive::PassiveAttr )
		try{ mArch.at().setI(value,time()); }
		catch(TError err){ mess_err(nodePath().c_str(),_("Write value to archive error: %s"),err.mess.c_str()); }
	}
    }
}

void TVal::setR( double value, long long tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	    setS( (value!=EVAL_REAL) ? TSYS::real2str(value) : EVAL_STR, tm, sys );	break;
	case TFld::Integer:
	    setI( (value!=EVAL_REAL) ? (int)value : EVAL_INT, tm, sys );		break;
	case TFld::Boolean:
	    setB( (value!=EVAL_REAL) ? (bool)value : EVAL_BOOL, tm, sys );		break;
	case TFld::Real:
	{
	    //> Set value to config
	    if( mCfg )	{ src.cfg->setR( value ); return; }
	    //> Check to write
	    if( !sys && fld().flg()&TFld::NoWrite )	throw TError("Val","Write access is denied!");
	    //> Set current value and time
	    if( !(fld().flg()&TFld::Selected) && fld().selValR()[1] > fld().selValR()[0] && value != EVAL_REAL )
		value = vmin(fld().selValR()[1],vmax(fld().selValR()[0],value));
	    double pvl = val.val_r; val.val_r = value;
	    mTime = tm;
	    if( !mTime ) mTime = TSYS::curTime();
	    if( fld().flg()&TVal::DirWrite && !sys )	owner().vlSet( *this, pvl );
	    //> Set to archive
	    if( !mArch.freeStat() && mArch.at().srcMode() == TVArchive::PassiveAttr )
		try{ mArch.at().setR(value,time()); }
		catch(TError err){ mess_err(nodePath().c_str(),_("Write value to archive error: %s"),err.mess.c_str()); }
	}
    }
}

void TVal::setB( char value, long long tm, bool sys )
{
    switch( fld().type() )
    {
	case TFld::String:
	    setS( (value!=EVAL_BOOL) ? TSYS::int2str((bool)value) : EVAL_STR, tm, sys );	break;
	case TFld::Integer:
	    setI( (value!=EVAL_BOOL) ? (bool)value : EVAL_INT, tm, sys );	break;
	case TFld::Real:
	    setR( (value!=EVAL_BOOL) ? (bool)value : EVAL_REAL, tm, sys);	break;
	case TFld::Boolean:
	{
	    //> Set value to config
	    if( mCfg )	{ src.cfg->setB( value ); return; }
	    //> Check to write
	    if( !sys && fld().flg()&TFld::NoWrite )	throw TError("Val","Write access is denied!");
	    //> Set current value and time
	    char pvl = val.val_b; val.val_b = value;
	    mTime = tm;
	    if( !mTime ) mTime = TSYS::curTime();
	    if( fld().flg()&TVal::DirWrite && !sys )	owner().vlSet( *this, pvl );
	    //> Set to archive
	    if( !mArch.freeStat() && mArch.at().srcMode() == TVArchive::PassiveAttr )
		try{ mArch.at().setB(value,time()); }
		catch(TError err){ mess_err(nodePath().c_str(),_("Write value to archive error: %s"),err.mess.c_str()); }
	}
    }
}

void TVal::cntrCmdProc( XMLNode *opt )
{
    string a_path = opt->attr("path");
    //> Service commands process
    if( a_path == "/serv/val"  )		//Values access
    {
	if( ctrChkNode(opt,"info",RWRWRW,"root","root",SEQ_RD) )	//Value's data information
	{
	    if( !arch().freeStat() ) arch().at().cntrCmdProc(opt);
	    else
	    {
		opt->setAttr("end","0")->setAttr("beg","0")->setAttr("per","0");
		opt->setAttr("vtp",TSYS::int2str(fld().type()));
	    }
	}
	else if( ctrChkNode(opt,"get",RWRWRW,"root","root",SEQ_RD) )    //Value's data request
	{
	    if( !atoll(opt->attr("tm_grnd").c_str()) && opt->attr("arch").empty() )
	    {
		long long tm = atoll(opt->attr("tm").c_str());
		opt->setText(getS(&tm));
		opt->setAttr("tm",TSYS::ll2str(tm));
	    }
	    else if( !arch().freeStat() ) arch().at().cntrCmdProc(opt);
	    else throw TError(nodePath().c_str(),"Attribute doesn't have archive");
	}
	return;
    }

    //> Interface comands process
    //>> Info command process
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
	return;
    }
    //>> Process command to page
    TCntrNode::cntrCmdProc(opt);
}
