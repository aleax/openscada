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
//#define VAL_T_VAL    32  //cascade conection of value elements into value <in project>
//==== Value source ====
#define VAL_S_LOCAL  1   //local (forming of controllers)
#define VAL_S_BD     2   //from BD (forming of controllers) (name field BD into <vals>)
//==== Values IO mode ==== 
#define VAL_IO_DEF     0  //default mode write and read to internal buffer!
#define VAL_IO_W_DIR   1  //write direct to controller (no create internal buffer)!
#define VAL_IO_R_DIR   2  //read direct from controller (no create internal buffer)!
#define VAL_IO_B_SYNC  4  //enable sync buffer
#define VAL_IO_B_ASYNC 8  //enable async buffer

struct SVAL
{
    string name;      //Name cell
    string lname;     //Long name cell
    string descr;     //Description cell
    int    type;      //VAL_T_REAL, VAL_T_BOOL, VAL_T_INT, VAL_T_STRING | VAL_T_SELECT
    int    source;    //VAL_S_LOCAL, VAL_S_BD
    int    io;        //VAL_IO_W_DIR, VAL_IO_R_DIR, VAL_IO_B_SYNC, VAL_IO_B_ASYNC
    string view;      //view to gui (x.y - x - level viewing; y - priority)
    string vals;      //values ("0;100" - min = 0, max = 100 if no select $MIN;$MAX - из полей в БД) ("0;2;5" - enumerate if select)
    string n_sel;     //selectable element's name    
    string access;    //Access to cell ("0644&$ACCESS")
    int    l_buf;     //Buffer len
};

struct _SVAL
{
    string         name;      //Name cell
    string         lname;     //Long name cell
    string         descr;     //Description cell
    char           type:6;    //VAL_T_REAL, VAL_T_BOOL, VAL_T_INT, VAL_T_STRING | VAL_T_SELECT
    char           source:2;  //VAL_S_LOCAL, VAL_S_BD
    char           io:8;      //VAL_IO_W_DIR, VAL_IO_R_DIR
    string         view;      //view to gui (x.y - x - level viewing; y - priority) 
    vector<string> vals;      //values ("0;100" - min = 0, max = 100 if no select $MIN;$MAX - из полей в БД) ("0;2;5" - enumerate if select)
    vector<string> n_sel;     //selectable element's name
    string         access;    //Access to cell ("0644&$ACCESS")
    unsigned	   l_buf;     //buffer len
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
    void Add(SVAL *element) { Add(Size(),element); return; }
    void Add(unsigned int id_val, SVAL *element);
    /*
     * Delete value from a param buffer
     */
    void Del(unsigned int id_val);

    void Get( unsigned int id, SVAL &element ) const;
    /*
     * Size value 
     */
    unsigned int Size() const { return(elem.size()); }

    string Name() const { return(name); }

    void List(vector<string> &List) const;

    unsigned int NameToId( string name ) const;

    int Type( unsigned int id ) const;

    int TypeIO( unsigned int id ) const;    
/** Private atributes: */
private:
    string            name;
    vector<_SVAL>     elem;
    vector<TValue *>  value;

    static const char *o_name;
};

#endif // TVALUEELEM_H

