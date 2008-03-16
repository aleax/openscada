
//OpenSCADA system module UI.Vision file: vis_devel_dlgs.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
 *   rom_as@diyaorg.dp.ua                                                  *
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

#include <QTreeWidget>
#include <QItemEditorFactory>
#include <QPushButton>
#include <QAction>
#include <QGroupBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QBuffer>
#include <QTableWidget>
#include <QPainter>
#include <QSplitter>

#include <tsys.h>

#include "vis_devel.h"
#include "tvision.h"
#include "vis_widgs.h"
#include "vis_devel_dlgs.h"

using namespace VISION;


//****************************************
//* Library properties dialog            *
//****************************************
LibProjProp::LibProjProp( VisDevelop *parent ) : 
    QDialog((QWidget*)parent), show_init(false), is_modif(false), ico_modif(false)
{
    QLabel *lab;
    QGroupBox *grp;
    QGridLayout *dlg_lay, *glay;
    QImage ico_t;
    setWindowTitle(_("Widget's library properties"));
    setWindowIcon(owner()->actVisItProp->icon());

    //- Create tabulator -
    QVBoxLayout *tab_lay = new QVBoxLayout(this);
    tab_lay->setMargin(5);
    wdg_tabs = new QTabWidget(this);
    tab_lay->addWidget(wdg_tabs);
    connect(wdg_tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    
    //- Add tab 'Widget' -
    //--------------------
    wdg_tabs->addTab(new QWidget,_("Widgets library"));    
    QWidget *tab_w = wdg_tabs->widget(0);     
    
    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);
    
    //- State parameters -
    grp = new QGroupBox(_("State"),tab_w);
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);

    obj_ico = new QPushButton(tab_w);
    obj_ico->setObjectName("/obj/cfg/ico");
    obj_ico->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );
    obj_ico->setIconSize(QSize(60,60));
    obj_ico->setAutoDefault(false);
    connect(obj_ico, SIGNAL(released()), this, SLOT(selectIco()));
    glay->addWidget(obj_ico,0,0,3,1);

    lab = new QLabel(_("Enabled:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,0,1);
    obj_enable = new QCheckBox(tab_w);
    obj_enable->setObjectName("/obj/st/en");
    connect(obj_enable, SIGNAL(stateChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_enable,0,2,1,2);

    lab = new QLabel(_("Container DB:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,1,1);
    obj_db = new LineEdit(tab_w);
    obj_db->setObjectName("/obj/st/db");
    connect(obj_db, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(obj_db,1,2,1,2);

    lab = new QLabel(_("User and group:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,2,1);    
    obj_user = new QComboBox(tab_w);
    obj_user->setObjectName("/obj/st/user");
    connect(obj_user, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_user,2,2);
    obj_grp = new QComboBox(tab_w);
    obj_grp->setObjectName("/obj/st/grp");
    connect(obj_grp, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_grp,2,3);
    grp->setLayout(glay);    
    dlg_lay->addWidget(grp,0,0);
    
    //- Access parameters -
    grp = new QGroupBox(_("Access"),tab_w);
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);    
    lab = new QLabel(_("User:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,0,0);
    obj_accuser = new QComboBox(tab_w);
    obj_accuser->setObjectName("/obj/cfg/u_a");
    connect(obj_accuser, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accuser,0,1);    
    lab = new QLabel(_("Group:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,1,0);    
    obj_accgrp  = new QComboBox(tab_w);
    obj_accgrp->setObjectName("/obj/cfg/g_a");
    connect(obj_accgrp, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accgrp,1,1);    
    lab = new QLabel(_("Other:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,2,0);
    obj_accother= new QComboBox(tab_w);
    obj_accother->setObjectName("/obj/cfg/o_a");    
    connect(obj_accother, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accother,2,1);
    grp->setLayout(glay);    
    dlg_lay->addWidget(grp,0,1);    
    
    //- Config parameters -
    grp = new QGroupBox(_("Configuration"),tab_w);    
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);

    glay->addWidget(new QLabel(_("Id:"),tab_w),0,0);
    obj_id = new QLabel(this);
    obj_id->setTextInteractionFlags(Qt::TextSelectableByMouse);
    obj_id->setObjectName("/obj/cfg/id");
    obj_id->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(obj_id,0,1);

    glay->addWidget(new QLabel(_("Name:"),tab_w),1,0);
    obj_name = new LineEdit(this);
    obj_name->setObjectName("/obj/cfg/name");
    connect(obj_name, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(obj_name,1,1);

    glay->addWidget(new QLabel(_("Description:"),tab_w),2,0);
    obj_descr = new TextEdit(this);
    obj_descr->setObjectName("/obj/cfg/descr");
    connect(obj_descr, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(obj_descr,3,0,1,2);
  
    grp->setLayout(glay);
    dlg_lay->addWidget(grp,1,0,1,2);     


    //- Add tab 'Mime data' -
    //------------------------
    wdg_tabs->addTab(new QWidget,_("Mime data"));
    tab_w = wdg_tabs->widget(1);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);
    
    mimeDataTable = new QTableWidget(0,3,tab_w);     
    mimeDataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(mimeDataTable, SIGNAL(cellChanged(int,int)), this, SLOT(mimeDataChange(int,int)));
    QStringList headLabels;
    headLabels << _("Id") << _("Mime type") << _("Data size");
    mimeDataTable->setHorizontalHeaderLabels(headLabels);    
    dlg_lay->addWidget(mimeDataTable,0,0,1,4);
    
    buttDataAdd = new QPushButton(_("Add record"),tab_w);
    connect(buttDataAdd, SIGNAL(clicked()), this, SLOT(addMimeData()));
    dlg_lay->addWidget(buttDataAdd,1,0);
    buttDataDel = new QPushButton(_("Delete record"),tab_w);
    connect(buttDataDel, SIGNAL(clicked()), this, SLOT(delMimeData()));
    dlg_lay->addWidget(buttDataDel,1,1);    
    buttDataLoad = new QPushButton(_("Load data"),tab_w);
    connect(buttDataLoad, SIGNAL(clicked()), this, SLOT(loadMimeData()));    
    dlg_lay->addWidget(buttDataLoad,1,2);
    buttDataUnload = new QPushButton(_("Unload data"),tab_w);
    connect(buttDataUnload, SIGNAL(clicked()), this, SLOT(unloadMimeData()));
    dlg_lay->addWidget(buttDataUnload,1,3);

    //- Add button box -
    //------------------ 
    butbox = new QDialogButtonBox(QDialogButtonBox::Apply|
				  QDialogButtonBox::Cancel|
				  QDialogButtonBox::Close, Qt::Horizontal,this);
    //-- Init Apply button --
    butbox->button(QDialogButtonBox::Apply)->setText(_("Save to DB"));
    if(!ico_t.load(TUIS::icoPath("vision_db_save").c_str())) ico_t.load(":/images/db_save.png");
    butbox->button(QDialogButtonBox::Apply)->setIcon(QPixmap::fromImage(ico_t));
    connect(butbox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(pressApply()));    
    //-- Init Cancel button --
    butbox->button(QDialogButtonBox::Cancel)->setText(_("Load from DB"));
    if(!ico_t.load(TUIS::icoPath("vision_db_load").c_str())) ico_t.load(":/images/db_load.png");
    butbox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    //butbox->button(QDialogButtonBox::Cancel)->setShortcut(QKeySequence("Esc"));
    connect(butbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(pressCancel()));
    //-- Init close button --
    butbox->button(QDialogButtonBox::Close)->setText(_("Close"));    
    connect(butbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(pressClose()));

    tab_lay->addWidget(butbox);

    //- End resize -
    //-------------- 
    resize(500,400);    
}

LibProjProp::~LibProjProp( )
{

}

VisDevelop *LibProjProp::owner()
{
    return (VISION::VisDevelop*)parentWidget();
}

void LibProjProp::showDlg( const string &iit, bool reload )
{
    vector<string> ls;
    string sval;
    QImage ico_t;
    ed_it = iit;    

    show_init = true;

    //- Update elements present, visible and values -
    //-----------------------------------------------
    XMLNode req("get");
    
    XMLNode info_req("info");
    info_req.setAttr("path",ed_it);
    if( owner()->cntrIfCmd(info_req) )
    {
	mod->postMess( mod->nodePath().c_str(),
        	QString(_("Get node '%1' information error.")).arg(ed_it.c_str()),TVision::Error, this );
	return;
    }
    XMLNode *root, *gnd;    
    root = info_req.childGet(0);

    setWindowTitle( root->attr("dscr").c_str() );

    //- Generic dialog's page -    
    gnd=TCntrNode::ctrId(root,"/obj",true);
    wdg_tabs->setTabEnabled(0,gnd);
    if( gnd )
    {
    	wdg_tabs->setTabText(0,gnd->attr("dscr").c_str());
 	//-- Enable stat --
	gnd=TCntrNode::ctrId(root,obj_enable->objectName().toAscii().data(),true);
	obj_enable->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );        
	if( gnd )
	{
	    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_enable->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) obj_enable->setChecked(atoi(req.text().c_str()));
	}
	//-- DB value --
       	gnd=TCntrNode::ctrId(root,obj_db->objectName().toAscii().data(),true);
	obj_db->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );        
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_db->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) obj_db->setValue(req.text().c_str());
	}
 	//-- User --
	gnd=TCntrNode::ctrId(root,obj_user->objectName().toAscii().data(),true);
	obj_user->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );        
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_user->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) )	sval = req.text();

	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/obj/u_lst",TSYS::PathEl));
	    obj_user->clear();
       	    if( !owner()->cntrIfCmd(req) )
		for(int i_l = 0; i_l < req.childSize(); i_l++)
		{
		    obj_user->addItem(req.childGet(i_l)->text().c_str());
		    if( sval == req.childGet(i_l)->text() )	obj_user->setCurrentIndex(i_l);
		}
	}
 	//-- Group --
	gnd=TCntrNode::ctrId(root,obj_grp->objectName().toAscii().data(),true);
	obj_grp->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );            
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_grp->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) sval = req.text();
	
   	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/obj/g_lst",TSYS::PathEl));
    	    obj_grp->clear();
	    if( !owner()->cntrIfCmd(req) )
		for(int i_l = 0; i_l < req.childSize(); i_l++)
		{
		    obj_grp->addItem(req.childGet(i_l)->text().c_str());
		    if( sval == req.childGet(i_l)->text() )	obj_grp->setCurrentIndex(i_l);
		}
	}                                          
 	//-- Icon --
	gnd=TCntrNode::ctrId(root,obj_ico->objectName().toAscii().data(),true);    
	ico_modif = gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR;        
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_ico->objectName().toAscii().data(),TSYS::PathEl));
    	    if( !owner()->cntrIfCmd(req) && (sval = TSYS::strDecode(req.text(),TSYS::base64)).size() && 
		    ico_t.loadFromData((const uchar*)sval.c_str(),sval.size()) )
		obj_ico->setIcon(QPixmap::fromImage(ico_t));
	    else obj_ico->setIcon(QIcon());
	} 
 	//-- Permition --
	int luser_acc, lgrp_acc, loth_acc;        
	gnd=TCntrNode::ctrId(root,obj_accuser->objectName().toAscii().data(),true);
	obj_accuser->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );        
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accuser->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) luser_acc = atoi(req.text().c_str());
	}    
     	gnd=TCntrNode::ctrId(root,obj_accgrp->objectName().toAscii().data(),true);    
	obj_accgrp->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );        
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accgrp->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) lgrp_acc = atoi(req.text().c_str());
	}	
	gnd=TCntrNode::ctrId(root,obj_accother->objectName().toAscii().data(),true);	    
	obj_accother->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );        
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accother->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) loth_acc = atoi(req.text().c_str());
	}	
	req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/obj/a_lst",TSYS::PathEl));
	obj_accuser->clear( );
	obj_accgrp->clear( );
	obj_accother->clear( );
	if( !owner()->cntrIfCmd(req) )
	    for(int i_l = 0; i_l < req.childSize(); i_l++)
	    {
		int vl = atoi(req.childGet(i_l)->attr("id").c_str());
		obj_accuser->addItem(req.childGet(i_l)->text().c_str(),vl);	    
		if( luser_acc == vl )	obj_accuser->setCurrentIndex(i_l);
		obj_accgrp->addItem(req.childGet(i_l)->text().c_str(),vl);
		if( lgrp_acc == vl )	obj_accgrp->setCurrentIndex(i_l);
		obj_accother->addItem(req.childGet(i_l)->text().c_str(),vl);
		if( loth_acc == vl )	obj_accother->setCurrentIndex(i_l);
	    }
 	//-- Id --
	gnd=TCntrNode::ctrId(root,obj_id->objectName().toAscii().data(),true);    
	if( gnd )
	{   
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_id->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) obj_id->setText(req.text().c_str());
	} 
 	//-- Name --
	gnd=TCntrNode::ctrId(root,obj_name->objectName().toAscii().data(),true);    
	obj_name->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );        
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_name->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) obj_name->setValue(req.text().c_str()); 
	} 
 	//-- Description --
	gnd=TCntrNode::ctrId(root,obj_descr->objectName().toAscii().data(),true);	
	obj_descr->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );         
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_descr->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) obj_descr->setText(req.text().c_str()); 
	} 
    }
    //- Mime data page -
    gnd=TCntrNode::ctrId(root,"/mime",true);
    wdg_tabs->setTabEnabled(1,gnd);
    if( gnd )	wdg_tabs->setTabText(1,gnd->attr("dscr").c_str());

    is_modif = false;

    //- Show dialog -
    show();
    raise();
    activateWindow();

    if( reload ) tabChanged(wdg_tabs->currentIndex());
    else wdg_tabs->setCurrentIndex(0);

    show_init = false;
}

