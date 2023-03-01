
//OpenSCADA module UI.Vision file: vis_widgs.cpp
/***************************************************************************
 *   Copyright (C) 2007-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#include <math.h>
#include <algorithm>

#include <QVBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QEvent>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QTimeEdit>
#include <QKeyEvent>
#include <QTextEdit>
#include <QFontComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QTimer>
#include <QCalendarWidget>
#include <QApplication>
#include <QStatusBar>
#include <QToolTip>
#include <QCompleter>
#include <QDesktopWidget>
#include <QScrollBar>

#include "vis_run.h"
#include "vis_shapes.h"
#include "vis_widgs.h"
#include "vis_run_widgs.h"

using namespace OSCADA_QT;
using namespace VISION;

#undef _
#define _(mess) mod->I18N(mess, lang.c_str()).c_str()

//*************************************************
//* Id and name input dialog                      *
//*************************************************
InputDlg::InputDlg( QWidget *parent, const QIcon &icon, const QString &mess, const QString &ndlg, bool with_id, bool with_nm,
		    const string &lang, const string &istCtxId ) :
	QDialog(parent), mId(NULL), mName(NULL), stCtxId(istCtxId)
{
    setWindowModality(Qt::WindowModal);
    //setMaximumSize(800, 600);
    setWindowTitle(ndlg);
    setWindowIcon(icon);
    //setSizeGripEnabled(true);

    QVBoxLayout *dlg_lay = new QVBoxLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);

    //Icon label and text message
    QHBoxLayout *intr_lay = new QHBoxLayout;
    intr_lay->setSpacing(6);

    QLabel *icon_lab = new QLabel(this);
    icon_lab->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
    icon_lab->setPixmap(icon.pixmap(48));
    intr_lay->addWidget(icon_lab);

    inpLab = new QLabel(mess, this);
    //inpLab->setAlignment(Qt::AlignHCenter);
    inpLab->setWordWrap(true);
    intr_lay->addWidget(inpLab);
    dlg_lay->addItem(intr_lay);

    mEdLay = new QGridLayout;
    mEdLay->setSpacing(6);
    //Id and name fields
    if(with_id) {
	mEdLay->addWidget(new QLabel(_("Identifier:"),this), 0, 0);
	mId = new QLineEdit(this);
	mEdLay->addWidget(mId, 0, 1);
    }
    if(with_nm) {
	mEdLay->addWidget(new QLabel(_("Name:"),this), 1, 0);
	mName = new QLineEdit(this);
	mEdLay->addWidget(mName, 1, 1);
    }

    mEdLay->addItem(new QSpacerItem(10,0,QSizePolicy::Minimum,QSizePolicy::Expanding), 100, 0, 1, -1);

    dlg_lay->addItem(mEdLay);

    //Qk and Cancel buttons
    QFrame *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Raised);
    dlg_lay->addWidget(sep);

    QDialogButtonBox *butBox = new QDialogButtonBox(QDialogButtonBox::Ok|
						     QDialogButtonBox::Cancel, Qt::Horizontal, this);
    QImage ico_t;
    butBox->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
    butBox->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(butBox, SIGNAL(accepted()), this, SLOT(accept()));
    butBox->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
    butBox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    connect(butBox, SIGNAL(rejected()), this, SLOT(reject()));
    dlg_lay->addWidget(butBox);

    //Restore the window state
    if(parentWidget()->property("oscdUser").toString().size() && stCtxId.size()) {
	int off = 0;
	string rst = mod->uiPropGet("InDlgSt"+stCtxId, parentWidget()->property("oscdUser").toString().toStdString());
	int	wH = s2i(TSYS::strParse(rst,0,":",&off)),
	    wW = s2i(TSYS::strParse(rst,0,":",&off));
	if(wH > 100 && wW > 100) resize(wH, wW);
	else resize(400, 120+(40*with_nm)+(40*with_id));
    }
}

InputDlg::~InputDlg( )
{
    //Save the window state
    if(parentWidget()->property("oscdUser").toString().size() && stCtxId.size())
	mod->uiPropSet("InDlgSt"+stCtxId, i2s(width())+":"+i2s(height()), parentWidget()->property("oscdUser").toString().toStdString());
}

QString InputDlg::id( )		{ return mId ? mId->text() : ""; }

QString InputDlg::name( )	{ return mName ? mName->text() : ""; }

QString InputDlg::mess( )	{ return inpLab->text(); }

void InputDlg::setId(const QString &val)
{
    if(mId) mId->setText(val);
}

void InputDlg::setName(const QString &val)
{
    if(mName) mName->setText(val);
}

void InputDlg::setMess( const QString &val )	{ inpLab->setText(val); }

void InputDlg::setIdLen( int len )
{
    if(mId) mId->setMaxLength(len);
}

void InputDlg::showEvent( QShowEvent * event )
{
    QSize src = size();
    adjustSize();
    resize(size().expandedTo(src));

#if defined(__ANDROID__)
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2);
#endif
}

//*************************************************
//* User select dialog                            *
//*************************************************
DlgUser::DlgUser( const QString &iuser, const QString &ipass, const QString &iVCAstat, QWidget *parent, const string &hint, const string &lang ) :
    QDialog(parent), VCAstat(iVCAstat), stSel(NULL)
{
    setWindowTitle(_("Selecting an user"));

    QVBoxLayout *dlg_lay = new QVBoxLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);

    QGridLayout *edLay = new QGridLayout;
    edLay->setSpacing(6);
    if(VCAstat == "*") {
	edLay->addWidget(new QLabel(_("Station:"),this), 0, 0);
	stSel = new QComboBox(this);
	stSel->addItem(_("<Local>"), ".");
	vector<TTransportS::ExtHost> lst;
	SYS->transport().at().extHostList("*", lst, false, -1, lang);
	sort(lst.begin(), lst.end(), TVision::compareHosts);
	for(unsigned iLs = 0; iLs < lst.size(); iLs++)
	    stSel->addItem(lst[iLs].name.c_str(), lst[iLs].id.c_str());
	edLay->addWidget(stSel, 0, 1);
	connect(stSel, SIGNAL(currentIndexChanged(int)), this, SLOT(stChanged(int)));
    }
    edLay->addWidget(new QLabel(_("User:"),this), 1, 0);
    users = new QComboBox(this);
    users->setEditable(true);
    edLay->addWidget(users, 1, 1);
    edLay->addWidget(new QLabel(_("Password:"),this), 2, 0);
    passwd = new QLineEdit(this);
    passwd->setEchoMode(QLineEdit::Password);
    edLay->addWidget(passwd, 2, 1);
    dlg_lay->addItem(edLay);

    dlg_lay->addItem(new QSpacerItem(20,0,QSizePolicy::Minimum,QSizePolicy::Expanding));

    QFrame *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Raised);
    dlg_lay->addWidget(sep);

    butBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this);
    QImage ico_t;
    butBox->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
    butBox->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(butBox, SIGNAL(accepted()), this, SLOT(accept()));
    butBox->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
    butBox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    connect(butBox, SIGNAL(rejected()), this, SLOT(reject()));
    dlg_lay->addWidget(butBox);

    connect(this, SIGNAL(finished(int)), this, SLOT(finish(int)));

    //users->setEditText(mod->userStart().c_str());
    if(VCAstat == "*") VCAstat = ".";
    users->setEditText(iuser);
    passwd->setText(ipass);

    mAutoRes = NoAuto;

    fillUsers(hint);
}

QString DlgUser::user( )	{ return users->currentText(); }

QString DlgUser::password( )	{ return passwd->text(); }

void DlgUser::fillUsers( const string &hint )
{
    bool chckHintUser = hint.size() && hint != "*";

    //Fill users list
    users->clear();
    if(stSel && VCAstat != ".") users->setEditText("");
    XMLNode req("get"); req.setAttr("path","/Security/%2fusgr%2fusers");
    if(!mod->cntrIfCmd(req,user().toStdString(),password().toStdString(),VCAstat.toStdString(),true)) {
	size_t	pasSep = hint.find(":");
	string	h_user = (pasSep == string::npos) ? hint : TSYS::strDecode(hint.substr(0,pasSep),TSYS::Custom),
		h_pass = (pasSep == string::npos) ? "" : TSYS::strDecode(hint.substr(pasSep+1),TSYS::Custom);
	for(unsigned iU = 0; iU < req.childSize(); iU++) {
	    users->addItem(req.childGet(iU)->text().c_str());
	    if(chckHintUser && h_user == req.childGet(iU)->text()) {
		users->setEditText(h_user.c_str());
		if(pasSep == string::npos)
		    mAutoRes = (VCAstat == "." && dynamic_cast<VisRun*>(parentWidget()->window()) && SYS->security().at().usrAt(((VisRun*)parentWidget()->window())->user()).at().permitCmpr(user().toStdString()) <= 0)
				? SelOK : SelErr;
		else {
		    passwd->setText(h_pass.c_str());
		    finish(SelOK);
		    mAutoRes = (Results)result();
		}
		break;
	    }
	}
    }
    if(stSel && VCAstat != ".") users->setEditText("");
}

void DlgUser::stChanged( int index )
{
    QComboBox *stSel = (QComboBox*)sender();

    if(index == -1) {
	VCAstat = "*";
	users->clear();
	butBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	return;
    }
    else VCAstat = stSel->itemData(index).toString();

    butBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    fillUsers();
}

void DlgUser::finish( int result )
{
    if(result) {
	//Check user auth
	XMLNode req("get"); req.setAttr("path", "/%2fgen%2fid");
	if((VCAstat == "." && (user().toStdString() == mod->userStart() || (SYS->security().at().usrPresent(user().toStdString()) &&
		    SYS->security().at().usrAt(user().toStdString()).at().auth(password().toStdString())))) ||
		(VCAstat != "." && !mod->cntrIfCmd(req,user().toStdString(),password().toStdString(),VCAstat.toStdString(),true)))
	{
	    if(user().isEmpty()) users->setEditText(req.attr("user").c_str());
	    setResult(SelOK);
	}
	else setResult(SelErr);
    }
    else setResult(SelCancel);
}

void DlgUser::showEvent( QShowEvent * event )
{
    QSize src = size();
    adjustSize();
    resize(size().expandedTo(src));

#if defined(__ANDROID__)
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2);
#endif
}

//*********************************************
//* Status bar user widget                    *
//*********************************************
UserStBar::UserStBar( const string &iuser, const string &ipass, const string &iVCAstat, QWidget *parent ) : QLabel(parent)
{
    setUser(iuser);
    setPass(ipass);
    setVCAStation(iVCAstat);
}

string UserStBar::user( )
{
    mod->dataRes().lock();
    string rez = userTxt;
    mod->dataRes().unlock();
    return rez;
}

string UserStBar::pass( )
{
    mod->dataRes().lock();
    string rez = userPass;
    mod->dataRes().unlock();
    return rez;
}

void UserStBar::setUser( const string &val )
{
    MtxAlloc res(mod->dataRes(), true);
    setText(QString("<font color='%1'>%2</font>").arg(colorAdjToBack((val=="root")?"red":"green",palette().color(QPalette::Window)).name()).arg(val.size()?val.c_str():"*"));
    userTxt = val;

    if(window()) window()->setProperty("oscdUser", val.c_str());

    if(userTxtOrig.empty()) userTxtOrig = val;
}

void UserStBar::setPass( const string &val )
{
    mod->dataRes().lock();
    userPass = val;
    if(userPassOrig.empty()) userPassOrig = val;
    mod->dataRes().unlock();
}

bool UserStBar::event( QEvent *event )
{
    if(event->type() == QEvent::MouseButtonDblClick)	userSel();
    else if(event->type() == QEvent::PaletteChange) setUser(user());

    return QLabel::event(event);
}

bool UserStBar::userSel( const string &ihint )
{
    string lang = dynamic_cast<VisRun*>(window()) ? ((VisRun*)window())->lang() : "";
    string hint = (ihint == "$") ? userTxtOrig + ":" + userPassOrig : ihint;

    DlgUser d_usr(user().c_str(), pass().c_str(), VCAStation().c_str(), parentWidget(), hint, lang);

    int rez = (d_usr.autoRes() == DlgUser::NoAuto) ? d_usr.exec() : d_usr.autoRes();

    if(rez == DlgUser::SelOK && d_usr.user().toStdString() != user()) {
	QString old_user = user().c_str(), old_pass = pass().c_str();
	setUser(d_usr.user().toStdString());
	setPass(d_usr.password().toStdString());
	emit userChanged(old_user, old_pass);
	return true;
    }
    else if(rez == DlgUser::SelErr && d_usr.autoRes() == DlgUser::NoAuto)
	mod->postMess(mod->nodePath().c_str(), QString(_("Error authenticating the user '%1'!!!")).arg(d_usr.user()), TVision::Warning, this);

    return false;
}

#undef _
#define _(mess) mod->I18N(mess).c_str()

//*********************************************
//* Font select dialog                        *
//*********************************************
FontDlg::FontDlg( QWidget *parent, const QString &ifnt )
{
    setWindowTitle(_("Selecting a font"));

    QGridLayout *dlg_lay = new QGridLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);

    QLabel *lab = new QLabel(_("Font:"),this);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    dlg_lay->addWidget(lab,0,0,1,2);
    fntSel = new QFontComboBox(this);
    connect(fntSel, SIGNAL(currentFontChanged(const QFont&)), this, SLOT(cfgChange()));
    dlg_lay->addWidget(fntSel,1,0,1,2);

    QVBoxLayout *sz_lay = new QVBoxLayout;
    sz_lay->setSpacing(6);
    lab = new QLabel(_("Size:"),this);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    sz_lay->addWidget(lab);
    spBox = new QSpinBox(this);
    spBox->setValue(10);
    connect(spBox, SIGNAL(valueChanged(int)), this, SLOT(cfgChange()));
    sz_lay->addWidget(spBox);
    QGroupBox *grpBox = new QGroupBox(_("Style:"),this);
    QVBoxLayout *grpLay = new QVBoxLayout;
    grpLay->setMargin(5);
    chBold = new QCheckBox(_("Bold"), this);		grpLay->addWidget(chBold);
    connect(chBold, SIGNAL(stateChanged(int)), this, SLOT(cfgChange()));
    chItalic = new QCheckBox(_("Italic"), this);	grpLay->addWidget(chItalic);
    connect(chItalic, SIGNAL(stateChanged(int)), this, SLOT(cfgChange()));
    chStrike = new QCheckBox(_("Strikeout"), this);	grpLay->addWidget(chStrike);
    connect(chStrike, SIGNAL(stateChanged(int)), this, SLOT(cfgChange()));
    chUnder = new QCheckBox(_("Underline"), this);	grpLay->addWidget(chUnder);
    connect(chUnder, SIGNAL(stateChanged(int)), this, SLOT(cfgChange()));
    grpBox->setLayout(grpLay);
    sz_lay->addWidget(grpBox);
    dlg_lay->addItem(sz_lay,2,0);

    grpBox = new QGroupBox(_("Sample:"),this);
    grpLay = new QVBoxLayout;
    grpLay->setMargin(5);
    sampleText = new QLineEdit(_("AaBbCcDdEeFf"),this);
    sampleText->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    sampleText->setAlignment(Qt::AlignCenter);
    grpLay->addWidget(sampleText);
    grpBox->setLayout(grpLay);
    dlg_lay->addWidget(grpBox,2,1);

    dlg_lay->addItem(new QSpacerItem(20,0,QSizePolicy::Minimum,QSizePolicy::Expanding), 3, 0, 1, 2);

    QFrame *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Raised);
    dlg_lay->addWidget(sep, 4, 0, 1, 2);

    QDialogButtonBox *butBox = new QDialogButtonBox(QDialogButtonBox::Ok|
						     QDialogButtonBox::Cancel, Qt::Horizontal, this);
    QImage ico_t;
    butBox->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
    butBox->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(butBox, SIGNAL(accepted()), this, SLOT(accept()));
    butBox->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
    butBox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    connect(butBox, SIGNAL(rejected()), this, SLOT(reject()));
    dlg_lay->addWidget(butBox, 5, 0, 1, 2);

    setFont(ifnt);
}

QString FontDlg::font( )
{
    return QString("%1 %2 %3 %4 %5 %6").arg(fntSel->currentFont().family().replace(QRegExp(" "),"_")).
					arg(spBox->value()).
					arg(chBold->checkState()?"1":"0").
					arg(chItalic->checkState()?"1":"0").
					arg(chUnder->checkState()?"1":"0").
					arg(chStrike->checkState()?"1":"0");
}

void FontDlg::setFont( const QString &fnt )
{
    char family[101]; strcpy(family,"Arial");
    int size = 10, bold = 0, italic = 0, underline = 0, strike = 0;
    sscanf(fnt.toStdString().c_str(),"%100s %d %d %d %d %d",family,&size,&bold,&italic,&underline,&strike);
    fntSel->setCurrentFont(QFont(QString(family).replace(QRegExp("_")," ")));
    spBox->setValue(size);
    chBold->setCheckState(bold?Qt::Checked:Qt::Unchecked);
    chItalic->setCheckState(italic?Qt::Checked:Qt::Unchecked);
    chStrike->setCheckState(strike?Qt::Checked:Qt::Unchecked);
    chUnder->setCheckState(underline?Qt::Checked:Qt::Unchecked);
}

void FontDlg::cfgChange( )
{
    QFont fnt;
    fnt.setFamily(fntSel->currentFont().family());
    fnt.setPixelSize(spBox->value());
    fnt.setBold(chBold->checkState());
    fnt.setItalic(chItalic->checkState());
    fnt.setUnderline(chUnder->checkState());
    fnt.setStrikeOut(chStrike->checkState());
    sampleText->setFont(fnt);
}

void FontDlg::showEvent( QShowEvent * event )
{
    QSize src = size();
    adjustSize();
    resize(size().expandedTo(src));

#if defined(__ANDROID__)
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2);
#endif
}

//*********************************************************************************************
//* Universal edit line widget. Contain support of: QLineEdit, QSpinBox, QDoubleSpinBox,      *
//* QTimeEdit, QDateEdit and QDateTimeEdit.                                                   *
//*********************************************************************************************
LineEdit::LineEdit( QWidget *parent, LType tp, bool prev_dis, bool resApply ) :
    QWidget(parent), m_tp((LineEdit::LType)-1), mPrev(!prev_dis), applyReserve(resApply), mIsEdited(false),
    edFld(NULL), btFld(NULL), btTm(NULL)
{
    QHBoxLayout *box = new QHBoxLayout(this);
    box->setMargin(0);
    box->setSpacing(0);

    if(resApply) {
	btTm = new QTimer(this);
	connect(btTm, SIGNAL(timeout()), this, SLOT(cancelSlot()));
    }

    setType(tp);
}

void LineEdit::viewApplyBt( bool view )
{
    if(view == (bool)btFld) return;

    if(view && !btFld) {
	btFld = new QPushButton(this);
	btFld->setIcon(QIcon(":/images/button_ok.png"));
	btFld->setIconSize(QSize(icoSize(),icoSize()));
	btFld->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	btFld->setMaximumWidth(icoSize(1.2));
	connect(btFld, SIGNAL(clicked()), this, SLOT(applySlot()));
	layout()->addWidget(btFld);
    }
    if(!view && btFld) {
	if(btTm) btTm->stop(); //btTm->deleteLater(); btTm = NULL;
	btFld->deleteLater(); btFld = NULL;
	mIsEdited = false;
    }
}

bool LineEdit::isEdited( )	{ return mIsEdited; }

void LineEdit::setReadOnly( bool val )
{
    if(!edFld)	return;
    switch(type()) {
	case Text: case Password:
	    ((QLineEdit*)edFld)->setReadOnly(val);			break;
	case Integer: case Real: case Time: case Date: case DateTime:
	    ((QAbstractSpinBox*)edFld)->setReadOnly(val);		break;
	case Combo:	((QComboBox*)edFld)->setEnabled(!val);		break;
    }
}

void LineEdit::setType( LType tp )
{
    needReserver = false;
    if(tp == m_tp) return;

    //Delete previous
    if(tp >= 0 && edFld) edFld->deleteLater(); //delete edFld;

    //Create new widget
    switch(tp) {
	case Text: case Password:
	    edFld = new QLineEdit(this);
	    ((QLineEdit*)edFld)->setEchoMode((tp==Password)?QLineEdit::Password:QLineEdit::Normal);
	    connect((QLineEdit*)edFld, SIGNAL(textEdited(const QString&)), SLOT(changed()));
	    break;
	case Integer:
	    edFld = new QSpinBox(this);
	    connect((QSpinBox*)edFld, SIGNAL(valueChanged(int)), SLOT(changed()));
	    if(mPrev) needReserver = true;
	    break;
	case Real:
	    edFld = new QDoubleSpinBox(this);
	    connect((QDoubleSpinBox*)edFld, SIGNAL(valueChanged(double)), SLOT(changed()));
	    if(mPrev) needReserver = true;
	    break;
	case Time:
	    edFld = new QTimeEdit(this);
	    connect((QTimeEdit*)edFld, SIGNAL(timeChanged(const QTime&)), SLOT(changed()));
	    if(mPrev) needReserver = true;
	    break;
	case Date:
	    edFld = new QDateEdit(this);
	    ((QDateEdit*)edFld)->setCalendarPopup(true);
	    ((QDateEdit*)edFld)->calendarWidget()->setGridVisible(true);
	    ((QDateEdit*)edFld)->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	    connect((QDateEdit*)edFld, SIGNAL(dateChanged(const QDate&)), SLOT(changed()));
	    break;
	case DateTime:
	    edFld = new QDateTimeEdit(this);
	    ((QDateEdit*)edFld)->setCalendarPopup(true);
	    ((QDateEdit*)edFld)->calendarWidget()->setGridVisible(true);
	    ((QDateEdit*)edFld)->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	    connect((QDateTimeEdit*)edFld, SIGNAL(dateTimeChanged(const QDateTime&)), SLOT(changed()));
	    break;
	case Combo:
	    edFld = new QComboBox(this);
	    ((QComboBox*)edFld)->setEditable(true);
	    connect((QComboBox*)edFld, SIGNAL(editTextChanged(const QString&)), SLOT(changed()));
	    connect((QComboBox*)edFld, SIGNAL(activated(int)), this, SLOT(applySlot()));
	    break;
    }
    ((QBoxLayout*)layout())->insertWidget(0, edFld);
    if(applyReserve && needReserver) {
	edFld->setMaximumWidth(width()-icoSize(1.2)); edFld->setMinimumWidth(width()-icoSize(1.2));
	((QBoxLayout*)layout())->setAlignment(edFld, Qt::AlignLeft);
    }
    setFocusProxy(edFld);

    m_tp = tp;
}

void LineEdit::changed( )
{
    //Enable apply
    if(mPrev && !btFld) viewApplyBt(true);
    if(btTm) btTm->start(mPrev ? 10000 : 500);
    mIsEdited = true;

    emit valChanged(value());
}

void LineEdit::setValue( const QString &txt )
{
    if(!isEdited()) {
	if(edFld) edFld->blockSignals(true);
	switch(type()) {
	    case Text: case Password:
		if(txt == value())	break;
		((QLineEdit*)edFld)->setText(txt);
		((QLineEdit*)edFld)->setCursorPosition(0);
		break;
	    case Integer:
		((QSpinBox*)edFld)->setValue(txt.toInt());
		break;
	    case Real:
		((QDoubleSpinBox*)edFld)->setValue(txt.toDouble());
		break;
	    case Time: {
		int secs = txt.toInt();
		((QTimeEdit*)edFld)->setTime(QTime(secs/3600,(secs/60)%60,secs%60)/*.addSecs(txt.toInt())*/);
		break;
	    }
	    case Date: case DateTime:
		if(((QDateTimeEdit*)edFld)->calendarWidget() && ((QDateTimeEdit*)edFld)->calendarWidget()->isVisible()) break;
		((QDateTimeEdit*)edFld)->setDateTime(QDateTime::fromTime_t(txt.toInt()));
		break;
	    case Combo:
		if(((QComboBox*)edFld)->findText(txt) < 0) ((QComboBox*)edFld)->addItem(txt);
		if(txt != value()) {
		    ((QComboBox*)edFld)->setEditText(txt);
		    ((QComboBox*)edFld)->setCurrentIndex(((QComboBox*)edFld)->findText(txt));
		}
		break;
	}
	if(edFld) edFld->blockSignals(false);
    }

    m_val = txt;

    //if(btFld) viewApplyBt(false);
}

