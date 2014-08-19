
//OpenSCADA system module UI.Vision file: vis_devel_dlgs.cpp
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <QBuffer>
#include <QTableWidget>
#include <QPainter>
#include <QSplitter>
#include <QScrollArea>
#include <QMessageBox>

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

    //Create tabulator
    QVBoxLayout *tab_lay = new QVBoxLayout(this);
    tab_lay->setMargin(5);
    wdg_tabs = new QTabWidget(this);
    tab_lay->addWidget(wdg_tabs);
    connect(wdg_tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

    //Add tab 'Widget'
    //------------------
    QScrollArea *scrl = new QScrollArea();
    wdg_tabs->addTab(scrl,_("Widgets library"));
    QWidget *tab_w = new QWidget(scrl);
    scrl->setWidget(tab_w);
    scrl->setWidgetResizable(true);
    //QWidget *tab_w = wdg_tabs->widget(0);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);

    //State parameters
    grp = new QGroupBox(_("State"),tab_w);
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);

    obj_ico = new QPushButton(tab_w);
    obj_ico->setObjectName("/obj/cfg/ico");
    obj_ico->setToolTip(_("Item's icon. Click for download other."));
    obj_ico->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
    obj_ico->setIconSize(QSize(60,60));
    obj_ico->setAutoDefault(false);
    connect(obj_ico, SIGNAL(released()), this, SLOT(selectIco()));
    glay->addWidget(obj_ico, 0, 0, 3, 1);

    lab = new QLabel(_("Enabled:"), tab_w);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred));
    glay->addWidget(lab, 0, 1);
    obj_enable = new QCheckBox(tab_w);
    obj_enable->setObjectName("/obj/st/en");
    connect(obj_enable, SIGNAL(stateChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_enable, 0, 2);

    lab = new QLabel(_("Container DB:"), tab_w);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred));
    glay->addWidget(lab, 1, 1);
    obj_db = new LineEdit(tab_w, LineEdit::Combo);
    obj_db->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
    obj_db->setObjectName("/obj/st/db");
    connect(obj_db, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(obj_db, 1, 2);

    lab = new QLabel(_("Date of modification:"), tab_w);
    glay->addWidget(lab, 2, 1);
    obj_tmstmp = new QLabel(tab_w);
    obj_tmstmp->setTextInteractionFlags(Qt::TextSelectableByMouse);
    obj_tmstmp->setObjectName("/obj/st/timestamp");
    obj_tmstmp->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(obj_tmstmp, 2, 2);

    grp->setLayout(glay);
    dlg_lay->addWidget(grp, 0, 0);

    //Configuration parameters
    grp = new QGroupBox(_("Configuration"), tab_w);
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);

    glay->addWidget(new QLabel(_("Id:"),tab_w), 0, 0);
    obj_id = new QLabel(tab_w);
    obj_id->setTextInteractionFlags(Qt::TextSelectableByMouse);
    obj_id->setObjectName("/obj/cfg/id");
    obj_id->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(obj_id, 0, 1, 1, 3);

    glay->addWidget(new QLabel(_("Name:"),tab_w), 1, 0);
    obj_name = new LineEdit(this);
    obj_name->setObjectName("/obj/cfg/name");
    connect(obj_name, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(obj_name, 1, 1, 1, 3);

    glay->addWidget(new QLabel(_("Description:"),tab_w), 2, 0);
    obj_descr = new TextEdit(this);
    obj_descr->setObjectName("/obj/cfg/descr");
    connect(obj_descr, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(obj_descr, 3, 0, 1, 4);

    lab = new QLabel(_("Owner, group:"), tab_w);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred));
    glay->addWidget(lab, 4, 0);
    obj_user = new QComboBox(tab_w);
    obj_user->setObjectName("/obj/cfg/owner");
    connect(obj_user, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_user, 4, 1);
    obj_grp = new QComboBox(tab_w);
    obj_grp->setObjectName("/obj/cfg/grp");
    connect(obj_grp, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_grp,4,2);
    lab = new QLabel(_("Access:"), tab_w);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred));
    glay->addWidget(lab, 5, 0);
    obj_accuser = new QComboBox(tab_w);
    obj_accuser->setObjectName("/obj/cfg/u_a");
    connect(obj_accuser, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accuser, 5, 1);
    obj_accgrp  = new QComboBox(tab_w);
    obj_accgrp->setObjectName("/obj/cfg/g_a");
    connect(obj_accgrp, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accgrp, 5, 2);
    obj_accother= new QComboBox(tab_w);
    obj_accother->setObjectName("/obj/cfg/o_a");
    connect(obj_accother, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accother, 5, 3);

    // Specific parameter: project calc time
    lab = new QLabel(_("Calculate period (ms):"), tab_w);
    glay->addWidget(lab, 6, 0);
    prj_ctm = new LineEdit(tab_w, LineEdit::Integer, false, false);
    prj_ctm->setCfg("0:10000:10");
    prj_ctm->setObjectName("/obj/cfg/per");
    prj_ctm->setWindowIconText(TSYS::addr2str(lab).c_str());
    connect(prj_ctm, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(prj_ctm,6,1,1,3);
    lab = new QLabel(_("Run window mode:"),tab_w);
    glay->addWidget(lab,7,0);
    prj_runw = new QComboBox( tab_w);
    prj_runw->setObjectName("/obj/cfg/runWin");
    prj_runw->setWindowIconText(TSYS::addr2str(lab).c_str());
    connect(prj_runw, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(prj_runw,7,1,1,3);
    lab = new QLabel(_("Keep aspect ratio on scale:"),tab_w);
    glay->addWidget(lab,8,0);
    prj_keepAspRt = new QCheckBox(tab_w);
    prj_keepAspRt->setObjectName("/obj/cfg/keepAspRatio");
    prj_keepAspRt->setWindowIconText(TSYS::addr2str(lab).c_str());
    connect(prj_keepAspRt, SIGNAL(stateChanged(int)), this, SLOT(isModify()));
    glay->addWidget(prj_keepAspRt,8,1,1,3);

    grp->setLayout(glay);
    dlg_lay->addWidget(grp,1,0);

    //Add tab 'Mime data'
    //---------------------
    wdg_tabs->addTab(new QWidget,_("Mime data"));
    tab_w = wdg_tabs->widget(1);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);

    mimeDataTable = new QTableWidget(0, 3, tab_w);
    mimeDataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(mimeDataTable, SIGNAL(cellChanged(int,int)), this, SLOT(mimeDataChange(int,int)));
    mimeDataTable->setHorizontalHeaderLabels(QStringList() << _("Id") << _("Mime type") << _("Data size"));
    dlg_lay->addWidget(mimeDataTable, 0, 0, 1, 4);

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

    //Add tab 'Styles'
    //------------------
    wdg_tabs->addTab(new QWidget,_("Styles"));
    tab_w = wdg_tabs->widget(2);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);

    lab = new QLabel(_("Style:"),tab_w);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    dlg_lay->addWidget(lab, 0, 0);
    stl_select = new QComboBox(tab_w);
    stl_select->setObjectName("/style/style");
    connect(stl_select, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    dlg_lay->addWidget(stl_select, 0, 1, 1, 2);

    buttStlDel = new QPushButton(_("Erase style"),tab_w);
    buttStlDel->setObjectName("/style/erase");
    connect(buttStlDel, SIGNAL(clicked()), this, SLOT(isModify()));
    dlg_lay->addWidget(buttStlDel,0,3);

    dlg_lay->addWidget(new QLabel(_("Name:"),tab_w), 1, 0);
    stl_name = new LineEdit(tab_w);
    stl_name->setObjectName("/style/name");
    connect(stl_name, SIGNAL(apply()), this, SLOT(isModify()));
    dlg_lay->addWidget(stl_name, 1, 1, 1, 2);

    stl_table = new QTableWidget(0, 2, tab_w);
    stl_table->setObjectName("/style/props");
    stl_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(stl_table, SIGNAL(cellChanged(int,int)), this, SLOT(stlTableChange(int,int)));
    stl_table->setHorizontalHeaderLabels(QStringList() << _("Id") << _("Value"));
    dlg_lay->addWidget(stl_table, 4, 0, 1, 4);

    buttStlTableDel = new QPushButton(_("Delete record"),tab_w);
    connect(buttStlTableDel, SIGNAL(clicked()), this, SLOT(delStlItem()));
    dlg_lay->addWidget(buttStlTableDel,5,3);

    //Add tab 'Diagnostic'
    //------------------
    wdg_tabs->addTab((tab_w=new QWidget),_("Diagnostics"));
    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);

    lab = new QLabel(_("Time:"), tab_w);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred));
    dlg_lay->addWidget(lab, 0, 0);
    messTime = new LineEdit(tab_w, LineEdit::DateTime);
    messTime->setCfg("dd-MM-yyyy hh:mm:ss");
    messTime->setObjectName("/mess/tm");
    connect(messTime, SIGNAL(apply()), this, SLOT(isModify()));
    dlg_lay->addWidget(messTime, 0, 1, 1, 1);
    if(!ico_t.load(TUIS::icoGet("reload",NULL,true).c_str())) ico_t.load(":/images/reload.png");
    rldPushBt = new QPushButton(QPixmap::fromImage(ico_t), _("Refresh"), tab_w);
    rldPushBt->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred));
    rldPushBt->setObjectName("/mess/rld");
    connect(rldPushBt, SIGNAL(released()), this, SLOT(isModify()));
    dlg_lay->addWidget(rldPushBt, 0, 2, 1, 1);

    lab = new QLabel(_("Size:"), tab_w);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred));
    dlg_lay->addWidget(lab, 1, 0);
    messSize = new LineEdit(tab_w, LineEdit::Integer);
    messSize->setCfg("1:1000:1:: s");
    messSize->setObjectName("/mess/size");
    connect(messSize, SIGNAL(apply()), this, SLOT(isModify()));
    dlg_lay->addWidget(messSize, 1, 1, 1, 2);

    messTable = new QTableWidget(0, 5, tab_w);
    messTable->setObjectName("/mess/mess");
    messTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    messTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    messTable->setMinimumHeight(150);
    messTable->setHorizontalHeaderLabels(QStringList() << _("Time") << _("mcsec") << _("Category") << _("Lev.") << _("Message"));
    dlg_lay->addWidget(messTable, 2, 0, 1, 3);

    //Add button box
    //----------------
    butbox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    // Init close button
    butbox->button(QDialogButtonBox::Close)->setText(_("Close"));
    connect(butbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));

    tab_lay->addWidget(butbox);

    //End resize
    //------------
    resize(800,600);
}

