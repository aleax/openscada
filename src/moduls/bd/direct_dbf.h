/* Test Modul
** ==============================================================
*/

#ifndef TEST_BD_H
#define TEST_BD_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "../../tmodule.h"
#include "../../tbds.h"

class TBasaDBF;

struct Shd
{
    int      use;
    string   name_bd;
    TBasaDBF *basa;
    string   codepage;
};

class TTableDir : public TTable
{
    public:
    	TTableDir(string name, bool create);
    	~TTableDir(  );

        string Name(){ return(n_table); }
	
    	void   _Save( );
	
	string _GetCellS( int colm, int line );
	double _GetCellR( int colm, int line );
	int    _GetCellI( int colm, int line );
	bool   _GetCellB( int colm, int line );    
	void   _SetCellS( int colm, int line, const string cell );
	void   _SetCellR( int colm, int line, double val );
	void   _SetCellI( int colm, int line, int val );
	void   _SetCellB( int colm, int line, bool val );	
    	int    _NLines( );
    	int    _AddLine( unsigned int line );
    	void   _DelLine( unsigned int line );
    	int    _NColums(  );
    	int    _AddColum( SColmAttr *colm );
    	void   _DelColum( int colm );
    	void   _GetColumAttr( int colm, SColmAttr *attr );
	void   _SetColumAttr( int colm, SColmAttr *attr );
    	int    _ColumNameToId( string colm );
    	string _GetCodePage( );
    	void   _SetCodePage( string codepage );
    private:
	string n_table;
	string codepage;
	TBasaDBF *basa;
};

class TBDdir : public TBD
{
    public:
	TBDdir( string name );
	~TBDdir(  );

	TTable *TableOpen( string table, bool create );
	void TableDel( string table );
    private:    
	string p_bd;
};


class TDirectDB : public TModule
{
  public:
    TDirectDB( char *name );
    ~TDirectDB(  );

    //void info( const string & name, string & info );
    void connect( void *obj );

    TBD *BDOpen( string name, bool create );
    
    void CheckCommandLine( char **argv, int argc );
    void UpdateOpt();
  private:
    void pr_opt_descr( FILE * stream );
  private:
    static SExpFunc ExpFuncLc[];
    TTipBD          *Tbd;   
    
    static string   o_name;
};

#endif // TEST_BD_H
