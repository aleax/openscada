    
#ifndef TELEM_H
#define TELEM_H

#include <string>
#include <vector>
//---- New Type ----
#define T_STRING  0x01
#define T_DEC     0x02
#define T_OCT     0x04
#define T_HEX     0x08
#define T_REAL    0x10
#define T_BOOL    0x20

#define T_SELECT  0x40   //Connnect to simple elements

#define F_SELF    0x80   //Create self field
#define F_NRD     0x100  //Readable
#define F_NWR     0x200  //Writeable
#define F_PREV    0x400  //Prevent owner for change

using std::string;
using std::vector;

struct SFld
{
    string   name;       // Name of element (name column into BD);
    string   descr;      // Description of element;
    unsigned type;       // Type of element (T_STRING, T_DEC, T_OCT, T_HEX, T_REAL, T_BOOL, T_SELECT|T_STRING);
    string   valDef;     // default value;
    string   valLen;     // len field (for string element and other element: 4, 4.2, 3.5) 
    string   vals;       // values ("0;100" - min = 0, max = 100 if no select) ("0;2;5" - enumerate if select);
    string   nSel;       // names of selectable element's
};

//Internal structures
class TFld
{
    public:
	TFld( SFld *fld = NULL );
	~TFld();

	// Name of element (name column into BD); 
	const string &name() { return m_name; }
       	// Description of element;        
	string &descr()      { return m_descr; }
       	// len field (for string element and other element: 4, 4.2, 3.5)         
	const string &len()  { return m_len; }
       	// Type of element (T_STRING, T_DEC, T_OCT, T_HEX, T_REAL, T_SELECT, T_SELECT|T_STRING);        
	unsigned type()      { return m_type; }
       	// default value; 
	const string &def()  { return m_def; }
       	// enable write to atribut (modified)
	//bool         wr()    { return m_wr; }
	
       	// values ("0;100" - min = 0, max = 100 if no select) ("0;2;5" - enumerate if select) ("23" - maximum string len);        
	//vector<string> &vals();
	
	vector<string> &val_s();
	vector<int>    &val_i();
	vector<double> &val_r();
	vector<bool>   &val_b();
       	// selectable element's name        
	vector<string> &nSel();

	string selName( string &val );
	string selName( int val );
	string selName( double val );
	string selName( bool val );
	
	string &selVals( string &name );	
	int    selVali( string &name );	
	double selValr( string &name );	
	bool   selValb( string &name );	

	TFld &operator=( TFld &fld );
    private:
	string          m_name;  
	string          m_descr; 
	string          m_len;
	unsigned        m_type;  
	string          m_def;
	//vector<string>  m_vals;  
	union           
	{
	    vector<string> *v_s;
	    vector<double> *v_r;
	    vector<int>    *v_i;
	    vector<bool>   *v_b;
	}m_val;
	vector<string>     *m_sel;  
}; 

class TTable;
class TContElem;

class TElem
{
    /** Public methods: */
    public:
	TElem( string name );
	~TElem();
	
	string &elName( ) { return(m_name); }
	// Add Element to position <id> and return realy position
	int elAdd( unsigned int id, SFld *element );
	int elAdd( SFld *element ){ return(elAdd(elSize(),element)); }
	// Delete element, free cell and route all elementes
	void elDel(unsigned int id);
	// Get element's numbers
	void elLoad( SFld *elements, int numb );

        void cntAtt( TContElem *cnt ); 
        void cntDet( TContElem *cnt ); 
	
	void elList( vector<string> &list );
	unsigned elSize(){ return(elem.size()); }    
	unsigned int elNameId(string name);
	TFld &elAt( unsigned int id );
	int elType( unsigned int id ) const;
    
	void elUpdateBDAttr( TTable &tbl );
    private:
    /**Attributes: */
    private:
        string             m_name;
	vector<TFld*>      elem;
	vector<TContElem*> cont;        //Conteiners
	static const char  *o_name;
};

//**********************************************************************
//************* TContElem - container of elements **********************
//**********************************************************************

class TContElem
{
    friend class TElem;
    /** Public methods: */
    public:	
	TContElem();
	virtual ~TContElem();
    protected:
	// Add element
	virtual void addElem( unsigned id ) = 0;
	// Del element
	virtual void delElem( unsigned id ) = 0;
};


#endif // TCONFIGELEM_H
