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
#include "../../tbd.h"

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
    ~TDirectDB(  );

    void info( const string & name, string & info );
    void init( void *param );

    void CheckCommandLine(  );
    void UpdateOpt();
    int NewBD( string name );
    int OpenBD( string name );
    void CloseBD( unsigned int hdi );
    void SaveBD( unsigned int hdi );
    void DelBD(string name );
    string GetCellS( unsigned int hdi, int colm, int line);
    double GetCellR( unsigned int hdi, int colm, int line);
    int    GetCellI( unsigned int hdi, int colm, int line);
    bool   GetCellB( unsigned int hdi, int colm, int line);    
    void SetCellS( unsigned int hdi, int colm, int line, const string cell);
    void SetCellR( unsigned int hdi, int colm, int line, double val);
    void SetCellI( unsigned int hdi, int colm, int line, int val);
    void SetCellB( unsigned int hdi, int colm, int line, bool val);
    int NLines( unsigned int hdi );
    int AddLine(unsigned int hdi, unsigned int line);
    void DelLine(unsigned int hdi, unsigned int line);
    int NColums( unsigned int hdi );
    int AddColum(unsigned int hdi, SColmAttr *colm);
    void DelColum(unsigned int hdi, string colm);
    void GetColumAttr(unsigned int hdi, int colm, SColmAttr *attr);
    void SetColumAttr(unsigned int hdi, int colm, SColmAttr *attr);
    int ColumNameToId(unsigned int hdi, string colm);

    string GetCodePageBD( unsigned int hdi );
    void SetCodePageBD( unsigned int hdi, string codepage );
  public:
  private:
    void pr_opt_descr( FILE * stream );
    void CheckHD( unsigned int hdi )
    { if(hdi>=hd.size() || hd[hdi].use <= 0 ) throw TError(o_name+": hd of BD error!"); }
  private:
    static SExpFunc ExpFuncLc[];
    string          pathsBD;
    vector          <Shd> hd;
    string          extens;
    static string   o_name;
};

#endif // TEST_BD_H
