
//OpenSCADA system module UI.Vision file: vis_devel_dlgs.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
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

#include <QTreeWidget>
#include <QItemEditorFactory>
#include <QPushButton>
#include <QAction>
#include <QGroupBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
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
WdgLibProp::WdgLibProp( VisDevelop *parent ) : 
    QDialog((QWidget*)parent), show_init(false), is_modif(false), ico_modif(false)
{
    QLabel *lab;
    QGroupBox *grp;
    QGridLayout *dlg_lay, *glay;
    QImage ico_t;
    setWindowTitle(_("Widget's library properties"));
    setWindowIcon(owner()->actWdgLibProp->icon());

    //- Create tabulator -
    QVBoxLayout *tab_lay = new QVBoxLayout(this);
    tab_lay->setMargin(5);
    wdg_tabs = new QTabWidget(this);
    tab_lay->addWidget(wdg_tabs);
    
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
    wlb_ico = new QPushButton(tab_w);
    wlb_ico->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );
    wlb_ico->setIconSize(QSize(60,60));
    wlb_ico->setAutoDefault(false);
    connect(wlb_ico, SIGNAL(released()), this, SLOT(selectIco()));
    glay->addWidget(wlb_ico,0,0,3,1);
    lab = new QLabel(_("Enabled:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,0,1);
    wlb_enable = new QCheckBox(tab_w);
    connect(wlb_enable, SIGNAL(stateChanged(int)), this, SLOT(isModify()));
    glay->addWidget(wlb_enable,0,2,1,2);    
    lab = new QLabel(_("Library DB:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,1,1);
    wlb_db = new QLineEdit(tab_w);
    connect(wlb_db, SIGNAL(textChanged(const QString&)), this, SLOT(isModify()));
    glay->addWidget(wlb_db,1,2,1,2);
    lab = new QLabel(_("User and group:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,2,1);    
    wlb_user = new QComboBox(tab_w);
    connect(wlb_user, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectUser(const QString&)));
    glay->addWidget(wlb_user,2,2);
    wlb_grp = new QComboBox(tab_w);
    connect(wlb_grp, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectPermission()));        
    glay->addWidget(wlb_grp,2,3);
    grp->setLayout(glay);    
    dlg_lay->addWidget(grp,0,0);
    
    //- Access parameters -
    grp = new QGroupBox(_("Access"),tab_w);
    QStringList perm_ls;
    perm_ls << _("No access") << _("Use(open)") << _("Modify") << _("Full");        
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);    
    lab = new QLabel(_("User:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,0,0);
    wlb_accuser = new QComboBox(tab_w);
    wlb_accuser->addItems(perm_ls);    
    connect(wlb_accuser, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wlb_accuser,0,1);    
    lab = new QLabel(_("Group:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,1,0);    
    wlb_accgrp  = new QComboBox(tab_w);
    wlb_accgrp->addItems(perm_ls);    
    connect(wlb_accgrp, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wlb_accgrp,1,1);    
    lab = new QLabel(_("Other:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,2,0);
    wlb_accother= new QComboBox(tab_w);
    wlb_accother->addItems(perm_ls);    
    connect(wlb_accother, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wlb_accother,2,1);
    grp->setLayout(glay);    
    dlg_lay->addWidget(grp,0,1);    
    
    //- Config parameters -
    grp = new QGroupBox(_("Configuration"),tab_w);    
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);
    glay->addWidget(new QLabel(_("Id:"),tab_w),0,0);
    wlb_id = new QLabel(this);
    wlb_id->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(wlb_id,0,1);    
    glay->addWidget(new QLabel(_("Name:"),tab_w),1,0);
    wlb_name = new QLineEdit(this);
    connect(wlb_name, SIGNAL(textChanged(const QString&)), this, SLOT(isModify()));
    glay->addWidget(wlb_name,1,1);  
    glay->addWidget(new QLabel(_("Description:"),tab_w),2,0);
    wlb_descr = new QTextEdit(this);
    connect(wlb_descr, SIGNAL(textChanged()), this, SLOT(isModify()));
    glay->addWidget(wlb_descr,3,0,1,2);
    grp->setLayout(glay);
    dlg_lay->addWidget(grp,1,0,1,2);    

    //- Add tab 'Mime data' -
    //------------------------
    wdg_tabs->addTab(new QWidget,_("Mime data"));
    tab_w = wdg_tabs->widget(1);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);
    
    mimeDataTable = new QTableWidget(0,4,tab_w);     
    mimeDataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(mimeDataTable, SIGNAL(cellChanged(int,int)), this, SLOT(mimeDataChange(int,int)));
    QStringList headLabels;
    headLabels << _("Id") << _("Mime type") << _("Data size") << "*";
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
    butbox->button(QDialogButtonBox::Apply)->setText(_("Apply"));
    if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t.load(":/images/button_ok.png");
    butbox->button(QDialogButtonBox::Apply)->setIcon(QPixmap::fromImage(ico_t));
    butbox->button(QDialogButtonBox::Apply)->setEnabled(false);
    connect(butbox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(pressApply()));    
    //-- Init Cancel button --
    butbox->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t.load(":/images/button_cancel.png");
    butbox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    butbox->button(QDialogButtonBox::Cancel)->setShortcut(QKeySequence("Esc"));
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    connect(butbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(pressCancel()));
    //-- Init close button --
    connect(butbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(pressClose()));

    tab_lay->addWidget(butbox);

    //- End resize -
    //-------------- 
    resize(500,400);    
}

WdgLibProp::~WdgLibProp( )
{

}

VisDevelop *WdgLibProp::owner()
{
    return (VISION::VisDevelop*)parentWidget();
}

string WdgLibProp::user()
{
    return wlb_user->currentText().toAscii().data();
}

string WdgLibProp::grp()
{
    if(wlb_grp->currentText().isEmpty())
	return "UI";
    return wlb_grp->currentText().toAscii().data();
}		

short WdgLibProp::permit()
{
    string accs;
    short permit = 0, wperm;

    for(int i_p = 0; i_p < 3; i_p++)
    {
	switch(i_p)
	{
	    case 0: accs = wlb_accother->currentText().toAscii().data();break;
	    case 1: accs = wlb_accgrp->currentText().toAscii().data();	break;		
	    case 2: accs = wlb_accuser->currentText().toAscii().data();	break;
	}
	wperm = 0;
	if( accs == _("Use(open)") )	wperm = 4;
	else if( accs == _("Modify") )	wperm = 2;
	else if( accs == _("Full") )	wperm = 6;

	permit|=(wperm<<(i_p*3));
    }
    
    return permit;
}

void WdgLibProp::setPermit( short vl )
{
    short wperm = vl;
    string accs;
    
    for(int i_p = 0; i_p < 3; i_p++)
    {
        switch(wperm&0x7)
        {
    	    case 0: accs = _("No access");	break;
	    case 4: accs = _("Use(open)");	break;
	    case 2: accs = _("Modify");	break;
	    case 6: accs = _("Full");	break;
	}
	switch(i_p)
	{
	    case 0: wlb_accother->setCurrentIndex(wlb_accother->findText(accs.c_str()));	break;
	    case 1: wlb_accgrp->setCurrentIndex(wlb_accother->findText(accs.c_str()));	break;		
	    case 2: wlb_accuser->setCurrentIndex(wlb_accother->findText(accs.c_str()));	break;
	}
	wperm>>=3;
    }
}
 
void WdgLibProp::showDlg( const string &ilb, bool reload )
{
    vector<string> ls;
    QImage ico_t;
    AutoHD<VCA::WidgetLib> wlb;
    ed_lib = ilb;    

    //- Load library data -
    try{ wlb = mod->engine().at().wlbAt(ilb); }
    catch(TError err)
    { 
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Library '%1' no present.")).arg(ilb.c_str()),TVision::Warning, this );
	return;
    }

    show_init = true;
    //-- Load library icon --
    string simg = TSYS::strDecode(wlb.at().ico(),TSYS::base64);
    if(ico_t.loadFromData((const uchar*)simg.c_str(),simg.size()))
	wlb_ico->setIcon(QPixmap::fromImage(ico_t));
    else wlb_ico->setIcon(QIcon());
    //setWindowIcon(QPixmap::fromImage(ico_t));
    //-- Load library state --
    wlb_enable->setChecked(wlb.at().enable());
    //-- Load library DB --
    wlb_db->setText(wlb.at().fullDB().c_str());
    //-- Load users and groups --
    SYS->security().at().usrList(ls);
    //--- Delete users ---
    wlb_user->clear();
    for(int i_l = 0; i_l < ls.size(); i_l++)
    {
	string simg = TSYS::strDecode(SYS->security().at().usrAt(ls[i_l]).at().picture(),TSYS::base64);
        QImage img;
        if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
            wlb_user->addItem(QPixmap::fromImage(img),ls[i_l].c_str());
        else wlb_user->addItem(ls[i_l].c_str());
	if( wlb.at().user() == ls[i_l] ) wlb_user->setCurrentIndex(i_l);
    }
    wlb_grp->setCurrentIndex(wlb_grp->findText(wlb.at().grp().c_str()));
    setPermit(wlb.at().permit());
    
    //selectGroup(wlb.at().grp().c_str());
    //-- Load library identifier, name and description --    
    wlb_id->setText(wlb.at().id().c_str());
    wlb_name->setText(wlb.at().name().c_str());
    wlb_descr->setPlainText(wlb.at().descr().c_str());
    
    //- Load mime data -
    wlb.at().mimeDataList(ls);
    mimeDataTable->setRowCount(ls.size());
    QTableWidgetItem *w_it;
    for( int i_l = 0; i_l < ls.size(); i_l++ )
    {
	string mimeType;
	wlb.at().mimeDataGet( ls[i_l], mimeType );
	if( !mimeDataTable->item(i_l,0) ) 	
	{
	    mimeDataTable->setItem(i_l,0,new QTableWidgetItem());
	    mimeDataTable->item(i_l,0)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
	}
	mimeDataTable->item(i_l,0)->setText(ls[i_l].c_str());
	mimeDataTable->item(i_l,0)->setData(Qt::UserRole,ls[i_l].c_str());
	if( !mimeDataTable->item(i_l,1) )
	{
	    mimeDataTable->setItem(i_l,1,new QTableWidgetItem());
	    mimeDataTable->item(i_l,1)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
	}
	mimeDataTable->item(i_l,1)->setText(TSYS::strSepParse(mimeType,0,';').c_str());	
	if( !mimeDataTable->item(i_l,2) ) 
	{
	    mimeDataTable->setItem(i_l,2,new QTableWidgetItem());
	    mimeDataTable->item(i_l,2)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	}
	mimeDataTable->item(i_l,2)->setText(TSYS::strSepParse(mimeType,1,';').c_str());
	if( !mimeDataTable->item(i_l,3) ) 
	{
	    mimeDataTable->setItem(i_l,3,new QTableWidgetItem());
	    mimeDataTable->item(i_l,3)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	}
	mimeDataTable->item(i_l,3)->setCheckState(Qt::Unchecked);
    }
    mimeDataTable->resizeColumnsToContents();

    //- Disable OK and Cancel buttons -
    is_modif = false;
    butbox->button(QDialogButtonBox::Apply)->setEnabled(false);
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(false);

    //- Show dialog -
    show();
    raise();
    activateWindow();

    if( !reload ) wdg_tabs->setCurrentIndex(0);

    show_init = false;
}

void WdgLibProp::selectIco( )
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
    
    wlb_ico->setIcon(QPixmap::fromImage(ico_t));
    
    isModify();
}

void WdgLibProp::isModify( )
{
    is_modif = true;
    butbox->button(QDialogButtonBox::Apply)->setEnabled(true);
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(true);    
}

void WdgLibProp::pressApply( )
{    
    AutoHD<VCA::WidgetLib> wlb;
    //- Download modified data -

    try
    { 
	//-- Open library --    
	wlb = mod->engine().at().wlbAt(ed_lib); 

	//-- Save library icon --
	if(!wlb_ico->icon().isNull())
	{
	    QByteArray ba;
    	    QBuffer buffer(&ba);
	    buffer.open(QIODevice::WriteOnly);
	    wlb_ico->icon().pixmap(64,64).save(&buffer,"PNG");
	    wlb.at().setIco(TSYS::strEncode(string(ba.data(),ba.size()),TSYS::base64));
	}
	//-- Save library DB --
	wlb.at().setFullDB(wlb_db->text().toAscii().data());
	//-- Save users and groups --
	wlb.at().setUser(user());
	wlb.at().setGrp(grp());
	//-- Set permition --
	wlb.at().setPermit(permit());
	//-- Save library name and description --
	wlb.at().setName(wlb_name->text().toAscii().data());
	wlb.at().setDescr(wlb_descr->toPlainText().toAscii().data());    
	//-- Save library state --
	wlb.at().setEnable(wlb_enable->isChecked());	
	
	//-- Save of the mime data container changes --
	//--- Update changed and add new records ---
	for( int i_rw = 0; i_rw < mimeDataTable->rowCount(); i_rw++ )
	{
	    if( mimeDataTable->item(i_rw,3)->checkState() != Qt::Checked )
		continue;
	    QByteArray data = mimeDataTable->item(i_rw,2)->data(Qt::UserRole).toByteArray();
	    if( mimeDataTable->item(i_rw,0)->text() != mimeDataTable->item(i_rw,0)->data(Qt::UserRole).toString() &&
		    !data.size() )
	    {
		string mimeType, mimeData;
		wlb.at().mimeDataGet( mimeDataTable->item(i_rw,0)->data(Qt::UserRole).toString().toAscii().data(),
				      mimeType, &mimeData );
		mimeData = TSYS::strDecode(string(mimeData.data(),mimeData.size()),TSYS::base64);
		data = QByteArray::fromRawData(mimeData.data(),mimeData.size());						
	    }	    
	    wlb.at().mimeDataSet( mimeDataTable->item(i_rw,0)->text().toAscii().data(),
				  (mimeDataTable->item(i_rw,1)->text()+";"+mimeDataTable->item(i_rw,2)->text()).toAscii().data(),
				  TSYS::strEncode(string(data.data(),data.size()),TSYS::base64) );
	}
	//--- Check for deleted records ---
	vector<string> ls;
	wlb.at().mimeDataList(ls);
	for( int i_l = 0; i_l < ls.size(); i_l++ )
	{
	    int i_rw;
	    for( i_rw = 0; i_rw < mimeDataTable->rowCount(); i_rw++ )
		if( mimeDataTable->item(i_rw,0)->text() == ls[i_l].c_str() )
		    break;
	    if( i_rw >= mimeDataTable->rowCount() )
		wlb.at().mimeDataDel(ls[i_l]);
	}	
    }
    catch(TError err)
    { 
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Library '%1' error: %2")).arg(ed_lib.c_str()).arg(err.mess.c_str()), 
		TVision::Warning, this );
	return;
    }    
    
    //- Update widgets tree and toolbars -
    emit apply(ed_lib);
    
    //- Reload date -
    showDlg(ed_lib,true);
}

void WdgLibProp::pressCancel( )
{    
    showDlg(ed_lib,true);
}

void WdgLibProp::pressClose( )
{
    if( is_modif )
    {
	InputDlg dlg(this,wlb_ico->icon(),
             _("Some attributes is changed. You sure to close window?"),_("Close window"),false,false);
	if( dlg.exec() != QDialog::Accepted )	return;
    }			    

    is_modif = false;
    ed_lib = "";
    hide();
}

void WdgLibProp::selectUser(const QString &val)
{
    //Get default users group
    vector<string> gls;
    SYS->security().at().usrGrpList(val.toAscii().data(),gls);    
    string vgrp = gls.size()?gls[0].c_str():"UI";
    wlb_grp->clear();
    for(int i_l = 0; i_l < gls.size(); i_l++)
    {
	wlb_grp->addItem(gls[i_l].c_str());
        if( vgrp == gls[i_l] ) wlb_grp->setCurrentIndex(i_l);
    }
    selectPermission();    
}

void WdgLibProp::selectPermission( )
{
    wlb_enable->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWRWR_));
    wlb_db->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wlb_user->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,"root","root",RWRWR_));
    wlb_grp->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wlb_accuser->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wlb_accgrp->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wlb_accother->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wlb_name->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit()));
    wlb_descr->setEnabled( SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit()));
    ico_modif = SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit());
    
    isModify( );    
}

