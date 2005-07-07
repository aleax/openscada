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
 
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <getopt.h>
#include <string>
#include <errno.h>

#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include <tprotocols.h>
#include <tmodule.h>
#include "socket.h"

//============ Modul info! =====================================================
#define MOD_ID	    "socket"
#define MOD_NAME    "Transport \"socket\""
#define MOD_TYPE    "Transport"
#define VER_TYPE    VER_TR
#define VERSION     "1.1.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "All3Cow transport \"socket\". Transport make a communication based for inet, unix sockets. Inet socket support TCP and UDP."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    TModule::SAt module( int n_mod )
    {
    	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
	    AtMod.t_ver = VER_TYPE;
	}
	else
    	    AtMod.id	= "";

    	return( AtMod );
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	Sockets::TTransSock *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new Sockets::TTransSock( source );       

	return ( self_addr );
    }
}

using namespace Sockets;

//==============================================================================
//== TTransSock ================================================================
//==============================================================================
    
TTransSock::TTransSock( string name ) 
    : max_queue(10), max_fork(10), buf_len(4)
{
    mId		= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= name;
}

TTransSock::~TTransSock()
{

}


string TTransSock::optDescr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"max_sock_queue <len>      length of the queue for TCP and UNIX sockets (default 10);\n"
	"max_fork       <connects> maximum number of opened client's TCP and UNIX sockets (default 10);\n"
	"buf_len        <kb>       length of the input buffer (default 4 kb);\n\n"),
	MOD_TYPE,MOD_ID,MOD_ID);

    return(buf);
}

void TTransSock::modLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);    
    
    //========== Load parameters from config file =============
    try{ max_queue = atoi( modCfgNode()->childGet("id","max_sock_queue")->text().c_str() ); }
    catch(...) {  }
    try{ max_fork = atoi( modCfgNode()->childGet("id","max_fork")->text().c_str() ); }
    catch(...) {  }
    try{ buf_len = atoi( modCfgNode()->childGet("id","buf_len")->text().c_str() ); }
    catch(...) {  }
}

TTransportIn *TTransSock::In( const string &name )
{
    return( new TSocketIn(name,this) );
}

TTransportOut *TTransSock::Out( const string &name )
{
    return( new TSocketOut(name,this) );
}

//================== Controll functions ========================
void TTransSock::cntrCmd_( const string &a_path, XMLNode *opt, int cmd )
{
    if( cmd==TCntrNode::Info )
    {	
	TTipTransport::cntrCmd_( a_path, opt, cmd );

	ctrInsNode("area",1,opt,a_path.c_str(),"/bs",I18N(MOD_NAME));	
	ctrMkNode("area",opt,a_path.c_str(),"/bs/opt",I18N("The input transport options"),0440);
	ctrMkNode("fld",opt,a_path.c_str(),"/bs/opt/q_ln",I18N("Queue length for TCP and UNIX sockets"),0660,0,0,"dec");
	ctrMkNode("fld",opt,a_path.c_str(),"/bs/opt/cl_n",I18N("Maximum number opened client TCP and UNIX sockets"),0660,0,0,"dec");
	ctrMkNode("fld",opt,a_path.c_str(),"/bs/opt/bf_ln",I18N("Input buffer length (kbyte)"),0660,0,0,"dec");
	ctrMkNode("fld",opt,a_path.c_str(),"/help/g_help",Mess->I18N("Options help"),0440,0,0,"str")->
	    attr_("cols","90")->attr_("rows","5");
    }
    else if( cmd==TCntrNode::Get )
    {
	if( a_path == "/bs/opt/q_ln" )		ctrSetI( opt, max_queue );
	else if( a_path == "/bs/opt/cl_n" )	ctrSetI( opt, max_fork );
	else if( a_path == "/bs/opt/bf_ln" )	ctrSetI( opt, buf_len );
	else if( a_path == "/help/g_help" ) 	ctrSetS( opt, optDescr() );       
    	else TTipTransport::cntrCmd_( a_path, opt, cmd );
    }
    else if( cmd==TCntrNode::Set )
    {
	if( a_path == "/bs/opt/q_ln" )        max_queue = ctrGetI( opt );
	else if( a_path == "/bs/opt/cl_n" )   max_fork  = ctrGetI( opt );
	else if( a_path == "/bs/opt/bf_ln" )  buf_len   = ctrGetI( opt );
	else TTipTransport::cntrCmd_( a_path, opt, cmd );
    }
}    

