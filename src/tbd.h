
#ifndef TBD_H
#define TBD_H

#include <string>
using std::string;
#include <vector>
using std::vector;
#include "tgrpmodule.h"


class TBD : public TGRPModule
{

/** Public methods: */
public:
     TBD(  );

     ~TBD(  );

    void Init(  );

    int NewBD( string nametype, string name );
    int NewBD( int idtype, string name );

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

    int GetCell( int hd, int row, int line, string & cell);
    int GetCell( string nametype, int hd, int row, int line, string & cell);
    int GetCell( int idtype, int hd, int row, int line, string & cell);

    int GetCell( int hd, string row, int line, string & cell);
    int GetCell( string nametype, int hd, string row, int line, string & cell);
    int GetCell( int idtype, int hd, string row, int line, string & cell);

    int SetCell( int hd, int row, int line, const string & cell);
    int SetCell( string nametype, int hd, int row, int line, const string & cell);
    int SetCell( int idtype, int hd, int row, int line, const string & cell);

    int SetCell( int hd, string row, int line, const string & cell);
    int SetCell( string nametype, int hd, string row, int line, const string & cell);
    int SetCell( int idtype, int hd, string row, int line, const string & cell);

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

    int AddRow( unsigned int hd, string row, char type, unsigned int len=10, unsigned int dec=2);
    int AddRow( string nametype, unsigned int hd, string row, char type, unsigned int len=10, unsigned int dec=2);
    int AddRow( int idtype, unsigned int hd, string row, char type, unsigned int len=10, unsigned int dec=2);

    int DelRow(unsigned int hd, string row);
    int DelRow( string nametype, unsigned int hd, string row);
    int DelRow( int idtype, unsigned int hd, string row);

    int GetCodePage( string nametype, unsigned int hd, string & codepage);
    int GetCodePage( int idtype, unsigned int hd, string & codepage);
 
    int SetCodePage( string nametype, unsigned int hd, string codepage);
    int SetCodePage( int idtype, unsigned int hd, string codepage);
   
    void CheckCommandLine(  );
    

    bool test(int idtype);
/** Public atributes:: */
public:

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
