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

#ifndef LIBCOMPL1_H
#define LIBCOMPL1_H

#include <string>
#include <vector>

#include <tfunctions.h>

using std::string;
using std::vector;

namespace StatFunc
{

//Complex1 functions library
class Complex1 : public TLibFunc
{
    public:
	Complex1( );
	~Complex1();
	
	string name();
	string descr();    
};

//Complex1 functions:

//------------------------------------------------------------------------------------
//Fastener block
//Support fasteners: 0-Open;1-Start;2-Enable;3-Norm;4-Block
//------------------------------------------------------------------------------------
class DigitBlock : public TFunction
{
    public:
	DigitBlock();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//Simple summator
//Formula: out=in1_1*in1_2+in2_1*in2_2+in3_1*in3_2+in4_1*in4_2+
//             in5_1*in5_2+in6_1*in6_2+in7_1*in7_2+in8_1*in8_2;
//------------------------------------------------------------------------------------
class Sum : public TFunction
{
    public:
	Sum();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//Simple moltiplicator
//Formula: out=(in1_1*in1_2*in1_3*in1_4*in1_5*in1_6)/(in2_1*in2_2*in2_3*in2_4);
//------------------------------------------------------------------------------------
class Mult : public TFunction
{
    public:
	Mult();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//Sumator + moltiplicator
//Formula: out=(in1_1*in1_2*in1_3*in1_4*in1_5*(in2_1*in2_2*in2_3*in2_4*in2_5+"
//                      (in3_1*in3_2*in3_3*in3_4*in3_5)/(in4_1*in4_2*in4_3*in4_4*in4_5)))
//------------------------------------------------------------------------------------
class MultDiv : public TFunction
{
    public:
	MultDiv();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//Expanenta
//Formula: out=exp (in1_1*in1_2*in1_3*in1_4*in1_5 +
//                  (in2_1*in2_2*in2_3*in2_4*in2_5+in3) / (in4_1*in4_2*in4_3*in4_4*in4_5+in5) )
//------------------------------------------------------------------------------------
class Exp : public TFunction
{
    public:
	Exp();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//Power
//Formula: out=(in1_1*in1_2*in1_3*in1_4*in1_5)^(in2_1*in2_2*in2_3*in2_4*in2_5 +
//                      (in3_1*in3_2*in3_3*in3_4*in3_5)/(in4_1*in4_2*in4_3*in4_4*in4_5))
//------------------------------------------------------------------------------------
class Pow : public TFunction
{
    public:
	Pow();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//Condition <
//Formula: out=if( in1<(in2_1*in2_2*in2_3*in2_4) ) then in3_1*in3_2*in3_3*in3_4;
//             else in4_1*in4_2*in4_3*in4_4;
//------------------------------------------------------------------------------------
class Cond1 : public TFunction
{
    public:
	Cond1();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//Condition >
//Formula: out=if( in1>(in2_1*in2_2*in2_3*in2_4) ) then in3_1*in3_2*in3_3*in3_4;
//             else in4_1*in4_2*in4_3*in4_4;
//------------------------------------------------------------------------------------
class Cond2 : public TFunction
{
    public:
	Cond2();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//Condition x>, x<, >x>
//Formula: out = if( in1<(in2_1*in2_2*in2_3*in2_4) )    then in3_1*in3_2*in3_3*in3_4;
//               else if( in1>(in4_1*in4_2*in4_3*in4_4) then in5_1*in5_2*in5_3*in5_4;
//               else in6_1*in6_2*in6_3*in6_4;
//------------------------------------------------------------------------------------
class Cond3 : public TFunction
{
    public:
	Cond3();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//Selector
//Formula: out = if( sel = 1 )  then in1_1*in1_2*in1_3*in1_4;
//               if( sel = 2 )  then in2_1*in2_2*in2_3*in2_4;
//               if( sel = 3 )  then in3_1*in3_2*in3_3*in3_4;
//               if( sel = 4 )  then in4_1*in4_2*in4_3*in4_4;
//------------------------------------------------------------------------------------
class Select : public TFunction
{
    public:
	Select();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//Incrementator
//Formula: out = if( in > prev )then prev + k_pos*(in-prev); else prev - k_neg*(prev-in);
//------------------------------------------------------------------------------------
class Increm : public TFunction
{
    public:
	Increm();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//Divider
//Formula: out = (in1_1*in1_2*in1_3*in1_4*in1_5 + in2_1*in2_2*in2_3*in2_4*in2_5 + in3) / 
//		 (in4_1*in4_2*in4_3*in4_4*in4_5 + in5_1*in5_2*in5_3*in5_4*in5_5 + in6);
//------------------------------------------------------------------------------------
class Divider : public TFunction
{
    public:
	Divider();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

//------------------------------------------------------------------------------------
//PID regulator
//------------------------------------------------------------------------------------
class PID : public TFunction
{
    public:
	PID();
	
	string name();
	string descr();    

	void calc( TValFunc *val );	
};

} //End namespace virtual

#endif //LIBCOMPL1_H