LibProjProp::~LibProjProp( )
{

}

VisDevelop *LibProjProp::owner( )	{ return (VISION::VisDevelop*)parentWidget(); }

void LibProjProp::showDlg( const string &iit, bool reload )
{
    vector<string> ls;
    string sval;
    QImage ico_t;
    ed_it = iit;

    show_init = true;

    //Update elements present, visible and values
    XMLNode req("get");

    XMLNode info_req("info");
    info_req.setAttr("path",ed_it);

    if(owner()->cntrIfCmd(info_req)) {
	mod->postMess(mod->nodePath().c_str(), QString(_("Get node '%1' information error.")).arg(ed_it.c_str()),TVision::Error, this);
	return;
    }
    XMLNode *root = info_req.childGet(0),
	    *gnd;

    setWindowTitle(root->attr("dscr").c_str());

    //Generic dialog's page
    gnd = TCntrNode::ctrId(root, "/obj", true);
    wdg_tabs->setTabEnabled(0,gnd);
    if(gnd) {
	wdg_tabs->setTabText(0,gnd->attr("dscr").c_str());
	// Enable stat
	gnd = TCntrNode::ctrId(root,obj_enable->objectName().toStdString(),true);
	obj_enable->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_enable->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_enable->setChecked(s2i(req.text()));
	}
	// DB value
	gnd = TCntrNode::ctrId(root,obj_db->objectName().toStdString(),true);
	obj_db->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_db->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_db->setValue(req.text().c_str());
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/db/tblList",TSYS::PathEl)+":"+TSYS::pathLev(ed_it,0));
	    if(!owner()->cntrIfCmd(req)) {
		string els;
		for(unsigned i_l = 0; i_l < req.childSize(); i_l++)
		    els += req.childGet(i_l)->text() + "\n";
		obj_db->setCfg(els.c_str());
	    }
	}
	// TimeStamp
	gnd = TCntrNode::ctrId(root, obj_tmstmp->objectName().toStdString(), true);
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_tmstmp->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_tmstmp->setText(tm2s(s2i(req.text()),"").c_str());
	    else obj_tmstmp->setText(_("No set"));
	}else obj_tmstmp->setText("");
	// User
	gnd = TCntrNode::ctrId(root,obj_user->objectName().toStdString(),true);
	obj_user->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_user->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) sval = req.text();

	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/obj/u_lst",TSYS::PathEl));
	    obj_user->clear();
	    if(!owner()->cntrIfCmd(req))
		for(unsigned i_l = 0; i_l < req.childSize(); i_l++) {
		    obj_user->addItem(req.childGet(i_l)->text().c_str());
		    if(sval == req.childGet(i_l)->text()) obj_user->setCurrentIndex(i_l);
		}
	}
	// Group
	gnd = TCntrNode::ctrId(root,obj_grp->objectName().toStdString(),true);
	obj_grp->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_grp->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) sval = req.text();

	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/obj/g_lst",TSYS::PathEl));
	    obj_grp->clear();
	    if(!owner()->cntrIfCmd(req))
		for(unsigned i_l = 0; i_l < req.childSize(); i_l++) {
		    obj_grp->addItem(req.childGet(i_l)->text().c_str());
		    if(sval == req.childGet(i_l)->text()) obj_grp->setCurrentIndex(i_l);
		}
	}
	// Icon
	gnd = TCntrNode::ctrId(root,obj_ico->objectName().toStdString(),true);
	ico_modif = gnd && s2i(gnd->attr("acs"))&SEC_WR;
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_ico->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req) && (sval = TSYS::strDecode(req.text(),TSYS::base64)).size() &&
		    ico_t.loadFromData((const uchar*)sval.c_str(),sval.size()))
		obj_ico->setIcon(QPixmap::fromImage(ico_t));
	    else obj_ico->setIcon(QIcon());
	}
	// Permition
	string wstr;
	gnd = TCntrNode::ctrId(root,obj_accuser->objectName().toStdString(),true);
	obj_accuser->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    obj_accuser->clear();
	    for(int i_l = 0, i_i = 0; (wstr=TSYS::strSepParse(gnd->attr("sel_list"),0,';',&i_l)).size(); )
		obj_accuser->addItem(wstr.c_str(),s2i(TSYS::strSepParse(gnd->attr("sel_id"),0,';',&i_i)));
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accuser->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_accuser->setCurrentIndex(obj_accuser->findData(s2i(req.text())));
	}
	gnd = TCntrNode::ctrId(root,obj_accgrp->objectName().toStdString(),true);
	obj_accgrp->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    obj_accgrp->clear();
	    for(int i_l = 0, i_i = 0; (wstr=TSYS::strSepParse(gnd->attr("sel_list"),0,';',&i_l)).size(); )
		obj_accgrp->addItem(wstr.c_str(),s2i(TSYS::strSepParse(gnd->attr("sel_id"),0,';',&i_i)));
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accgrp->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_accgrp->setCurrentIndex(obj_accgrp->findData(s2i(req.text())));
	}
	gnd = TCntrNode::ctrId(root,obj_accother->objectName().toStdString(),true);
	obj_accother->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    obj_accother->clear();
	    for(int i_l = 0, i_i = 0; (wstr=TSYS::strSepParse(gnd->attr("sel_list"),0,';',&i_l)).size(); )
		obj_accother->addItem(wstr.c_str(),s2i(TSYS::strSepParse(gnd->attr("sel_id"),0,';',&i_i)));
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accother->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_accother->setCurrentIndex(obj_accother->findData(s2i(req.text())));
	}
	// Id
	gnd = TCntrNode::ctrId(root,obj_id->objectName().toStdString(),true);
	if(gnd) {
	    req.clear()->setAttr("path", ed_it+"/"+TSYS::strEncode(obj_id->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_id->setText(req.text().c_str());
	}
	// Name
	gnd = TCntrNode::ctrId(root,obj_name->objectName().toStdString(),true);
	obj_name->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.clear()->setAttr("path", ed_it+"/"+TSYS::strEncode(obj_name->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_name->setValue(req.text().c_str());
	}
 	// Description
	gnd = TCntrNode::ctrId(root,obj_descr->objectName().toStdString(),true);
	obj_descr->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.clear()->setAttr("path", ed_it+"/"+TSYS::strEncode(obj_descr->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_descr->setText(req.text().c_str());
	}

	// Special fields
	//  Page type
	gnd = TCntrNode::ctrId(root,prj_ctm->objectName().toStdString(),true);
	prj_ctm->setVisible(gnd); ((QLabel*)TSYS::str2addr(prj_ctm->windowIconText().toStdString()))->setVisible(gnd);
	if(gnd) {
	    prj_ctm->setEnabled(s2i(gnd->attr("acs"))&SEC_WR);
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(prj_ctm->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) prj_ctm->setValue(req.text().c_str());
	}
	//  Run window mode
	gnd = TCntrNode::ctrId(root,prj_runw->objectName().toStdString(),true);
	prj_runw->setVisible(gnd); ((QLabel*)TSYS::str2addr(prj_runw->windowIconText().toStdString()))->setVisible(gnd);
	if(gnd) {
	    prj_runw->clear();
	    for(int i_l = 0, i_i = 0; (wstr=TSYS::strSepParse(gnd->attr("sel_list"),0,';',&i_l)).size(); )
		prj_runw->addItem(wstr.c_str(), s2i(TSYS::strSepParse(gnd->attr("sel_id"),0,';',&i_i)));
	    req.clear()->setAttr("path", ed_it+"/"+TSYS::strEncode(prj_runw->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) prj_runw->setCurrentIndex(prj_runw->findData(s2i(req.text())));
	}
	//  Run window mode
	gnd = TCntrNode::ctrId(root,prj_keepAspRt->objectName().toStdString(),true);
	prj_keepAspRt->setVisible(gnd); ((QLabel *)TSYS::str2addr(prj_keepAspRt->windowIconText().toStdString()))->setVisible(gnd);
	if(gnd) {
	    req.setAttr("path", ed_it+"/"+TSYS::strEncode(prj_keepAspRt->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) prj_keepAspRt->setChecked(s2i(req.text()));
	}
    }

    //Mime data page
    gnd = TCntrNode::ctrId(root, "/mime", true);
    wdg_tabs->setTabEnabled(1, gnd);
    if(gnd)	wdg_tabs->setTabText(1,gnd->attr("dscr").c_str());

    //Style
    gnd = TCntrNode::ctrId(root,"/style",true);
    wdg_tabs->setTabEnabled(2,gnd);
    if(gnd) {
	wdg_tabs->setTabText(2,gnd->attr("dscr").c_str());

	// Style
	gnd = TCntrNode::ctrId(root,stl_select->objectName().toStdString(),true);
	stl_select->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.clear()->setAttr("path", ed_it+"/"+TSYS::strEncode(stl_select->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) sval = req.text();

	    req.clear()->setAttr("path", ed_it+"/"+TSYS::strEncode(gnd->attr("select"),TSYS::PathEl));
	    stl_select->clear();
	    if(!owner()->cntrIfCmd(req))
		for(unsigned i_l = 0; i_l < req.childSize(); i_l++) {
		    stl_select->addItem(req.childGet(i_l)->text().c_str(),req.childGet(i_l)->attr("id").c_str());
		    if(sval == req.childGet(i_l)->attr("id"))	stl_select->setCurrentIndex(i_l);
		}
	}

	// Style delete button
	gnd = TCntrNode::ctrId(root,buttStlDel->objectName().toStdString(),true);
	buttStlDel->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);

	// Name
	gnd = TCntrNode::ctrId(root,stl_name->objectName().toStdString(),true);
	stl_name->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.clear()->setAttr("path", ed_it+"/"+TSYS::strEncode(stl_name->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) stl_name->setValue(req.text().c_str());
	}

	// Load table data
	gnd = TCntrNode::ctrId(root, stl_table->objectName().toStdString(), true);
	stl_table->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	buttStlTableDel->setEnabled(stl_table->isEnabled());
	if(gnd) {
	    req.clear()->setAttr("path", ed_it+"/"+TSYS::strEncode(stl_table->objectName().toStdString(),TSYS::PathEl));
	    owner()->cntrIfCmd(req);
	    for(unsigned i_c = 0; i_c < req.childSize() && i_c < 2; i_c++) {
		stl_table->setRowCount(req.childGet(i_c)->childSize());
		for(unsigned i_r = 0; i_r < req.childGet(i_c)->childSize(); i_r++) {
		    stl_table->setItem(i_r, i_c, new QTableWidgetItem(req.childGet(i_c)->childGet(i_r)->text().c_str()));
		    stl_table->item(i_r, i_c)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|((i_c==1)?Qt::ItemIsEditable:(Qt::ItemFlags)0));
		}
	    }
	    stl_table->resizeColumnsToContents();
	}
    }

    //Diagnostics
    gnd = TCntrNode::ctrId(root, "/mess", true);
    wdg_tabs->setTabEnabled(3, gnd);
    if(gnd) {
	// Time
	if((gnd=TCntrNode::ctrId(root,messTime->objectName().toStdString(),true))) {
	    req.clear()->setAttr("path", ed_it+"/"+TSYS::strEncode(messTime->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) messTime->setValue(req.text().c_str());
	}
	messTime->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	// Size
	if((gnd=TCntrNode::ctrId(root,messSize->objectName().toStdString(),true))) {
	    req.clear()->setAttr("path", ed_it+"/"+TSYS::strEncode(messSize->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) messSize->setValue(req.text().c_str());
	}
	messSize->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	// Messages
	if((gnd=TCntrNode::ctrId(root,messTable->objectName().toStdString(),true))) {
	    req.clear()->setAttr("path", ed_it+"/"+TSYS::strEncode(messTable->objectName().toStdString(),TSYS::PathEl));
	    owner()->cntrIfCmd(req);
	    for(unsigned i_c = 0; i_c < req.childSize() && i_c < 5; i_c++) {
		messTable->setRowCount(req.childGet(i_c)->childSize());
		for(unsigned i_r = 0; i_r < req.childGet(i_c)->childSize(); i_r++) {
		    string val = req.childGet(i_c)->childGet(i_r)->text();
		    if(i_c == 0) val = tm2s(s2i(val),"");
		    messTable->setItem(i_r, i_c, new QTableWidgetItem(val.c_str()));
		    messTable->item(i_r, i_c)->setFlags(Qt::ItemIsEnabled);
		}
	    }

	    //  Resize to optimal
	    messTable->resizeColumnsToContents();
	    int tblWdth = size().width() * 0.9;
	    int tblHeight = size().height() * 0.8;
	    int averWdth = tblWdth/messTable->columnCount();
	    int fullColsWdth = 0, niceForceColsWdth = 0, busyCols = 0;
	    //   Count width params
	    for(int i_c = 0; i_c < messTable->columnCount(); i_c++) {
		fullColsWdth += messTable->columnWidth(i_c);
		if(messTable->columnWidth(i_c) <= averWdth) niceForceColsWdth += messTable->columnWidth(i_c);
		else busyCols++;
	    }
	    //   Set busyCols
	    if(fullColsWdth > tblWdth && busyCols) {
		int busyColsWdth = (tblWdth-niceForceColsWdth)/busyCols;
		for(int i_c = 0; i_c < messTable->columnCount(); i_c++)
		    if(messTable->columnWidth(i_c) > averWdth && messTable->columnWidth(i_c) > busyColsWdth)
			messTable->setColumnWidth(i_c, busyColsWdth);
	    }

	    messTable->resizeRowsToContents();
	    for(int i_rw = 0; i_rw < messTable->rowCount(); i_rw++)
		messTable->setRowHeight(i_rw, vmin(messTable->rowHeight(i_rw),tblHeight/1.3));
	}
    }

    is_modif = false;

    //Show dialog
    show();
    raise();
    activateWindow();

    if(reload) tabChanged(wdg_tabs->currentIndex());
    else wdg_tabs->setCurrentIndex(0);

    show_init = false;
}

void LibProjProp::tabChanged( int itb )
{
    if(itb == 1) {
	show_init = true;

	//Load mime data
	XMLNode req("get");
	req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl));
	if(!owner()->cntrIfCmd(req)) {
	    XMLNode *id_col = req.childGet(0);
	    mimeDataTable->setRowCount(id_col->childSize());
	    for(unsigned i_l = 0; i_l < id_col->childSize(); i_l++) {
		if(!mimeDataTable->item(i_l,0)) {
		    mimeDataTable->setItem(i_l,0,new QTableWidgetItem());
		    mimeDataTable->item(i_l,0)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
		}
		mimeDataTable->item(i_l,0)->setText(id_col->childGet(i_l)->text().c_str());
		mimeDataTable->item(i_l,0)->setData(Qt::UserRole,id_col->childGet(i_l)->text().c_str());
		if(!mimeDataTable->item(i_l,1)) {
		    mimeDataTable->setItem(i_l,1,new QTableWidgetItem());
		    mimeDataTable->item(i_l,1)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
		}
		mimeDataTable->item(i_l,1)->setText(req.childGet(1)->childGet(i_l)->text().c_str());
		if(!mimeDataTable->item(i_l,2)) {
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

    if(!ico_modif)	return;

    QString fileName = owner()->getFileName(_("Load icon picture"),"",_("Images (*.png *.jpg)"));
    if(fileName.isEmpty())	return;
    if(!ico_t.load(fileName)) {
	mod->postMess(mod->nodePath().c_str(), QString(_("Load icon image '%1' error.")).arg(fileName), TVision::Warning, this);
	return;
    }

    obj_ico->setIcon(QPixmap::fromImage(ico_t));

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    ico_t.save(&buffer, "PNG");

    XMLNode req("set");
    req.setAttr("path", ed_it+"/"+TSYS::strEncode(obj_ico->objectName().toStdString(),TSYS::PathEl))->
	setText(TSYS::strEncode(string(ba.data(),ba.size()),TSYS::base64));
    if(owner()->cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    is_modif = true;
}

void LibProjProp::isModify( QObject *snd )
{
    bool update = false;
    if(show_init) return;
    if(!snd) snd = sender();

    QString oname = snd->objectName();

    XMLNode req("set");

    if(oname == obj_enable->objectName() || oname == prj_keepAspRt->objectName())
    {
	req.setText(i2s(((QCheckBox*)snd)->isChecked()));
	update = true;
    }
    else if(oname == obj_db->objectName() || oname == obj_name->objectName() || oname == prj_ctm->objectName())
	req.setText(((LineEdit*)snd)->value().toStdString());
    else if(oname == stl_name->objectName()) { req.setText(((LineEdit*)snd)->value().toStdString()); update = true; }
    else if(oname == obj_user->objectName() || oname == obj_grp->objectName() || oname == stl_select->objectName())
    {
	int cPos = ((QComboBox*)snd)->currentIndex();
	req.setText(((QComboBox*)snd)->itemData(cPos).isNull() ? ((QComboBox*)snd)->itemText(cPos).toStdString() :
								 ((QComboBox*)snd)->itemData(cPos).toString().toStdString());
	update = true;
    }
    else if(oname == obj_accuser->objectName() || oname == obj_accgrp->objectName() || oname == obj_accother->objectName() ||
	oname == prj_runw->objectName())
    {
	req.setText(((QComboBox*)snd)->itemData(((QComboBox*)snd)->currentIndex()).toString().toStdString());
	update = true;
    }
    else if(oname == obj_descr->objectName()) req.setText(obj_descr->text().toStdString());
    else if(oname == buttStlDel->objectName()) update = true;
    else if(oname == messTime->objectName()) { req.setText(messTime->value().toStdString()); update = true; }
    else if(oname == messSize->objectName()) { req.setText(messSize->value().toStdString()); update = true; }
    else if(oname == rldPushBt->objectName()){ oname = messTime->objectName(); req.setText("0"); update = true; }
    else return;

    req.setAttr("path",ed_it+"/"+TSYS::strEncode(oname.toStdString(),TSYS::PathEl));
    if(owner()->cntrIfCmd(req)) {
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	showDlg(ed_it,true);
    }
    else if(update)	showDlg(ed_it,true);

    is_modif = true;
}

void LibProjProp::closeEvent( QCloseEvent *ce )
{
    //Check for not apply LineEdit and TextEdit widgets
    QList<LineEdit*> lnEdWs = findChildren<LineEdit*>();
    QList<TextEdit*> txtEdWs = findChildren<TextEdit*>();
    bool notApplyPresent = false;
    for(int i_it = 0; !notApplyPresent && i_it < lnEdWs.size(); ++i_it) notApplyPresent = lnEdWs[i_it]->isEdited();
    for(int i_it = 0; !notApplyPresent && i_it < txtEdWs.size(); ++i_it) notApplyPresent = txtEdWs[i_it]->isEdited();
    if(notApplyPresent && QMessageBox::information(this,_("Changes apply"),_("Some changes you don't apply!\nApply now or lost the?"),
	    QMessageBox::Apply|QMessageBox::Cancel,QMessageBox::Apply) == QMessageBox::Apply)
    {
	for(int i_it = 0; i_it < lnEdWs.size(); ++i_it) if(lnEdWs[i_it]->isEdited()) isModify(lnEdWs[i_it]);
	for(int i_it = 0; i_it < txtEdWs.size(); ++i_it) if(txtEdWs[i_it]->isEdited()) isModify(txtEdWs[i_it]);
    }

    if(is_modif) emit apply(ed_it);

    is_modif = false;
    ed_it = "";

    ce->accept();
}

void LibProjProp::showEvent( QShowEvent * event )
{
    QSize src = size();
    adjustSize();
    resize(size().expandedTo(src));
}

void LibProjProp::addMimeData( )
{
    XMLNode req("add");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl));
    if(owner()->cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    tabChanged(1);
}

void LibProjProp::delMimeData( )
{
    int row = mimeDataTable->currentRow();
    if(row < 0) { mod->postMess( mod->nodePath().c_str(),_("No one row is selected."),TVision::Warning,this ); return; }

    XMLNode req("del");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl))->
	setAttr("key_id",mimeDataTable->item(row,0)->text().toStdString());
    if(owner()->cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    tabChanged(1);
}

void LibProjProp::loadMimeData( )
{
    QString fileName = owner()->getFileName(_("Load data"),"",_("All files (*.*)"));
    if(fileName.isEmpty())	return;
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)) {
	mod->postMess(mod->nodePath().c_str(), QString(_("Open file '%1' is fail: %2")).arg(fileName).arg(file.errorString()), TVision::Error, this);
	return;
    }
    if(file.size() >= USER_FILE_LIMIT) {
	mod->postMess(mod->nodePath().c_str(), QString(_("Loadable file '%1' is too large.")).arg(fileName), TVision::Error, this);
	return;
    }
    QByteArray data = file.readAll();

    XMLNode req("set");
    req.setAttr("path", ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl))->
	setAttr("col", "dt")->
	setAttr("key_id", mimeDataTable->selectedItems().empty() ?
	    QFileInfo(fileName).fileName().toStdString() :
	    mimeDataTable->selectedItems()[0]->text().toStdString())->
		setText(TSYS::strEncode(string(data.data(),data.size()),TSYS::base64));
    if(owner()->cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    tabChanged(1);
}

void LibProjProp::unloadMimeData( )
{
    int row = mimeDataTable->currentRow();
    if(row < 0) { mod->postMess( mod->nodePath().c_str(), _("No one row is selected."), TVision::Warning, this ); return; }

    string fext = TSYS::pathLev(mimeDataTable->item(row,1)->text().toStdString(),1);
    QString fileName = owner()->getFileName(_("Save data"), mimeDataTable->item(row,0)->text()+(fext.size()?("."+fext).c_str():""),
	_("All files (*.*)"), QFileDialog::AcceptSave);
    if(fileName.isEmpty())	return;
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
	mod->postMess(mod->nodePath().c_str(),
		QString(_("Open file '%1' is fail: %2")).arg(fileName).arg(file.errorString()), TVision::Error, this);
	return;
    }

    XMLNode req("get");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl))->
	setAttr("data","1")->
	setAttr("col","dt")->
	setAttr("key_id",mimeDataTable->item(row,0)->text().toStdString());
    if(owner()->cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(), req.text().c_str(), TVision::Error, this);
    else {
	string mimeData = TSYS::strDecode(req.text(),TSYS::base64);
	if(file.write(mimeData.data(),mimeData.size()) < 0)
	    mod->postMess(mod->nodePath().c_str(),
		QString(_("Write data to file '%1' is fail: %2")).arg(fileName).arg(file.errorString()), TVision::Error, this);
    }
}

