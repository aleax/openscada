#ifndef TVALUEELEM_H
#define TVALUEELEM_H

#include <string>
using std::string;
#include <vector>
using std::vector;

//==== Value types (mode of store) ====
#define VAL_T_REAL   1   //float
#define VAL_T_BOOL   2   //boolean
#define VAL_T_INT    4   //int
#define VAL_T_STRING 8   //string 

#define VAL_T_SELECT 16  //selectable of VAL_T_REAL,VAL_T_BOOL,VAL_T_INT,VAL_T_STRING
//==== Value source ====
#define VAL_S_LOCAL  1   //local (forming of controllers)
#define VAL_S_BD     2   //from BD (forming of controllers) (name field BD into <vals>)
//==== Values IO mode ==== 
#define VAL_IO_DEF   0   //default mode write and read to internal buffer!
#define VAL_IO_W_DIR 1   //write direct to controller (no create internal buffer)!
#define VAL_IO_R_DIR 2   //read direct from controller (no create internal buffer)!


/*
//==== Value stats (mode displaing) ==== 
#define VAL_S_GENER 0   //display how generic parameter into GUI
#define VAL_S_UTIL  1   //display how utiliti/koefficient into GUI
#define VAL_S_SYS   2   //display how system/hidden into GUI
//==== Value source date (set elem data from bd or fixed) ==== 
#define VAL_D_FIX   1   //fix date 
#define VAL_D_BD    2   //date from bd (field max,min,access loading from BD fields MAX,MIN,ACCESS)
#define VAL_D_VBD   4   //value from bd (load value from bd it field)
//==== Value mode (static, dinamic) ==== 
#define VAL_M_OFTN  0   //often  change (must be arhived)
#define VAL_M_SELD  1   //seldom change (may be arhived)
#define VAL_M_CNST  2   //no change     (no arhived)
*/

struct SVAL
{
    string name;      //Name cell
    string lname;     //Long name cell
    string descr;     //Description cell
    int    type:6;    //VAL_T_REAL, VAL_T_BOOL, VAL_T_INT, VAL_T_STRING | VAL_T_SELECT
    int    source:2;  //VAL_S_LOCAL, VAL_S_BD
    int    io:2;      //VAL_IO_W_DIR, VAL_IO_R_DIR
    string view;      //view to gui (x.y - x - level viewing; y - priority)
    string vals;      //values ("0;100" - min = 0, max = 100 if no select $MIN;$MAX - из полей в БД) ("0;2;5" - enumerate if select)
    string n_sel;     //selectable element's name    
    string access;    //Access to cell (rw-rw-r--, $ACCESS - поле в БД)
    string valid;     //valid source element value or default value (default individual and valid false) ("$VALID", "false","true")
};

struct _SVAL
{
    string         name;      //Name cell
    string         lname;     //Long name cell
    string         descr;     //Description cell
    int            type:6;    //VAL_T_REAL, VAL_T_BOOL, VAL_T_INT, VAL_T_STRING | VAL_T_SELECT
    int            source:2;  //VAL_S_LOCAL, VAL_S_BD
    int            io:2;      //VAL_IO_W_DIR, VAL_IO_R_DIR
    string         view;      //view to gui (x.y - x - level viewing; y - priority) 
    vector<string> vals;      //values ("0;100" - min = 0, max = 100 if no select $MIN;$MAX - из полей в БД) ("0;2;5" - enumerate if select)
    vector<string> n_sel;     //selectable element's name
    string         access;    //Access to cell (rw-rw-r--, $ACCESS - поле в БД)
    int            valid;     //valid id source element
    int            id;        //id elements into buffer (-1 - no element into buffer)    
};

/*
struct SVAL
{
    string name;      //Name cell
    string lname;     //Long name cell
    string descr;     //Description cell
    int    type:6;    //VAL_T_REAL, VAL_T_BOOL, VAL_T_INT, VAL_T_STRING 
    int    stat:2;    //VAL_S_GENER, VAL_S_UTIL, VAL_S_SYS
    int    mode:2;    //VAL_M_OFTN, VAL_M_SELD, VAL_M_CNST
    int    data:2;    //VAL_D_FIX, VAL_D_BD, VAL_D_VBD
    int    access:10; //Access to cell (0444)
    float  min;       //Minimum numberic val
    float  max;       //Maximum nemberic val
};
*/
class TValue;

class TValueElem
{
/** Public methods: */
public:
    friend class TValue;
    
    TValueElem( string name_v );
    ~TValueElem( );
    /*
     * Add value into a param buffer
     */
    void Add(SVAL *block) { Add(Size(),block); return; }
    void Add(unsigned int id_val, SVAL *block);
    /*
     * Delete value from a param buffer
     */
    void Del(unsigned int id_val);
    /*
     * Size value 
     */
    int Size()    { return(elem.size()); }

    string Name() { return(name); }

    void List(vector<string> &List);

/** Public methods: */
public:
/** Private atributes: */
private:
    string            name;
    vector<_SVAL>     elem;

    static const char *o_name;
/** Protected atributes: */
protected:
    vector< TValue *> value;
};

#endif // TVALUEELEM_H

