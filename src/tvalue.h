#ifndef TVALUE_H
#define TVALUE_H

#include <string>
using std::string;
#include <vector>
using std::vector;

//====  Mode access to value ====
#define VAL_MA_DIRECT  0  //Direct access to controller

#include "tvalueelem.h"

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

    int SetValType( TValueElem *ValEl );
    /*
     * Object of element for value
     */
    TValueElem *Elem() { return(elem);}
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

