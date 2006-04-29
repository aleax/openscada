
//OpenSCADA system module UI.VISION file: tconfiguration.h
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

#include <qstringlist.h>
#include <qxml.h>

#ifndef TCONFIG
#define TCONFIG

namespace VISION
{

//Каталог подсистемы:
#define subdir "VISION/"
#define configFile subdir"vision.cfg"

//Класс "Конфигурация подсистемы"
class TConfiguration
{
   public:
      TConfiguration();
      
      bool addFrameToProject(const QString &frameName);
      bool renameFrameInProject(const QString &oldFrameName, const QString &newFrameName);
      bool deleteFrameFromProject(const QString &frameName);
      
      bool saveCfgToFile();//const QString &fileName);
      
      QStringList getProjectFrames() const;
      
      //начальный кадр:
      QString getStartFrame() const;
      bool setStartFrame(const QString &s);
      
   private:
      void createNewCfgFile(const QString &fileName);
      
      QStringList frames; //список кадров проекта
      QString startFrame; //начальный кадр
};

//Класс "Работа с файлом конфигурации"
class TCfgSaxHandler : public QXmlDefaultHandler 
{ 
public: 
  TCfgSaxHandler(TConfiguration *configuration); 
  
  bool startElement(const QString &namespaceURI, const QString &localName, 
                    const QString &qName, const QXmlAttributes &attribs); 
  bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName); 
  bool characters(const QString &str); 
  bool fatalError(const QXmlParseException &exception); 
  
private:
  TConfiguration *configuration;
  
  bool configurationWas;
  bool addingFrame;
};

}

#endif

