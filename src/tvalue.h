#ifndef TVALUE_H
#define TVALUE_H

#include <string>
using std::string;
#include <vector>
using std::vector;

//====  Mode access to value ====
#define VAL_MA_DIRECT  0  //Direct access to controller

#include "terror.h"
#include "tvalueelem.h"

union SValue
{
    string *val_s;     //string value
    float  val_r;     //real value
    bool   val_b;     //boolean value
    int    val_i;     //integer value
};

class TValue
{
/** Public methods: */
public:
    friend class TValueElem;
    TValue( );
    ~TValue();

    void SetValType( TValueElem *ValEl );
    /*
     * Object of element for value
     */
    TValueElem *Elem() { if(elem == NULL) throw(TError("%s: Value without type",o_name)); return(elem);}
    /*
     * Size buffer
     */
    int Size() {return(buffer.size());}
    /*
     * Resize buffer
     */
    void ResizeBuff(unsigned int len);
    /*
     * Set new value(s) into buffer
     */
    /*
    string Set_SEL(int id, string val, bool owner=false);
    string Set_S(int id, string val, bool owner=false);
    int Set_I(int id, int val, bool owner=false);
    double Set_R(int id, double val, bool owner=false);
    bool Set_B(int id, bool val, bool owner=false);
    */
    /*
     * Read curent value
     */
    /*
    string Get_SEL(int id);
    string Get_S(int id);
    int Get_I(int id);
    double Get_R(int id);
    bool Get_B(int id);
    */
/** Public atributes: */
public:

/** Private metods */
private:
    /*
     * Add elem into TValueElem
     */
    void AddElem(int id_val); 
    /*
     * Del elem without TValueElem
     */
    void DelElem(int id_val);  
    
/** Private atributes: */
private:
    vector<SValue>           value;  // curent value
    vector<vector <SValue> > buffer; // buffer values
    TValueElem               *elem;  // elements  
    static const char        *o_name;
};

#endif // TVALUE_H

