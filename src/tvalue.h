#ifndef TVALUE_H
#define TVALUE_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tvalueelem.h"

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
#define VAL_D_BD    2   //date from bd
#define VAL_D_VBD   4   //value from bd
//==== Value mode (static, dinamic) ==== 
#define VAL_M_OFTN  0   //often  change (must be arhived)
#define VAL_M_SELD  1   //seldom change (may be arhived)
#define VAL_M_CNST  2   //no change     (no arhived)

//==== Value mode access 
#define VAL_MA_DIRECT 0 //Direct access to controller
#define VAL_MA_OFTN   1 //Periodic writen fast value elements (require internal buffer)
#define VAL_MA_SELD   2 //Periodic writen all changes value elements (require internal buffer)

union SValue
{
    float val_r;     //real value
    bool  val_b;     //boolean value
    int   val_i;     //integer value
};

class TValue
{
/** Public methods: */
public:
    friend class TValueElem;
    TValue( );
    ~TValue();

    /*
     * Object of element for value
     */
    TValueElem *Elem() {return(elem);}
    /*
     * Size buffer
     */
    int Size() {return(value.size());}
    /*
     * Resize buffer
     */
    int ResizeBuff(unsigned int len);
    /*
     * Write new value(s) into buffer
     */
    int write(double val);
    int write(int val);
    int write(bool val);
    
/** Public atributes: */
public:

/** Private metods */
private:
    /*
     * Add elem into TValueElem
     */
    int AddElem(int id_val); 
    /*
     * Del elem without TValueElem
     */
    int DelElem(int id_val);  
    
/** Private atributes: */
private:
    vector<vector <SValue> > value; // buffer values
    TValueElem               *elem; // elements   
    char                     mode;  // Value mode                       
};

#endif // TVALUE_H

