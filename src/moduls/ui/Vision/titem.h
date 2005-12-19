// file "titem.h"

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

#ifndef TITEM
#define TITEM

#include <list>
#include <stack>

#include <qcanvas.h>
#include <qstring.h>
#include <qimage.h>
#include <qfile.h>

#include "xpm/tcrectangle.xpm"
#include "xpm/tcline.xpm"
#include "xpm/tctext.xpm"
#include "xpm/tcimage.xpm"

using std::list;
using std::stack;

namespace VISION
{
//Тип свойства элемента:
typedef enum {unknownType, intType, floatType, stringType, enumType, colorType} TPropType;
typedef enum {Development, Runtime} TVISIONMode;
typedef list<QString> TListOfString;

//направление изменения размеров элемента:
enum {resizeNothing, resizeTop, resizeTopRight, resizeRight, resizeBottomRight, 
      resizeBottom, resizeBottomLeft, resizeLeft, resizeTopLeft};

//Словарь, действия для описания динамизации:
typedef enum {ta_unknown, ta_eps, ta_value, ta_enum, ta_proportion, ta_name, ta_string, ta_real, ta_uint, 
              ta_format, ta_p, ta_n, ta_v, ta_o, ta_c, 
	      na_S, na_V, na_E, na_P, na_Format, na_EnumPair, na_P1, na_P2, na_P3, na_ParamOrValue, 
	      na_ParamOrValue2, na_Param_name_V, na_Param_name_E, na_Param_name_P3, na_Attr_name_V, 
	      na_Attr_name_E, na_Attr_name_P, na_Attr_name_P2, na_Attr_name_P3,
	      A_V_1, A_V_2, A_V_3, A_V_4, A_E_1, A_E_2, A_E_3, A_E_4,  A_P_1, A_P_2, A_P_3, A_P_4, A_P_5, 
	      A_P_6, A_P_7, A_P_8, A_P_9, A_P_10, A_P_11, A_P_12, A_V, A_E, A_P} TVocabulary;
	      
//Тип динаизации:
typedef enum{noDType, valueDType, enumDType, proportionDType} TDynamicType;

//Пустой символ:
#define epsSymbol "#"

//Стек распознающего автомата:
typedef stack<TVocabulary> TStack;

//Описание динамики типа "значение":
struct TDynamicTypeValue
{
   QString paramName;
   QString attrName;
   char format;
   int precision;
};

//Описание динамики типа "перечисление":
struct TDynamicTypeEnumElem
{
   QString paramName;
   QString attrName;
   QString value;
};

typedef list<TDynamicTypeEnumElem> TDynamicTypeEnumList;

struct TDynamicTypeEnum
{
   TDynamicTypeEnumList dynamicEnumElem;
   QString valueDefault;
};

//Описание динамики типа "пропорция":
struct TDynamicTypeProportion
{
   QString paramName0;
   QString attrName0;
   double val0;
   int size0;
   bool yesSize0;
   QString paramName100;
   QString attrName100;
   double val100;
   int size100;
   bool yesSize100;
   QString paramNameX;
   QString attrNameX;
};

class TDBGW;
class TItem;

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------Класс динамизации свойств ЭО--------------------------------------------
class TDynamicProp
{
public:
   TDynamicProp(TItem *);
   
   //проверяет правильность текстового описания динамики text свойства propName
   bool isValid(const QString &text, const QString &propName); 
   
   //имя динамизируемого свойства
   QString getPropName() const {return QString(propName);}; 
   
    //текстовое описание динамики
   QString getPropText() const {return QString(propText);};
   bool setPropText(const QString &text, const QString &name);
   
   bool doDynamic(); //изменить свойство propName элемента item
   TDynamicType getDynamicType();
private:
   TItem *item; //элемент TItem
   QString propName; //имя динамизируемого свойства элемента TItem
   QString propText; //текстовое описание динамики
   
   TDynamicType dynamicType; //способ динамизации
   
   bool getPropValue(QString &value) const; //получить значение динамизируемого свойства
   
