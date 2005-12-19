// file "titem.cpp"

/***************************************************************************
 *   Copyright (C) 2005 by Evgen Zaichuk                               
 *   evgen@diyaorg.dp.ua                                                     
 *                                                                         
 *   This program is free software; you can redistribute it and/or modify  
 *   it under the terms of the GNU General Public License as published by  
 *   the Free Software Foundation; either version 2 of the License, or     
 *   (at your option) any later version.                                   
 *                                                                         
 *   This program is distributed in the hope that it will be useful,       
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *   GNU General Public License for more details.                          
 *                                                                         
 *   You should have received a copy of the GNU General Public License     
 *   along with this program; if not, write to the                         
 *   Free Software Foundation, Inc.,                                       
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             
 ***************************************************************************/

#include <math.h>
#include <stack>

#include <qpainter.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qfontdatabase.h>

#include "titem.h"
#include "tdbgw.h"
#include "tvisionerror.h"

#include "tframeview.h"

using std::stack;

using namespace VISION;

//-----------------------------------TItem--------------------------------------------------
TItem::TItem (TVISIONMode m)
   :name("Item"),
    mode(m),
    visibleItem(true)
{
}

TItem::~TItem ()
{
}

bool TItem::isVisibleProp() const
{
   return visibleItem;
}

bool TItem::saveToFile(QFile &s)
{
   QTextStream stream(&s);
   stream.setEncoding(QTextStream::UnicodeUTF8);
   TListOfString listOfString;
   unsigned int numSpaces = 2;
   stream << "  <item class = \"" << rtti() << "\">\n";
   // ---z-координата:
   stream << "   <property name = \"z\">\n";
   stream << "    <value>" << QString::number(zItem()) << "</value>\n";
   stream << "   </property>\n";
   // ---z-координата---
   //проход по категориям:
   listOfString = getPropCategories();
   for (TListOfString::iterator i = listOfString.begin(); i != listOfString.end(); i++)
      {       
       //проход по именам свойств в данной категории:
       numSpaces++;
       TListOfString propertiesNames = getPropNames(&(*i));
       for (TListOfString::iterator i2 = propertiesNames.begin(); i2 != propertiesNames.end(); i2++)
          {
	   for (int j = 0; j < numSpaces; j++)
              stream << " ";
           stream << "<property name = \"" << (*i2) << "\">\n";
	   //проход по значению свойства:
	   QString value = getPropValue(*i2);
	   for (int j = 0; j < numSpaces; j++)
              stream << " ";
	   stream << " <value>" << value << "</value>\n";
	   ///проход по динамике свойства:
	   for (TListDynamicProp::iterator j = listDynamicProp.begin(); j != listDynamicProp.end(); j++)
	      if ( (*j).getPropName() == (*i2) )
	         {
		  for (int j2 = 0; j2 < numSpaces; j2++)
                     stream << " ";
		  stream << " <dynamic>" << (*j).getPropText() << "</dynamic>\n";
		  break;
		 }
	   ///
	   
	   for (int j = 0; j < numSpaces; j++)
              stream << " ";
           stream << "</property>\n";
	  }
       numSpaces--;
      }
   
   stream << "  </item>\n";
}

//установка/изменение динамизации свойства propName:
bool TItem::setDynamic(const QString &propName, const QString &text)
{
   bool result = false;
   bool present = false;
   TListDynamicProp::iterator presentDp;
   //поиск propName в списке TListDynamicProp:
   for (TListDynamicProp::iterator i = listDynamicProp.begin(); i != listDynamicProp.end(); i++)
       if ((*i).getPropName() == propName)
          {
           present = true;
	   presentDp = i;
	   break;
          }
   if (!present)
      {//добавление TDynamicProp в список:
       try
          {
          TDynamicProp dp(this);
          if (dp.setPropText(text, propName))
             {
              listDynamicProp.push_back(dp);
	      result = true;
	     }
	  }
        catch (TVisionError)
	   {throw;};
      }
      else //изменение TDynamicProp в списке:
	  result = (*presentDp).setPropText(text, propName);         
   
   return result;
}

//динамизация свойства propName:
bool TItem::getDynamic(const QString &propName, QString &text)
{
   bool res = false;
   for (TListDynamicProp::iterator j = listDynamicProp.begin(); j != listDynamicProp.end(); j++)
	      if ( (*j).getPropName() == propName )
	         {
		  text = (*j).getPropText();
		  res = true;
		  break;
		 }
   return res;
}
//динамизация (Runtime):
bool TItem::doDynamic()
{//QMessageBox::information( NULL, "TItem::doDynamic()", "doDynamic begin");
   bool result = true;
   for (TListDynamicProp::iterator i = listDynamicProp.begin(); i != listDynamicProp.end(); i++)
      result = result && (*i).doDynamic();
   //QMessageBox::information( NULL, "TItem::doDynamic()", "doDynamic end");
   return result;
}
  
//--------------------------------------TDynamicProp---------------------------------------------
TDynamicProp::TDynamicProp(TItem *i)
{
   item = i;
   dynamicType = noDType;
   
   clearStructs();
}

void TDynamicProp::clearStructs()
{
   dynamicValue.paramName = dynamicValue.attrName = "";
   dynamicValue.format = 'g';
   dynamicValue.precision = 2;
       
   dynamicEnum.dynamicEnumElem.clear();
   dynamicEnum.valueDefault = "";
       
   dynamicProportion.paramName0 = dynamicProportion.attrName0 = 
      dynamicProportion.paramName100 = dynamicProportion.attrName100 = 
      dynamicProportion.paramNameX = dynamicProportion.attrNameX = "";
   dynamicProportion.size0 = 0;
   dynamicProportion.size100 = 100;
   dynamicProportion.val0 = 0;
   dynamicProportion.val100 = 100;
   dynamicProportion.yesSize0 = false;
   dynamicProportion.yesSize100 = false;
}
//проверяет правильность текстового описания динамики text свойства propName:
bool TDynamicProp::isValid(const QString &text, const QString &propName)
{
   bool result;
   bool parsing = true;
   QString s(text);
   try
      {
       parsing = parse(s); //синтаксический разбор
      }
   catch (TVisionError)
      {
       clearStructs();
       result = false;
       throw;
      }
   
   result = parsing;
   
   if (!parsing)
      //убираем мусор в случае синтаксической ошибки:
      {
       clearStructs();
       result = false;
      }
      else
         {
	  TDynamicTypeEnumList::iterator i;
	  //проверка типов:
	  TPropType itemPropType = item->getPropType(&propName);
	  switch (dynamicType)
	     {//{unknownType, intType, floatType, stringType, enumType, colorType} TPropType;
	      case valueDType : if (itemPropType == enumType)
	                                    {
					     result = false;
					     throw TVisionError(QString("DynamicProp"), 
					           QString("incompatible types of Dynamization and Property"));
					    }
					  break;
	      case enumDType : if ( (itemPropType == floatType) || (itemPropType == intType) )
	                                    {
					     result = false;
					     throw TVisionError(QString("DynamicProp"), 
					           QString("incompatible types of Dynamization and Property"));
					    }
					  //если динамизируется свойство типа "перечисление", 
					  // то проверяем возможные значения:
					  if (itemPropType == enumType)
					     for (i = dynamicEnum.dynamicEnumElem.begin(); 
					           i != dynamicEnum.dynamicEnumElem.end(); i++)
					        if (!(item->setPropValue(propName, i->value, false)))
						   {
						    result = false;
						    throw TVisionError(QString("DynamicProp"), 
						          QString("error in enum: '" + (*i).value + 
							          "' is not member of '" + propName + "'"));
						   }
					  break;
	      case proportionDType : if ( (itemPropType == stringType) || (itemPropType == enumType) )
	                                    {
					     result = false;
					     throw TVisionError(QString("DynamicProp"), 
					           QString("incompatible types of Dynamization and Property"));
					    }
					  break;
	      
	     }
	 }
   
   return result;
}

bool TDynamicProp::setPropText(const QString &text, const QString &name)
{
   try
      {
       propName = name;
       propText = text; ///!!!оптимизация: удаление пробелов...
       isValid(text, name);
      }
   catch (TVisionError e)
      {propName = propText = ""; throw;}
   
   return true;
}

//получить значение динамизируемого свойства (анализ структур dynamicValue, dynamicEnum, dynamicProportion)
bool TDynamicProp::getPropValue(QString &value) const
{//QMessageBox::information( NULL, "TDynamicProp", "getPropValue" ); 
   bool res;
   TDynamicTypeEnumList::const_iterator i;
   int intValue;
   bool doEnum = false;
   int sizeMin, sizeMax; //для valueDProportion
   sizeMin = 0;
   sizeMax = 1;
   double valMin, valMax, currVal; //для valueDProportion
   QString val;
   
   switch (dynamicType)
      {
       case valueDType : dbgw->getValue(dynamicValue.paramName, dynamicValue.attrName, value);
       				   if (!((dynamicValue.format=='s')||(dynamicValue.format=='S')))
                                           value = QString::number(value.toDouble(&res), dynamicValue.format,
				                                                       dynamicValue.precision);
					else res = true;
				   if (!res)
				      value = "ЗНАЧ?";
				  break;
       //enum{\"0x0F0F0F\"::'T101'.'Error';\"0xFFF0000\"::'T101'.'HH';}
       case enumDType : for (i = dynamicEnum.dynamicEnumElem.begin(); i != dynamicEnum.dynamicEnumElem.end(); i++)
                                      {//проход по строкам "значение::условие", 
				       // если условие выполнено, то установка value и выход;
				       // УСЛОВИЕ!!!: в БД атрибут параметра должен иметь значение типа Int
				       dbgw->getValue((*i).paramName, (*i).attrName, value);
				       intValue = value.toInt(&res);
				       if (!res)
					   break;
				       if (intValue)
				          {
				           value = (*i).value;
					   doEnum = true;
					   break;
					  }
				      }
				   if (!doEnum)
				      //если ни одно условие не выполнено:!!!
				      value = dynamicEnum.valueDefault;
				   break;
       //proportion{0::'T101'.'Min';150::.8e+1;::'T101'.'Value';}
       case proportionDType : //границы изменения свойства элемента:
                                           sizeMin = dynamicProportion.size0;
					   sizeMax = dynamicProportion.size100;
					   //границы изменения динамизируемого параметра:
					   if (dynamicProportion.paramName0 == "")
					      valMin = dynamicProportion.val0;
					      else {dbgw->getValue(dynamicProportion.paramName0,
					                           dynamicProportion.attrName0, val);
					              valMin = val.toDouble(&res);
						      if (!res)
						         {
							  value = "ЗНАЧ?";
						          break;
							 }
						     }
				           if (dynamicProportion.paramName100 == "")
					      valMax = dynamicProportion.val100;
					      else {dbgw->getValue(dynamicProportion.paramName100,
					                           dynamicProportion.attrName100, val);
					              valMax = val.toDouble(&res);
						      if (!res)
						         {
							  value = "ЗНАЧ?";
						          break;
							 }
						     }
				           //текущее значение динамизируемого параметра
					   dbgw->getValue(dynamicProportion.paramNameX, 
					                  dynamicProportion.attrNameX, val);
					   currVal = val.toDouble(&res);
					   if (!res)
					      {
					       value = "ЗНАЧ?";
					       break;
					      }
					   if ((valMax-valMin) == 0)
					      {
					       value = "ЗНАЧ?";
					       break;
					      }
					   value = QString::number( (int)((currVal-valMin)/(valMax-valMin)*
					                                         (sizeMax-sizeMin)+sizeMin) );
					   /*QMessageBox::information(NULL, "", "sizeMin; sizeMax; valMin; valmax; currVal =" +        QString::number(sizeMin)+"; "+ QString::number(sizeMax)+"; "+QString::number(valMin)+"; "+QString::number(valMax)+"; "+QString::number(currVal)+"; ::"+value);*/
					   break;
       default : value = "НЕТ ДИНАМИКИ!"; 
      }//switch
   
   return true;
}

