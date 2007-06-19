
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

#include <QVBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QEvent>
#include <QDialogButtonBox>

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
    
    QLabel *inp_lab = new QLabel(mess,this);
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
    
    resize(250,120+(40*with_nm)+(40*with_id));
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

void InputDlg::setId(const QString &val)
{
    if( m_id )	m_id->setText(val);
}

void InputDlg::setName(const QString &val)
{
    if( m_name )m_name->setText(val);
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

//****************************************
//* Shape widget view                    *
//****************************************
WdgView::WdgView( const string &iwid, int ilevel, QMainWindow *mainWind, QWidget *parent ) :
    QWidget(parent), idWidget(iwid), shape(NULL), w_level(ilevel), main_win(mainWind), z_coord(0), reqtm(0)
{

}

WdgView::~WdgView( )
{
    if( shape ) shape->destroy(this);
}

string WdgView::root( )
{
    if( shape ) return shape->id();
    return "";
}

WdgView *WdgView::newWdgItem( const string &iwid )
{
    return new WdgView(iwid,wLevel()+1,mainWin(),this);
}

void WdgView::load( const string& item, bool dt_up )
{
    if( item.empty() || item == id() )
    {    
	XMLNode get_req("get");
	get_req.setAttr("user",user());
	if( !dt_up )
	{
	    //- Init generic data -
	    //-- Reinit childs --
	    string b_nm = id();
	    get_req.setAttr("user",user())->setAttr("path",id()+"/%2fwdg%2fcfg%2fpath")->setAttr("resLink","1");
	    if( !mod->cntrIfCmd(get_req) ) b_nm = get_req.text();
	    
	    vector<string> lst;
	    get_req.setAttr("user",user())->setAttr("path",b_nm+"/%2finclwdg%2fwdg");
	    if( !mod->cntrIfCmd(get_req) )
		for( int i_el = 0; i_el < get_req.childSize(); i_el++ )
		    lst.push_back(get_req.childGet(i_el)->attr("id"));
	    //--- Delete child widgets ---
	    for( int i_c = 0; i_c < children().size(); i_c++ )
	    {
		if( !qobject_cast<WdgView*>(children().at(i_c)) ) continue;
		int i_l;
		for( i_l = 0; i_l < lst.size(); i_l++ )		
		    if( qobject_cast<WdgView*>(children().at(i_c))->id() == (b_nm+"/wdg_"+lst[i_l]) )
			break;
		if( i_l >= lst.size() ) delete children().at(i_c);
	    }
	    //--- Create new child widget ---
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
			if( !item.empty() ) load("",dt_up);
		    }
		}
	    }
            
	    //-- Reinit shape --
	    //--- Get root id ---
	    get_req.setAttr("user",user())->setAttr("path",id()+"/%2fwdg%2fcfg%2froot");
	    if( !mod->cntrIfCmd(get_req) && ( !shape || shape->id() != get_req.text() ) )
	    {
		if( shape ) shape->destroy(this);
		shape = mod->getWdgShape(get_req.text());
		if( shape ) shape->init(this);
	    }
	}
	
	//- Request to widget for last attributes -
	reqtm = dt_up ? reqtm : 0;
	get_req.setAttr("user",user())->setAttr("path",id()+"/%2fattr%2fscmd")->setAttr("tm",TSYS::uint2str(reqtm));
	get_req.childClean();
	if( !mod->cntrIfCmd(get_req) )
	{
	    dataReq().clear();
	    for( int i_el = 0; i_el < get_req.childSize(); i_el++ )
		dataReq()[get_req.childGet(i_el)->attr("id").c_str()] = get_req.childGet(i_el)->text().c_str();
	    reqtm = strtoul(get_req.attr("tm").c_str(),0,10);
	}
	
	//- Load generic data -
	bool act = false;
	int g_x, g_y;
	QMap<QString, QString>::const_iterator vl;	    
	if( wLevel( ) > 0 )
	{
	    //-- Update position --
	    if( (vl=dataReq().find("geomX")) == dataReq().end() ) g_x = pos().x();
	    else { act = true; g_x = vl.value().toInt(); }
	    if( (vl=dataReq().find("geomY")) == dataReq().end() ) g_y = pos().y();
	    else { act = true; g_y = vl.value().toInt(); }
	    if( act ) move(g_x,g_y);
	    //-- Update level --
	    if( (vl=dataReq().find("geomZ")) != dataReq().end() ) z_coord = vl.value().toInt();	
	}
	//-- Update size --
	act = false;
	if( (vl=dataReq().find("geomW")) == dataReq().end() ) g_x = size().width();
	else { act = true; g_x = vl.value().toInt(); }
	if( (vl=dataReq().find("geomH")) == dataReq().end() ) g_y = size().height();
	else { act = true; g_y = vl.value().toInt(); }
	if( act ) resize(g_x,g_y);

	if( shape ) shape->load( this );
	
	//- Update view -
	update();
    }

    if( item != id() )
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<WdgView*>(children().at(i_c)) )
		((WdgView*)children().at(i_c))->load(item,dt_up);
    
    if( !dt_up ) orderUpdate( );
}

void WdgView::save( const string& item )
{
    if( item.empty() || item == id() )
    {
	XMLNode set_req("set");
	set_req.setAttr("user",user())->setAttr("path",id()+"/%2fattr%2fscmd");
	set_req.childAdd("el")->setAttr("id","geomX")->setText(TSYS::int2str(pos().x()));
	set_req.childAdd("el")->setAttr("id","geomY")->setText(TSYS::int2str(pos().y()));
	set_req.childAdd("el")->setAttr("id","geomW")->setText(TSYS::int2str(size().width()));
	set_req.childAdd("el")->setAttr("id","geomH")->setText(TSYS::int2str(size().height()));
	set_req.childAdd("el")->setAttr("id","geomZ")->setText(TSYS::int2str(z_coord));	
	mod->cntrIfCmd(set_req);
    }
    if( item != id() && wLevel() == 0 )
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<WdgView*>(children().at(i_c)) )
		qobject_cast<WdgView*>(children().at(i_c))->save(item);
}

void WdgView::orderUpdate( )
{
    WdgView *lw = NULL;
    for( int i_c = 0; i_c < children().size(); i_c++ )
    {
	WdgView *cw = qobject_cast<WdgView*>(children().at(i_c));
	if( !cw ) continue;
	if( !lw || (cw->z() >= lw->z()) ) lw = cw;
	else
	{	
	    cw->stackUnder(lw);
	    i_c = -1;
	    lw = NULL;
	}
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

    return false; // QWidget::event(event);
}

bool WdgView::eventFilter( QObject *object, QEvent *event )
{
    if( shape )	return shape->eventFilter(this,object,event);
    return false;
}

