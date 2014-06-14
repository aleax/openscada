
//OpenSCADA system module UI.Vision file: vis_widgs.cpp
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

#include <tsys.h>

#include "../VCAEngine/types.h"
#include "vis_shapes.h"
#include "vis_widgs.h"
#include "vis_run_widgs.h"

using namespace VISION;
using namespace VCA;

//*************************************************
//* Id and name input dialog                      *
//*************************************************
InputDlg::InputDlg( QWidget *parent, const QIcon &icon, const QString &mess, const QString &ndlg, bool with_id, bool with_nm ) :
	QDialog(parent), m_id(NULL), m_name(NULL)
{
    setMaximumSize(800,600);
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
    icon_lab->setSizePolicy( QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum) );
    icon_lab->setPixmap(icon.pixmap(48));
    intr_lay->addWidget(icon_lab);

    inp_lab = new QLabel(mess,this);
    //inp_lab->setAlignment(Qt::AlignHCenter);
    inp_lab->setWordWrap(true);
    intr_lay->addWidget(inp_lab);
    dlg_lay->addItem(intr_lay);

    ed_lay = new QGridLayout;
    ed_lay->setSpacing(6);
    //Id and name fields
    if( with_id )
    {
	ed_lay->addWidget(new QLabel(_("ID:"),this), 0, 0);
	m_id = new QLineEdit(this);
	ed_lay->addWidget(m_id, 0, 1);
    }
    if( with_nm )
    {
	ed_lay->addWidget(new QLabel(_("Name:"),this), 1, 0);
	m_name = new QLineEdit(this);
	ed_lay->addWidget(m_name, 1, 1);
    }

    ed_lay->addItem(new QSpacerItem(10, 0, QSizePolicy::Minimum, QSizePolicy::Expanding),100,0,1,-1);

    dlg_lay->addItem(ed_lay);

    //Qk and Cancel buttons
    QFrame *sep = new QFrame(this);
    sep->setFrameShape( QFrame::HLine );
    sep->setFrameShadow( QFrame::Raised );
    dlg_lay->addWidget( sep );

    QDialogButtonBox *but_box = new QDialogButtonBox( QDialogButtonBox::Ok|
						      QDialogButtonBox::Cancel, Qt::Horizontal, this );
    QImage ico_t;
    but_box->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
    but_box->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(accepted()), this, SLOT(accept()));
    but_box->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
    but_box->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(rejected()), this, SLOT(reject()));
    dlg_lay->addWidget( but_box );

    resize(400,120+(40*with_nm)+(40*with_id));
}

QString InputDlg::id()
{
    if( m_id )	return m_id->text();
    return "";
}

QString InputDlg::name()
{
    if( m_name )return m_name->text();
    return "";
}

QString InputDlg::mess( )
{
    return inp_lab->text();
}

void InputDlg::setId(const QString &val)
{
    if( m_id )	m_id->setText(val);
}

void InputDlg::setName(const QString &val)
{
    if( m_name )m_name->setText(val);
}

void InputDlg::setMess( const QString &val )
{
    inp_lab->setText( val );
}

void InputDlg::setIdLen( int len )
{
    if( m_id )	m_id->setMaxLength(len);
}

void InputDlg::showEvent( QShowEvent * event )
{
    QSize src = size();
    adjustSize();
    resize(size().expandedTo(src));
}

//*************************************************
//* User select dialog                            *
//*************************************************
DlgUser::DlgUser( const QString &iuser, const QString &ipass, const QString &iVCAstat, QWidget *parent ) :
    QDialog(parent), VCAstat(iVCAstat)
{
    setWindowTitle(_("Select user"));

    QVBoxLayout *dlg_lay = new QVBoxLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);

    QGridLayout *ed_lay = new QGridLayout;
    ed_lay->setSpacing(6);
    ed_lay->addWidget( new QLabel(_("User:"),this), 0, 0 );
    users = new QComboBox(this);
    users->setEditable(true);
    ed_lay->addWidget( users, 0, 1 );
    ed_lay->addWidget( new QLabel(_("Password:"),this), 1, 0 );
    passwd = new QLineEdit(this);
    passwd->setEchoMode(QLineEdit::Password);
    ed_lay->addWidget(passwd, 1, 1);
    dlg_lay->addItem(ed_lay);

    dlg_lay->addItem( new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    QFrame *sep = new QFrame(this);
    sep->setFrameShape( QFrame::HLine );
    sep->setFrameShadow( QFrame::Raised );
    dlg_lay->addWidget( sep );

    QDialogButtonBox *but_box = new QDialogButtonBox( QDialogButtonBox::Ok|
						      QDialogButtonBox::Cancel, Qt::Horizontal, this );
    QImage ico_t;
    but_box->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
    but_box->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(accepted()), this, SLOT(accept()));
    but_box->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
    but_box->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(rejected()), this, SLOT(reject()));
    dlg_lay->addWidget( but_box );

    connect(this, SIGNAL(finished(int)), this, SLOT(finish(int)));

    //Fill users list
    XMLNode req("get");
    req.setAttr("path","/Security/%2fusgr%2fusers");
    if(!mod->cntrIfCmd(req,iuser.toStdString(),ipass.toStdString(),iVCAstat.toStdString(),true))
	for(unsigned i_u = 0; i_u < req.childSize(); i_u++)
	    users->addItem(req.childGet(i_u)->text().c_str());

    users->setEditText(iuser);
}

QString DlgUser::user()
{
    return users->currentText();
}