//изменить свойство propName элемента item
bool TDynamicProp::doDynamic()
{   
   QString value;
   if (getPropValue(value))
      if (item->setPropValue(propName, value))
         {//QMessageBox::information( NULL, "TDynamicProp", "doDynamic Ok: " +  propName + "; " + value);
          return true;
	 }
	 else 
	    {/*{QMessageBox::information( NULL, "TDynamicProp", "doDynamic FAULT: " +  propName + "; " + value);}*/;
             return false;
	    }
}

TDynamicType TDynamicProp::getDynamicType()
{
   return dynamicType;
}

//===================================СИНТАКСИЧЕСКИЙ АНАЛИЗ==================================================
//Выделение лексем:
bool TDynamicProp::isitName(const QString &s, const int begin, int &end)
{//<Name> = '<Char>+ '
   if (s[begin] != '\'')
      return false;
   int p = begin+1;
   while ( (p<s.length()) && (s[p] != '\'') )
      p++;
   if ( (s[p] == '\'') && (p-begin>1) )
      {
       end = p+1;
       return true;
      }
      else return false;
}

bool TDynamicProp::isitString(const QString &s, const int begin, int &end)
{//<String> = "<Char>+"
      if (s[begin] != '\"')
      return false;
   int p = begin+1;
   while ( (p<s.length()) && (s[p] != '\"') )
      p++;
   if ( (s[p] == '\"') && (p-begin>1) )
      {
       end = p+1;
       return true;
      }
      else return false;
}

bool TDynamicProp::isitUInt(const QString &s, const int begin, int &end)
{//<Digit> = 0|1|...|9
 //<UInt> = <Digit>+
   int p = begin;
   while ( (p<s.length()) && s[p].isDigit() )
      p++;
   if (p-begin>0)
      {
       end = p;//+1;
       return true;
      }
      else return false;
}

bool TDynamicProp::isitInt(const QString &s, const int begin, int &end)
{//<Digit> = 0|1|...|9
 //<Int> = <Sign><Digit>+
   int p = begin;
   if ( (s[p] == '+') || (s[p] == '-') )
      p++;
   while ( (p<s.length()) && s[p].isDigit() )
      p++;
   if (p-begin>0)
      {
       end = p;//+1;
       return true;
      }
      else return false;
}

bool TDynamicProp::isitDecimal(const QString &s, const int begin, int &end)
{//<Decimal> = <sign>*(<digit>*.<digit>+|<digit>+.<digit>*)
   int p = begin;
   if ( (s[p] == '+') || (s[p] == '-') )
      p++;
   if (s[p].isDigit())
      {//<digit>+.<digit>*
       while ((p<s.length()) && s[p].isDigit())
          p++;
       //if (p>s.length()) return false;
       if ( (s[p] == ',') || (s[p] == '.') )
          {
	   p++;
	   while ((p<s.length()) && s[p].isDigit())
              p++;
	   end = p;//+1;
	   return true;
	  }
	   else return false;
      }
      
   if ( (s[p] == ',') || (s[p] == '.') )
      {//.<digit>+
       p++;
       bool wasDigit = false;
       while ((p<s.length()) && s[p].isDigit())
          {
           p++;
	   wasDigit = true;
	  }
       if (wasDigit)
          {
	   end = p;//+1;
	   return true;
	  }
	  else return false;
      }
      
   return false;
}

bool TDynamicProp::isitReal(const QString &s, const int begin, int &end)
{//<Real> = x<int>x |<decimal>|<decimal>(E|e)<int>
   //поиск e|E:
  int posE = -1;
  int p = begin;
  while ((p<s.length()) && (posE<0) )
     if (s[p].lower() == 'e')
        posE = p;
	else p++;
   if (!((p==begin) || (posE+1>=s.length())))//если е найден, он не в начале или в конце строки, то
      {
       QString s2 = s.mid(begin, posE-begin);
       int end2;
       if (isitDecimal(s2, 0, end2))
          if (end2==s2.length())
             {
	      if (isitInt(s, posE+1, end))
	         return true;
	     }
      }
      
   if (isitDecimal(s, begin, end))
      return true;
   /*if (isitInt(s, begin, end))
      return true;*/
   return false;
}

bool TDynamicProp::isitFormat(const QString &s, const int begin, int &end)
{//<Format> =  f | e | E | g | G | s | S
   bool yes = false;
   if (s[begin].lower() == 'f')
      yes = true;
   if (s[begin].lower() == 'e')
      yes = true;
   if (s[begin].lower() == 'g')
      yes = true;
   if (s[begin].lower() == 's')
      yes = true;
   if (yes)
      {
       end = begin + 1;
       return true;
      }
   return false;
}

bool TDynamicProp::isitValue(const QString &s, const int begin, int &end)
{
   QString s2("value");
   if (s.mid(begin, s2.length()) == s2)
      {
       end = begin + s2.length();
       return true;
      }
   return false;
}

bool TDynamicProp::isitEnum(const QString &s, const int begin, int &end)
{
   QString s2("enum");
   if (s.mid(begin, s2.length()) == s2)
      {
       end = begin + s2.length();
       return true;
      }
   return false;
}

bool TDynamicProp::isitProportion(const QString &s, const int begin, int &end)
{
   QString s2("proportion");
   if (s.mid(begin, s2.length()) == s2)
      {
       end = begin + s2.length();
       return true;
      }
   return false;
}

bool TDynamicProp::isitPoint(const QString &s, const int begin, int &end)
{
   if (s[begin] == '.')
      {
       end = begin + 1;
       return true;
      }
   return false;
}

bool TDynamicProp::isitN(const QString &s, const int begin, int &end)
{//";"
   if (s[begin] == ';')
      {
       end = begin + 1;
       return true;
      }
   return false;
}

bool TDynamicProp::isitV(const QString &s, const int begin, int &end)
{//"::"
   QString s2("::");
   if (s.mid(begin, s2.length()) == s2)
      {
       end = begin + s2.length();
       return true;
      }
   return false;
}

bool TDynamicProp::isitOpenScope(const QString &s, const int begin, int &end)
{
   if (s[begin] == '{')
      {
       end = begin + 1;
       return true;
      }
   return false;
}

bool TDynamicProp::isitCloseScope(const QString &s, const int begin, int &end)
{
   if (s[begin] == '}')
      {
       end = begin + 1;
       return true;
      }
   return false;
}

bool TDynamicProp::isitEps(const QString &s, const int begin, int &end)
{
   if (s[begin] == QString(epsSymbol)[0])
      {
       end = begin + 1;
       return true;
      }
   return false;
}