void LibProjProp::mimeDataChange( int row, int column )
{
    if( show_init ) return;

    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode("/mime/mime",TSYS::PathEl))->
	setAttr("col",(column==0)?"id":(column==1)?"tp":"")->
	setAttr("key_id",mimeDataTable->item(row,0)->data(Qt::UserRole).toString().toStdString())->
	setText(mimeDataTable->item(row,column)->text().toStdString());
    if(owner()->cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    tabChanged(1);
}

void LibProjProp::delStlItem( )
{
    int row = stl_table->currentRow();
    if(row < 0) { mod->postMess(mod->nodePath().c_str(), _("No one row is selected."), TVision::Warning, this); return; }

    XMLNode req("del");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(stl_table->objectName().toStdString(),TSYS::PathEl))->
	setAttr("key_id",stl_table->item(row,0)->text().toStdString());
    if(owner()->cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    showDlg(ed_it,true);
}

void LibProjProp::stlTableChange( int row, int column )
{
    if(show_init) return;

    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(stl_table->objectName().toStdString(),TSYS::PathEl))->
	setAttr("col","vl")->
	setAttr("key_id",stl_table->item(row,0)->text().toStdString())->
	setText(stl_table->item(row,column)->text().toStdString());
    if(owner()->cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    showDlg(ed_it, true);
}

