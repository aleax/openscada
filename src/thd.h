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
    string   user;
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
	THD( const char *obj_n );
	~THD( );

	/*
	 * Objects avoid counter
	 */
        unsigned obj_cnt( );
	/*
	 * Internal free object stat
	 */
        bool &obj_free(){ return(m_free); }
	/*
	 * Avoid object list
	 */	
        void obj_list( vector<string> &list );
	/*
	 * Add object
	 */	
	void obj_add( void *obj, string *name, int pos = -1 );
	/*
	 * Delete object
	 */	
        void *obj_del( string &name, long tm = 0);	
	/*
	 * Rotate object (rotate position objects )
	 */	
        void obj_rotate( string &name1, string &name2 );	
	/*
	 * Use object counter.
	 */
	unsigned obj_use( string &name );
	unsigned obj_use( unsigned i_hd );
	/*
	 * Get object. Dangerous no resources!!!!!
	 */
	void *obj( string &name );


	/*
	 * Attach to object and make hd for access it
	 */
        unsigned hd_att( string &name, string user = "" );
	/*
	 * Detach from object 
	 */
	void hd_det( unsigned i_hd );
	/*
	 * Get attached object
	 */
        void *hd_at( unsigned i_hd );	
	
	/*
	 * Lock for attach and add object
	 */
	void lock() { m_lock = true; }
	/*
	 * Use external resource (header)
	 */
	int  res( int id_res );
    private:
	vector<SHD_hd>  m_hd;
	vector<SHD_obj> m_obj;

	int hd_res;
        bool res_ext;       //External resource used
	bool m_lock;        //Locked hd
	bool m_free;        //No object avoid 

	const char *u_name;     //Object name
	
	static const char *o_name; 
};


#endif // THD_H
