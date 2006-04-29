
//OpenSCADA system module UI.VISION file: tdbgw.h
/***************************************************************************
 *   Copyright (C) 2005-2006 by Evgen Zaichuk                               
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

#ifndef TDBGW_H
#define TDBGW_H

#include <list>
#include <vector>
#include <string>

#include "titem.h"

namespace VISION
{
//Атрибут (свойство) параметра логического уровня:
struct TAttrDev {std::string name; //имя атрибута
		 std::string descr; //описание атрибута
		 TPropType type; //тип атрибута
		 std::string value; //значение атрибута
		 };

//Параметр логического уровня:
struct TParamDev {std::string name; //имя параметра
   	          list <TAttrDev> properties; //свойства (атрибуты) параметра
                  };

typedef list <TParamDev> TListParamDev;//список параметров логичского уровня

//Класс "Доступ к подсистеме параметров OpenSCADA"
class TDBGW
{
public:
   TDBGW();
   ~TDBGW();
   //получить список параметров логического уровня и их атрибутов; фильтр по атрибутам - атрибуты типа TPropType:
   bool getParamsDev(TListParamDev &listParamDev, const TPropType); 
   
   //получить значение атрибута attrName параметра paramName:
   bool getValue(const QString &paramName, const QString &attrName, QString &value); 
};

extern TDBGW *dbgw;
}
#endif