//****************************************
//* Widget properties dialog             *
//****************************************
VisItProp::VisItProp( VisDevelop *parent ) :
    QDialog((QWidget*)parent), show_init(false), is_modif(false), ico_modif(false)
{
    QLabel *lab;
    QGroupBox *grp;
    QGridLayout *dlg_lay, *glay;
    QImage ico_t;
    setWindowTitle(_("Widget properties"));
    setWindowIcon(owner()->actVisItProp->icon());

    //Create tabulator
    QVBoxLayout *tab_lay = new QVBoxLayout(this);
    tab_lay->setMargin(5);
    wdg_tabs = new QTabWidget(this);
    tab_lay->addWidget(wdg_tabs);
    connect(wdg_tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

    //Add tab 'Widget'
    QScrollArea *scrl = new QScrollArea();
    wdg_tabs->addTab(scrl,_("Widget"));
    QWidget *tab_w = new QWidget(scrl);
    scrl->setWidget(tab_w);
    scrl->setWidgetResizable(true);
    //QWidget *tab_w = wdg_tabs->widget(0);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);

    // State parameters
    grp = new QGroupBox(_("State"),tab_w);
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);
    obj_ico = new QPushButton(tab_w);
    obj_ico->setObjectName("/wdg/cfg/ico");
    obj_ico->setToolTip(_("Item's icon. Click for download other."));
    obj_ico->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    obj_ico->setIconSize(QSize(60,60));
    obj_ico->setAutoDefault(false);
    connect(obj_ico, SIGNAL(released()), this, SLOT(selectIco()));
    glay->addWidget(obj_ico,0,0,4,1);

    lab = new QLabel(_("Enabled:"),tab_w);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    glay->addWidget(lab,0,1);
    obj_enable = new QCheckBox(tab_w);
    obj_enable->setObjectName("/wdg/st/en");
    connect(obj_enable, SIGNAL(stateChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_enable,0,2,1,4);

    lab = new QLabel(_("Parent widget:"),tab_w);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    glay->addWidget(lab,1,1);
    obj_parent = new QComboBox(tab_w);
    obj_parent->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    obj_parent->setObjectName("/wdg/st/parent");
    connect(obj_parent, SIGNAL(activated(int)), this, SLOT(isModify()));
    glay->addWidget(obj_parent,1,2,1,4);

    //  Specific parameter: page type
    lab = new QLabel(_("Page type:"),tab_w);
    glay->addWidget(lab,2,1);
    pg_tp = new QComboBox(tab_w);
    pg_tp->setObjectName("/wdg/st/pgTp");
    pg_tp->setWindowIconText(TSYS::addr2str(lab).c_str());
    connect(pg_tp, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(pg_tp,2,2);

    lab = new QLabel(_("Date of modification:"),tab_w);
    glay->addWidget(lab,3,1);
    obj_tmstmp = new QLabel(tab_w);
    obj_tmstmp->setTextInteractionFlags(Qt::TextSelectableByMouse);
    obj_tmstmp->setObjectName("/wdg/st/timestamp");
    obj_tmstmp->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(obj_tmstmp,3,2,1,4);

    grp->setLayout(glay);
    dlg_lay->addWidget(grp,0,0);

    //  Configuration parameters
    grp = new QGroupBox(_("Configuration"),tab_w);
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);

    glay->addWidget(new QLabel(_("Id:"),tab_w),0,0);
    obj_id = new QLabel(tab_w);
    obj_id->setTextInteractionFlags(Qt::TextSelectableByMouse);
    obj_id->setObjectName("/wdg/cfg/id");
    obj_id->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(obj_id,0,1,1,3);

    glay->addWidget(new QLabel(_("Root:"),tab_w),1,0);
    obj_root = new QLabel(tab_w);
    obj_root->setTextInteractionFlags(Qt::TextSelectableByMouse);
    obj_root->setObjectName("/wdg/cfg/root");
    obj_root->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(obj_root,1,1,1,3);

    glay->addWidget(new QLabel(_("Path:"),tab_w),2,0);
    obj_path = new QLabel(tab_w);
    obj_path->setTextInteractionFlags(Qt::TextSelectableByMouse);
    obj_path->setObjectName("/wdg/cfg/path");
    obj_path->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(obj_path,2,1,1,3);

    glay->addWidget(new QLabel(_("Name:"),tab_w),3,0);
    obj_name = new LineEdit(tab_w);
    obj_name->setObjectName("/wdg/cfg/name");
    connect(obj_name, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(obj_name,3,1,1,3);

    glay->addWidget(new QLabel(_("Description:"),tab_w),4,0);
    obj_descr = new TextEdit(tab_w);
    obj_descr->setObjectName("/wdg/cfg/descr");
    connect(obj_descr, SIGNAL(apply()), this, SLOT(isModify()));
    glay->addWidget(obj_descr,5,0,1,4);

    lab = new QLabel(_("Owner, group:"),tab_w);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    glay->addWidget(lab,6,0);
    obj_user = new QComboBox(tab_w);
    obj_user->setObjectName("/wdg/cfg/owner");
    connect(obj_user, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_user,6,1);
    obj_grp = new QComboBox(tab_w);
    obj_grp->setObjectName("/wdg/cfg/grp");
    connect(obj_grp, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_grp,6,2);
    lab = new QLabel(_("Access:"),tab_w);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    glay->addWidget(lab,7,0);
    obj_accuser = new QComboBox(tab_w);
    obj_accuser->setObjectName("/wdg/cfg/u_a");
    connect(obj_accuser, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accuser,7,1);
    obj_accgrp  = new QComboBox(tab_w);
    obj_accgrp->setObjectName("/wdg/cfg/g_a");
    connect(obj_accgrp, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accgrp,7,2);
    obj_accother= new QComboBox(tab_w);
    obj_accother->setObjectName("/wdg/cfg/o_a");
    connect(obj_accother, SIGNAL(currentIndexChanged(int)), this, SLOT(isModify()));
    glay->addWidget(obj_accother,7,3);

    grp->setLayout(glay);
    dlg_lay->addWidget(grp,1,0);

    //Add tab 'Attributes'
    wdg_tabs->addTab(new QWidget,_("Attributes"));
    tab_w = wdg_tabs->widget(1);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);

    // Add attributes view widget
    obj_attr = new InspAttr(tab_w,owner());
    connect(obj_attr, SIGNAL(modified(const string&)), this, SIGNAL(apply(const string&)));
    dlg_lay->addWidget(obj_attr,0,0);

    //Add tab 'Attribute cofiguration'
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

    // Add attributes configuration widget
    obj_attr_cfg = new QTreeWidget(attr_cf_fr);
    obj_attr_cfg->setAlternatingRowColors(true);
    obj_attr_cfg->setObjectName("/proc/attr");
    obj_attr_cfg->setItemDelegate(new ItemDelegate);
    obj_attr_cfg->setSelectionBehavior(QAbstractItemView::SelectRows);
    obj_attr_cfg->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(obj_attr_cfg, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(changeAttr(QTreeWidgetItem*,int)));
    obj_attr_cfg->setHeaderLabels(QStringList() << _("Id") << _("Name") << _("Data type") << _("Work area") << _("Proc.")
						<< _("Configuration") << _("Configuration template"));
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
    lab = new QLabel(_("Procedure calculate (ms):"),wdg_proc_fr);
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

    //Add tab 'Links'
    wdg_tabs->addTab(new QWidget,_("Links"));
    tab_w = wdg_tabs->widget(3);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);

    // Add attributes view widget
    obj_lnk = new InspLnk(tab_w,owner());
    //connect(obj_attr, SIGNAL(modified(const string&)), this, SIGNAL(apply(const string&)));
    dlg_lay->addWidget(obj_lnk,0,0);

    //Add button box
    butbox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    connect(butbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));

    tab_lay->addWidget(butbox);

    //End resize
    resize(800,600);
}