//получить очередную лексему:
TVocabulary TDynamicProp::getLexeme(const QString &s, const int begin, int &end, QString &lexemeValue)
{
   if (end==s.length())
      {
       lexemeValue = "";
       return ta_eps;
      }
   if (isitName(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_name;
      }
   if (isitString(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_string;
      }
   if (isitReal(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_real;
      }
   if (isitUInt(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_uint;
      }
   if (isitValue(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_value;
      }
   if (isitEnum(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_enum;
      }
   if (isitProportion(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_proportion;
      }
   if (isitPoint(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_p;
      }
   if (isitFormat(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_format;
      }
   if (isitN(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_n;
      }
   if (isitV(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_v;
      }
   if (isitOpenScope(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_o;
      }
   if (isitCloseScope(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_c;
      }
   if (isitEps(s, begin, end))
      {
       lexemeValue = s.mid(begin, end-begin);
       return ta_eps;
      }
   
   lexemeValue = "";
   return ta_unknown;
}

//выполнение правил грамматики (заполнение стека) во время разбора:
bool TDynamicProp::parseDoStack(TStack &st, const TVocabulary lexeme)
{
   TVocabulary stackTop = st.top();//извлекаем верхний элемент из стека и запоминаем его
   st.pop();
   bool result = true;
   //заполнение стека в соответствии с таблицей разбора:
   switch (stackTop)
      {
       case na_S : switch (lexeme)
                          { 
			   case ta_value : st.push(A_V);
			                          st.push(na_V);
						  break;
			   case ta_enum : st.push(A_E);
			                          st.push(na_E);
			                          break;
			   case ta_proportion : st.push(A_P);
			                          st.push(na_P);
			                          break;
			   default : result = false;
			  }
			break;
       case na_V : if (lexeme == ta_o) 
                            {st.push(ta_c);
			     //st.push(ta_n);
			     st.push(na_Format);
			     st.push(ta_v);
			     st.push(na_Attr_name_V);
			     st.push(ta_p);
			     st.push(na_Param_name_V);
			    }
			    else result = false;
       			break;
       case na_E : if (lexeme == ta_o) 
                            {//st.push(ta_c);
			     st.push(na_EnumPair);
			    }
			    else result = false;
       			break;
       case na_P : if (lexeme == ta_o) 
                            {st.push(ta_c);
			     st.push(na_P3);
			     st.push(na_P2);
			     st.push(na_P1);
			    }
			    else result = false;
       			break;
       case na_Format : switch (lexeme) 
                                   {
                            	   case ta_format : st.push(ta_n);
			     				     st.push(ta_uint);
							     st.push(A_V_3);
							     break;
				   case ta_n : break;
				   case ta_eps: break;
				   default : result = false;
				   }//switch
       			          break;
       case na_EnumPair : switch (lexeme) 
                                   {
                            	   case ta_string : st.push(na_EnumPair);
			     				   st.push(ta_n);
							   st.push(na_Attr_name_E);
							   st.push(ta_p);
							   st.push(na_Param_name_E);
							   st.push(ta_v);
							   st.push(A_E_1);
							   break;
				   case ta_v :        st.push(ta_c);
			     				   st.push(ta_n);
							   st.push(A_E_4);
							   st.push(ta_string);
							   break;
				   
				   //case ta_c : break;
				   case ta_eps: break;
				   default : result = false;
				   }//switch
       			          break;
       case na_P1 : switch (lexeme) 
                                   {
                            	   case ta_uint    : st.push(ta_n);
							   st.push(na_ParamOrValue);
							   st.push(ta_v);
							   st.push(A_P_1);
							   break;
				   case ta_v       : st.push(ta_n);
				                          st.push(na_ParamOrValue);
							  st.push(A_P_11);
				                          break;
				   default : result = false;
				   }//switch
       			          break;
       case na_P2 : switch (lexeme) 
                                   {
                            	   case ta_uint    : st.push(ta_n);
							   st.push(na_ParamOrValue2);
							   st.push(ta_v);
							   st.push(A_P_2);
							   break;
				   case ta_v       : st.push(ta_n);
				                          st.push(na_ParamOrValue2);
							  st.push(A_P_12);
				                          break;
				   default : result = false;
				   }//switch
       			          break;
       case na_P3 : switch (lexeme) 
                                   {
				   case ta_v       : st.push(ta_n);
				                          st.push(na_Attr_name_P3);
							  st.push(ta_p);
							  st.push(na_Param_name_P3);
				                          break;
				   default : result = false;
				   }//switch
       			          break;
       case na_ParamOrValue : switch (lexeme) 
                                   {
				   case ta_name: st.push(na_Attr_name_P);
							  st.push(ta_p);
							  st.push(A_P_3);
				                          break;
				   case ta_real :   st.push(A_P_5);
				                          break;
				   case ta_uint :   st.push(A_P_5);
				                          break;
				   default : result = false;
				   }//switch
       			          break;
       case na_ParamOrValue2 : switch (lexeme) 
                                   		{
				   		case ta_name: st.push(na_Attr_name_P2);
								      st.push(ta_p);
							  	      st.push(A_P_6);
				                            	      break;
				   		case ta_real :   st.push(A_P_8);
				                		          break;
				   		case ta_uint :   st.push(A_P_8);
				                		          break;
				   		default : result = false;
				   		}//switch
       			          	      break;
       case na_Param_name_V : switch (lexeme) 
                                   		{
				   		case ta_name: st.push(A_V_1);
				                		          break;
				   		default : result = false;
				   		}//switch
       			          	      break;
       case na_Attr_name_V : switch (lexeme) 
                                   		{
				   		case ta_name: st.push(A_V_2);
				                		          break;
				   		default : result = false;
				   		}//switch
       			          	      break;
       case na_Param_name_E : switch (lexeme) 
                                   		{
				   		case ta_name: st.push(A_E_2);
				                		          break;
				   		default : result = false;
				   		}//switch
       			          	      break;
       case na_Attr_name_E : switch (lexeme) 
                                   		{
				   		case ta_name: st.push(A_E_3);
				                		          break;
				   		default : result = false;
				   		}//switch
       			          	      break;
       case na_Attr_name_P : switch (lexeme) 
                                   		{
				   		case ta_name: st.push(A_P_4);
				                		          break;
				   		default : result = false;
				   		}//switch
       			          	      break;
       case na_Attr_name_P2 : switch (lexeme) 
                                   		{
				   		case ta_name: st.push(A_P_7);
				                		          break;
				   		default : result = false;
				   		}//switch
       			          	      break;
       case na_Param_name_P3 : switch (lexeme) 
                                   		{
				   		case ta_name: st.push(A_P_9);
				                		          break;
				   		default : result = false;
				   		}//switch
       			          	      break;
       case na_Attr_name_P3 : switch (lexeme) 
                                   		{
				   		case ta_name: st.push(A_P_10);
				                		          break;
				   		default : result = false;
				   		}//switch
       			          	      break;
       case ta_value : if (lexeme == ta_value) ; else result = false;
       			      break;
       case ta_enum : if (lexeme == ta_enum) ; else result = false;
       			      break;
       case ta_proportion : if (lexeme == ta_proportion) ; else result = false;
       			      break;
       case ta_name : if (lexeme == ta_name) ; else result = false;
       			      break;
       case ta_string : if (lexeme == ta_string) ; else result = false;
       			      break;
       case ta_real : if (lexeme == ta_real) ; else result = false;
       			      break;
       case ta_uint : if (lexeme == ta_uint) st.push(A_V_4); else result = false;
       			      break;
       case ta_format : if (lexeme == ta_format) ; else result = false;
       			      break;
       case ta_p : if (lexeme == ta_p) ; else result = false;
       			      break;
       case ta_n : if (lexeme == ta_n) ; else result = false;
       			      break;
       case ta_v : if (lexeme == ta_v) ; else result = false;
       			      break;
       case ta_o : if (lexeme == ta_o) ; else result = false;
       			      break;
       case ta_c : if (lexeme == ta_c) ; else result = false;
       			      break;
       case ta_eps : if (lexeme == ta_eps) ; else result = false;
       			      break;
       
      }//switch
   
   return result;
}

//выполнение действий, включенных в грамматику:
bool TDynamicProp::parseDoActions(TStack &st, const QString &lexemeValue)
{
   bool res;
   bool was = true;
   bool result = true;
   TVocabulary stTop;
   TDynamicTypeEnumElem elemEnum; //вновь добавляемый элемент для Enum
   TDynamicTypeEnumList::iterator i; //итератор для последнего элемента в списке (для Enum)
   while (was)
   {
   stTop = st.top();
   switch (stTop)
      {
       case A_V :     dynamicType = valueDType;
                           //QMessageBox::warning( NULL, "action", "dynamicType = valueDType");
			   break;
       case A_E :     dynamicType = enumDType;
                           //QMessageBox::warning( NULL, "action", "dynamicType = enumDType");
			   break;
       case A_P :     dynamicType = proportionDType;
                           //QMessageBox::warning( NULL, "action", "dynamicType = proportionDType");
			   break;
       case A_V_1 : dynamicValue.paramName = lexemeValue.mid(1, lexemeValue.length()-2);
                           //QMessageBox::warning( NULL, "action", "dynamicValue.paramName = " + 
			   // lexemeValue.mid(1, lexemeValue.length()-2));
			   break;
       case A_V_2 : dynamicValue.attrName = lexemeValue.mid(1, lexemeValue.length()-2);
                           //QMessageBox::warning( NULL, "action", "dynamicValue.attrName = " + 
			   // lexemeValue.mid(1, lexemeValue.length()-2));
			   break;
       case A_V_3 : dynamicValue.format = lexemeValue[0];
                           //QMessageBox::warning( NULL, "action", "dynamicValue.format = " + lexemeValue + "[0]");
			   break;
       case A_V_4 : dynamicValue.precision = lexemeValue.toUInt(&res);
                           //QMessageBox::warning( NULL, "action", "dynamicValue.precision = " + lexemeValue);
			   if (!res) result = false;
                           break;
       
       case A_E_1 : //TDynamicTypeEnumElem *elem1 = new TDynamicTypeEnumElem();
                           dynamicEnum.dynamicEnumElem.push_back(elemEnum);
			   i = dynamicEnum.dynamicEnumElem.end();
                           i--;
                           (*i).value = lexemeValue.mid(1, lexemeValue.length()-2);
			   //QMessageBox::warning( NULL, "action", "dynamicEnum: adding new element."
			   // "dynamicEnum.value = " + lexemeValue.mid(1, lexemeValue.length()-2));
                           break;
       case A_E_4 : //TDynamicTypeEnumElem *elem1 = new TDynamicTypeEnumElem();
			   dynamicEnum.valueDefault = lexemeValue.mid(1, lexemeValue.length()-2);
			   //QMessageBox::warning( NULL, "action", "dynamicEnum.valueDefault = " 
			   // + lexemeValue.mid(1, lexemeValue.length()-2));
                           break;
       case A_E_2 : i = dynamicEnum.dynamicEnumElem.end();
                           i--;
                           (*i).paramName = lexemeValue.mid(1, lexemeValue.length()-2);
			   //QMessageBox::warning( NULL, "action", "dynamicEnum.paramName = " + 
			   // lexemeValue.mid(1, lexemeValue.length()-2));
                           break;
       case A_E_3 : i = dynamicEnum.dynamicEnumElem.end();
                           i--;
                           (*i).attrName = lexemeValue.mid(1, lexemeValue.length()-2);
			   //QMessageBox::warning( NULL, "action", "dynamicEnum.attrName = " + 
			   // lexemeValue.mid(1, lexemeValue.length()-2));
                           break;
       case A_P_1 : dynamicProportion.yesSize0 = true;
                           dynamicProportion.size0 = (int) lexemeValue.toDouble(&res);
                           //QMessageBox::warning( NULL, "action", "dynamicProportion.yesSize0=true; 
			   // size0 = " + lexemeValue);
			   if (!res) result = false;
			   break;
       case A_P_2 : dynamicProportion.yesSize100 = true;
                           dynamicProportion.size100 = (int) lexemeValue.toDouble(&res);
                           //QMessageBox::warning( NULL, "action", "dynamicProportion.yesSize100=true; 
			   // size100 = " + lexemeValue);
			   if (!res) result = false;
			   break;
       case A_P_3 : dynamicProportion.paramName0 = lexemeValue.mid(1, lexemeValue.length()-2);
                           //QMessageBox::warning( NULL, "action", "dynamicProportion.paramName0 = " 
			   // + lexemeValue.mid(1, lexemeValue.length()-2));
			   break;
       case A_P_4 : dynamicProportion.attrName0 = lexemeValue.mid(1, lexemeValue.length()-2);
                           //QMessageBox::warning( NULL, "action", "dynamicProportion.attrName0 = "
			   // + lexemeValue.mid(1, lexemeValue.length()-2));
			   break;
       case A_P_5 : dynamicProportion.paramName0 = dynamicProportion.attrName0 = "";
                           dynamicProportion.val0 = lexemeValue.toDouble(&res);
			   if (!res) result = false;
			   //QMessageBox::warning( NULL, "action", "dynamicProportion.attrName0=paramName0='';
			   // val0 = '" + lexemeValue + "' '" + QString::number(dynamicProportion.val0)+"'");
                           break;
       case A_P_6 : dynamicProportion.paramName100 = lexemeValue.mid(1, lexemeValue.length()-2);
                           //QMessageBox::warning( NULL, "action", "dynamicProportion.paramName100 = " +
			   // lexemeValue.mid(1, lexemeValue.length()-2));
			   break;
       case A_P_7 : dynamicProportion.attrName100 = lexemeValue.mid(1, lexemeValue.length()-2);
                           //QMessageBox::warning( NULL, "action", "dynamicProportion.attrName100 = " +
			   // lexemeValue.mid(1, lexemeValue.length()-2));
			   break;
       case A_P_8 : dynamicProportion.paramName100 = dynamicProportion.attrName100 = "";
                           dynamicProportion.val100 = lexemeValue.toDouble(&res);
			   if (!res) result = false;
			   //QMessageBox::warning( NULL, "action", "dynamicProportion.attrName100=paramName100='';
			   // val100 = " + lexemeValue);
                           break;
       case A_P_9 : dynamicProportion.paramNameX = lexemeValue.mid(1, lexemeValue.length()-2);
                           //QMessageBox::warning( NULL, "action", "dynamicProportion.paramNameX = " +
			   // lexemeValue.mid(1, lexemeValue.length()-2));
			   break;
       case A_P_10 : dynamicProportion.attrNameX = lexemeValue.mid(1, lexemeValue.length()-2);
                            //QMessageBox::warning( NULL, "action", "dynamicProportion.attrNameX = " +
			    // lexemeValue.mid(1, lexemeValue.length()-2));
			    break;
       case A_P_11 : dynamicProportion.yesSize0 = false;
                           dynamicProportion.size0 = 0;//(int) lexemeValue.toDouble(&res);
                           //QMessageBox::warning( NULL, "action", "dynamicProportion.yesSize0=false; size0 = 0");
			   if (!res) result = false;
			   break;
       case A_P_12 : dynamicProportion.yesSize100 = false;
                           dynamicProportion.size100 = (int) item->getPropValue(propName).toDouble(&res);
			   //(int) lexemeValue.toDouble(&res);
                           //QMessageBox::warning( NULL, "action", "dynamicProportion.yesSize100=false; size100 = "
			   // + item->getPropValue(&propName));
			   if (!res) result = false;
			   break;
       default : was = false;;//result = false;
      }//switch
      if (was)
         st.pop();
    }//while
   return true;
}

bool TDynamicProp::parse(QString s)
{
   s += epsSymbol;//добавляем в анализируемую строку признак конца
   int begin, end;//положение парсера в анализируемой строке
   QString currLexemeValue;//текстовое значение лексемы
   begin = end = 0;
   TStack st;//стек
   TVocabulary currLexeme;//словарь (терминалы, нетерминалы, действия)
   
   st.push(ta_eps);
   st.push(na_S);
   int num = 0;///
   while ( (st.size() > 0) && (begin <= s.length()-1) )
      {
       currLexeme = getLexeme(s, begin, end, currLexemeValue);
       if (currLexeme == ta_unknown)
          {
           throw TVisionError(QString("DynamicProp"), 
	                      QString("syntax error: unknown lexeme at " + 
			      QString::number(begin) + " : " + QString::number(end)));
	   return false;
	  }
       //выполнение правил грамматики (заполнение стека):
       if (!parseDoStack(st, currLexeme))
          {
           throw TVisionError(QString("DynamicProp"), 
	                      QString("syntax error at " + QString::number(begin) + " : " + 
			      QString::number(end)));
	   return false;
	  }
       if ( (st.size()==0) && (currLexeme==ta_eps) )
          return true;
       //выполнение действий, включенных в грамматику
       if (!parseDoActions(st, currLexemeValue))
          {
	   throw TVisionError(QString("DynamicProp"), 
	                      QString("syntax error: unknown action at " + 
			      QString::number(begin) + " : " + QString::number(end)));
	   return false;
	  }
       begin = end;num++;
      }
   //QMessageBox::warning( NULL, "parsing end", QString::number(num) + ": stack top = " + 
   // QString::number(st.top()) + "; lexeme = " + QString::number(currLexeme) + "; currLexemeValue = "
   // + currLexemeValue);
   throw TVisionError(QString("DynamicProp"), 
                      QString("syntax error at " + QString::number(begin) + " : " + QString::number(end)));
   return false;
}
//=====================================================================================

//---------------------------------------TCRectangle-------------------------------------------------------

TCRectangle::TCRectangle (QCanvas *canvas, TVISIONMode m, double z, int x, int y, int width, int height, void *f)
    : TItem(m), QCanvasRectangle(x, y, width, height, canvas)
     
{
   setZ(z);
   QCanvasRectangle::setBrush(QBrush(QColor(0xff, 0xff, 0xff)));
   QCanvasRectangle::setPen(QPen(QColor(0, 0, 0), 1));
   QCanvasRectangle::setSize(width, height);
   QCanvasRectangle::move(x, y);
   
   name = "Rectangle";
}

TCRectangle::~TCRectangle()
{
   //QCanvasRectangle::~QCanvasRectangle();
}

int TCRectangle::RTTI = 10001;
int TCRectangle::rtti() const
{
   return RTTI;
}

bool TCRectangle::getCursorShape(int x, int y, char &resizeItem, int &cursor, int precision)
{
   if (mode == Development)
      {
           // УСЛОВИЯ: ВЫБРАН ОДИН ЭЛЕМЕНТ
	   //если перемещаюсь в пределах границы этого элемента, то изменить форму курсора на соответствующую:
	   resizeItem = resizeNothing;
	   //левый верхний угол?:
	   if ( (x > this->x()-precision) && 
	        (x < this->x()+precision) && (y > this->y()-precision)  && (y < this->y()+precision) )
	      {
	       resizeItem = resizeTopLeft;
	       cursor = Qt::SizeFDiagCursor;
	       return true;
	      }
	   //правый верхний угол?:
	   if ( (x > this->x()+width()-precision) && (x < this->x()+width()+precision)
	        && (y > this->y()-precision)  && (y < this->y()+precision) )
	     {
	       resizeItem = resizeTopRight;
	       cursor = Qt::SizeBDiagCursor;
	       return true;
	      }
	   //левый нижний угол?:
	   if ( (x > this->x()-precision) && (x < this->x()+precision)  && 
	               (y > this->y()+height()-precision)  && (y < this->y()+height()+precision))
	      {
	       resizeItem = resizeBottomLeft;
	       cursor = Qt::SizeBDiagCursor;
	       return true;
	      }
	   //правый нижний угол?:
	   if ( (x > this->x()+width()-precision) && (x < this->x()+width()+precision)  && 
	               (y > this->y()+height()-precision)  && (y < this->y()+height()+precision))
	      {
	       resizeItem = resizeBottomRight;
	       cursor = Qt::SizeFDiagCursor;
	       return true;
	      }
	   //левая сторона?:
	   if ( (x > this->x()-precision) && (x < this->x()+precision) 
	             && (y >= this->y()) && (y <= this->y()+ height()) )
	      {
	       resizeItem = resizeLeft;
	       cursor = Qt::SizeHorCursor;
	       return true;
	      }
	   //правая сторона?:
	   if ( (x > this->x() + width()-precision) && (x < this->x() + width()+precision) 
	               && (y >= this->y()) && (y <= this->y()+ height()) )
	      {
	       resizeItem = resizeRight;
	       cursor = Qt::SizeHorCursor;
	       return true;
	      }
	   //верхняя сторона?:
	   if ( (y > this->y()-precision) && (y < this->y()+precision) 
	             && (x >= this->x()) && (x <= this->x()+ width()) )
	      {
	       resizeItem = resizeTop;
	       cursor = Qt::SizeVerCursor;
	       return true;
	      }
	    //нижняя сторона?:
	   if ( (y > this->y()+height()-precision) && (y < this->y()+height()+precision) 
	             && (x >= this->x()) && (x <= this->x()+ width()) )
	      {
	       resizeItem = resizeBottom;
	       cursor = Qt::SizeVerCursor;
	       return true;
	      }
	   //внутри?:
	   if ( (y >= this->y()) && (y <= this->y()+height()) && (x >= this->x()) && (x <= this->x()+ width()) )
	      {
	       resizeItem = resizeNothing;
	       cursor = Qt::SizeAllCursor;
	       return false;
	      }
   }
	   
   
   return false;
}

bool TCRectangle::resizeOnMouse(int x, int y, int prevX, int prevY, char resizeItem)
{
   //result = true, если изменения размера произошли. 
   // В противном случае result = false, а в TFrameView вызываем OnMouseRelease
   bool result = true;
   
   switch(resizeItem)
          {
	   case resizeTop : 
	           {
		    int d = -(y - prevY);
	            if ( (d >= 0) || ((d < 0) && (height() >= -d)) )
			   {
		            moveBy(0, -d);
		            setSize(width(), height()+d);
		           }
			   else result = false;
		    break;
		   }
	   case resizeBottom : 
	           {
		    int d = (y - prevY);
		    if ( (d >= 0) || ((d < 0) && (height() >= -d)) )
		            setSize(width(), height()+d);
			    else result = false;
		    break;
		   }
	   case resizeLeft : 
	           {
		    int d = -(x - prevX);
		        if ( (d >= 0) || ((d < 0) && (width() >= -d)) )
			   {
		            moveBy(-d, 0);
		            setSize(width()+d, height());
		           }
			   else result = false;
		    break;
		   }
	   case resizeRight : 
	           {
		    int d = (x - prevX);
		    if ( (d >= 0) || ((d < 0) && (width() >= -d)) )
		            setSize(width()+d, height());
			    else result = false;
		    break;
		   }
	   case resizeTopLeft : 
	           {
		    int dx = -(x - prevX);
		    int dy = -(y - prevY);
		    if ( ((dx >= 0) || ((dx < 0) && (width() >= -dx))) && 
		         ((dy >= 0) || ((dy < 0) && (height() >= -dy))) )
			   {
		            moveBy(-dx, -dy);
		            setSize(width()+dx, height()+dy);
		           }
			   else result = false;
		    break;
		   }
	   case resizeBottomRight : 
	           {
		    int dx = (x - prevX);
		    int dy = (y - prevY);
                    if ( ((dx >= 0) || ((dx < 0) && (width() >= -dx))) && 
		        ((dy >= 0) || ((dy < 0) && (height() >= -dy))) )
		            setSize(width()+dx, height()+dy);
			   else result = false;
		    break;
		   }
	   case resizeTopRight : 
	           {
		    int dx = (x - prevX);
		    int dy = -(y - prevY);
		    if ( ((dx >= 0) || ((dx < 0) && (width() >= -dx))) && 
		         ((dy >= 0) || ((dy < 0) && (height() >= -dy))) )
			   {
		            moveBy(0, -dy);
		            setSize(width()+dx, height()+dy);
		           }
			   else result = false;
		    break;
		   }
	   case resizeBottomLeft : 
	           {
		    int dx = -(x - prevX);
		    int dy = (y - prevY);
		    if ( ((dx >= 0) || ((dx < 0) && (width() >= -dx))) && 
		         ((dy >= 0) || ((dy < 0) && (height() >= -dy))) )
			   {
		            moveBy(-dx, 0);
		            setSize(width()+dx, height()+dy);
		           }
			   else result = false;
		    break;
		   }
	   
	   default : break;
	  } //switch
   
   return result;
}

//Изменение размеров элемента мышью (true - если изменения произошли, false - если нет 
// (н-р, достигли минимального значения)):
bool TCRectangle::moveOnMouse(int x, int y, int prevX, int prevY)
{
   moveBy(x - prevX, y - prevY);
   return true;
}

void TCRectangle::setSelectedItem (bool yes)
{
   setSelected(yes);
}

void TCRectangle::showItem()
{
   show();
}

bool TCRectangle::isSelectedItem()
{
   return isSelected();
}

double TCRectangle::zItem() const
{
   return z();
}

void TCRectangle::setZItem(double z)
{
   setZ(z);
}

void TCRectangle::moveByItem (double dx, double dy)
{
   moveBy(dx, dy);
}

void TCRectangle::drawShape ( QPainter & p )
{
   p.drawRect((int)QCanvasRectangle::x(), (int)QCanvasRectangle::y(), 
              QCanvasRectangle::width(), QCanvasRectangle::height());

   if (isSelectedItem())
      {
       QPen pen = p.pen();
       QBrush brush = p.brush();
       p.setPen(QPen(QColor(0x0, 0x0, 0x0), 1));
       p.setBrush(QBrush(QColor(0x0, 0xff, 0x0)));
       p.drawRect((int)QCanvasRectangle::x()-2, (int)QCanvasRectangle::y()-2, 5, 5);
       p.drawRect((int)QCanvasRectangle::x()+QCanvasRectangle::width()-2-1, (int)QCanvasRectangle::y()-2, 5, 5);
       p.drawRect((int)QCanvasRectangle::x()-2, (int)QCanvasRectangle::y()+QCanvasRectangle::height()-2-1, 5, 5);
       p.drawRect((int)QCanvasRectangle::x()+QCanvasRectangle::width()-1-2,
                  (int)QCanvasRectangle::y()+QCanvasRectangle::height()-1-2, 5, 5);
       p.drawRect((int)(QCanvasRectangle::x()+QCanvasRectangle::width()/2-2), (int)QCanvasRectangle::y()-2, 5, 5);
       p.drawRect((int)(QCanvasRectangle::x()+QCanvasRectangle::width()/2-2),
                  (int)QCanvasRectangle::y()+QCanvasRectangle::height()-1-2, 5, 5);
       p.drawRect((int)QCanvasRectangle::x()-2, (int)(QCanvasRectangle::y()+QCanvasRectangle::height()/2-2), 5, 5);
       p.drawRect((int)QCanvasRectangle::x()+QCanvasRectangle::width()-1-2,
                  (int)(QCanvasRectangle::y()+QCanvasRectangle::height()/2-2), 5, 5);
       p.setPen(pen);
       p.setBrush(brush);
      }
}

QPointArray TCRectangle::areaPoints () const
{
   QPointArray p = QCanvasRectangle::areaPoints();
   p[0] = p[0] + QPoint(-2, -2);
   p[1] = p[1] + QPoint(2, -2);
   p[2] = p[2] + QPoint(2, 2);
   p[3] = p[3] + QPoint(-2, 2);
   return p;
}

void TCRectangle::setVisibleProp (bool yes)
{
   visibleItem = yes;
   if (mode == Runtime)
      setVisible(yes);
}

TListOfString TCRectangle::getPropCategories()
{
   TListOfString result;
   result.push_back(("Item"));
   result.push_back(("Geometry"));
   result.push_back(("Colors"));
   result.push_back(("Styles"));
   result.push_back(("Blinkings"));

   return result;
}

TListOfString TCRectangle::getPropNames(const QString *category)
{
   TListOfString result;
   if (category == NULL)
      {
       return result;
      }
   
   if (*category == QString("Item"))
      {
       result.push_back(("ItemName"));
      }
      
   if (!strcmp(category->ascii(), ("Geometry")))
      {
       result.push_back(("x"));
       result.push_back(("y"));
       result.push_back(("width"));
       result.push_back(("height"));
       result.push_back(("visible"));
       result.push_back(("angle"));
      }

   if (!strcmp(category->ascii(), ("Colors")))
      {
       result.push_back(("LineColor"));
       result.push_back(("BackgroundColor"));
      }
   
   if (!strcmp(category->ascii(), ("Styles")))
       result.push_back(("LineStyle"));
   if (!strcmp(category->ascii(), ("Styles")))
       result.push_back(("BrushStyle"));
   if (!strcmp(category->ascii(), ("Styles")))
       result.push_back(("LineWeight"));
   if (!strcmp(category->ascii(), ("Styles")))
       result.push_back(("LineCap"));
   if (!strcmp(category->ascii(), ("Styles")))
       result.push_back(("LineJoin"));   
   
   return result;
}

TPropType TCRectangle::getPropType(const QString *propertyName)
{
   if (*propertyName == QString("ItemName"))
      return stringType;
   if (*propertyName == QString("x"))
      return intType;
   if (*propertyName == QString("y"))
      return intType;
   if (*propertyName == QString("width"))
      return intType;
   if (*propertyName == QString("height"))
      return intType;
   if (*propertyName == QString("angle"))
      return intType;
   if (*propertyName == QString("LineWeight"))
      return intType;
   if (*propertyName == QString("visible"))
      return enumType;
   if (*propertyName == QString("LineStyle"))
      return enumType;
   if (*propertyName == QString("BrushStyle"))
      return enumType;
   if (*propertyName == QString("LineCap"))
      return enumType;
   if (*propertyName == QString("LineJoin"))
      return enumType;
   if (*propertyName == QString("LineColor"))
      return colorType;
   if (*propertyName == QString("BackgroundColor"))
      return colorType;
   
   return unknownType;
}

TListOfString TCRectangle::getPropEnumTypeValues(const QString *propertyName)
{
   TListOfString result;
   if (*propertyName == QString("visible"))
      {
       result.push_back(("true"));
       result.push_back(("false"));
      }
   if (*propertyName == QString("LineStyle"))
      {
       result.push_back(("NoPen"));
       result.push_back(("SolidLine"));
       result.push_back(("DashLine"));
       result.push_back(("DotLine"));
       result.push_back(("DashDotLine"));
       result.push_back(("DashDotDotLine"));
      }
   if (*propertyName == QString("BrushStyle"))
      {
       result.push_back(("NoBrush"));
       result.push_back(("SolidPattern"));
       result.push_back(("Dense1Pattern"));
       result.push_back(("Dense2Pattern"));
       result.push_back(("Dense3Pattern"));
       result.push_back(("Dense4Pattern"));
       result.push_back(("Dense5Pattern"));
       result.push_back(("Dense6Pattern"));
       result.push_back(("Dense7Pattern"));
       result.push_back(("HorPattern"));
       result.push_back(("VerPattern"));
       result.push_back(("CrossPattern"));
       result.push_back(("BDiagPattern"));
       result.push_back(("FDiagPattern"));
       result.push_back(("DiagCrossPattern"));
       result.push_back(("CustomPattern"));
      }
   if (*propertyName == QString("LineCap"))
      {
       result.push_back(("FlatCap"));
       result.push_back(("SquareCap"));
       result.push_back(("RoundCap"));
      }
   if (*propertyName == QString("LineJoin"))
      {
       result.push_back(("MiterJoin"));
       result.push_back(("BevelJoin"));
       result.push_back(("RoundJoin"));
      }
   return result;
}

QString TCRectangle::getPropValue(const QString &propertyName)
{
   QString result;
   if (*propertyName == QString("ItemName"))
      return name;
   if (*propertyName == QString("x"))
      return QString::number((int)x());
   if (*propertyName == QString("y"))
      return QString::number((int)y());
   if (*propertyName == QString("width"))
      return QString::number(width());
   if (*propertyName == QString("height"))
      return QString::number(height());
   if (*propertyName == QString("angle"))
      return QString::number(0);//height());
   if (*propertyName == QString("LineWeight"))
      return QString::number(pen().width());
   if (*propertyName == QString("visible"))
      if (isVisibleProp())
         return QString(("true"));
	 else return QString(("false"));
   if (*propertyName == QString("LineColor"))
      return QString::number(pen().color().rgb(), 10);//pen().color().name();
   if (*propertyName == QString("BackgroundColor"))
      return QString::number(brush().color().rgb(), 10);
   if (*propertyName == QString("LineStyle"))
      switch (pen().style())
      {
       case Qt::NoPen : return QString("NoPen");
                                 break;
       case Qt::SolidLine : return QString("SolidLine");
                                 break;
       case Qt::DashLine : return QString("DashLine");
                                 break;
       case Qt::DotLine : return QString("DotLine");
                                 break;
       case Qt::DashDotLine : return QString("DashDotLine");
                                 break;
       case Qt::DashDotDotLine : return QString("DashDotDotLine");
                                 break;
       
       default : break;
      }
    if (*propertyName == QString("BrushStyle"))
      switch (brush().style())
      {
       case Qt::NoBrush : return QString("NoBrush");
                                 break;
       case Qt::SolidPattern : return QString("SolidPattern");
                                 break;
       case Qt::Dense1Pattern : return QString("Dense1Pattern");
                                 break;
       case Qt::Dense2Pattern : return QString("Dense2Pattern");
                                 break;
       case Qt::Dense3Pattern : return QString("Dense3Pattern");
                                 break;
       case Qt::Dense4Pattern : return QString("Dense4Pattern");
                                 break;
       case Qt::Dense5Pattern : return QString("Dense5Pattern");
                                 break;
       case Qt::Dense6Pattern : return QString("Dense6Pattern");
                                 break;
       case Qt::Dense7Pattern : return QString("Dense7Pattern");
                                 break;
       case Qt::HorPattern : return QString("HorPattern");
                                 break;
       case Qt::VerPattern : return QString("VerPattern");
                                 break;
       case Qt::CrossPattern : return QString("CrossPattern");
                                 break;
       case Qt::BDiagPattern : return QString("BDiagPattern");
                                 break;
       case Qt::FDiagPattern : return QString("FDiagPattern");
                                 break;
       case Qt::DiagCrossPattern : return QString("DiagCrossPattern");
                                 break;
       case Qt::CustomPattern : return QString("CustomPattern");
                                 break;
       
       default : break;
      }
    if (*propertyName == QString("LineCap"))
      switch (pen().capStyle())
      {
       case Qt::FlatCap : return QString("FlatCap");
                                 break;
       case Qt::SquareCap : return QString("SquareCap");
                                 break;
       case Qt::RoundCap : return QString("RoundCap");
                                 break;
       
       default : break;
      }
   if (*propertyName == QString("LineJoin"))
      switch (pen().joinStyle())
      {
       case Qt::MiterJoin : return QString("MiterJoin");
                                 break;
       case Qt::BevelJoin : return QString("BevelJoin");
                                 break;
       case Qt::RoundJoin : return QString("RoundJoin");
                                 break;
       
       default : break;
      }
   
   return result;
}

//изменение значения свойства;
bool TCRectangle::setPropValue(const QString &propertyName, const QString &newValue, const bool doIt)
{
   bool result = false;
   
   if (propertyName == "ItemName")
      {
       if (doIt) name = newValue;
       return true;
      }
   if (propertyName == "x")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          setX(newVal);
       return result;
      }
   if (propertyName == "y")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          setY(newVal);
       return result;
      }
   if (propertyName == "z")
      {
       double newVal = newValue.toDouble(&result);
       if (result && doIt)
          setZItem(newVal);
       return result;
      }
   if (propertyName == "width")
      {
       unsigned int newVal = newValue.toUInt(&result, 10);
       //result= result && ()
       if (result && doIt)
          setSize(newVal, height());
       return result;
      }
   if (propertyName == "height")
      {
       unsigned int newVal = newValue.toUInt(&result, 10);
       if (result && doIt)
          setSize(width(), newVal);
       return result;
      }
   if (propertyName == "angle")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          ;//setSize(width(), newVal);
       return result;
      }
   if (propertyName == "LineWeight")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          {
	   QPen p = pen();
	   p.setWidth(newVal);
	   setPen(p);
	  }
       return result;
      }
   if (propertyName == "visible")
       if (newValue == "false")
          {
           if (doIt) setVisibleProp(false);
	   return true;
	  }
	  else if (newValue == "true")
          {
           if (doIt) setVisibleProp(true);
	   return true;
	  }
   if (propertyName == "BackgroundColor")
      {
       int newVal = newValue.toULong(&result);
       if (result && doIt)
          {
           QBrush b = brush();
	   b.setColor(QColor(QRgb(newVal)));
	   setBrush(b);
	  }
       return result;
      }
   if (propertyName == "LineColor")
      {
       int newVal = newValue.toULong(&result);
       if (result && doIt)
          {
           QPen p = pen();
	   p.setColor(QColor(QRgb(newVal)));
	   setPen(p);
	  }
       return result;
      }
   if (propertyName == "LineStyle")
      {QPen p = pen();
       if (newValue == "NoPen")
          {
           p.setStyle(Qt::NoPen);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "SolidLine")
          {
           p.setStyle(Qt::SolidLine);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "DashLine")
          {
           p.setStyle(Qt::DashLine);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "DotLine")
          {
           p.setStyle(Qt::DotLine);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "DashDotLine")
          {
           p.setStyle(Qt::DashDotLine);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "DashDotDotLine")
          {
           p.setStyle(Qt::DashDotDotLine);
	   if (doIt) setPen(p);
	   return true;
	  }
      }
   if (propertyName == "BrushStyle")
      {QBrush b = brush();
       if (newValue == "NoBrush")
          {
           b.setStyle(Qt::NoBrush);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "SolidPattern")
          {
           b.setStyle(Qt::SolidPattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "Dense1Pattern")
          {
           b.setStyle(Qt::Dense1Pattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "Dense2Pattern")
          {
           b.setStyle(Qt::Dense2Pattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "Dense3Pattern")
          {
           b.setStyle(Qt::Dense3Pattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "Dense4Pattern")
          {
           b.setStyle(Qt::Dense4Pattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "Dense5Pattern")
          {
           b.setStyle(Qt::Dense5Pattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "Dense6Pattern")
          {
           b.setStyle(Qt::Dense6Pattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "Dense7Pattern")
          {
           b.setStyle(Qt::Dense7Pattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "HorPattern")
          {
           b.setStyle(Qt::HorPattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "VerPattern")
          {
           b.setStyle(Qt::VerPattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "CrossPattern")
          {
           b.setStyle(Qt::CrossPattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "BDiagPattern")
          {
           b.setStyle(Qt::BDiagPattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "FDiagPattern")
          {
           b.setStyle(Qt::FDiagPattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "DiagCrossPattern")
          {
           b.setStyle(Qt::DiagCrossPattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       if (newValue == "CustomPattern")
          {
           b.setStyle(Qt::CustomPattern);
	   if (doIt) setBrush(b);
	   return true;
	  }
       
      }
   if (propertyName == "LineCap")
      {QPen p = pen();
       if (newValue == "FlatCap")
          {
           p.setCapStyle(Qt::FlatCap);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "SquareCap")
          {
           p.setCapStyle(Qt::SquareCap);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "RoundCap")
          {
           p.setCapStyle(Qt::RoundCap);
	   if (doIt) setPen(p);
	   return true;
	  }
       }
   if (propertyName == "LineJoin")
      {QPen p = pen();
       if (newValue == "MiterJoin")
          {
           p.setJoinStyle(Qt::MiterJoin);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "BevelJoin")
          {
           p.setJoinStyle(Qt::BevelJoin);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "RoundJoin")
          {
           p.setJoinStyle(Qt::RoundJoin);
	   if (doIt) setPen(p);
	   return true;
	  }
      }
   
   return result;
}

//----------------------------------------TCLine---------------------------------------------------------

TCLine::TCLine (QCanvas *canvas, TVISIONMode m, double z, int x, int y, int width, int height, void *f)
   : TItem(m),
     QCanvasLine(canvas)
{
   setZ(z);
   QCanvasLine::setPen(QPen(QColor(0, 0, 0), 1));
   
   setSize(0, 0);
   x1 = x2 = y1 = y2 = 0;
   x1 = x;
   y2 = y;
   x2 = x1 + width;
   y1 = y2 + height;

   setPoints(x1, y1, x2, y2);
   show();
   
   name = "Line";
}

TCLine::~TCLine()
{
   //QCanvasLine::~QCanvasLine();
}

int TCLine::RTTI = 10002;
int TCLine::rtti() const
{
   return RTTI;
}

void TCLine::setSize(int width, int height)
{
   if (x1 <= x2)
      x2 += width - this->width();
      else x1 += width - this->width();
   if (y1 > y2)
      y2 -= height - this->height();
      else y1 -= height - this->height();
   
   setPoints(x1, y1, x2, y2);
}

int TCLine::width() const
{
   return abs(x1-x2);
}

int TCLine::height()const
{
   return abs(y1-y2);
}

void TCLine::moveBy(double dx, double dy)
{
   QCanvasLine::moveBy(dx, dy);
}

void TCLine::moveByItem(double dx, double dy)
{
   this->moveBy(dx, dy);
}

void TCLine::showItem()
{
   show();
};

bool TCLine::isSelectedItem()
{
   return isSelected();
};

void TCLine::setSelectedItem(bool yes)
{
   setSelected(yes);
};

double TCLine::zItem() const
{
   return z();
}

void TCLine::setZItem(double z)
{
   setZ(z);
}

void TCLine::drawShape (QPainter & p)
{
   //p.drawLine(x1, y1, x2, y2);
   QCanvasLine::drawShape(p);
   if (QCanvasLine::isSelected())
      {
       QPen pen = p.pen();
       QBrush brush = p.brush();
       p.setPen(QPen(QColor(0x0, 0x0, 0x0), 1));
       p.setBrush(QBrush(QColor(0x0, 0xff, 0x0)));
       int w = 0;//pen().width();
       //w <= 1 ? w = 0;
       //w /= 2;
       p.drawRect((int)(x()+x1-(2+w)), (int)(y()+y1-(2+w)), 5, 5);
       p.drawRect((int)(x()+x2-(2+w)), (int)(y()+y2-(2+w)), 5, 5);

       p.setPen(pen);
       p.setBrush(brush);
      }
}

QPointArray TCLine::areaPoints () const
{
   QPointArray p = QCanvasLine::areaPoints();
   return p;
}

void TCLine::setVisibleProp (bool yes)
{
   visibleItem = yes;
   if (mode == Runtime)
      setVisible(yes);
}

bool TCLine::getCursorShape(int x, int y, char &resizeItem, int &cursor, int precision)
{
   if (mode == Development)
      {
           // УСЛОВИЯ: ВЫБРАН ОДИН ЭЛЕМЕНТ
	   //если перемещаюсь в пределах границы этого элемента, то изменить форму курсора на соответствующую:
	   resizeItem = resizeNothing;
	   //Первый конец?:
	   if ( (x > this->x()+x1-precision) && (x < this->x()+x1+precision) && 
	        (y > this->y()+y1-precision)  && (y < this->y()+y1+precision) )
	      {
	       resizeItem = resizeTopLeft;
	       cursor = Qt::CrossCursor;
	       return true;
	      }
	   //Второй конец?:
	   if ( (x > this->x()+x2-precision) && (x < this->x()+x2+precision) && 
	        (y > this->y()+y2-precision)  && (y < this->y()+y2+precision) )
	      {
	       resizeItem = resizeBottomRight;
	       cursor = Qt::CrossCursor;
	       return true;
	      }
	   //внутри?:
	   int y1_ = y1 <= y2 ? y1 : y2;
	   int x1_ = x1 <= x2 ? x1 : x2;
	   int y2_ = y1 <= y2 ? y2 : y1;
	   int x2_ = x1 <= x2 ? x2 : x1;
	   //уравнение прямой, проходящей через две точки:
	   int yLine = (int)(x2-x1 != 0 ? ((x-(x1+this->x()))*(y2-y1)+(y1+this->y())*(x2-x1))/(x2-x1) : y1); 
	   int xLine = (int)(y2-y1 != 0 ? ((y-(y1+this->y()))*(x2-x1)+(x1+this->x())*(y2-y1))/(y2-y1) : x1);
	   if ( (y >= this->y()+y1_) && (y <= this->y()+y2_) && (x >= this->x()+x1_) && (x <= this->x()+x2_) 
	                   && ( (abs(y-yLine) <= (1+pen().width())) || (abs(x-xLine) <= (1+pen().width())) ) )
	      {
	       resizeItem = resizeNothing;
	       cursor = Qt::SizeAllCursor;
	       return false;
	      }
   }
   
   return false;
}

bool TCLine::resizeOnMouse(int x, int y, int prevX, int prevY, char resizeItem)
{
bool result = true;
   
   switch(resizeItem)
          {
	   case resizeTopLeft : 
	           {
		    x1 = (int)(x-this->x());
		    y1 = (int)(y-this->y());
		    break;
		   }
	   case resizeBottomRight : 
	           {
		    x2 = (int)(x-this->x());
		    y2 = (int)(y-this->y());
		    break;
		   } 
	   default : break;
	  } //switch
   
   setPoints(x1, y1, x2, y2);
   return result;
}

//Изменение размеров элемента мышью (true - если изменения произошли, false - если нет 
// (н-р, достигли минимального значения)):
bool TCLine::moveOnMouse(int x, int y, int prevX, int prevY)
{
   moveBy(x - prevX, y - prevY);
   return true;
}

TListOfString TCLine::getPropCategories()
{
   TListOfString result;
   result.push_back(("Item"));
   result.push_back(("Geometry"));
   result.push_back(("Colors"));
   result.push_back(("Styles"));
   result.push_back(("Blinkings"));

   return result;
}

TListOfString TCLine::getPropNames(const QString *category)
{
   TListOfString result;
   if (category == NULL)
      {
       return result;
      }
   
   if (*category == QString("Item"))
      {
       result.push_back(("ItemName"));
      }
      
   if (!strcmp(category->ascii(), ("Geometry")))
      {
       result.push_back(("x"));
       result.push_back(("y"));
       result.push_back(("width"));
       result.push_back(("height"));
       result.push_back(("visible"));
       result.push_back(("angle"));
      }

   if (!strcmp(category->ascii(), ("Colors")))
       result.push_back(("LineColor"));
   
   if (!strcmp(category->ascii(), ("Styles")))
       result.push_back(("LineStyle"));
      
   if (!strcmp(category->ascii(), ("Styles")))
       result.push_back(("LineWeight"));
   if (!strcmp(category->ascii(), ("Styles")))
       result.push_back(("LineCap"));
   //if (!strcmp(category->ascii(), ("Styles")))
   //    result.push_back(("LineJoin"));
   
   return result;
}

TPropType TCLine::getPropType(const QString *propertyName)
{
   if (*propertyName == QString("ItemName"))
      return stringType;
   if (*propertyName == QString("x"))
      return intType;
   if (*propertyName == QString("y"))
      return intType;
   if (*propertyName == QString("width"))
      return intType;
   if (*propertyName == QString("height"))
      return intType;
   if (*propertyName == QString("angle"))
      return intType;
   if (*propertyName == QString("LineWeight"))
      return intType;
   if (*propertyName == QString("visible"))
      return enumType;
   if (*propertyName == QString("LineStyle"))
      return enumType;
   if (*propertyName == QString("LineCap"))
      return enumType;
   //if (*propertyName == QString("LineJoin"))
   //   return enumType;
   if (*propertyName == QString("LineColor"))
      return colorType;
   
   return unknownType;
}

TListOfString TCLine::getPropEnumTypeValues(const QString *propertyName)
{
   TListOfString result;
   if (*propertyName == QString("visible"))
      {
       result.push_back(("true"));
       result.push_back(("false"));
      }
   if (*propertyName == QString("LineStyle"))
      {
       result.push_back(("NoPen"));
       result.push_back(("SolidLine"));
       result.push_back(("DashLine"));
       result.push_back(("DotLine"));
       result.push_back(("DashDotLine"));
       result.push_back(("DashDotDotLine"));
      }
   if (*propertyName == QString("LineCap"))
      {
       result.push_back(("FlatCap"));
       result.push_back(("SquareCap"));
       result.push_back(("RoundCap"));
      }
   return result;
}

QString TCLine::getPropValue(const QString &propertyName)
{
   QString result;
   if (*propertyName == QString("ItemName"))
      return name;
   if (*propertyName == QString("x"))
      return QString::number((int)x()+x1);
   if (*propertyName == QString("y"))
      return QString::number((int)y()+y1);
   if (*propertyName == QString("width"))
      return QString::number(width());
   if (*propertyName == QString("height"))
      return QString::number(height());
   if (*propertyName == QString("angle"))
      return QString::number(0);//height());
   if (*propertyName == QString("LineWeight"))
      return QString::number(pen().width());
   if (*propertyName == QString("visible"))
      if (isVisibleProp())
         return QString(("true"));
	 else return QString(("false"));
   if (*propertyName == QString("LineColor"))
      return QString::number(pen().color().rgb(), 10);
   if (*propertyName == QString("LineStyle"))
      switch (pen().style())
      {
       case Qt::NoPen : return QString("NoPen");
                                 break;
       case Qt::SolidLine : return QString("SolidLine");
                                 break;
       case Qt::DashLine : return QString("DashLine");
                                 break;
       case Qt::DotLine : return QString("DotLine");
                                 break;
       case Qt::DashDotLine : return QString("DashDotLine");
                                 break;
       case Qt::DashDotDotLine : return QString("DashDotDotLine");
                                 break;
       
       default : break;
      }
    if (propertyName == "LineCap")
      switch (pen().capStyle())
      {
       case Qt::FlatCap : return QString("FlatCap");
                                 break;
       case Qt::SquareCap : return QString("SquareCap");
                                 break;
       case Qt::RoundCap : return QString("RoundCap");
                                 break;
       
       default : break;
      }
     
   return result;
}

//изменение значения свойства;
bool TCLine::setPropValue(const QString &propertyName, const QString &newValue, const bool doIt)
{
   bool result = false;
   
   if (propertyName == "ItemName")
      {
       if (doIt) name = *newValue;
       return true;
      }
   if (propertyName == "x")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          moveBy(newVal-(x()+x1), 0);//setX(newVal);
       return result;
      }
   if (propertyName == "y")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          moveBy(0, newVal-(y()+y1));
       return result;
      }
   if (propertyName == "z")
      {
       int newVal = newValue.toInt(&result);
       if (result && doIt)
          setZItem(newVal);
       return result;
      }
   if (propertyName == "width")
      {
       int newVal = newValue.toUInt(&result, 10);
       if (result && doIt)
          setSize(newVal, height());
       return result;
      }
   if (propertyName == "height")
      {//QMessageBox::information(NULL, "", name + ": " + *newValue);
       int newVal = newValue.toUInt(&result, 10);
       if (result && doIt)
          setSize(width(), newVal);
       return result;
      }
   if (propertyName == "angle")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          ;//setSize(width(), newVal);
       return result;
      }
   if (propertyName == "LineWeight")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          {
	   QPen p = pen();
	   p.setWidth(newVal);
	   setPen(p);
	  }
       return result;
      }
   if (propertyName == "visible")
       if (newValue == "false")
          {
           if (doIt) setVisibleProp(false);
	   return true;
	  }
	  else if (newValue == "true")
          {
           if (doIt) setVisibleProp(true);
	   return true;
	  }
   if (propertyName == "LineColor")
      {
       int newVal = newValue.toULong(&result);
       if (result && doIt)
          {
           QPen p = pen();
	   p.setColor(QColor(QRgb(newVal)));
	   setPen(p);
	  }
       return result;
      }
   if (propertyName == "LineStyle")
      {QPen p = pen();
       if (newValue == "NoPen")
          {
           p.setStyle(Qt::NoPen);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "SolidLine")
          {
           p.setStyle(Qt::SolidLine);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "DashLine")
          {
           p.setStyle(Qt::DashLine);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "DotLine")
          {
           p.setStyle(Qt::DotLine);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "DashDotLine")
          {
           p.setStyle(Qt::DashDotLine);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "DashDotDotLine")
          {
           p.setStyle(Qt::DashDotDotLine);
	   if (doIt) setPen(p);
	   return true;
	  }
      }
if (propertyName == "LineCap")
      {QPen p = pen();
       if (newValue == "FlatCap")
          {
           p.setCapStyle(Qt::FlatCap);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "SquareCap")
          {
           p.setCapStyle(Qt::SquareCap);
	   if (doIt) setPen(p);
	   return true;
	  }
       if (newValue == "RoundCap")
          {
           p.setCapStyle(Qt::RoundCap);
	   if (doIt) setPen(p);
	   return true;
	  }
       }
   
   return result;
}

//-------------------------------------TCText-------------------------------------------------------
TCText::TCText(QCanvas *canvas, TVISIONMode m, double z, int x, int y, const QString& text, void *f)
   : TItem(m), QCanvasText(text, canvas)
{
   setZ(z);
   QCanvasText::setColor(QColor(0xf, 0xf, 0xf));
   //QCanvasText::setText("text");
   QCanvasText::move(x, y);
   name = "Text";
}

TCText::~TCText()
{
   //QCanvasText::~QCanvasText();
}

int TCText::RTTI = 10003;
int TCText::rtti() const
{
   return RTTI;
}

void TCText::setVisibleProp (bool yes)
{
   visibleItem = yes;
   if (mode == Runtime)
      setVisible(yes);
}

void TCText::setSelectedItem (bool yes)
{
   setSelected(yes);
}

bool TCText::isSelectedItem()
{
   return isSelected();
}

void TCText::showItem()
{
   show();
}

double TCText::zItem() const
{
   return z();
}

void TCText::setZItem(double z)
{
   setZ(z);
}

void TCText::moveByItem (double dx, double dy)
{
   moveBy(dx, dy);
}

void TCText::draw(QPainter & p)
{
   QCanvasText::draw(p);
   if (isSelectedItem())
      {
       QPen pen = p.pen();
       QBrush brush = p.brush();
       p.setPen(QPen(QColor(0x0, 0x0, 0x0), 1, Qt::DotLine));
       p.setBrush(QBrush(QColor(0x0, 0x0, 0x0), Qt::NoBrush));
       p.drawRect(QCanvasText::boundingRect());
       
       p.setPen(pen);
       p.setBrush(brush);
      }
}

bool TCText::getCursorShape(int x, int y, char &resizeItem, int &cursor, int precision)
{
   if (mode == Development)
      {
           // УСЛОВИЯ: ВЫБРАН ОДИН ЭЛЕМЕНТ
	   //если перемещаюсь в пределах границы этого элемента, то изменить форму курсора на соответствующую:
	   resizeItem = resizeNothing;
	   //левый верхний угол?:
	   QRect r = QCanvasText::boundingRect();
	   if ( (x >= r.x()) && (x <= r.right()) && (y >= r.y())  && (y <= r.bottom()) )
	      {
	       resizeItem = resizeNothing;
	       cursor = Qt::SizeAllCursor;
	       return false;
	      }
      }

   return false;
}

bool TCText::resizeOnMouse(int x, int y, int prevX, int prevY, char resizeItem)
{
   //result = true, если изменения размера произошли. 
   // В противном случае result = false, а в TFrameView вызываем OnMouseRelease
   bool result = false;
   return result;
}

//Изменение размеров элемента мышью (true - если изменения произошли, false - если нет 
// (н-р, достигли минимального значения)):
bool TCText::moveOnMouse(int x, int y, int prevX, int prevY)
{
   moveBy(x - prevX, y - prevY);
   return true;
}

TListOfString TCText::getPropCategories()
{
   TListOfString result;
   result.push_back(("Item"));
   result.push_back(("Geometry"));
   result.push_back(("Colors"));
   //result.push_back(("Styles"));
   result.push_back(("Blinkings"));
   result.push_back(("Font"));

   return result;
}

TListOfString TCText::getPropNames(const QString *category)
{
   TListOfString result;
   if (category == NULL)
      {
       return result;
      }
   
   if (*category == QString("Item"))
      {
       result.push_back(("ItemName"));
      }
      
   if (!strcmp(category->ascii(), ("Geometry")))
      {
       result.push_back(("x"));
       result.push_back(("y"));
       result.push_back(("width"));
       result.push_back(("height"));
       result.push_back(("visible"));
       result.push_back(("angle"));
      }

   if (!strcmp(category->ascii(), ("Colors")))
      {
       result.push_back(("TextColor"));
      }
   
   if (!strcmp(category->ascii(), ("Font")))
      {
       result.push_back(("Text"));
       result.push_back(("family"));
       result.push_back(("PointSize"));
       result.push_back(("Bold"));
       result.push_back(("Italic"));
       result.push_back(("Underline"));
       result.push_back(("Strikeout"));
      }
   
   return result;
}

TPropType TCText::getPropType(const QString *propertyName)
{
   if (*propertyName == QString("ItemName"))
      return stringType;
   if (*propertyName == QString("x"))
      return intType;
   if (*propertyName == QString("y"))
      return intType;
   if (*propertyName == QString("width"))
      return intType;
   if (*propertyName == QString("height"))
      return intType;
   if (*propertyName == QString("angle"))
      return intType;
   if (*propertyName == QString("visible"))
      return enumType;
   if (*propertyName == QString("TextColor"))
      return colorType;
   if (*propertyName == QString("Text"))
      return stringType;
   if (*propertyName == QString("family"))
      return enumType;
   if (*propertyName == QString("PointSize"))
      return intType;
   if ( (*propertyName == QString("Bold")) || (*propertyName == QString("Italic")) || 
        (*propertyName == QString("Underline")) || (*propertyName == QString("Strikeout")) )
      return enumType;
   
   return unknownType;
}

TListOfString TCText::getPropEnumTypeValues(const QString *propertyName)
{
   TListOfString result;
   if ( (*propertyName == QString("visible")) || (*propertyName == QString("Bold")) || 
        (*propertyName == QString("Italic")) || (*propertyName == QString("Underline")) ||
	(*propertyName == QString("Strikeout")) )
      {
       result.push_back(("true"));
       result.push_back(("false"));
      }
   if (*propertyName == QString("family"))
      {
       QStringList l = QFontDatabase().families();
       for (QStringList::Iterator i = l.begin(); i != l.end(); i++)
          result.push_back(*i);
      }
   return result;
}

QString TCText::getPropValue(const QString &propertyName)
{
   QString result;
   if (*propertyName == QString("ItemName"))
      return name;
   if (*propertyName == QString("x"))
      return QString::number((int)x());
   if (*propertyName == QString("y"))
      return QString::number((int)y());
   if (*propertyName == QString("width"))
      return QString::number(QCanvasText::boundingRect().width());
   if (*propertyName == QString("height"))
      return QString::number(QCanvasText::boundingRect().height());
   if (*propertyName == QString("angle"))
      return QString::number(0);//QCanvasText::boundingRect().height());
   if (*propertyName == QString("Text"))
      return text();
   if (*propertyName == QString("visible"))
      if (isVisibleProp())
         return QString(("true"));
	 else return QString(("false"));
   if (*propertyName == QString("TextColor"))
      return QString::number(color().rgb(), 10);
   if (*propertyName == QString("family"))
      return QString(font().family());
   if (*propertyName == QString("PointSize"))
      return QString::number(font().pointSize());
   if (*propertyName == QString("Bold"))
      if (font().bold())
         return QString(("true"));
	 else return QString(("false"));
   if (*propertyName == QString("Italic"))
      if (font().italic())
         return QString(("true"));
	 else return QString(("false"));
   if (*propertyName == QString("Underline"))
      if (font().underline())
         return QString(("true"));
	 else return QString(("false"));
   if (*propertyName == QString("Strikeout"))
      if (font().strikeOut())
         return QString(("true"));
	 else return QString(("false"));
   
   return result;
}

//изменение значения свойства;
bool TCText::setPropValue(const QString &propertyName, const QString &newValue, const bool doIt)
{
   bool result = false;
   
   if (propertyName == "ItemName")
      {
       if (doIt) name = newValue;
       return true;
      }
   if (propertyName == "x")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          setX(newVal);
       return result;
      }
   if (propertyName == "y")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          setY(newVal);
       return result;
      }
   if (propertyName == "z")
      {
       int newVal = newValue.toInt(&result);
       if (result && doIt)
          setZItem(newVal);
       return result;
      }
   if (propertyName == "width")
      {
       return true;//false;
      }
   if (propertyName == "height")
      {
       return true;//false;
      }
   if (propertyName == "angle")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          ;//setY(newVal);
       return result;
      }
   if (propertyName == "Text")
      {
       if (doIt) setText(newValue);
       return true;
      }
   if (propertyName == "visible")
       if (newValue == "false")
          {
           if (doIt) setVisibleProp(false);
	   return true;
	  }
	  else if (newValue == "true")
          {
           if (doIt) setVisibleProp(true);
	   return true;
	  }
   if (propertyName == "TextColor")
      {
       int newVal = newValue.toULong(&result);
       if (result && doIt)
          {
	   setColor(QColor(QRgb(newVal)));
	  }
       return result;
      }
   if (propertyName == "family")
      {
       if (doIt) 
          {
	   QFont f = font();
	   f.setFamily(newValue);
	   setFont(f);
	  }
       return true;
      }
   if (propertyName == "PointSize")
      {
       QFont f = font();
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          {
	   f.setPointSize(newVal);
           setFont(f);
	  }
       return result;
      }
   if (propertyName == "Bold")
       if (newValue == "false")
          {
           if (doIt) 
	      {
	       QFont f = font(); 
	       f.setBold(false); 
	       setFont(f);
	      }
	   return true;
	  }
	  else if (newValue == "true")
          {
           if (doIt) 
	      {
	       QFont f = font(); 
	       f.setBold(true); 
	       setFont(f);
	      }
	   return true;
	  }
   if (propertyName == "Italic")
       if (newValue == "false")
          {
           if (doIt) 
	      {
	       QFont f = font(); 
	       f.setItalic(false); 
	       setFont(f);
	      }
	   return true;
	  }
	  else if (newValue == "true")
          {
           if (doIt) 
	      {
	       QFont f = font(); 
	       f.setItalic(true); 
	       setFont(f);
	      }
	   return true;
	  }
   if (propertyName == "Underline")
       if (newValue == "false")
          {
           if (doIt) 
	      {
	       QFont f = font(); 
	       f.setUnderline(false); 
	       setFont(f);
	      }
	   return true;
	  }
	  else if (newValue == "true")
          {
           if (doIt) 
	      {
	       QFont f = font(); 
	       f.setUnderline(true); 
	       setFont(f);
	      }
	   return true;
	  }
   if (propertyName == "Strikeout")
       if (newValue == "false")
          {
           if (doIt) 
	      {
	       QFont f = font(); 
	       f.setStrikeOut(false); 
	       setFont(f);
	      }
	   return true;
	  }
	  else if (newValue == "true")
          {
           if (doIt) 
	      {
	       QFont f = font(); 
	       f.setStrikeOut(true); 
	       setFont(f);
	      }
	   return true;
	  }
   
   return result;
}

//-------------------------------------TCImage------------------------------------
TCImage::TCImage(QCanvas *canvas, TVISIONMode m, double z, int x, int y, void *f)
   : TItem(m), QCanvasRectangle(x, y, 20, 20, canvas)
{
   setZ(z);
   QCanvasRectangle::setBrush(QBrush(QColor(0xff, 0xff, 0xff), Qt::NoBrush));
   QCanvasRectangle::setPen(QPen(QColor(0, 0, 0), 1, Qt::DashDotLine));
   QCanvasRectangle::setSize(20, 20);
   QCanvasRectangle::move(x, y);

   name = "Text";
}

TCImage::~TCImage()
{
   //QCanvasText::~QCanvasText();
}

int TCImage::RTTI = 10004;
int TCImage::rtti() const
{
   return RTTI;
}

void TCImage::setVisibleProp (bool yes)
{
   visibleItem = yes;
   if (mode == Runtime)
      QCanvasRectangle::setVisible(yes);
}

void TCImage::setSelectedItem (bool yes)
{
   QCanvasRectangle::setSelected(yes);
}

bool TCImage::isSelectedItem()
{
   return QCanvasRectangle::isSelected();
}

void TCImage::showItem()
{
   show();
}

double TCImage::zItem() const
{
   return z();
}

void TCImage::setZItem(double z)
{
   setZ(z);
}

void TCImage::moveByItem (double dx, double dy)
{
   moveBy(dx, dy);
}

void TCImage::draw(QPainter & p)
{
   if (QCanvasRectangle::isVisible())
      {
       if (image.isNull())
          QCanvasRectangle::draw(p); //прорисовка границ, если нет изображения
       p.drawImage(int(QCanvasRectangle::x()), int(QCanvasRectangle::y()), image, 0, 0, -1, -1, OrderedAlphaDither);
      }
   if (isSelectedItem())
      {
       QPen pen = p.pen();
       QBrush brush = p.brush();
       p.setPen(QPen(QColor(0x0, 0x0, 0x0), 1, Qt::DotLine));
       p.setBrush(QBrush(QColor(0x0, 0x0, 0x0), Qt::NoBrush));
       p.drawRect(QRect((int)QCanvasRectangle::x(), (int)QCanvasRectangle::y(),
                                     QCanvasRectangle::width(), QCanvasRectangle::height()));
       p.setPen(pen);
       p.setBrush(brush);
      }
}

bool TCImage::getCursorShape(int x, int y, char &resizeItem, int &cursor, int precision)
{
   if (mode == Development)
      {
           // УСЛОВИЯ: ВЫБРАН ОДИН ЭЛЕМЕНТ
	   //если перемещаюсь в пределах границы этого элемента, то изменить форму курсора на соответствующую:
	   resizeItem = resizeNothing;
	   //левый верхний угол?:
	   QRect r = QCanvasRectangle::boundingRect();
	   if ( (x >= r.x()) && (x <= r.right()) && (y >= r.y())  && (y <= r.bottom()) )
	      {
	       resizeItem = resizeNothing;
	       cursor = Qt::SizeAllCursor;
	       return false;
	      }
      }

   return false;
}

bool TCImage::resizeOnMouse(int x, int y, int prevX, int prevY, char resizeItem)
{
   //result = true, если изменения размера произошли. 
   // В противном случае result = false, а в TFrameView вызываем OnMouseRelease
   bool result = false;
   return result;
}

//Изменение размеров элемента мышью 
// (true - если изменения произошли, false - если нет (н-р, достигли минимального значения)):
bool TCImage::moveOnMouse(int x, int y, int prevX, int prevY)
{
   moveBy(x - prevX, y - prevY);
   return true;
}

TListOfString TCImage::getPropCategories()
{
   TListOfString result;
   result.push_back(("Item"));
   result.push_back(("Geometry"));
   result.push_back(("Image"));
   result.push_back(("Blinkings"));

   return result;
}

TListOfString TCImage::getPropNames(const QString *category)
{
   TListOfString result;
   if (category == NULL)
      {
       return result;
      }
   
   if (*category == QString("Item"))
      {
       result.push_back(("ItemName"));
      }
      
   if (!strcmp(category->ascii(), ("Geometry")))
      {
       result.push_back(("x"));
       result.push_back(("y"));
       result.push_back(("width"));
       result.push_back(("height"));
       result.push_back(("visible"));
       result.push_back(("angle"));
      }
   
   if (*category == QString("Image"))
      {
       result.push_back(("fileName"));
      }
   
   return result;
}

TPropType TCImage::getPropType(const QString *propertyName)
{
   if (*propertyName == QString("ItemName"))
      return stringType;
   if (*propertyName == QString("x"))
      return intType;
   if (*propertyName == QString("y"))
      return intType;
   if (*propertyName == QString("width"))
      return intType;
   if (*propertyName == QString("height"))
      return intType;
   if (*propertyName == QString("angle"))
      return intType;
   if (*propertyName == QString("visible"))
      return enumType;
   if (*propertyName == QString("fileName"))
      return stringType;

   return unknownType;
}

TListOfString TCImage::getPropEnumTypeValues(const QString *propertyName)
{
   TListOfString result;
   if (*propertyName == QString("visible"))
      {
       result.push_back(("true"));
       result.push_back(("false"));
      }
   return result;
}

QString TCImage::getPropValue(const QString &propertyName)
{
   QString result;
   if (*propertyName == QString("ItemName"))
      return name;
   if (*propertyName == QString("x"))
      return QString::number((int)x());
   if (*propertyName == QString("y"))
      return QString::number((int)y());
   if (*propertyName == QString("width"))
      return QString::number(QCanvasRectangle::width());
   if (*propertyName == QString("height"))
      return QString::number(QCanvasRectangle::height());
   if (*propertyName == QString("angle"))
      return QString::number(0);//QCanvasText::boundingRect().height());
   if (*propertyName == QString("fileName"))
      return fileName;
   if (*propertyName == QString("visible"))
      if (isVisibleProp())
         return QString(("true"));
	 else return QString(("false"));
   
   return result;
}

//изменение значения свойства;
bool TCImage::setPropValue(const QString &propertyName, const QString &newValue, const bool doIt)
{
   bool result = false;
   
   if (propertyName == "ItemName")
      {
       if (doIt) name = newValue;
       return true;
      }
   if (propertyName == "x")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          setX(newVal);
       return result;
      }
   if (propertyName == "y")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          setY(newVal);
       return result;
      }
   if (propertyName == "z")
      {
       int newVal = newValue.toInt(&result);
       if (result && doIt)
          setZItem(newVal);
       return result;
      }
   if (propertyName == "width")
      {
       return true;//false;
      }
   if (propertyName == "height")
      {
       return true;//false;
      }
   if (propertyName == "angle")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result && doIt)
          ;//setY(newVal);
       return result;
      }
   if (propertyName == "fileName")
      {
       if (doIt)
          {
	   if (!image.load(newValue))
	      return false;
	      else
	         {
		  fileName = newValue;
		  setSize(image.width(), image.height());
		 }
	  }
       return true;
      }
   if (propertyName == "visible")
       if (newValue == "false")
          {
           if (doIt) setVisibleProp(false);
	   return true;
	  }
	  else if (newValue == "true")
          {
           if (doIt) setVisibleProp(true);
	   return true;
	  }
   
   return result;
}