void LibProjProp::tabChanged( int itb )
{
    if( itb == 1 )
    {
	show_init = true;
    	//- Load mime data -
	XMLNode req("get");
    	req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl));
    	if( !owner()->cntrIfCmd(req) )
	{
	    QTableWidgetItem *w_it;            
            XMLNode *id_col = req.childGet(0);
	    mimeDataTable->setRowCount(id_col->childSize());
    	    for( int i_l = 0; i_l < id_col->childSize(); i_l++ )
	    {
		if( !mimeDataTable->item(i_l,0) ) 	
		{
		    mimeDataTable->setItem(i_l,0,new QTableWidgetItem());
		    mimeDataTable->item(i_l,0)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
		}
		mimeDataTable->item(i_l,0)->setText(id_col->childGet(i_l)->text().c_str());
		mimeDataTable->item(i_l,0)->setData(Qt::UserRole,id_col->childGet(i_l)->text().c_str());
		if( !mimeDataTable->item(i_l,1) )
		{
		    mimeDataTable->setItem(i_l,1,new QTableWidgetItem());
		    mimeDataTable->item(i_l,1)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
		}
		mimeDataTable->item(i_l,1)->setText(req.childGet(1)->childGet(i_l)->text().c_str());	
		if( !mimeDataTable->item(i_l,2) ) 
		{
		    mimeDataTable->setItem(i_l,2,new QTableWidgetItem());
		    mimeDataTable->item(i_l,2)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
		}
		mimeDataTable->item(i_l,2)->setText(req.childGet(2)->childGet(i_l)->text().c_str());
	    }
	    mimeDataTable->resizeColumnsToContents();	    
	}
	show_init = false;
    }
}