VisItProp::~VisItProp( )
{

}

VisDevelop *VisItProp::owner( )	{ return (VISION::VisDevelop*)parentWidget(); }

void VisItProp::showDlg( const string &iit, bool reload )
{
    vector<string> ls;
    string sval;
    QImage ico_t;
    ed_it = iit;

    show_init = true;

    //Update elements present, visible and values
    XMLNode req("get");

    XMLNode info_req("info");
    info_req.setAttr("path",ed_it);
    if(owner()->cntrIfCmd(info_req)) {
	mod->postMess(mod->nodePath().c_str(), QString(_("Getting node '%1' information error.")).arg(ed_it.c_str()), TVision::Error, this);
	return;
    }

    XMLNode *root, *gnd;
    root = info_req.childGet(0);

    setWindowTitle( root->attr("dscr").c_str() );

    //Generic dialog's page
    gnd = TCntrNode::ctrId(root,"/wdg",true);
    wdg_tabs->setTabEnabled(0,gnd);
    if(gnd) {
	wdg_tabs->setTabText(0,gnd->attr("dscr").c_str());

	// Enable stat
	gnd = TCntrNode::ctrId(root,obj_enable->objectName().toStdString(),true);
	obj_enable->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_enable->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_enable->setChecked(s2i(req.text()));
	}
	// Parent widget
	gnd = TCntrNode::ctrId(root,obj_parent->objectName().toStdString(),true);
	obj_parent->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) selectParent();
	// TimeStamp
	gnd = TCntrNode::ctrId(root, obj_tmstmp->objectName().toStdString(), true);
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_tmstmp->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_tmstmp->setText(tm2s(s2i(req.text()),"").c_str());
	}else obj_tmstmp->setText("");

	// User
	gnd = TCntrNode::ctrId(root,obj_user->objectName().toStdString(),true);
	obj_user->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_user->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) sval = req.text();
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/wdg/u_lst",TSYS::PathEl));
	    obj_user->clear();
	    if(!owner()->cntrIfCmd(req))
		for(unsigned i_l = 0; i_l < req.childSize(); i_l++)
		{
		    obj_user->addItem(req.childGet(i_l)->text().c_str());
		    if(sval == req.childGet(i_l)->text()) obj_user->setCurrentIndex(i_l);
		}
	}
	// Group
	gnd = TCntrNode::ctrId(root,obj_grp->objectName().toStdString(),true);
	obj_grp->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_grp->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) sval = req.text();
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/wdg/g_lst",TSYS::PathEl));
	    obj_grp->clear();
	    if(!owner()->cntrIfCmd(req))
		for(unsigned i_l = 0; i_l < req.childSize(); i_l++)
		{
		    obj_grp->addItem(req.childGet(i_l)->text().c_str());
		    if(sval == req.childGet(i_l)->text()) obj_grp->setCurrentIndex(i_l);
		}
	}

	// Icon
	gnd = TCntrNode::ctrId(root,obj_ico->objectName().toStdString(),true);
	ico_modif = gnd && s2i(gnd->attr("acs"))&SEC_WR;
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_ico->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req) && (sval = TSYS::strDecode(req.text(),TSYS::base64)).size() &&
		    ico_t.loadFromData((const uchar*)sval.c_str(),sval.size()))
		obj_ico->setIcon(QPixmap::fromImage(ico_t));
	    else obj_ico->setIcon(QIcon());
	}
	// Permition
	string wstr;
	gnd = TCntrNode::ctrId(root,obj_accuser->objectName().toStdString(),true);
	obj_accuser->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    obj_accuser->clear();
	    for(int i_l = 0, i_i = 0; (wstr=TSYS::strSepParse(gnd->attr("sel_list"),0,';',&i_l)).size(); )
		obj_accuser->addItem(wstr.c_str(),s2i(TSYS::strSepParse(gnd->attr("sel_id"),0,';',&i_i)));
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accuser->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_accuser->setCurrentIndex(obj_accuser->findData(s2i(req.text())));
	}
	gnd = TCntrNode::ctrId(root,obj_accgrp->objectName().toStdString(),true);
	obj_accgrp->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    obj_accgrp->clear();
	    for(int i_l = 0, i_i = 0; (wstr=TSYS::strSepParse(gnd->attr("sel_list"),0,';',&i_l)).size(); )
		obj_accgrp->addItem(wstr.c_str(),s2i(TSYS::strSepParse(gnd->attr("sel_id"),0,';',&i_i)));
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accgrp->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_accgrp->setCurrentIndex(obj_accgrp->findData(s2i(req.text())));
	}
	gnd = TCntrNode::ctrId(root,obj_accother->objectName().toStdString(),true);
	obj_accother->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    obj_accother->clear();
	    for(int i_l = 0, i_i = 0; (wstr=TSYS::strSepParse(gnd->attr("sel_list"),0,';',&i_l)).size(); )
		obj_accother->addItem(wstr.c_str(),s2i(TSYS::strSepParse(gnd->attr("sel_id"),0,';',&i_i)));
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_accother->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_accother->setCurrentIndex(obj_accother->findData(s2i(req.text())));
	}
	// Id
	gnd = TCntrNode::ctrId(root,obj_id->objectName().toStdString(),true);
	if(gnd) {
	    req.clear();
	    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_id->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_id->setText(req.text().c_str());
	}
	// Root
	gnd = TCntrNode::ctrId(root,obj_root->objectName().toStdString(),true);
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_root->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_root->setText(req.text().c_str());
	}
	// Path
	gnd = TCntrNode::ctrId(root,obj_path->objectName().toStdString(),true);
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_path->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_path->setText(req.text().c_str());
	}
	//Name
	gnd = TCntrNode::ctrId(root,obj_name->objectName().toStdString(),true);
	obj_name->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_name->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_name->setValue(req.text().c_str());
	}
	//Description
	gnd = TCntrNode::ctrId(root,obj_descr->objectName().toStdString(),true);
	obj_descr->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	if(gnd) {
	    req.clear();
	    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_descr->objectName().toStdString(),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req)) obj_descr->setText(req.text().c_str());
	}

	// Special fields
	//  Page type
	gnd = TCntrNode::ctrId(root,pg_tp->objectName().toStdString(),true);
	pg_tp->setVisible(gnd); ((QLabel*)TSYS::str2addr(pg_tp->windowIconText().toStdString()))->setVisible(gnd);
	if(gnd) {
	    pg_tp->setEnabled(s2i(gnd->attr("acs"))&SEC_WR);

	    int sel_val = 0;
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(pg_tp->objectName().toStdString(),TSYS::PathEl));
	    if( !owner()->cntrIfCmd(req) ) sel_val = s2i(req.text());
	    //   Get combo list
	    pg_tp->clear();
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(gnd->attr("select"),TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req))
		for(unsigned i_el = 0; i_el < req.childSize(); i_el++)
		    pg_tp->addItem(req.childGet(i_el)->text().c_str(),s2i(req.childGet(i_el)->attr("id")));
	    pg_tp->setCurrentIndex(pg_tp->findData(sel_val));
	}
    }

    //Attributes dialog's page
    gnd = TCntrNode::ctrId(root,"/attr",true);
    wdg_tabs->setTabEnabled(1,gnd);
    if(gnd) wdg_tabs->setTabText(1,gnd->attr("dscr").c_str());

    gnd = TCntrNode::ctrId(root,"/links",true);
    wdg_tabs->setTabEnabled(3,gnd);
    if(gnd) wdg_tabs->setTabText(3,gnd->attr("dscr").c_str());

    //Process dialog's page
    gnd=TCntrNode::ctrId(root,"/proc",true);
    wdg_tabs->setTabEnabled(2,gnd);
    if(gnd) wdg_tabs->setTabText(2,gnd->attr("dscr").c_str());
    is_modif = false;

    //Show dialog
    show();
    raise();
    activateWindow();
    if(reload) tabChanged(wdg_tabs->currentIndex());
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

	    // Get node information
	    XMLNode info_req("info");
	    info_req.setAttr("path",ed_it);
	    if(owner()->cntrIfCmd(info_req)) {
		mod->postMess(mod->nodePath().c_str(),
			QString(_("Getting node '%1' information error.")).arg(ed_it.c_str()),TVision::Error, this);
		return;
	    }
	    string sval;
	    XMLNode *root, *gnd;
	    root = info_req.childGet(0);
	    // Get widgets list
	    vector<string>	wlst;
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/proc/w_lst",TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req))
		for(unsigned i_w = 0; i_w < req.childSize(); i_w++)
		    wlst.push_back(req.childGet(i_w)->text());
		    //wlst.push_back(req.childGet(i_w)->attr("id"));
	    //  Fill table
	    //  Delete no present root items
	    for(int i_r = 0; i_r < obj_attr_cfg->topLevelItemCount(); i_r++)
	    {
		unsigned i_w;
		for(i_w = 0; i_w < wlst.size(); i_w++)
		    if(obj_attr_cfg->topLevelItem(i_r)->text(0) == wlst[i_w].c_str()) break;
		if(i_w >= wlst.size())	delete obj_attr_cfg->topLevelItem(i_r--);
	    }
	    //  Add root items
	    for(unsigned i_w = 0; i_w < wlst.size(); i_w++)
	    {
		QTreeWidgetItem *root_it;
		int i_r;
		for(i_r = 0; i_r < obj_attr_cfg->topLevelItemCount(); i_r++)
		    if(obj_attr_cfg->topLevelItem(i_r)->text(0) == wlst[i_w].c_str()) break;
		if(i_r < obj_attr_cfg->topLevelItemCount()) root_it = obj_attr_cfg->topLevelItem(i_r);
		else root_it = new QTreeWidgetItem(0);

		req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(obj_attr_cfg->objectName().toStdString(),TSYS::PathEl))->
		    setAttr("wdg",wlst[i_w].c_str());
		if(owner()->cntrIfCmd(req)) continue;

		root_it->setText(0,wlst[i_w].c_str());
		root_it->setData(0,Qt::UserRole,0);
		obj_attr_cfg->addTopLevelItem(root_it);

		//  Delete no presents widget's items
		for(int i_r = 0; i_r < root_it->childCount(); i_r++)
		{
		    unsigned i_l;
		    for(i_l = 0; i_l < req.childGet(0)->childSize(); i_l++)
			if(root_it->child(i_r)->text(0) == req.childGet("id","id")->childGet(i_l)->text().c_str())
			    break;
		    if(i_l >= req.childGet(0)->childSize())	delete root_it->child(i_r--);
		}

		//  Add widget's items
		for(unsigned i_l = 0; i_l < req.childGet(0)->childSize(); i_l++)
		{
		    QTreeWidgetItem *cur_it;
		    int i_r;
		    for(i_r = 0; i_r < root_it->childCount(); i_r++)
			if(root_it->child(i_r)->text(0) == req.childGet("id","id")->childGet(i_l)->text().c_str())
			    break;
		    if(i_r < root_it->childCount()) cur_it = root_it->child(i_r);
		    else cur_it = new QTreeWidgetItem(root_it);
		    cur_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
		    cur_it->setText(0,req.childGet("id","id")->childGet(i_l)->text().c_str());
		    cur_it->setData(0,Qt::UserRole,cur_it->text(0));
		    cur_it->setText(1,req.childGet("id","name")->childGet(i_l)->text().c_str());
		    cur_it->setData(2,Qt::DisplayRole,s2i(req.childGet("id","type")->childGet(i_l)->text()));
		    cur_it->setText(3,req.childGet("id","wa")->childGet(i_l)->text().c_str());
		    cur_it->setData(4,Qt::DisplayRole,(bool)s2i(req.childGet("id","proc")->childGet(i_l)->text()));
		    cur_it->setData(5,Qt::DisplayRole,s2i(req.childGet("id","cfg")->childGet(i_l)->text()));
		    cur_it->setText(6,req.childGet("id","cfgtmpl")->childGet(i_l)->text().c_str());
		}
	    }
	    //  Load types and configurations
	    QStringList	atypes;
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/proc/tp_ls",TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req))
		for(unsigned i_el = 0; i_el < req.childSize(); i_el++)
		    atypes.push_back((req.childGet(i_el)->text()+"|"+req.childGet(i_el)->attr("id")).c_str());
	    if(obj_attr_cfg->topLevelItemCount()) obj_attr_cfg->topLevelItem(0)->setData(0,Qt::UserRole,atypes);
	    atypes.clear();
	    req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/proc/lnk_ls",TSYS::PathEl));
	    if(!owner()->cntrIfCmd(req))
		for(unsigned i_el = 0; i_el < req.childSize(); i_el++)
		    atypes.push_back((req.childGet(i_el)->text()+"|"+req.childGet(i_el)->attr("id")).c_str());
	    if(obj_attr_cfg->topLevelItemCount()) obj_attr_cfg->topLevelItem(0)->setData(0,Qt::UserRole+1,atypes);

	    //  Calculate period
	    gnd = TCntrNode::ctrId(root,proc_per->objectName().toStdString(),true);
	    proc_per->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	    if(gnd) {
		req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(proc_per->objectName().toStdString(),TSYS::PathEl));
		if(!owner()->cntrIfCmd(req)) proc_per->setValue(req.text().c_str());
	    }
	    //  Calc language
	    gnd = TCntrNode::ctrId(root,proc_lang->objectName().toStdString(),true);
	    proc_lang->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	    if(gnd) {
		req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(proc_lang->objectName().toStdString(),TSYS::PathEl));
		if(!owner()->cntrIfCmd(req)) sval = req.text().c_str();
		//   Get combo list
		proc_lang->clear();
		req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode(gnd->attr("select"),TSYS::PathEl));
		if(!owner()->cntrIfCmd(req))
		    for(unsigned i_el = 0; i_el < req.childSize(); i_el++)
			proc_lang->addItem(req.childGet(i_el)->text().c_str());
		int cur_el = proc_lang->findText(sval.c_str());
		if(cur_el < 0) proc_lang->addItem(sval.c_str());
		proc_lang->setCurrentIndex(proc_lang->findText(sval.c_str()));
	    }
	    //  Calc procedure
	    gnd = TCntrNode::ctrId(root,proc_text->objectName().toStdString(),true);
	    proc_text->setEnabled(gnd && s2i(gnd->attr("acs"))&SEC_WR);
	    if(gnd) {
		req.clear()->setName("CntrReqs")->setAttr("path",ed_it);
		req.childAdd("get")->setAttr("path",TSYS::strEncode(proc_text->objectName().toStdString(),TSYS::PathEl));
		req.childAdd("SnthHgl")->setAttr("path",TSYS::strEncode(proc_text->objectName().toStdString(),TSYS::PathEl));
		if(!owner()->cntrIfCmd(req)) {
		    proc_text->setText(req.childGet(0)->text().c_str());
		    proc_text->setSnthHgl(*req.childGet(1));
		}
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
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_parent->objectName().toStdString(),TSYS::PathEl));
    if(!owner()->cntrIfCmd(req)) {
	QString cur_val = req.text().c_str();

	//Get values list
	req.clear()->setAttr("path",ed_it+"/"+TSYS::strEncode("/wdg/w_lst",TSYS::PathEl));
	owner()->cntrIfCmd(req);

	//Load combobox
	obj_parent->clear();
	for(unsigned i_l = 0; i_l < req.childSize(); i_l++)
	    obj_parent->addItem(req.childGet(i_l)->text().c_str());
	if(obj_parent->findText(cur_val) < 0) obj_parent->addItem(cur_val);
	obj_parent->setCurrentIndex(obj_parent->findText(cur_val));
    }
}

