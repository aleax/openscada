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

#ifndef STDMATH_H
#define STDMATH_H

#include <math.h>
#include <string>
#include <vector>

#include <tfunctions.h>

using std::string;
using std::vector;

namespace StatFunc
{


//Standart mathematic functions:

//------------------------------------------------------------------------------------
//Arc cosine
//Formula: y=acos(x)
//------------------------------------------------------------------------------------
class MathAcos : public TFunction
{
    public:
	MathAcos() : TFunction("acos")
	{
	    ioAdd( new IO("y","Y(rad.)",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "ArcCos"; }
	string descr()	{ return "Arc cosine math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,acos(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Arc sine
//Formula: y=asin(x)
//------------------------------------------------------------------------------------
class MathAsin : public TFunction
{
    public:
	MathAsin() : TFunction("asin")
	{
	    ioAdd( new IO("y","Y(rad.)",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "ArcSin"; }
	string descr()	{ return "Arc sine math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,asin(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Arc tangent
//Formula: y=atan(x)
//------------------------------------------------------------------------------------
class MathAtan : public TFunction
{
    public:
	MathAtan() : TFunction("atan")
	{
	    ioAdd( new IO("y","Y(rad.)",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "ArcTan"; }
	string descr()	{ return "Arc tangent math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,atan(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Cosine
//Formula: y=cos(x)
//------------------------------------------------------------------------------------
class MathCos : public TFunction
{
    public:
	MathCos() : TFunction("cos")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X(rad.)",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Cosine"; }
	string descr()	{ return "Cosine math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,cos(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Cosine hyperbolical
//Formula: y=cosh(x)
//------------------------------------------------------------------------------------
class MathCosh : public TFunction
{
    public:
	MathCosh() : TFunction("cosh")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Cosine hyperbolical"; }
	string descr()	{ return "Cosine hyperbolical math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,cosh(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Exponent
//Formula: y=exp(x)
//------------------------------------------------------------------------------------
class MathExp : public TFunction
{
    public:
	MathExp() : TFunction("exp")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Exponent"; }
	string descr()	{ return "Exponent math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,exp(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Natural logarithm
//Formula: y=ln(x)
//------------------------------------------------------------------------------------
class MathLn : public TFunction
{
    public:
	MathLn() : TFunction("ln")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Natural logarithm"; }
	string descr()	{ return "Natural logarithm math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,log(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Decimal logarithm
//Formula: y=lg(x)
//------------------------------------------------------------------------------------
class MathLg : public TFunction
{
    public:
	MathLg() : TFunction("lg")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Decimal logarithm"; }
	string descr()	{ return "Decimal logarithm math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,log10(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Power
//Formula: y=pow(x,p)
//------------------------------------------------------------------------------------
class MathPow : public TFunction
{
    public:
	MathPow() : TFunction("pow")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	    ioAdd( new IO("p","Power",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Power"; }
	string descr()	{ return "Power math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,pow(val->getR(1),val->getR(2)));
	}
};

//------------------------------------------------------------------------------------
//Sine
//Formula: y=sin(x)
//------------------------------------------------------------------------------------
class MathSin : public TFunction
{
    public:
	MathSin() : TFunction("sin")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X(rad.)",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Sine"; }
	string descr()	{ return "Sine math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,sin(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Sine hyperbolical
//Formula: y=sinh(x)
//------------------------------------------------------------------------------------
class MathSinh : public TFunction
{
    public:
	MathSinh() : TFunction("sinh")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Sine hyperbolical"; }
	string descr()	{ return "Sine hyperbolical math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,sinh(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Square root
//Formula: y=sqrt(x)
//------------------------------------------------------------------------------------
class MathSqrt : public TFunction
{
    public:
	MathSqrt() : TFunction("sqrt")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Square root"; }
	string descr()	{ return "Square root math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,sqrt(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Tangent
//Formula: y=tan(x)
//------------------------------------------------------------------------------------
class MathTan : public TFunction
{
    public:
	MathTan() : TFunction("tan")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X(rad.)",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Tangent"; }
	string descr()	{ return "Tangent math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,tan(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Tangent hyperbolical
//Formula: y=tanh(x)
//------------------------------------------------------------------------------------
class MathTanh : public TFunction
{
    public:
	MathTanh() : TFunction("tanh")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Tangent hyperbolical"; }
	string descr()	{ return "Tangent hyperbolical math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,tanh(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Ceil
//Formula: y=ceil(x)
//------------------------------------------------------------------------------------
class MathCeil : public TFunction
{
    public:
	MathCeil() : TFunction("ceil")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Ceil"; }
	string descr()	{ return "Ceil math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,ceil(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Absolute
//Formula: y=abs(x)
//------------------------------------------------------------------------------------
class MathAbs : public TFunction
{
    public:
	MathAbs() : TFunction("abs")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Absolute"; }
	string descr()	{ return "Absolute math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,fabs(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Floor
//Formula: y=floor(x)
//------------------------------------------------------------------------------------
class MathFloor : public TFunction
{
    public:
	MathFloor() : TFunction("floor")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x","X",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "Floor"; }
	string descr()	{ return "Floor math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,floor(val->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Randomize (0..100)
//Formula: y=rand(x)
//------------------------------------------------------------------------------------
class MathRand : public TFunction
{
    public:
	MathRand() : TFunction("rand")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	}
	
	string name()	{ return "Rand"; }
	string descr()	{ return "Rand math function."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,100.*rand()/RAND_MAX);
	}
};

//------------------------------------------------------------------------------------
//Condition check 
//Formula: y=if(cond,true,false)
//------------------------------------------------------------------------------------
class MathIf : public TFunction
{
    public:
	MathIf() : TFunction("if")
	{
	    ioAdd( new IO("y","Y",IO::Real,IO::Return,"0") );
	    ioAdd( new IO("cond","Condition",IO::Real,IO::Input,"1") );
	    ioAdd( new IO("true","True value",IO::Real,IO::Input,"0") );
	    ioAdd( new IO("false","False value",IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return "IF condition"; }
	string descr()	{ return "Function's type 'if'-condition."; }     

	void calc( TValFunc *val )
	{
	    val->setR(0,val->getR(1)?val->getR(2):val->getR(3));
	}
};


//=============================================================
//========== Standart mathematic functions library ============
//=============================================================
class StdMath : public TLibFunc
{
    public:
	StdMath( ) : TLibFunc("math")
	{
	    reg( new MathAcos() );
	    reg( new MathAsin() );
	    reg( new MathAtan() );
	    reg( new MathCos() );
	    reg( new MathCosh() );
	    reg( new MathExp() );
	    reg( new MathLn() );
	    reg( new MathLg() );
	    reg( new MathPow() );
	    reg( new MathSin() );
	    reg( new MathSinh() );
	    reg( new MathSqrt() );
	    reg( new MathTan() );
	    reg( new MathTanh() );
	    reg( new MathCeil() );
	    reg( new MathAbs() );
	    reg( new MathFloor() );
	    reg( new MathRand() );
	    reg( new MathIf() );
	}
	
	string name() 	{ return "Mathematic functions"; }
	string descr()	{ return "Allow standart mathematic functions."; }    
};

}

#endif //STDMATH_H