void LibProjProp::selectIco( )
{
    QImage ico_t;
    
    if( !ico_modif )	return;
    
    QString fileName = QFileDialog::getOpenFileName(this,_("Load icon picture"),"",_("Images (*.png *.jpg)"));
    
    if( fileName.isEmpty() )	return;
    if(!ico_t.load(fileName))
    {
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Loaded icon image '%1' error.")).arg(fileName),TVision::Warning, this );
	return;    
    }
    
    obj_ico->setIcon(QPixmap::fromImage(ico_t));
    
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    ico_t.save(&buffer,"PNG");
    
    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_ico->objectName().toAscii().data(),TSYS::PathEl))->
	setText(TSYS::strEncode(string(ba.data(),ba.size()),TSYS::base64));
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    is_modif = true;    
}

void LibProjProp::isModify( )
{
    bool update = false;
    if( show_init )	return;
    
    QString oname = sender( )->objectName();

    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(oname.toAscii().data(),TSYS::PathEl));
    
    if( oname == obj_enable->objectName() )
    {
	req.setText(TSYS::int2str(obj_enable->isChecked()));
	update = true;
    }
    else if( oname == obj_db->objectName() || oname == obj_name->objectName() )
	req.setText(((LineEdit*)sender())->value().toAscii().data());        
    else if( oname == obj_user->objectName() || oname == obj_grp->objectName() )
    {
	req.setText(((QComboBox*)sender())->currentText().toAscii().data());
	update = true;
    }
    else if( oname == obj_accuser->objectName() || oname == obj_accgrp->objectName() || oname == obj_accother->objectName() )
    {
	req.setText(((QComboBox*)sender())->itemData(((QComboBox*)sender())->currentIndex()).toString().toAscii().data());
	update = true;
    }
    else if( oname == obj_descr->objectName() )
	req.setText(obj_descr->text().toAscii().data());
    else return;
    
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
    else if( update )	showDlg(ed_it,true);
    
    is_modif = true;
}

void LibProjProp::pressApply( )
{    
    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/obj/cfg/save",TSYS::PathEl));
        
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    is_modif = false; 
    
    emit apply(ed_it);
}

void LibProjProp::pressCancel( )
{    
    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/obj/cfg/load",TSYS::PathEl));
        
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
    else showDlg(ed_it,true);
}

void LibProjProp::pressClose( )
{
    if( is_modif )
    {
	InputDlg dlg(this,obj_ico->icon(),
             _("Some attributes is changed. You sure to close window?"),_("Close window"),false,false);
	if( dlg.exec() != QDialog::Accepted )	return;
    }			    

    is_modif = false;
    ed_it = "";
    hide();
}

void LibProjProp::addMimeData( )
{
    XMLNode req("add");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl));    
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    tabChanged(1);
}

void LibProjProp::delMimeData( )
{
    int row = mimeDataTable->currentRow( );
    if( row < 0 )
    {
	mod->postMess( mod->nodePath().c_str(),_("No one row is selected."),TVision::Warning,this );
	return;
    }

    XMLNode req("del");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl))->
	setAttr("key_id",mimeDataTable->item(row,0)->text().toAscii().data());
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    tabChanged(1);	
}

void LibProjProp::loadMimeData( )
{
    int row = mimeDataTable->currentRow( );
    if( row < 0 )
    {
	mod->postMess( mod->nodePath().c_str(),_("No one row is selected."),TVision::Warning,this );
	return;
    }

    QString fileName = QFileDialog::getOpenFileName(this,_("Load data"),"",_("All files (*.*)"));
    if( fileName.isEmpty() )	return;
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly))
	mod->postMess( mod->nodePath().c_str(),
		QString(_("Open file '%1' is fail: %2")).arg(fileName).arg(file.errorString()),TVision::Error,this);
    QByteArray data = file.readAll();		

    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl))->
	setAttr("col","dt")->
	setAttr("key_id",mimeDataTable->item(row,0)->text().toAscii().data())->
	setText(TSYS::strEncode(string(data.data(),data.size()),TSYS::base64));
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    tabChanged(1);    
}

void LibProjProp::unloadMimeData( )
{
    int row = mimeDataTable->currentRow( );
    if( row < 0 )
    {
	mod->postMess( mod->nodePath().c_str(),_("No one row is selected."),TVision::Warning,this );
	return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,_("Save data"),
			    mimeDataTable->item(row,0)->text(),_("All files (*.*)"));
    if( fileName.isEmpty() )	return;
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
	mod->postMess( mod->nodePath().c_str(),
		QString(_("Open file '%1' is fail: %2")).arg(fileName).arg(file.errorString()),TVision::Error,this);

    XMLNode req("get");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl))->
	setAttr("data","1")->
	setAttr("col","dt")->
	setAttr("key_id",mimeDataTable->item(row,0)->text().toAscii().data());
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);    
    else
    {
	string mimeData = TSYS::strDecode(req.text(),TSYS::base64);	
	if( file.write(mimeData.data(),mimeData.size()) < 0 )
	    mod->postMess( mod->nodePath().c_str(),
		QString(_("Write data to file '%1' is fail: %2")).arg(fileName).arg(file.errorString()),TVision::Error,this);
    }
}

void LibProjProp::mimeDataChange( int row, int column )
{
    if( show_init ) return;
    
    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl))->
	setAttr("col",(column==0)?"id":(column==1)?"tp":"")->
	setAttr("key_id",mimeDataTable->item(row,0)->data(Qt::UserRole).toString().toAscii().data())->
	setText(mimeDataTable->item(row,column)->text().toAscii().data());
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    tabChanged(1);
}


