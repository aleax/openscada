// file "tdbgw.cpp"

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

#include <tsys.h>
#include <terror.h>

#include "tdbgw.h"
#include "tvision.h"

VISION::TDBGW *VISION::dbgw;

using namespace VISION;


TDBGW::TDBGW()
{
   dbgw = this;
}

TDBGW::~TDBGW()
{
   dbgw = NULL;
}

 //получить список параметров и их атрибутов; атрибуты - типа TPropType:
bool TDBGW::getParamsDev(TListParamDev &listParamDev, const TPropType propTypeFilter)
{
    listParamDev.clear();
    
    vector<string> listPar; //список параметров лог-го уровня
    
    AutoHD<TParamS> params = mod->owner().owner().param();//SYS->param();// //
    params.at().list(listPar); //получили список параметров
    //проход по списку параметров:
    for (int i = 0; i < listPar.size(); i++)
       {
         
	 TParamDev paramDev;
	 paramDev.name = listPar[i];
	 TFld fld;
	 //подключаемся к параметру listPar[i]:
	 AutoHD<TParam> param = params.at().at(listPar[i], "VISION");
	 //список атрибутов значений:
	 vector <string> attrNames; //список имен атрибутов для данного параметра
	 param.at().vlList(attrNames);
	 
	 //проход по атрибутaм значений:
	 for (int j = 0; j < attrNames.size(); j++)
	    {
	     AutoHD<TVal> val = param.at().vlAt(attrNames[j]);
	     TAttrDev attrDev;
	     fld = val.at().fld();
	     attrDev.name = fld.name();//attrNames[j];
	     attrDev.descr = fld.descr();
	     switch (fld.type())
	        {
		 case TFld::Dec : attrDev.type = intType;
		                         //attrDev.value = val.at().getS();
		                         break;
		 case TFld::Hex : attrDev.type = intType;
		                         break;
		 case TFld::Oct : attrDev.type = intType;
		                         break;
		 case TFld::Real : attrDev.type = floatType;
		                         //attrDev.value = val.at().getS();
		                         break;
		 case TFld::String : attrDev.type = stringType;
		                         //attrDev.value = val.at().getS();
		                         break;
		 case TFld::Bool : attrDev.type = enumType;
		                         break;
		 default : attrDev.type = unknownType;
		}
	     attrDev.value = val.at().getS();
	     paramDev.properties.push_back(attrDev);//пополнение списка атрибутов значений
	    }

	listParamDev.push_back(paramDev);//пополнение списка параметров
       }
}

 //получить значение атрибута attrName параметра paramName:
bool TDBGW::getValue(const QString &paramName, const QString &attrName, QString &value)
{
   bool result = true;
   string sParamName (paramName.operator std::string () );
   string sAttrName (attrName.operator std::string () );
   
   try
      {
       AutoHD<TParamS> params = mod->owner().owner().param();
       //подключаемся к параметру paramName:
       AutoHD<TParam> param = params.at().at(sParamName, "VISION");
       AutoHD<TVal> val = param.at().vlAt(sAttrName);
       value = val.at().getS();
       }
       catch (TError)
          {
	   result = false;
	  }
   return result;
}