void LineEdit::setCfg( const QString &cfg )
{
    if(edFld) edFld->blockSignals(true);
    switch(type()) {
	case Text:
	    ((QLineEdit*)edFld)->setInputMask(cfg);
	    break;
	case Integer: {
	    int		minv = 0, maxv = 100, sstep = 1;
	    string	pref, suff;
	    if(!cfg.isEmpty()) {
		minv  = s2i(TSYS::strSepParse(cfg.toStdString(),0,':'));
		maxv  = s2i(TSYS::strSepParse(cfg.toStdString(),1,':'));
		sstep = s2i(TSYS::strSepParse(cfg.toStdString(),2,':'));
		pref  = TSYS::strSepParse(cfg.toStdString(),3,':');
		suff  = TSYS::strSepParse(cfg.toStdString(),4,':');
	    }
	    ((QSpinBox*)edFld)->setRange(minv,maxv);
	    ((QSpinBox*)edFld)->setSingleStep(sstep);
	    ((QSpinBox*)edFld)->setPrefix(pref.c_str());
	    ((QSpinBox*)edFld)->setSuffix(suff.c_str());
	    break;
	}
	case Real: {
	    double minv = 0, maxv = 100, sstep = 1;
	    string pref, suff;
	    int dec = 2;
	    if(!cfg.isEmpty()) {
		minv  = s2r(TSYS::strSepParse(cfg.toStdString(),0,':'));
		maxv  = s2r(TSYS::strSepParse(cfg.toStdString(),1,':'));
		sstep = s2r(TSYS::strSepParse(cfg.toStdString(),2,':'));
		pref  = TSYS::strSepParse(cfg.toStdString(),3,':');
		suff  = TSYS::strSepParse(cfg.toStdString(),4,':');
		dec   = s2i(TSYS::strSepParse(cfg.toStdString(),5,':'));
	    }
	    ((QDoubleSpinBox*)edFld)->setRange(minv,maxv);
	    ((QDoubleSpinBox*)edFld)->setSingleStep(sstep);
	    ((QDoubleSpinBox*)edFld)->setPrefix(pref.c_str());
	    ((QDoubleSpinBox*)edFld)->setSuffix(suff.c_str());
	    ((QDoubleSpinBox*)edFld)->setDecimals(dec);
	    break;
	}
	case Time: case Date: case DateTime:
	    ((QDateTimeEdit*)edFld)->setDisplayFormat(cfg);
	    break;
	case Combo: {
	    QString ctext = ((QComboBox*)edFld)->currentText();
	    ((QComboBox*)edFld)->clear();
	    ((QComboBox*)edFld)->addItems(cfg.split("\n"));
	    if(!ctext.isEmpty()) {
		if(((QComboBox*)edFld)->findText(ctext) < 0) ((QComboBox*)edFld)->addItem(ctext);
		((QComboBox*)edFld)->setEditText(ctext);
	    }
	    if(((QComboBox*)edFld)->completer()) ((QComboBox*)edFld)->completer()->setCaseSensitivity(Qt::CaseSensitive);
	    break;
	}
	default: break;
    }
    if(btFld) viewApplyBt(false);
    if(edFld) edFld->blockSignals(false);
}

