#ifndef THD_H
#define THD_H

#include <string>
#include <vector>

using std::string;
using std::vector;

//======================================================================================
//====================== THD ===========================================================
//======================================================================================
struct SHD_hd
{
    unsigned use;
    unsigned hd;
};

struct SHD_obj
{
    void     *obj;
    string   *name;
    bool     del;     //If object deleted (no attached)
};

class THD
{
    public:
	THD( );
	~THD( );

	int  res( int id_res );

        unsigned hd_obj_cnt( );  
        bool &hd_obj_free(){ return(m_free); }
        void hd_obj_list( vector<string> &list );
	void hd_obj_add( void *obj, string *name );
        void *hd_obj_del( string &name, long tm = 0);	

        unsigned hd_att( string &name );
	void hd_det( unsigned i_hd );
        void *hd_at( unsigned i_hd );	
	
	void *obj( string &name );
	void *obj( unsigned i_hd );
	unsigned obj_use( string &name );
	unsigned obj_use( unsigned i_hd );
	
	void lock() { m_lock = true; }
    private:
	vector<SHD_hd>  m_hd;
	vector<SHD_obj> m_obj;

	int hd_res;
        bool res_ext;       //External resource used
	bool m_lock;        //Locked hd
	bool m_free;        //No object avoid 
	
	static const char *o_name; 
};


#endif // THD_H
