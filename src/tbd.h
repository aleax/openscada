
#ifndef TBD_H
#define TBD_H

#include <stdio.h>
#include <string>
using std::string;
#include <vector>
using std::vector;
#include "terror.h"
#include "tgrpmodule.h"

#define BD_ROW_STRING  0
#define BD_ROW_REAL    1
#define BD_ROW_INT     2
#define BD_ROW_BOOLEAN 3

struct SColmAttr
{
    string   name;  // Name column
    char     tp;    // type cell (BD_FLD_STRING, BD_FLD_REAL, BD_FLD_INT, BD_FLD_BOOLEAN)
    unsigned len;   // Len in symbol for string and numberic in string 
    unsigned dec;   // Dec for real numberic
};

class TBD : public TGRPModule
{
/** Public methods: */
public:
     TBD(  );

     ~TBD(  );

    int NewBD( string name );
    int NewBD( int idtype, string name );

    void DelBD( string name );
    void DelBD( int idtype, string name );
    
    int OpenBD( string name );
    int OpenBD( int idtype, string name );
    
    void CloseBD( unsigned int hd );
    void CloseBD( unsigned int idtype, unsigned int hd );

    void SaveBD( unsigned int hd );
    void SaveBD( unsigned int idtype, unsigned int hdi );

    string GetCellS( unsigned int hd, int colm, int line);
    string GetCellS( unsigned int idtype, unsigned int hd, int colm, int line);

    double GetCellR( unsigned int hd, int colm, int line);
    double GetCellR( unsigned int idtype, unsigned int hd, int colm, int line);

    int GetCellI( unsigned int hd, int colm, int line);
    int GetCellI( unsigned int idtype, unsigned int hd, int colm, int line);

    bool GetCellB( unsigned int hd, int colm, int line);
    bool GetCellB( unsigned int idtype, unsigned int hd, int colm, int line);

    void SetCellS( unsigned int hd, int colm, int line, const string cell);
    void SetCellS( unsigned int idtype, unsigned int hd, int colm, int line, const string cell);

    void SetCellR( unsigned int hd, int colm, int line, double val);
    void SetCellR( unsigned int idtype, unsigned int hd, int colm, int line, double val);

    void SetCellI( unsigned int hd, int colm, int line, int val);
    void SetCellI( unsigned int idtype, unsigned int hd, int colm, int line, int val);

    void SetCellB( unsigned int hd, int colm, int line, bool val);
    void SetCellB( unsigned int idtype, unsigned int hd, int colm, int line, bool val);

    int NLines( unsigned int hd );
    int NLines( unsigned int idtype, unsigned int hd );

    int AddLine( unsigned int hd, unsigned int line);
    int AddLine( unsigned int idtype, unsigned int hdi, unsigned int line );

    void DelLine(unsigned int hd, unsigned int line);
    void DelLine( unsigned int idtype, unsigned int hdi, unsigned int line );
    
    int NColums( unsigned int hd );
    int NColums( unsigned int idtype, unsigned int hd );

    void AddColum( unsigned int hd, SColmAttr *colm );
    int  AddColum( int idtype, unsigned int hd, SColmAttr *colm );

    void DelColum( unsigned int hd, string colm);
    void DelColum( int idtype, unsigned int hd, string colm);

    void GetColumAttr( unsigned int hd, int colm, SColmAttr *attr );
    void GetColumAttr( int idtype, unsigned int hd, int colm, SColmAttr *attr );

    void SetColumAttr( unsigned int hd, int colm, SColmAttr *attr );
    void SetColumAttr( int idtype, unsigned int hd, int colm, SColmAttr *attr ); 
    
    int ColumNameToId( unsigned int hd, string colm);
    int ColumNameToId( int idtype, unsigned int hd, string colm);
    
    string GetCodePage( unsigned int idtype, unsigned int hd );
 
    void SetCodePage( unsigned int idtype, unsigned int hd, string codepage);
   
    void UpdateOpt();
    void CheckCommandLine(  );
/** Public atributes:: */
public:

    string def_t_bd;
/** Protected methods: */
protected:
    
    
/** Private methods: */
private:
    void pr_opt_descr( FILE * stream );
    virtual int AddM( TModule *modul );
    virtual int DelM( int hd );
    void CheckHD( unsigned int hd )
    { if( hd >= hdBD.size()) throw TError("%s: BD's hd error!",o_name); }
/** Private atributes: */
private:
    vector< vector<int> > hdBD;
    static const char     *o_name;
    static const char     *n_opt;
};

#endif // TBD_H