void WdgLibProp::addMimeData( )
{
    int row = mimeDataTable->rowCount();
    mimeDataTable->setRowCount(row+1);
    QTableWidgetItem *w_it;
    w_it = new QTableWidgetItem(_("New mime data"));
    w_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
    mimeDataTable->setItem(row,0,w_it);
    w_it = new QTableWidgetItem(_("image/png"));
    w_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
    mimeDataTable->setItem(row,1,w_it);
    w_it = new QTableWidgetItem("0");
    w_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    mimeDataTable->setItem(row,2,w_it);
    w_it = new QTableWidgetItem();
    w_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    w_it->setCheckState(Qt::Checked);
    mimeDataTable->setItem(row,3,w_it);
    
    isModify( );
}

void WdgLibProp::delMimeData( )
{
    int row = mimeDataTable->currentRow( );
    if( row < 0 )
    {
	mod->postMess( mod->nodePath().c_str(),_("No one row is selected."),TVision::Warning,this );
	return;
    }
    
    //Request to confirm
    InputDlg dlg(this,this->windowIcon(),
	QString(_("You sure for delete data record '%1'.")).arg(mimeDataTable->item(row,0)->text()),
		_("Delete data record"),false,false);
    if( dlg.exec() == QDialog::Accepted )
	mimeDataTable->removeRow(row);
}