//==============================================================================
//== TSocketIn =================================================================
//==============================================================================

TSocketIn::TSocketIn( string name, TTipTransport *n_owner ) : 
    TTransportIn(name,n_owner), cl_free(true), max_queue(((TTransSock *)n_owner)->max_queue), 
    max_fork(((TTransSock &)owner()).max_fork), buf_len(((TTransSock *)n_owner)->buf_len)
{
    sock_res = ResAlloc::resCreate();
}

TSocketIn::~TSocketIn()
{
    try{ stop(); }catch(...){ }
    ResAlloc::resDelete(sock_res);
}

void TSocketIn::start()
{
    pthread_attr_t pthr_attr;

    if( run_st ) throw TError("(%s) Input transport <%s> started!",MOD_ID,name().c_str());

    string s_type = TSYS::strSepParse(m_addr,0,':');
        
    if( s_type == S_NM_TCP )
    {
    	if( (sock_fd = socket(PF_INET,SOCK_STREAM,0) )== -1 ) 
    	    throw TError("%s: error create %s socket!",MOD_ID,s_type.c_str());
	type = SOCK_TCP;
    }
    else if( s_type == S_NM_UDP )
    {	
	if( (sock_fd = socket(PF_INET,SOCK_DGRAM,0) )== -1 ) 
    	    throw TError("%s: error create %s socket!",MOD_ID,s_type.c_str());
	type = SOCK_UDP;
    }
    else if( s_type == S_NM_UNIX )
    {
    	if( (sock_fd = socket(PF_UNIX,SOCK_STREAM,0) )== -1) 
    	    throw TError("%s: error create %s socket!",MOD_ID,s_type.c_str());
	type = SOCK_UNIX;
    }
    else throw TError("%s: type socket <%s> error!",MOD_ID,s_type.c_str());

    if( type == SOCK_TCP || type == SOCK_UDP )
    {
	struct sockaddr_in  name_in;
	struct hostent *loc_host_nm;
	memset(&name_in,0,sizeof(name_in));
	name_in.sin_family = AF_INET;
        
	host	= TSYS::strSepParse(m_addr,1,':');
	port	= TSYS::strSepParse(m_addr,2,':');
	if( host.size() )
	{
	    loc_host_nm = gethostbyname(host.c_str());
	    if(loc_host_nm == NULL || loc_host_nm->h_length == 0)
		throw TError("%s: socket name <%s> error!",MOD_ID,host.c_str());
	    name_in.sin_addr.s_addr = *( (int *) (loc_host_nm->h_addr_list[0]) );
	}
	else name_in.sin_addr.s_addr = INADDR_ANY;  
	if( type == SOCK_TCP )
	{
	    mode	= atoi( TSYS::strSepParse(m_addr,3,':').c_str() );
	    //Get system port for "oscada" /etc/services
	    struct servent *sptr = getservbyname(port.c_str(),"tcp");
	    if( sptr != NULL )                       name_in.sin_port = sptr->s_port;
	    else if( htons(atol(port.c_str())) > 0 ) name_in.sin_port = htons( atol(port.c_str()) );
	    else name_in.sin_port = 10001;
	    
    	    if( bind(sock_fd,(sockaddr *)&name_in,sizeof(name_in) ) == -1) 
	    {
	    	shutdown( sock_fd,SHUT_RDWR );
		close( sock_fd );
		throw TError("%s: TCP socket no bind <%s>!",MOD_ID,m_addr.c_str());
	    }
	    listen(sock_fd,max_queue);
	}
	else if(type == SOCK_UDP )
	{
	    //Get system port for "oscada" /etc/services
	    struct servent *sptr = getservbyname(port.c_str(),"udp");
	    if( sptr != NULL )                       name_in.sin_port = sptr->s_port;
	    else if( htons(atol(port.c_str())) > 0 ) name_in.sin_port = htons( atol(port.c_str()) );
	    else name_in.sin_port = 10001;
	    
    	    if( bind(sock_fd,(sockaddr *)&name_in,sizeof(name_in) ) == -1) 
	    {
	    	shutdown( sock_fd,SHUT_RDWR );
		close( sock_fd );
		throw TError("%s: UDP socket no bind <%s>!",MOD_ID,m_addr.c_str());
	    }
	}
    }
    else if( type == SOCK_UNIX )
    {
	path	= TSYS::strSepParse(m_addr,1,':');
	mode	= atoi( TSYS::strSepParse(m_addr,2,':').c_str() );
	if( !path.size() ) path = "/tmp/oscada";	
	remove( path.c_str());
	struct sockaddr_un  name_un;	
	memset(&name_un,0,sizeof(name_un));
	name_un.sun_family = AF_UNIX;
	strncpy( name_un.sun_path,path.c_str(),sizeof(name_un.sun_path) );
	if( bind(sock_fd,(sockaddr *)&name_un,sizeof(name_un) ) == -1) 
	{
	    close( sock_fd );
	    throw TError("%s: UNIX socket no bind <%s>!",MOD_ID,m_addr.c_str());
	}
	listen(sock_fd,max_queue);
    }    
    
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( TSYS::eventWait( run_st, true, string(MOD_ID)+": SocketIn "+name()+" is opening....",5) )
       	throw TError("%s: SocketIn %s no open!",MOD_ID,name().c_str());   
}

