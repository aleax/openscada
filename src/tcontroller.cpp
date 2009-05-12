
//OpenSCADA system file: tcontroller.cpp
/***************************************************************************
 *   Copyright (C) 2003-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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
#include "ttiparam.h"
#include "tcontroller.h"

//*************************************************
//* TController					  *
//*************************************************
TController::TController( const string &id_c, const string &daq_db, TElem *cfgelem ) :
    mDB(daq_db), TConfig(cfgelem), run_st(false), en_st(false), mRedntUse(true), mRedntFirst(true),
    mId(cfg("ID").getSd()), mName(cfg("NAME").getSd()), mDescr(cfg("DESCR").getSd()),
    mAEn(cfg("ENABLE").getBd()), mAStart(cfg("START").getBd())
{
    mId = id_c;
    mPrm = grpAdd("prm_");
}

TController::~TController(  )
{
    nodeDelAll();
}

TCntrNode &TController::operator=( TCntrNode &node )
{
    TController *src_n = dynamic_cast<TController*>(&node);
    if( !src_n ) return *this;

    //> Configuration copy
    string tid = id();
    *(TConfig*)this = *(TConfig*)src_n;
    mId = tid;
    setDB(src_n->mDB);

    //> Parameters copy
    if( src_n->enableStat( ) )
    {
	if( !enableStat( ) )	enable();
	vector<string> prm_ls;
	src_n->list(prm_ls);
	for( int i_p = 0; i_p < prm_ls.size(); i_p++ )
	{
	    if( !owner().tpPrmPresent(src_n->at(prm_ls[i_p]).at().type().name) ) continue;
	    if( !present(prm_ls[i_p]) )	add( prm_ls[i_p], owner().tpPrmToId(src_n->at(prm_ls[i_p]).at().type().name) );
	    (TCntrNode&)at(prm_ls[i_p]).at() = (TCntrNode&)src_n->at(prm_ls[i_p]).at();
	    if( toEnable( ) && !enableStat( ) )	enable();
	}
    }

    return *this;
}

void TController::preDisable(int flag)
{
    if( startStat() )	stop( );
    if( enableStat() )	disable( );
}

void TController::postDisable(int flag)
{
    try
    {
	if( flag )
	{
	    //> Delete DB record
	    SYS->db().at().dataDel(fullDB(),owner().nodePath()+"DAQ",*this,true);

	    //> Delete parameter's tables
	    for(unsigned i_tp = 0; i_tp < owner().tpPrmSize(); i_tp++)
	    {
		string tbl = DB()+"."+cfg(owner().tpPrmAt(i_tp).db).getS();
		SYS->db().at().open(tbl);
		SYS->db().at().close(tbl,true);
	    }
	}
    }catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

TTipDAQ &TController::owner( )	{ return *(TTipDAQ*)nodePrev(); }

string TController::workId( )	{ return owner().modId()+"."+id(); }

string TController::name()	{ return mName.size() ? mName : id(); }

string TController::tbl( )	{ return owner().owner().subId()+"_"+owner().modId(); }

string TController::getStatus( )
{
    string rez;
    if( startStat() )
    {
	rez = _("Started. ");
	if( redntUse( ) ) rez += _("Geting data from remote station. ");
    }
    else if( enableStat() ) rez = _("Enabled. ");
    else rez = _("Disabled. ");

    return rez;
}

void TController::load_( )
{
    if( !SYS->chkSelDB(DB()) ) return;

    mess_info(nodePath().c_str(),_("Load controller's configs!"));

    SYS->db().at().dataGet(fullDB(),owner().nodePath()+"DAQ",*this);

    //- Load parameters if enabled -
    if( en_st )	LoadParmCfg( );
}

void TController::save_( )
{
    mess_info(nodePath().c_str(),_("Save controller's configs!"));

    //- Update type controller bd record -
    SYS->db().at().dataSet(fullDB(),owner().nodePath()+"DAQ",*this);
}

bool TController::cfgChange( TCfg &cfg )
{
    modif( );

    return true;
}

void TController::start( )
{
    //> Enable if no enabled
    if( run_st ) return;
    if( !en_st ) enable();

    mess_info(nodePath().c_str(),_("Start controller!"));

    //> First archives synchronization
    if( owner().redntAllow() && redntMode( ) != TController::Off ) redntDataUpdate(true);

    //> Start for children
    start_();

    run_st = true;
}

void TController::stop( )
{
    if( !run_st ) return;

    mess_info(nodePath().c_str(),_("Stop controller!"));

    //- Stop for children -
    stop_();

    run_st = false;
}

void TController::enable( )
{
    if( !en_st )
    {
	mess_info(nodePath().c_str(),_("Enable controller!"));

	//- Enable for children -
	enable_();

	//- Load parameters -
	LoadParmCfg( );
    }

    //- Enable parameters -
    vector<string> prm_list;
    list(prm_list);
    for( int i_prm = 0; i_prm < prm_list.size(); i_prm++ )
	if( at(prm_list[i_prm]).at().toEnable() )
	    try{ at(prm_list[i_prm]).at().enable(); }
	    catch(TError err)
	    {
		mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
		mess_warning(nodePath().c_str(),_("Enable parameter <%s> error."),prm_list[i_prm].c_str());
	    }

    //- Set enable stat flag -
    en_st=true;
}

void TController::disable( )
{
    if( !en_st ) return;
    //- Stop if runed -
    if( run_st ) stop();

    mess_info(nodePath().c_str(),_("Disable controller!"));

    //- Disable parameters -
    vector<string> prm_list;
    list(prm_list);
    for( int i_prm = 0; i_prm < prm_list.size(); i_prm++ )
	if( at(prm_list[i_prm]).at().enableStat() )
	try{ at(prm_list[i_prm]).at().disable(); }
	catch(TError err)
	{
	    mess_warning(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_warning(nodePath().c_str(),_("Disable parameter <%s> error."),prm_list[i_prm].c_str());
	}

    //- Disable for children -
    disable_();

    //- Clear enable flag -
    en_st = false;
}

void TController::LoadParmCfg(  )
{
    //- Search and create new parameters -
    for( int i_tp = 0; i_tp < owner().tpPrmSize(); i_tp++ )
    {
	if( owner().tpPrmAt(i_tp).db.empty() ) continue;
	try
	{
	    TConfig c_el(&owner().tpPrmAt(i_tp));
	    c_el.cfgViewAll(false);

	    for( int fld_cnt = 0; SYS->db().at().dataSeek(DB()+"."+cfg(owner().tpPrmAt(i_tp).db).getS(),
					   owner().nodePath()+cfg(owner().tpPrmAt(i_tp).db).getS(),fld_cnt++,c_el); )
	    {
		try
		{
		    if( !present(c_el.cfg("SHIFR").getS()) ) add( c_el.cfg("SHIFR").getS(), i_tp );
		}
		catch(TError err)
		{
		    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
		    mess_err(nodePath().c_str(),_("Add parameter <%s> error."),c_el.cfg("SHIFR").getS().c_str());
		}
	    }
	}catch(TError err)
	{
	    mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	    mess_err(nodePath().c_str(),_("Search and create new parameters error."));
	}
    }

    //- Force load present parameters -
    vector<string> prm_ls;
    list(prm_ls);
    for( int i_p = 0; i_p < prm_ls.size(); i_p++ )
    {
	at(prm_ls[i_p]).at().modifG();
	at(prm_ls[i_p]).at().load( );
    }
}

void TController::add( const string &name, unsigned type )
{
    if( chldPresent(mPrm,name) ) return;
    chldAdd(mPrm,ParamAttach( name, type ));
}

TParamContr *TController::ParamAttach( const string &name, int type)
{
    return new TParamContr(name, &owner().tpPrmAt(type));
}

TController::Redundant TController::redntMode( )	{ return (TController::Redundant)cfg("REDNT").getI(); }

void TController::setRedntMode( Redundant vl )		{ cfg("REDNT").setI(vl); modif(); }

string TController::redntRun( )				{ return cfg("REDNT_RUN").getS(); }

void TController::setRedntRun( const string &vl )	{ cfg("REDNT_RUN").setS(vl); modif(); }

void TController::setRedntUse( bool vl )
{
    if( mRedntUse == vl ) return;
    mRedntUse = mRedntFirst = vl;
}

void TController::redntDataUpdate( bool firstArchiveSync )
{
    vector<string> pls;
    list(pls);

    //> Prepare group request to parameters
    AutoHD<TParamContr> prm;
    XMLNode req("CntrReqs"); req.setAttr("path",nodePath(0,true));
    if( firstArchiveSync || mRedntFirst ) req.setAttr("all","1");
    for( int i_p = 0; i_p < pls.size(); i_p++ )
    {
	prm = at(pls[i_p]);
	if( !prm.at().enableStat( ) ) { pls.erase(pls.begin()+i_p); i_p--; continue; }
	//>> Check attributes last present data time into archives
	if( firstArchiveSync || mRedntFirst )
	{
	    prm.at().mRedntTmLast = 0;
	    vector<string> listV;
	    prm.at().vlList(listV);
	    for( int iV = 0; iV < listV.size(); iV++ )
	    {
		AutoHD<TVal> vl = prm.at().vlAt(listV[iV]);
		prm.at().mRedntTmLast = vmax(prm.at().mRedntTmLast,vl.at().time());
		if( !vl.at().arch().freeStat() )
		    prm.at().mRedntTmLast = vmax(prm.at().mRedntTmLast,vl.at().arch().at().end(""));
	    }
	    if( prm.at().mRedntTmLast )
		prm.at().mRedntTmLast = vmax(prm.at().mRedntTmLast,TSYS::curTime()-(long long)(3.6e9*owner().owner().rdRestDtTm()));
	    else prm.at().mRedntTmLast = TSYS::curTime();
	}
	req.childAdd("get")->setAttr("path","/prm_"+pls[i_p]+"/%2fserv%2fattr")->
	    setAttr("tm",TSYS::ll2str(prm.at().mRedntTmLast));
    }

    //> Send request to first active station for this controller
    try{ owner().owner().rdStRequest(workId(),req); }
    catch(TError err) { return; }

    //> Write data to parameters
    for( int i_p = 0; i_p < pls.size(); i_p++ )
    {
	prm = at(pls[i_p]);
	prm.at().mRedntTmLast = atoll(req.childGet(i_p)->attr("tm").c_str());
	for( int i_a = 0; i_a < req.childGet(i_p)->childSize(); i_a++ )
	{
	    XMLNode *aNd = req.childGet(i_p)->childGet(i_a);
	    if( !prm.at().vlPresent(aNd->attr("id")) ) continue;
	    AutoHD<TVal> vl = prm.at().vlAt(aNd->attr("id"));

	    if( aNd->attr("tm").empty() ) vl.at().setS(aNd->text(),prm.at().mRedntTmLast,true);
	    else if( aNd->childSize() )
	    {
		long long btm = atoll(aNd->attr("tm").c_str());
		long long per = atoll(aNd->attr("per").c_str());
		if( !vl.at().arch().freeStat() )
		{
		    TValBuf buf(vl.at().arch().at().valType(),0,per,true,true);
		    for( int i_v = 0; i_v < aNd->childSize(); i_v++ )
			buf.setS(aNd->childGet(i_v)->text(),btm+per*i_v);
		    vl.at().arch().at().setVal(buf,buf.begin(),buf.end(),"");
		}
		vl.at().setS(aNd->childGet(aNd->childSize()-1)->text(),btm+per*(aNd->childSize()-1),true);
	    }
	}
    }

    mRedntFirst = false;
}

void TController::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if( opt->name() == "info" )
    {
	TCntrNode::cntrCmdProc(opt);
	ctrMkNode("oscada_cntr",opt,-1,"/",_("Controller: ")+name(),0664,"root","root");
	ctrMkNode("branches",opt,-1,"/br","",0444);
	if(ctrMkNode("area",opt,-1,"/cntr",_("Controller")))
	{
	    if(ctrMkNode("area",opt,-1,"/cntr/st",_("State")))
	    {
		ctrMkNode("fld",opt,-1,"/cntr/st/status",_("Status"),0444,"root","root",1,"tp","str");
		ctrMkNode("fld",opt,-1,"/cntr/st/en_st",_("Enable"),0664,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/cntr/st/run_st",_("Run"),0664,"root","root",1,"tp","bool");
		ctrMkNode("fld",opt,-1,"/cntr/st/db",_("Controller DB"),0664,"root","root",4,"tp","str","dest","select","select","/db/list",
		    "help",_("DB address in format [<DB module>.<DB name>].\nFor use main work DB set '*.*'."));
	    }
	    if( ctrMkNode("area",opt,-1,"/cntr/cfg",_("Config")) )
	    {
		TConfig::cntrCmdMake(opt,"/cntr/cfg",0,"root","root",0664);
		//>> Append configuration properties
		XMLNode *xt = ctrId(opt->childGet(0),"/cntr/cfg/REDNT_RUN",true);
		if( xt ) xt->setAttr("dest","select")->setAttr("select","/cntr/redRunLst");
	    }
	}
	if( owner().tpPrmSize() )
	{
	    ctrMkNode("grp",opt,-1,"/br/prm_",_("Parameter"),0660,"root","root",2,"idm","1","idSz","20");
	    if(ctrMkNode("area",opt,-1,"/prm",_("Parameters")))
	    {
		if( owner().tpPrmSize() > 1 )
		    ctrMkNode("fld",opt,-1,"/prm/t_prm",_("To add parameters"),0660,"root","root",3,"tp","str","dest","select","select","/prm/t_lst");
		ctrMkNode("fld",opt,-1,"/prm/nmb",_("Number"),0444,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/prm/prm",_("Parameters"),0660,"root","root",5,"tp","br","idm","1","s_com","add,del","br_pref","prm_","idSz","20");
	    }
	}
	return;
    }

    //> Process command to page
    vector<string> c_list;
    string a_path = opt->attr("path");
    if( a_path == "/cntr/st/status" && ctrChkNode(opt) )	opt->setText(getStatus());
    else if( a_path == "/prm/nmb" && ctrChkNode(opt) )
    {
	list(c_list);
	int e_c = 0;
	for( int i_a = 0; i_a < c_list.size(); i_a++ )
	    if( at(c_list[i_a]).at().enableStat( ) )	e_c++;
	opt->setText(TSYS::strMess(_("All: %d; Enabled: %d"),c_list.size(),e_c));
    }
    else if( a_path == "/prm/t_prm" && owner().tpPrmSize() )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )
	    opt->setText(TBDS::genDBGet(owner().nodePath()+"addType",owner().tpPrmAt(0).name,opt->attr("user")));
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )
	    TBDS::genDBSet(owner().nodePath()+"addType",opt->text(),opt->attr("user"));
    }
    else if( (a_path == "/br/prm_" || a_path == "/prm/prm") && owner().tpPrmSize() )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )
	{
	    list(c_list);
	    for( unsigned i_a=0; i_a < c_list.size(); i_a++ )
		opt->childAdd("el")->setAttr("id",c_list[i_a])->setText(at(c_list[i_a]).at().name());
	}
	if( ctrChkNode(opt,"add",0660,"root","root",SEQ_WR) )
	{
	    string vid = TSYS::strEncode(opt->attr("id"),TSYS::ID);
	    add(vid,owner().tpPrmToId(TBDS::genDBGet(owner().nodePath()+"addType",owner().tpPrmAt(0).name,opt->attr("user"))));
	    at(vid).at().setName(opt->text());
	}
	if( ctrChkNode(opt,"del",0660,"root","root",SEQ_WR) )	del(opt->attr("id"),true);
    }
    else if( a_path == "/prm/t_lst" && owner().tpPrmSize() && ctrChkNode(opt,"get",0444) )
    {
	for( unsigned i_a=0; i_a < owner().tpPrmSize(); i_a++ )
	    opt->childAdd("el")->setAttr("id",owner().tpPrmAt(i_a).name)->setText(owner().tpPrmAt(i_a).descr);
    }
    else if( a_path == "/cntr/st/db" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(DB());
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setDB(opt->text());
    }
    else if( a_path == "/cntr/st/en_st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(en_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	atoi(opt->text().c_str())?enable():disable();
    }
    else if( a_path == "/cntr/st/run_st" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText(run_st?"1":"0");
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	atoi(opt->text().c_str())?start():stop();
    }
    else if( a_path.substr(0,9) == "/cntr/cfg" )
    {
	TConfig::cntrCmdProc(opt,TSYS::pathLev(a_path,2),"root","root",0664);
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )
	    for( int i_t = 0; i_t < owner().tpPrmSize( ); i_t++ )
		if( owner().tpPrmAt(i_t).db == TSYS::pathLev(a_path,2) )
		     modifG( );
    }
    else if( a_path == "/cntr/redRunLst" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id","<high>")->setText(_("<High level>"));
	opt->childAdd("el")->setAttr("id","<low>")->setText(_("<Low level>"));
	opt->childAdd("el")->setAttr("id","<optimal>")->setText(_("<Optimal>"));
	vector<string> sls;
	owner().owner().rdStList(sls);
	for( int i_s = 0; i_s < sls.size(); i_s++ )
	    opt->childAdd("el")->setAttr("id",sls[i_s])->setText(SYS->transport().at().extHostGet("*",sls[i_s]).name);
    }
    else TCntrNode::cntrCmdProc(opt);
}