void LineEdit::setFont( const QFont &f )
{
    if(!workWdg()) return;
    workWdg()->setFont(f);
    switch(type()) {
	case Combo: {
	    QList<QLineEdit*> lnEdWs = workWdg()->findChildren<QLineEdit*>();
	    if(lnEdWs.size()) lnEdWs[0]->setFont(f);
	    break;
	}
	default: break;
    }
}

QString LineEdit::value( )
{
    switch(type()) {
	case Text: case Password:
			return ((QLineEdit*)edFld)->text();
	case Integer:	return QString::number(((QSpinBox*)edFld)->value());
	case Real:	return QString::number(((QDoubleSpinBox*)edFld)->value());
	case Time: {
	    QTime tm = ((QTimeEdit*)edFld)->time();
	    return QString::number(tm.hour()*3600 + tm.minute()*60 + tm.second() /*QTime().secsTo(((QTimeEdit*)edFld)->time())*/);
	}
	case Date: case DateTime:
			return QString::number(((QDateTimeEdit*)edFld)->dateTime().toTime_t());
	case Combo:	return ((QComboBox*)edFld)->currentText();
    }
    return "";
}

void LineEdit::applySlot( )
{
    viewApplyBt(false);

    if(m_val == value()) return;
    m_val = value();

    emit apply();
}

