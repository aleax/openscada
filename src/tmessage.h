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
    string mess;
};

class TMessage
{
    /** Public methods: */
    public:
	TMessage(  );

	~TMessage(  );

	int Sconv(const char *fromCH, const char *toCH, string & buf);
	int SconvIn(const char *fromCH, string & buf);
	int SconvOut(const char *toCH, string & buf);
	void SetCharset(string charset) { IOCharSet = charset; }
	void SetDLevel(int level)       { d_level   = level; }
	void SetLogDir(int dir)         { log_dir   = dir; }
	/*
	 * Put message for category "SYS" and <level>
	 */	
	//void put( int level, char *fmt,  ... );
	/*
	 * Put message for <categ> and <level> 
	 */	
	void put( string categ, int level, char *fmt,  ... );
        void GetMess( time_t b_tm, time_t e_tm, vector<SBufRec> & recs );
    
	void UpdateOpt();
	/*
	 * Update comand line option
	 */
	void CheckCommandLine( );
	/*
	 * Print comand line options!
	 */
	void pr_opt_descr( FILE * stream );    
    
    /**Attributes: */
    private:
	string IOCharSet;      //Internal charset
	int    d_level;        //Debug level
	int    log_dir;        //Log direction
	
	int    head_buf;       //Head buffer
	vector<SBufRec> m_buf; //Messages buffer
};

extern TMessage *Mess;

#endif // TMESSAGE_H
