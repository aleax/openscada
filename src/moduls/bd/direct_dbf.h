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
    string   name_bd;
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
    int NewBD( string name );
    int OpenBD( string name );
    int CloseBD( int hdi );
    int SaveBD(unsigned int hdi );
    char *GetCharSetBD(int hdi);
    int GetCell1( int hdi, int row, int line, string & cell);
    int GetCell2( int hdi, string row, int line, string & cell);
    int SetCell1( int hdi, int row, int line, const string & cell);
    int SetCell2( int hdi, string row, int line, const string & cell);
    int NLines( int hdi );
    int AddLine(unsigned int hdi, unsigned int line);
    int DelLine(unsigned int hdi, unsigned int line);
    int NRows( int hdi );
    int AddRow(unsigned int hdi, string row, char type, unsigned int len=10, unsigned int dec=2);
    int DelRow(unsigned int hdi, string row);
  public:
  private:
    void pr_opt_descr( FILE * stream );
  private:
    static SExpFunc ExpFuncLc[];
    string pathsBD;
    vector <Shd *> hd;
    string extens;
};

#endif // TEST_BD_H