void TSocketIn::stop()
{
    if( !run_st ) throw TError("(%s) Input transport <%s> stoped!",MOD_ID,name().c_str());
    
    endrun = true;
    TSYS::eventWait( run_st, false, string(MOD_ID)+": SocketIn "+name()+" is closing....",5);
    pthread_join( pthr_tsk, NULL );
    
    shutdown(sock_fd,SHUT_RDWR);
    close(sock_fd); 
    if( type == SOCK_UNIX ) remove( path.c_str() );
}

void *TSocketIn::Task(void *sock_in)
{  
    char           *buf;   
    fd_set         rd_fd;
    struct timeval tv;
    TSocketIn *sock = (TSocketIn *)sock_in;
    AutoHD<TProtocolIn> prot_in;

#if OSC_DEBUG
    sock->owner().mPut("DEBUG",TMess::Debug,"%s:Thread <%d>!",sock->name().c_str(),getpid() );
#endif	
    
    pthread_t      th;
    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_attr_setdetachstate(&pthr_attr, PTHREAD_CREATE_DETACHED);
    
    sock->run_st    = true;  
    sock->endrun_cl = false;  
    sock->endrun    = false;
    
    if( sock->type == SOCK_UDP ) 
	buf = new char[sock->buf_len*1000 + 1];
		
    while( !sock->endrun )
    {
	tv.tv_sec  = 0;
	tv.tv_usec = STD_WAIT_DELAY*1000;  
    	FD_ZERO(&rd_fd);
	FD_SET(sock->sock_fd,&rd_fd);		
	
	int kz = select(sock->sock_fd+1,&rd_fd,NULL,NULL,&tv);
	if( kz == 0 || (kz == -1 && errno == EINTR) ) continue;
	if( kz < 0) continue;
	if( FD_ISSET(sock->sock_fd, &rd_fd) )
	{
	    struct sockaddr_in name_cl;
	    socklen_t          name_cl_len = sizeof(name_cl);			    
	    if( sock->type == SOCK_TCP )
	    {
		int sock_fd_CL = accept(sock->sock_fd, (sockaddr *)&name_cl, &name_cl_len);
		if( sock_fd_CL != -1 )
		{
		    if( sock->max_fork <= (int)sock->cl_id.size() )
		    {
			close(sock_fd_CL);
			continue;
		    }
		    SSockIn *s_inf = new SSockIn;
		    s_inf->s_in    = sock;
		    s_inf->cl_sock = sock_fd_CL;
		    s_inf->sender  = inet_ntoa(name_cl.sin_addr);		    
		    if( pthread_create(&th,&pthr_attr,ClTask,s_inf) < 0)
    		        sock->owner().mPut("SYS",TMess::Error,"%s:Error create pthread!",sock->name().c_str() );
		}
	    }
	    else if( sock->type == SOCK_UNIX )
	    {
		int sock_fd_CL = accept(sock->sock_fd, NULL, NULL);
		if( sock_fd_CL != -1 )
		{
		    if( sock->max_fork <= (int)sock->cl_id.size() )
		    {
			close(sock_fd_CL);
			continue;
		    }		    
		    SSockIn *s_inf = new SSockIn;
		    s_inf->s_in    = sock;
		    s_inf->cl_sock = sock_fd_CL;		    
                    if( pthread_create(&th,&pthr_attr,ClTask,s_inf) < 0 ) 
		    	sock->owner().mPut("SYS",TMess::Error,"%s:Error create pthread!",sock->name().c_str() );
		}	    
	    }
	    else if( sock->type == SOCK_UDP )
    	    {
    		int r_len;//, hds=-1;
    		string  req, answ;

		r_len = recvfrom(sock->sock_fd, buf, sock->buf_len*1000, 0,(sockaddr *)&name_cl, &name_cl_len);
		if( r_len <= 0 ) continue;
		req.assign(buf,r_len);
		Mess->put("TRANSPORT",TMess::Info,sock->owner().I18N("The socket <%s> receive a datagram <%d> from <%s>!"),
			sock->name().c_str(), r_len, inet_ntoa(name_cl.sin_addr) );
		
		sock->PutMess(sock->sock_fd, req, answ, inet_ntoa(name_cl.sin_addr),prot_in);
		if( !prot_in.freeStat() ) continue;		
		    
		Mess->put("TRANSPORT",TMess::Info,sock->owner().I18N("The socket <%s> reply a datagram <%d> to <%s>!"),
			sock->name().c_str(), answ.size(), inet_ntoa(name_cl.sin_addr) );
		sendto(sock->sock_fd,answ.c_str(),answ.size(),0,(sockaddr *)&name_cl, name_cl_len);
	    }
	}
    }
    pthread_attr_destroy(&pthr_attr);
    
    if( sock->type == SOCK_UDP ) delete []buf;
    //Client tasks stop command
    sock->endrun_cl = true;
    TSYS::eventWait( sock->cl_free, true, string(MOD_ID)+": "+sock->name()+" client task is stoping....");

    sock->run_st = false;
    
    return(NULL);
}