   TDynamicTypeValue dynamicValue;
   TDynamicTypeEnum dynamicEnum;
   TDynamicTypeProportion dynamicProportion;
   void clearStructs();
   //Выделение лексем:
   bool isitName(const QString &s, const int begin, int &end);
   bool isitString(const QString &s, const int begin, int &end);
   bool isitInt(const QString &s, const int begin, int &end);
   bool isitDecimal(const QString &s, const int begin, int &end);
   bool isitReal(const QString &s, const int begin, int &end);
   bool isitUInt(const QString &s, const int begin, int &end);
   bool isitFormat(const QString &s, const int begin, int &end);
   bool isitValue(const QString &s, const int begin, int &end);
   bool isitEnum(const QString &s, const int begin, int &end);
   bool isitProportion(const QString &s, const int begin, int &end);
   bool isitPoint(const QString &s, const int begin, int &end);
   bool isitN(const QString &s, const int begin, int &end);//";"
   bool isitV(const QString &s, const int begin, int &end);//"::"
   bool isitOpenScope(const QString &s, const int begin, int &end);
   bool isitCloseScope(const QString &s, const int begin, int &end);
   bool isitEps(const QString &s, const int begin, int &end);
   //получить очередную лексему:
   TVocabulary getLexeme(const QString &s, int begin, int &end, QString &lexemeValue);
   //синтаксический разбор:
   bool parse(QString s);
   bool parseDoActions(TStack &st, const QString &lexemeValue); //выполнение действий, включенных в грамматику
   bool parseDoStack(TStack &st, const TVocabulary lexeme); //выполнение правил грамматики (заполнение стека)
};
//------------------------------------------------------------------------------------------------------------
typedef list<TDynamicProp> TListDynamicProp;

//------------------------------------------------------------------------------------------------------------
//----------------------------------Базовый класс ЭО----------------------------------------------------------
class TItem
{ 
public: 
  TItem(TVISIONMode m);
  virtual ~TItem();

  virtual int rtti() const = 0;
  
  //Видимость элемента:
  bool isVisibleProp() const;
  virtual void setVisibleProp (bool yes) = 0;
  
  //Работа со свойствами элемента:
  //список названий категорий свойств для элемента:
  virtual TListOfString getPropCategories() = 0;
  //список названий свойств в категории category:
  virtual TListOfString getPropNames(const QString *category) = 0;
  //тип свойства propertyName:
  virtual TPropType getPropType(const QString *propertyName) = 0; 
  //список возможных значений для свойства перечислимого типа:
  virtual TListOfString getPropEnumTypeValues(const QString *propertyName) = 0; 
  //значение свойства:
  virtual QString getPropValue(const QString &propertyName) = 0; 
  //изменение значения свойства:
  virtual bool setPropValue(const QString &propertyName, const QString &newValue, const bool doIt = true) = 0;
  
  //Работа с выбором элемента, перемещением:
  virtual bool isSelectedItem() = 0;
  virtual void setSelectedItem (bool yes) = 0;
  virtual double zItem() const = 0;
  virtual void setZItem(double z) = 0;
  virtual void showItem() = 0;
  virtual void moveByItem( double dx, double dy ) = 0;  
  
  //Обработка событий мыши (установка вида курсора в зависимости от его положения относительно элемента,
  //                              изменение размеров элемента с помощью мыши, перемещение элемента мышью):
  //Изменение курсора cursor (resizeItem) на соответствущий, если курсор находится в пределах 
  // границы элемента (x, y - положение курсора):
  virtual bool getCursorShape(int x, int y, char &resizeItem, int &cursor, int precision = 4) = 0;
  //Изменение размеров элемента мышью (true - если изменения произошли, false - если нет 
  // (н-р, достигли минимального значения)):
  virtual bool resizeOnMouse(int x, int y, int prevX, int prevY, char resizeItem) = 0;
  //Перетаскивание элемента мышью (true - если изменения произошли, false - если нет ):
  virtual bool moveOnMouse(int x, int y, int prevX, int prevY) = 0;
  
  virtual bool saveToFile(QFile &);
  
  //установка/изменение динамизации свойства propName:
  bool setDynamic(const QString &propName, const QString &text);
  //динамизация свойства propName:
  bool getDynamic(const QString &propName, QString &text);
  //динамизация (Runtime):
  bool doDynamic();
protected: 
   QString name; //имя элемента
   TVISIONMode mode; //режим работы
   bool visibleItem; //в режиме Development элемент все равно показываем, 
                     //а в режиме Runtime - в зависимости от значения этого свойства;
    
    TListDynamicProp listDynamicProp;//список динамизированных свойств
private:

};
//----------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------
//---------------------------------------Класс ЭО "Прямоугольник"-------------------------------------------
class TCRectangle : public TItem, public QCanvasRectangle
{ 
public: 
  TCRectangle(QCanvas *canvas, TVISIONMode m, double z, int x, int y, 
                                              int width = 40, int height = 30, void *f = NULL);
  virtual ~TCRectangle();
  
  static int RTTI;
  static QString itemName() {return "Rectangle";};
  static QImage itemImage() {return QImage(tcrectangle);};
  
  virtual QPointArray areaPoints () const;
  virtual int rtti() const;
     
  //Изменение видимости:
  virtual void setVisibleProp (bool yes);

