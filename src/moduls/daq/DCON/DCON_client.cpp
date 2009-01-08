
//OpenSCADA system module DAQ.DCON file: DCON_client.cpp
/***************************************************************************
 *   Copyright (C) 2008 by Almaz Karimov                                   *
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

#include <getopt.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <tsys.h>
#include <ttiparam.h>

#include "DCON_client.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"DCON"
#define MOD_NAME	"DCON client"
#define MOD_TYPE	"DAQ"
#define VER_TYPE	VER_CNTR
#define VERSION		"0.1.0"
#define AUTORS		"Almaz Karimov"
#define DESCRIPTION	"Allow realisation of DCON client service. Supported I-7000 DCON protocol."
#define LICENSE		"GPL"
//*************************************************

DCONDAQ::TTpContr *DCONDAQ::mod;  //Pointer for direct access to module

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new DCONDAQ::TTpContr( source );
	return NULL;
    }
}

using namespace DCONDAQ;

//******************************************************
//* TTpContr                                           *
//******************************************************
TTpContr::TTpContr( string name )
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;

    m_sdev	= grpAdd("sdev_");
}

TTpContr::~TTpContr()
{

}

string TTpContr::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

string TTpContr::serDevDB( )
{
    return SYS->workDB()+"."+modId()+"_sdevs";
}

void TTpContr::serDevAdd( const string &dev )
{
    if( chldPresent(m_sdev,dev) ) return;
    chldAdd( m_sdev, new SSerial(dev,this) );
}

void TTpContr::postEnable( int flag )
{
    TModule::postEnable( flag );

    //- Controler's bd structure -
    fldAdd( new TFld("PRM_BD",_("Parameteres table"),TFld::String,TFld::NoFlag,"30","") );
    fldAdd( new TFld("PERIOD",_("Gather data period (s)"),TFld::Real,TFld::NoFlag,"6.2","1","0.01;100") );
    fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","0;100") );
    fldAdd( new TFld("ADDR",_("Serial port"),TFld::String,TFld::NoFlag,"30","/dev/ttyUSB0") );

    //- Parameter type bd structure -
    int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
    tpPrmAt(t_prm).fldAdd( new TFld("MOD_TP",_("I-7000 module type"),TFld::Integer,TFld::Selected|TCfg::NoVal,"1","0","0;1;2;3;4",_("I-7051;I-7045;I-7063;I-7017;I-7024")) );
    tpPrmAt(t_prm).fldAdd( new TFld("MOD_ADDR",_("I-7000 module address"),TFld::Integer,TFld::NoFlag|TCfg::NoVal,"20","1","0;255") );
    tpPrmAt(t_prm).fldAdd( new TFld("CRC_CTRL",_("CRC control"),TFld::Boolean,TFld::NoFlag|TCfg::NoVal,"1","1") );

    //- Serial devices DB structure -
    el_ser_dev.fldAdd( new TFld("ID",_("ID"),TFld::String,TCfg::Key,"30") );
    el_ser_dev.fldAdd( new TFld("SPEED",_("Speed"),TFld::Integer,TFld::NoFlag,"6","9600") );
    el_ser_dev.fldAdd( new TFld("LEN",_("Length"),TFld::Integer,TFld::NoFlag,"1","8") );
    el_ser_dev.fldAdd( new TFld("TWOSTOP",_("Two stop bit"),TFld::Boolean,TFld::NoFlag,"1","1") );
    el_ser_dev.fldAdd( new TFld("PARITY",_("Parity"),TFld::Integer,TFld::NoFlag,"1","0") );
    el_ser_dev.fldAdd( new TFld("TM_FRM",_("Time frame"),TFld::Integer,TFld::NoFlag,"5","1000") );
    el_ser_dev.fldAdd( new TFld("TM_CHAR",_("Time char"),TFld::Real,TFld::NoFlag,"5.2","4") );
    el_ser_dev.fldAdd( new TFld("TM_REQ",_("Time request"),TFld::Integer,TFld::NoFlag,"5","2000") );
}

void TTpContr::load_( )
{
    //- Load parameters from command line -
    int next_opt;
    const char *short_opt = "h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind = opterr = 0;
    do
    {
	next_opt = getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch( next_opt )
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while( next_opt != -1 );

    //- Load Serial devices configuration -
    try
    {
	TConfig c_el(&serDevE());
	for( int fld_cnt = 0; SYS->db().at().dataSeek(serDevDB(),nodePath()+"sDevs/",fld_cnt,c_el); fld_cnt++ )
	{
	    string id = c_el.cfg("ID").getS();
	    c_el.cfg("ID").setS("");
	    try
	    {
		if( !serDevPresent(id) )	serDevAdd(id);
		serDevAt(id).at().load();
		if( !serDevAt(id).at().hasOpen() )	serDevAt(id).at().setOpen(true);
	    }catch( TError err ) { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
	}
    }catch( TError err )
    {
	mess_err(err.cat.c_str(),"%s",err.mess.c_str());
	mess_err(nodePath().c_str(),_("Search and load serial devices' DB error."));
    }
}

void TTpContr::save_()
{
    //- Save Serial devices configuration -
    TConfig cfg(&serDevE());
    vector<string> lst;
    serDevList(lst);
    for( int i_l = 0; i_l < lst.size(); i_l++ )
	serDevAt(lst[i_l]).at().save();
}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
    return new TMdContr(name,daq_db,this);
}

void TTpContr::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TTipDAQ::cntrCmdProc( opt );
	if( ctrMkNode("area",opt,0,"/mod",_("DCON")) )
	{
	    if( ctrMkNode("table",opt,-1,"/mod/dev",_("Serial devices"),0664,"root","root",2,"s_com","add,del","key","dev") )
	    {
		ctrMkNode("list",opt,-1,"/mod/dev/dev",_("Device"),0664,"root","root",1,"tp","str");
		ctrMkNode("list",opt,-1,"/mod/dev/speed",_("Speed"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/mod/dev/lsspd");
		ctrMkNode("list",opt,-1,"/mod/dev/len",_("Char length (bit)"),0664,"root","root",3,"tp","dec","min","5","max","8");
		ctrMkNode("list",opt,-1,"/mod/dev/stop",_("Stop bits"),0664,"root","root",3,"tp","dec","min","1","max","2");
		ctrMkNode("list",opt,-1,"/mod/dev/parity",_("Parity"),0664,"root","root",4,"tp","dec","idm","1","dest","select","select","/mod/dev/lsprt");
		ctrMkNode("list",opt,-1,"/mod/dev/frTm",_("Time frame"),0664,"root","root",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/charTm",_("Time char"),0664,"root","root",1,"tp","real");
		ctrMkNode("list",opt,-1,"/mod/dev/reqTm",_("Time request"),0664,"root","root",1,"tp","dec");
		ctrMkNode("list",opt,-1,"/mod/dev/open",_("Opened"),0664,"root","root",1,"tp","bool");
	    }
	}
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/mod/dev" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )
	{
	    //- Fill Archives table -
	    XMLNode *n_dev	= ctrMkNode("list",opt,-1,"/mod/dev/dev","");
	    XMLNode *n_speed	= ctrMkNode("list",opt,-1,"/mod/dev/speed","");
	    XMLNode *n_len	= ctrMkNode("list",opt,-1,"/mod/dev/len","");
	    XMLNode *n_stop	= ctrMkNode("list",opt,-1,"/mod/dev/stop","");
	    XMLNode *n_parity	= ctrMkNode("list",opt,-1,"/mod/dev/parity","");
	    XMLNode *n_frTm	= ctrMkNode("list",opt,-1,"/mod/dev/frTm","");
	    XMLNode *n_charTm	= ctrMkNode("list",opt,-1,"/mod/dev/charTm","");
	    XMLNode *n_reqTm	= ctrMkNode("list",opt,-1,"/mod/dev/reqTm","");
	    XMLNode *n_open	= ctrMkNode("list",opt,-1,"/mod/dev/open","");
	    vector<string> lst;
	    serDevList(lst);
	    for( int i_s = 0; i_s < lst.size(); i_s++ )
	    {
		AutoHD<SSerial>	ser = serDevAt(lst[i_s]);
		if( n_dev )	n_dev->childAdd("el")->setText(lst[i_s]);
		if( n_speed )	n_speed->childAdd("el")->setText(TSYS::int2str(ser.at().speed()));
		if( n_len )	n_len->childAdd("el")->setText(TSYS::int2str(ser.at().len()));
		if( n_stop )	n_stop->childAdd("el")->setText(TSYS::int2str(ser.at().twostop()?2:1));
		if( n_parity )	n_parity->childAdd("el")->setText(TSYS::int2str(ser.at().parity()));
		if( n_frTm )	n_frTm->childAdd("el")->setText(TSYS::int2str(ser.at().timeoutFrame()));
		if( n_charTm )	n_charTm->childAdd("el")->setText(TSYS::real2str(ser.at().timeoutChar()));
		if( n_reqTm )	n_reqTm->childAdd("el")->setText(TSYS::int2str(ser.at().timeoutReq()));
		if( n_open )	n_open->childAdd("el")->setText(TSYS::int2str(ser.at().hasOpen()));
	    }
	}
	if( ctrChkNode(opt,"add",0664,"root","root",SEQ_WR) )
	{
	    string devNm = "/dev/NewSDev";
	    if( serDevPresent(devNm) )
		for( int i_d = 0; true; i_d++ )
		    if( !serDevPresent(devNm+TSYS::int2str(i_d)) )
		    { devNm = devNm+TSYS::int2str(i_d); break; }
	    serDevAdd(devNm);
	}
	if( ctrChkNode(opt,"del",0664,"root","root",SEQ_WR) )
	    serDevDel(opt->attr("key_dev"),true);
	if( ctrChkNode(opt,"set",0666,"root","root",SEQ_WR) )
	{
	    string col	= opt->attr("col");
	    string dev  = opt->attr("key_dev");
	    if( col == "dev" )
	    {
		serDevAdd(opt->text());
		(TConfig&)serDevAt(opt->text()).at() = (TConfig&)serDevAt(dev).at();
		serDevAt(opt->text()).at().cfg("ID").setS(opt->text());
		serDevDel(dev,true);
	    }
	    else if( col == "speed" )	serDevAt(dev).at().setSpeed(atoi(opt->text().c_str()),true);
	    else if( col == "len" )	serDevAt(dev).at().setLen(atoi(opt->text().c_str()));
	    else if( col == "stop" )	serDevAt(dev).at().setTwostop(atoi(opt->text().c_str())==2);
	    else if( col == "parity" )	serDevAt(dev).at().setParity(atoi(opt->text().c_str()));
	    else if( col == "frTm" )	serDevAt(dev).at().setTimeoutFrame(atoi(opt->text().c_str()));
	    else if( col == "charTm" )	serDevAt(dev).at().setTimeoutChar(atof(opt->text().c_str()));
	    else if( col == "reqTm" )	serDevAt(dev).at().setTimeoutReq(atoi(opt->text().c_str()));
	    else if( col == "open" )	serDevAt(dev).at().setOpen(atoi(opt->text().c_str()));
	}
    }
    else if( a_path == "/mod/dev/lsspd" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id","300")->setText("300");
	opt->childAdd("el")->setAttr("id","600")->setText("600");
	opt->childAdd("el")->setAttr("id","1200")->setText("1200");
	opt->childAdd("el")->setAttr("id","2400")->setText("2400");
	opt->childAdd("el")->setAttr("id","4800")->setText("4800");
	opt->childAdd("el")->setAttr("id","9600")->setText("9600");
	opt->childAdd("el")->setAttr("id","19200")->setText("19.2k");
	opt->childAdd("el")->setAttr("id","38400")->setText("38.4k");
	opt->childAdd("el")->setAttr("id","57600")->setText("57.6k");
	opt->childAdd("el")->setAttr("id","115200")->setText("115.2k");
	opt->childAdd("el")->setAttr("id","230400")->setText("230.4k");
	opt->childAdd("el")->setAttr("id","460800")->setText("460.8k");
    }
    else if( a_path == "/mod/dev/lsprt" && ctrChkNode(opt) )
    {
	opt->childAdd("el")->setAttr("id","0")->setText(_("No check"));
	opt->childAdd("el")->setAttr("id","1")->setText(_("Parity"));
	opt->childAdd("el")->setAttr("id","2")->setText(_("Odd parity"));
    }
    else TTipDAQ::cntrCmdProc( opt );
}

//*************************************************
//* SSerial                                       *
//*************************************************
SSerial::SSerial( const string &iDev, TTpContr *iown ) :
    TCntrNode(iown), TConfig( &iown->serDevE() ), fd(-1),
    m_id(cfg("ID").getSd()), m_speed(cfg("SPEED").getId()), m_len(cfg("LEN").getId()),
    m_twostop(cfg("TWOSTOP").getBd()), m_parity(cfg("PARITY").getId()),
    frTm(cfg("TM_FRM").getId()), charTm(cfg("TM_CHAR").getRd()), reqTm(cfg("TM_REQ").getId())
{
    m_id = iDev;
}

void SSerial::postDisable( int flag )
{
    if( hasOpen() )	setOpen(false);
    //- Delete serial device from DB -
    try
    {
	if( flag )
	    SYS->db().at().dataDel(owner().serDevDB(),mod->nodePath()+"sDevs/",*this);
    }catch(TError err)
    { mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

void SSerial::load_( )
{
    SYS->db().at().dataGet(owner().serDevDB(),mod->nodePath()+"sDevs/",*this);
}

void SSerial::save_( )
{
    SYS->db().at().dataSet(owner().serDevDB(),mod->nodePath()+"sDevs/",*this);
}

void SSerial::setOpen( bool vl )
{
    ResAlloc res(m_res,true);
    if( vl == hasOpen() )	return;

    if( vl )
    {
	//- Serial port open -
	fd = open( id().c_str(), O_RDWR|O_NOCTTY );
	if( fd < 0 )	throw TError(mod->nodePath().c_str(),_("Serial port '%s' open error."),id().c_str());
	//-- Set serial port parameters --
	struct termios tio;
	bzero( &tio, sizeof(tio) );
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag = B9600|CS8|CREAD|CLOCAL;
	tio.c_lflag = 0;
	tio.c_cc[VTIME] = 0;           ///< inter-character timer unused
	tio.c_cc[VMIN] = 0;            ///< blocking read until 1 character arrives
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );
	res.release();
	//-- Set byte length --
	setLen(len());
	//-- Set stop bits number --
	setTwostop(twostop());
	//-- Set parity --
	setParity(parity());
	//-- Set speed --
	setSpeed(speed());
    }
    else
    {
	close(fd);
	fd = -1;
    }
}

void SSerial::setSpeed( int val, bool tmAdj )
{
    m_speed = val;

    if( hasOpen() )
    {
	ResAlloc res( m_res, true );

	struct termios tio;
	tcgetattr( fd, &tio );
	speed_t tspd = B9600;
	switch( val )
	{
	    case 300:	tspd = B300;	break;
	    case 600:	tspd = B600;	break;
	    case 1200:	tspd = B1200;	break;
	    case 2400:	tspd = B2400;	break;
	    case 4800:	tspd = B4800;	break;
	    case 9600:	tspd = B9600;	break;
	    case 19200:	tspd = B19200;	break;
	    case 38400:	tspd = B38400;	break;
	    case 57600:	tspd = B57600;	break;
	    case 115200:tspd = B115200;	break;
	    case 230400:tspd = B230400;	break;
	    case 460800:tspd = B460800;	break;
	}
	cfsetispeed( &tio, tspd );
	cfsetospeed( &tio, tspd );
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );
    }
    //- Times adjust -
    if( tmAdj )
    {
	setTimeoutChar(TSYS::realRound((12.*1000.*3.)/(double)val,2));
	setTimeoutFrame((12*1000*512)/val);
	setTimeoutReq(timeoutFrame()*2);
    }
    modif();
}

void SSerial::setLen( int val )
{
    m_len = vmin(8,vmax(5,val));

    if( hasOpen() )
    {
	ResAlloc res( m_res, true );

	struct termios tio;
	tcgetattr( fd, &tio );
	tio.c_cflag &= ~CSIZE;
	switch(m_len)
	{
	    case 5:	tio.c_cflag |= CS5;	break;
	    case 6:	tio.c_cflag |= CS6;	break;
	    case 7:	tio.c_cflag |= CS7;	break;
	    case 8:	tio.c_cflag |= CS8;	break;
	}
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );
    }
    modif();
}

void SSerial::setTwostop( bool val )
{
    m_twostop = val;

    if( hasOpen() )
    {
	ResAlloc res( m_res, true );

	struct termios tio;
	tcgetattr( fd, &tio );
	if( val )	tio.c_cflag &= ~CSTOPB;
	else		tio.c_cflag |= CSTOPB;
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );
    }
    modif();
}

void SSerial::setParity( int val )
{
    m_parity = val;

    if( hasOpen() )
    {
	ResAlloc res( m_res, true );

	struct termios tio;
	tcgetattr( fd, &tio );
	switch(val)
	{
	    case 0:
		tio.c_cflag &= ~PARENB;
		break;
	    case 1:
		tio.c_cflag |= PARENB;
		tio.c_cflag &= ~PARODD;
		break;
	    case 2:
		tio.c_cflag |= PARENB;
		tio.c_cflag |= PARODD;
		break;
	}
	tcflush( fd, TCIFLUSH );
	tcsetattr( fd, TCSANOW, &tio );
    }
    modif();
}

string SSerial::req( const string &vl, int req_len )
{
    ResAlloc res( m_res, true );
    if( !hasOpen() )	throw TError(mod->nodePath().c_str(),_("Serial port '%s' is not opened."),id().c_str());

    tcflush( fd, TCIFLUSH );

    //- Write request -
    if( write( fd, vl.data(), vl.size() ) == -1 )
	throw TError(mod->nodePath().c_str(),_("Write to serial port '%s' error."),id().c_str());

    //- Read reply -
    char buf[1000];
    int  blen = 0;
    long long tmptm;
    fd_set fdset;

    //-- Char timeout init --
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = vmax((int)(timeoutChar()*1000.0),1);

    //--- Serial timeout ---
    tmptm = TSYS::curTime();
    while( true )
    {
	int bytes = 0;
	ioctl( fd, FIONREAD, &bytes );
	if( bytes > req_len )	break;
	if( (TSYS::curTime() - tmptm) >= vmin(timeoutReq()*1000,10000000) )
	    throw TError(mod->nodePath().c_str(),_("Respond from serial port '%s' is timeouted."),id().c_str());
	usleep( 1000 );
    }
    fcntl( fd, F_SETFL, 0 );
    blen = read( fd, buf, sizeof(buf) );

//    cout << blen << "\n";

    //--- Frame timeout ---
    tmptm = TSYS::curTime();
    FD_ZERO( &fdset );
    FD_SET( fd, &fdset );
    while( true )
    {
        if( select(fd+1,&fdset,NULL,NULL,&tv) <= 0 )	break;
        blen += read( fd, buf+blen, sizeof(buf)-blen );
	if( (TSYS::curTime()-tmptm) > vmin(timeoutFrame()*1000,10000000) )	break;
    }

    return string( buf, blen );
}

//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem ) :
	TController( name_c, daq_db, cfgelem ), prc_st(false), endrun_req(false), tm_gath(0),
	m_per(cfg("PERIOD").getRd()), m_prior(cfg("PRIOR").getId()),
	m_addr(cfg("ADDR").getSd())
{
    cfg("PRM_BD").setS("DCONPrm_"+name_c);
}

TMdContr::~TMdContr()
{
    if( run_st ) stop();
}

TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
    return new TMdPrm( name, &owner().tpPrmAt(type) );
}

void TMdContr::disable_( )
{

}

void TMdContr::start_( )
{
    if( !prc_st )
    {
	//- Establish connection -
	mod->serDevAt(m_addr);

	//- Start the gathering data task -
	pthread_attr_t pthr_attr;
	pthread_attr_init( &pthr_attr );
	struct sched_param prior;
	if( m_prior && SYS->user() == "root" )
	    pthread_attr_setschedpolicy( &pthr_attr, SCHED_RR );
	else pthread_attr_setschedpolicy( &pthr_attr, SCHED_OTHER );
	prior.__sched_priority = m_prior;
	pthread_attr_setschedparam( &pthr_attr, &prior );

	pthread_create( &procPthr, &pthr_attr, TMdContr::Task, this );
	pthread_attr_destroy( &pthr_attr );
	if( TSYS::eventWait( prc_st, true, nodePath()+"start", 5 ) )
	    throw TError( nodePath().c_str(), _("Gathering task is not started!") );
    }
}

void TMdContr::stop_( )
{
    if( prc_st )
    {
	//- Stop the request and calc data task -
	endrun_req = true;
	pthread_kill( procPthr, SIGALRM );
	if( TSYS::eventWait( prc_st, false, nodePath()+"stop", 5 ) )
	    throw TError( nodePath().c_str(), _("Gathering task is not stopped!") );
	pthread_join( procPthr, NULL );
    }
}

bool TMdContr::cfgChange( TCfg &icfg )
{
    TController::cfgChange(icfg);
    return true;
}

void TMdContr::prmEn( const string &id, bool val )
{
    int i_prm;

    ResAlloc res( en_res, true );
    for( i_prm = 0; i_prm < p_hd.size(); i_prm++ )
	if( p_hd[i_prm].at().id() == id ) break;

    if( val && i_prm >= p_hd.size() )	p_hd.push_back(at(id));
    if( !val && i_prm < p_hd.size() )	p_hd.erase(p_hd.begin()+i_prm);
}

string TMdContr::DCONCRC( string str )
{
    const string HexSymbol[16] = {"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};
    unsigned char CRC=0;
    for (int i=0; i<str.size(); i++) CRC+=*str.substr(i,1).c_str();
    int iCRC=CRC;
    return HexSymbol[15 & (iCRC>>4)] + HexSymbol[15 & iCRC];
}

string TMdContr::DCONReq( string &pdu , int req_len)
{
    ResAlloc res( req_res, true );
    string rez = "";
    try
    {
	rez = mod->serDevAt(m_addr).at().req(pdu+"\r", req_len);
    }catch( TError err )
    {
	//mess_err( err.cat.c_str(), err.mess.c_str() );
	return _("14:Request error: ")+err.mess;
    }
    pdu = rez;
    return "";
}

void *TMdContr::Task( void *icntr )
{
    const string HexSymbol[16] = {"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};
    string pdu;
    int acq_len;
    long long work_tm, last_tm = 0;
    struct timespec get_tm;
    TMdContr &cntr = *(TMdContr *)icntr;

    cntr.endrun_req = false;
    cntr.prc_st = true;

    try
    {
	while( !cntr.endrun_req )
	{
	    long long t_cnt = SYS->shrtCnt();

	    //- Update controller's data -
	    ResAlloc res( cntr.en_res, false );
	    for( int i_p = 0; i_p < cntr.p_hd.size(); i_p++ )
		{
		    if( cntr.endrun_req ) break;
		    //- Request with I-7000 modules
		    switch (cntr.p_hd[i_p].at().mod_tp)
		    {
			//- Read DAQ parameter's for I-7051 module
			case 0:
			{
			    //- Request with I-7051 module
			    pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
			    if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
			    if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=7; else acq_len=5;
//			    cout << pdu.substr(0,pdu.size()) << " writing\n";
			    cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu,acq_len);
//			    cout << pdu.substr(0,pdu.size()-1) << " reading\n";

			    //Check errors
			    cntr.p_hd[i_p].at().module_err=false;
			    if ((pdu.size()-1)!=acq_len) cntr.p_hd[i_p].at().module_err=true;
			    if (!cntr.p_hd[i_p].at().module_err) if (pdu.substr(0,1)!=">") cntr.p_hd[i_p].at().module_err=true;
			    if ((cntr.p_hd[i_p].at().crc_ctrl)&&(!cntr.p_hd[i_p].at().module_err)) if ((pdu.substr(5,2))!=(cntr.DCONCRC(pdu.substr(0,5)))) cntr.p_hd[i_p].at().module_err=true;

			    //Set DAQ atributes
			    if (!cntr.p_hd[i_p].at().module_err)
			    {
				unsigned int DI = strtoul(pdu.substr(1,4).c_str(),NULL,16);
//				cout << DI << "\n";
				cntr.p_hd[i_p].at().DI[0]=0x00001 & DI;
				cntr.p_hd[i_p].at().DI[1]=0x00002 & DI;
				cntr.p_hd[i_p].at().DI[2]=0x00004 & DI;
				cntr.p_hd[i_p].at().DI[3]=0x00008 & DI;
				cntr.p_hd[i_p].at().DI[4]=0x00010 & DI;
				cntr.p_hd[i_p].at().DI[5]=0x00020 & DI;
				cntr.p_hd[i_p].at().DI[6]=0x00040 & DI;
				cntr.p_hd[i_p].at().DI[7]=0x00080 & DI;
				cntr.p_hd[i_p].at().DI[8]=0x00100 & DI;
				cntr.p_hd[i_p].at().DI[9]=0x00200 & DI;
				cntr.p_hd[i_p].at().DI[10]=0x00400 & DI;
				cntr.p_hd[i_p].at().DI[11]=0x00800 & DI;
				cntr.p_hd[i_p].at().DI[12]=0x01000 & DI;
				cntr.p_hd[i_p].at().DI[13]=0x02000 & DI;
				cntr.p_hd[i_p].at().DI[14]=0x04000 & DI;
				cntr.p_hd[i_p].at().DI[15]=0x08000 & DI;
			    }
			    break;
			}
			//- Write DAQ parameter's for I-7045 module
			case 1:
			{
			    //- Request with I-7045 module
			    pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
			    pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[15])+4*(1&cntr.p_hd[i_p].at().DO[14])+2*(1&cntr.p_hd[i_p].at().DO[13])+(1&cntr.p_hd[i_p].at().DO[12])];
			    pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[11])+4*(1&cntr.p_hd[i_p].at().DO[10])+2*(1&cntr.p_hd[i_p].at().DO[9])+(1&cntr.p_hd[i_p].at().DO[8])];
			    pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[7])+4*(1&cntr.p_hd[i_p].at().DO[6])+2*(1&cntr.p_hd[i_p].at().DO[5])+(1&cntr.p_hd[i_p].at().DO[4])];
			    pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[3])+4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
			    if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
			    if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//			    cout << pdu.substr(0,pdu.size()) << " writing\n";
			    cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu,acq_len);
//			    cout << pdu.substr(0,pdu.size()-1) << " reading\n";

			    //Check errors
			    cntr.p_hd[i_p].at().module_err=false;
			    if ((pdu.size()-1)!=acq_len) cntr.p_hd[i_p].at().module_err=true;
			    if (!cntr.p_hd[i_p].at().module_err) if (pdu.substr(0,1)!=">") cntr.p_hd[i_p].at().module_err=true;
			    if ((cntr.p_hd[i_p].at().crc_ctrl)&&(!cntr.p_hd[i_p].at().module_err)) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1)))) cntr.p_hd[i_p].at().module_err=true;
			    break;
			}
			//- Read-Write DAQ parameter's for I-7063 module
			case 2:
			{
			    //- Request with I-7063 module
			    pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
			    if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
			    if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=7; else acq_len=5;
//			    cout << pdu.substr(0,pdu.size()) << " writing\n";
			    cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu,acq_len);
//			    cout << pdu.substr(0,pdu.size()-1) << " reading\n";

			    //Check errors
			    cntr.p_hd[i_p].at().module_err=false;
			    if ((pdu.size()-1)!=acq_len) cntr.p_hd[i_p].at().module_err=true;
			    if (!cntr.p_hd[i_p].at().module_err) if (pdu.substr(0,1)!=">") cntr.p_hd[i_p].at().module_err=true;
			    if ((cntr.p_hd[i_p].at().crc_ctrl)&&(!cntr.p_hd[i_p].at().module_err)) if ((pdu.substr(5,2))!=(cntr.DCONCRC(pdu.substr(0,5)))) cntr.p_hd[i_p].at().module_err=true;

			    //Set DAQ atributes
			    if (!cntr.p_hd[i_p].at().module_err)
			    {
				unsigned int DI = strtoul(pdu.substr(3,2).c_str(),NULL,16);
//				cout << DI << "\n";
				cntr.p_hd[i_p].at().DI[0]=0x00001 & DI;
				cntr.p_hd[i_p].at().DI[1]=0x00002 & DI;
				cntr.p_hd[i_p].at().DI[2]=0x00004 & DI;
				cntr.p_hd[i_p].at().DI[3]=0x00008 & DI;
				cntr.p_hd[i_p].at().DI[4]=0x00010 & DI;
				cntr.p_hd[i_p].at().DI[5]=0x00020 & DI;
				cntr.p_hd[i_p].at().DI[6]=0x00040 & DI;
				cntr.p_hd[i_p].at().DI[7]=0x00080 & DI;
			    }

			    //- Request with I-7063 module
			    pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
			    pdu+=HexSymbol[4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
			    if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
			    if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//			    cout << pdu.substr(0,pdu.size()) << " writing\n";
			    cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu,acq_len);
//			    cout << pdu.substr(0,pdu.size()-1) << " reading\n";

			    //Check errors
			    if ((pdu.size()-1)!=acq_len) cntr.p_hd[i_p].at().module_err=true;
			    if (!cntr.p_hd[i_p].at().module_err) if (pdu.substr(0,1)!=">") cntr.p_hd[i_p].at().module_err=true;
			    if ((cntr.p_hd[i_p].at().crc_ctrl)&&(!cntr.p_hd[i_p].at().module_err)) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1)))) cntr.p_hd[i_p].at().module_err=true;
			    break;
			}
			//- Read DAQ parameter's for I-7017 module
			case 3:
			{
			    //- Request with I-7017 module
			    pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
			    if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
			    if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=59; else acq_len=57;
//			    cout << pdu.substr(0,pdu.size()) << " writing\n";
			    cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu,57);
//			    cout << pdu.substr(0,pdu.size()-1) << " reading\n";

			    //Check errors
			    cntr.p_hd[i_p].at().module_err=false;
			    if ((pdu.size()-1)!=acq_len) cntr.p_hd[i_p].at().module_err=true;
			    if (!cntr.p_hd[i_p].at().module_err) if (pdu.substr(0,1)!=">") cntr.p_hd[i_p].at().module_err=true;
			    if ((cntr.p_hd[i_p].at().crc_ctrl)&&(!cntr.p_hd[i_p].at().module_err)) if ((pdu.substr(57,2))!=(cntr.DCONCRC(pdu.substr(0,57)))) cntr.p_hd[i_p].at().module_err=true;

			    //Set DAQ atributes
			    if (!cntr.p_hd[i_p].at().module_err)
			    {
				cntr.p_hd[i_p].at().AI[0]=atof(pdu.substr(1,7).c_str());
				cntr.p_hd[i_p].at().AI[1]=atof(pdu.substr(8,7).c_str());
				cntr.p_hd[i_p].at().AI[2]=atof(pdu.substr(15,7).c_str());
				cntr.p_hd[i_p].at().AI[3]=atof(pdu.substr(22,7).c_str());
				cntr.p_hd[i_p].at().AI[4]=atof(pdu.substr(29,7).c_str());
				cntr.p_hd[i_p].at().AI[5]=atof(pdu.substr(36,7).c_str());
				cntr.p_hd[i_p].at().AI[6]=atof(pdu.substr(43,7).c_str());
				cntr.p_hd[i_p].at().AI[7]=atof(pdu.substr(50,7).c_str());
			    }
			    break;
			}
			//- Write DAQ parameter's for I-7024 module
			case 4:
			{
			    int dec, sign;
			    int ndig = 3;

			    //- Request with I-7024 module
			    string str=fcvt(cntr.p_hd[i_p].at().AO[0],ndig,&dec,&sign);
			    if (dec==1) str="0"+str;
			    str="+"+str.substr(0,2)+"."+str.substr(2,3);
			    pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr]+"0"+str;
			    if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
			    if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//			    cout << pdu.substr(0,pdu.size()) << " writing\n";
			    cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu,acq_len);
//			    cout << pdu.substr(0,pdu.size()-1) << " reading\n";

			    //Check errors
			    cntr.p_hd[i_p].at().module_err=false;
			    if ((pdu.size()-1)!=acq_len) cntr.p_hd[i_p].at().module_err=true;
			    if (!cntr.p_hd[i_p].at().module_err) if (pdu.substr(0,1)!=">") cntr.p_hd[i_p].at().module_err=true;
			    if ((cntr.p_hd[i_p].at().crc_ctrl)&&(!cntr.p_hd[i_p].at().module_err)) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1)))) cntr.p_hd[i_p].at().module_err=true;

			    //- Request with I-7024 module
			    str=fcvt(cntr.p_hd[i_p].at().AO[1],ndig,&dec,&sign);
			    if (dec==1) str="0"+str;
			    str="+"+str.substr(0,2)+"."+str.substr(2,3);
			    pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr]+"1"+str;
			    if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
			    if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//			    cout << pdu.substr(0,pdu.size()) << " writing\n";
			    cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu,acq_len);
//			    cout << pdu.substr(0,pdu.size()-1) << " reading\n";

			    //Check errors
			    if ((pdu.size()-1)!=acq_len) cntr.p_hd[i_p].at().module_err=true;
			    if (!cntr.p_hd[i_p].at().module_err) if (pdu.substr(0,1)!=">") cntr.p_hd[i_p].at().module_err=true;
			    if ((cntr.p_hd[i_p].at().crc_ctrl)&&(!cntr.p_hd[i_p].at().module_err)) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1)))) cntr.p_hd[i_p].at().module_err=true;

			    //- Request with I-7024 module
			    str=fcvt(cntr.p_hd[i_p].at().AO[2],ndig,&dec,&sign);
			    if (dec==1) str="0"+str;
			    str="+"+str.substr(0,2)+"."+str.substr(2,3);
			    pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr]+"2"+str;
			    if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
			    if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//			    cout << pdu.substr(0,pdu.size()) << " writing\n";
			    cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu,acq_len);
//			    cout << pdu.substr(0,pdu.size()-1) << " reading\n";

			    //Check errors
			    if ((pdu.size()-1)!=acq_len) cntr.p_hd[i_p].at().module_err=true;
			    if (!cntr.p_hd[i_p].at().module_err) if (pdu.substr(0,1)!=">") cntr.p_hd[i_p].at().module_err=true;
			    if ((cntr.p_hd[i_p].at().crc_ctrl)&&(!cntr.p_hd[i_p].at().module_err)) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1)))) cntr.p_hd[i_p].at().module_err=true;

			    //- Request with I-7024 module
			    str=fcvt(cntr.p_hd[i_p].at().AO[3],ndig,&dec,&sign);
			    if (dec==1) str="0"+str;
			    str="+"+str.substr(0,2)+"."+str.substr(2,3);
			    pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr]+"3"+str;
			    if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
			    if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//			    cout << pdu.substr(0,pdu.size()) << " writing\n";
			    cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu,acq_len);
//			    cout << pdu.substr(0,pdu.size()-1) << " reading\n";

			    //Check errors
			    if ((pdu.size()-1)!=acq_len) cntr.p_hd[i_p].at().module_err=true;
			    if (!cntr.p_hd[i_p].at().module_err) if (pdu.substr(0,1)!=">") cntr.p_hd[i_p].at().module_err=true;
			    if ((cntr.p_hd[i_p].at().crc_ctrl)&&(!cntr.p_hd[i_p].at().module_err)) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1)))) cntr.p_hd[i_p].at().module_err=true;

			    break;
			}
			default:
			{
			    break;
			}
		    }
		}
	    res.release();

	    //- Calc acquisition process time -
	    cntr.tm_gath = 1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk());

	    //- Calc next work time and sleep -
	    clock_gettime( CLOCK_REALTIME, &get_tm );
	    work_tm = (((long long)get_tm.tv_sec*1000000000+get_tm.tv_nsec)/(long long)(cntr.period()*1000000000) + 1)*(long long)(cntr.period()*1000000000);
	    if( last_tm == work_tm )	work_tm += (long long)(cntr.period()*1000000000);	//Fix early call
	    last_tm = work_tm;
	    get_tm.tv_sec = work_tm/1000000000; get_tm.tv_nsec = work_tm%1000000000;
	    clock_nanosleep( CLOCK_REALTIME, TIMER_ABSTIME, &get_tm, NULL );
	}
    }
    catch( TError err )	{ mess_err( err.cat.c_str(), err.mess.c_str() ); }

    cntr.prc_st = false;

    return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TController::cntrCmdProc(opt);
	ctrMkNode( "fld", opt, -1, "/cntr/st/gath_tm", _("Gather data time (ms)"), 0444, "root", "root", 1, "tp", "real" );
	return;
    }
    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/cntr/st/gath_tm" && ctrChkNode(opt) )	opt->setText(TSYS::real2str(tm_gath,6));
    else TController::cntrCmdProc(opt);
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : 
    TParamContr( name, tp_prm ), p_el("w_attr"), mod_tp(cfg("MOD_TP").getId()), crc_ctrl(cfg("CRC_CTRL").getBd()),
	mod_addr(cfg("MOD_ADDR").getId())
{
    // Default input/output data
    for( int i = 0; i < 16; i++ ) {DI[i]=0;DO[i]=0;AI[i]=0;AO[i]=0;}
}

TMdPrm::~TMdPrm( )
{
    nodeDelAll( );
}

void TMdPrm::postEnable( int flag )
{
    TParamContr::postEnable(flag);
    if( !vlElemPresent(&p_el) )	vlElemAtt(&p_el);
}

void TMdPrm::enable()
{
    if( enableStat() )	return;

    TParamContr::enable();

    //- Delete DAQ parameter's attributes -
    while(p_el.fldSize()>0)
    {
	try{ p_el.fldDel(0); }
	catch(TError err){ mess_warning(err.cat.c_str(),err.mess.c_str()); }
    }

    //- Make DAQ parameter's attributes
    switch(mod_tp)
    {
	//- Make DAQ parameter's for I-7051 module
	case 0:
	{
	    p_el.fldAdd( new TFld("module_err", "module_err", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI0", "DI0", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI1", "DI1", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI2", "DI2", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI3", "DI3", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI4", "DI4", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI5", "DI5", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI6", "DI6", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI7", "DI7", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI8", "DI8", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI9", "DI9", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI10", "DI10", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI11", "DI11", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI12", "DI12", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI13", "DI13", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI14", "DI14", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI15", "DI15", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    break;
	}
	//- Make DAQ parameter's for I-7045 module
	case 1:
	{
	    p_el.fldAdd( new TFld("module_err", "module_err", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO0", "DO0", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO1", "DO1", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO2", "DO2", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO3", "DO3", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO4", "DO4", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO5", "DO5", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO6", "DO6", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO7", "DO7", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO8", "DO8", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO9", "DO9", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO10", "DO10", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO11", "DO11", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO12", "DO12", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO13", "DO13", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO14", "DO14", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO15", "DO15", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    break;
	}
	//- Make DAQ parameter's for I-7063 module
	case 2:
	{
	    p_el.fldAdd( new TFld("module_err", "module_err", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI0", "DI0", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI1", "DI1", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI2", "DI2", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI3", "DI3", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI4", "DI4", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI5", "DI5", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI6", "DI6", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("DI7", "DI7", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );

	    p_el.fldAdd( new TFld("DO0", "DO0", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO1", "DO1", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    p_el.fldAdd( new TFld("DO2", "DO2", TFld::Boolean, TVal::DirRead|TVal::DirWrite, "", "", "" ) );
	    break;
	}
	case 3:
	{
	    p_el.fldAdd( new TFld("module_err", "module_err", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("AI0", "AI0", TFld::Real, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("AI1", "AI1", TFld::Real, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("AI2", "AI2", TFld::Real, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("AI3", "AI3", TFld::Real, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("AI4", "AI4", TFld::Real, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("AI5", "AI5", TFld::Real, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("AI6", "AI6", TFld::Real, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("AI7", "AI7", TFld::Real, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    break;
	}
	case 4:
	{
	    p_el.fldAdd( new TFld("module_err", "module_err", TFld::Boolean, TFld::NoWrite|TVal::DirRead, "", "", "" ) );
	    p_el.fldAdd( new TFld("AO0", "AO0", TFld::Real, TVal::DirRead|TVal::DirWrite,"20","1","4;20") );
	    p_el.fldAdd( new TFld("AO1", "AO1", TFld::Real, TVal::DirRead|TVal::DirWrite,"20","1","4;20") );
	    p_el.fldAdd( new TFld("AO2", "AO2", TFld::Real, TVal::DirRead|TVal::DirWrite,"20","1","4;20") );
	    p_el.fldAdd( new TFld("AO3", "AO3", TFld::Real, TVal::DirRead|TVal::DirWrite,"20","1","4;20") );
	    break;
	}
	default:
	{
	    break;
	}
    }

    owner().prmEn( id(), true );
}

void TMdPrm::disable()
{
    if( !enableStat() )  return;

    owner().prmEn( id(), false );

    TParamContr::disable();

    //- Set EVAL to parameter attributes -
    vector<string> ls;
    elem().fldList(ls);
    for(int i_el = 0; i_el < ls.size(); i_el++)
	vlAt(ls[i_el]).at().setS( EVAL_STR, 0, true );
}

void TMdPrm::vlGet( TVal &val )
{
    if( !enableStat() || !owner().startStat() )
    {
	if( val.name() == "err" )
	{
	    if( !enableStat() )			val.setS(_("1:Parameter is disabled."),0,true);
	    else if(!owner().startStat())	val.setS(_("2:Acquisition is stoped."),0,true);
	}
	else val.setS(EVAL_STR,0,true);
	return;
    }

//    cout<<val.name().substr(0,2)<<"  "<<atoi(val.name().substr(2,val.name().size()-2).c_str())<<"\n";

    if (val.name()=="module_err") val.setB(module_err,0,true);
    if (val.name().substr(0,2)=="DI") val.setB(DI[atoi(val.name().substr(2,val.name().size()-2).c_str())],0,true);
    if (val.name().substr(0,2)=="DO") val.setB(DO[atoi(val.name().substr(2,val.name().size()-2).c_str())],0,true);
    if (val.name().substr(0,2)=="AI") val.setR(AI[atoi(val.name().substr(2,val.name().size()-2).c_str())],0,true);
    if (val.name().substr(0,2)=="AO") val.setR(AO[atoi(val.name().substr(2,val.name().size()-2).c_str())],0,true);

//    if (val.name()=="err") val.setS(_("No errors."),0,true);
}

void TMdPrm::vlSet( TVal &valo )
{
    if( !enableStat() )	valo.setI( EVAL_INT, 0, true );

    if (valo.name().substr(0,2)=="DI") DI[atoi(valo.name().substr(2,valo.name().size()-2).c_str())]=valo.getB(NULL,true);
    if (valo.name().substr(0,2)=="DO") DO[atoi(valo.name().substr(2,valo.name().size()-2).c_str())]=valo.getB(NULL,true);
    if (valo.name().substr(0,2)=="AI") AI[atoi(valo.name().substr(2,valo.name().size()-2).c_str())]=valo.getR(NULL,true);
    if (valo.name().substr(0,2)=="AO") AO[atoi(valo.name().substr(2,valo.name().size()-2).c_str())]=valo.getR(NULL,true);
}

void TMdPrm::vlArchMake( TVal &val )
{
    if( val.arch().freeStat() ) return;
    val.arch().at().setSrcMode( TVArchive::ActiveAttr, val.arch().at().srcData() );
    val.arch().at().setPeriod( (long long)(owner().period()*1000000) );
    val.arch().at().setHardGrid( true );
    val.arch().at().setHighResTm( true );
}
