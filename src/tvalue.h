#ifndef TVALUE_H
#define TVALUE_H

#include <string>
#include <vector>

#include "terror.h"
#include "telem.h"

using std::string;
using std::vector;

struct STime
{
    time_t s;
    long   us;
};

/*
struct SCale
{
    double max;
    double min;	
};
*/


struct SBasync
{
    int           i_head;          //id of header
    vector<STime> time;            //element's time
    union
    {
        vector<string> *val_s;     //string value
	vector<double> *val_r;     //real value
	vector<int>    *val_i;     //integer value
	vector<bool>   *val_b;     //boolean value
    }val;
};

struct SBsync
{
    STime t_head;            //element's time
    STime t_per;             //element's period
    int   i_head;
    int   i_tail;    
    union
    {
        vector<string> *val_s;     //string value
	vector<double> *val_r;     //real value
	vector<int>    *val_i;     //integer value
	vector<bool>   *val_b;     //boolean value
    }val;
};

union SBUF 
{
    SBsync  *sync;     
    SBasync *async;
};

#define V_NOVAL   0x1000  //No value mirored
#define V_RD_D    0x2000  //Direct read
#define V_WR_D    0x4000  //Direct write

class TValue;

class TVal
{
    public:
	TVal(TFld &fld, TValue &owner);
	TVal(TCfg &cfg, TValue &owner);
	~TVal();
	
	const string &name();

	TFld &fld();
       	// stat element 
    	bool valid()           { return(m_valid); }
    	void valid( bool val ) { m_valid = val; }
	// Read curent value (direct)
	string getSEL( STime *tm = NULL );
	string &getS( STime *tm = NULL );
	double &getR( STime *tm = NULL );
	int    &getI( STime *tm = NULL );
	bool   &getB( STime *tm = NULL );
	// Set curent value
	string setSEL( string value, STime *tm = NULL, bool sys = false );
	string &setS( string value, STime *tm = NULL, bool sys = false );
	double &setR( double value, STime *tm = NULL, bool sys = false );
	int    &setI( int value, STime *tm = NULL, bool sys = false );
	bool   &setB( bool value, STime *tm = NULL, bool sys = false );    
	
    protected:
	void vlSet(  );
	void vlGet(  );
	
    private:
	union
	{ 
	    string *val_s;   //string value
	    double val_r;    //real value
	    int    val_i;    //integer value	
	    bool   val_b;    //boolean value
	} val;
	
	bool     m_cfg;    //Config id
        union
	{
	    TFld *fld;
	    TCfg *cfg;
	} src;	
	TValue   &m_owner; //Owner
	bool     m_valid;  
	STime    time;     // Time
};


class TConfig;

class TValue: public TContElem
{
    friend class TVal;
/** Public methods: */
public:
    TValue( );
    TValue( TConfig *cfg );
    virtual ~TValue();

    void vlElem( TElem *ValEl );
    // Object of element for value
    TElem &vlElem() 
    { if(elem == NULL) throw(TError("%s: Value without type",o_name)); return(*elem); }

    void vlList( vector<string> &list );
    TVal &vlVal( string name );    
/** Protected metods */
protected:    
    virtual void vlSet( TVal &val ){};
    virtual void vlGet( TVal &val ){};
/** Private metods */
private:
    // Set value direct into controller param's
    virtual void vlSet( int id_elem )
    { throw TError("%s: Direct access to write value no avoid",o_name); }
    // Get value direct from controller param's    
    virtual void vlGet( int id_elem )
    { throw TError("%s: Direct access to read value no avoid",o_name); }
    // Get Cfg for BD    
    
    // Add elem into TElem
    void addElem(unsigned id_val); 
    // Del elem without TElem
    void delElem(unsigned id_val); 

/** Private atributes: */
private:
    vector<TVal *>    val;    // curent value
        
    TElem             *elem;  // elements  

    int		      l_cfg;  // Config len
    TConfig           *m_cfg; // Configs (static part)    
    vector<TValue *>  m_br;   // Value's branchs
    
    static const char *o_name;
};

#endif // TVALUE_H

