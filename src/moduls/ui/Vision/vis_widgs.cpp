
//OpenSCADA system module UI.Vision file: vis_widgs.cpp
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

#include <math.h>

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

#include <tsys.h>

#include "vis_shapes.h"
#include "tvision.h"
#include "vis_widgs.h"

using namespace VISION;

//*************************************************
//* Id and name input dialog                      *
//*************************************************
InputDlg::InputDlg( QWidget *parent, const QIcon &icon, const QString &mess, 
	const QString &ndlg, bool with_id, bool with_nm ) :
		QDialog(parent), m_id(NULL), m_name(NULL)
{
    setWindowTitle(ndlg);
    setMinimumSize( QSize( 120, 150 ) );
    setWindowIcon(icon);
    setSizeGripEnabled(true);

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

    //Id and name fields
    if( with_nm || with_id )
    {
	QGridLayout *ed_lay = new QGridLayout;
	ed_lay->setSpacing(6);
	if( with_id )
	{
	    ed_lay->addWidget( new QLabel(_("ID:"),this), 0, 0 );
	    m_id = new QLineEdit(this);
	    ed_lay->addWidget( m_id, 0, 1 );
	}
	if( with_nm )
	{
	    ed_lay->addWidget( new QLabel(_("Name:"),this), 1, 0 );
	    m_name = new QLineEdit(this);
	    ed_lay->addWidget( m_name, 1, 1 );
	}
	dlg_lay->addItem(ed_lay);
    }

    //Qk and Cancel buttons
    dlg_lay->addItem( new QSpacerItem( 10, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    QFrame *sep = new QFrame(this);
    sep->setFrameShape( QFrame::HLine );
    sep->setFrameShadow( QFrame::Raised );
    dlg_lay->addWidget( sep );
    
    QDialogButtonBox *but_box = new QDialogButtonBox(QDialogButtonBox::Ok|
                                    		     QDialogButtonBox::Cancel,Qt::Horizontal,this);
    QImage ico_t;
    but_box->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t.load(":/images/button_ok.png");
    but_box->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(accepted()), this, SLOT(accept()));
    but_box->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t.load(":/images/button_cancel.png");
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

//*************************************************
//* User select dialog                            *
//*************************************************
DlgUser::DlgUser( QWidget *parent ) : QDialog(parent)
{
    setWindowTitle(_("Select user"));

    QVBoxLayout *dlg_lay = new QVBoxLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);

    QGridLayout *ed_lay = new QGridLayout;
    ed_lay->setSpacing(6);
    ed_lay->addWidget( new QLabel(_("User:"),this), 0, 0 );
    users = new QComboBox(this);
    ed_lay->addWidget( users, 0, 1 );
    ed_lay->addWidget( new QLabel(_("Password:"),this), 1, 0 );
    passwd = new QLineEdit(this);
    passwd->setEchoMode( QLineEdit::Password );
    ed_lay->addWidget( passwd, 1, 1 );
    dlg_lay->addItem(ed_lay);

    dlg_lay->addItem( new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    QFrame *sep = new QFrame(this);
    sep->setFrameShape( QFrame::HLine );
    sep->setFrameShadow( QFrame::Raised );
    dlg_lay->addWidget( sep );

    QDialogButtonBox *but_box = new QDialogButtonBox(QDialogButtonBox::Ok|
                                    		     QDialogButtonBox::Cancel,Qt::Horizontal,this);
    QImage ico_t;    
    but_box->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t.load(":/images/button_ok.png");
    but_box->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(accepted()), this, SLOT(accept()));
    but_box->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t.load(":/images/button_cancel.png");
    but_box->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(rejected()), this, SLOT(reject()));
    dlg_lay->addWidget( but_box );
    
    connect(this, SIGNAL(finished(int)), this, SLOT(finish(int)));
    
    //Fill users list
    vector<string> u_list;	
    SYS->security().at().usrList(u_list);
    for(int i_l = 0; i_l < u_list.size(); i_l++ )
    {
        string simg = TSYS::strDecode(SYS->security().at().usrAt(u_list[i_l]).at().picture(),TSYS::base64);
        QImage img;
        if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
	    users->addItem(QPixmap::fromImage(img),u_list[i_l].c_str());
	else users->addItem(u_list[i_l].c_str());
    }
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
	//Check user
	if( SYS->security().at().usrPresent(user().toAscii().data()) && 
		SYS->security().at().usrAt(user().toAscii().data()).at().auth(password().toAscii().data()) )
	    setResult(SelOK);
	else setResult(SelErr);
    }
    else setResult(SelCancel);
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

    QDialogButtonBox *but_box = new QDialogButtonBox(QDialogButtonBox::Ok|
                                    		     QDialogButtonBox::Cancel,Qt::Horizontal,this);
    QImage ico_t;    
    but_box->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t.load(":/images/button_ok.png");
    but_box->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(accepted()), this, SLOT(accept()));
    but_box->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t.load(":/images/button_cancel.png");
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
    sscanf(fnt.toAscii().data(),"%100s %d %d %d %d %d",family,&size,&bold,&italic,&underline,&strike);
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