  //Работа с выбором элемента, перемещением:
  virtual void setSelectedItem (bool yes);
  virtual void showItem();
  virtual bool isSelectedItem();
  virtual double zItem() const;
  virtual void setZItem(double z);
  virtual void moveByItem( double dx, double dy );
 
  //Работа со свойствами элемента:
  //список названий категорий свойств для элемента:
  virtual TListOfString getPropCategories();
  //список названий свойств в категории category для элемента:
  virtual TListOfString getPropNames(const QString *category); 
  //тип свойства propertyName:
  virtual TPropType getPropType(const QString *propertyName); 
  //список возможных значений для свойства перечислимого типа:
  virtual TListOfString getPropEnumTypeValues(const QString *propertyName); 
  //значение свойства:
  virtual QString getPropValue(const QString &propertyName); 
  //изменение значения свойства:
  virtual bool setPropValue(const QString &propertyName, const QString &newValue, const bool doIt = true); 
  
  //Обработка событий мыши (установка вида курсора в зависимости от его положения относительно элемента,
  //                        изменение размеров элемента с помощью мыши, перемещение элемента мышью):
  //Изменение курсора cursor (resizeItem) на соответствущий, если курсор находится в пределах границы элемента 
  // (x, y - положение курсора):
  virtual bool getCursorShape(int x, int y, char &resizeItem, int &cursor, int precision = 4);
  //Изменение размеров элемента мышью (true - если изменения произошли, false - если нет 
  // (н-р, достигли минимального значения)):
  virtual bool resizeOnMouse(int x, int y, int prevX, int prevY, char resizeItem);
  //Перетаскивание элемента мышью (true - если изменения произошли, false - если нет ):
  virtual bool moveOnMouse(int x, int y, int prevX, int prevY);
  
protected: 
   virtual void drawShape ( QPainter & p );
  
private:  

};
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
//------------------------------------Класс ЭО "Линия"--------------------------------------------------------
class TCLine : public TItem, public QCanvasLine
{ 
public: 
  TCLine(QCanvas *canvas, TVISIONMode m, double z, int x, int y, int width = 40, int height = 30, void *f = NULL);
  virtual ~TCLine();
  
  static int RTTI;
  static QString itemName() {return "Line";};
  static QImage itemImage() {return QImage(tcline);};
  
  virtual QPointArray areaPoints () const;
  virtual int rtti() const;
  
  //Изменение видимости:
  virtual void setVisibleProp (bool yes);
  
  virtual double zItem() const;
  virtual void setZItem(double z);
  
  void setSize(int width, int height);
  int width() const;
  int height() const;

  //Работа с выбором элемента, перемещением:
  virtual void setSelectedItem (bool yes);
  virtual void showItem();
  virtual bool isSelectedItem();
  virtual void moveByItem(double dx, double dy);
  virtual void moveBy(double dx, double dy);
 
  //Работа со свойствами элемента:
  //список названий категорий свойств для элемента:
  virtual TListOfString getPropCategories(); 
  //список названий свойств в категории category для элемента:
  virtual TListOfString getPropNames(const QString *category); 
  //тип свойства propertyName:
  virtual TPropType getPropType(const QString *propertyName); 
  //список возможных значений для свойства перечислимого типа:
  virtual TListOfString getPropEnumTypeValues(const QString *propertyName); 
  //значение свойства:
  virtual QString getPropValue(const QString &propertyName); 
  //изменение значения свойства:
  virtual bool setPropValue(const QString &propertyName, const QString &newValue, const bool doIt = true); 
  
  //Обработка событий мыши (установка вида курсора в зависимости от его положения относительно элемента,
  //                        изменение размеров элемента с помощью мыши, перемещение элемента мышью):
  // P.S. В QCanvasLine положения точек выражаются через сумму начала элемента и собственно координаты точки.
  //      Метод moveBy изменяет координаты начала элемента, в то время как все остальные - координаты точки (точек)
  //Изменение курсора cursor (resizeItem) на соответствущий, если курсор находится в пределах границы элемента 
  // (x, y - положение курсора):
  virtual bool getCursorShape(int x, int y, char &resizeItem, int &cursor, int precision = 4);
  //Изменение размеров элемента мышью (true - если изменения произошли, false - если нет 
  // (н-р, достигли минимального значения)):
  virtual bool resizeOnMouse(int x, int y, int prevX, int prevY, char resizeItem);
  //Перетаскивание элемента мышью (true - если изменения произошли, false - если нет ):
  virtual bool moveOnMouse(int x, int y, int prevX, int prevY);

protected: 
   virtual void drawShape ( QPainter & p ); //см. P.S.
   int x1, x2, y1, y2;
  
private:  

};
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
//------------------------------Класс ЭО "Текст"-------------------------------------------------------------
class TCText : public TItem, public QCanvasText
{ 
public: 
  TCText(QCanvas *canvas, TVISIONMode m, double z, int x, int y, const QString& text, void *f = NULL);
  virtual ~TCText();
  