void LineEdit::cancelSlot( )
{
    viewApplyBt(false);
    //mIsEdited = false;

    if(mPrev) {
	setValue(m_val);
	emit cancel();
    }
    else applySlot();
}

bool LineEdit::event( QEvent * e )
{
    if(e->type() == QEvent::KeyRelease && btFld) {
	QKeyEvent *keyEvent = (QKeyEvent*)e;
	if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
	    btFld->animateClick();
	    return true;
	}
	else if(keyEvent->key() == Qt::Key_Escape )	{ cancelSlot(); return true; }
    }
    else if(e->type() == QEvent::Resize && applyReserve && needReserver) {
	int btW = btFld ? btFld->width() : icoSize(1.2);
	edFld->setMaximumWidth(width()-btW);
	edFld->setMinimumWidth(width()-btW);
    }

    return QWidget::event(e);
}

#undef _
#define _(mess) mod->I18N(mess, lang.c_str()).c_str()

//*************************************************
//* Text edit widget                              *
//*************************************************
TextEdit::TextEdit( QWidget *parent, bool prev_dis ) :
    QWidget(parent), isInit(false), sntHgl(NULL), butBox(NULL), stWin(NULL)
{
    lang = dynamic_cast<VisRun*>(window()) ? ((VisRun*)window())->lang() : "";

    QVBoxLayout *box = new QVBoxLayout(this);
    box->setMargin(0);
    box->setSpacing(0);

    edFld = new QTextEdit(this);
    edFld->setContextMenuPolicy(Qt::CustomContextMenu);
#if QT_VERSION >= 0x050A00
    edFld->setTabStopDistance(20);
#else
    edFld->setTabStopWidth(20);
#endif
    edFld->setAcceptRichText(false);
    edFld->setLineWrapMode(QTextEdit::NoWrap);
    setFocusProxy(edFld);
    connect(edFld, SIGNAL(textChanged()), this, SLOT(changed()));
    connect(edFld, SIGNAL(cursorPositionChanged()), this, SLOT(curPosChange()) );
    connect(edFld, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(custContextMenu()));
    box->addWidget(edFld);

    QImage ico_t;
    if(!ico_t.load(TUIS::icoGet("find",NULL,true).c_str())) ico_t.load(":/images/find.png");
    actFind = new QAction(QPixmap::fromImage(ico_t), _("Find"), edFld);
    actFind->setShortcut(Qt::CTRL+Qt::Key_F);
    actFind->setShortcutContext(Qt::WidgetShortcut);
    connect(actFind, SIGNAL(triggered()), this, SLOT(find()));
    edFld->addAction(actFind);
    actFindNext = new QAction(_("Find next"), edFld);
    actFindNext->setShortcut(Qt::Key_F3);
    actFindNext->setShortcutContext(Qt::WidgetShortcut);
    connect(actFindNext, SIGNAL(triggered()), this, SLOT(find()));
    edFld->addAction(actFindNext);

    if(!prev_dis) {
	butBox = new QDialogButtonBox(QDialogButtonBox::Apply|QDialogButtonBox::Cancel,Qt::Horizontal,this);
	QImage ico_t;
	butBox->button(QDialogButtonBox::Apply)->setText("");
	if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
	butBox->button(QDialogButtonBox::Apply)->setIcon(QPixmap::fromImage(ico_t));
	butBox->button(QDialogButtonBox::Apply)->setIconSize(QSize(icoSize(),icoSize()));
	connect(butBox->button(QDialogButtonBox::Apply), SIGNAL(pressed()), this, SLOT(applySlot()));
	butBox->button(QDialogButtonBox::Cancel)->setText("");
	if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
	butBox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
	butBox->button(QDialogButtonBox::Cancel)->setIconSize(QSize(icoSize(),icoSize()));
	connect(butBox->button(QDialogButtonBox::Cancel), SIGNAL(pressed()), this, SLOT(cancelSlot()));
	butBox->setVisible(false);
	butBox->setEnabled(false);
	box->addWidget(butBox);
    }

    //Check for window with status present
    QWidget *w = parentWidget();
    while(w && w->parentWidget() && (!dynamic_cast<QMainWindow *>(w) || !((QMainWindow*)w)->statusBar())) w = w->parentWidget();
    stWin = dynamic_cast<QMainWindow *>(w);

    btTm = new QTimer(this);
    connect(btTm, SIGNAL(timeout()), this, SLOT(applySlot()));
}

