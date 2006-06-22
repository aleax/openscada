// file "tvisionrun.cpp"

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

#include <qaction.h> 
#include <qapplication.h> 
#include <qmenubar.h> 
#include <qmessagebox.h> 
#include <qpopupmenu.h> 
#include <qsettings.h> 
#include <qstatusbar.h>
#include <qsplitter.h>
#include <qtabbar.h>
#include <qtoolbox.h>
#include <qvbox.h>
#include <qimage.h>
#include <qcanvas.h>

#include "tvisionrun.h"
#include "tframeview.h"
#include "tvision.h"

using namespace VISION;

TVisionRun::TVisionRun(void *v, TConfiguration *cfg, QWidget *parent, const char *name, int wflags) 
    : QMainWindow(/*parent*/0, name, /*wflags*/WDestructiveClose),
    v(v)
{
    mod->regWin( this );    
 
  if (cfg != NULL )
     this->cfg = cfg;
     else cfg = new TConfiguration();
  
  createActions(); 
  createMenus(); 
  //createToolBars(); 
  createStatusBar(); 
  //readSettings(); 
  setCaption(tr("OpenSCADA VISION Runtime")); 
  //setIcon(QPixmap::fromMimeSource("icon.png")); 
  createWorkspace();
  
} 

TVisionRun::~TVisionRun( )
{
    mod->unregWin( this );
}

void TVisionRun::createActions() 
{ 
  exitAct = new QAction(/*QIconSet(QImage(saveFrame)),*/tr("&Exit"), tr("Ctrl+E"), this); 
  exitAct->setStatusTip(tr("Exit from VISION Runtime")); 
  connect(exitAct, SIGNAL(activated()), this, SLOT(exitVR()));
}
  
void TVisionRun::createMenus() 
{ 
  systemMenu = new QPopupMenu(this); 
  exitAct->addTo(systemMenu);
}

void TVisionRun::createStatusBar() 
{ 
}

void TVisionRun::createWorkspace()
{
  centralWidget = new QVBox(this);//QWidget(this);
  setCentralWidget(centralWidget);
  
  //начальный кадр:
  QString fileName = subdir + cfg->getStartFrame();
  
  QFile file(fileName);
  QXmlSimpleReader reader; 
  
   QCanvas *c = new QCanvas(1, 1);
   TFrameView *frmView = new TFrameView(c, VISION::Runtime, centralWidget);
   TSaxHandler handler(frmView); 
   reader.setContentHandler(&handler); 
   reader.setErrorHandler(&handler); 
   bool result = reader.parse(&file); 
   if (!result)
      {
       QMessageBox::critical( NULL, "Opening error", 
            "Error occured when opening frame '" + fileName + "'!");
       delete frmView; // !!!
      }
      else
         {
	  frmView->startDynamic();
	 }
	 
}

void TVisionRun::exitVR()
{
   
   close(); //~TVisionRun
}

void TVisionRun::closeEvent(QCloseEvent* ce)
{
   //emit runtimeIsClosing();
   ((TVision *)v)->closeRuntime();
   ce->accept();
}