//*********************************************
//* Status bar user widget                    *
//*********************************************
UserStBar::UserStBar( const QString &iuser, QWidget *parent ) : QLabel(parent)
{
    setUser(iuser);
}

QString UserStBar::user()
{
    return user_txt;
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
    DlgUser d_usr(parentWidget());
    int rez = d_usr.exec();
    if( rez == DlgUser::SelOK && d_usr.user() != user() )
    {
        setUser( d_usr.user() );
	emit userChanged();
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
LineEdit::LineEdit( QWidget *parent, LType tp, bool prev_dis ) :
    QWidget( parent ), m_tp((LineEdit::LType)-1), bt_fld(NULL), ed_fld(NULL)
{
    QHBoxLayout *box = new QHBoxLayout(this);
    box->setMargin(0);
    box->setSpacing(0);

    if( !prev_dis )
    {
	bt_fld = new QPushButton(this);
	bt_fld->setIcon(QIcon(":/images/ok.png"));
	bt_fld->setIconSize(QSize(12,12));
	bt_fld->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
	bt_fld->setMaximumWidth(15);
	//bt_fld->setMinimumSize(15,15);
	//bt_fld->setFlat( true );
	bt_fld->setEnabled( false );
	bt_fld->setVisible( false );
	connect( bt_fld, SIGNAL( pressed() ), this, SLOT( applySlot() ) );
	box->addWidget(bt_fld);
    }
    setType(tp);
}

LineEdit::~LineEdit( )
{
    //printf("TEST 00: %d\n",d_ptr->inEventHandler);
}

bool LineEdit::isEdited( )
{
    if( bt_fld && bt_fld->isVisible() )	return true;
    return false;
}

void LineEdit::setType( LType tp )
{
    if( tp == m_tp ) return;

    //- Delete previous -
    if( tp >= 0 && ed_fld ) delete ed_fld;

    //- Create new widget -
    switch( tp )
    {
	case Text:
	    ed_fld = new QLineEdit(this);
	    connect( (QLineEdit*)ed_fld, SIGNAL( textEdited(const QString&) ), SLOT( changed() ) );
	    break;
	case Integer:
	    ed_fld = new QSpinBox(this);
	    connect( (QSpinBox*)ed_fld, SIGNAL( valueChanged(int) ), SLOT( changed() ) );
	    break;
	case Real:
	    ed_fld = new QDoubleSpinBox(this);
	    connect( (QDoubleSpinBox*)ed_fld, SIGNAL( valueChanged(double) ), SLOT( changed() ) );
	    break;
	case Time:
	    ed_fld = new QTimeEdit(this);
	    connect( (QTimeEdit*)ed_fld, SIGNAL( timeChanged(const QTime&) ), SLOT( changed() ) );
	    break;
	case Date:
	    ed_fld = new QDateEdit(this);
	    connect( (QDateEdit*)ed_fld, SIGNAL( dateChanged(const QDate&) ), SLOT( changed() ) );
	    break;
	case DateTime:
	    ed_fld = new QDateTimeEdit(this);
	    connect( (QDateTimeEdit*)ed_fld, SIGNAL( dateTimeChanged(const QDateTime&) ), SLOT( changed() ) );
	    break;
	case Combo:
	    ed_fld = new QComboBox(this);
	    ((QComboBox*)ed_fld)->setEditable(true);
	    connect( (QComboBox*)ed_fld, SIGNAL( editTextChanged(const QString&) ), SLOT( changed() ) );
	    break;
    }
    ((QBoxLayout*)layout())->insertWidget(0,ed_fld);
    setFocusProxy( ed_fld );

    
    m_tp = tp;
}

void LineEdit::changed( )
{
    //- Enable apply 
    if( bt_fld && !bt_fld->isEnabled() )
    {
	bt_fld->setEnabled(true);
        bt_fld->setVisible(true);
	//QWidget::setTabOrder( mod->getFocusedWdg(ed_fld), mod->getFocusedWdg(bt_fld) );	
    }
    
    emit valChanged(value());
}

void LineEdit::setValue(const QString &txt)
{
    switch(type())
    {
	case Text: 
	    ((QLineEdit*)ed_fld)->setText(txt);
	    ((QLineEdit*)ed_fld)->setCursorPosition(0);
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
	    ((QDateTimeEdit*)ed_fld)->setDateTime(QDateTime::fromTime_t(txt.toInt()));
	    break;
	case Combo:
	    if( ((QComboBox*)ed_fld)->findText(txt) < 0 ) ((QComboBox*)ed_fld)->addItem(txt);
	    ((QComboBox*)ed_fld)->setEditText(txt);
	    break;
    }
    
    m_val = txt;

    if( bt_fld && bt_fld->isEnabled() )
    {
        bt_fld->setEnabled(false);
        bt_fld->setVisible(false);
    }    
}

void LineEdit::setCfg(const QString &cfg)
{
    switch(type())
    {
	case Text: 	((QLineEdit*)ed_fld)->setInputMask(cfg);	break;
	case Integer:
	{
	    int	minv = 0, maxv = 100, sstep = 1;
	    string pref, suff;
	    if( !cfg.isEmpty() )
	    {
		minv  = atoi(TSYS::strSepParse(cfg.toAscii().data(),0,':').c_str());
		maxv  = atoi(TSYS::strSepParse(cfg.toAscii().data(),1,':').c_str());
		sstep = atoi(TSYS::strSepParse(cfg.toAscii().data(),2,':').c_str());
		pref  = TSYS::strSepParse(cfg.toAscii().data(),3,':');
		suff  = TSYS::strSepParse(cfg.toAscii().data(),4,':');
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
		minv  = atof(TSYS::strSepParse(cfg.toAscii().data(),0,':').c_str());
		maxv  = atof(TSYS::strSepParse(cfg.toAscii().data(),1,':').c_str());
		sstep = atof(TSYS::strSepParse(cfg.toAscii().data(),2,':').c_str());
		pref  = TSYS::strSepParse(cfg.toAscii().data(),3,':');
		suff  = TSYS::strSepParse(cfg.toAscii().data(),4,':');
		dec   = atoi(TSYS::strSepParse(cfg.toAscii().data(),5,':').c_str());
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
    if( bt_fld && bt_fld->isEnabled() )
    {
        bt_fld->setEnabled(false);
        bt_fld->setVisible(false);
    }        
}
				    
QString LineEdit::value()
{
    switch(type())
    {
	case Text: 	return ((QLineEdit*)ed_fld)->text();
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
    bt_fld->setEnabled(false);
    bt_fld->setVisible(false);

    m_val = value();
	
    emit apply();
}
	    
bool LineEdit::event( QEvent * e )
{
    if(e->type() == QEvent::KeyRelease && bt_fld && bt_fld->isEnabled())
    {
        QKeyEvent *keyEvent = (QKeyEvent *)e;
        if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
        {
            bt_fld->animateClick( );
            return true;
        }
        else if(keyEvent->key() == Qt::Key_Escape )
        {
            emit cancel();
	    setValue(m_val);
            return true;
        }
    }
    return QWidget::event(e);
}

//*************************************************
//* Text edit widget                              *
//*************************************************
TextEdit::TextEdit( QWidget *parent, bool prev_dis ) :
    QWidget(parent), but_box(NULL), isInit(false)
{
    QVBoxLayout *box = new QVBoxLayout(this);
    box->setMargin(0);
    box->setSpacing(0);
		    
    ed_fld = new QTextEdit(this);
    setFocusProxy( ed_fld );
    connect( ed_fld, SIGNAL( textChanged() ), this, SLOT( changed() ) );
    box->addWidget(ed_fld);
				
    if( !prev_dis )
    {
        but_box = new QDialogButtonBox(QDialogButtonBox::Apply|QDialogButtonBox::Cancel,Qt::Horizontal,this);
        QImage ico_t;
        but_box->button(QDialogButtonBox::Apply)->setText("");
        if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t.load(":/images/button_ok.png");
        but_box->button(QDialogButtonBox::Apply)->setIcon(QPixmap::fromImage(ico_t));
	but_box->button(QDialogButtonBox::Apply)->setIconSize(QSize(12,12));
        connect(but_box->button(QDialogButtonBox::Apply), SIGNAL(pressed()), this, SLOT(applySlot()));
        but_box->button(QDialogButtonBox::Cancel)->setText("");
	if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t.load(":/images/button_cancel.png");
        but_box->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
	but_box->button(QDialogButtonBox::Cancel)->setIconSize(QSize(12,12));
        connect(but_box->button(QDialogButtonBox::Cancel), SIGNAL(pressed()), this, SLOT(cancelSlot()));
        but_box->setVisible(false);
	but_box->setEnabled(false);
        box->addWidget(but_box);
    }
}

QString TextEdit::text()
{
    return ed_fld->toPlainText();
}
    
void TextEdit::setText(const QString &text)
{
    isInit=true;
    ed_fld->setPlainText(text);
    if( but_box && but_box->isEnabled() )
    {
	but_box->setVisible(false);
	but_box->setEnabled(false);
    }    
    isInit=false;
    
    m_text = text;
}
			    
void TextEdit::changed()
{
    if( isInit ) return;
    if( but_box && !but_box->isEnabled() && text() != m_text ) 
    {
	but_box->setVisible(true);
	but_box->setEnabled(true);
    }
    emit textChanged(text());
}

void TextEdit::applySlot( )
{
    if( but_box && but_box->isEnabled() )
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

//****************************************
//* Shape widget view                    *
//****************************************
WdgView::WdgView( const string &iwid, int ilevel, QMainWindow *mainWind, QWidget *parent, Qt::WindowFlags f ) :
    QWidget(parent,f), idWidget(iwid), shape(NULL), w_level(ilevel), main_win(mainWind), 
    x_scale(1.0), y_scale(1.0), z_coord(0), all_attr_load(false)
{
    //setAttribute(Qt::WA_OpaquePaintEvent,true);
}

WdgView::~WdgView( )
{
    if( shape ) shape->destroy(this);
}

float WdgView::xScale( bool full )
{
    if( full && wLevel( ) > 0 )	return x_scale*((WdgView*)parentWidget())->xScale(full);
    return x_scale;
}

float WdgView::yScale( bool full )
{
    if( full && wLevel( ) > 0 )	return y_scale*((WdgView*)parentWidget())->yScale(full);
    return y_scale;
}

string WdgView::root( )
{
    if( shape ) return shape->id();
    return "";
}

void WdgView::moveF( const QPointF &pos )
{
    w_pos = pos;
    move( pos.toPoint() );
}

void WdgView::resizeF( const QSizeF &size )
{
    w_size = size;
    w_size.setWidth(vmax(w_size.width(),3));
    w_size.setHeight(vmax(w_size.height(),3));
    resize( w_size.toSize() );
}

WdgView *WdgView::newWdgItem( const string &iwid )
{
    return new WdgView(iwid,wLevel()+1,mainWin(),this);
}

bool WdgView::attrSet( const string &attr, const string &val, int uiPrmPos )
{
    //- Send value to model -
    if( !attr.empty() )
    {
	XMLNode req("set");
	req.setAttr("path",id()+"/%2fserv%2f0");
	req.childAdd("el")->setAttr("id",attr)->setText(val);
	cntrIfCmd(req);
    }
    switch(uiPrmPos)
    {
	case 0:	return false;
	case 7:
	    if( wLevel( ) == 0 )break;
	    moveF(QPointF(((WdgView*)parentWidget())->xScale(true)*atof(val.c_str()),posF().y()));
	    break;
	case 8:
	    if( wLevel( ) == 0 )break;
	    moveF(QPointF(posF().x(),((WdgView*)parentWidget())->yScale(true)*atof(val.c_str())));
	    break;
	case 9:	resizeF(QSizeF(xScale(true)*atof(val.c_str()),sizeF().height()));	break;
	case 10:resizeF(QSizeF(sizeF().width(),yScale(true)*atof(val.c_str())));	break;
	case 11:if(wLevel( )>0) z_coord = atoi(val.c_str());				break;
	case 13:
	    resizeF(QSizeF((atof(val.c_str())/x_scale)*sizeF().width(),sizeF().height()));
	    x_scale = atof(val.c_str());
	    break;
	case 14:
	    resizeF(QSizeF(sizeF().width(),(atof(val.c_str())/y_scale)*sizeF().height()));
	    y_scale = atof(val.c_str());
	    break;
    }
    return shape ? shape->attrSet(this,uiPrmPos,val) : true;
}

string WdgView::resGet( const string &res )
{
    XMLNode req("get");
    req.setAttr("path",id()+"/%2fwdg%2fres")->setAttr("id",res);
    if( !cntrIfCmd(req) )	return TSYS::strDecode(req.text(),TSYS::base64);

    return "";
}

void WdgView::load( const string& item, bool load, bool init )
{
    //printf("TEST 00: Load: %s (%d:%d)\n",id().c_str(),load,init);

    unsigned long long t_cnt;
    if( wLevel() == 0 ) t_cnt = SYS->shrtCnt();

    //- Load from data model -
    if( load )
    {
	childsUpdate( false );
	setAllAttrLoad( true );
	if( item.empty() || item == id() )
	{
	    shapeUpdate( );
	
	    //-- Request to widget for last attributes --
	    XMLNode req("get");
	    req.setAttr("path",id()+"/%2fserv%2f0");
	    if( !cntrIfCmd(req) )
		for( int i_el = 0; i_el < req.childSize(); i_el++ )
		    attrSet("",req.childGet(i_el)->text(),atoi(req.childGet(i_el)->attr("pos").c_str()));
	}
	setAllAttrLoad( false );
    }

    //- Init loaded data -
    if( init && (item.empty() || item == id()) && wLevel()>0 )	attrSet("","load",-1);

    //- Going to children load and/or init -
    for( int i_c = 0; i_c < children().size(); i_c++ )
    {
	WdgView *wdg = qobject_cast<WdgView*>(children().at(i_c));
	if( wdg && (item.empty() || item == id() || wdg->id() == item.substr(0,wdg->id().size())) )
	    wdg->load((item==id())?"":item,load,(wLevel()>0)?init:false);
    }

    //- Children widgets order update -
    if( load ) orderUpdate( );

    //- Post load init for root widget -
    if( wLevel() == 0 )
    {
	//printf("TEST 01: Load '%s' time %fms\n",item.c_str(),1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk()));
	//t_cnt = SYS->shrtCnt();

	attrSet("","load",-1);
	for( int i_c = 0; i_c < children().size(); i_c++ )
	{
	    WdgView *wdg = qobject_cast<WdgView*>(children().at(i_c));
	    if( wdg && (item.empty() || item == id() || wdg->id() == item.substr(0,wdg->id().size())) )
		wdg->load((item==id())?"":item,false,true);
	}
	update();

	//printf("TEST 02: Init '%s' time %fms\n",item.c_str(),1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk()));
    }
}

void WdgView::childsUpdate( bool newLoad )
{
    XMLNode req("get");

    string b_nm = id();
    req.setAttr("path",id()+"/%2fwdg%2fcfg%2fpath")->setAttr("resLink","1");
    if( !cntrIfCmd(req) ) b_nm = req.text();

    vector<string> lst;
    req.clear()->setAttr("path",b_nm+"/%2finclwdg%2fwdg");
    if( !cntrIfCmd(req) )
	for( int i_el = 0; i_el < req.childSize(); i_el++ )
	    lst.push_back(req.childGet(i_el)->attr("id"));

    //- Delete child widgets -
    for( int i_c = 0; i_c < children().size(); i_c++ )
    {
	if( !qobject_cast<WdgView*>(children().at(i_c)) ) continue;
	int i_l;
	for( i_l = 0; i_l < lst.size(); i_l++ )
	    if( qobject_cast<WdgView*>(children().at(i_c))->id() == (b_nm+"/wdg_"+lst[i_l]) )
		break;
	if( i_l >= lst.size() ) delete children().at(i_c);
    }

    //- Create new child widget -
    for( int i_l = 0; i_l < lst.size(); i_l++ )
    {
	int i_c;
	for( i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<WdgView*>(children().at(i_c)) && 
		    qobject_cast<WdgView*>(children().at(i_c))->id() == (b_nm+"/wdg_"+lst[i_l]) )
		break;
	if( i_c >= children().size() )
	{
	    WdgView *nwdg = newWdgItem(b_nm+"/wdg_"+lst[i_l]);
	    if( nwdg ) 
	    {
		nwdg->show();
		if( newLoad ) nwdg->load("");
	    }
	}
    }
}

void WdgView::shapeUpdate( )
{
    //- Get root id -
    XMLNode req("get");
    req.setAttr("path",id()+"/%2fwdg%2fcfg%2froot");
    if( !cntrIfCmd(req) && ( !shape || shape->id() != req.text() ) )
    {
	if( shape ) shape->destroy(this);
	shape = mod->getWdgShape(req.text());
	if( shape ) shape->init(this);
    }
}

void WdgView::orderUpdate( )
{
    WdgView *lw = NULL;
    for( int i_c = 0; i_c < children().size(); i_c++ )
    {
	WdgView *cw = qobject_cast<WdgView*>(children().at(i_c));
	if( !cw ) continue;
	if( lw && (cw->z() < lw->z()) )
	{
	    cw->stackUnder(lw);
	    i_c = -1;
	    lw = NULL;
	}
	else lw = cw;
    }
}

bool WdgView::event( QEvent *event )
{
    //- Paint event process -
    if( event->type() == QEvent::Paint )
    {
	//- Self widget view -
	if( shape )	return shape->event(this,event);
	return true;
    }

    return false; //QWidget::event(event);
}

bool WdgView::eventFilter( QObject *object, QEvent *event )
{
    if( shape )	return shape->eventFilter(this,object,event);
    return false;
}