//****************************************
//* Widget properties dialog             *
//****************************************
VisItProp::VisItProp( VisDevelop *parent ) : 
    QDialog((QWidget*)parent), is_modif(false), ico_modif(false), show_init(false)
{
    QLabel *lab;
    QGroupBox *grp;
    QGridLayout *dlg_lay, *glay;
    QImage ico_t;
    setWindowTitle(_("Widget properties"));
    setWindowIcon(owner()->actVisItProp->icon());

    //- Create tabulator -
    QVBoxLayout *tab_lay = new QVBoxLayout(this);
    tab_lay->setMargin(5);
    wdg_tabs = new QTabWidget(this);
    tab_lay->addWidget(wdg_tabs);
    connect(wdg_tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    
    //- Add tab 'Widget' -
    //--------------------
    wdg_tabs->addTab(new QWidget,_("Widget"));    
    QWidget *tab_w = wdg_tabs->widget(0);
    
    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);
    
    //-- State parameters --
    grp = new QGroupBox(_("State"),tab_w);
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);    
    obj_ico = new QPushButton(tab_w);    
    obj_ico->setObjectName("/wdg/cfg/ico");
    obj_ico->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );
    obj_ico->setIconSize(QSize(60,60));
    obj_ico->setAutoDefault(false);
    connect(obj_ico, SIGNAL(released()), this, SLOT(selectIco()));
    glay->addWidget(obj_ico,0,0,3,1);
    
    lab = new QLabel(_("Enabled:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,0,1);
    obj_enable = new QCheckBox(tab_w);
    obj_enable->setObjectName("/wdg/st/en");    
    connect(obj_enable, SIGNAL(stateChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_enable,0,2,1,2);    
    
    lab = new QLabel(_("Parent widget:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,1,1);    
    obj_parent = new QComboBox(tab_w);
    obj_parent->setObjectName("/wdg/st/parent");
    connect(obj_parent, SIGNAL(activated(int)), this, SLOT(isModify()));
    glay->addWidget(obj_parent,1,2,1,2);
    
    lab = new QLabel(_("User and group:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,2,1);
    obj_user = new QComboBox(tab_w);
    obj_user->setObjectName("/wdg/st/user");
    connect(obj_user, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_user,2,2);    
    obj_grp = new QComboBox(tab_w);
    obj_grp->setObjectName("/wdg/st/grp");
    connect(obj_grp, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_grp,2,3);
    
    //--- Specific parameter: page type ---
    lab = new QLabel(_("Page type:"),tab_w);
    glay->addWidget(lab,3,1);
    pg_tp = new QComboBox(tab_w);
    pg_tp->setObjectName("/wdg/st/pgTp");
    pg_tp->setWindowIconText(TSYS::addr2str(lab).c_str());
    connect(pg_tp, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(pg_tp,3,2,1,2);
    
    grp->setLayout(glay);
    dlg_lay->addWidget(grp,0,0);
    
    //-- Access parameters --
    grp = new QGroupBox(_("Access"),tab_w);
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);        
    lab = new QLabel(_("User:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,0,0);
    obj_accuser = new QComboBox(tab_w);
    obj_accuser->setObjectName("/wdg/cfg/u_a");
    connect(obj_accuser, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accuser,0,1);    
    lab = new QLabel(_("Group:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,1,0);
    obj_accgrp  = new QComboBox(tab_w);
    obj_accgrp->setObjectName("/wdg/cfg/g_a");    
    connect(obj_accgrp, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accgrp,1,1);    
    lab = new QLabel(_("Other:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,2,0);
    obj_accother= new QComboBox(tab_w);    
    obj_accother->setObjectName("/wdg/cfg/o_a");
    connect(obj_accother, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accother,2,1);
    grp->setLayout(glay);
    dlg_lay->addWidget(grp,0,1);    
    
    //-- Config parameters --
    grp = new QGroupBox(_("Configuration"),tab_w);    
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);
    
    glay->addWidget(new QLabel(_("Id:"),tab_w),0,0);
    obj_id = new QLabel(tab_w);
    obj_id->setTextInteractionFlags(Qt::TextSelectableByMouse);
    obj_id->setObjectName("/wdg/cfg/id");
    obj_id->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(obj_id,0,1);
    
    glay->addWidget(new QLabel(_("Root:"),tab_w),1,0);
    obj_root = new QLabel(tab_w);
    obj_root->setTextInteractionFlags(Qt::TextSelectableByMouse);
    obj_root->setObjectName("/wdg/cfg/root");
    obj_root->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(obj_root,1,1);        
    
    glay->addWidget(new QLabel(_("Path:"),tab_w),2,0);
    obj_path = new QLabel(tab_w);
    obj_path->setTextInteractionFlags(Qt::TextSelectableByMouse);
    obj_path->setObjectName("/wdg/cfg/path");
    obj_path->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(obj_path,2,1);
    
    glay->addWidget(new QLabel(_("Name:"),tab_w),3,0);
    obj_name = new LineEdit(tab_w);
    obj_name->setObjectName("/wdg/cfg/name");
    connect(obj_name, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(obj_name,3,1);
    
    glay->addWidget(new QLabel(_("Description:"),tab_w),4,0);
    obj_descr = new TextEdit(tab_w);
    obj_descr->setObjectName("/wdg/cfg/descr");
    connect(obj_descr, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(obj_descr,5,0,1,2);
    
    grp->setLayout(glay);
    dlg_lay->addWidget(grp,1,0,1,2);

    //- Add tab 'Attributes' -
    //------------------------
    wdg_tabs->addTab(new QWidget,_("Attributes"));
    tab_w = wdg_tabs->widget(1);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);

    //-- Add attributes view widget --
    obj_attr = new InspAttr(tab_w,owner());
    connect(obj_attr, SIGNAL(modified(const string&)), this, SIGNAL(apply(const string&)));
    dlg_lay->addWidget(obj_attr,0,0);

    //- Add tab 'Attribute cofiguration' -
    //------------------------------------
    QSplitter *split = new QSplitter();
    split->setOrientation( Qt::Vertical );
    wdg_tabs->addTab(split,_("Widget process"));
    
    QFrame *attr_cf_fr = new QFrame(split);
    attr_cf_fr->setFrameShape(QFrame::StyledPanel);
    attr_cf_fr->setFrameShadow(QFrame::Raised);
    split->addWidget(attr_cf_fr);
    
    glay = new QGridLayout(attr_cf_fr);
    glay->setMargin(9);
    glay->setSpacing(6);
        
    //-- Add attributes configuration widget --
    obj_attr_cfg = new QTreeWidget(attr_cf_fr);
    obj_attr_cfg->setObjectName("/proc/attr");
    obj_attr_cfg->setItemDelegate(new ItemDelegate);
    obj_attr_cfg->setSelectionBehavior(QAbstractItemView::SelectRows);
    obj_attr_cfg->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(obj_attr_cfg, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(changeAttr(QTreeWidgetItem*,int)));
    QStringList headLabels;
    headLabels << _("Id") << _("Name") << _("Data type") << _("Work area") << _("Proc") << _("Config") << _("Config template");
    obj_attr_cfg->setHeaderLabels(headLabels);
    glay->addWidget(obj_attr_cfg,0,0,1,2);
    
    buttAttrAdd = new QPushButton(_("Add attribute"),attr_cf_fr); 
    connect(buttAttrAdd, SIGNAL(clicked()), this, SLOT(addAttr()));
    glay->addWidget(buttAttrAdd,1,0);
    buttAttrDel = new QPushButton(_("Delete attribute"),attr_cf_fr);
    connect(buttAttrDel, SIGNAL(clicked()), this, SLOT(delAttr()));
    glay->addWidget(buttAttrDel,1,1);
    
    QFrame *wdg_proc_fr = new QFrame(split);
    wdg_proc_fr->setFrameShape(QFrame::StyledPanel);
    wdg_proc_fr->setFrameShadow(QFrame::Raised);
    split->addWidget(wdg_proc_fr);

    glay = new QGridLayout(wdg_proc_fr);
    glay->setMargin(9);
    glay->setSpacing(6);
    

    lab = new QLabel(_("Procedure language:"),wdg_proc_fr);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,1,0);
    proc_lang = new QComboBox(wdg_proc_fr);
    proc_lang->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred) );
    proc_lang->setObjectName("/proc/calc/progLng");
    connect(proc_lang, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(proc_lang,1,1);
    lab = new QLabel(_("Procedure calc period:"),wdg_proc_fr);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,1,2);
    proc_per = new LineEdit(wdg_proc_fr);
    proc_per->setObjectName("/proc/calc/per");
    connect( proc_per, SIGNAL(apply()), this, SLOT(isModify()));    
    glay->addWidget(proc_per,1,3);
    proc_text = new TextEdit(wdg_proc_fr);
    proc_text->setObjectName("/proc/calc/prog");
    connect(proc_text, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(proc_text,2,0,1,4);

    //- Add tab 'Links' -
    //------------------------
    wdg_tabs->addTab(new QWidget,_("Links"));
    tab_w = wdg_tabs->widget(3);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);

    //-- Add attributes view widget --
    obj_lnk = new InspLnk(tab_w,owner());
    //connect(obj_attr, SIGNAL(modified(const string&)), this, SIGNAL(apply(const string&)));
    dlg_lay->addWidget(obj_lnk,0,0);

    //- Add button box -
    //------------------
    butbox = new QDialogButtonBox(QDialogButtonBox::Apply|
				  QDialogButtonBox::Cancel|
				  QDialogButtonBox::Close, Qt::Horizontal,this);
    //-- Init Apply button --
    butbox->button(QDialogButtonBox::Apply)->setText(_("Save to DB"));
    if(!ico_t.load(TUIS::icoPath("vision_db_save").c_str())) ico_t.load(":/images/db_save.png");
    butbox->button(QDialogButtonBox::Apply)->setIcon(QPixmap::fromImage(ico_t));
    connect(butbox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(pressApply()));    
    //-- Init Cancel button --
    butbox->button(QDialogButtonBox::Cancel)->setText(_("Load from DB"));
    if(!ico_t.load(TUIS::icoPath("vision_db_load").c_str())) ico_t.load(":/images/db_load.png");
    butbox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    //butbox->button(QDialogButtonBox::Cancel)->setShortcut(QKeySequence("Esc"));
    connect(butbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(pressCancel()));
    //-- Init close button --
    connect(butbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(pressClose()));

    tab_lay->addWidget(butbox);

    //- End resize -
    //--------------
    resize(500,400);    
}

VisItProp::~VisItProp( )
{

}

VisDevelop *VisItProp::owner()
{
    return (VISION::VisDevelop*)parentWidget();
}
 
void VisItProp::showDlg( const string &iit, bool reload )
{
    vector<string> ls;
    string sval;
    QImage ico_t;
    ed_it = iit;

    show_init = true; 

    //- Update elements present, visible and values -
    //-----------------------------------------------
    XMLNode req("get");

    XMLNode info_req("info");
    info_req.setAttr("path",ed_it);
    if( owner()->cntrIfCmd(info_req) )
    {
	mod->postMess( mod->nodePath().c_str(),
        	QString(_("Get node '%1' information error.")).arg(ed_it.c_str()),TVision::Error, this );
	return;
    }
    
    XMLNode *root, *gnd;    
    root = info_req.childGet(0);    

    setWindowTitle( root->attr("dscr").c_str() );

    //- Generic dialog's page -    
    gnd=TCntrNode::ctrId(root,"/wdg",true);
    wdg_tabs->setTabEnabled(0,gnd);
    if( gnd )
    {
    	wdg_tabs->setTabText(0,gnd->attr("dscr").c_str());
    
	//-- Enable stat --
	gnd=TCntrNode::ctrId(root,obj_enable->objectName().toAscii().data(),true);
	obj_enable->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );        
	if( gnd )
	{
	    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_enable->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) obj_enable->setChecked(atoi(req.text().c_str()));
	}
	//-- Parent widget --
	gnd=TCntrNode::ctrId(root,obj_parent->objectName().toAscii().data(),true);	
	obj_parent->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );
	if( gnd ) selectParent( );
	
	//-- User --
	gnd=TCntrNode::ctrId(root,obj_user->objectName().toAscii().data(),true);
	obj_user->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_user->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) sval = req.text();
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/wdg/u_lst",TSYS::PathEl));
	    obj_user->clear();
       	    if( !owner()->cntrIfCmd(req) )
		for(int i_l = 0; i_l < req.childSize(); i_l++)
		{
		    obj_user->addItem(req.childGet(i_l)->text().c_str());
		    if( sval == req.childGet(i_l)->text() )	obj_user->setCurrentIndex(i_l);
		}
	}
	//-- Group --
	gnd=TCntrNode::ctrId(root,obj_grp->objectName().toAscii().data(),true);	
	obj_grp->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );    
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_grp->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) sval = req.text();
   	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/wdg/g_lst",TSYS::PathEl));
    	    obj_grp->clear();
	    if( !owner()->cntrIfCmd(req) )
		for(int i_l = 0; i_l < req.childSize(); i_l++)
		{
		    obj_grp->addItem(req.childGet(i_l)->text().c_str());
		    if( sval == req.childGet(i_l)->text() )	obj_grp->setCurrentIndex(i_l);
		}
	}
	
	//-- Icon --
	gnd=TCntrNode::ctrId(root,obj_ico->objectName().toAscii().data(),true);
	ico_modif = gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR;
	if( gnd )
	{   
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_ico->objectName().toAscii().data(),TSYS::PathEl));
    	    if( !owner()->cntrIfCmd(req) && (sval = TSYS::strDecode(req.text(),TSYS::base64)).size() &&
		    ico_t.loadFromData((const uchar*)sval.c_str(),sval.size()) )
		obj_ico->setIcon(QPixmap::fromImage(ico_t));
	    else obj_ico->setIcon(QIcon());
	}
	//-- Permition --
	int luser_acc, lgrp_acc, loth_acc;
	gnd=TCntrNode::ctrId(root,obj_accuser->objectName().toAscii().data(),true);	
	obj_accuser->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );
	if( gnd )
	{
	    req.clear();
    	    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accuser->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) luser_acc = atoi(req.text().c_str());
	}    
       	gnd=TCntrNode::ctrId(root,obj_accgrp->objectName().toAscii().data(),true);    
	obj_accgrp->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );        
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accgrp->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) lgrp_acc = atoi(req.text().c_str());
	}	
	gnd=TCntrNode::ctrId(root,obj_accother->objectName().toAscii().data(),true);	    
	obj_accother->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );        
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accother->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) loth_acc = atoi(req.text().c_str());
	}
	req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/wdg/a_lst",TSYS::PathEl));
	obj_accuser->clear( );
	obj_accgrp->clear( );
	obj_accother->clear( );
	if( !owner()->cntrIfCmd(req) )
	    for(int i_l = 0; i_l < req.childSize(); i_l++)
	    {
		int vl = atoi(req.childGet(i_l)->attr("id").c_str());
		obj_accuser->addItem(req.childGet(i_l)->text().c_str(),vl);	    
		if( luser_acc == vl )	obj_accuser->setCurrentIndex(i_l);
		obj_accgrp->addItem(req.childGet(i_l)->text().c_str(),vl);
		if( lgrp_acc == vl )	obj_accgrp->setCurrentIndex(i_l);
		obj_accother->addItem(req.childGet(i_l)->text().c_str(),vl);
		if( loth_acc == vl )	obj_accother->setCurrentIndex(i_l);
	    }
	//-- Id --
	gnd=TCntrNode::ctrId(root,obj_id->objectName().toAscii().data(),true);
	if( gnd )
	{   
	    req.clear(); 
    	    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_id->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) obj_id->setText(req.text().c_str());
	}
	//-- Root --
	gnd=TCntrNode::ctrId(root,obj_root->objectName().toAscii().data(),true);
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_root->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) obj_root->setText(req.text().c_str());
	}
	//-- Path --
	gnd=TCntrNode::ctrId(root,obj_path->objectName().toAscii().data(),true);
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_path->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) obj_path->setText(req.text().c_str());
	}	
	//-- Name --
	gnd=TCntrNode::ctrId(root,obj_name->objectName().toAscii().data(),true);    
	obj_name->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR );        
	if( gnd )
	{
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_name->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) obj_name->setValue(req.text().c_str()); 
	}
	//-- Description --
	gnd=TCntrNode::ctrId(root,obj_descr->objectName().toAscii().data(),true);
	obj_descr->setEnabled( gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR ); 
	if( gnd )
	{
	    req.clear();
	    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_descr->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) obj_descr->setText(req.text().c_str()); 
	}
	
	//-- Special fields --
	//--- Page type ---
	gnd=TCntrNode::ctrId(root,pg_tp->objectName().toAscii().data(),true);
	pg_tp->setVisible(gnd); ((QLabel *)TSYS::str2addr(pg_tp->windowIconText().toAscii().data()))->setVisible(gnd);
	if( gnd )
	{
	    pg_tp->setEnabled( atoi(gnd->attr("acs").c_str())&SEQ_WR );
	    
	    int sel_val = 0;
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(pg_tp->objectName().toAscii().data(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) sel_val = atoi(req.text().c_str());
	    //---- Get combo list ----
	    pg_tp->clear();
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(gnd->attr("select"),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) )
		for( int i_el = 0; i_el < req.childSize(); i_el++ )
		    pg_tp->addItem(req.childGet(i_el)->text().c_str(),atoi(req.childGet(i_el)->attr("id").c_str()));
	    pg_tp->setCurrentIndex(pg_tp->findData(sel_val));
	}
    }

    //- Attributes dialog's page -
    gnd=TCntrNode::ctrId(root,"/attr",true);
    wdg_tabs->setTabEnabled(1,gnd);
    if( gnd )	wdg_tabs->setTabText(1,gnd->attr("dscr").c_str());

    gnd=TCntrNode::ctrId(root,"/links",true);
    wdg_tabs->setTabEnabled(3,gnd);
    if( gnd )	wdg_tabs->setTabText(3,gnd->attr("dscr").c_str());

    //- Process dialog's page -
    gnd=TCntrNode::ctrId(root,"/proc",true);
    wdg_tabs->setTabEnabled(2,gnd);
    if( gnd )	wdg_tabs->setTabText(2,gnd->attr("dscr").c_str());
    is_modif = false;

    //- Show dialog -
    show();
    raise();
    activateWindow();
    if( reload ) tabChanged(wdg_tabs->currentIndex());
    else wdg_tabs->setCurrentIndex(0);
    
    show_init = false;
}

