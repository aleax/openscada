
#ifndef TBDS_H
#define TBDS_H

#include <stdio.h>
#include <string>
using std::string;
#include <vector>
using std::vector;
#include "terror.h"
#include "tmodule.h"
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

class TTable
{
    public:
	TTable();
	virtual ~TTable();

	void Save();
	string GetCellS( int colm, int line);
	double GetCellR( int colm, int line);
	int GetCellI( int colm, int line);
	bool GetCellB( int colm, int line);
	void SetCellS( int colm, int line, const string cell);
	void SetCellR( int colm, int line, double val);
	void SetCellI( int colm, int line, int val);
	void SetCellB( int colm, int line, bool val);
	int NLines( );
	int AddLine( unsigned int line);
	void DelLine( unsigned int line);
	int NColums( );
	int AddColum( SColmAttr *colm );
	void DelColum( int colm);
	void GetColumAttr( int colm, SColmAttr *attr );
	void SetColumAttr( int colm, SColmAttr *attr );
	int ColumNameToId( string colm );
	string GetCodePage( );
	void SetCodePage( string codepage );
    private:
        void ENTER();
	void EXIT();
	
	virtual void _Save()
	{ throw TError(_err,"Save",o_name); }	
	virtual string _GetCellS( int colm, int line)
	{ throw TError(_err,"GetCellS",o_name); }
	virtual double _GetCellR( int colm, int line)
	{ throw TError(_err,"GetCellR",o_name); }
	virtual int _GetCellI( int colm, int line)
	{ throw TError(_err,"GetCellI",o_name); }
	virtual bool _GetCellB( int colm, int line)
	{ throw TError(_err,"GetCellB",o_name); }
	virtual void _SetCellS( int colm, int line, const string cell)
	{ throw TError(_err,"SetCellS",o_name); }
	virtual void _SetCellR( int colm, int line, double val)
	{ throw TError(_err,"SetCellR",o_name); }
	virtual void _SetCellI( int colm, int line, int val)
	{ throw TError(_err,"SetCellI",o_name); }
	virtual void _SetCellB( int colm, int line, bool val)
	{ throw TError(_err,"SetCellB",o_name); }
	virtual int _NLines( )
	{ throw TError(_err,"NLines",o_name); }
	virtual int _AddLine( unsigned int line)
	{ throw TError(_err,"AddLines",o_name); }
	virtual void _DelLine( unsigned int line)
	{ throw TError(_err,"DelLines",o_name); }
	virtual int _NColums( )
	{ throw TError(_err,"NColums",o_name); }
	virtual int _AddColum( SColmAttr *colm )
	{ throw TError(_err,"AddColum",o_name); }
	virtual void _DelColum( int colm)
	{ throw TError(_err,"DelColum",o_name); }
	virtual void _GetColumAttr( int colm, SColmAttr *attr )
	{ throw TError(_err,"GetColumAttr",o_name); }
	virtual void _SetColumAttr( int colm, SColmAttr *attr )
	{ throw TError(_err,"SetColumAttr",o_name); }
	virtual int _ColumNameToId( string colm )
	{ throw TError(_err,"ColumNameToId",o_name); }
	virtual string _GetCodePage( )
	{ throw TError(_err,"GetCodePage",o_name); }
	virtual void _SetCodePage( string codepage )
	{ throw TError(_err,"SetCodePage",o_name); }
    private:
	unsigned hd_res;
	unsigned use;
	
	static const char *o_name;
	static char *_err;
};    

struct STable
{
    int    use;
    TTable *tbl;
};

class TBD
{
    public:
	TBD();
	virtual ~TBD();
	
	/*
	 * Open bd table. if create = true then try create if no avoid table
	 */
	int OpenTable( string table, bool create );
	/*
	 * Save and Close table
	 */
	void CloseTable( unsigned int hd );
	/*
	 * Delete table
	 */
	TTable *at(unsigned int id)
	{ 
	    if(id > table.size() || table[id].use <= 0 ) throw TError("%s: table identificator error!");
	    return(table[id].tbl);
	}
	TTable *operator[](unsigned id ) { return(at(id)); }

	void DelTable( string table ){ TableDel(table); }
    protected:
	vector< STable > table;	
    
    private:
	virtual TTable *TableOpen( string table, bool create )
	{ throw TError("%s: function TableOpen no support!",o_name); }
	virtual void TableDel( string table )
	{ throw TError("%s: function TableDel no support!",o_name); }
    private:
	unsigned      hd_res;
    
	static const char *o_name;
};

struct SBD
{
    int use;
    TBD *bd;
};

class TBDS;

class TTipBD : public TModule
{
/** Public methods: */
    public:
	TTipBD(  );

	virtual ~TTipBD(  );

	unsigned int OpenBD( string name, bool create );
	void CloseBD( unsigned int hd );


	TBD *at( unsigned int id );  
	TBD *operator[](unsigned id ) { return(at(id)); }
/** Public atributes:: */
    public:
	TBDS *owner;
/** Public atributes:: */
    private:    
	virtual TBD *BDOpen( string name, bool create )
	{throw TError("%s: Error open BD %s!",o_name,name.c_str()); }
/** Private atributes:: */
    private:
	unsigned      hd_res;
	vector< SBD > bd;	

	static const char *o_name;
};
	

struct SGTable
{
    int use;
    int type_bd;
    int bd;
    int table;
};

class TKernel;

class TBDS : public TGRPModule
{         
/** Public methods: */
    public:
	TBDS( TKernel *app );
    
       	~TBDS(  );

	TTipBD *at_tp( unsigned id ) { return( (TTipBD *)gmd_at(id) ); }
	TTipBD *operator[](unsigned id ) { return(at_tp(id)); }

	int OpenTable( string tb_name, string b_name, string t_name, bool create = false );
	void CloseTable( unsigned int id );
        TTable *at_tbl( unsigned int id );

	void gmd_UpdateOpt();
	void gmd_CheckCommandLine( );

    public:

/** Private methods: */
    private:
	void pr_opt_descr( FILE * stream );
	virtual int  gmd_AddM( TModule *modul );
	virtual void gmd_DelM( unsigned hd );

/** Private atributes: */
    private:
	//vector< TTipBD *> TBD;  
	vector< SGTable > Table;  
	
	static const char     *o_name;
	static const char     *n_opt;
};

#endif // TBDS_H