void WdgLibProp::loadMimeData( )
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
		QString(_("Filed to open file '%1': %2")).arg(fileName).arg(file.errorString()),TVision::Error,this);

    QByteArray data = file.readAll();
    //mimeDataTable->item(row,0)->setText(fileName);
    mimeDataTable->item(row,2)->setData(Qt::DisplayRole,(float)data.size()/1000.);
    mimeDataTable->item(row,2)->setData(Qt::UserRole,data);
    mimeDataTable->item(row,3)->setCheckState(Qt::Checked);
        
    isModify();
}

void WdgLibProp::unloadMimeData( )
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
		QString(_("Filed to open file '%1': %2")).arg(fileName).arg(file.errorString()),TVision::Error,this);

    if( !mimeDataTable->item(row,2)->data(Qt::UserRole).toByteArray().size() )
	try
	{
    	    AutoHD<VCA::WidgetLib> wlb = mod->engine().at().wlbAt(ed_lib); 
	    string mimeType, mimeData;
	    wlb.at().mimeDataGet( mimeDataTable->item(row,0)->text().toAscii().data(), mimeType, &mimeData );
	    mimeData = TSYS::strDecode(string(mimeData.data(),mimeData.size()),TSYS::base64);
	    mimeDataTable->item(row,2)->setData(Qt::UserRole,
		    QByteArray::fromRawData(mimeData.data(),mimeData.size()));
	}
	catch(TError err)
	{ 
	    mod->postMess( mod->nodePath().c_str(), 
		    QString(_("Library '%1' error: %2")).arg(ed_lib.c_str()).arg(err.mess.c_str()), 
		    TVision::Warning, this );
	    return;
	}    	

    if( file.write(mimeDataTable->item(row,2)->data(Qt::UserRole).toByteArray()) < 0 )
	mod->postMess( mod->nodePath().c_str(),
		QString(_("Filed writing data to file '%1': %2")).arg(fileName).arg(file.errorString()),TVision::Error,this);
}

