
//OpenSCADA module DAQ.JavaLikeCalc file: freefunc.h
/***************************************************************************
 *   Copyright (C) 2005-2019 by Roman Savochenko, <rom_as@oscada.org>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef FREEFUNC_H
#define FREEFUNC_H

#include <stdint.h>

#include <string>
#include <vector>
#include <deque>

#include <tfunction.h>
#include <tconfig.h>

//#define OSC_DEBUG
//#define IN_F_SHARE_REGS

#define EXT_F_LIM	128
#define EXT_F_AS_PREF	"stFunc:"
#define IN_F_REC_LIM	100

using std::string;
using std::vector;
using std::deque;
using namespace OSCADA;

namespace JavaLikeCalc
{

//Bison parse function
int yyparse( );

//*************************************************
//* Reg: Compile register                         *
//*************************************************
class Func;

class Reg
{
    public:
	//Data
	enum Type {
	    Free,	//Free
	    Bool,	//Boolean
	    Int,	//Integer
	    Real,	//Real
	    String,	//String
	    Dynamic,	//Dynamic type
	    Obj,	//Object
	    Var,	//IO variable
	    PrmAttr,	//Parameter attribute
	    Function	//Assigned static function
	};

	enum Code {	//Byte codes
	    End,	//[E]: End program.
	    EndFull,	//[E]: Full end from program.
	    MviB,	//[CRRB]: Load Boolean <B> to register <R>.
	    MviI,	//[CRR________]: Load Integer <________> to register <R>.
	    MviR,	//[CRR________]: Load Real <________> to register <R>.
	    MviS,	//[CRRn_____]: Load String len <n> to register <R>.
	    MviObject,	//[CRR]: Load object.
	    MviArray,	//[CRRnrr....]: Load array from registers list.
	    MviRegExp,	//[CRRrrRR]: Load Regular expression object.
	    MviSysObject,//[CRRn_____]: Load system object
	    MviFuncArg,	//[CRR]: Load the function arguments object.
	    Ass,	//[CRRrr]: Assign from register to register.
	    Mov,	//[CRRrr]: Move from register to register.
	    Delete,	//[CRR]: Delete/free object into the register.
	    OPrpSt,	//[CRRn_____]: Load string of object's properties len <n>  to register <R>.
	    OPrpDin,	//[CRRrr]: Load register's value of object's properties to register <R>.
	    Add, AddAss,//[CRRrrRR]: Real, string add.
	    Sub, SubAss,//[CRRrrRR]: Real subtract.
	    Mul, MulAss,//[CRRrrRR]: Real multiply.
	    Div, DivAss,//[CRRrrRR]: Real divide.
	    RstI,	//[CRRrrRR]: Integer divide rest.
	    BitOr,	//[CRRrrRR]: Integer bit or.
	    BitAnd,	//[CRRrrRR]: Integer bit and.
	    BitXor,	//[CRRrrRR]: Integer bit xor.
	    BitShLeft,	//[CRRrrRR]: Integer bit shift left.
	    BitShRight,	//[CRRrrRR]: Integer bit shift right.
	    LOr,	//[CRRrrRR]: Boolean OR.
	    LCOr,	//[CRRrrRRnn]: Conditional Boolean OR.
	    LAnd,	//[CRRrrRR]: Boolean AND.
	    LCAnd,	//[CRRrrRRnn]: Conditional Boolean AND.
	    LT,		//[CRRrrRR]: Real least.
	    GT,		//[CRRrrRR]: Real great.
	    LEQ,	//[CRRrrRR]: Real least equal.
	    GEQ,	//[CRRrrRR]: Real great equal.
	    EQU,	//[CRRrrRR]: Real, string equal.
	    NEQ,	//[CRRrrRR]: Real, string no equal.
	    Not,	//[CRRrr]: Boolean not.
	    BitNot,	//[CRRrr]: Integer bit not.
	    Neg,	//[CRRrr]: Negate real.
	    If,		//[CRR00nn]: Construction [if(R) else <00>; <nn>]
	    Cycle,	//[CRRbbaann]: Cycles construction [for(<first_init>;R=<cond>;aa)<bb>;<nn>] [while(R=<cond>)<bb>;<nn>]
	    CycleObj,	//[COObbRRnn]: Object cycles construction [for(RR in OO)<bb>;<nn>]
	    Break,	//[C]: Break for cycles
	    Continue,	//[C]: Continue for cycles
	    FSin,	//[CRRrr]: Function sine.
	    FCos,	//[CRRrr]: Function cosine.
	    FTan,	//[CRRrr]: Function tangent.
	    FSinh,	//[CRRrr]: Function sine hyperbolic.
	    FCosh,	//[CRRrr]: Function cosine hyperbolic.
	    FTanh,	//[CRRrr]: Function tangent hyperbolic.
	    FAsin,	//[CRRrr]: Function arcsine.
	    FAcos,	//[CRRrr]: Function arccosine.
	    FAtan,	//[CRRrr]: Function arctangent.
	    FRand,	//[CRRrr]: Function randomize.
	    FLg,	//[CRRrr]: Function decimal logarithm.
	    FLn,	//[CRRrr]: Function natural logarithm.
	    FExp,	//[CRRrr]: Function exponent.
	    FPow,	//[CRRrrRR]: Function power.
	    FMin,	//[CRRrrRR]: Function minimum.
	    FMax,	//[CRRrrRR]: Function maximum.
	    FSqrt,	//[CRRrr]: Function sqrt.
	    FAbs,	//[CRRrr]: Function absolute.
	    FSign,	//[CRRrr]: Function sign.
	    FCeil,	//[CRRrr]: Function ceil.
	    FFloor,	//[CRRrr]: Function floor.
	    FTypeOf,	//[CRRrr]: Function for get type of value.
	    FTr,	//[CRRrr]: Function for get translation of base message.
	    CProc,	//[CFnRRff____]: Procedure (RR - doesn't used). ff sets to register for for F < 0
	    CFunc,	//[CFnRRff____]: Function. ff sets to register for for F < 0
	    CFuncObj,	//[COOlnRR____]: Object's function for <OO> object's register, function name <l> length, arguments number <n>, result <RR>
			//		and first same name and arguments into {__}.
	    IFuncDef,	//[CNNnRR____]: Internal function's header definition where:
			//		{NN} - code size, {n} - arguments number, {__} - argument's variables, {RR} - return
	    IFunc	//[CNNnRR____]: Internal function call into the absolute position {NN} and for {n} arguments {__} and return into {RR}.
	};

	union El {
	    char		b;	//Boolean for constant and local variable
	    int64_t		i;	//Integer for constant and local variable
	    double		r;	//Real for constant and local variable
	    string		*s;	//String for constant and local variable
	    AutoHD<TVarObj>	*o;	//Object for constant and local variable
	    int			io;	//IO id for IO variable
	    AutoHD<TVal>	*pA;	//Parameter attribute
	    AutoHD<TFunction>	*f;	//Dynamic linked external function
	};

	//Methods
	Reg( ) : mPos(-1), mObjEl(false), mLock(false), 
#ifdef IN_F_SHARE_REGS
	    mInFnc(false),
#endif
	    mTp(Free)	{  }
	Reg( int ipos ) : mPos(ipos), mObjEl(false), mLock(false),
#ifdef IN_F_SHARE_REGS
	    mInFnc(false),
#endif
	    mTp(Free)	{  }
	Reg( const Reg &src ) : mPos(-1), mObjEl(false), mLock(false),
#ifdef IN_F_SHARE_REGS
	    mInFnc(false),
#endif
	    mTp(Free)	{ operator=(src); }
	~Reg( );

	Reg &operator=( const Reg &irg );
	void operator=( bool ivar )		{ setType(Bool);   el.b = ivar;  }
	void operator=( char ivar )		{ setType(Bool);   el.b = ivar;  }
	void operator=( int ivar )		{ setType(Int);	   el.i = ivar;  }
	void operator=( int64_t ivar )		{ setType(Int);	   el.i = ivar;  }
	void operator=( double ivar )		{ setType(Real);   el.r = ivar;  }
	void operator=( const string &ivar )	{ setType(String); *el.s = ivar; }
	void operator=( AutoHD<TVarObj> ivar )	{ setType(Obj);	   *el.o = ivar; }

	string name( ) const			{ return mNm; }
	Type type( ) const			{ return mTp; }
	Type vType( Func *fnc );
	int pos( )				{ return mPos; }
	bool lock( )				{ return mLock; }
	bool objEl( )				{ return mObjEl; }

	void setName( const string &nm )	{ mNm = nm; }
	void setType( Type tp );
	void setLock( bool vl )			{ mLock = vl; }
	void setObjEl( )			{ mObjEl = true; }
	void setVar( int ivar )			{ setType(Var); el.io = ivar; }
	void setPAttr( const AutoHD<TVal> &ivattr )	{ setType(PrmAttr); *el.pA = ivattr; }

#ifdef IN_F_SHARE_REGS
	bool inFnc( ) const			{ return mInFnc; }
	void setInFnc( bool vl )		{ mInFnc = vl; }
#else
	string inFnc( ) const			{ return mInFnc; }
	void setInFnc( const string &vl )	{ mInFnc = vl; }
#endif

	void free( );

	El &val( )				{ return el; }

    private:
	//Attributes
	int	mPos;
	string	mNm;
	unsigned mObjEl :1,	//Object's element
		 mLock  :1;	//Locked register
#ifdef IN_F_SHARE_REGS
	unsigned mInFnc :1;	//In internal function context
#else
	string	mInFnc;		//In internal function context
#endif

	Type	mTp;
	El	el;
};

//*************************************************
//* RegW : Work register                          *
//*************************************************
class RegW
{
    public:
	RegW( );
	RegW( const RegW &iRW );
	~RegW( );

	RegW &operator=( const RegW &iRW );
	void operator=( char ivar )		{ setType(Reg::Bool);	el.b = ivar; }
	void operator=( int ivar )		{ setType(Reg::Int);	el.i = ivar; }
	void operator=( int64_t ivar )		{ setType(Reg::Int);	el.i = ivar; }
	void operator=( double ivar )		{ setType(Reg::Real);	el.r = ivar; }
	void operator=( const string &ivar )	{ setType(Reg::String);	*el.s = ivar;}
	void operator=( AutoHD<TVarObj> ivar )	{ setType(Reg::Obj);	*el.o = ivar;}
	void operator=( const TVariant &ivar );

	Reg::Type type( ) const			{ return mTp; }
	Reg::Type vType( Func *fnc );
	bool vConst( )				{ return mConst; }
	void setType( Reg::Type tp );
	void setVConst( )			{ mConst = true; }

	Reg::El &val( )				{ return el; }

	vector<string> &props( )		{ return mPrps; }

    private:
	Reg::Type	mTp;
	Reg::El		el;
	vector<string>	mPrps;
	bool		mConst;		//Constant, prevent write
};

//*************************************************
//* Func: Function                                *
//*************************************************
class Lib;

class Func : public TConfig, public TFunction
{
    friend int yylex();
    friend int yyparse();
    friend void yyerror( const char* );
    public:
	// Addition flags for IO
	enum IOSpecFlgs {
	    SysAttr	= 0x10	//Lock attribute
	};

	//Attributes
	Func( const string &id, const string &name = "" );
	Func( const Func &func );
	~Func( );

	TCntrNode &operator=( const TCntrNode &node );

	Func &operator=( const Func &func );

	string	name( );
	string	descr( )		{ return cfg("DESCR").getS(); }
	string	stor( ) const;
	bool	toStart( )		{ return cfg("START"); }
	int maxCalcTm( )		{ return mMaxCalcTm; }
	bool	progTr( )		{ return cfg("PR_TR"); }
	string	prog( )			{ return cfg("FORMULA").getS(); }
	const string &usings( )		{ return mUsings; }
	int64_t	timeStamp( )		{ return mTimeStamp; }
	unsigned cnstStatLim( )		{ return 32767; }

	void setName( const string &nm );
	void setDescr( const string &dscr );
	void setToStart( bool vl )	{ cfg("START") = vl; }
	void setMaxCalcTm( int vl );
	void setProgTr( bool vl )	{ cfg("PR_TR") = vl; }
	void setProg( const string &prg );
	void setStart( bool val );
	void setUsings( const string &val )	{ mUsings = val; }

	void del( );

	void calc( TValFunc *val );

	void valAtt( TValFunc *vfnc );
        void valDet( TValFunc *vfnc );

	// External functions
	int funcGet( const string &path );

	// Internal functions
	int inFuncGet( const string &nm );
	void inFuncDef( const string &nm, int pos );

	// Registers' list functions
	int regNew( bool sep = false, int recom = -1, bool inFncNS = true );
	int regGet( const string &nm, bool inFncNS = false );
	int ioGet( const string &nm );
	Reg *regAt( int id )	{ return (id>=0) ? mRegs.at(id) : NULL; }
	void buildClear( );
	void workClear( );
	void regClear( );

	// Temporary registers' list functions
	Reg *regTmpNew( );
	void regTmpClear( );

	// Parse function
	void progCompile( );

	// Code functions
	Reg *cdTypeConv( Reg *opi, Reg::Type tp, bool no_code = false );
	Reg *cdMvi( Reg *op, bool no_code = false );
	Reg *cdMviObject( );
	Reg *cdMviArray( int p_cnt );
	Reg *cdMviRegExp( int p_cnt );
	void cdAssign( Reg *rez, Reg *op );
	Reg *cdMove( Reg *rez, Reg *op, bool force = true );
	Reg *cdBinaryOp( Reg::Code cod, Reg *op1, Reg *op2, Reg *rez = NULL );
	Reg *cdCondBinaryOp( int p_cmd, Reg *op1, Reg *op2, int p_end );
	Reg *cdUnaryOp( Reg::Code cod, Reg *op );
	Reg *cdCond( Reg *cond, int p_cmd, int p_else, int p_end, Reg *thn = NULL, Reg *els = NULL);
	void cdCycle(int p_cmd, Reg *cond, int p_solve, int p_end, int p_postiter );
	void cdCycleObj(int p_cmd, Reg *cond, int p_solve, int p_end, Reg *var );
	Reg *cdBldFnc( int f_id, Reg *prm1 = NULL, Reg *prm2 = NULL );
	Reg *cdExtFnc( int f_id, int p_cnt, bool proc = false, Reg *f_r = NULL );
	Reg *cdIntFnc( int fOff, int pCnt, bool proc = false );
	Reg *cdObjFnc( Reg *obj, const string &fNm, int p_cnt );
	Reg *cdProp( Reg *obj, const string &sprp, Reg *dprp = NULL );

	// Properties and functions for base object's process
	TVariant oPropGet( TVariant vl, const string &prop );
	TVariant oFuncCall( TVariant &vl, const string &prop, vector<TVariant> &parms );

	// Variable access
	TVariant getVal( TValFunc *io, RegW &rg, bool fObj = false );
	string	getValS( TValFunc *io, RegW &rg );
	int64_t	getValI( TValFunc *io, RegW &rg );
	char	getValB( TValFunc *io, RegW &rg );
	double	getValR( TValFunc *io, RegW &rg );
	AutoHD<TVarObj>	getValO( TValFunc *io, RegW &rg );

	void setVal( TValFunc *io, RegW &rg, const TVariant &val );
	void setValS( TValFunc *io, RegW &rg, const string &val );
	void setValI( TValFunc *io, RegW &rg, int64_t val );
	void setValR( TValFunc *io, RegW &rg, double val );
	void setValB( TValFunc *io, RegW &rg, char val );
	void setValO( TValFunc *io, RegW &rg, AutoHD<TVarObj> val );

	// IO operations
	void ioAdd( IO *io );
	void ioIns( IO *io, int pos );
	void ioDel( int pos );
	void ioMove( int pos, int to );

	Lib &owner( ) const;

    protected:
	//Data
	struct ExecData {
	    time_t	startTm;	//Start time
	    unsigned char flg;		//0x01 - recursive exit stat;
					//0x02 - break operator flag;
					//0x04 - continue operator flag;
					//0x08 - error flag
#ifdef IN_F_SHARE_REGS
	    vector< vector<RegW> > inFShareRegs;	//Shared registers stack of the internal functions
#else
	    map<int, vector< vector<RegW> > > inFRegs;	//Shared registers stack of the internal functions per function
#endif
	};

	//Methods
	void postEnable( int flag );
	void postDisable( int flag );
	bool cfgChange( TCfg &co, const TVariant &pc );
	void cntrCmdProc( XMLNode *opt );	//Control interface command process

	void load_( TConfig *cfg );
	void save_( );

	void loadIO( );
	void saveIO( );
	void delIO( );

	void exec( TValFunc *val, const uint8_t *cprg, ExecData &dt );

	void workRegControl( TValFunc *vfnc, bool toFree = false );

    private:
	//Attributes
	int64_t	&mMaxCalcTm,
		&mTimeStamp;

	// Parser's data
	string		sprg, prg;	//Built program
	unsigned	laPos;		//LA position
	string		pErr;		//Parse error
	string		mUsings,	//External functions usings namespaces
			mInFnc;		//Current internal function namespace
	vector<AutoHD<TFunction> > mFncs;//Static external functions list in action
	map<string, int> mInFncs;	//Internal functions list in compile
	vector<Reg*>	mRegs;		//Registers list in action
	vector<Reg*>	mTmpRegs;	//Constant temporary list
	deque<Reg*>	fPrmst;		//Function's parameters stack
#ifdef IN_F_SHARE_REGS
	vector<int>	mShareRegs;	//Shared registers to store the internal functions context
#else
	map<int, vector<int> >	mInFncRegs; //Input functions' personal registers list
#endif
	ResRW		&parseRes;
	unsigned	cntrInF		: 8,
			cntrCnst	: 16,
			cntrInFRegs	: 16;
};

extern Func *pF;

} //End namespace StatFunc

#endif //FREEFUNC_H
