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

#include <math.h>

#include <string>
#include <vector>

#include <tfunctions.h>

#include "statfunc.h"

using std::string;
using std::vector;

namespace StatFunc
{

//Complex1 functions:

//------------------------------------------------------------------------------------
//DigitBlock
//Support digital blocks: 0-Open;1-Start;2-Enable;3-Norm;4-Block
//------------------------------------------------------------------------------------ 
class DigitBlock : public TFunction
{
    public:
	DigitBlock() : TFunction("digitBlock")
	{
	    //Inputs
	    ioAdd( new IO("cmdOpen",st_lib->I18N("Command \"Open\""),IO::Boolean,IO::Input,"0") );
	    ioAdd( new IO("cmdClose",st_lib->I18N("Command \"Close\""),IO::Boolean,IO::Input,"0") );
	    ioAdd( new IO("cmdStop",st_lib->I18N("Command \"Stop\""),IO::Boolean,IO::Input,"0") );

	    //Outputs
	    ioAdd( new IO("stOpen",st_lib->I18N("Stat \"Opened\""),IO::Boolean,IO::Output,"0") );
	    ioAdd( new IO("stClose",st_lib->I18N("Stat \"Closed\""),IO::Boolean,IO::Output,"0") );
	}
	
	string name()	{ return st_lib->I18N("Digital block"); }
	string descr()	{ return st_lib->I18N("Digital assemble block."); }	//!!!! make full description 

	void calc( TValFunc *val ){ }
};

//------------------------------------------------------------------------------------
//Simple summator
//Formula: out=in1_1*in1_2+in2_1*in2_2+in3_1*in3_2+in4_1*in4_2+
//             in5_1*in5_2+in6_1*in6_2+in7_1*in7_2+in8_1*in8_2;
//------------------------------------------------------------------------------------
class Sum : public TFunction
{
    public:
	Sum() : TFunction("sum")
	{
	    char id_buf[10], nm_buf[20];
    
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"1") );
	    for( int i_in=1; i_in <= 8; i_in++ )
	    {
		snprintf(id_buf,sizeof(id_buf),"in%d_1",i_in);
		snprintf(nm_buf,sizeof(nm_buf),st_lib->I18N("Input %d.1"),i_in);
		ioAdd( new IO(id_buf,nm_buf,IO::Real,IO::Input,"0") );
		snprintf(id_buf,sizeof(id_buf),"in%d_2",i_in);
		snprintf(nm_buf,sizeof(nm_buf),st_lib->I18N("Input %d.2"),i_in);
		ioAdd( new IO(id_buf,nm_buf,IO::Real,IO::Input,"0") );
	    }
	}
	
	string name()	{ return st_lib->I18N("Simple summator"); }
	string descr()	
	{ 
	    return st_lib->I18N("Simple summator per formula:\n"
		"out=in1_1*in1_2+in2_1*in2_2+in3_1*in3_2+in4_1*in4_2+\n"
		"    in5_1*in5_2+in6_1*in6_2+in7_1*in7_2+in8_1*in8_2;"); 
	}

	void calc( TValFunc *val )
	{
	    val->setR(0,
		val->getR(1)*val->getR(2)+
		val->getR(3)*val->getR(4)+
		val->getR(5)*val->getR(6)+
		val->getR(7)*val->getR(8)+
		val->getR(9)*val->getR(10)+
		val->getR(11)*val->getR(12)+
		val->getR(13)*val->getR(14)+
		val->getR(15)*val->getR(16));
	}
};