bool TextEdit::isEdited( )	{ return (butBox && butBox->isEnabled()); }	//isVisible() sometime wrong but it can be hidden commonly

QString TextEdit::text( )	{ return edFld->toPlainText(); }

void TextEdit::setText( const QString &itext )
{
    isInit = true;
    if(itext != text()) {
	if(!sntHgl || sntHgl->isBuiltInSH) {	//Try the builtin syntax higlihgt
	    XMLNode hglO("SnthHgl");
	    if(SnthHgl::checkInSnthHgl(itext,hglO)) { setSnthHgl(hglO); sntHgl->isBuiltInSH = true; }
	}

	edFld->setPlainText(itext);
    }
    if(butBox && butBox->isEnabled()) {
	butBox->setVisible(false);
	butBox->setEnabled(false);
    }
    //if(!itext.size() && sntHgl) { delete sntHgl; sntHgl = NULL; }	//!!!! Otherwise all empty text fields will be without the syntax highlighting
    isInit = false;

    mText = itext;
}

void TextEdit::setSnthHgl( XMLNode nd )
{
    if(!sntHgl) sntHgl = new SnthHgl(edFld->document());
    sntHgl->setSnthHgl(nd);
}

void TextEdit::changed( )
{
    if(isInit) return;
    if(butBox && !butBox->isEnabled() && text() != mText) {
	butBox->setVisible(true);
	butBox->setEnabled(true);

	string labApply = _("Apply"), labCncl = _("Cancel");
	bool noLab = (QFontMetrics(butBox->font()).size(Qt::TextSingleLine,(labApply+labCncl).c_str()).width()+30) > width();
	butBox->button(QDialogButtonBox::Apply)->setText(noLab?"":labApply.c_str());
	butBox->button(QDialogButtonBox::Cancel)->setText(noLab?"":labCncl.c_str());
    }

    if(!butBox) btTm->start(500);

    if(text() != mText) emit textChanged(text());
}