void VisItProp::selectIco( )
{
    QImage ico_t;

    if(!ico_modif)	return;

    QString fileName = owner()->getFileName(_("Load icon picture"),"",_("Images (*.png *.jpg)"));

    if(fileName.isEmpty())	return;
    if(!ico_t.load(fileName)) {
	mod->postMess(mod->nodePath().c_str(), QString(_("Load icon image '%1' error.")).arg(fileName), TVision::Warning, this);
	return;
    }

    obj_ico->setIcon(QPixmap::fromImage(ico_t));

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    ico_t.save(&buffer,"PNG");

    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_ico->objectName().toStdString(),TSYS::PathEl))->
	setText(TSYS::strEncode(string(ba.data(),ba.size()),TSYS::base64));
    if(owner()->cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);

    is_modif = true;
}

void VisItProp::isModify( QObject *snd )
{
    bool update = false;
    if(show_init) return;
    if(!snd) snd = sender();

    //Prepare command
    QString oname = snd->objectName();

    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(oname.toStdString(),TSYS::PathEl));

    if(oname == obj_enable->objectName()) {
	req.setText(i2s(((QCheckBox*)snd)->isChecked()));
	update = true;
    }
    else if(oname == obj_parent->objectName()) {
	req.setText(((QComboBox*)snd)->currentText().toStdString());
	update = true;
    }
    else if(oname == obj_user->objectName() || oname == obj_grp->objectName() || oname == proc_lang->objectName())
    {
	req.setText(((QComboBox*)snd)->currentText().toStdString());
	update = true;
    }
    else if(oname == obj_accuser->objectName() || oname == obj_accgrp->objectName() || oname == obj_accother->objectName())
    {
	req.setText(((QComboBox*)snd)->itemData(((QComboBox*)snd)->currentIndex()).toString().toStdString());
	update = true;
    }
    else if(oname == obj_name->objectName() || oname == proc_per->objectName())
	req.setText(((LineEdit*)snd)->value().toStdString());
    else if(oname == obj_descr->objectName() || oname == proc_text->objectName())
	req.setText(((TextEdit*)snd)->text().toStdString());
    else if(oname == pg_tp->objectName()) {
	req.setText(((QComboBox*)snd)->itemData(((QComboBox*)snd)->currentIndex()).toString().toStdString());
	update = true;
    }
    else return;

    //Send command
    if(owner()->cntrIfCmd(req)) {
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	showDlg(ed_it,true);
    }
    else {
	//Post command updating
	if(oname == obj_parent->objectName())	selectParent();
	if(update) showDlg(ed_it,true);
    }

    is_modif = true;
}