QString DlgUser::password()
{
    return passwd->text();
}

void DlgUser::finish( int result )
{
    if( result )
    {
	//> Check user auth
	XMLNode req("get");
	req.setAttr("path",string("/Security/")+user().toStdString()+"/%2fauth")->setAttr("password",password().toStdString());
	if(!mod->cntrIfCmd(req,user().toStdString(),password().toStdString(),VCAstat.toStdString(),true) && atoi(req.text().c_str()))
	    setResult(SelOK);
	else setResult(SelErr);
    }
    else setResult(SelCancel);
}

void DlgUser::showEvent( QShowEvent * event )
{
    QSize src = size();
    adjustSize();
    resize(size().expandedTo(src));
}

//*********************************************
//* Font select dialog                        *
//*********************************************
FontDlg::FontDlg( QWidget *parent, const QString &ifnt )
{
    setWindowTitle(_("Font select"));

    QGridLayout *dlg_lay = new QGridLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);

    QLabel *lab = new QLabel(_("Font:"),this);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed) );
    dlg_lay->addWidget(lab,0,0,1,2);
    fntSel = new QFontComboBox(this);
    connect(fntSel, SIGNAL(currentFontChanged(const QFont&)), this, SLOT(cfgChange()));
    dlg_lay->addWidget(fntSel,1,0,1,2);

    QVBoxLayout *sz_lay = new QVBoxLayout;
    sz_lay->setSpacing(6);
    lab = new QLabel(_("Size:"),this);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed) );
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
    sampleText->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred) );
    sampleText->setAlignment(Qt::AlignCenter);
    grpLay->addWidget(sampleText);
    grpBox->setLayout(grpLay);
    dlg_lay->addWidget(grpBox,2,1);

    dlg_lay->addItem( new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ), 3, 0, 1, 2 );

    QFrame *sep = new QFrame(this);
    sep->setFrameShape( QFrame::HLine );
    sep->setFrameShadow( QFrame::Raised );
    dlg_lay->addWidget( sep, 4, 0, 1, 2 );

    QDialogButtonBox *but_box = new QDialogButtonBox( QDialogButtonBox::Ok|
						      QDialogButtonBox::Cancel, Qt::Horizontal, this );
    QImage ico_t;
    but_box->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
    but_box->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(accepted()), this, SLOT(accept()));
    but_box->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
    but_box->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(rejected()), this, SLOT(reject()));
    dlg_lay->addWidget( but_box, 5, 0, 1, 2 );

    setFont(ifnt);
}

QString	FontDlg::font( )
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

void FontDlg::cfgChange()
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
}

//*********************************************
//* Status bar user widget                    *
//*********************************************
UserStBar::UserStBar( const QString &iuser, const QString &ipass, const QString &iVCAstat, QWidget *parent ) : 
    QLabel(parent)
{
    setUser(iuser);
    setPass(ipass);
    setVCAStation(iVCAstat);
}

void UserStBar::setUser( const QString &val )
{
    setText(QString("<font color='%1'>%2</font>").arg((val=="root")?"red":"green").arg(val));
    user_txt = val;
}

bool UserStBar::event( QEvent *event )
{
    if( event->type() == QEvent::MouseButtonDblClick )	userSel();
    return QLabel::event( event );
}

bool UserStBar::userSel()
{
    DlgUser d_usr(user(),pass(),VCAStation(),parentWidget());
    int rez = d_usr.exec();
    if( rez == DlgUser::SelOK && d_usr.user() != user() )
    {
	QString old_user = user(),
		old_pass = pass();
	setUser( d_usr.user() );
	setPass( d_usr.password() );
	emit userChanged( old_user, old_pass );
	return true;
    }
    else if( rez == DlgUser::SelErr )
	mod->postMess(mod->nodePath().c_str(),_("Auth wrong!!!"),TVision::Warning,this);

    return false;
}

//*********************************************************************************************
//* Universal edit line widget. Contain support of: QLineEdit, QSpinBox, QDoubleSpinBox,      *
//* QTimeEdit, QDateEdit and QDateTimeEdit.                                                   *
//*********************************************************************************************
LineEdit::LineEdit( QWidget *parent, LType tp, bool prev_dis, bool resApply ) :
    QWidget(parent), m_tp((LineEdit::LType)-1), mPrev(!prev_dis), applyReserve(resApply), mIsEdited(false),
    ed_fld(NULL), bt_fld(NULL), bt_tm(NULL)
{
    QHBoxLayout *box = new QHBoxLayout(this);
    box->setMargin(0);
    box->setSpacing(0);

    bt_tm = new QTimer(this);
    connect(bt_tm, SIGNAL(timeout()), this, SLOT(cancelSlot()));

    setType(tp);
}

void LineEdit::viewApplyBt( bool view )
{
    if(view == (bool)bt_fld) return;

    if(view && !bt_fld)
    {
	bt_fld = new QPushButton(this);
	bt_fld->setIcon(QIcon(":/images/ok.png"));
	bt_fld->setIconSize(QSize(12,12));
	bt_fld->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	bt_fld->setMaximumWidth(15);
	connect(bt_fld, SIGNAL(clicked()), this, SLOT(applySlot()));
	layout()->addWidget(bt_fld);
    }
    if(!view && bt_fld)
    {
	bt_tm->stop(); //bt_tm->deleteLater(); bt_tm = NULL;
	bt_fld->deleteLater(); bt_fld = NULL;
	mIsEdited = false;
    }
}

bool LineEdit::isEdited( )	{ return mIsEdited; }