void TextEdit::applySlot( )
{
    if(butBox && butBox->isEnabled()) {
	butBox->setVisible(false);
	butBox->setEnabled(false);
    }

    if(mText == text()) return;

    mText = text();

    emit apply();

    //Try the builtin syntax higlihgt
    if(!sntHgl || sntHgl->isBuiltInSH) {
	XMLNode hglO("SnthHgl");
	if(SnthHgl::checkInSnthHgl(text(),hglO) && (!sntHgl || sntHgl->snthHgl().save() != hglO.save())) {
	    setSnthHgl(hglO);
	    sntHgl->isBuiltInSH = true;

	    //Activation the syntax rules change
	    int cursorPos = edFld->textCursor().position();
	    int scrollVPos = edFld->verticalScrollBar()->value();
	    int scrollHPos = edFld->horizontalScrollBar()->value();

	    edFld->blockSignals(true);
	    edFld->setPlainText(text());
	    edFld->blockSignals(false);

	    //Cursor position restore
	    QTextCursor tCur = edFld->textCursor(); tCur.setPosition(cursorPos);
	    edFld->setTextCursor(tCur); edFld->ensureCursorVisible();
	    edFld->verticalScrollBar()->setValue(scrollVPos);
	    edFld->horizontalScrollBar()->setValue(scrollHPos);
	}
    }
}

void TextEdit::cancelSlot( )
{
    int edPosSave = text().size() ? workWdg()->textCursor().position() : -1;
    setText(mText);
    if(edPosSave >= 0 && text().size()) {
	QTextCursor tCur = workWdg()->textCursor(); tCur.setPosition(edPosSave);
	workWdg()->setTextCursor(tCur); workWdg()->ensureCursorVisible();
    }

    emit cancel();
}

void TextEdit::curPosChange( )
{
    QStatusBar *stBar = window()->findChild<QStatusBar*>();
    if(!stBar && stWin) stBar = stWin->statusBar();
    if(stBar) stBar->showMessage(QString(_("Cursor = (%1:%2)")).arg(edFld->textCursor().blockNumber()+1).arg(edFld->textCursor().columnNumber()+1), 10000);
}

bool TextEdit::event( QEvent * e )
{
    if(butBox && e->type() == QEvent::KeyRelease) {
	QKeyEvent *keyEvent = (QKeyEvent *)e;
	if((keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) && QApplication::keyboardModifiers()&Qt::ControlModifier) {
	    butBox->button(QDialogButtonBox::Apply)->animateClick();
	    return true;
	}
	else if(keyEvent->key() == Qt::Key_Escape) {
	    butBox->button(QDialogButtonBox::Cancel)->animateClick();
	    return true;
	}
    }

    return QWidget::event(e);
}

void TextEdit::custContextMenu( )
{
    QMenu *menu = edFld->createStandardContextMenu();
    menu->addSeparator();
    menu->addAction(actFind);
    menu->addAction(actFindNext);
    menu->exec(QCursor::pos());
    menu->deleteLater(); //delete menu;
}

void TextEdit::find( )
{
    bool isFind = false;
    int fopt = (QTextDocument::FindFlag)actFind->objectName().section(':',false,false).toInt();
    QString fstr = actFind->objectName().section(':',1);
    if(sender() == actFind) {
	InputDlg dlg(this,actFind->icon(),QString(_("Enter a string to search:")),_("Search string"),0,0);
	QLineEdit *le = new QLineEdit(fstr,&dlg);
	dlg.edLay()->addWidget(le, 0, 0);
	QCheckBox *bw = new QCheckBox(_("Backward"),&dlg);
	if(fopt & QTextDocument::FindBackward) bw->setCheckState(Qt::Checked);
	dlg.edLay()->addWidget(bw, 1, 0);
	QCheckBox *cs = new QCheckBox(_("Case sensitively"),&dlg);
	if(fopt & QTextDocument::FindCaseSensitively) cs->setCheckState(Qt::Checked);
	dlg.edLay()->addWidget(cs, 2, 0);
	QCheckBox *ww = new QCheckBox(_("Whole words"),&dlg);
	if(fopt & QTextDocument::FindWholeWords) ww->setCheckState(Qt::Checked);
	dlg.edLay()->addWidget(ww, 3, 0);
	le->setFocus(Qt::OtherFocusReason);
	dlg.resize(400,210);
	if(dlg.exec() == QDialog::Accepted && !le->text().isEmpty()) {
	    fopt = (QTextDocument::FindFlag)0;
	    if(bw->checkState()==Qt::Checked) fopt |= QTextDocument::FindBackward;
	    if(cs->checkState()==Qt::Checked) fopt |= QTextDocument::FindCaseSensitively;
	    if(ww->checkState()==Qt::Checked) fopt |= QTextDocument::FindWholeWords;
	    fstr = le->text();
	    isFind = true;
	}
    }
    else if(sender() == actFindNext && !fstr.isEmpty()) isFind = true;

    if(isFind) {
	edFld->find(fstr,(QTextDocument::FindFlag)fopt);
	actFind->setObjectName(QString::number(fopt)+":"+fstr);
    }
}

