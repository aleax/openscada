    
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

class TConfig
{
/** Public methods: */
public:
    TConfig( TConfigElem *Elements );
    ~TConfig();

    friend class TConfigElem;
    
    string Get_SEL( string n_val, unsigned int id = 0 );
    string Get_S( string n_val, unsigned int id = 0 );
    double Get_R( string n_val, unsigned int id = 0 );
    int    Get_I( string n_val, unsigned int id = 0 );
    bool   Get_B( string n_val, unsigned int id = 0 );

    void Set_SEL( string n_val, string val, unsigned int id = 0 );
    void Set_S( string n_val, string val, unsigned int id = 0);
    void Set_R( string n_val, double val, unsigned int id = 0);
    void Set_I( string n_val, int val, unsigned int id = 0);
    void Set_B( string n_val, bool val, unsigned int id = 0);

    int Size(){ return(value.size()); }
    /*
     * Init record <id_rec>. 
     */
    int InitRecord( unsigned int id);
    /*
     * Add record <id_rec>. 
     */
    int AddRecord( unsigned int id);
    /*
     * Free record <id_rec> whith rotated other record. 
     */
    void FreeRecord( unsigned int id);
    /*
     * Load value for record <id_rec> from BD <bd>. 
     */
    void LoadValBD( string NameFld, string t_bd, string n_bd, string n_tb, unsigned int id_rec=0 );
    void LoadValBD( string NameFld, unsigned int hd_bd, unsigned int id_rec=0 );
    void LoadValBD( int line_bd, unsigned int hd_bd, unsigned int id_rec=0 );    
    /*
     * Save value for record <id_rec> to BD <bd>. 
     * If BD absent then create new BD into default BD type.
     * If field absent into BD then it created;
     * If field no use then no change.
     */
    void SaveValBD( string NameFld, string t_bd, string n_bd, string n_tb, unsigned int id_rec=0);
    void SaveValBD( string NameFld, unsigned int hd_bd, unsigned int id_rec=0);
    void SaveValBD( int line_bd, unsigned int hd_bd, unsigned int id_rec=0);
    /*
     * Load all value from BD <bd> into whith add internal value
     */
    void LoadAllValBD( string t_bd, string n_bd, string n_tb );
    /*
     * Save all internal value into BD <bd> whith free <bd>
     */
    int SaveAllValBD( string t_bd, string n_bd, string n_tb);
    /*
     * Equalited congigs
     */
    TConfig & operator=(TConfig & Cfg);

    TConfigElem *ConfElem() const { return(elem); }

    void ListEl( vector<string> &list, unsigned int id = 0 );
/**Attributes: */
public:

/** Public methods: */
private:
    /*
     * Add elem into TValueElem
     */
    int AddElem(int id);
    /*
     * Del elem without TValueElem
     */
    int DelElem(int id);

    bool ViewEl( unsigned id_el, unsigned id = 0 );
/**Attributes: */
private:
    vector< vector< _EVal > > value;
    TConfigElem               *elem;
    static const char         *o_name;
};

#endif // TCONFIG_H