void LineEdit::setReadOnly( bool val )
{
    if(!ed_fld)	return;
    switch(type())
    {
	case Text:	((QLineEdit*)ed_fld)->setReadOnly(val);		break;
	case Integer: case Real: case Time: case Date: case DateTime:
	    ((QAbstractSpinBox*)ed_fld)->setReadOnly(val);		break;
	case Combo:	((QComboBox*)ed_fld)->setEnabled(!val);		break;
    }
}

void LineEdit::setType( LType tp )
{
    needReserver = false;
    if(tp == m_tp) return;

    //> Delete previous
    if(tp >= 0 && ed_fld) delete ed_fld;

    //> Create new widget
    switch(tp)
    {
	case Text:
	    ed_fld = new QLineEdit(this);
	    connect((QLineEdit*)ed_fld, SIGNAL(textEdited(const QString&)), SLOT(changed()));
	    break;
	case Integer:
	    ed_fld = new QSpinBox(this);
	    connect((QSpinBox*)ed_fld, SIGNAL(valueChanged(int)), SLOT(changed()));
	    if(mPrev) needReserver = true;
	    break;
	case Real:
	    ed_fld = new QDoubleSpinBox(this);
	    connect((QDoubleSpinBox*)ed_fld, SIGNAL(valueChanged(double)), SLOT(changed()));
	    if(mPrev) needReserver = true;
	    break;
	case Time:
	    ed_fld = new QTimeEdit(this);
	    connect((QTimeEdit*)ed_fld, SIGNAL(timeChanged(const QTime&)), SLOT(changed()));
	    if(mPrev) needReserver = true;
	    break;
	case Date:
	    ed_fld = new QDateEdit(this);
	    ((QDateEdit*)ed_fld)->setCalendarPopup(true);
	    ((QDateEdit*)ed_fld)->calendarWidget()->setGridVisible(true);
	    ((QDateEdit*)ed_fld)->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	    connect((QDateEdit*)ed_fld, SIGNAL(dateChanged(const QDate&)), SLOT(changed()));
	    break;
	case DateTime:
	    ed_fld = new QDateTimeEdit(this);
	    ((QDateEdit*)ed_fld)->setCalendarPopup(true);
	    ((QDateEdit*)ed_fld)->calendarWidget()->setGridVisible(true);
	    ((QDateEdit*)ed_fld)->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	    connect((QDateTimeEdit*)ed_fld, SIGNAL(dateTimeChanged(const QDateTime&)), SLOT(changed()));
	    break;
	case Combo:
	    ed_fld = new QComboBox(this);
	    ((QComboBox*)ed_fld)->setEditable(true);
	    connect((QComboBox*)ed_fld, SIGNAL(editTextChanged(const QString&)), SLOT(changed()));
	    connect((QComboBox*)ed_fld, SIGNAL(activated(int)), this, SLOT(applySlot()));
	    break;
    }
    ((QBoxLayout*)layout())->insertWidget(0, ed_fld);
    if(applyReserve && needReserver)
    {
	ed_fld->setMaximumWidth(width()-12); ed_fld->setMinimumWidth(width()-12);
	((QBoxLayout*)layout())->setAlignment(ed_fld, Qt::AlignLeft);
    }
    setFocusProxy(ed_fld);

    m_tp = tp;
}

void LineEdit::changed( )
{
    //> Enable apply
    if(mPrev && !bt_fld) viewApplyBt(true);
    bt_tm->start(mPrev ? 5000 : 500);
    mIsEdited = true;

    emit valChanged(value());
}

void LineEdit::setValue( const QString &txt )
{
    if(ed_fld) ed_fld->blockSignals(true);
    switch(type())
    {
	case Text:
	    if(txt != value())
	    {
		((QLineEdit*)ed_fld)->setText(txt);
		((QLineEdit*)ed_fld)->setCursorPosition(0);
	    }
	    break;
	case Integer:
	    ((QSpinBox*)ed_fld)->setValue(txt.toInt());
	    break;
	case Real:
	    ((QDoubleSpinBox*)ed_fld)->setValue(txt.toDouble());
	    break;
	case Time:
	    ((QTimeEdit*)ed_fld)->setTime(QTime().addSecs(txt.toInt()));
	    break;
	case Date: case DateTime:
	    if(((QDateTimeEdit*)ed_fld)->calendarWidget()->isVisible()) break;
	    ((QDateTimeEdit*)ed_fld)->setDateTime(QDateTime::fromTime_t(txt.toInt()));
	    break;
	case Combo:
	    if(((QComboBox*)ed_fld)->findText(txt) < 0) ((QComboBox*)ed_fld)->addItem(txt);
	    if(txt != value())	((QComboBox*)ed_fld)->setEditText(txt);
	    break;
    }
    if(ed_fld) ed_fld->blockSignals(false);

    m_val = txt;

    if(bt_fld) viewApplyBt(false);
}