//------------------------------------------------------------------------------------
//Simple moltiplicator
//Formula: out=(in1_1*in1_2*in1_3*in1_4*in1_5*in1_6)/(in2_1*in2_2*in2_3*in2_4);
//------------------------------------------------------------------------------------
class Mult : public TFunction
{
    public:
	Mult() : TFunction("mult")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"1") );
	    for( int i_c = 1; i_c <= 6; i_c++ )
		ioAdd( new IO(("in1_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 1.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in2_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 2.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Simple multiplicator"); }
	string descr()	
	{ 
	    return st_lib->I18N("Simple moltiplicator per formula:\n"
	    	"out=(in1_1*in1_2*in1_3*in1_4*in1_5*in1_6)/(in2_1*in2_2*in2_3*in2_4);"); 
	}

	void calc( TValFunc *val )
	{
	    val->setR(0,
		(val->getR(1)*val->getR(2)*val->getR(3)*val->getR(4)*val->getR(5)*val->getR(6))/
		(val->getR(7)*val->getR(8)*val->getR(9)*val->getR(10)));
	}
};

//------------------------------------------------------------------------------------
//Multiplicator+divider
//Formula: out=(in1_1*in1_2*in1_3*in1_4*in1_5*(in2_1*in2_2*in2_3*in2_4*in2_5+"
//                      (in3_1*in3_2*in3_3*in3_4*in3_5)/(in4_1*in4_2*in4_3*in4_4*in4_5)))
//------------------------------------------------------------------------------------
class MultDiv : public TFunction
{
    public:
	MultDiv() : TFunction("multDiv")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"0") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in1_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 1.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in2_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 2.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in3_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 3.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in4_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 4.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Multiplicator+divider"); }
	string descr()	
	{     
	    return st_lib->I18N("Multiplicator+divider per formula:\n"
		"out=in1_1*in1_2*in1_3*in1_4*in1_5*(in2_1*in2_2*in2_3*in2_4*in2_5+\n"
		"          (in3_1*in3_2*in3_3*in3_4*in3_5)/(in4_1*in4_2*in4_3*in4_4*in4_5));");
	}

	void calc( TValFunc *val )
	{
	    double tmp1 = val->getR(16)*val->getR(17)*val->getR(18)*val->getR(19)*val->getR(20);
	    double tmp2 = val->getR(1)*val->getR(2)*val->getR(3)*val->getR(4)*val->getR(5);
	    double tmp3 = val->getR(6)*val->getR(7)*val->getR(8)*val->getR(9)*val->getR(10);
	    double tmp4 = val->getR(11)*val->getR(12)*val->getR(13)*val->getR(14)*val->getR(15);
	    val->setR(0,tmp2*(tmp3+tmp4/tmp1));
	}
};

