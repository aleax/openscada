    
#ifndef TCONFIG_H
#define TCONFIG_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tconfigelem.h"

struct _SVal
{
    string *sval;      // String and select value
    double *nval;      // Number value
}; 

class TConfig
{
/** Public methods: */
public:
    TConfig( TConfigElem *Elements );
    ~TConfig();

    friend class TConfigElem;
    
    int GetVal( unsigned int id_ctr, string n_val, string & val);
    int GetVal( unsigned int id_ctr, string n_val, double & val);
    int GetVal( string n_val, string & val);
    int GetVal( string n_val, double & val);

    int SetVal( unsigned int id_ctr, string n_val, string val);
    int SetVal( unsigned int id_ctr, string n_val, double val);
    int SetVal( string n_val, string val);
    int SetVal( string n_val, double val);

    int Size(){ return(value.size()); }
    
    /*
     * Init record <id_rec>. 
     */
    int InitRecord( unsigned int id_rec);
    /*
     * Add record <id_rec>. 
     */
    int AddRecord( unsigned int id_rec);
    /*
     * Free record <id_rec> whith rotated other record. 
     */
    int FreeRecord( unsigned int id_rec);
    /*
     * Load value for record <id_rec> from BD <bd>. 
     */
    int LoadRecValBD(unsigned int id_rec, string NameFld, string bd);
    int LoadRecValBD(unsigned int id_rec, string NameFld, int hd_bd);
    int LoadRecValBD(unsigned int id_rec, int line_bd, int hd_bd);
    int LoadRecValBD(string NameFld, string bd);
    int LoadRecValBD(string NameFld, int hd_bd);
    int LoadRecValBD(int line_bd, int hd_bd);
    /*
     * Save value for record <id_rec> to BD <bd>. 
     * If BD absent then create new BD into default BD type.
     * If field absent into BD then it created;
     * If field no use then no change.
     */
    int SaveRecValBD(unsigned int id_rec, string NameFld, string bd);
    int SaveRecValBD(unsigned int id_rec, string NameFld, int hd_bd);
    int SaveRecValBD(unsigned int id_rec, int line_bd, int hd_bd);
    int SaveRecValBD(string NameFld, string bd);
    int SaveRecValBD(string NameFld, int hd_bd);
    int SaveRecValBD(int line_bd, int hd_bd);
    /*
     * Load all value from BD <bd> into whith add internal value
     */
    int LoadValBD( string bd );
    /*
     * Save all internal value into BD <bd> whith free <bd>
     */
    int SaveValBD( string bd );
    /*
     * Equalited congigs
     */
    TConfig & operator=(TConfig & Cfg);
/**Attributes: */
public:

/** Public methods: */
private:
    string CheckSelect(int id_elem, string val);
/**Attributes: */
private:
    vector< vector< _SVal > > value;
    TConfigElem *elem;
};

#endif // TCONFIG_H
