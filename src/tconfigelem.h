    
#ifndef TCONFIGELEM_H
#define TCONFIGELEM_H

#include <string>
using std::string;
#include <vector>
using std::vector;

//---- Type ----
#define CFGTP_STRING 0
#define CFGTP_NUMBER 1
#define CFGTP_SELECT 2

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

class TConfig;

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
    int Size(){ return(elem.size()); }
    int NameToId(string name);
    /*
     * Update attributes BD (resize, change type of rows ....
     */
    int UpdateBDAtr( string bd );
    int UpdateBDAtr( int hd_bd );
/**Attributes: */
private:
    vector< SElem > elem;
    vector< TConfig *> config;
};

#endif // TCONFIGELEM_H
