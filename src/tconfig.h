    
#ifndef TCONFIG_H
#define TCONFIG_H

#include <string>
using std::string;
#include <vector>
using std::vector;

//---- Type ----
#define CFGTP_STRING 0
#define CFGTP_NUMBER 1
#define CFGTP_SELECT 2

//External structures


//Internal structures
struct SRecStr
{
    string def;        // default value
};

struct SRecNumb
{
    double    min;        // min value
    double    max;        // max value
    double    def;        // default value
    int       depth;      // Depth Row into BD for real value
    int       view;       // 0 - dec, 1 - oct, 2 - hex, 3 - real
};

struct SRecSel
{
    string def;          // default value
    string name_varnt;   // Names of variant whith separator ';' ("Manual input;Automatic input;Test")
    string val_varnt;    // Values of variant whith separator ';' ("0;auto;1;")
};

struct SElem
{
    string  name;      // Name of element (name row into BD)
    string  descript;  // Description of element
    char    type;      // Type of element (type row into BD) [CFGTP_STRING ('C'), CFGTP_NUMBER ('N'), CFGTP_SELECT ('C')]

    int     len;       // Len Row into BD

    string  ElDep;     // Name element of depende ( "" - nodependens ) 
    string  val_dep;   // Value of depende (string or numberic) "1","1500","ok"

    SRecStr  *rstr;     // Views, params of elemente for CFGTP_STRING
    SRecNumb *rnumb;    // Views, params of elemente for CFGTP_NUMBER
    SRecSel  *rsel;     // Views, params of elemente for CFGTP_SELECT
};

struct SVal
{
    string *sval;      // String and select value
    double *nval;      // Number value
};


class TConfigElem
{
/** Public methods: */
public:

    TConfigElem();
    ~TConfigElem();

    friend class TConfig;
    /*
     * Add Element to position <id> and return realy position
     */
    int Add(unsigned int id, SElem *elem);
    /* 
     * Delete element, free cell and route all elementes
     */
    int Del(unsigned int id);
    /*
     * Get element's numbers
     */
    int Load( SElem *elements, int numb );
    int Size();
    int NameToId(string name);
/**Attributes: */
private:
    vector< SElem > elem;
    vector< TConfig *> config;
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
    int LoadValBD( string bd);
    /*
     * Save all internal value into BD <bd> whith free <bd>
     */
    int SaveValBD( string bd);
/**Attributes: */
public:

/** Public methods: */
private:

/**Attributes: */
private:
    vector< vector< SVal > > value;
    TConfigElem *elem;
};

#endif // TCONFIG_H
