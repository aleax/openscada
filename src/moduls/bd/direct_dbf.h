/* Test Modul
** ==============================================================
*/

#ifndef TEST_BD_H
#define TEST_BD_H

#include <string>
#include <vector>

#include "../../tmodule.h"
#include "../../tbds.h"

using std::string;
using std::vector;

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
	
    	void   Save( );
	
	string GetCellS( int colm, int line );
	double GetCellR( int colm, int line );
	int    GetCellI( int colm, int line );
	bool   GetCellB( int colm, int line );    
	void   SetCellS( int colm, int line, string cell );
	void   SetCellR( int colm, int line, double val );
	void   SetCellI( int colm, int line, int val );
	void   SetCellB( int colm, int line, bool val );	
    	int    NLines( );
    	int    AddLine( unsigned int line );
    	void   DelLine( unsigned int line );
    	int    NColums(  );
    	int    AddColum( SColmAttr *colm );
    	void   DelColum( int colm );
    	void   GetColumAttr( int colm, SColmAttr *attr );
	void   SetColumAttr( int colm, SColmAttr *attr );
    	int    ColumNameToId( string colm );
    	string GetCodePage( );
    	void   SetCodePage( string codepage );
    private:
	string n_table;
	string codepage;
	TBasaDBF *basa;
	int    m_res;
};

class TBDdir : public TBD
{
    public:
	TBDdir( string name );
	~TBDdir(  );

	TTable *TableOpen( string table, bool create );
	void TableDel( string table );
    private:    
};


class TDirectDB : public TTipBD
{
  public:
    TDirectDB( string name );
    ~TDirectDB(  );

    TBD *BDOpen( string name, bool create );
    
    void mod_CheckCommandLine( );
    void mod_UpdateOpt();
  private:
    void pr_opt_descr( FILE * stream );
  private:
};

#endif // TEST_BD_H
