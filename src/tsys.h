#ifndef TSYS_H
#define TSYS_H

#define TO_FREE         NULL // Object free
#define STR_BUF_LEN     1024 // Len of string buffers (no string class)

#include <semaphore.h>
#include <stdio.h>

#include <string>
#include <vector>

#include "xml.h"

using std::string;
using std::vector;

class TKernel;

struct SSem
{
    bool  use;          // using flag
    bool  del;          // deleting flag    
    sem_t sem;          // semafor id 
    int   rd_c;         // readers counter
};

class TSYS
{
    /** Public methods: */
    public:
	TSYS( int argi, char ** argb, char **env );
	~TSYS(  );

	int Start(  );	
        //========= System function ====================
	/** Semaphores/Resources **/
	unsigned ResCreate( unsigned val = 1 );
	void ResDelete( unsigned res );
    
	void WResRequest( unsigned res, long tm = 0 ); // Write request
        void WResRelease( unsigned res );              // Write release
	void RResRequest( unsigned res, long tm = 0 ); // Read request
	void RResRelease( unsigned res );              // Read release
    	/*
	 * Convert path to absolut name
    	 */
	string FixFName( const string &fname ) const;    
	
	/** Config file's functions **/
        XMLNode *XMLCfgNode();
	
        /** Programms options **/
	string UserName() { return(User); }
	/*
 	 * Get option from generic config file and update data from XML config.
	 */
	void UpdateOpt();
	/*
	 * Update comand line option
	 */
	void CheckCommandLine( );
	/*
	 * Print comand line options!
	*/
	void pr_opt_descr( FILE * stream );
	/* Set task title */
	void SetTaskTitle(const char *fmt, ...);
	string CfgFile() { return(Conf_File); }
	string Station() { return(m_station); }
	
        // Kernel function
	void KernList( vector<string> & list ) const;
	TKernel &KernMake( const string name );
	void KernRemove( const string name );
	TKernel &at( const string name ) const;
	TKernel &operator[]( const string name ) const { return(at(name)); }
	
	static void sighandler( int signal );
    public:
	/*
 	 * A comand line seting counter.
	 */
	const int argc;
	/*
	 * A comand line seting buffer.
	 */
	const char **argv;
	/*
 	 * A system environment.
	 */
	const char **envp;							     

    private:
	void ScanCfgFile( );
    /** Private atributes: */
    private:
    	/*
         * A owner user name!
 	 */
	string User;
	string Conf_File;
	string m_station;
	/** Semaphores/Resources **/
	vector<SSem>  sems;
	//OpenScada and station XML config node
	XMLNode root_n;
	XMLNode *stat_n;
	
	int    stop_signal;

	vector<TKernel *> m_kern;

	static const char *o_name;    
	static const char *n_opt;
};

extern TSYS *SYS;

#endif // TSYS_H