void LineEdit::setCfg(const QString &cfg)
{
    switch(type())
    {
	case Text:	((QLineEdit*)ed_fld)->setInputMask(cfg);	break;
	case Integer:
	{
	    int		minv = 0, maxv = 100, sstep = 1;
	    string	pref, suff;
	    if( !cfg.isEmpty() )
	    {
		minv  = atoi(TSYS::strSepParse(cfg.toStdString(),0,':').c_str());
		maxv  = atoi(TSYS::strSepParse(cfg.toStdString(),1,':').c_str());
		sstep = atoi(TSYS::strSepParse(cfg.toStdString(),2,':').c_str());
		pref  = TSYS::strSepParse(cfg.toStdString(),3,':');
		suff  = TSYS::strSepParse(cfg.toStdString(),4,':');
	    }
	    ((QSpinBox*)ed_fld)->setRange(minv,maxv);
	    ((QSpinBox*)ed_fld)->setSingleStep(sstep);
	    ((QSpinBox*)ed_fld)->setPrefix(pref.c_str());
	    ((QSpinBox*)ed_fld)->setSuffix(suff.c_str());
	    break;
	}
	case Real:
	{
	    double minv = 0, maxv = 100, sstep = 1;
	    string pref, suff;
	    int    dec = 2;
	    if( !cfg.isEmpty() )
	    {
		minv  = atof(TSYS::strSepParse(cfg.toStdString(),0,':').c_str());
		maxv  = atof(TSYS::strSepParse(cfg.toStdString(),1,':').c_str());
		sstep = atof(TSYS::strSepParse(cfg.toStdString(),2,':').c_str());
		pref  = TSYS::strSepParse(cfg.toStdString(),3,':');
		suff  = TSYS::strSepParse(cfg.toStdString(),4,':');
		dec   = atoi(TSYS::strSepParse(cfg.toStdString(),5,':').c_str());
	    }
	    ((QDoubleSpinBox*)ed_fld)->setRange(minv,maxv);
	    ((QDoubleSpinBox*)ed_fld)->setSingleStep(sstep);
	    ((QDoubleSpinBox*)ed_fld)->setPrefix(pref.c_str());
	    ((QDoubleSpinBox*)ed_fld)->setSuffix(suff.c_str());
	    ((QDoubleSpinBox*)ed_fld)->setDecimals(dec);
	    break;
	}
	case Time: case Date: case DateTime:
	    ((QDateTimeEdit*)ed_fld)->setDisplayFormat(cfg);
	    break;
	case Combo:
	{
	    QString ctext = ((QComboBox*)ed_fld)->currentText();
	    ((QComboBox*)ed_fld)->clear();
	    ((QComboBox*)ed_fld)->addItems(cfg.split("\n"));
	    if( !ctext.isEmpty() )
	    {
		if( ((QComboBox*)ed_fld)->findText(ctext) < 0 ) ((QComboBox*)ed_fld)->addItem(ctext);
		((QComboBox*)ed_fld)->setEditText(ctext);
	    }
	    break;
	}
    }
    if( bt_fld ) viewApplyBt(false);
}

QString LineEdit::value()
{
    switch(type())
    {
	case Text:	return ((QLineEdit*)ed_fld)->text();
	case Integer:	return QString::number(((QSpinBox*)ed_fld)->value());
	case Real:	return QString::number(((QDoubleSpinBox*)ed_fld)->value());
	case Time:	return QString::number(QTime().secsTo(((QTimeEdit*)ed_fld)->time()));
	case Date: case DateTime:
			return QString::number(((QDateTimeEdit*)ed_fld)->dateTime().toTime_t());
	case Combo:	return ((QComboBox*)ed_fld)->currentText();
    }
    return "";
}

void LineEdit::applySlot( )
{
    viewApplyBt(false);

    if( m_val == value() ) return;
    m_val = value();

    emit apply();
}

void LineEdit::cancelSlot( )
{
    mIsEdited = false;
    if(mPrev)
    {
	setValue(m_val);
	emit cancel();
    }
    else applySlot();
}

bool LineEdit::event( QEvent * e )
{
    if( e->type() == QEvent::KeyRelease && bt_fld )
    {
	QKeyEvent *keyEvent = (QKeyEvent *)e;
	if( keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return )
	{
	    bt_fld->animateClick( );
	    return true;
	}
	else if(keyEvent->key() == Qt::Key_Escape )	{ cancelSlot(); return true; }
    }
    else if( e->type() == QEvent::Resize && applyReserve && needReserver )
    {
	ed_fld->setMaximumWidth(width()-12);
	ed_fld->setMinimumWidth(width()-12);
    }
    return QWidget::event(e);
}

//*************************************************
//* SyntxHighl: Syntax highlighter                *
//*************************************************
SyntxHighl::SyntxHighl(QTextDocument *parent) : QSyntaxHighlighter(parent)
{

}

void SyntxHighl::setSnthHgl( XMLNode nd )
{
    rules = nd;

    //> Set current font settings
    document()->setDefaultFont(WdgShape::getFont(rules.attr("font"), 1, false));

    rehighlight();
}