void *TSocketIn::ClTask(void *s_inf)
{
    SSockIn *s_in = (SSockIn *)s_inf;    
    
#if OSC_DEBUG
    s_in->s_in->owner().mPut("DEBUG",TMess::Debug,"%s:Client thread <%d>!",s_in->s_in->name().c_str(),getpid() );
#endif	
    
    Mess->put("TRANSPORT",TMess::Info,s_in->s_in->owner().I18N("The socket <%s> have been connected by <%s>!"),s_in->s_in->name().c_str(),s_in->sender.c_str() );   
    s_in->s_in->RegClient( getpid( ), s_in->cl_sock );
    s_in->s_in->ClSock( *s_in );
    s_in->s_in->UnregClient( getpid( ) );
    Mess->put("TRANSPORT",TMess::Info,s_in->s_in->owner().I18N("The socket <%s> have been disconnected by <%s>!"),s_in->s_in->name().c_str(),s_in->sender.c_str() );
    delete s_in;

    pthread_exit(NULL);    
}

void TSocketIn::ClSock( SSockIn &s_in )
{
    struct  timeval tv;
    fd_set  rd_fd;
    int     r_len;
    string  req, answ;
    char    buf[buf_len*1000 + 1];    
    AutoHD<TProtocolIn> prot_in;
    
    if(mode)
    {
    	while( !endrun_cl )
	{
    	    tv.tv_sec  = 0;
    	    tv.tv_usec = STD_WAIT_DELAY*1000;  
	    FD_ZERO(&rd_fd);
	    FD_SET(s_in.cl_sock,&rd_fd);		
	    
	    int kz = select(s_in.cl_sock+1,&rd_fd,NULL,NULL,&tv);
	    if( kz == 0 || (kz == -1 && errno == EINTR) ) continue;
	    if( kz < 0) continue;
	    if( FD_ISSET(s_in.cl_sock, &rd_fd) )
	    {
		r_len = read(s_in.cl_sock,buf,buf_len*1000);
		Mess->put("TRANSPORT",TMess::Info,s_in.s_in->owner().I18N("The socket <%s> receive of message <%d> from <%s>!"),
		    s_in.s_in->name().c_str(), r_len, s_in.sender.c_str() );
    		if(r_len <= 0) break;
    		req.assign(buf,r_len);

		PutMess(s_in.cl_sock,req,answ,s_in.sender,prot_in);
		if( !prot_in.freeStat() ) continue;		
    		Mess->put("TRANSPORT",TMess::Info,s_in.s_in->owner().I18N("The socket <%s> reply of message <%d> to <%s>!"),
		    s_in.s_in->name().c_str(), answ.size(), s_in.sender.c_str() );
	    	r_len = write(s_in.cl_sock,answ.c_str(),answ.size());   
	    }
	}
    }
    else
    {
	do
	{
	    r_len = read(s_in.cl_sock,buf,buf_len*1000);
	    Mess->put("TRANSPORT",TMess::Info,s_in.s_in->owner().I18N("The socket <%s> receive of message <%d> from <%s>!"),
		s_in.s_in->name().c_str(), r_len, s_in.sender.c_str() );
	    if(r_len > 0) 
	    {
		req.assign(buf,r_len);

		PutMess(s_in.cl_sock,req,answ,s_in.sender,prot_in);
		if( answ.size() && prot_in.freeStat()  ) 
		{
		    Mess->put("TRANSPORT",TMess::Info,s_in.s_in->owner().I18N("The socket <%s> reply of message <%d> to <%s>!"),
			s_in.s_in->name().c_str(), answ.size(), s_in.sender.c_str() );
		    r_len = write(s_in.cl_sock,answ.c_str(),answ.size());   
		}		
	    }
	    else break;
	}
	while( !prot_in.freeStat() );
    }
}

