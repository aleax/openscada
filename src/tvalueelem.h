#ifndef TVALUEELEM_H
#define TVALUEELEM_H

#include <string>
using std::string;
#include <vector>
using std::vector;

//==== Value types (mode of store) ====
#define VAL_T_REAL  0   //float
#define VAL_T_BOOL  1   //boolean
#define VAL_T_INT   2   //int
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

struct SVAL
{
    string name;      //Name cell
    string lname;     //Long name cell
    string descr;     //Description cell
    int    type:2;    //VAL_T_REAL, VAL_T_BOOL, VAL_T_INT 
    int    stat:2;    //VAL_S_GENER, VAL_S_UTIL, VAL_S_SYS
    int    mode:2;    //VAL_M_OFTN, VAL_M_SELD, VAL_M_CNST
    int    data:2;    //VAL_D_FIX, VAL_D_BD, VAL_D_VBD
    int    access:10; //Access to cell (0444)
    float  min;       //Minimum numberic val
    float  max;       //Maximum nemberic val
};

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
    int Add(int id_val, SVAL *block);
    /*
     * Delete value from a param buffer
     */
    int Del(int id_val);
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
    vector<SVAL>      elem;
/** Protected atributes: */
protected:
    vector< TValue *> value;
};

#endif // TVALUEELEM_H