void SyntxHighl::rule( XMLNode *irl, const QString &text, int off, char lev )
{
    XMLNode *rl;
    vector<int> rul_pos(irl->childSize(),-1);
    int minPos = -1, minRule, endIndex, startBlk, sizeBlk;
    QTextCharFormat kForm;
    QRegExp expr;

    if(lev > 3) return;

    //> Init previous block continue
    int curBlk = (currentBlockState()>>(lev*8))&0xFF;

    //> Stream process by rules
    for(int i_t = 0; i_t < text.length(); )
    {
	if(curBlk && !i_t) { minRule = curBlk-1; minPos = 0; }
	else minRule = -1;

	for(int i_ch = 0; i_t != minPos && i_ch < (int)irl->childSize(); i_ch++)
	{
	    if(!(minPos < i_t || rul_pos[i_ch] < i_t || rul_pos[i_ch] < minPos)) continue;
	    if(rul_pos[i_ch] >= i_t && rul_pos[i_ch] < minPos)	{ minPos = rul_pos[i_ch]; minRule = i_ch; continue; }
	    if(rul_pos[i_ch] == i_t && rul_pos[i_ch] == minPos) { minRule = i_ch; break; }

	    //> Call rule
	    rl = irl->childGet(i_ch);
	    if(rl->name() == "rule")    expr.setPattern(rl->attr("expr").c_str());
	    else if(rl->name() == "blk")expr.setPattern(rl->attr("beg").c_str());
	    else continue;
	    expr.setMinimal(atoi(rl->attr("min").c_str()));
	    rul_pos[i_ch] = expr.indexIn(text,i_t);
	    if(expr.matchedLength() <= 0) continue;
	    if(rul_pos[i_ch] < 0) rul_pos[i_ch] = text.length();
	    if(minPos < i_t || rul_pos[i_ch] < minPos) { minPos = rul_pos[i_ch]; minRule = i_ch; }
	}
	if(minRule < 0) break;

	//> Process minimal rule
	rl = irl->childGet(minRule);
	kForm.setForeground(QColor(rl->attr("color").c_str()));
	kForm.setFontWeight(atoi(rl->attr("font_weight").c_str()) ? QFont::Bold : QFont::Normal);
	kForm.setFontItalic(atoi(rl->attr("font_italic").c_str()));

	if(rl->name() == "rule")
	{
	    expr.setPattern(rl->attr("expr").c_str());
	    expr.setMinimal(atoi(rl->attr("min").c_str()));
	    if(expr.indexIn(text,i_t) != rul_pos[minRule]) break;
	    setFormat(rul_pos[minRule]+off, expr.matchedLength(), kForm);
	    //> Call include rules
	    if(rl->childSize()) rule(rl, text.mid(rul_pos[minRule],expr.matchedLength()), rul_pos[minRule]+off, lev+1);
	    i_t = rul_pos[minRule]+expr.matchedLength();
	}
	else if(rl->name() == "blk")
	{
	    if(curBlk) rul_pos[minRule] = curBlk = startBlk = 0;
	    else
	    {
		expr.setPattern(rl->attr("beg").c_str());
		expr.setMinimal(atoi(rl->attr("min").c_str()));
		if(expr.indexIn(text,i_t) != rul_pos[minRule]) break;
		startBlk = rul_pos[minRule]+expr.matchedLength();
	    }
	    QRegExp eExpr(rl->attr("end").c_str());
	    eExpr.setMinimal(atoi(rl->attr("min").c_str()));
	    endIndex = eExpr.indexIn(text, startBlk);
	    if(endIndex == -1 || eExpr.matchedLength() <= 0)
	    {
		setFormat(rul_pos[minRule]+off, (text.length()-rul_pos[minRule]), kForm);
		sizeBlk = text.length()-startBlk;
		i_t = text.length();
	    }
	    else
	    {
		setFormat(rul_pos[minRule]+off, (endIndex-rul_pos[minRule]+eExpr.matchedLength()), kForm);
		sizeBlk = endIndex-startBlk;
		i_t = endIndex + eExpr.matchedLength();
	    }
	    //> Call include rules
	    if(rl->childSize()) rule(rl, text.mid(startBlk,sizeBlk), startBlk+off, lev+1);
	    if(endIndex == -1 || eExpr.matchedLength() <= 0)
		setCurrentBlockState(((minRule+1)<<(lev*8))|currentBlockState());
	    else setCurrentBlockState(currentBlockState()& ~(0xFFFFFFFF<<(lev*8)));
	}
    }
}

void SyntxHighl::highlightBlock(const QString &text)
{
    setCurrentBlockState((previousBlockState()<0)?0:previousBlockState());
    rule(&rules,text);
}

//*************************************************
//* Text edit widget                              *
//*************************************************
TextEdit::TextEdit( QWidget *parent, bool prev_dis ) :
    QWidget(parent), isInit(false), snt_hgl(NULL), but_box(NULL), stWin(NULL)
{
    QVBoxLayout *box = new QVBoxLayout(this);
    box->setMargin(0);
    box->setSpacing(0);

    ed_fld = new QTextEdit(this);
    ed_fld->setContextMenuPolicy(Qt::CustomContextMenu);
    ed_fld->setTabStopWidth(20);
    ed_fld->setAcceptRichText(false);
    ed_fld->setLineWrapMode(QTextEdit::NoWrap);
    setFocusProxy( ed_fld );
    connect(ed_fld, SIGNAL(textChanged()), this, SLOT(changed()) );
    connect(ed_fld, SIGNAL(cursorPositionChanged()), this, SLOT(curPosChange()) );
    connect(ed_fld, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ctrTreePopup()));
    box->addWidget(ed_fld);

    QImage ico_t;
    if(!ico_t.load(TUIS::icoGet("find",NULL,true).c_str())) ico_t.load(":/images/find.png");
    actFind = new QAction(QPixmap::fromImage(ico_t), _("Find"), ed_fld);
    actFind->setShortcut(Qt::CTRL+Qt::Key_F);
    actFind->setShortcutContext(Qt::WidgetShortcut);
    connect(actFind, SIGNAL(triggered()), this, SLOT(find()));
    ed_fld->addAction(actFind);
    actFindNext = new QAction(_("Find next"), ed_fld);
    actFindNext->setShortcut(Qt::Key_F3);
    actFindNext->setShortcutContext(Qt::WidgetShortcut);
    connect(actFindNext, SIGNAL(triggered()), this, SLOT(find()));
    ed_fld->addAction(actFindNext);

    if(!prev_dis) {
	but_box = new QDialogButtonBox(QDialogButtonBox::Apply|QDialogButtonBox::Cancel,Qt::Horizontal,this);
	QImage ico_t;
	but_box->button(QDialogButtonBox::Apply)->setText("");
	if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
	but_box->button(QDialogButtonBox::Apply)->setIcon(QPixmap::fromImage(ico_t));
	but_box->button(QDialogButtonBox::Apply)->setIconSize(QSize(12,12));
	connect(but_box->button(QDialogButtonBox::Apply), SIGNAL(pressed()), this, SLOT(applySlot()));
	but_box->button(QDialogButtonBox::Cancel)->setText("");
	if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
	but_box->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
	but_box->button(QDialogButtonBox::Cancel)->setIconSize(QSize(12,12));
	connect(but_box->button(QDialogButtonBox::Cancel), SIGNAL(pressed()), this, SLOT(cancelSlot()));
	but_box->setVisible(false);
	but_box->setEnabled(false);
	box->addWidget(but_box);
    }

    //Check for window with status present
    QWidget *w = parentWidget();
    while(w && w->parentWidget() && (!dynamic_cast<QMainWindow *>(w) || !((QMainWindow*)w)->statusBar())) w = w->parentWidget();
    stWin = dynamic_cast<QMainWindow *>(w);

    bt_tm = new QTimer(this);
    connect(bt_tm, SIGNAL(timeout()), this, SLOT(applySlot()));
}