void TSocketIn::PutMess( int sock, string &request, string &answer, string sender, AutoHD<TProtocolIn> &prot_in )
{
    try
    {
	AutoHD<TProtocol> proto = owner().owner().owner().protocol().gmdAt(m_prot);
	string n_pr = name()+TSYS::int2str(sock);
    
        if( prot_in.freeStat() ) 
	{
	    if( !proto.at().openStat(n_pr) ) proto.at().open( n_pr );
	    prot_in = proto.at().at( n_pr );
	}
	if( prot_in.at().mess(request,answer,sender) ) return;
	prot_in.free();
	if( proto.at().openStat(n_pr) ) proto.at().close( n_pr );
    
	prot_in.free();
	if( proto.at().openStat(n_pr) ) proto.at().close( n_pr );
    }catch(TError err){ owner().mPut("SYS",TMess::Error,"%s:%s!",err.what().c_str() ); }
}

void TSocketIn::RegClient(pid_t pid, int i_sock)
{
    ResAlloc res(sock_res,true);
    //find already registry
    for( unsigned i_id = 0; i_id < cl_id.size(); i_id++)
	if( cl_id[i_id].cl_pid == pid ) return;
    SSockCl scl = { pid, i_sock };
    cl_id.push_back(scl);
    cl_free = false;
}

void TSocketIn::UnregClient(pid_t pid)
{
    ResAlloc res(sock_res,true);
    for( unsigned i_id = 0; i_id < cl_id.size(); i_id++ ) 
	if( cl_id[i_id].cl_pid == pid ) 
	{
	    shutdown(cl_id[i_id].cl_sock,SHUT_RDWR);
	    close(cl_id[i_id].cl_sock);
	    cl_id.erase(cl_id.begin() + i_id);
	    if( !cl_id.size() ) cl_free = true;
	    break;
	}
}

//==============================================================================
//== TSocketOut ================================================================
//==============================================================================

TSocketOut::TSocketOut(string name, TTipTransport *owner) : TTransportOut(name,owner), sock_fd(-1)
{
    
}

TSocketOut::~TSocketOut()
{
    try{ stop(); }catch(...){ }    
}

