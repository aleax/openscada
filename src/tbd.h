
#ifndef TBD_H
#define TBD_H

#include <stdio.h>
#include <string>
using std::string;
#include <vector>
using std::vector;
#include "tgrpmodule.h"

struct SRowAttr
{
    string   name;  // Name row
    char     type;  // type cell ('C' - string, 'N' - numberic )
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
    int NewBD( string nametype, string name );
    int NewBD( int idtype, string name );

    int DelBD( string name );
    int DelBD( string nametype, string name );
    int DelBD( int idtype, string name );
    
    int OpenBD( string name );
    int OpenBD( string nametype, string name );
    int OpenBD( int idtype, string name );
    
    int CloseBD( int hd );
    int CloseBD( string nametype, int hd );
    int CloseBD( int idtype, int hd );

    int SaveBD(unsigned int hd );
    int SaveBD( string nametype, unsigned int hdi);
    int SaveBD( unsigned int idtype, unsigned int hdi );

    int GetCellS( int hd, int row, int line, string & cell);
    int GetCellS( string nametype, int hd, int row, int line, string & cell);
    int GetCellS( int idtype, int hd, int row, int line, string & cell);

    int GetCellS( int hd, string row, int line, string & cell);
    int GetCellS( string nametype, int hd, string row, int line, string & cell);
    int GetCellS( int idtype, int hd, string row, int line, string & cell);

    int GetCellN( int hd, int row, int line, double & val);
    int GetCellN( string nametype, int hd, int row, int line, double & val);
    int GetCellN( int idtype, int hd, int row, int line, double & val);

    int GetCellN( int hd, string row, int line, double & val);
    int GetCellN( string nametype, int hd, string row, int line, double & val);
    int GetCellN( int idtype, int hd, string row, int line, double & val);

    int SetCellS( int hd, int row, int line, const string & cell);
    int SetCellS( string nametype, int hd, int row, int line, const string & cell);
    int SetCellS( int idtype, int hd, int row, int line, const string & cell);

    int SetCellS( int hd, string row, int line, const string & cell);
    int SetCellS( string nametype, int hd, string row, int line, const string & cell);
    int SetCellS( int idtype, int hd, string row, int line, const string & cell);

    int SetCellN( int hd, int row, int line, double val);
    int SetCellN( string nametype, int hd, int row, int line, double val);
    int SetCellN( int idtype, int hd, int row, int line, double val);

    int SetCellN( int hd, string row, int line, double val);
    int SetCellN( string nametype, int hd, string row, int line, double val);
    int SetCellN( int idtype, int hd, string row, int line, double val);
    
    int NLines( int hd );
    int NLines( string nametype, int hd );
    int NLines( int idtype, int hd );

    int AddLine( unsigned int hd, unsigned int line);
    int AddLine( string nametype, unsigned int hdi, unsigned int line );
    int AddLine( unsigned int idtype, unsigned int hdi, unsigned int line );

    int DelLine(unsigned int hd, unsigned int line);
    int DelLine( string nametype, unsigned int hdi, unsigned int line );
    int DelLine( unsigned int idtype, unsigned int hdi, unsigned int line );
    
    int NRows( int hd );
    int NRows( string nametype, int hd );
    int NRows( int idtype, int hd );

    int AddRow( unsigned int hd, SRowAttr *row );
    int AddRow( string nametype, unsigned int hd, SRowAttr *row );
    int AddRow( int idtype, unsigned int hd, SRowAttr *row );

    int DelRow( unsigned int hd, string row);
    int DelRow( string nametype, unsigned int hd, string row);
    int DelRow( int idtype, unsigned int hd, string row);

    int GetRowAttr( unsigned int hd, int row, SRowAttr *attr );
    int GetRowAttr( string nametype, unsigned int hd, int row, SRowAttr *attr );
    int GetRowAttr( int idtype, unsigned int hd, int row, SRowAttr *attr );

    int SetRowAttr( unsigned int hd, int row, SRowAttr *attr );
    int SetRowAttr( string nametype, unsigned int hd, int row, SRowAttr *attr );
    int SetRowAttr( int idtype, unsigned int hd, int row, SRowAttr *attr ); 
    
    int RowNameToId( unsigned int hd, string namerow);
    int RowNameToId( string nametype, unsigned int hd, string namerow);
    int RowNameToId( int idtype, unsigned int hd, string namerow);
    
    int GetCodePage( string nametype, unsigned int hd, string & codepage);
    int GetCodePage( int idtype, unsigned int hd, string & codepage);
 
    int SetCodePage( string nametype, unsigned int hd, string codepage);
    int SetCodePage( int idtype, unsigned int hd, string codepage);
   
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
/** Private atributes: */
private:
    vector< vector<int> > hdBD;
};

#endif // TBD_H
