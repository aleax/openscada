#ifndef BASE_H
#define BASE_H

#include "../../tmodule.h"
#include "../../xml.h"
#include "../../tarhives.h"

namespace BaseArh
{
    class TFileArh
    {
	public:
 	    TFileArh();
 	    TFileArh( string name, time_t beg, time_t end, string charset, int time_size );
 	    ~TFileArh();

	    void Attach( string name );
	    void put( SBufRec mess );
	    void Sync( );                            //Write changes to arhive file

	    string &Name() { return(m_name); }
	    time_t &Begin(){ return(m_beg); }
	    time_t &End()  { return(m_end); }
	    bool   &Err()  { return(m_err); }
	    
	public:
    	    bool    scan;    // arhive scaned (for check deleted files)
	private:	    
    	    string  m_name;    // name arhive file;
    	    string  m_chars;   // arhive charset;
    	    bool    m_err;     // arhive err
    	    bool    m_write;   // arhive had changed but no writed to file
	    bool    m_load;    // arhiv load to m_node
    	    time_t  m_beg;     // begin arhive file;
    	    time_t  m_end;     // end arhive file;
    	    XMLNode m_node;    // XMLNode = !NULL if opened 
    	    int     m_res;     // resource to access;	
    };

    class TMessArh: public TArhiveMess
    {
	public:
	    TMessArh( string name, string addr, string categoris, TTipArhive *owner );
	    ~TMessArh( );

	    void put( vector<SBufRec> &mess );
	    void GetMess( time_t b_tm, time_t e_tm, vector<SBufRec> &mess, string category = "", char level = 0 );
	    string GetCodePage( ); 
	    void SetCodePage( string codepage );
	private:	
	    void ScanDir();

	    static void *Task(void *param);	
	private:	
    	    int       m_res;     // resource to access;	
            bool      m_stat;    // pthread stat;
            bool      m_endrun;  // pthread end run command;	    
	    pthread_t m_pthr;
	    
	    vector<TFileArh *>  arh_s;	    
    };
    
    class TMArhive: public TTipArhive
    {
	friend class TMessArh;
	public:
	    TMArhive(char *name);
	    ~TMArhive();

	    void mod_CheckCommandLine( );
	    void mod_UpdateOpt();
	public:
    
	private:
	    void mod_connect(  );

	    TArhiveMess *AMess(string name, string addr, string categories );
	    
	    void pr_opt_descr( FILE * stream );
	private:
	    string m_mess_charset;   // default message charset
	    int    m_mess_max_size;  // maximum size kb of arhives file
	    int    m_mess_numb_file; // number of arhive files
	    int    m_mess_time_size; // number days to one file
	
	    static SExpFunc ExpFuncLc[];
    };
}

#endif //BASE_H