//*********************************************
//* TreeView item delegate for ComboBox.      *
//*********************************************
/*TreeComboDelegate::TreeComboDelegate( QObject *parent ) : QItemDelegate(parent)
{

}

void TreeComboDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    if(index.isValid() && index.column() == 0)
    {
	QStyleOptionViewItem opt = option;
	opt.font = index.data(Qt::FontRole).value<QFont>();
        drawDisplay(painter, opt, opt.rect, index.data(Qt::UserRole).toString());
        return;
    }
    QItemDelegate::paint(painter, option, index);
}*/

#undef _
#define _(mess) mod->I18N(mess).c_str()

//****************************************
//* Shape widget view                    *
//****************************************
WdgView::WdgView( const string &iwid, int ilevel, QMainWindow *mainWind, QWidget *parent, Qt::WindowFlags f ) :
    QWidget(parent,f), shpData(NULL), isReload(false), mAllAttrLoad(false), mWLevel(ilevel),
    x_scale(1.0), y_scale(1.0), z_coord(0), idWidget(iwid), shape(NULL), main_win(mainWind)
{
    //setAttribute(Qt::WA_OpaquePaintEvent,true);
}

WdgView::~WdgView( )
{
    childsClear();

    if(shape) shape->destroy(this);
}

void WdgView::childsClear( )
{
    //Child widgets remove before
    QObjectList chLst = children();
    for(int iC = 0; iC < chLst.size(); iC++) {
	WdgView *cw = qobject_cast<WdgView*>(chLst[iC]);
	if(cw)	delete cw;//cw->deleteLater(); //!!!! Direct deleting due to this step is last one mostly
    }
}

float WdgView::xScale( bool full )	{ return (full && wLevel() > 0) ? x_scale*((WdgView*)parentWidget())->xScale(full) : x_scale; }

float WdgView::yScale( bool full )	{ return (full && wLevel() > 0) ? y_scale*((WdgView*)parentWidget())->yScale(full) : y_scale; }

string WdgView::root( )	{ return shape ? shape->id() : ""; }

void WdgView::moveF( const QPointF &pos )
{
    mWPos = pos;
    move(pos.toPoint());	//Equal to rRnd()
}

void WdgView::resizeF( const QSizeF &isz )
{
    mWSize = isz;
    mWSize.setWidth(vmax(mWSize.width(),3));
    mWSize.setHeight(vmax(mWSize.height(),3));
    resize(rRnd(posF().x()+sizeF().width()-xScale(true))-rRnd(posF().x())+1,
	rRnd(posF().y()+sizeF().height()-yScale(true))-rRnd(posF().y())+1);
}

WdgView *WdgView::newWdgItem( const string &iwid )	{ return new WdgView(iwid,wLevel()+1,mainWin(),this); }

bool WdgView::attrSet( const string &attr, const string &val, int uiPrmPos, bool toModel )
{
    //Send value to model
    if(!attr.empty() && toModel) {
	XMLNode req("set");
	req.setAttr("path", id()+"/%2fserv%2fattr");
	req.childAdd("el")->setAttr("id", attr)->setText(val);
	cntrIfCmd(req);
    }
    bool up = false, upChlds = false;

    switch(uiPrmPos) {
	case A_COM_LOAD: up = true;	break;
	//case A_NO_ID:	return false;
	case A_ROOT:
	    if(shape && shape->id() == val)	break;
	    if(shape) {
		shape->destroy(this);
		// Cleaning up all child objects after
		while(children().size()) delete children()[0];
	    }
	    shape = mod->getWdgShape(val);
	    if(shape) shape->init(this);
	    break;
	case A_GEOM_X:
	    //if(wLevel() == 0)	break;
	    if(wLevel() == 0) mWPos = QPointF(s2r(val),posF().y());
	    else mWPos = QPointF(((WdgView*)parentWidget())->xScale(true)*s2r(val), posF().y());
	    up = true;
	    break;
	case A_GEOM_Y:
	    //if(wLevel() == 0)	break;
	    if(wLevel() == 0) mWPos = QPointF(posF().x(),s2r(val));
	    else mWPos = QPointF(posF().x(), ((WdgView*)parentWidget())->yScale(true)*s2r(val));
	    up = true;
	    break;
	case A_GEOM_W:
	    mWSizeOrig = QSizeF(s2r(val), sizeOrigF().height());
	    mWSize = QSizeF(xScale(true)*s2r(val), sizeF().height());
	    up = true;
	    break;
	case A_GEOM_H:
	    mWSizeOrig = QSizeF(sizeOrigF().width(), s2r(val));
	    mWSize = QSizeF(sizeF().width(), yScale(true)*s2r(val));
	    up = true;
	    break;
	case A_GEOM_Z: if(wLevel() > 0) z_coord = s2i(val);	break;
	case A_GEOM_X_SC:
	    mWSize = QSizeF((s2r(val)/x_scale)*sizeF().width(), sizeF().height());
	    x_scale = s2r(val);
	    up = upChlds = true;
	    break;
	case A_GEOM_Y_SC:
	    mWSize = QSizeF(sizeF().width(), (s2r(val)/y_scale)*sizeF().height());
	    y_scale = s2r(val);
	    up = upChlds = true;
	    break;
	case A_TIP_TOOL: setToolTip(val.c_str());	break;
	case A_TIP_STATUS: setStatusTip(val.c_str());	break;
    }
    if(up && !allAttrLoad()) {
	if(wLevel() > 0) moveF(posF());
	resizeF(sizeF());
	for(int iC = 0; upChlds && iC < children().size(); iC++)
	    if(qobject_cast<WdgView*>(children().at(iC)))
		((WdgView*)children().at(iC))->load("");
    }

    if(shape)	return shape->attrSet(this, uiPrmPos, val, attr);

    return true;
}

void WdgView::attrsSet( AttrValS &attrs )
{
    XMLNode req("set");
    req.setAttr("path", id()+"/%2fserv%2fattr");
    string attrId, attrPos;
    for(AttrValS::iterator iA = attrs.begin(); iA != attrs.end(); iA++) {
	int off = 0;
	attrId = TSYS::strParse(iA->first, 0, ":", &off);
	attrPos = TSYS::strParse(iA->first, 0, ":", &off);
	if(!attrId.empty())	req.childAdd("el")->setAttr("id",attrId)->setText(iA->second);
	if(!attrPos.empty())	attrSet("", iA->second, s2i(attrPos));
    }
    if(req.childSize())	cntrIfCmd(req);
}