void VisItProp::tabChanged( int itb )
{
    switch(itb)
    {
	case 1:	obj_attr->setWdg(ed_it);	break;
	case 2:
	{
	    show_init = true;

	    XMLNode req("get");
	    
	    //-- Get node information --
	    XMLNode info_req("info");
	    info_req.setAttr("path",ed_it);
	    if( owner()->cntrIfCmd(info_req) )
	    {
		mod->postMess( mod->nodePath().c_str(),
			QString(_("Get node '%1' information error.")).arg(ed_it.c_str()),TVision::Error, this );
		return;
	    }
	    string sval;
	    XMLNode *root, *gnd;    
	    root = info_req.childGet(0);     
	    //-- Get widgets list --
	    vector<string>	wlst;
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/proc/w_lst",TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) )
		for( int i_w = 0; i_w < req.childSize(); i_w++ )
		    wlst.push_back(req.childGet(i_w)->text());
		    //wlst.push_back(req.childGet(i_w)->attr("id"));
	    //--- Fill table ---
	    //--- Delete no present root items ---
	    for( int i_r = 0; i_r < obj_attr_cfg->topLevelItemCount(); i_r++ )
	    {
		int i_w;
		for( i_w = 0; i_w < wlst.size(); i_w++ )
		    if( obj_attr_cfg->topLevelItem(i_r)->text(0) == wlst[i_w].c_str() ) break;
		if( i_w >= wlst.size() )	delete obj_attr_cfg->topLevelItem(i_r--);
	    }
	    //--- Add root items ---
	    for( int i_w = 0; i_w < wlst.size(); i_w++ )
	    {
		QTreeWidgetItem *root_it;	
		int i_r;
		for( i_r = 0; i_r < obj_attr_cfg->topLevelItemCount(); i_r++ )
		    if( obj_attr_cfg->topLevelItem(i_r)->text(0) == wlst[i_w].c_str() ) break;		
		if( i_r < obj_attr_cfg->topLevelItemCount() ) root_it = obj_attr_cfg->topLevelItem(i_r);
		else root_it = new QTreeWidgetItem(0);
	
		req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_attr_cfg->objectName().toAscii().data(),TSYS::PathEl))->
		    setAttr("wdg",wlst[i_w].c_str());
		if( owner()->cntrIfCmd(req) ) continue;
	    
		root_it->setText(0,wlst[i_w].c_str());
		root_it->setData(0,Qt::UserRole,0);
		obj_attr_cfg->addTopLevelItem(root_it);
	    
		//--- Delete no presents widget's items ---
		for( int i_r = 0; i_r < root_it->childCount(); i_r++ )
		{
		    int i_l;
		    for( i_l = 0; i_l < req.childGet(0)->childSize(); i_l++ )
			if( root_it->child(i_r)->text(0) == req.childGet("id","id")->childGet(i_l)->text().c_str() )
			    break;
		    if( i_l >= req.childGet(0)->childSize() )	delete root_it->child(i_r--);
		}
	    
		//--- Add widget's items ---
		for( int i_l = 0; i_l < req.childGet(0)->childSize(); i_l++ )
		{
		    QTreeWidgetItem *cur_it;	    
		    int i_r;
		    for( i_r = 0; i_r < root_it->childCount(); i_r++ )
			if( root_it->child(i_r)->text(0) == req.childGet("id","id")->childGet(i_l)->text().c_str() ) 
			    break;
		    if( i_r < root_it->childCount() ) cur_it = root_it->child(i_r);
		    else cur_it = new QTreeWidgetItem(root_it);
		    cur_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
		    cur_it->setText(0,req.childGet("id","id")->childGet(i_l)->text().c_str());
		    cur_it->setData(0,Qt::UserRole,cur_it->text(0));
		    cur_it->setText(1,req.childGet("id","name")->childGet(i_l)->text().c_str());
 		    cur_it->setData(2,Qt::DisplayRole,atoi(req.childGet("id","type")->childGet(i_l)->text().c_str()));
		    cur_it->setText(3,req.childGet("id","wa")->childGet(i_l)->text().c_str());
 		    cur_it->setData(4,Qt::DisplayRole,(bool)atoi(req.childGet("id","proc")->childGet(i_l)->text().c_str()));
 		    cur_it->setData(5,Qt::DisplayRole,atoi(req.childGet("id","cfg")->childGet(i_l)->text().c_str()));
 		    cur_it->setText(6,req.childGet("id","cfgtmpl")->childGet(i_l)->text().c_str());
		}
	    }
	    //--- Load types and configs ---
	    QStringList	atypes;
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/proc/tp_ls",TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) )
		for( int i_el = 0; i_el < req.childSize(); i_el++ )
		    atypes.push_back( (req.childGet(i_el)->text()+"|"+req.childGet(i_el)->attr("id")).c_str() );
	    if( obj_attr_cfg->topLevelItemCount() )
		obj_attr_cfg->topLevelItem(0)->setData(0,Qt::UserRole,atypes);
	    atypes.clear();
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/proc/lnk_ls",TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) )
		for( int i_el = 0; i_el < req.childSize(); i_el++ )
		    atypes.push_back( (req.childGet(i_el)->text()+"|"+req.childGet(i_el)->attr("id")).c_str() );
	    if( obj_attr_cfg->topLevelItemCount() )
		obj_attr_cfg->topLevelItem(0)->setData(0,Qt::UserRole+1,atypes);
	    
	    //--- Calc period ---
	    gnd = TCntrNode::ctrId(root,proc_per->objectName().toAscii().data(),true);
	    proc_per->setEnabled(gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR);
	    if( gnd )
	    {
		req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(proc_per->objectName().toAscii().data(),TSYS::PathEl));
		if( !owner()->cntrIfCmd(req) ) proc_per->setValue(req.text().c_str());
	    }	    
    	    //--- Calc language ---
	    gnd = TCntrNode::ctrId(root,proc_lang->objectName().toAscii().data(),true);
	    proc_lang->setEnabled(gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR);
	    if( gnd )
	    {
		req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(proc_lang->objectName().toAscii().data(),TSYS::PathEl));
		if( !owner()->cntrIfCmd(req) ) sval = req.text().c_str();
		//---- Get combo list ----
		proc_lang->clear();
		req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(gnd->attr("select"),TSYS::PathEl));
		if( !owner()->cntrIfCmd(req) )
		    for( int i_el = 0; i_el < req.childSize(); i_el++ )
			proc_lang->addItem(req.childGet(i_el)->text().c_str());
		int cur_el = proc_lang->findText(sval.c_str());
		if( cur_el < 0 ) proc_lang->addItem(sval.c_str());
		proc_lang->setCurrentIndex(proc_lang->findText(sval.c_str()));
	    }
	    //--- Calc procedure ---
 	    gnd=TCntrNode::ctrId(root,proc_text->objectName().toAscii().data(),true);
	    proc_text->setEnabled(gnd && atoi(gnd->attr("acs").c_str())&SEQ_WR);
	    if( gnd )
	    {
		req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(proc_text->objectName().toAscii().data(),TSYS::PathEl));
		if( !owner()->cntrIfCmd(req) )	proc_text->setText(req.text().c_str());
	    }

	    show_init = false;
	    break;
	}
	case 3:	obj_lnk->setWdg(ed_it);	break;
    }
}