void WdgLibProp::mimeDataChange( int row, int column )
{
    if( show_init ) return;
    if( mimeDataTable->item(row,3) )
	mimeDataTable->item(row,3)->setCheckState(Qt::Checked);
	
    isModify();
}


//****************************************
//* Widget properties dialog             *
//****************************************
WdgProp::WdgProp( VisDevelop *parent ) : 
    QDialog((QWidget*)parent), is_modif(false), ico_modif(false), show_init(false)
{
    QLabel *lab;
    QGroupBox *grp;
    QGridLayout *dlg_lay, *glay;
    QImage ico_t;
    setWindowTitle(_("Widget properties"));
    setWindowIcon(owner()->actWdgProp->icon());

    //- Create tabulator -
    QVBoxLayout *tab_lay = new QVBoxLayout(this);
    tab_lay->setMargin(5);
    wdg_tabs = new QTabWidget(this);
    tab_lay->addWidget(wdg_tabs);
    
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
    wdg_ico = new QPushButton(tab_w);
    wdg_ico->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );
    wdg_ico->setIconSize(QSize(60,60));
    wdg_ico->setAutoDefault(false);
    connect(wdg_ico, SIGNAL(released()), this, SLOT(selectIco()));
    glay->addWidget(wdg_ico,0,0,3,1);    
    lab = new QLabel(_("Enabled:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,0,1);
    wdg_enable = new QCheckBox(tab_w);
    connect(wdg_enable, SIGNAL(stateChanged(int)), this, SLOT(isModify()));
    glay->addWidget(wdg_enable,0,2,1,2);    
    lab = new QLabel(_("Parent widget:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,1,1);    
    wdg_parent = new QComboBox(tab_w);
    connect(wdg_parent, SIGNAL(activated(const QString&)), this, SLOT(selectParent(const QString&)));
    glay->addWidget(wdg_parent,1,2,1,2);    
    lab = new QLabel(_("User and group:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,2,1);
    wdg_user = new QComboBox(tab_w);
    connect(wdg_user, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectUser(const QString&)));
    glay->addWidget(wdg_user,2,2);    
    wdg_grp = new QComboBox(tab_w);
    connect(wdg_grp, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectPermission()));
    glay->addWidget(wdg_grp,2,3);
    grp->setLayout(glay);
    dlg_lay->addWidget(grp,0,0);
    
    //-- Access parameters --
    grp = new QGroupBox(_("Access"),tab_w);
    QStringList perm_ls;
    perm_ls << _("No access") << _("Use(open)") << _("Modify") << _("Full");        
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);        
    lab = new QLabel(_("User:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,0,0);
    wdg_accuser = new QComboBox(tab_w);
    wdg_accuser->addItems(perm_ls);    
    connect(wdg_accuser, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wdg_accuser,0,1);    
    lab = new QLabel(_("Group:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,1,0);
    wdg_accgrp  = new QComboBox(tab_w);
    wdg_accgrp->addItems(perm_ls);    
    connect(wdg_accgrp, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wdg_accgrp,1,1);    
    lab = new QLabel(_("Other:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,2,0);
    wdg_accother= new QComboBox(tab_w);
    wdg_accother->addItems(perm_ls);    
    connect(wdg_accother, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wdg_accother,2,1);
    grp->setLayout(glay);    
    dlg_lay->addWidget(grp,0,1);
    
    //-- Config parameters --
    grp = new QGroupBox(_("Configuration"),tab_w);    
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);        
    glay->addWidget(new QLabel(_("Id:"),tab_w),0,0);
    wdg_id = new QLabel(tab_w);
    wdg_id->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(wdg_id,0,1);
    glay->addWidget(new QLabel(_("Name:"),tab_w),1,0);
    wdg_name = new QLineEdit(tab_w);
    connect(wdg_name, SIGNAL(textChanged(const QString&)), this, SLOT(isModify()));
    glay->addWidget(wdg_name,1,1);    
    glay->addWidget(new QLabel(_("Description:"),tab_w),2,0);
    wdg_descr = new QTextEdit(tab_w);
    connect(wdg_descr, SIGNAL(textChanged()), this, SLOT(isModify()));
    glay->addWidget(wdg_descr,3,0,1,2);
    grp->setLayout(glay);
    dlg_lay->addWidget(grp,1,0,1,2);

    //- Add tab 'Attributes' -
    //------------------------
    wdg_tabs->addTab(new QWidget,_("Attributes and links"));
    tab_w = wdg_tabs->widget(1);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);

    //-- Add attributes view widget --
    wdg_attr = new InspAttr(tab_w);
    connect(wdg_attr, SIGNAL(modified(const string&)), this, SIGNAL(apply(const string&)));
    dlg_lay->addWidget(wdg_attr,0,0);

    //- Add tab 'Attribute cofiguration' -
    //------------------------
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
    wdg_attr_cfg = new QTreeWidget(attr_cf_fr);
    wdg_attr_cfg->setItemDelegate(new ItemDelegate);
    wdg_attr_cfg->setSelectionBehavior(QAbstractItemView::SelectRows);
    wdg_attr_cfg->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(wdg_attr_cfg, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(changeAttr(QTreeWidgetItem*,int)));
    QStringList headLabels;
    headLabels << _("Id") << _("Name") << _("Data type") << _("Work area") << _("Proc") << "*";
    wdg_attr_cfg->setHeaderLabels(headLabels);
    glay->addWidget(wdg_attr_cfg,0,0,1,2);
    
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
    
    glay->addWidget(new QLabel(_("Procedure language:"),wdg_proc_fr),1,0);
    proc_lang = new QLineEdit(wdg_proc_fr);
    connect(proc_lang, SIGNAL(textChanged(const QString&)), this, SLOT(isModify()));
    glay->addWidget(proc_lang,1,1);
    proc_text = new QTextEdit(wdg_proc_fr);
    connect(proc_text, SIGNAL(textChanged()), this, SLOT(isModify()));
    glay->addWidget(proc_text,2,0,1,2);    

    //- Add button box -
    //------------------
    butbox = new QDialogButtonBox(QDialogButtonBox::Apply|
				  QDialogButtonBox::Cancel|
				  QDialogButtonBox::Close, Qt::Horizontal,this);
    //-- Init Apply button --
    butbox->button(QDialogButtonBox::Apply)->setText(_("Apply"));
    if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t.load(":/images/button_ok.png");
    butbox->button(QDialogButtonBox::Apply)->setIcon(QPixmap::fromImage(ico_t));
    butbox->button(QDialogButtonBox::Apply)->setEnabled(false);
    connect(butbox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(pressApply()));    
    //-- Init Cancel button --
    butbox->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t.load(":/images/button_cancel.png");
    butbox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    butbox->button(QDialogButtonBox::Cancel)->setShortcut(QKeySequence("Esc"));
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    connect(butbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(pressCancel()));
    //-- Init close button --
    connect(butbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(pressClose()));

    tab_lay->addWidget(butbox);

    //- End resize -
    //--------------
    resize(500,400);    
}

WdgProp::~WdgProp( )
{

}

VisDevelop *WdgProp::owner()
{
    return (VISION::VisDevelop*)parentWidget();
}

string WdgProp::user()
{
    return wdg_user->currentText().toAscii().data();
}

string WdgProp::grp()
{
    if(wdg_grp->currentText().isEmpty())
	return "UI";
    return wdg_grp->currentText().toAscii().data();
}		

short WdgProp::permit()
{
    string accs;
    short permit = 0, wperm;

    for(int i_p = 0; i_p < 3; i_p++)
    {
	switch(i_p)
	{
	    case 0: accs = wdg_accother->currentText().toAscii().data();break;
	    case 1: accs = wdg_accgrp->currentText().toAscii().data();	break;		
	    case 2: accs = wdg_accuser->currentText().toAscii().data();	break;
	}
	wperm = 0;
	if( accs == _("Use(open)") )	wperm = 4;
	else if( accs == _("Modify") )	wperm = 2;
	else if( accs == _("Full") )	wperm = 6;
	    
	permit|=(wperm<<(i_p*3));
    }
    
    return permit;
}

void WdgProp::setPermit( short vl )
{
    short wperm = vl;
    string accs;
    
    for(int i_p = 0; i_p < 3; i_p++)
    {
        switch(wperm&0x7)
        {
    	    case 0: accs = _("No access");	break;
	    case 4: accs = _("Use(open)");	break;
	    case 2: accs = _("Modify");	break;
	    case 6: accs = _("Full");	break;
	}
	switch(i_p)
	{
	    case 0: wdg_accother->setCurrentIndex(wdg_accother->findText(accs.c_str()));break;
	    case 1: wdg_accgrp->setCurrentIndex(wdg_accother->findText(accs.c_str()));	break;		
	    case 2: wdg_accuser->setCurrentIndex(wdg_accother->findText(accs.c_str()));	break;
	}
	wperm>>=3;
    }
}
 
void WdgProp::showDlg( const string &ilb, bool reload )
{
    vector<string> ls;
    QImage ico_t;
    AutoHD<VCA::Widget> wdg;
    ed_lib = ilb;

    //- Load widget data -
    try
    {
	string wlib_id = TSYS::strSepParse(ed_lib,0,'.');
	string wdg_id = TSYS::strSepParse(ed_lib,1,'.');
	string wdgc_id = TSYS::strSepParse(ed_lib,2,'.');
	
	if(wdgc_id.size())
	    wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(wdgc_id);
	else wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id);
	lib_wdg = !wdgc_id.size();
    }
    catch(TError err)
    { 
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Widget '%1' no present.")).arg(ed_lib.c_str()), TVision::Warning, this );
	return;
    }

    show_init = true;

    //-- Load widget icon --
    string simg = TSYS::strDecode(wdg.at().ico(),TSYS::base64);
    if(ico_t.loadFromData((const uchar*)simg.c_str(),simg.size()))
	wdg_ico->setIcon(QPixmap::fromImage(ico_t));
    else wdg_ico->setIcon(QIcon());
    //-- Load widget state --
    wdg_enable->setChecked(wdg.at().enable());
    //-- Load parent widget --
    selectParent(wdg.at().parentNm().c_str());
    //-- Load users and groups --
    SYS->security().at().usrList(ls);
    //--- Delete users ---
    wdg_user->clear();
    for(int i_l = 0; i_l < ls.size(); i_l++)
    {
	string simg = TSYS::strDecode(SYS->security().at().usrAt(ls[i_l]).at().picture(),TSYS::base64);
        QImage img;
        if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
            wdg_user->addItem(QPixmap::fromImage(img),ls[i_l].c_str());
        else wdg_user->addItem(ls[i_l].c_str());
	if( wdg.at().user() == ls[i_l] ) wdg_user->setCurrentIndex(i_l);
    }
    wdg_grp->setCurrentIndex(wdg_grp->findText(wdg.at().grp().c_str()));
    setPermit(wdg.at().permit());
    
    //-- Load widget identifier, name and description --    
    wdg_id->setText(wdg.at().id().c_str());
    wdg_name->setText(wdg.at().name().c_str());
    wdg_descr->setPlainText(wdg.at().descr().c_str());

    if( lib_wdg )
    {
	wdg_tabs->setTabEnabled(2,true);
	//- Load configurable attributes -
	vector<string> cw_ls;
	wdg.at().wdgList(cw_ls);
	QTableWidgetItem *w_it;
	int gen_flags = Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable;
	int noed_flags = Qt::ItemIsEnabled|Qt::ItemIsEditable;
    
	wdg_attr_cfg->clear();
	for( int i_cw = -1, i_rw = 0; i_cw < (int)cw_ls.size(); i_cw++ )
	{
	    QTreeWidgetItem *root_it;
	    if( i_cw < 0 )	wdg.at().attrList(ls);
	    else
	    {
		wdg.at().wdgAt(cw_ls[i_cw]).at().attrList(ls);
		root_it = new QTreeWidgetItem(0);
		root_it->setText(0,cw_ls[i_cw].c_str());
		root_it->setData(0,Qt::UserRole,0);
		wdg_attr_cfg->addTopLevelItem(root_it);
	    }

	    for( int i_l = 0; i_l < ls.size(); i_l++, i_rw++ )
	    {
		QTreeWidgetItem *cur_it;
		AutoHD<VCA::Attr> w_atr;
		if( i_cw < 0 ) 
		{
		    w_atr = wdg.at().attrAt(ls[i_l]);
		    cur_it = new QTreeWidgetItem(0);
		    wdg_attr_cfg->addTopLevelItem(cur_it);
		}
		else
		{
		    w_atr = wdg.at().wdgAt(cw_ls[i_cw]).at().attrAt(ls[i_l]);
		    cur_it = new QTreeWidgetItem(root_it);
		}
		cur_it->setFlags((Qt::ItemFlags)gen_flags);
 		//string wid = ((i_cw<0)?string(""):cw_ls[i_cw]+"_")+w_atr.at().id();            
		cur_it->setText(0,w_atr.at().id().c_str());
		cur_it->setData(0,Qt::UserRole,w_atr.at().flgGlob());
		cur_it->setText(1,w_atr.at().name().c_str());
 		cur_it->setData(2,Qt::DisplayRole,
		    w_atr.at().type()+((w_atr.at().flgGlob()&(TFld::Selected|VCA::Attr::Color|VCA::Attr::Image|VCA::Attr::Font|VCA::Attr::Address))<<4) );
		cur_it->setText(3,(w_atr.at().fld().values()+"|"+w_atr.at().fld().selNames()).c_str());
 		cur_it->setData(4,Qt::DisplayRole,(bool)(w_atr.at().flgSelf()&VCA::Attr::AttrCalc));
 		cur_it->setData(5,Qt::DisplayRole,false);
	    }
	}
	proc_lang->setText(wdg.at().calcLang().c_str());
	proc_text->setText(wdg.at().calcProg().c_str());
    }
    else wdg_tabs->setTabEnabled(2,false);

    //- Disable OK and Cancel buttons -
    is_modif = false;
    butbox->button(QDialogButtonBox::Apply)->setEnabled(false);
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(false);

    //- Set attributes -
    wdg_attr->setWdg(ilb);

    //- Show dialog -
    show();
    raise();
    activateWindow();
    if( !reload ) wdg_tabs->setCurrentIndex(0);
    
    show_init = false;
}

void WdgProp::selectIco( )
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
    
    wdg_ico->setIcon(QPixmap::fromImage(ico_t));
    
    isModify();
}

void WdgProp::selectParent( const QString &val )
{
    wdg_parent->clear();
    
    int c_lv = 0;
    string c_path = "";
    string lnk = val.toAscii().data();

    wdg_parent->addItem(c_path.c_str());
    while(TSYS::strSepParse(lnk,c_lv,'.').size())
    {
        if( c_lv ) c_path+=".";
        c_path = c_path+TSYS::strSepParse(lnk,c_lv,'.');
        wdg_parent->addItem(c_path.c_str());
        c_lv++;
    }
    if(c_lv) c_path+=".";
    vector<string>  ls;
    switch(c_lv)
    {
        case 0: mod->engine().at().wlbList(ls);   break;
        case 1:
            if( mod->engine().at().wlbPresent(TSYS::strSepParse(lnk,0,'.')) )
                mod->engine().at().wlbAt(TSYS::strSepParse(lnk,0,'.')).at().list(ls);
            break;
    }
    for(int i_l = 0; i_l < ls.size(); i_l++)
        wdg_parent->addItem((c_path+ls[i_l]).c_str());

    int cur_id = wdg_parent->findText(val);
    if( cur_id >= 0 )	wdg_parent->setCurrentIndex( cur_id );
	
    isModify( );
}

void WdgProp::isModify( )
{
    is_modif = true;
    butbox->button(QDialogButtonBox::Apply)->setEnabled(true);
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(true);    
}

void WdgProp::pressApply( )
{    
    AutoHD<VCA::Widget> wdg;
    //- Download modified data -
    try
    { 
	//-- Open widget --		    
       	string wlib_id = TSYS::strSepParse(ed_lib,0,'.');
	string wdg_id = TSYS::strSepParse(ed_lib,1,'.');
	string wdgc_id = TSYS::strSepParse(ed_lib,2,'.');

	if(wdgc_id.size())
	    wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(wdgc_id);
	else wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id);

	//-- Save widget icon --
	if(!wdg_ico->icon().isNull())
	{
	    QByteArray ba;
    	    QBuffer buffer(&ba);
	    buffer.open(QIODevice::WriteOnly);
	    wdg_ico->icon().pixmap(64,64).save(&buffer,"PNG");
	    wdg.at().setIco(TSYS::strEncode(string(ba.data(),ba.size()),TSYS::base64));
        }
	//-- Save parent widget --
	wdg.at().setParentNm(wdg_parent->currentText().toAscii().data());
	//-- Save users and groups --
	wdg.at().setUser(user());
	wdg.at().setGrp(grp());
	//-- Set permition --
	wdg.at().setPermit(permit());
	//-- Save widget name and description --
	wdg.at().setName(wdg_name->text().toAscii().data());
	wdg.at().setDescr(wdg_descr->toPlainText().toAscii().data());
	//-- Save widget state --
	wdg.at().setEnable(wdg_enable->isChecked());
	
	if( lib_wdg )
	{
	    //-- Save attributes and process changes --
	    vector<string> lst;
	    wdg.at().attrList(lst);
	    //--- Check for delete attribute ---
	    for( int i_l = 0; i_l < lst.size(); i_l++ )
	    {
		int i_a;
		for( i_a = 0; i_a < wdg_attr_cfg->topLevelItemCount(); i_a++ )
		    if( !wdg_attr_cfg->topLevelItem(i_a)->childCount() && 
			    wdg_attr_cfg->topLevelItem(i_a)->text(0) == lst[i_l].c_str() )
			break;
		if( i_a >= wdg_attr_cfg->topLevelItemCount() )
		    wdg.at().attrDel(lst[i_l]);
	    }
	    //--- Update attributes data ---
	    for( int i_a = 0; i_a < wdg_attr_cfg->topLevelItemCount(); i_a++ )
	    {
		if( wdg_attr_cfg->topLevelItem(i_a)->childCount() || 
			!wdg_attr_cfg->topLevelItem(i_a)->data(5,Qt::DisplayRole).toBool() )
		    continue;
		string     aid = wdg_attr_cfg->topLevelItem(i_a)->text(0).toAscii().data();
		TFld::Type tp  = (TFld::Type)(wdg_attr_cfg->topLevelItem(i_a)->data(2,Qt::DisplayRole).toInt()&0x0f);
		unsigned   flg = (wdg_attr_cfg->topLevelItem(i_a)->data(2,Qt::DisplayRole).toInt()>>4)|VCA::Attr::IsUser;
		string     vals= wdg_attr_cfg->topLevelItem(i_a)->text(3).toAscii().data();
		VCA::Attr::SelfAttrFlgs stflg = wdg_attr_cfg->topLevelItem(i_a)->
		    				data(4,Qt::DisplayRole).toBool()?VCA::Attr::AttrCalc:(VCA::Attr::SelfAttrFlgs)0;
		//---- Check to present attribute ----
		int i_l;
		for( i_l = 0; i_l < lst.size(); i_l++ )
		    if( aid == lst[i_l] )	break;
		if( i_l >= lst.size() || tp != wdg.at().attrAt(aid).at().type() || 
				     (wdg.at().attrAt(aid).at().flgGlob()^flg)&TFld::Selected )
		{
		    if( i_l < lst.size() )	wdg.at().attrDel(lst[i_l]);
		    wdg.at().attrAdd( new TFld(aid.c_str(),"",tp,flg) );
		}
		wdg.at().attrAt(aid).at().fld().setDescr(wdg_attr_cfg->topLevelItem(i_a)->text(1).toAscii().data());
		wdg.at().attrAt(aid).at().fld().setValues(TSYS::strSepParse(vals,0,'|'));
		wdg.at().attrAt(aid).at().fld().setSelNames(TSYS::strSepParse(vals,1,'|'));
		VCA::Attr::SelfAttrFlgs sflg = wdg.at().attrAt(aid).at().flgSelf();
		wdg.at().attrAt(aid).at().flgSelf((VCA::Attr::SelfAttrFlgs)(sflg^((sflg^stflg)&VCA::Attr::AttrCalc)));
		unsigned tflg = wdg.at().attrAt(aid).at().fld().flg();
		wdg.at().attrAt(aid).at().fld().setFlg(tflg^((tflg^flg)&(VCA::Attr::Color|VCA::Attr::Image|VCA::Attr::Font|VCA::Attr::Address)));
	    }
	    //--- Save process language and text ---
	    wdg.at().setCalcLang(proc_lang->text().toAscii().data());
	    wdg.at().setCalcProg(proc_text->toPlainText().toAscii().data());
	}
    }
    catch(TError err)
    { 
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Widget '%1' error: %2")).arg(ed_lib.c_str()).arg(err.mess.c_str()),
		TVision::Warning,this);
	return;
    }   
     
    //- Update widgets tree and toolbars -
    emit apply(ed_lib);
    
    //- Reload date -
    showDlg(ed_lib,true);
}

void WdgProp::pressCancel( )
{    
    showDlg(ed_lib,true);
}

void WdgProp::pressClose( )
{
    if( is_modif )
    {
	InputDlg dlg(this,wdg_ico->icon(),
             _("Some attributes is changed. You sure to close window?"),_("Close window"),false,false);
	if( dlg.exec() != QDialog::Accepted )	return;
    }			    

    is_modif = false;
    ed_lib = "";
    hide();
}

void WdgProp::selectUser(const QString &val)
{
    //Get default users group
    vector<string> gls;
    SYS->security().at().usrGrpList(val.toAscii().data(),gls);    
    string vgrp = gls.size()?gls[0].c_str():"UI";
    wdg_grp->clear();
    for(int i_l = 0; i_l < gls.size(); i_l++)
    {
	wdg_grp->addItem(gls[i_l].c_str());
        if( vgrp == gls[i_l] ) wdg_grp->setCurrentIndex(i_l);
    }
    selectPermission();    
}

void WdgProp::selectPermission( )
{
    wdg_enable->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWRWR_));
    wdg_parent->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit()));
    wdg_user->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,"root","root",RWRWR_));
    wdg_grp->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wdg_accuser->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wdg_accgrp->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wdg_accother->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wdg_name->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit()));
    wdg_descr->setEnabled( SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit()));
    ico_modif = SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit());
    
    isModify( );
}

