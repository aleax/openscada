    
#ifndef TCONFIGELEM_H
#define TCONFIGELEM_H

#include <string>
using std::string;
#include <vector>
using std::vector;
//---- New Type ----
#define CFG_T_STRING  1
#define CFG_T_INT     2
#define CFG_T_REAL    4
#define CFG_T_BOOLEAN 8

#define CFG_T_SELECT  16 //multiple with elementeres elements


struct SCfgFld
{
    string   name;       // Name of element (name column into BD);
    string   descript;   // Description of element;
    char     type;       // Type of element (CFG_T_STRING, CFG_T_INT, CFG_T_REAL, CFG_T_SELECT, CFG_T_SELECT|CFG_T_STRING);
    string   ElDep;      // Name element of depende ( "" - nodependens );
    string   val_dep;    // Value of depende (name select value) "PID","1500","ok";
    string   def;        // default value;
    string   len;        // len field (for string element and other element: 4, 4.2, 3.5) 
    string   vals;       // values ("0;100" - min = 0, max = 100 if no select) ("0;2;5" - enumerate if select) ("23" - maximum string len); //?!?!
    string   view;       // view mask("%d","%4.3f","%x","%s");
    string   n_sel;      // selectable element's name
};

//Internal structures
struct SCfgElem
{
    string          name;      	// Name of element (name column into BD);
    string          descript;  	// Description of element;
    char            type;       // Type of element (CFG_T_STRING, CFG_T_INT, CFG_T_REAL, CFG_T_SELECT, CFG_T_SELECT|CFG_T_STRING);
    string          ElDep;     	// Name element of depende ( "" - nodependens );
    string          val_dep;   	// Value of depende (name select value) "PID","1500","ok";
    string          def;       	// default value;
    string          len;        // len field (for string element and other element: 4, 4.2, 3.5) 
    vector<string>  vals;      	// values ("0;100" - min = 0, max = 100 if no select) ("0;2;5" - enumerate if select) ("23" - maximum string len);
    string          view;      	// view mask("%d","%4.3f","%x","%s");
    vector<string>  n_sel;      // selectable element's name
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
    int Add( unsigned int id, SCfgFld *element );
    int Add( SCfgFld *element ){ return(Add(Size(),element)); }
    /* 
     * Delete element, free cell and route all elementes
     */
    void Del(unsigned int id);
    /*
     * Get element's numbers
     */
    void Load( SCfgFld *elements, int numb );
    int Size(){ return(elem.size()); }
    unsigned int NameToId(string name);
    /*
     * Update attributes BD (resize, change type of columns ....
     */
    void UpdateBDAtr( string bd );
    void UpdateBDAtr( int hd_bd );
/**Attributes: */
private:
    vector< SCfgElem > elem;
    vector< TConfig *> config;
    static const char  *o_name;
};

#endif // TCONFIGELEM_H
