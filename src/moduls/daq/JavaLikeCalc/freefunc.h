
//OpenSCADA system module DAQ.JavaLikeCalc file: freefunc.h
/***************************************************************************
 *   Copyright (C) 2005-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#include <string>
#include <vector>
#include <deque>

#include <tfunction.h>
#include <tconfig.h>

using std::string;
using std::vector;
using std::deque;

namespace JavaLikeCalc
{

//Bison parse function
int yyparse( );

//*************************************************
//* UFunc: Using func list element                *
//*************************************************
class UFunc
{
    public:
	//Methods
	UFunc( const string &path ) : m_path(path)
	{
	    if( dynamic_cast<TFunction *>(&SYS->nodeAt(path,0,'.').at()) )
		m_func = SYS->nodeAt(path,0,'.');
	}
	const string &path( )		{ return m_path; }
	AutoHD<TFunction> &func( )	{ return m_func; }

    private:
	//Attributes
	string 		m_path;
	AutoHD<TFunction> m_func;
};

//*************************************************
//* Reg: Compile register                         *
//*************************************************
class Func;

class Reg
{
    public:
	//Data
	enum Type
	{
	    Free,	//Free
	    Bool,	//Boolean
	    Int,	//Integer
	    Real,	//Real
	    String,	//String
	    Var,	//IO variable
	    PrmAttr	//Parameter attribute
	};

	enum Code	//Byte codes
	{
	    End,	//[E]: End programm.
	    EndFull,	//[E]: Full end from programm.
	    MviB,	//[CRRB]: Load boolean <B> to register <R>.
	    MviI,	//[CRR____]: Load integer <____> to register <R>.
	    MviR,	//[CRR______]: Load real <______> to register <R>.
	    MviS,	//[CRRn_____]: Load string len <n> to to register <R>.
	    AssB,	//[CRRrr]: Assign bool from register to register.
	    AssI,	//[CRRrr]: Assign integer from register to register.
	    AssR,	//[CRRrr]: Assign real from register to register.
	    AssS,	//[CRRrr]: Assign string from register to register.
	    MovB,	//[CRRrr]: Move bool from register to register.
	    MovI,	//[CRRrr]: Move integer from register to register.
	    MovR,	//[CRRrr]: Move real from register to register.
	    MovS,	//[CRRrr]: Move string from register to register.
	    AddR,	//[CRRrrRR]: Real add.
	    AddS,	//[CRRrrRR]: String add.
	    Sub,	//[CRRrrRR]: Real subtract.
	    Mul,	//[CRRrrRR]: Real multiply.
	    Div,	//[CRRrrRR]: Real divide.
	    RstI,	//[CRRrrRR]: Integer divide rest.
	    BitOr,	//[CRRrrRR]: Integer bit or.
	    BitAnd,	//[CRRrrRR]: Integer bit and.
	    BitXor,	//[CRRrrRR]: Integer bit xor.
	    BitShLeft,	//[CRRrrRR]: Integer bit shift left.
	    BitShRight,	//[CRRrrRR]: Integer bit shift right.
	    LOr,	//[CRRrrRR]: Boolean OR.
	    LAnd,	//[CRRrrRR]: Boolean AND.
	    LT,		//[CRRrrRR]: Real least.
	    GT,		//[CRRrrRR]: Real great.
	    LER,	//[CRRrrRR]: Real least equal.
	    GER,	//[CRRrrRR]: Real great equal.
	    EQR,	//[CRRrrRR]: Real equal.
	    EQS,	//[CRRrrRR]: String equal.
	    NER,	//[CRRrrRR]: Real no equal.
	    NES,	//[CRRrrRR]: String no equal.
	    Not,	//[CRRrr]: Boolean not.
	    BitNot,	//[CRRrr]: Integer bit not.
	    Neg,	//[CRRrr]: Negate real.
	    If,		//[CRR00nn]: Construction [if(R)  else <00>; <nn>]
	    Cycle,	//[CRRbbaann]: Cycles construction [for(<first_init>;R=<cond>;aa)<bb>;<nn>] [while(R=<cond>)<bb>;<nn>]
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
	    CProc,	//[CFnRR____]: Procedure (RR - don't used).
	    CFunc	//[CFnRR____]: Function.
	};

	union El
	{
	    char	b_el;	//Boolean for constant and local variable
	    int		i_el;	//Integer for constant and local variable
	    double	r_el;	//Real for constant and local variable
	    string	*s_el;	//String for constant and local variable
	    int		io;	//IO id for IO variable
	    AutoHD<TVal>*p_attr;//Parameter attribute
	};

	//Methods
	Reg( ) : m_tp(Free), m_lock(false), m_nm(NULL), m_pos(-1) {  }
	Reg( int ipos ) : m_tp(Free), m_lock(false), m_nm(NULL), m_pos(ipos) {  }
	~Reg( );

	Reg &operator=( Reg &irg );
	void operator=( char ivar )		{ setType(Bool);	el.b_el = ivar; }
	void operator=( int ivar )		{ setType(Int);		el.i_el = ivar; }
	void operator=( double ivar )		{ setType(Real);	el.r_el = ivar; }
	void operator=( const string &ivar )	{ setType(String);	*el.s_el = ivar;}

	string name( ) const;
	Type type( ) const			{ return m_tp; }
	Type vType( Func *fnc );
	int pos( )				{ return m_pos; }
	bool lock( )				{ return m_lock; }

	void setName( const char *nm );
	void setType( Type tp );
	void setLock( bool vl )			{ m_lock = vl; }
	void setVar( int ivar )			{ setType(Var);	el.io = ivar; }
	void setPAttr( const AutoHD<TVal> &ivattr )	{ setType(PrmAttr); *el.p_attr = ivattr; }

	void free( );

	El &val( )				{ return el; }

    private:
	//Attributes
	int	m_pos;
	string	*m_nm;
	bool	m_lock;	//Locked register
	Type	m_tp;
	El	el;
};

//*************************************************
//* RegW : Work register                          *
//*************************************************
class RegW
{
    public:
	RegW( ) : m_tp(Reg::Free)		{  }
	~RegW( )				{ setType(Reg::Free); }

	void operator=( char ivar )		{ setType(Reg::Bool);	el.b_el = ivar; }
	void operator=( int ivar )		{ setType(Reg::Int); 	el.i_el = ivar; }
	void operator=( double ivar )		{ setType(Reg::Real); 	el.r_el = ivar; }
	void operator=( const string &ivar )	{ setType(Reg::String);	*el.s_el = ivar;}

	Reg::Type type( ) const			{ return m_tp; }
	void setType( Reg::Type tp )
	{
	    if( m_tp == tp )    return;
	    //Free old type
	    if( m_tp == Reg::String )		delete el.s_el;
	    else if( m_tp == Reg::PrmAttr )	delete el.p_attr;
	    //Set new type
	    if( tp == Reg::String )		el.s_el = new string;
	    else if( tp == Reg::PrmAttr )	el.p_attr = new AutoHD<TVal>;
	    m_tp = tp;	
	}
	
	Reg::El &val( )				{ return el; }

    private:
	Reg::Type	m_tp;
	Reg::El		el;
};

//*************************************************
//* Func: Function                                *
//*************************************************
class Lib;

class Func : public TConfig, public TFunction
{
    friend int yylex( );
    friend int yyparse( );
    friend void yyerror(const char*);
    public:
	//> Addition flags for IO
	enum IOSpecFlgs
	{
	    SysAttr	= 0x10	//Lock attribute
	};

	//Attributes
	Func( const char *, const char *name = "" );
	~Func( );

	TCntrNode &operator=( TCntrNode &node );

	Func &operator=( Func &func );

	string name( );
	string descr( )			{ return mDescr; }
	int maxCalcTm( )		{ return max_calc_tm; }
	const string &prog( )		{ return prg_src; }
	const string &usings( )		{ return mUsings; }

	void setName( const string &nm );
	void setDescr( const string &dscr );
	void setMaxCalcTm( int vl );
	void setProg( const string &prg );
	void setStart( bool val );
	void setUsings( const string &val )	{ mUsings = val; }

	void del( );

	void calc( TValFunc *val );

	void preIOCfgChange( );
	void postIOCfgChange( );

	//- Functins` list functions -
	int funcGet( const string &path );
	UFunc *funcAt( int id )	{ return mFncs.at(id); }
	void funcClear( );

	//- Registers` list functions -
	int regNew( bool var = false );
	int regGet( const char *nm );
	Reg *regAt( int id )	{ return (id>=0) ? mRegs.at(id) : NULL; }
	void regClear( );

	//- Temporary registers` list functions -
	Reg *regTmpNew( );
	void regTmpClean( );

	//- Parse function -
	void progCompile( );

	//- Code functions -
	Reg *cdTypeConv( Reg *opi, Reg::Type tp, bool no_code = false );
	Reg *cdMvi( Reg *op, bool no_code = false );
	void cdAssign( Reg *rez, Reg *op );
	Reg *cdMove( Reg *rez, Reg *op );
	Reg *cdBinaryOp( Reg::Code cod, Reg *op1, Reg *op2 );
	Reg *cdUnaryOp( Reg::Code cod, Reg *op );
	Reg *cdCond( Reg *cond, int p_cmd, int p_else, int p_end, Reg *thn = NULL, Reg *els = NULL);
	void cdCycle(int p_cmd, Reg *cond, int p_solve, int p_end, int p_postiter );
	Reg *cdBldFnc( int f_id, Reg *prm1 = NULL, Reg *prm2 = NULL);
	Reg *cdExtFnc( int f_id, int p_cnt, bool proc = false );

	//- Variable access -
	string	getValS( TValFunc *io, RegW &rg );
	int	getValI( TValFunc *io, RegW &rg );
	char	getValB( TValFunc *io, RegW &rg );
	double	getValR( TValFunc *io, RegW &rg );

	void setValS( TValFunc *io, RegW &rg, const string &val );
	void setValI( TValFunc *io, RegW &rg, int val );
	void setValR( TValFunc *io, RegW &rg, double val );
	void setValB( TValFunc *io, RegW &rg, char val );

	//- IO operations -
	void ioAdd( IO *io );
	void ioIns( IO *io, int pos );
	void ioDel( int pos );
	void ioMove( int pos, int to );

	Lib &owner( );

    protected:
	//Data
	struct ExecData
	{
	    unsigned	com_cnt;	//Command counter;
	    time_t	start_tm;	//Start time
	    unsigned char flg;		//0x01 - recursive exit stat;
					//0x02 - break operator flag;
					//0x04 - continue operator flag;
	};

	//Methods
	void postEnable( int flag );
	void preDisable( int flag );
	void postDisable( int flag );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	void load_( );
	void save_( );

	void loadIO( );
	void saveIO( );
	void delIO( );

	void exec( TValFunc *val, RegW *reg, const ui8 *cprg, ExecData &dt );

    private:
	//Attributes
	string	&mName;
	string	&mDescr;
	int	&max_calc_tm;
	string	&prg_src;

	bool	be_start;		//Change structure check
	Res	calc_res;

	//- Parser's data -
	string		prg;		//Build prog
	int		la_pos;		//LA position
	string		p_err;		//Parse error
	string		mUsings;	//Functions usings namespaces
	vector<UFunc*>	mFncs;		//Work functions list
	vector<Reg*>	mRegs;		//Work registers list
	vector<Reg*>	mTmpRegs;	//Constant temporary list
	deque<Reg*>	f_prmst;	//Function's parameters stack
	Res		&parse_res;
};

extern Func *p_fnc;

} //End namespace StatFunc

#endif //FREEFUNC_H