void WdgProp::addAttr( )
{
    //- Search last root attribute -
    int last_root;
    for( last_root = 0; last_root < wdg_attr_cfg->topLevelItemCount(); last_root++ )
	if( wdg_attr_cfg->topLevelItem(last_root)->childCount() )  
	    break;
    //- Insert new root attribute -
    QTreeWidgetItem *cur_it = new QTreeWidgetItem(0);
    cur_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
    cur_it->setText(0,_("NewAttr"));
    cur_it->setData(0,Qt::UserRole,VCA::Attr::IsUser);
    cur_it->setText(1,_("New attribute"));
    cur_it->setData(2,Qt::DisplayRole,TFld::String);
    cur_it->setText(3,"|");
    cur_it->setData(4,Qt::DisplayRole,false);
    cur_it->setData(5,Qt::DisplayRole,true);
    wdg_attr_cfg->insertTopLevelItem(last_root,cur_it);
    
    isModify( );
}

void WdgProp::delAttr( )
{
    //- Check curent attribute -
    if( !wdg_attr_cfg->currentItem() || 
	!(wdg_attr_cfg->currentItem()->data(0,Qt::UserRole).toInt()&VCA::Attr::IsUser) ||
	wdg_attr_cfg->currentItem()->data(0,Qt::UserRole).toInt()&VCA::Attr::IsInher )
    {
	mod->postMess( mod->nodePath().c_str(), 
		_("No select attribute or attribute deleting no permited"),
		TVision::Warning,this);
	return;
    }    
    //- Delete curent attribute -
    //-- Request to confirm --
    InputDlg dlg(this,this->windowIcon(),
	QString(_("You sure for delete this attribute '%1'.")).arg(wdg_attr_cfg->currentItem()->text(0)),
		_("Delete attribute"),false,false);
    if( dlg.exec() == QDialog::Accepted )
    {
	delete wdg_attr_cfg->currentItem();
	
	isModify( );
    }
}	

