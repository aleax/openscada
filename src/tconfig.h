    
#ifndef TCONFIG_H
#define TCONFIG_H

#include <string>
#include <vector>

#include "telem.h"

using std::string;
using std::vector;

class TConfig;

class TCfg
{
    public:
	TCfg( TFld &fld, TConfig &owner );
	~TCfg();
	
	const string &name();
	bool  view(){ return(m_view); }
	TFld &fld(){ return(*m_fld); }	
	
        string getSEL( );
	string &getS( );
	double &getR( );
	int    &getI( );
	bool   &getB( );
	
	void setSEL( string val );
	void setS( string val );
	void setR( double val );
	void setI( int val );
	void setB( bool val );

	bool operator==(TCfg & cfg);
	TCfg &operator=(TCfg & cfg);

    private:
        bool  m_view;
	
    	union 
	{
	    string *s_val;
	    double r_val;
	    int    i_val;
	    bool   b_val;
	}m_val;	
	
	TFld     *m_fld;

	TConfig  &m_owner;
	
	static const char       *o_name;
};


class TTable;

class TConfig: public TContElem
{
    /** Public methods: */
    public:
	TConfig( TElem *Elements );
	~TConfig();

	friend class TElem;

	TCfg &cfg( string n_val, unsigned int id = 0 );    

	unsigned cfSize(){ return(value.size()); }
	// Add record <id_rec>. 
	int cf_AddRecord( unsigned int id);
	// Free record <id_rec> whith rotated other record. 
	void cfFreeRecord( unsigned int id);
	/*
	 * Free dublicated record
	 *   n_val - field for search dubl;
	 *   mode  - mode search (false - begin; true - end)
	 */
	void cfFreeDubl( string n_val, bool mode );
	// Load value for record <id_rec> from BD <bd>.     
	void cfLoadValBD( string NameFld, TTable &table, unsigned int id_rec=0 );
	void cfLoadValBD( int line_bd, TTable &table, unsigned int id_rec=0 );
	/*
	 * Save value for record <id_rec> to BD <bd>. 
	 * If BD absent then create new BD into default BD type.
	 * If field absent into BD then it created;
	 * If field no use then no change.
	 */
	void cfSaveValBD( string NameFld, TTable &table, unsigned int id_rec=0 );
	void cfSaveValBD( int line_bd, TTable &table, unsigned int id_rec=0 );
	/*
	 * Load all value from BD <bd> into whith add internal value
	 *   table    - poiner to bd table;
	 */
	void cfLoadAllValBD( TTable &table, bool free = true );
	// Save all internal value into BD <bd> whith free <bd>
	int cfSaveAllValBD( TTable &table );
	// Equalited congigs
	TConfig & operator=(TConfig & Cfg);

	void cfConfElem(TElem *Elements); 
	TElem &cfConfElem();

	void cfListEl( vector<string> &list, unsigned int id = 0 );
	
	virtual bool cfChange( TCfg &cfg ){ return(true); }
    /** Public methods: */
    private:
	// Add elem into TElem
	void addElem( unsigned id );
	// Del elem without TElem
	void delElem( unsigned id );	
    /**Attributes: */
    private:
	vector< vector<TCfg*> > value;
	TElem                   *elem;
        bool                    single;
	
	static const char       *o_name;
};

#endif // TCONFIG_H
