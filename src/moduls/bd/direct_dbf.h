/* Test Modul
** ==============================================================
*/

#ifndef TEST_BD_H
#define TEST_BD_H

#include <string>
#include <vector>

#include "../gener/tmodule.h"

class TBasaDBF;

struct Shd
{
    int      use;
    string   path;
    TBasaDBF *basa;
};

class TDirectDB:public TModule
{
  public:
    TDirectDB( char *name );
      virtual ~ TDirectDB(  );

    virtual int info( const string & name, string & info );
    virtual int init(  );

    void CheckCommandLine(  );
    int OpenBD( string name );
    int CloseBD( int hdi );
    int GetCell1( int hdi, int row, int line, string & cell);
    int GetCell2( int hdi, string row, int line, string & cell);
    int SetCell1( int hdi, int row, int line, const string & cell);
    int SetCell2( int hdi, string row, int line, const string & cell);
    int NLines( int hdi );
    int NRows( int hdi );
  public:
  private:
    void pr_opt_descr( FILE * stream );
  private:
    static SExpFunc ExpFuncLc[];
    string pathsBD;
    vector <Shd *> hd;
};

#endif // TEST_BD_H