void VisItProp::selectParent( )
{
    XMLNode req("get");    
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_parent->objectName().toAscii().data(),TSYS::PathEl));
    if( !owner()->cntrIfCmd(req) )
    {
	QString cur_val = req.text().c_str();
     
        //- Get values list -
	req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/wdg/w_lst",TSYS::PathEl));
	owner()->cntrIfCmd(req);
	
	//- Load combobox -
	obj_parent->clear();
	for( int i_l = 0; i_l < req.childSize(); i_l++ )
	    obj_parent->addItem(req.childGet(i_l)->text().c_str());
	if( obj_parent->findText(cur_val) < 0 ) obj_parent->addItem(cur_val);
	obj_parent->setCurrentIndex(obj_parent->findText(cur_val));
    }
}

void VisItProp::selectIco( )
{
    QImage ico_t;
    
    if( !ico_modif )	return;
    
    QString fileName = QFileDialog::getOpenFileName(this,_("Load icon picture"),"",_("Images (*.png *.jpg)"));
    
    if( fileName.isEmpty() )	return;
    if(!ico_t.load(fileName))
    {
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Loaded icon image '%1' error.")).arg(fileName),TVision::Warning, this );
	return;    
    }
    
    obj_ico->setIcon(QPixmap::fromImage(ico_t));

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    ico_t.save(&buffer,"PNG");
    
    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_ico->objectName().toAscii().data(),TSYS::PathEl))->
	setText(TSYS::strEncode(string(ba.data(),ba.size()),TSYS::base64));
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    is_modif = true;    
}