//------------------------------------------------------------------------------------
//Exponent
//Formula: out=exp (in1_1*in1_2*in1_3*in1_4*in1_5 +
//                  (in2_1*in2_2*in2_3*in2_4*in2_5+in3) / (in4_1*in4_2*in4_3*in4_4*in4_5+in5) )
//------------------------------------------------------------------------------------
class Exp : public TFunction
{
    public:
	Exp() : TFunction("exp")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"0") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in1_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 1.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in2_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 2.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    ioAdd( new IO("in3",st_lib->I18N("Input 3"),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in4_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 4.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    ioAdd( new IO("in5",st_lib->I18N("Input 5"),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Exponent"); }
	string descr()	
	{    
	    return st_lib->I18N("Exponent per formula:\n"
		"out=exp (in1_1*in1_2*in1_3*in1_4*in1_5 +\n"
		"         (in2_1*in2_2*in2_3*in2_4*in2_5+in3) / (in4_1*in4_2*in4_3*in4_4*in4_5+in5) );");
	}

	void calc( TValFunc *v )
	{
	    double tmp1=v->getR(12)*v->getR(13)*v->getR(14)*v->getR(15)*v->getR(16)+v->getR(17);
	    double tmp2=v->getR(1)*v->getR(2)*v->getR(3)*v->getR(4)*v->getR(5);
	    double tmp3=v->getR(6)*v->getR(7)*v->getR(8)*v->getR(9)*v->getR(10)+v->getR(11);
	    v->setR(0,exp(tmp2+tmp3/tmp1));
	}
};

//------------------------------------------------------------------------------------
//Power
//Formula: out=(in1_1*in1_2*in1_3*in1_4*in1_5)^(in2_1*in2_2*in2_3*in2_4*in2_5 +
//                      (in3_1*in3_2*in3_3*in3_4*in3_5)/(in4_1*in4_2*in4_3*in4_4*in4_5))
//------------------------------------------------------------------------------------
class Pow : public TFunction
{
    public:
	Pow() : TFunction("pow")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"0") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in1_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 1.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in2_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 2.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in3_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 3.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in4_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 4.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Power"); }
	string descr()
	{
	    return st_lib->I18N("Power per formula:\n"
		"out=(in1_1*in1_2*in1_3*in1_4*in1_5)^(in2_1*in2_2*in2_3*in2_4*in2_5 +\n"
		"	(in3_1*in3_2*in3_3*in3_4*in3_5)/(in4_1*in4_2*in4_3*in4_4*in4_5));");
	}

	void calc( TValFunc *v )
	{
	    double tmp1=v->getR(16)*v->getR(17)*v->getR(18)*v->getR(19)*v->getR(20);
	    double tmp2=v->getR(6)*v->getR(7)*v->getR(8)*v->getR(9)*v->getR(10);
	    double tmp3=v->getR(11)*v->getR(12)*v->getR(13)*v->getR(14)*v->getR(15);
	    tmp2=tmp2+tmp3/tmp1;
	    tmp3=v->getR(1)*v->getR(2)*v->getR(3)*v->getR(4)*v->getR(5);
	    v->setR(0,pow(tmp3,tmp2));
	}
};

//------------------------------------------------------------------------------------
//Condition <
//Formula: out=if( in1<(in2_1*in2_2*in2_3*in2_4) ) then in3_1*in3_2*in3_3*in3_4;
//             else in4_1*in4_2*in4_3*in4_4;
//------------------------------------------------------------------------------------
class Cond1 : public TFunction
{
    public:
	Cond1() : TFunction("cond <")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"0") );
	    ioAdd( new IO("in1",st_lib->I18N("Input 1"),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in2_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 2.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in3_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 3.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in4_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 4.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Condition '<'"); }
	string descr()	
        {     
	    return st_lib->I18N("Condition '<' per formula:\n"
		"out=if( in1<(in2_1*in2_2*in2_3*in2_4) ) then in3_1*in3_2*in3_3*in3_4;\n"
		"    else in4_1*in4_2*in4_3*in4_4;");
	}

	void calc( TValFunc *v )
	{
	    double tmp1=v->getR(2)*v->getR(3)*v->getR(4)*v->getR(5);
	    if( v->getR(1)<tmp1 ) 	v->setR(0,v->getR(6)*v->getR(7)*v->getR(8)*v->getR(9));
	    else 			v->setR(0,v->getR(10)*v->getR(11)*v->getR(12)*v->getR(13));
	}
};

//------------------------------------------------------------------------------------
//Condition >
//Formula: out=if( in1>(in2_1*in2_2*in2_3*in2_4) ) then in3_1*in3_2*in3_3*in3_4;
//             else in4_1*in4_2*in4_3*in4_4;
//------------------------------------------------------------------------------------
class Cond2 : public TFunction
{
    public:
	Cond2() : TFunction("cond >")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"0") );
	    ioAdd( new IO("in1",st_lib->I18N("Input 1"),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in2_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 2.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in3_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 3.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in4_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 4.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Condition '>'"); }
	string descr()
	{
	    return st_lib->I18N("Condition '>' per formula:\n"
		"out=if( in1>(in2_1*in2_2*in2_3*in2_4) ) then in3_1*in3_2*in3_3*in3_4;\n"
		"    else in4_1*in4_2*in4_3*in4_4;");
	}

	void calc( TValFunc *v )
	{
	    double tmp1=v->getR(2)*v->getR(3)*v->getR(4)*v->getR(5);
	    if( v->getR(1)>tmp1 ) 	v->setR(0,v->getR(6)*v->getR(7)*v->getR(8)*v->getR(9));
	    else 			v->setR(0,v->getR(10)*v->getR(11)*v->getR(12)*v->getR(13));
	}
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
	Cond3() : TFunction("cond_full")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"0") );
	    ioAdd( new IO("in1",st_lib->I18N("Input 1"),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in2_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 2.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in3_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 3.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in4_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 4.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in5_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 5.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );	
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in6_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 6.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Full condition"); }
	string descr()	
	{     
	    return st_lib->I18N("Full condition per formula:\n"
		"out = if( in1<(in2_1*in2_2*in2_3*in2_4) )    then in3_1*in3_2*in3_3*in3_4;\n"
		"      else if( in1>(in4_1*in4_2*in4_3*in4_4) then in5_1*in5_2*in5_3*in5_4;\n"
		"      else in6_1*in6_2*in6_3*in6_4;");
	}

	void calc( TValFunc *v )
	{
	    double tmp1=v->getR(2)*v->getR(3)*v->getR(4)*v->getR(5);
	    double tmp2=v->getR(10)*v->getR(11)*v->getR(12)*v->getR(13);
	    double tmp3=v->getR(6)*v->getR(7)*v->getR(8)*v->getR(9);
	    double tmp4=v->getR(18)*v->getR(19)*v->getR(20)*v->getR(21);
	    double tmp5=v->getR(14)*v->getR(15)*v->getR(16)*v->getR(17);
	    if(v->getR(1)<tmp1) 	v->setR(0,tmp3);
	    else if( v->getR(1)>tmp2)	v->setR(0,tmp5);
	    else			v->setR(0,tmp4);
	}
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
	Select() : TFunction("select")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"0") );
	    ioAdd( new IO("sel",st_lib->I18N("Select"),IO::Integer,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in1_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 1.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in2_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 2.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in3_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 3.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 4; i_c++ )
		ioAdd( new IO(("in4_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 4.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Selector"); }
	string descr()	
	{
	    return st_lib->I18N("Selector per formula:\n"
		"out = if( sel = 1 )  then in1_1*in1_2*in1_3*in1_4;\n"
		"      if( sel = 2 )  then in2_1*in2_2*in2_3*in2_4;\n"
		"      if( sel = 3 )  then in3_1*in3_2*in3_3*in3_4;\n"
		"      if( sel = 4 )  then in4_1*in4_2*in4_3*in4_4;");
	}

	void calc( TValFunc *v )
	{
	    switch(v->getI(1))
	    {
		case  1: v->setR(0,v->getR(2)*v->getR(3)*v->getR(4)*v->getR(5)); break;
		case  2: v->setR(0,v->getR(6)*v->getR(7)*v->getR(8)*v->getR(9)); break;
		case  3: v->setR(0,v->getR(10)*v->getR(11)*v->getR(12)*v->getR(13)); break;
		case  4: v->setR(0,v->getR(14)*v->getR(15)*v->getR(16)*v->getR(17)); break;
		default: v->setR(0,0.0);
	    }
	}
};

//------------------------------------------------------------------------------------
//Incrementator
//Formula: out = if( in > prev )then prev + k_pos*(in-prev); else prev - k_neg*(prev-in);
//------------------------------------------------------------------------------------
class Increm : public TFunction
{
    public:
	Increm() : TFunction("increment")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"0") );
	    ioAdd( new IO("in",st_lib->I18N("Input"),IO::Real,IO::Input,"1") );
	    ioAdd( new IO("prev",st_lib->I18N("Previous"),IO::Real,IO::Input,"1") );
	    ioAdd( new IO("k+",st_lib->I18N("Positive koef"),IO::Real,IO::Input,"0.1") );
	    ioAdd( new IO("k-",st_lib->I18N("Negative koef"),IO::Real,IO::Input,"0.1") );
	}
	
	string name()	{ return st_lib->I18N("Incrementator"); }
	string descr()
	{
	    return st_lib->I18N("Incrementator per formula:\n"
		"out = if( in1 > in2 )then in2 + in3*(in1-in2); else in2 - in4*(in2-in1);");
	}

	void calc( TValFunc *v )
	{
	    if( v->getR(1) > v->getR(2) )
		v->setR(0,v->getR(2) + v->getR(3)*(v->getR(1) - v->getR(2)));
	    else v->setR(0,v->getR(2) - v->getR(4)*(v->getR(2) - v->getR(1)));
	}
};

