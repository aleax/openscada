
#ifndef TCONFIG_H
#define TCONFIG_H

#include <vector>
using std::vector;
#include <string>
using std::string;

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

    int     ElDep;     // Number element of depende <0-n> ( -1 - nodepend ) 
    string  val_dep;   // Value of depende (string or numberic) "1","1500","ok"

    SRecStr  *rstr;     // Views, params of elemente for CFGTP_STRING
    SRecNumb *rnumb;    // Views, params of elemente for CFGTP_NUMBER
    SRecSel  *rsel;     // Views, params of elemente for CFGTP_SELECT
};

struct SVal
{
    string *sval;      // String value
    double *nval;      // Number value
    string *slval;     // Select value    	
};


class TConfig
{
/** Public methods: */
public:
    TConfig();
    ~TConfig();
    
//    int AddElem(int id, SElem *elem);
//    int DelElem(int id);
//    int NElem();

/**Attributes: */
public:

/** Public methods: */
private:

/**Attributes: */
private:
    vector< SElem > elem;
    vector< vector< SVal > > value;
};

#endif // TCONFIG_H
