
#ifndef TMODSCHEDUL_H
#define TMODSCHEDUL_H

#include <pthread.h>

#include "tcontr.h"

//For a multi moduls declaration into once a shared lib
struct SUse
{
    int id_tmod;
    int id_mod;
};

struct SHD
{
    void         *hd;         // NULL - share lib avoid but no attached
    vector<SUse> use;         // if share lib attached to show how modules used 
    time_t       m_tm;        // data modify of share lib for automatic update    
    string       name;        // share lib path
};

class TGRPModule;

class TModSchedul : public TContr   
{
    /** Public methods: */
    public:
	TModSchedul( TKernel *app );
    
	~TModSchedul(  );
	// Check command line all TGRPModules 
	void CheckCommandLine(  );
	void CheckCommandLineMod(  );
	// Update options from generic config file
    	void UpdateOpt();
	void UpdateOptMod();
	// Load all share libs and registry moduls into TGRPModule	
    	void LoadAll(  );
	// Load share libs for <dest> from <path> whith call gmd_Init if set <full>
    	void Load( string path, int dest, bool full );

	void InitAll(  );
    
	void DeinitAll(  );

	void StartAll(  );
	
	void StartSched(  );
	// Register group moduls
    	int RegGroupM( TGRPModule *gmod );
	// Unregister group moduls   	 
    	int UnRegGroupM( TGRPModule *gmod );
	// List avoid share libs
    	void ListSO( vector<string> &list );
	// Get stat share lib <name>
    	SHD SO( string name );    
	/*
	 * Attach share libs
	 *   name = SO name;
	 *   dest = direct loading <dest> type modules
	 *   full = after loading will be call gmd_Init;
	 */
	void AttSO( const string &name, bool full = false, int dest = -1);
	// Detach share libs
    	void DetSO( const string &name );

	TKernel &Owner() const { return(*owner); }
    public:
    /** Private methods: */
    private:
	// Scan directory for OpenScada share libs
    	void ScanDir( const string &Paths, vector<string> &files, bool new_f ) const;
	// Check file to OpenScada share libs
    	bool CheckFile( const string &name, bool new_f = false ) const;
	// Registre avoid share lib
    	int  RegSO( const string &name );
	// Check file to auto attaching
    	bool CheckAuto( const string &name) const;    
	// Unreg deleted share lib
	void UnregSO( const string &name );
	//================== Controll functions ========================
	void ctr_fill_info( XMLNode &inf );
	void ctr_opt_apply( XMLNode &opt ); 

    	static void *SchedTask(void *param);    
    private:
	TKernel  		 *owner;
   
	unsigned             hd_res;   
	vector<TGRPModule *> grpmod; 
	vector<SHD *>        SchHD;
	pthread_t            pthr_tsk;
	bool                 m_stat;
	bool                 m_endrun;

	static const char   *i_cntr;
	static const char   *o_name;
};

#endif // TMODSCHEDUL_H

