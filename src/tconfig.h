    
#ifndef TCONFIG_H
#define TCONFIG_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tconfigelem.h"

union _EVal
{
    string *s_val;
    double r_val;
    int    i_val;
    bool   b_val;
};

class TTable;

class TConfig
{
    /** Public methods: */
    public:
	TConfig( TConfigElem *Elements );
	~TConfig();

	friend class TConfigElem;
    
	string cf_Get_SEL( string n_val, unsigned int id = 0 );
	string cf_Get_S( string n_val, unsigned int id = 0 );
	double cf_Get_R( string n_val, unsigned int id = 0 );
	int    cf_Get_I( string n_val, unsigned int id = 0 );
	bool   cf_Get_B( string n_val, unsigned int id = 0 );

	void cf_Set_SEL( string n_val, string val, unsigned int id = 0 );
	void cf_Set_S( string n_val, string val, unsigned int id = 0);
	void cf_Set_R( string n_val, double val, unsigned int id = 0);
	void cf_Set_I( string n_val, int val, unsigned int id = 0);
	void cf_Set_B( string n_val, bool val, unsigned int id = 0);

	unsigned cf_Size(){ return(value.size()); }
	/*
	 * Init record <id_rec>. 
	 */
	int cf_InitRecord( unsigned int id);
	/*
	 * Add record <id_rec>. 
	 */
	int cf_AddRecord( unsigned int id);
	/*
	 * Free record <id_rec> whith rotated other record. 
	 */
	void cf_FreeRecord( unsigned int id);
	/*
	 * Free dublicated record
	 *   n_val - field for search dubl;
	 *   mode  - mode search (false - begin; true - end)
	 */
	void cf_FreeDubl( string n_val, bool mode );
	/*
	 * Load value for record <id_rec> from BD <bd>. 
	 */    
	void cf_LoadValBD( string NameFld, TTable &table, unsigned int id_rec=0 );
	void cf_LoadValBD( int line_bd, TTable &table, unsigned int id_rec=0 );
	/*
	 * Save value for record <id_rec> to BD <bd>. 
	 * If BD absent then create new BD into default BD type.
	 * If field absent into BD then it created;
	 * If field no use then no change.
	 */
	void cf_SaveValBD( string NameFld, TTable &table, unsigned int id_rec=0 );
	void cf_SaveValBD( int line_bd, TTable &table, unsigned int id_rec=0 );
	/*
	 * Load all value from BD <bd> into whith add internal value
	 *   table    - poiner to bd table;
	 */
	void cf_LoadAllValBD( TTable &table );
	/*
	 * Save all internal value into BD <bd> whith free <bd>
	 */
	int cf_SaveAllValBD( TTable &table );
	/*
	 * Equalited congigs
	 */
	TConfig & operator=(TConfig & Cfg);

	void cf_ConfElem(TConfigElem *Elements); 
	TConfigElem *cf_ConfElem();

	void cf_ListEl( vector<string> &list, unsigned int id = 0 );

    /** Public methods: */
    private:
	/*
	 * Add elem into TValueElem
	 */
	int cf_AddElem(int id);
	/*
	 * Del elem without TValueElem
	 */
	int cf_DelElem(int id);
	/*
	 * Check viewed element
	 */
	bool cf_ViewEl( unsigned id_el, unsigned id = 0 );
	
    /**Attributes: */
    private:
	vector< vector< _EVal > > value;
	TConfigElem               *elem;
	static const char         *o_name;
};

#endif // TCONFIG_H