string WdgView::resGet( const string &res )
{
    if(res.empty())	return "";

    XMLNode req("get");
    req.setAttr("path", id()+"/%2fwdg%2fres")->setAttr("id", res);
    if(!cntrIfCmd(req))	return TSYS::strDecode(req.text(), TSYS::base64);

    return "";
}

void WdgView::load( const string& item, bool isLoad, bool isInit, XMLNode *aBr )
{
    int64_t d_cnt = 0;
    if(mess_lev() == TMess::Debug && wLevel() == 0) d_cnt = TSYS::curTime();

    isReload = shape;

    //Load from the data model
    if(isLoad) {
	bool reqBrCr = false;
	if(!aBr) {
	    aBr = new XMLNode("get");
	    aBr->setAttr("path", id()+"/%2fserv%2fattrBr");
	    cntrIfCmd(*aBr);
	    reqBrCr = true;
	    if(mess_lev() == TMess::Debug)
		mess_debug(mod->nodePath().c_str(), _("Time of requesting the VCA engine '%s': %f ms."), id().c_str(), 1e-3*(TSYS::curTime()-d_cnt));
	}

	setAllAttrLoad(true);
	if(item.empty() || item == id())
	    for(unsigned iEl = 0; iEl < aBr->childSize(); iEl++) {
		XMLNode *cN = aBr->childGet(iEl);
		if(cN->name() == "el") attrSet(cN->attr("id"), cN->text(), s2i(cN->attr("p")));
	    }
	setAllAttrLoad(false);

	// Delete child widgets
	string b_nm = aBr->attr("lnkPath");
	if(b_nm.empty()) b_nm = id();
	for(int iC = 0, iL = 0; iC < children().size(); iC++) {
	    if(!qobject_cast<WdgView*>(children().at(iC))) continue;
	    for(iL = 0; iL < (int)aBr->childSize(); iL++)
		if(aBr->childGet(iL)->name() == "w" &&
			qobject_cast<WdgView*>(children().at(iC))->id() == (b_nm+"/wdg_"+aBr->childGet(iL)->attr("id")))
		    break;
	    if(iL >= (int)aBr->childSize()) children().at(iC)->deleteLater();
	}

	// Create new child widget
	for(int iL = 0, iC = 0; iL < (int)aBr->childSize(); iL++) {
	    if(aBr->childGet(iL)->name() != "w") continue;
	    for(iC = 0; iC < children().size(); iC++)
		if(qobject_cast<WdgView*>(children().at(iC)) &&
			qobject_cast<WdgView*>(children().at(iC))->id() == (b_nm+"/wdg_"+aBr->childGet(iL)->attr("id")))
		{
		    ((WdgView*)children().at(iC))->load((item==id())?"":item,true,(wLevel()>0)?isInit:false,aBr->childGet(iL));
		    break;
		}
	    if(iC < children().size()) continue;
	    WdgView *nwdg = newWdgItem(b_nm+"/wdg_"+aBr->childGet(iL)->attr("id"));
	    nwdg->show();
	    nwdg->load((item==id())?"":item,true,(wLevel()>0)?isInit:false,aBr->childGet(iL));
	}

	// Children widgets order update
	orderUpdate();

	if(reqBrCr) delete aBr;
    }
    //Going to children init
    else
	for(int iC = 0; iC < children().size(); iC++) {
	    WdgView *wdg = qobject_cast<WdgView*>(children().at(iC));
	    if(wdg && (item.empty() || item == id() || wdg->id() == item.substr(0,wdg->id().size())))
		wdg->load((item==id())?"":item,false,(wLevel()>0)?isInit:false);
	}

    //Init loaded data
    if(isInit && (item.empty() || item == id()) && wLevel() > 0) attrSet("", "load", A_COM_LOAD);

    //Post load init for root widget
    if(wLevel() == 0) {
	if(mess_lev() == TMess::Debug) {
	    mess_debug(mod->nodePath().c_str(), _("Time of loading '%s': %f ms."), id().c_str(), 1e-3*(TSYS::curTime()-d_cnt));
	    d_cnt = TSYS::curTime();
	}

	attrSet("", "load", A_COM_LOAD);
	for(int iC = 0; iC < children().size(); iC++) {
	    WdgView *wdg = qobject_cast<WdgView*>(children().at(iC));
	    if(wdg && (item.empty() || item == id() || wdg->id() == item.substr(0,wdg->id().size())))
		wdg->load((item==id())?"":item,false,true);
	}
	update();
	//repaint();

	if(mess_lev() == TMess::Debug)
	    mess_debug(mod->nodePath().c_str(), _("Time of initiating '%s': %f ms."), id().c_str(), 1e-3*(TSYS::curTime()-d_cnt));
    }
}

void WdgView::orderUpdate( )
{
    QObjectList &ols = d_ptr->children;

    vector< pair<int,QObject*> > arr;
    arr.reserve(children().size());
    int needForSort = 0;
    int sysLev = 1000001;
    for(int iC = 0; iC < ols.size(); iC++) {
	WdgView *cw = qobject_cast<WdgView*>(ols[iC]);
	if(cw) needForSort++;
	arr.push_back(pair<int,QObject*>((cw?cw->z():sysLev),ols[iC]));
    }
    if(!needForSort) return;

    /*make_heap(arr.begin(),arr.end());
    sort_heap(arr.begin(),arr.end());*/
    sort(arr.begin(), arr.end());
    if(ols.size() && ols.size() == (int)arr.size()) {
	bool endIt = false;
	vector< pair<string,QObject*> > arrAftSrt;
	for(int iC = 0, iZ = 0, zPrev = arr[iZ].first; iC < ols.size(); ) {
	    ols[iC] = arr[iC].second;
	    ++iC;

	    // After sort
	    if((endIt=(iC>=ols.size())) || arr[iC].first != zPrev) {
		if((iC-iZ) >= 2 && zPrev != sysLev) {	//Resort
		    arrAftSrt.clear();
		    for(int iC1 = iZ; iC1 < iC; ++iC1)
			arrAftSrt.push_back(pair<string,QObject*>(((WdgView*)ols[iC1])->id(), ols[iC1]));
		    sort(arrAftSrt.begin(), arrAftSrt.end());
		    for(int iC1 = iZ, iA = 0; iC1 < iC; ++iC1, ++iA)
			ols[iC1] = arrAftSrt[iA].second;
		}
		if(!endIt) {
		    iZ = iC;
		    zPrev = arr[iC].first;
		}
	    }
	}
    }
}

bool WdgView::event( QEvent *event )
{
    //Paint event process
    if(event->type() == QEvent::Paint) return shape ? shape->event(this, event) : true;

    return false; //QWidget::event(event);
}

bool WdgView::eventFilter( QObject *object, QEvent *event )	{ return shape ? shape->eventFilter(this,object,event) : false; }