void VisItProp::closeEvent( QCloseEvent *ce )
{
    //Check for not apply LineEdit and TextEdit widgets
    QList<LineEdit*> lnEdWs = findChildren<LineEdit*>();
    QList<TextEdit*> txtEdWs = findChildren<TextEdit*>();
    bool notApplyPresent = false;
    for(int i_it = 0; !notApplyPresent && i_it < lnEdWs.size(); ++i_it) notApplyPresent = lnEdWs[i_it]->isEdited();
    for(int i_it = 0; !notApplyPresent && i_it < txtEdWs.size(); ++i_it) notApplyPresent = txtEdWs[i_it]->isEdited();
    if(notApplyPresent && QMessageBox::information(this,_("Changes apply"),_("Some changes you don't apply!\nApply now or lost the?"),
	    QMessageBox::Apply|QMessageBox::Cancel,QMessageBox::Apply) == QMessageBox::Apply)
    {
	for(int i_it = 0; i_it < lnEdWs.size(); ++i_it) if(lnEdWs[i_it]->isEdited()) isModify(lnEdWs[i_it]);
	for(int i_it = 0; i_it < txtEdWs.size(); ++i_it) if(txtEdWs[i_it]->isEdited()) isModify(txtEdWs[i_it]);
    }

    if(is_modif) emit apply(ed_it);

    is_modif = false;
    ed_it = "";

    ce->accept();
}