//------------------------------------------------------------------------------------
//Divider
//Formula: out = (in1_1*in1_2*in1_3*in1_4*in1_5 + in2_1*in2_2*in2_3*in2_4*in2_5 + in3) / 
//		 (in4_1*in4_2*in4_3*in4_4*in4_5 + in5_1*in5_2*in5_3*in5_4*in5_5 + in6);
//------------------------------------------------------------------------------------
class Divider : public TFunction
{
    public:
	Divider() : TFunction("div")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"0") );

	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in1_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 1.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in2_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 2.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    ioAdd( new IO("in3",st_lib->I18N("Input 3"),IO::Real,IO::Input,"1") );	
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in4_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 4.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in5_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 5.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    ioAdd( new IO("in6",st_lib->I18N("Input 6"),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Divider"); }
	string descr()
	{
	    return st_lib->I18N("Divider per formula:\n"
		"out = (in1_1*in1_2*in1_3*in1_4*in1_5 + in2_1*in2_2*in2_3*in2_4*in2_5 + in3) /\n"
		"      (in4_1*in4_2*in4_3*in4_4*in4_5 + in5_1*in5_2*in5_3*in5_4*in5_5 + in6);");
	}

	void calc( TValFunc *v )
	{
	    double t1 =	v->getR(1)*v->getR(2)*v->getR(3)*v->getR(4)*v->getR(5) + 
			v->getR(6)*v->getR(7)*v->getR(8)*v->getR(9)*v->getR(10) + v->getR(11);
	    double t2 = v->getR(12)*v->getR(13)*v->getR(14)*v->getR(15)*v->getR(16) +
			v->getR(17)*v->getR(18)*v->getR(19)*v->getR(20)*v->getR(21) + v->getR(22);
	    v->setR(0,t1/t2);
	}
};

//------------------------------------------------------------------------------------
//PID
//Inputs:
//  0:val  - Value
//  1:sp   - Setpoint
//  2:max  - Maximum scale
//  3:min  - Minimum scale
//  4:out  - Output
//  5:auto - Automatic mode
//  6:casc - Cascade mode
//  7:in1  - Add input 1
//  8:in2  - Add input 2
//  9:in3  - Add input 3
//  10:in4  - Add input 4
//Koefficients:
//  11:Kp    - Gain
//  12:Ti   - Integral time (ms)
//  13:Td   - Differencial time (ms)
//  14:Tf   - Filter (lag) time (ms)
//  15:Hup  - Up output limit (%)
//  16:Hdwn - Down output limit (%)
//  17:Zi   - Insensibility zone (%)
//  18:K1   - Scale input 1
//  19:K2   - Scale input 2
//  20:K3   - Scale input 3
//  21:K4   - Scale input 4
//  22:cycle- Calc cycle (ms)
//Internal data:
//  23:#int - Curent integral value
//  24:#dif - Curent differencial value
//  25:#lag - Curent lag value 
//------------------------------------------------------------------------------------
class PID : public TFunction
{
    public:
	PID() : TFunction("pid")
	{
	    //Inputs
	    ioAdd( new IO("var",st_lib->I18N("Variable"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("sp",st_lib->I18N("Setpoint"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("max",st_lib->I18N("Max scale"),IO::Real,IO::Input,"100") );
	    ioAdd( new IO("min",st_lib->I18N("Min scale"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("out",st_lib->I18N("Output (%)"),IO::Real,IO::Return,"0") );    
	    ioAdd( new IO("auto",st_lib->I18N("Auto mode"),IO::Boolean,IO::Input,"0") );
	    ioAdd( new IO("casc",st_lib->I18N("Cascade mode"),IO::Boolean,IO::Input,"0") );
	    ioAdd( new IO("in1",st_lib->I18N("Input 1"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("in2",st_lib->I18N("Input 2"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("in3",st_lib->I18N("Input 3"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("in4",st_lib->I18N("Input 4"),IO::Real,IO::Input,"0") );
    
	    //Koefficients
	    ioAdd( new IO("Kp",st_lib->I18N("Kp"),IO::Real,IO::Input,"1") );
	    ioAdd( new IO("Ti",st_lib->I18N("Ti (ms)"),IO::Integer,IO::Input,"1000") );    
	    ioAdd( new IO("Td",st_lib->I18N("Td (ms)"),IO::Integer,IO::Input,"0") );    
	    ioAdd( new IO("Tf",st_lib->I18N("Tf-lag (ms)"),IO::Integer,IO::Input,"0") );
	    ioAdd( new IO("Hup",st_lib->I18N("Out up limit (%)"),IO::Real,IO::Input,"100") );
	    ioAdd( new IO("Hdwn",st_lib->I18N("Out down limit (%)"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("Zi",st_lib->I18N("Insensibility (%)"),IO::Real,IO::Input,"1") );    
	    ioAdd( new IO("K1",st_lib->I18N("K input 1"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("K2",st_lib->I18N("K input 2"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("K3",st_lib->I18N("K input 3"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("K4",st_lib->I18N("K input 4"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("cycle",st_lib->I18N("Calc cycle (ms)"),IO::Integer,IO::Input,"1000") );
    
	    //Internal data:
	    ioAdd( new IO("int",st_lib->I18N("Integral value"),IO::Real,IO::Output,"0",true) );
	    ioAdd( new IO("dif",st_lib->I18N("Differencial value"),IO::Real,IO::Output,"0",true) );
	    ioAdd( new IO("lag",st_lib->I18N("Lag value"),IO::Real,IO::Output,"0",true) );
	}
	
	string name()	{ return st_lib->I18N("PID regulator"); }
	string descr()	{ return st_lib->I18N("PID regulator"); }	

	void calc( TValFunc *v )
	{
	    double	val	= v->getR(0),
			sp	= v->getR(1),
			max 	= v->getR(2),
			min	= v->getR(3),
			out	= v->getR(4),
			in1	= v->getR(7),
			in2	= v->getR(8),
			in3	= v->getR(9),
			in4	= v->getR(10),
			kp	= v->getR(11),
			h_up	= v->getR(15),
			h_dwn	= v->getR(16),
			zi	= v->getR(17),
			k1	= v->getR(18),
			k2 	= v->getR(19),
			k3 	= v->getR(20),
			k4 	= v->getR(21),
			cycle   = v->getI(22),
			integ   = v->getR(23),
			difer	= v->getR(24),
			lag     = v->getR(25);
    
	    double    	Kf	= (v->getI(14)>cycle)?cycle/v->getI(14):1.;
	    double	Kint	= (v->getI(12)>cycle)?cycle/v->getI(12):1.;
	    double	Kdif	= (v->getI(13)>cycle)?cycle/v->getI(13):1.;

	    //Scale error
	    if( max <= min )	return;
	    
	    //Prepare values
	    sp = 100.*(sp+min)/(max-min);
	    val = 100.*(val+min)/(max-min);
	    val += k1*in1 + k2*in2;
	    if(val >  100.) val = 100.;
	    if(val < -100.) val = -100.;
    
	    //Error
	    double err = sp - val;
    
	    //Insensibility
	    if( fabs(err) < zi )	err = 0.;
	    else
	    {
		if( err>0. )	err-=zi;
		else		err+=zi;    
	    }
    
	    //Gain
	    err*=kp;
	    if(err >  100.) err = 100.;
	    if(err < -100.) err = -100.;
    
	    //Input filter lag    
	    lag+=Kf*(err-lag);
	
	    //Automatic mode enabled
	    if(v->getB(5))
	    {
		integ+=Kint*lag;		//Integral
		difer-=Kdif*(difer-lag);	//Differecial lag
	    
		out = (2.*lag + integ - difer) + k3*in3 + k4*in4;
		if(out > h_up || out < h_dwn )
		{
		    if( out > h_up )	out = h_up;
		    if( out < h_dwn ) 	out = h_dwn;
		    //Fix integral
		    integ = out - 2.*lag + difer - k3*in3 + k4*in4;
		}
	    }     
	
	    //Write outputs
	    v->setR(4,out);
	    v->setR(23,integ);
	    v->setR(24,difer);
	    v->setR(25,lag);
	}
};

//------------------------------------------------------------------------------------
//Alarm
//Formula: out = if (val>max || val<min) then true; else false;
//------------------------------------------------------------------------------------
class Alarm : public TFunction
{
    public:
	Alarm() : TFunction("alarm")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Boolean,IO::Return,"0") );
	    ioAdd( new IO("val",st_lib->I18N("Value"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("max",st_lib->I18N("Maximum"),IO::Real,IO::Input,"100") );
	    ioAdd( new IO("min",st_lib->I18N("Minimum"),IO::Real,IO::Input,"0") );
	}
	
	string name()	{ return st_lib->I18N("Alarm"); }
	string descr()
	{
	    return st_lib->I18N("Scale parameter alarm:\n"
		"out = if (val>max || val<min) then true; else false;");
	}

	void calc( TValFunc *v )
	{
	    double max = v->getR(2),
		min = v->getR(3),
		val = v->getR(1);
       
	    if( max != min && ( val < min || val > max ) )
		v->setB(0,true);
	    else v->setB(0,false);
	}
};

//------------------------------------------------------------------------------------
//Flow
//Formula: f = k1*((k3+k4*x)^k2)+Krnd*RAND
//------------------------------------------------------------------------------------
class Flow : public TFunction
{
    public:
	Flow() : TFunction("flow")
	{
	    ioAdd( new IO("f",st_lib->I18N("Flow"),IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x",st_lib->I18N("X positon"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("K1",st_lib->I18N("K1"),IO::Real,IO::Input,"1") );
	    ioAdd( new IO("K2",st_lib->I18N("K2"),IO::Real,IO::Input,"1") );
	    ioAdd( new IO("K3",st_lib->I18N("K3"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("K4",st_lib->I18N("K4"),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Flow"); }
	string descr()	
	{
	    return st_lib->I18N("Flow calc per formule:\n"
		"f = K1*((K3+K4*x)^K2);");
	}

	void calc( TValFunc *v )
	{
	    v->setR(0,v->getR(2)*pow(v->getR(4)+v->getR(5)*v->getR(1),v->getR(3)));
	}
};

//------------------------------------------------------------------------------------
//Sum+mult
//Formula: out = in1_1*in1_2*(in1_3*in1_4+in1_5) + in2_1*in2_2*(in2_3*in2_4+in2_5) +
//               in4_1*in4_2*(in4_3*in4_4+in4_5) + in5_1*in5_2*(in5_3*in5_4+in5_5);
//------------------------------------------------------------------------------------
class SumMult : public TFunction
{
    public:
	SumMult() : TFunction("sum_mult")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"0") );
    
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in1_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 1.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in2_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 2.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in3_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 3.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in4_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 4.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Sum and mult"); }
	string descr()
	{
	    return st_lib->I18N("Sum and mult per formule:\n"
		"out = in1_1*in1_2*(in1_3*in1_4+in1_5) + in2_1*in2_2*(in2_3*in2_4+in2_5) +\n"
		"      in4_1*in4_2*(in4_3*in4_4+in4_5) + in5_1*in5_2*(in5_3*in5_4+in5_5);");
	}

	void calc( TValFunc *v )
	{
	    double t1 =	v->getR(1)*v->getR(2)*(v->getR(3)*v->getR(4)+v->getR(5));
	    double t2 =	v->getR(6)*v->getR(7)*(v->getR(8)*v->getR(9)+v->getR(10));
	    double t3 =	v->getR(11)*v->getR(12)*(v->getR(13)*v->getR(14)+v->getR(15));
	    double t4 =	v->getR(16)*v->getR(17)*(v->getR(18)*v->getR(19)+v->getR(20));
    
	    v->setR(0,t1+t2+t3+t4);
	}
};

//------------------------------------------------------------------------------------
//Sum+div
//Formula: out = in1_1*in1_2*(in1_3+in1_4/in1_5) + in2_1*in2_2*(in2_3+in2_4/in2_5) +
//               in4_1*in4_2*(in4_3+in4_4/in4_5) + in5_1*in5_2*(in5_3+in5_4/in5_5);
//------------------------------------------------------------------------------------
class SumDiv : public TFunction
{
    public:
	SumDiv() : TFunction("sum_div")
	{
	    ioAdd( new IO("out",st_lib->I18N("Output"),IO::Real,IO::Return,"0") );

	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in1_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 1.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in2_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 2.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in3_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 3.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	    for( int i_c = 1; i_c <= 5; i_c++ )
		ioAdd( new IO(("in4_"+TSYS::int2str(i_c)).c_str(),(st_lib->I18N("Input 4.")+TSYS::int2str(i_c)).c_str(),IO::Real,IO::Input,"1") );
	}
	
	string name()	{ return st_lib->I18N("Sum and divide"); }
	string descr()	
	{
	    return st_lib->I18N("Sum and divide per formule:\n"
		"out = in1_1*in1_2*(in1_3+in1_4/in1_5) + in2_1*in2_2*(in2_3+in2_4/in2_5) +\n"
		"      in4_1*in4_2*(in4_3+in4_4/in4_5) + in5_1*in5_2*(in5_3+in5_4/in5_5);");
	}

	void calc( TValFunc *v )
	{
	    double t1 =	v->getR(1)*v->getR(2)*(v->getR(3)+v->getR(4)/v->getR(5));
	    double t2 =	v->getR(6)*v->getR(7)*(v->getR(8)+v->getR(9)/v->getR(10));
	    double t3 =	v->getR(11)*v->getR(12)*(v->getR(13)+v->getR(14)/v->getR(15));
	    double t4 =	v->getR(16)*v->getR(17)*(v->getR(18)+v->getR(19)/v->getR(20));
    
	    v->setR(0,t1+t2+t3+t4);
	}
};

//------------------------------------------------------------------------------------
//Lag
//Formula: y = y - Klag*( y - x );
//------------------------------------------------------------------------------------
class Lag : public TFunction
{
    public:
	Lag() : TFunction("lag")
	{
	    ioAdd( new IO("y",st_lib->I18N("Y"),IO::Real,IO::Return,"0") );
	    ioAdd( new IO("x",st_lib->I18N("X"),IO::Real,IO::Input,"0") );
	    ioAdd( new IO("Klag",st_lib->I18N("Klag"),IO::Real,IO::Input,"0.1") );
	}
	
	string name()	{ return st_lib->I18N("Lag"); }
	string descr()	
	{
	    return st_lib->I18N("Lag per formule:\n"
		"y = y - Klag*( y - x );");
	}

	void calc( TValFunc *v )
	{
	    double y = v->getR(0);
	    v->setR(0,y - v->getR(2)*(y - v->getR(1)));
	}
};

//=====================================================
//================ Complex1 functions library =========
//=====================================================
class Complex1 : public TLibFunc
{
    public:
	Complex1( ) : TLibFunc("complex1")
	{ 
	    reg( new DigitBlock() );
	    reg( new Sum() );
	    reg( new Mult() );
	    reg( new MultDiv() );
	    reg( new Exp() );
	    reg( new Pow() );
	    reg( new Cond1() );
	    reg( new Cond2() );
	    reg( new Cond3() );
	    reg( new Select() );
	    reg( new Increm() );
	    reg( new Divider() );
	    reg( new PID() );
	    reg( new Alarm() );
	    reg( new Flow() );
	    reg( new SumMult() );
	    reg( new SumDiv() );
	    reg( new Lag() );	    
	}
	
	string name() 	{ return st_lib->I18N("Complex1 functions"); }
	string descr()	{ return st_lib->I18N("Allow functions of SCADA system 'Complex1' for compatibility."); }
};

} //End namespace virtual

#endif //LIBCOMPL1_H

