#ifndef TSYS_H
#define TSYS_H

#define TO_FREE         NULL // Object free
#define STR_BUF_LEN     2000 // Len of string buffers (no string class)
#define STD_WAIT_DELAY  100 // Standart wait dalay (ms)

#define C_INT_DEC 0
#define C_INT_OCT 1
#define C_INT_HEX 2

#include <semaphore.h>
#include <stdio.h>

#include <string>
#include <vector>

#include "xml.h"
#include "thd.h"
#include "tcontr.h"

using std::string;
using std::vector;

class TKernel;

//======================================================================================
//====================== TSYS ==========================================================
//======================================================================================

struct SSem
{
    bool  use;          // using flag
    bool  del;          // deleting flag    
    sem_t sem;          // semafor id 
    int   rd_c;         // readers counter
};

class TSYS : public TContr 
{
    // Public methods:
    public:
	TSYS( int argi, char ** argb, char **env );
	~TSYS(  );

	int Start(  );	
        //========= System function ====================
	// Semaphores/Resources
	static unsigned ResCreate( unsigned val = 1 );
	static void ResDelete( unsigned res );
    
	static void WResRequest( unsigned res, long tm = 0 ); // Write request
        static void WResRelease( unsigned res );              // Write release
	static void RResRequest( unsigned res, long tm = 0 ); // Read request
	static void RResRelease( unsigned res );              // Read release
    	// Convert path to absolut name
	string FixFName( const string &fname ) const;    
	// Convert value to string
        static string int2str( int val, char view = C_INT_DEC );
        static string real2str( double val );	
	
	// Config file's functions
        XMLNode *XMLCfgNode();
	
        // Programms options
	string UserName() { return(User); }               //Run user name 
	unsigned cr_file_perm( ) { return(m_cr_f_perm); } //Permision for created files ( default 0644 )
	unsigned cr_dir_perm( ) { return(m_cr_d_perm); }  //Permision for created files ( default 0755 )
	bool event_wait( bool &m_mess_r_stat, bool exempl, string loc, time_t time = 0 );
	
	// Get option from generic config file and update data from XML config.
	void UpdateOpt();
	// Update comand line option
	void CheckCommandLine( );
	// Print comand line options!
	string opt_descr( );
	//void pr_opt_descr( FILE * stream );
	// Set task title
	void SetTaskTitle(const char *fmt, ...);
	string CfgFile() { return(Conf_File); }
	string Station() { return(m_station); }
	
        //================== Kernel functions ========================
        void kern_list( vector<string> &list )
	{ m_kern.obj_list( list ); }
	void kern_add( string name );
	void kern_del( string name );
	unsigned kern_att( string name )
	{ return( m_kern.hd_att( name ) ); }
	void kern_det( unsigned hd )
	{ m_kern.hd_det( hd ); }
	TKernel &kern_at( unsigned hd )
	{ return( *(TKernel *)m_kern.hd_at( hd ) ); }
	
	static void sighandler( int signal );
    public:
	// A comand line seting counter.
	const int argc;
	// A comand line seting buffer.
	const char **argv;
	// A system environment.
	const char **envp;							     

    private:
	void ScanCfgFile( bool first = false );
        //================== Controll functions ========================
	void     ctr_fill_info( XMLNode *inf );
	void     ctr_din_get_( string a_path, XMLNode *opt );
	void     ctr_din_set_( string a_path, XMLNode *opt );
	void     ctr_cmd_go_( string a_path, XMLNode *fld, XMLNode *rez );
	unsigned ctr_att( string br ) { return( kern_att( ctr_path_l(br,2) ) ); }    
	void     ctr_det( string br, unsigned hd ) { kern_det( hd ); }
	TContr  &ctr_at( string br, unsigned hd )  { return( (TContr&)kern_at(hd) ); }
    /** Private atributes: */
    private:
    	// A owner user name!
	string User;
	string Conf_File;
	string m_station;
	unsigned m_cr_f_perm;
	unsigned m_cr_d_perm;
	/** Semaphores/Resources **/
	static vector<SSem>  sems;
	//OpenScada and station XML config node
	XMLNode root_n;
	XMLNode *stat_n;
	
	int    stop_signal;

	THD               m_kern;  // List kernels		

	static const char *o_name;    
	static const char *i_cntr;
};

extern TSYS *SYS;

#endif // TSYS_H
