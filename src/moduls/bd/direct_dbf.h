/* Test Modul
** ==============================================================
*/

#ifndef TEST_BD_H
#define TEST_BD_H

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "../gener/tmodule.h"

class TBasaDBF;

struct Shd
{
    int      use;
    string   name_bd;
    TBasaDBF *basa;
    string   codepage;
};

class TDirectDB:public TModule
{
  public:
    TDirectDB( char *name );
    virtual ~ TDirectDB(  );

    virtual int info( const string & name, string & info );
    virtual int init( void *param );

    void CheckCommandLine(  );
    int NewBD( string name );
    int OpenBD( string name );
    int CloseBD( int hdi );
    int SaveBD(unsigned int hdi );
    int DelBD(string name );
    int GetCellS( int hdi, int row, int line, string & cell);
    int GetCellN( int hdi, int row, int line, double & val);
    int SetCellS( int hdi, int row, int line, const string & cell);
    int SetCellN( int hdi, int row, int line, double val);
    int NLines( int hdi );
    int AddLine(unsigned int hdi, unsigned int line);
    int DelLine(unsigned int hdi, unsigned int line);
    int NRows( int hdi );
    int AddRow(unsigned int hdi, string row, char type, unsigned int len, unsigned int dec);
    int DelRow(unsigned int hdi, string row);
    int GetRowAttr(unsigned int hdi, int row, string & namerow, char & type, unsigned int & len, unsigned int & dec);
    int RowNameToId(unsigned int hdi, string namerow);

    int GetCodePageBD(int hdi, string & codepage );
    int SetCodePageBD(int hdi, string codepage );
  public:
  private:
    void pr_opt_descr( FILE * stream );
  private:
    static SExpFunc ExpFuncLc[];
    string pathsBD;
    vector <Shd> hd;
    string extens;
};

#endif // TEST_BD_H