void VisItProp::isModify( )
{
    bool update = false;
    if( show_init )	return;

    //- Prepare command -    
    QString oname = sender( )->objectName();
    
    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(oname.toAscii().data(),TSYS::PathEl));
    
    if( oname == obj_enable->objectName() )
    {
	req.setText(TSYS::int2str(((QCheckBox*)sender())->isChecked()));
	update = true;
    }
    else if( oname == obj_parent->objectName() )
 	req.setText(((QComboBox*)sender())->currentText().toAscii().data());
    else if( oname == obj_user->objectName() || oname == obj_grp->objectName() || oname == proc_lang->objectName() )
    {
	req.setText(((QComboBox*)sender())->currentText().toAscii().data());
	update = true;
    }
    else if( oname == obj_accuser->objectName() || oname == obj_accgrp->objectName() || oname == obj_accother->objectName() )
    {
	req.setText(((QComboBox*)sender())->itemData(((QComboBox*)sender())->currentIndex()).toString().toAscii().data());
	update = true;
    }
    else if( oname == obj_name->objectName() || oname == proc_per->objectName() )
	req.setText(((LineEdit*)sender())->value().toAscii().data());
    else if( oname == obj_descr->objectName() || oname == proc_text->objectName() )
	req.setText(((TextEdit*)sender())->text().toAscii().data());
    else if( oname == pg_tp->objectName() )
    {
	req.setText(((QComboBox*)sender())->itemData(((QComboBox*)sender())->currentIndex()).toString().toAscii().data());
	update = true;	
    }
    else return;
    
    //- Send command -
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
    else 
    {
	//- Post command updating -
	if( oname == obj_parent->objectName() )	selectParent();
	if( update )	showDlg(ed_it,true);
    }
    
    is_modif = true;
}