void VisItProp::showEvent( QShowEvent * event )
{
    QSize src = size();
    adjustSize();
    resize(size().expandedTo(src));
}

void VisItProp::addAttr( )
{
    string swdg;
    if(obj_attr_cfg->currentItem()) {
	if(obj_attr_cfg->currentItem()->parent())
	    swdg = obj_attr_cfg->currentItem()->parent()->text(0).toStdString();
	else swdg = obj_attr_cfg->currentItem()->text(0).toStdString();
    }
    //Check current attribute
    if(swdg.empty()) {
	mod->postMess(mod->nodePath().c_str(), _("Valid widget is not selected"),TVision::Warning,this);
	return;
    }

    XMLNode req("add");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_attr_cfg->objectName().toStdString(),TSYS::PathEl))->
	setAttr("wdg",swdg);
    if(owner()->cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
    else {
	tabChanged(2);
	is_modif = true;
    }
}

void VisItProp::delAttr( )
{
    //Check current attribute
    if(!obj_attr_cfg->currentItem() || !obj_attr_cfg->currentItem()->parent())
    {
	mod->postMess(mod->nodePath().c_str(), _("Valid attribute is not selected"),TVision::Warning,this);
	return;
    }

    //Delete current attribute
    XMLNode req("del");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_attr_cfg->objectName().toStdString(),TSYS::PathEl))->
	setAttr("wdg",obj_attr_cfg->currentItem()->parent()->text(0).toStdString())->
	setAttr("key_id",obj_attr_cfg->currentItem()->text(0).toStdString());
    if(owner()->cntrIfCmd(req)) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
    else {
	delete obj_attr_cfg->currentItem();
	is_modif = true;
    }
}

void VisItProp::changeAttr(QTreeWidgetItem *it, int col)
{
    if(show_init) return;

    //Check current attribute
    if(!it || !it->parent()) {
	mod->postMess(mod->nodePath().c_str(), _("Valid attribute is not selected"),TVision::Warning,this);
	return;
    }

    //Get collumn id
    QString scol, sval;
    switch(col)
    {
	case 0:	scol = "id";	sval = it->text(col);	break;
	case 1:	scol = "name";	sval = it->text(col);	break;
	case 2:	scol = "type";	sval = it->data(col,Qt::DisplayRole).toString();	break;
	case 3:	scol = "wa";	sval = it->text(col);	break;
	case 4:	scol = "proc";	sval = QString::number(it->data(col,Qt::DisplayRole).toBool());	break;
	case 5:	scol = "cfg";	sval = it->data(col,Qt::DisplayRole).toString();	break;
	case 6:	scol = "cfgtmpl"; sval = it->text(col);	break;
    }

    //Set current attribute
    XMLNode req("set");
    req.setAttr("path",ed_it+"/"+TSYS::strEncode(obj_attr_cfg->objectName().toStdString(),TSYS::PathEl))->
	setAttr("wdg",it->parent()->text(0).toStdString())->
	setAttr("key_id",it->data(0,Qt::UserRole).toString().toStdString())->
	setAttr("col",scol.toStdString())->setText(sval.toStdString());
    if(owner()->cntrIfCmd(req)) {
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
	if(scol == "id") {
	    show_init = true;
	    it->setText(0,it->data(0,Qt::UserRole).toString());
	    show_init = false;
	}
	tabChanged(2);
    }
    else {
	if(scol == "id") {
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
    if(index.isValid() && !index.model()->rowCount(index))
    {
	if(index.column() == 2 || index.column() == 5) {
	    QString val("String");
	    QStringList types = index.model()->index(0,0).data(Qt::UserRole+((index.column()==5)?1:0)).toStringList();
	    for(int i_l = 0; i_l < types.size(); i_l++)
		if(s2i(TSYS::strSepParse(types[i_l].toStdString(),1,'|')) == index.data(Qt::DisplayRole).toInt())
		    val = TSYS::strSepParse(types[i_l].toStdString(),0,'|').c_str();
	    drawDisplay(painter, option, option.rect, val);
	    return;
	}
	if(index.column() == 4) {
	    drawBackground(painter, option, index);
	    if(index.data(Qt::DisplayRole).toBool()) {
		QImage img(":/images/ok.png");
		painter->drawImage(option.rect.center().x()-img.width()/2,option.rect.center().y()-img.height()/2,img);
	    }
	    drawFocus(painter, option, option.rect);
	    return;
	}
    }
    QItemDelegate::paint(painter, option, index);
}

QWidget *VisItProp::ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *w_del = NULL;
    if(!index.isValid()) return 0;

    QVariant value = index.data(Qt::EditRole);

    if(index.parent().isValid()) {
	if(index.column() == 2 || index.column() == 5) w_del = new QComboBox(parent);
	else {
	    QItemEditorFactory factory;
	    w_del = factory.createEditor(value.type(),parent);
	}
    }

    return w_del;
}

void VisItProp::ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == 2 || index.column() == 5) {
	QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	QStringList types = index.model()->index(0,0).data(Qt::UserRole+((index.column()==5)?1:0)).toStringList();
	for(int i_l = 0; i_l < types.size(); i_l++)
	    comb->addItem(TSYS::strSepParse(types[i_l].toStdString(),0,'|').c_str(),
		    s2i(TSYS::strSepParse(types[i_l].toStdString(),1,'|')));
	comb->setCurrentIndex(comb->findData(index.data(Qt::DisplayRole).toInt()));
    }
    else QItemDelegate::setEditorData(editor, index);
}

void VisItProp::ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() == 2 || index.column() == 5) {
	QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	model->setData(index,comb->itemData(comb->currentIndex()),Qt::EditRole);
    }
    else if(index.column() == 4) {
	QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	model->setData(index,(bool)comb->currentIndex(),Qt::EditRole);
    }
    else QItemDelegate::setModelData(editor, model, index);
}
