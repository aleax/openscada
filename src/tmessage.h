#ifndef TMESSAGE_H
#define TMESSAGE_H

#include <stdio.h>

#include <string>
#include <vector>
#include <exception>

//Message levels
#define MESS_DEBUG   0
#define MESS_INFO    1
#define MESS_NOTICE  2
#define MESS_WARNING 3
#define MESS_ERR     4
#define MESS_CRIT    5
#define MESS_ALLERT  6
#define MESS_EMERG   7

using std::string;
using std::vector;
using std::exception;

struct SBufRec
{
    time_t time;
    string categ;
    char   level;
    string mess;
};

class TMessage
{
    /** Public methods: */
    public:
	TMessage(  );

	~TMessage(  );

	int Sconv( string fromCH, string toCH, string & buf);
	int SconvIn( string fromCH, string &buf);
	int SconvOut( string toCH, string & buf);
	
	string &charset( )    { return( IOCharSet ); }
	int d_level( )        { return( m_d_level ); }
	int log_direct( )     { return( log_dir ); }
	int mess_buf_len( )   { return( m_buf.size() ); }
	
	void charset( string charset ) { IOCharSet = charset; }
	void d_level(int level)        { m_d_level = level; }
	void log_direct(int dir)       { log_dir   = dir; }
	void mess_buf_len(int len);
	
	void put( string categ, int level, char *fmt,  ... );
        void get( time_t b_tm, time_t e_tm, vector<SBufRec> & recs, string category = "", char level = 0 );
    
	void UpdateOpt();
	// Update comand line option
	void CheckCommandLine( );
	// Print comand line options!
	void pr_opt_descr( FILE * stream );    
    
    /**Attributes: */
    private:
	string IOCharSet;      //Internal charset
	int    m_d_level;      //Debug level
	int    log_dir;        //Log direction
	
	int    m_res;          //Mess resource
	int    head_buf;       //Head buffer
	vector<SBufRec> m_buf; //Messages buffer

	static const char *o_name; 
};

extern TMessage *Mess;

#endif // TMESSAGE_H