void TSocketOut::start()
{
    if( run_st ) throw TError("(%s) Input transport <%s> started!",MOD_ID,name().c_str());

    string s_type = TSYS::strSepParse(m_addr,0,':');
    
    if( s_type == S_NM_TCP ) 		type = SOCK_TCP;
    else if( s_type == S_NM_UDP ) 	type = SOCK_UDP;
    else if( s_type == S_NM_UNIX )	type = SOCK_UNIX;
    else throw TError("%s: type socket <%s> error!",MOD_ID,s_type.c_str());

    if( type == SOCK_TCP || type == SOCK_UDP )
    {
	memset(&name_in,0,sizeof(name_in));
	name_in.sin_family = AF_INET;
        
	string host = TSYS::strSepParse(m_addr,1,':');
        string port = TSYS::strSepParse(m_addr,2,':');
	if( !host.size() )
	{
   	    struct hostent *loc_host_nm = gethostbyname(host.c_str());
	    if(loc_host_nm == NULL || loc_host_nm->h_length == 0)
		throw TError("%s: socket name <%s> error!",MOD_ID,host.c_str());
	    name_in.sin_addr.s_addr = *( (int *) (loc_host_nm->h_addr_list[0]) );
	}
	else name_in.sin_addr.s_addr = INADDR_ANY;  
	//Get system port for "oscada" /etc/services
	struct servent *sptr = getservbyname(port.c_str(),(type == SOCK_TCP)?"tcp":"udp");
	if( sptr != NULL )                       name_in.sin_port = sptr->s_port;
	else if( htons(atol(port.c_str())) > 0 ) name_in.sin_port = htons( atol(port.c_str()) );
	else name_in.sin_port = 10001;
    }
    else if( type == SOCK_UNIX )
    {
	string path = TSYS::strSepParse(m_addr,1,':');
	if( !path.size() ) path = "/tmp/oscada";	
	memset(&name_un,0,sizeof(name_un));
	name_un.sun_family = AF_UNIX;
	strncpy( name_un.sun_path,path.c_str(),sizeof(name_un.sun_path) );
    }    
    run_st = true;
}

void TSocketOut::stop()
{
    if( !run_st ) throw TError("(%s) Output transport <%s> stoped!",MOD_ID,name().c_str());
    
    if( sock_fd >= 0 )
    {
	shutdown(sock_fd,SHUT_RDWR);	
	close(sock_fd);     
    }
    run_st = false;
}

int TSocketOut::messIO(char *obuf, int len_ob, char *ibuf, int len_ib, int time )
{
    int rez;
    if( !run_st ) throw TError("(%s) transport <%s> no started!",MOD_ID,m_name.c_str());
    if( obuf != NULL && len_ob > 0)
    {
    	if( type == SOCK_TCP  )   
	{
	    if( sock_fd >= 0 ) close(sock_fd);     
    	    if( (sock_fd = socket(PF_INET,SOCK_STREAM,0) )== -1 ) 
	        throw TError("%s: error create %s socket!",MOD_ID,"TCP");
	    if( ::connect(sock_fd, (sockaddr *)&name_in, sizeof(name_in)) == -1 )
		throw TError(owner().I18N("%s: %s connect to %s error!"),MOD_ID,name().c_str(),"TCP");
	    write(sock_fd,obuf,len_ob);
	}
	if( type == SOCK_UNIX )
	{
	    if( sock_fd >= 0 ) close(sock_fd);     
	    if( (sock_fd = socket(PF_UNIX,SOCK_STREAM,0) )== -1) 
		throw TError("%s: error create %s socket!",MOD_ID,"UNIX");
	    if( ::connect(sock_fd, (sockaddr *)&name_un, sizeof(name_un)) == -1 )
		throw TError(owner().I18N("%s: %s connect to UNIX error!"),MOD_ID,name().c_str());
	    write(sock_fd,obuf,len_ob);
	}
	if( type == SOCK_UDP )
	{
	    if( sock_fd >= 0 ) close(sock_fd);     
	    if( (sock_fd = socket(PF_INET,SOCK_DGRAM,0) )== -1 ) 
		throw TError("%s: error create %s socket!",MOD_ID,"UDP");
	    if( ::connect(sock_fd, (sockaddr *)&name_in, sizeof(name_in)) == -1 )
		throw TError(owner().I18N("%s: %s connect to UDP error!"),MOD_ID,name().c_str());
	    write(sock_fd,obuf,len_ob);
	}
    }

    int i_b = 0;
    if( ibuf != NULL && len_ib > 0 && time > 0 )
    {
	fd_set rd_fd;
    	struct timeval tv;

	tv.tv_sec  = time;
	tv.tv_usec = 0;
	FD_ZERO(&rd_fd);
	FD_SET(sock_fd,&rd_fd);
	int kz = select(sock_fd+1,&rd_fd,NULL,NULL,&tv);
	if( kz < 0) throw TError("%s: socket error!",MOD_ID);
	if( kz == 0 || (kz == -1 && errno == EINTR) ) i_b = 0;
	else if( FD_ISSET(sock_fd, &rd_fd) ) i_b = read(sock_fd,ibuf,len_ib);
	else throw TError("%s: timeout error!",MOD_ID);
    }

    return(i_b);
}

