#ifndef SOCKET_H
#define SOCKET_H

#include <pthread.h>

#include "../../ttransports.h"

#define S_NM_TCP  "TCP"
#define S_NM_UDP  "UDP"
#define S_NM_UNIX "UNIX"

#define SOCK_TCP  0
#define SOCK_UDP  1
#define SOCK_UNIX 2

namespace Sockets
{
    class TSocketIn;

    struct SSockIn 
    {
	TSocketIn *s_in;
	int       cl_sock;
    };

    struct SSockCl
    {
	pid_t cl_pid;    // Client's pids
	int   cl_sock;
    };

    class TSocketIn: public TTransportIn
    {
	public:
	    /*
	     * Open input socket <name> for locale <address>
	     * address : <type:<specific>>
	     * 	type: 
	     * 	  TCP  - TCP socket with  "UDP:<host>:<port>"
	     * 	  UDP  - UDP socket with  "TCP:<host>:<port>"
	     * 	  UNIX - UNIX socket with "UNIX:<path>"
	     */
	    TSocketIn(string name, string address, string prot, TTipTransport *owner);
	    ~TSocketIn();

	    void SetParams(int m_queue, int m_fork, int b_len)
	    { max_queue = m_queue; max_fork = m_fork; buf_len = b_len; }
	public:
	    
	private:
	    static void *Task(void *);
	    static void sighd( int signal );
	    static void *ClTask(void *);
	
	    void ClSock(int sock);
	    void PutMess(string &request, string &answer );

	    void RegClient(pid_t pid, int i_sock);
	    void UnregClient(pid_t pid);
	private:
	    pthread_t pthr_tsk;
	    int       sock_fd;
	    unsigned  sock_res;
	
	    bool      run_st;      // Stat of task
	    bool      endrun;      // Command for stop task	    
	    bool      endrun_cl;   // Command for stop client tasks
	    //params
	    int       max_queue;   // max queue for TCP, UNIX sockets
	    int       max_fork;    // maximum forking (opened sockets)
	    int       buf_len;     // input buffer length	
    
    	    int       type;        // socket's types 
    	    string    path;        // path to file socket for UNIX socket
    	    string    host;        // host for TCP/UDP sockets
    	    string    port;        // port for TCP/UDP sockets
    	    int       mode;        // mode for TCP/UNIX sockets (0 - no hand; 1 - hand connect)

    	    int       prot_id;
    	    int       cnt_tst;
    	    vector<SSockCl> cl_id;    // Client's pids
    };

    class TSocketOut: public TTransportOut
    {
	public:
	    /*
	     * Open output socket <name> for locale <address>
	     * address : <type:<specific>>
	     * 	type: 
	     * 	  TCP  - TCP socket with  "UDP:<host>:<port>"
	     * 	  UDP  - UDP socket with  "TCP:<host>:<port>"
	     * 	  UNIX - UNIX socket with "UNIX:<path>"
	     */
	    TSocketOut(string name, string address);
	    ~TSocketOut();

	    int IOMess(char *obuf, int len_ob, char *ibuf = NULL, int len_ib = 0, int time = 0 );

	private:
	    int       sock_fd;
    
    	    int       type;        // socket's types 
    	    struct sockaddr_in  name_in;
    	    struct sockaddr_un  name_un;
    };

    class TTransSock: public TTipTransport
    {
	public:
	    TTransSock(char *name);
	    ~TTransSock();
	    
	    TTransportIn  *In(string name, string address, string prot );
	    TTransportOut *Out(string name, string address );	    
	
	    void mod_CheckCommandLine( );
	    void mod_UpdateOpt();	
	public:
    
	private:
	
	    void pr_opt_descr( FILE * stream );
	private:
	    //params
	    int       max_queue;   // max queue for TCP, UNIX sockets
	    int       max_fork;    // maximum forking (opened sockets)
	    int       buf_len;     // input buffer length	
    };
}

#endif //SOCKET_H

