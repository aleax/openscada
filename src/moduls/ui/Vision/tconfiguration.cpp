// file "tconfiguration.cpp"

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

#include <qmessagebox.h>

#include "tconfiguration.h"

using namespace VISION;

//--------------------------------------TConfiguration----------------------------------------------------
TConfiguration::TConfiguration()
{
   QFile file(configFile);
   if (!file.exists())
      createNewCfgFile(configFile);
      
   QXmlSimpleReader reader; 
  
   TCfgSaxHandler handler(this); 
   reader.setContentHandler(&handler); 
   reader.setErrorHandler(&handler); 
   bool result = reader.parse(&file); 
   if (!result)
      {
       QMessageBox::critical( NULL, "Opening error", 
	           "Error occured when opening VISION configuration file'" + QString(configFile) + "'!");
      }
}

void TConfiguration::createNewCfgFile(const QString &fileName)
{
   QFile file(fileName);
   if (file.open(IO_WriteOnly))
      {
       QTextStream stream(&file);
       stream << "<configuration>\n";
       stream << " <frames>\n" << " </frames>\n";
       
       stream << "</configuration>\n";
       file.close();
      }
}

bool TConfiguration::saveCfgToFile()
{
   QFile file(configFile);
   if (file.open(IO_WriteOnly))
      {
       QTextStream stream(&file);
       stream << "<configuration>\n";
       stream << " <frames>\n";
       for (QStringList::iterator i = frames.begin(); i != frames.end(); i++)
	   stream << "  <frame name = \"" << (*i) << "\">\n" << "  </frame>\n";
       stream << " </frames>\n";
       
       stream << " <startFrame name = \"" << startFrame << "\">\n" << " </startFrame>\n";
       stream << "</configuration>\n";
       file.close();
      }
      else return false;
   
   return true;
}

bool TConfiguration::addFrameToProject(const QString &frameName)
{
   bool result = true;
   for (QStringList::iterator i = frames.begin(); i != frames.end(); i++)
      if (frameName.lower() == (*i).lower())
         {
	  result = false;
	  break;
	 }
   if (result)
      frames.push_back(frameName);
   
   return result;
}

bool TConfiguration::renameFrameInProject(const QString &oldFrameName, const QString &newFrameName)
{
   //если кадр с именем newFrameName уже есть, то ошибка - возвращаем 0
   bool result = true;
   bool foundOld = false;
   QStringList::iterator renamed;
   for (QStringList::iterator i = frames.begin(); i != frames.end(); i++)
      {
      if ( (*i).lower() == oldFrameName.lower())
         foundOld = true;
         else if ( (*i).lower() == newFrameName.lower())
	    result = false;
      }
   if (result || !foundOld)
      for (QStringList::iterator i = frames.begin(); i != frames.end(); i++)
         if ((*i) == oldFrameName)
	    (*i) = newFrameName;
   return result;
}

bool TConfiguration::deleteFrameFromProject(const QString &frameName)
{
   bool result = false;
   for (QStringList::iterator i = frames.begin(); i != frames.end(); i++)
      if (frameName == (*i))
         {
          frames.erase(i);
	  result = true;
	  break;
	 }
   return result;
}
      
QStringList TConfiguration::getProjectFrames() const
{
   return QStringList(frames);
}

QString TConfiguration::getStartFrame() const
{
   return QString(startFrame);
}

bool TConfiguration::setStartFrame(const QString &s)
{
   startFrame = s;
   return true;
}

//--------------------------------------TCfgSaxHandler----------------------------------------------------
TCfgSaxHandler::TCfgSaxHandler(TConfiguration *configuration)
{
   this->configuration = configuration;
   configurationWas = false;
   addingFrame = false;
}

bool TCfgSaxHandler::startElement(const QString &, const QString &, const QString &qName, const QXmlAttributes &attribs) 
{ 
  bool result = false;
  if (qName == "configuration")
     {
      configurationWas = true;
      result = true;
     }
  if (qName == "frames")
     {
      addingFrame = true;
      result = true;
     }
  if ((qName == "frame") && addingFrame)
    {
     result = false;
     //QMessageBox::information( NULL, "sax", "adding tcrectangle"); result = true;currentItem = (TItem*)1;
     if (!configuration->addFrameToProject(attribs.value("name")))
        {
	 QMessageBox::critical( NULL, "Opening configuration error", 
	                              "Invalid frame name '" + attribs.value("name") + "'!");
	 return false;
	}
	else result = true;
    }
  if (qName == "startFrame")
    {
     result = false;
     //QMessageBox::information( NULL, "sax", "adding tcrectangle"); result = true;currentItem = (TItem*)1;
     if (!configuration->setStartFrame(attribs.value("name")))
        {
	 QMessageBox::critical( NULL, "Opening configuration error", 
	                              "Invalid frame name '" + attribs.value("name") + "'!");
	 return false;
	}
	else result = true;
    }
          
  return result; 
}

bool TCfgSaxHandler::endElement(const QString &, const QString &, const QString &qName) 
{
  if (qName == "frames")
     addingFrame = false;
  if (qName == "configuration")
     configurationWas = false;
  
  return true; 
}

bool TCfgSaxHandler::characters(const QString &str) 
{ 
  bool result;
  return true;
}

bool TCfgSaxHandler::fatalError(const QXmlParseException &exception) 
{
   QMessageBox::critical( NULL, "Opening error", 
	           "Error '" + QString(exception.message()) + "' in line #" + 
		   QString::number(exception.lineNumber()) + ", coloumn #" + 
		   QString::number(exception.columnNumber()) + "'!");
   
   return false; 
}