/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

typedef unsigned short int WORD;
typedef unsigned char BYTE;

#include <string>
#include <vector>

#include <tfunctions.h>
#include <tconfig.h>

using std::string;
using std::vector;

namespace FreeFunc
{
//Bison parse function
int yyparse( );

//=======================================================
//============= Symbol table element ====================
//=======================================================
class Func;

class Symbol
{
    public:
        enum Type { Undef, Var, Func, Check };
	
        struct SVar
        {
            int    io_id;       // if < 0 then internal variable
            double val;         // Value for internal variable
        };
	struct SFunc
        {
            AutoHD<TFunction> 	func;
	    TValFunc    	fval;
        };
        union Value
	{
            SVar  *var;   //Variable
            SFunc *fnc;   //Function	    
        };

    public:	
	Symbol( const char *name, FreeFunc::Func *afnc );
	~Symbol( );

	string &name() 	{ return m_name; }
	Type type() 	{ return m_tp; }

	void  type( Type tp );
	const Value &val()	{ return m_val; }
	
    private:
	string 	m_name;
	Type	m_tp;
	Value 	m_val;

	FreeFunc::Func	*fnc;
};

//=========================================================
//============= Programm stack element ====================
//=========================================================
class StkEl
{
    public:
        enum Type { Const, Var };	
	enum Code 	//Byte codes
	{
	    PushV  = 'X',   	//0x58: [Xnn]: Push variable <nn> to stack. nn - symbol number.
	    PushR  = 'R',	//0x52: [R____]: Rush float value to stack.
	    OpAdd  = '+',	//0x2B: [+]: Add two element from stack and push result.
	    OpSub  = '-',	//0x2D: [-]: Sub two element from stack and push result.
	    OpMul  = '*',	//0x2A: [*]: Mul two element from stack and push result.
	    OpDiv  = '/',	//0x2F: [/]: Divide two element from stack and push result.
	    OpPow  = '^',	//0x5E: [^]: Power two element from stack and push result.
	    OpNeg  = '_',	//0x5F: [_]: Negative element into stack.
	    OpOR   = ';',	//0x3B: [;]: Logical or.
	    OpAND  = ',',	//0x2C: [,]: Logical and.
	    OpGT   = '>',	//0x3E: [>]: Logical great.
	    OpGE   = 0xFF,	//0xFF: [>=]: Logical great equale.
	    OpLT   = '<',	//0x3C: [<]: Logical least.
	    OpLE   = 0xFE,	//0xFE: [<=]: Logical least equale.
	    OpEQ   = 0xFD,	//0xFD: [==]: Logical equale.
	    OpNE   = 0xFC,	//0xFC: [!=]: Logical not equale.
	    AddEq  = 0xFB,	//0xFB: [+=]: Add and equale.
	    SubEq  = 0xFA,      //0xFB: [-=]: Sub and equale.
	    MulEq  = 0xF9,      //0xFB: [*=]: Mul and equale.
	    DivEq  = 0xF8,      //0xFB: [/=]: Div and equale.
	    OpNot  = '!',	//0x21: [!]: Logical not.
	    Assign = '=',	//0x3D: [=nn]: Assign variable <nn> to value from stack and push variable.
	    PopEl  = 'p',	//0x70: [p]: Pop element from stack.
	    Proc   = 'P',	//0x50: [Pnnp]: Call procedure <nn> with <p> parameters from stack.
	    Func   = 'F',	//0x46: [Fnnp]: Call function <nn> with <p> parameters from stack. Result push stack
	    If	   = 'i',	//0x69: [inneeff]: Construction [if(cond) <nn> else <ee> <ff>]
	    IfExpr = '?',   	//0x3F: [?eeff]: Construction [<cond> ? <then> : <ee> <ff>   
	    End	   = 'E'	//0x45: [E]: End programm.	    
	};
	
	StkEl( float val ) 	{ type = Const; var.v_r = val; }
	StkEl( Symbol *symb )	{ type = Var; var.symb = symb; }

	Type type;
	union
	{
       	    double  	v_r;	//Const real
	    Symbol 	*symb;	//Variable symbol element
	}var;	
};
        
class NConst
{
    public:
	NConst( const char *inm, double ival ) : name(inm), val(ival) { }
	
	string name;
	double val;
};

//Function
class Lib;

class Func : public TConfig, public TFunction
{
    friend int yylex( );
    friend int yyparse( );
    friend void yyerror(char*);
    public:    
        Func( const char *, Lib *own, const char *name = "" );
        ~Func();	
	void postDisable(int flag);
	
	Func &operator=(Func &func);
	    
        string name()	{ return m_name; }
        string descr()	{ return m_descr; }
	
	void name( const char *nm )	{ m_name = nm; }
	
	void start( bool val );
	
	void load( );
        void save( );
	void del( );
	
        void calc( TValFunc *val );

	void chID( const char *id );

	Lib &owner();

	//Named constant
	NConst *constGet( const char *nm );	

	//Symbol's table functions
	int symbSize( ) 	{ return m_smb.size(); }
	Symbol *symbAt( int id ){ return m_smb[id]; }
	int symbGet( const char *nm );
	void symbClean();

	//Parse function
	void parseProg();

	//Variable access
	float getValR( TValFunc *io, StkEl stkel );
	void setValR( TValFunc *io, StkEl stkel, float val );

    protected:
	void cntrCmd_( const string &a_path, XMLNode *opt, int cmd );
	
	void loadIO( );
	void saveIO( );
	void delIO( );
	
	void exec( TValFunc *val, vector<StkEl> &stk, const BYTE *stprg, const BYTE *cprg );

    private:
	string 	&m_name;
	string 	&m_descr;
	string	&prg_src;

	int     la_pos;			//LA position
	string	p_err;			//Parse error
	vector<NConst> 	m_const;	//Name constant table
	vector<Symbol *> m_smb;		//Symbols table. Contein variables and functions
	string  prg;			//Work programm
	
	int     &parse_res;	
	Lib	*m_owner;	
};				    

extern Func *p_fnc;

} //End namespace StatFunc

#endif //FREEFUNC_H