void VisItProp::pressApply( )
{    
    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/wdg/cfg/save",TSYS::PathEl));
        
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this); 

    emit apply(ed_it);
    
    is_modif = false;
}

void VisItProp::pressCancel( )
{  
    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/wdg/cfg/load",TSYS::PathEl));
        
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
    else showDlg(ed_it,true);     
}

void VisItProp::pressClose( )
{
    if( is_modif )
    {
	InputDlg dlg(this,obj_ico->icon(),
             _("Some attributes is changed. You sure to close window?"),_("Close window"),false,false);
	if( dlg.exec() != QDialog::Accepted )	return;
    }			    

    is_modif = false;
    ed_it = "";
    hide();
}

void VisItProp::addAttr( )
{
    string swdg;
    if( obj_attr_cfg->currentItem() )
    {	
	if( obj_attr_cfg->currentItem()->parent() )
	    swdg = obj_attr_cfg->currentItem()->parent()->text(0).toAscii().data();
	else swdg = obj_attr_cfg->currentItem()->text(0).toAscii().data();
    }
    //- Check current attribute -
    if( swdg.empty() )
    {
	mod->postMess( mod->nodePath().c_str(), _("Valid widget no selected"),TVision::Warning,this);
	return;
    }    

    XMLNode req("add");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_attr_cfg->objectName().toAscii().data(),TSYS::PathEl))->
	setAttr("wdg",swdg);
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
    else 
    {
	tabChanged(2);
	is_modif = true;
    }
}

void VisItProp::delAttr( )
{
    //- Check current attribute -
    if( !obj_attr_cfg->currentItem() || !obj_attr_cfg->currentItem()->parent() )
    {
	mod->postMess( mod->nodePath().c_str(), _("Valid attribute no selected"),TVision::Warning,this);
	return;
    }    
    
    //- Delete current attribute -
    XMLNode req("del");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_attr_cfg->objectName().toAscii().data(),TSYS::PathEl))->
	setAttr("wdg",obj_attr_cfg->currentItem()->parent()->text(0).toAscii().data())->
	setAttr("key_id",obj_attr_cfg->currentItem()->text(0).toAscii().data());
    if( owner()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
    else 
    {
	delete obj_attr_cfg->currentItem();
	is_modif = true;
    }
}	

void VisItProp::changeAttr(QTreeWidgetItem *it, int col)
{
    if( show_init )     return;
    //- Check current attribute -
    if( !it || !it->parent() )
    {
	mod->postMess( mod->nodePath().c_str(), _("Valid attribute no selected"),TVision::Warning,this);
	return;
    }
    
    //- Get collumn id -
    QString scol, sval;
    switch(col)
    {
	case 0:	scol = "id";	sval = it->text(col);	break;
	case 1: scol = "name";	sval = it->text(col);	break;
	case 2:	scol = "type";	sval = it->data(col,Qt::DisplayRole).toString();	break;
	case 3:	scol = "wa";	sval = it->text(col);	break;
	case 4: scol = "proc";	sval = QString::number(it->data(col,Qt::DisplayRole).toBool());	break;
	case 5:	scol = "cfg";	sval = it->data(col,Qt::DisplayRole).toString();	break;
	case 6: scol = "cfgtmpl";	sval = it->text(col);   break;
    }
    
    //- Set current attribute -
    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_attr_cfg->objectName().toAscii().data(),TSYS::PathEl))->
	setAttr("wdg",it->parent()->text(0).toAscii().data())->
	setAttr("key_id",it->data(0,Qt::UserRole).toString().toAscii().data())->
	setAttr("col",scol.toAscii().data())->setText(sval.toAscii().data());
    if( owner()->cntrIfCmd(req) )
    {
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);	
	if( scol == "id" )
	{
	    show_init = true;	
	    it->setText(0,it->data(0,Qt::UserRole).toString());
	    show_init = false;
	}	    
	tabChanged(2);
    }
    else     
    {
	if( scol == "id" )
	{
	    show_init = true;
	    it->setData(0,Qt::UserRole,it->text(0));
	    show_init = false;
	}
	
	is_modif = true;
    }
}

//* Attributes item delegate    *
VisItProp::ItemDelegate::ItemDelegate( QObject *parent ) : QItemDelegate(parent)
{

}

void VisItProp::ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if( index.isValid() && !index.model()->rowCount(index) )
    {
	if( index.column() == 2 || index.column() == 5 )
	{
    	    QString val("String");
	    QStringList types = index.model()->index(0,0).data(Qt::UserRole+((index.column()==5)?1:0)).toStringList();
	    for( int i_l = 0; i_l < types.size(); i_l++ )
		if( atoi(TSYS::strSepParse(types[i_l].toAscii().data(),1,'|').c_str()) == index.data(Qt::DisplayRole).toInt() )
		    val = TSYS::strSepParse(types[i_l].toAscii().data(),0,'|').c_str();
	    drawDisplay(painter, option, option.rect,val);
	    return;
	}
	if( index.column() == 4 )
	{
	    drawBackground(painter, option, index);
    	    if( index.data(Qt::DisplayRole).toBool() )
	    {
		QImage img(":/images/ok.png");
		painter->drawImage(option.rect.center().x()-img.width()/2,
				option.rect.center().y()-img.height()/2,img);
	    }
	    drawFocus(painter, option, option.rect);
	    return;
	}
    }
    QItemDelegate::paint(painter,option,index);
}

QWidget *VisItProp::ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *w_del = NULL;
    if(!index.isValid()) return 0;

    int flg = index.model()->index(index.row(),0,index.parent()).data(Qt::UserRole).toInt();
    QVariant value = index.data(Qt::EditRole);
    
    if( index.parent().isValid() )
    { 
	if( index.column() == 2 || index.column() == 5 )
	    w_del = new QComboBox(parent);
	else 
	{
    	    QItemEditorFactory factory;
    	    w_del = factory.createEditor(value.type(),parent);
	}
    }
    
    return w_del;
}

void VisItProp::ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if( index.column() == 2 || index.column() == 5 )
    {
	QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	QStringList types = index.model()->index(0,0).data(Qt::UserRole+((index.column()==5)?1:0)).toStringList();
	for( int i_l = 0; i_l < types.size(); i_l++ )
	    comb->addItem(TSYS::strSepParse(types[i_l].toAscii().data(),0,'|').c_str(),
		    atoi(TSYS::strSepParse(types[i_l].toAscii().data(),1,'|').c_str()));
	comb->setCurrentIndex(comb->findData(index.data(Qt::DisplayRole).toInt()));
    }
    else QItemDelegate::setEditorData(editor, index);
}

void VisItProp::ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if( index.column() == 2 || index.column() == 5 )
    {
        QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	model->setData(index,comb->itemData(comb->currentIndex()),Qt::EditRole);
    }
    else if( index.column() == 4 )
    {	
	QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	model->setData(index,(bool)comb->currentIndex(),Qt::EditRole);
    }
    else QItemDelegate::setModelData(editor, model, index);
}

