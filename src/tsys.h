#ifndef TSYS_H
#define TSYS_H

#define TO_FREE         NULL // Object free

#include <semaphore.h>
#include <stdio.h>

#include <string>
using std::string;
#include <vector>
using std::vector;

class TSYS
{
    /** Public methods: */
    public:
	TSYS( int argi, char ** argb, char **env );
	~TSYS(  );
    
	/** Semaphores/Resources **/
	unsigned ResCreate( unsigned val = 1 );
	void ResDelete( unsigned res );
    
	void ResRequest( unsigned res, long tm = 0 );
        void ResRelease( unsigned res );
	/** Config file's functions **/
	bool GetOpt(string section, string opt, string &value, unsigned entry = 1, bool excep = false);
        /** Programms options **/
	string UserName() { return(User); }
	/*
 	 * Get optio from generic config file.
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
	
    /** Private atributes: */
    private:
    	/*
         * A owner user name!
 	 */
	string User;
	string Conf_File;
	/** Semaphores/Resources **/
	vector<sem_t *>  sems;

	static const char *o_name;    
	static const char *n_opt;
};

extern TSYS *SYS;

#endif // TSYS_H
