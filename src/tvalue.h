#ifndef TVALUE_H
#define TVALUE_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "tconfig.h"

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

struct SBlock
{
    char  *name;     //Name cell
    char  *lname;    //Long name cell
    char  *descr;    //Description cell
    int   type:2;    //VAL_T_REAL, VAL_T_BOOL, VAL_T_INT 
    int   stat:2;    //VAL_S_GENER, VAL_S_UTIL, VAL_S_SYS
    int   mode:2;    //VAL_M_OFTN, VAL_M_SELD, VAL_M_CNST
    int   data:2;    //VAL_D_FIX, VAL_D_BD, VAL_D_VBD
    int   access:10; //Access to cell (0444)
    float min;       //Minimum numberic val
    float max;       //Maximum nemberic val
};

union SValue
{
    float val_r;     //real value
    bool  val_b;     //boolean value
    int   val_i;     //integer value
};

class TValue : public TConfig
{
/** Public methods: */
public:
    TValue( TConfigElem *Elements );
    ~TValue();
    /*
     * Add value into a param buffer
     */
    int AddVal(int id_val, SBlock *block);
    /*
     * Delete value from a param buffer
     */
    int DelVal(int id_val);
    /*
     * Size value 
     */
    int SizeVal() {return(Size());}
    /*
     * Size buffer
     */
    int SizeBuff() {return(value.size());}
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
    
/** Public methods: */
public:
/** Private atributes: */
private:
    vector<vector <SValue> > value; // buffer values
};

#endif // TVALUE_H