bool TextEdit::isEdited( )	{ return (but_box && but_box->isVisible()); }

QString TextEdit::text( )	{ return ed_fld->toPlainText(); }

void TextEdit::setText( const QString &itext )
{
    isInit = true;
    if(itext != text()) ed_fld->setPlainText(itext);
    if(but_box && but_box->isEnabled())
    {
	but_box->setVisible(false);
	but_box->setEnabled(false);
    }
    isInit = false;

    m_text = itext;
}

void TextEdit::setSnthHgl( XMLNode nd )
{
    if(!snt_hgl) snt_hgl = new SyntxHighl(ed_fld->document());
    snt_hgl->setSnthHgl(nd);
}

void TextEdit::changed( )
{
    if(isInit) return;
    if(but_box && !but_box->isEnabled() && text() != m_text)
    {
	but_box->setVisible(true);
	but_box->setEnabled(true);
    }

    if(!but_box) bt_tm->start(500);

    emit textChanged(text());
}

void TextEdit::applySlot( )
{
    if(but_box && but_box->isEnabled())
    {
	but_box->setVisible(false);
	but_box->setEnabled(false);
    }

    m_text = text();

    emit apply();
}

void TextEdit::cancelSlot( )
{
    setText(m_text);

    emit cancel();
}

void TextEdit::curPosChange( )
{
    if(!stWin) return;
    stWin->statusBar()->showMessage(QString(_("Cursor = (%1:%2)")).arg(ed_fld->textCursor().blockNumber()+1).arg(ed_fld->textCursor().columnNumber()+1),10000);
}

bool TextEdit::event( QEvent * e )
{
    if( but_box && e->type() == QEvent::KeyRelease )
    {
	QKeyEvent *keyEvent = (QKeyEvent *)e;
	if( (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) && QApplication::keyboardModifiers()&Qt::ControlModifier )
	{
	    but_box->button(QDialogButtonBox::Apply)->animateClick( );
	    return true;
	}
	else if( keyEvent->key() == Qt::Key_Escape )
	{
	    but_box->button(QDialogButtonBox::Cancel)->animateClick( );
	    return true;
	}
    }
    if( e->type() == QEvent::ToolTip && hasFocus() && toolTip().isEmpty() )
    {
	QToolTip::showText( ((QHelpEvent *)e)->globalPos(), QString(_("Cursor = (%1:%2)")).arg(ed_fld->textCursor().blockNumber()+1).arg(ed_fld->textCursor().columnNumber()+1) );
	return true;
    }

    return QWidget::event(e);
}

void TextEdit::ctrTreePopup( )
{
    QMenu *menu = ed_fld->createStandardContextMenu();
    menu->addSeparator();
    menu->addAction(actFind);
    menu->addAction(actFindNext);
    menu->exec(QCursor::pos());
    delete menu;
}