  static int RTTI;
  static QString itemName() {return "Text";};
  static QImage itemImage() {return QImage(tctext);};

  virtual int rtti() const;
  
  //Изменение видимости:
  virtual void setVisibleProp (bool yes);

  //Работа с выбором элемента, перемещением:
  virtual void setSelectedItem (bool yes);
  virtual void showItem();
  virtual bool isSelectedItem();
  virtual double zItem() const;
  virtual void setZItem(double z);
  virtual void moveByItem( double dx, double dy );
 
  //Работа со свойствами элемента:
  //список названий категорий свойств для элемента:
  virtual TListOfString getPropCategories(); 
  //список названий свойств в категории category для элемента:
  virtual TListOfString getPropNames(const QString *category); 
  //тип свойства propertyName:
  virtual TPropType getPropType(const QString *propertyName); 
  //список возможных значений для свойства перечислимого типа:
  virtual TListOfString getPropEnumTypeValues(const QString *propertyName); 
  //значение свойства:
  virtual QString getPropValue(const QString &propertyName); 
  //изменение значения свойства:
  virtual bool setPropValue(const QString &propertyName, const QString &newValue, const bool doIt = true); 
  
  //Обработка событий мыши (установка вида курсора в зависимости от его положения относительно элемента,
  //                        изменение размеров элемента с помощью мыши, перемещение элемента мышью):
  //Изменение курсора cursor (resizeItem) на соответствущий, если курсор находится в пределах границы элемента 
  // (x, y - положение курсора):
  virtual bool getCursorShape(int x, int y, char &resizeItem, int &cursor, int precision = 4);
  //Изменение размеров элемента мышью (true - если изменения произошли, false - если нет 
  // (н-р, достигли минимального значения)):
  virtual bool resizeOnMouse(int x, int y, int prevX, int prevY, char resizeItem);
  //Перетаскивание элемента мышью (true - если изменения произошли, false - если нет ):
  virtual bool moveOnMouse(int x, int y, int prevX, int prevY);
  
protected: 
   virtual void draw( QPainter & p );
  
private:  

};
//-----------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------
//----------------------------Класс ЭО "Изображение"---------------------------------------------------------
class TCImage : public TItem, public QCanvasRectangle
{ 
public: 
  TCImage(QCanvas *canvas, TVISIONMode m, double z, int x, int y, void *f = NULL);
  virtual ~TCImage();
  
  static int RTTI;
  static QString itemName() {return "Image";};
  static QImage itemImage() {return QImage(tcimage);};

  virtual int rtti() const;
  
  //Изменение видимости:
  virtual void setVisibleProp (bool yes);

  //Работа с выбором элемента, перемещением:
  virtual void setSelectedItem (bool yes);
  virtual void showItem();
  virtual bool isSelectedItem();
  virtual double zItem() const;
  virtual void setZItem(double z);
  virtual void moveByItem( double dx, double dy );
 
  //Работа со свойствами элемента:
  //список названий категорий свойств для элемента:
  virtual TListOfString getPropCategories(); 
  //список названий свойств в категории category для элемента:
  virtual TListOfString getPropNames(const QString *category);
  //тип свойства propertyName: 
  virtual TPropType getPropType(const QString *propertyName); 
  //список возможных значений для свойства перечислимого типа:
  virtual TListOfString getPropEnumTypeValues(const QString *propertyName); 
  //значение свойства;
  virtual QString getPropValue(const QString &propertyName); 
  //изменение значения свойства:
  virtual bool setPropValue(const QString &propertyName, const QString &newValue, const bool doIt = true); 
  
  //Обработка событий мыши (установка вида курсора в зависимости от его положения относительно элемента,
  //                        изменение размеров элемента с помощью мыши, перемещение элемента мышью):
  //Изменение курсора cursor (resizeItem) на соответствущий, если курсор находится в пределах границы 
  // элемента (x, y - положение курсора):
  virtual bool getCursorShape(int x, int y, char &resizeItem, int &cursor, int precision = 4);
  //Изменение размеров элемента мышью (true - если изменения произошли, false - если нет 
  // (н-р, достигли минимального значения)):
  virtual bool resizeOnMouse(int x, int y, int prevX, int prevY, char resizeItem);
  //Перетаскивание элемента мышью (true - если изменения произошли, false - если нет ):
  virtual bool moveOnMouse(int x, int y, int prevX, int prevY);
  
protected: 
   virtual void draw( QPainter & p );
  
private:  
   QImage image;
   QString fileName;
};
//----------------------------------------------------------------------------------------------------------

}
#endif