void WdgProp::changeAttr(QTreeWidgetItem *it, int col)
{
    //- User attribute data change check -
    if( show_init || !it )	return;
    it->setData(5,Qt::DisplayRole,true);
    
    isModify( );
}

//* Attributes item delegate    *
WdgProp::ItemDelegate::ItemDelegate( QObject *parent ) : QItemDelegate(parent)
{

}

void WdgProp::ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if( index.isValid() && !index.model()->rowCount(index) )
    {
	if( index.column() == 2 )
	{
	    QString val;
	    switch(index.data(Qt::DisplayRole).toInt())
	    {
		case TFld::Boolean:	val = _("Boolean");	break;
		case TFld::Integer:	val = _("Integer");	break;
		case TFld::Real:	val = _("Real");	break;
		case TFld::String:	val = _("String");	break;
		case TFld::Integer+(TFld::Selected<<4):	val = _("Select integer");	break;
		case TFld::Real+(TFld::Selected<<4):	val = _("Select real");		break;
		case TFld::String+(TFld::Selected<<4):	val = _("Select string");	break;
		case TFld::String+(VCA::Attr::Color<<4):	val = _("Color");	break;
		case TFld::String+(VCA::Attr::Image<<4):	val = _("Image");	break;
		case TFld::String+(VCA::Attr::Font<<4):		val = _("Font");	break;
		case TFld::String+(VCA::Attr::Address<<4):	val = _("Address");	break;
	    }	
	
	    drawDisplay(painter, option, option.rect,val);
	    return;
	}
	if( index.column() == 4 || index.column() == 5 )
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

QWidget *WdgProp::ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *w_del = NULL;
    if(!index.isValid()) return 0;

    int flg = index.model()->index(index.row(),0,index.parent()).data(Qt::UserRole).toInt();
    QVariant value = index.data(Qt::EditRole);
    
    if( ((flg&VCA::Attr::IsUser && !(flg&VCA::Attr::IsInher)) || index.column() == 4) && index.column() != 5 )
    { 
	if( index.column() == 2 )
	    w_del = new QComboBox(parent);
	else 
	{
    	    QItemEditorFactory factory;
    	    w_del = factory.createEditor(value.type(),parent);
	}
    }
    
    return w_del;
}

void WdgProp::ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if( index.column() == 2 )
    {
	QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	comb->addItem(_("Boolean"),TFld::Boolean);
	comb->addItem(_("Integer"),TFld::Integer);
	comb->addItem(_("Real"),TFld::Real);
	comb->addItem(_("String"),TFld::String);
	comb->addItem(_("Select integer"),TFld::Integer+(TFld::Selected<<4));
	comb->addItem(_("Select real"),TFld::Real+(TFld::Selected<<4));
	comb->addItem(_("Select string"),TFld::String+(TFld::Selected<<4));
	comb->addItem(_("Color"),TFld::String+(VCA::Attr::Color<<4));
	comb->addItem(_("Image"),TFld::String+(VCA::Attr::Image<<4));
	comb->addItem(_("Font"),TFld::String+(VCA::Attr::Font<<4));
	comb->addItem(_("Address"),TFld::String+(VCA::Attr::Address<<4));
	comb->setCurrentIndex(comb->findData(index.data(Qt::DisplayRole).toInt()));
    }
    else QItemDelegate::setEditorData(editor, index);
}

void WdgProp::ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if( index.column() == 2 )
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