void TextEdit::find( )
{
    bool isFind = false;
    int fopt = (QTextDocument::FindFlag)actFind->objectName().section(':',false,false).toInt();
    QString fstr = actFind->objectName().section(':',1);
    if( sender() == actFind )
    {
	InputDlg dlg(this,actFind->icon(),QString(_("Enter text string for search:")),_("String search"),0,0);
	QLineEdit *le = new QLineEdit(fstr,&dlg);
	dlg.edLay()->addWidget(le, 0, 0);
	QCheckBox *bw = new QCheckBox(_("Backward"),&dlg);
	if( fopt & QTextDocument::FindBackward ) bw->setCheckState(Qt::Checked);
	dlg.edLay()->addWidget(bw, 1, 0);
	QCheckBox *cs = new QCheckBox(_("Case sensitively"),&dlg);
	if( fopt & QTextDocument::FindCaseSensitively ) cs->setCheckState(Qt::Checked);
	dlg.edLay()->addWidget(cs, 2, 0);
	QCheckBox *ww = new QCheckBox(_("Whole words"),&dlg);
	if( fopt & QTextDocument::FindWholeWords ) ww->setCheckState(Qt::Checked);
	dlg.edLay()->addWidget(ww, 3, 0);
	le->setFocus(Qt::OtherFocusReason);
	dlg.resize(400,210);
	if( dlg.exec() == QDialog::Accepted && !le->text().isEmpty() )
	{
	    fopt = (QTextDocument::FindFlag)0;
	    if( bw->checkState()==Qt::Checked ) fopt |= QTextDocument::FindBackward;
	    if( cs->checkState()==Qt::Checked ) fopt |= QTextDocument::FindCaseSensitively;
	    if( ww->checkState()==Qt::Checked ) fopt |= QTextDocument::FindWholeWords;
	    fstr = le->text();
	    isFind = true;
	}
    }
    else if( sender() == actFindNext && !fstr.isEmpty() ) isFind = true;

    if( isFind )
    {
	ed_fld->find(fstr,(QTextDocument::FindFlag)fopt);
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
    //> Child widgets remove before
    QObjectList chLst = children();
    for(int i_c = 0; i_c < chLst.size(); i_c++)
    {
	WdgView *cw = qobject_cast<WdgView*>(chLst[i_c]);
	if(cw)	delete cw;
    }
}

float WdgView::xScale( bool full )
{
    if(full && wLevel( ) > 0)	return x_scale*((WdgView*)parentWidget())->xScale(full);
    return x_scale;
}

float WdgView::yScale( bool full )
{
    if(full && wLevel( ) > 0)	return y_scale*((WdgView*)parentWidget())->yScale(full);
    return y_scale;
}

string WdgView::root( )
{
    if(shape) return shape->id();
    return "";
}

void WdgView::moveF( const QPointF &pos )
{
    mWPos = pos;
    move(QPoint((int)TSYS::realRound(pos.x()),(int)TSYS::realRound(pos.y())));
}

void WdgView::resizeF( const QSizeF &isz )
{
    mWSize = isz;
    mWSize.setWidth(vmax(mWSize.width(),3));
    mWSize.setHeight(vmax(mWSize.height(),3));
    resize(QSize((int)TSYS::realRound(mWSize.width()), (int)TSYS::realRound(mWSize.height())));
}

WdgView *WdgView::newWdgItem( const string &iwid )
{
    return new WdgView(iwid,wLevel()+1,mainWin(),this);
}

bool WdgView::attrSet( const string &attr, const string &val, int uiPrmPos )
{
    //> Send value to model
    if(!attr.empty())
    {
	XMLNode req("set");
	req.setAttr("path",id()+"/%2fserv%2fattr");
	req.childAdd("el")->setAttr("id",attr)->setText(val);
	cntrIfCmd(req);
    }
    bool up = false, upChlds = false;

    switch(uiPrmPos)
    {
	case A_COM_LOAD: up = true;	break;
	case 0:	return false;
	case A_ROOT:
	    if(shape && shape->id() == val)	break;
	    if(shape) shape->destroy(this);
	    shape = mod->getWdgShape(val);
	    if(shape) shape->init(this);
	    break;
	case A_GEOM_X:
	    if(wLevel() == 0)	break;
	    mWPos = QPointF(((WdgView*)parentWidget())->xScale(true)*atof(val.c_str()),posF().y());
	    up = true;
	    break;
	case A_GEOM_Y:
	    if(wLevel() == 0)	break;
	    mWPos = QPointF(posF().x(),((WdgView*)parentWidget())->yScale(true)*atof(val.c_str()));
	    up = true;
	    break;
	case A_GEOM_W: mWSize = QSizeF(xScale(true)*atof(val.c_str()),sizeF().height()); up = true;	break;
	case A_GEOM_H: mWSize = QSizeF(sizeF().width(),yScale(true)*atof(val.c_str())); up = true;	break;
	case A_GEOM_Z: if(wLevel() > 0) z_coord = atoi(val.c_str());					break;
	case A_GEOM_X_SC:
	    mWSize = QSizeF((atof(val.c_str())/x_scale)*sizeF().width(),sizeF().height());
	    x_scale = atof(val.c_str());
	    up = upChlds = true;
	    break;
	case A_GEOM_Y_SC:
	    mWSize = QSizeF(sizeF().width(),(atof(val.c_str())/y_scale)*sizeF().height());
	    y_scale = atof(val.c_str());
	    up = upChlds = true;
	    break;
	case A_TIP_TOOL: setToolTip(val.c_str());	break;
	case A_TIP_STATUS: setStatusTip(val.c_str());	break;
    }
    if(up && !allAttrLoad())
    {
	if(wLevel() > 0) moveF(posF());
	resizeF(sizeF());
	for(int i_c = 0; upChlds && i_c < children().size(); i_c++)
	    if(qobject_cast<WdgView*>(children().at(i_c)))
	        ((WdgView*)children().at(i_c))->load("");
    }

    if(shape)	return shape->attrSet(this,uiPrmPos,val);

    return true;
}

void WdgView::attrsSet( AttrValS &attrs )
{
    XMLNode req("set");
    req.setAttr("path", id()+"/%2fserv%2fattr");
    string attrId, attrPos;
    for(AttrValS::iterator i_a = attrs.begin(); i_a != attrs.end(); i_a++)
    {
	int off = 0;
	attrId = TSYS::strParse(i_a->first, 0, ":", &off);
	attrPos = TSYS::strParse(i_a->first, 0, ":", &off);
	if(!attrId.empty())	req.childAdd("el")->setAttr("id",attrId)->setText(i_a->second);
	if(!attrPos.empty())	attrSet("", i_a->second, atoi(attrPos.c_str()));
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

    //> Load from data model
    if(isLoad)
    {
        bool reqBrCr = false;
	if(!aBr)
	{
	    aBr = new XMLNode("get");
	    aBr->setAttr("path",id()+"/%2fserv%2fattrBr");
	    cntrIfCmd(*aBr);
	    reqBrCr = true;
	    if(mess_lev() == TMess::Debug)
		mess_debug(mod->nodePath().c_str(), _("Request to VCA engine '%s' time %f ms."), id().c_str(), 1e-3*(TSYS::curTime()-d_cnt));
	}

	setAllAttrLoad(true);
	if(item.empty() || item == id())
	    for(unsigned i_el = 0; i_el < aBr->childSize(); i_el++)
		if(aBr->childGet(i_el)->name() == "el")
		    attrSet("",aBr->childGet(i_el)->text(),atoi(aBr->childGet(i_el)->attr("p").c_str()));
	setAllAttrLoad(false);

	//>> Delete child widgets
	string b_nm = aBr->attr("lnkPath");
	if(b_nm.empty()) b_nm = id();
	for(int i_c = 0, i_l = 0; i_c < children().size(); i_c++)
	{
	    if(!qobject_cast<WdgView*>(children().at(i_c))) continue;
	    for(i_l = 0; i_l < (int)aBr->childSize(); i_l++)
		if(aBr->childGet(i_l)->name() == "w" &&
			qobject_cast<WdgView*>(children().at(i_c))->id() == (b_nm+"/wdg_"+aBr->childGet(i_l)->attr("id")))
		    break;
	    if(i_l >= (int)aBr->childSize()) children().at(i_c)->deleteLater();
	}

	//>> Create new child widget
	for(int i_l = 0, i_c = 0; i_l < (int)aBr->childSize(); i_l++)
	{
	    if(aBr->childGet(i_l)->name() != "w") continue;
	    for(i_c = 0; i_c < children().size(); i_c++)
		if(qobject_cast<WdgView*>(children().at(i_c)) &&
			qobject_cast<WdgView*>(children().at(i_c))->id() == (b_nm+"/wdg_"+aBr->childGet(i_l)->attr("id")))
		{
		    ((WdgView*)children().at(i_c))->load((item==id())?"":item,true,(wLevel()>0)?isInit:false,aBr->childGet(i_l));
		    break;
		}
	    if(i_c < children().size()) continue;
	    WdgView *nwdg = newWdgItem(b_nm+"/wdg_"+aBr->childGet(i_l)->attr("id"));
	    nwdg->show();
	    nwdg->load((item==id())?"":item,true,(wLevel()>0)?isInit:false,aBr->childGet(i_l));
	}

	//>> Children widgets order update
	orderUpdate();

	if(reqBrCr) delete aBr;
    }
    //> Going to children init
    else
	for(int i_c = 0; i_c < children().size(); i_c++)
	{
	    WdgView *wdg = qobject_cast<WdgView*>(children().at(i_c));
	    if(wdg && (item.empty() || item == id() || wdg->id() == item.substr(0,wdg->id().size())))
		wdg->load((item==id())?"":item,false,(wLevel()>0)?isInit:false);
	}

    //> Init loaded data
    if(isInit && (item.empty() || item == id()) && wLevel() > 0) attrSet("", "load", -1);

    //> Post load init for root widget
    if(wLevel() == 0)
    {
	if(mess_lev() == TMess::Debug)
	{
	    mess_debug(mod->nodePath().c_str(), _("Load '%s' time %f ms."), id().c_str(), 1e-3*(TSYS::curTime()-d_cnt));
	    d_cnt = TSYS::curTime();
	}

	attrSet("", "load", -1);
	for(int i_c = 0; i_c < children().size(); i_c++)
	{
	    WdgView *wdg = qobject_cast<WdgView*>(children().at(i_c));
	    if(wdg && (item.empty() || item == id() || wdg->id() == item.substr(0,wdg->id().size())))
		wdg->load((item==id())?"":item,false,true);
	}
	update();
        //repaint();

	if(mess_lev() == TMess::Debug)
	    mess_debug(mod->nodePath().c_str(), _("Init '%s' time %f ms."), id().c_str(), 1e-3*(TSYS::curTime()-d_cnt));
    }
}

void WdgView::orderUpdate( )
{
    QObjectList &ols = d_ptr->children;

    vector< pair<int,QObject*> > arr;
    arr.reserve(children().size());
    for(int i_c = 0; i_c < ols.size(); i_c++) {
	WdgView *cw = qobject_cast<WdgView*>(ols[i_c]);
	if(cw) arr.push_back(pair<int,QObject*>(cw->z(),cw));
	else arr.push_back(pair<int,QObject*>(100000,ols[i_c]));
    }
    make_heap(arr.begin(),arr.end());
    sort_heap(arr.begin(),arr.end());
    if(ols.size() == (int)arr.size())
	for(int i_c = 0; i_c < ols.size(); i_c++)
	    if(i_c < (int)arr.size()) ols[i_c] = arr[i_c].second;
}

bool WdgView::event( QEvent *event )
{
    //Paint event process
    if(event->type() == QEvent::Paint) return shape ? shape->event(this, event) : true;

    return false; //QWidget::event(event);
}

bool WdgView::eventFilter( QObject *object, QEvent *event )	{ return shape ? shape->eventFilter(this,object,event) : false; }
