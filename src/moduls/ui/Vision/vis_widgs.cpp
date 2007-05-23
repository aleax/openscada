
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
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QEvent>
#include <QPainter>
#include <QDialogButtonBox>
#include <QApplication>
#include <QPaintEvent>
#include <QStatusBar>
#include <QActionGroup>
#include <QToolBar>

#include <tsys.h>

#include "vis_shapes.h"
#include "vis_devel.h"
#include "vis_devel_widgs.h"
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
WdgView::WdgView( const string &iwid, int ilevel, bool devMod, QMainWindow *mainWind, QWidget *parent ) :
    QWidget(parent), idWidget(iwid), shape(NULL), selWidget(false), moveHold(false), 
    w_level(ilevel), mode_dev(devMod), main_win(mainWind), pnt_view(NULL), z_coord(0), reqtm(0)
{
    if( develMode() )
    {
	setMouseTracking(true);
	if( wLevel() == 0 )	
	{
	    pnt_view = new SizePntWdg(this);
	    pnt_view->setSelArea(QRect());	    
	    setFocusPolicy(Qt::StrongFocus);
	    setCursor(Qt::ArrowCursor);
	    setAcceptDrops(true);
	}
    }

    loadData(id());
}

WdgView::~WdgView( )
{

}

void WdgView::setSelect( bool vl, bool childs )
{
    int chld_cnt = 0;
    
    if( !develMode() )	return;    
    selWidget = vl;    
    if( wLevel() != 0 )	return;
    
    if( vl )
    {
	string sel_chlds = selectChilds(&chld_cnt);
	if( sel_chlds.size() )	emit selected(sel_chlds);
	else			emit selected(id());
    }
    else
    {
	if( childs )
	    for( int i_c = 0; i_c < children().size(); i_c++ )
		if( qobject_cast<WdgView*>(children().at(i_c)) )
		    qobject_cast<WdgView*>(children().at(i_c))->setSelect(false);
	emit selected("");
    }
    
    //- Update actions access -
    //-- Enable view toolbar --
    ((VisDevelop *)main_win)->wdgToolView->setVisible(vl);
    disconnect( ((VisDevelop *)main_win)->wdgToolView, SIGNAL(actionTriggered(QAction*)), 
		this, SLOT(wdgViewTool(QAction*)) );
    if( vl ) connect( ((VisDevelop *)main_win)->wdgToolView, SIGNAL(actionTriggered(QAction*)), 
		      this, SLOT(wdgViewTool(QAction*)) );

    //-- Update widget view tools --
    for( int i_a = 0; i_a < ((VisDevelop *)main_win)->wdgToolView->actions().size(); i_a++ )
	((VisDevelop *)main_win)->wdgToolView->actions().at(i_a)->setEnabled(chld_cnt>0);
    
    update();
}

string WdgView::selectChilds( int *cnt )
{
    string sel_chlds;

    if( cnt ) *cnt = 0;
    if( develMode() )
	for( int i_c = 0; i_c < children().size(); i_c++ )
        {
            WdgView *curw = qobject_cast<WdgView*>(children().at(i_c));
	    if( !curw )	continue;
            if( curw->select() )
	    { 
		sel_chlds=sel_chlds+curw->id()+";";
		if( cnt ) (*cnt)++;
	    }
        }
    return sel_chlds;
}

void WdgView::loadData( const string& item, bool dt_up )
{
    if( item.empty() || item == id() )
    {
	XMLNode get_req("get");
	if( !dt_up )
	{
	    //- Init generic data -
	    //-- Reinit childs --
	    string b_nm = id();
	    get_req.setAttr("user","user")->setAttr("path",id()+"/%2fwdg%2fst%2fpath")->setAttr("resLink","1");
	    if( !mod->cntrIfCmd(get_req) ) b_nm = get_req.text();
	    
	    vector<string> lst;
	    get_req.setAttr("user","user")->setAttr("path",b_nm+"/%2finclwdg%2fwdg");
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
		    WdgView *wv = new WdgView(b_nm+"/wdg_"+lst[i_l],wLevel()+1,develMode(),mainWin(),this);
		    wv->show();
		    if(pnt_view) pnt_view->raise();
		}
	    }
            
	    //-- Reinit shape --
	    //--- Get root id ---
	    get_req.setAttr("user","user")->setAttr("path",id()+"/%2fwdg%2fst%2froot");
	    if( !mod->cntrIfCmd(get_req) && ( !shape || shape->id() != get_req.text() ) )
	    {
		shape = mod->getWdgShape(get_req.text());
		shape->init(this); 
	    }
	}
	
	//- Request to widget for last attributes -
	reqtm = dt_up?reqtm:0;
	get_req.setAttr("user","user")->setAttr("path",id()+"/%2fattr%2fscmd")->setAttr("tm",TSYS::int2str(reqtm));
	get_req.childClean();
	if( !mod->cntrIfCmd(get_req) )
	{
	    dataReq().clear();
	    for( int i_el = 0; i_el < get_req.childSize(); i_el++ )
		dataReq()[get_req.childGet(i_el)->attr("id").c_str()] = get_req.childGet(i_el)->text().c_str();
	    reqtm = atoi(get_req.attr("tm").c_str());
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

	if( shape ) shape->loadData( this );

	//- Update view -
	update();
    }

    if( item != id() && (!develMode() || wLevel() == 0) )
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<WdgView*>(children().at(i_c)) )
		((WdgView*)children().at(i_c))->loadData(item,dt_up);
    
    if( !dt_up )
    {
	//- Update widgets deep layout (z) -
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
}

void WdgView::saveData( const string& item )
{
    if( item.empty() || item == id() )
    {
	XMLNode set_req("set");
	set_req.setAttr("user","user")->setAttr("path",id()+"/%2fattr%2fscmd");
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
		qobject_cast<WdgView*>(children().at(i_c))->saveData(item);

    //- For top items (like inspector) data update -
    if( develMode() && wLevel() == 0 )	setSelect(true);
}

bool WdgView::grepAnchor( const QPoint &apnt, const QPoint &cpnt )
{
    if( (cpnt.x() > apnt.x()-4) && (cpnt.x() < apnt.x()+4) &&
	    (cpnt.y() > apnt.y()-4) && (cpnt.y() < apnt.y()+4) )
	return true;
    else return false;
}

void WdgView::upMouseCursors( const QPoint &curp )
{
    if( develMode() && !moveHold )
    {
	Qt::CursorShape new_shp = Qt::ArrowCursor;	    		
	//- Check child's anchor selection and widget's geometry -
	leftTop  = false;
	QRect selRect;
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<WdgView*>(children().at(i_c)) && 
		    qobject_cast<WdgView*>(children().at(i_c))->select( ) )
		selRect = selRect.united(qobject_cast<WdgView*>(children().at(i_c))->geometry());
	//- Select childs anchors -
	if( !selRect.isNull() )
	{
	    if( grepAnchor(selRect.topLeft(),curp) )
	    { new_shp = Qt::SizeFDiagCursor; leftTop = true; }
	    else if( grepAnchor(selRect.bottomRight(),curp) )
		new_shp = Qt::SizeFDiagCursor;
	    else if( grepAnchor(selRect.bottomLeft(),curp) )
	    { new_shp = Qt::SizeBDiagCursor; leftTop = true; }
	    else if( grepAnchor(selRect.topRight(),curp) )
		new_shp = Qt::SizeBDiagCursor;
	    else if( grepAnchor(QPoint(selRect.center().x(),selRect.y()),curp) )
	    { new_shp = Qt::SizeVerCursor; leftTop = true; }
	    else if( grepAnchor(QPoint(selRect.center().x(),selRect.bottomRight().y()),curp) )
		new_shp = Qt::SizeVerCursor;
	    else if( grepAnchor(QPoint(selRect.x(),selRect.center().y()),curp) )
	    { new_shp = Qt::SizeHorCursor; leftTop = true; }
	    else if( grepAnchor(QPoint(selRect.bottomRight().x(),selRect.center().y()),curp) )
		new_shp = Qt::SizeHorCursor;
	    else if( selRect.contains(curp) )
	        new_shp = Qt::PointingHandCursor;
	    if( new_shp != Qt::ArrowCursor ) hold_child = true;
	}
	//- Widget geometry -
	if( new_shp == Qt::ArrowCursor )
	{
	    if( grepAnchor(rect().bottomRight(),curp) )
		new_shp = Qt::SizeFDiagCursor;		
	    else if( curp.x()>(rect().width()-4) && curp.x()<(rect().width()+4) )
	        new_shp = Qt::SizeHorCursor;
	    else if( curp.y()>(rect().height()-4) && curp.y()<(rect().height()+4) )
		new_shp = Qt::SizeVerCursor;
	    hold_child = false;
	}
	if( new_shp != cursor().shape() ) setCursor(new_shp);
    }
}

void WdgView::wdgViewTool( QAction *act )
{
    QStringList sact = act->objectName().split('_');
    if( sact.at(0) == "allign" )
    {
	//- Get selected rect -
	QRect selRect;
	int sel_cnt = 0;
	for( int i_c = 0; i_c < children().size(); i_c++ )
	{
	    WdgView *cwdg = qobject_cast<WdgView*>(children().at(i_c));
	    if( cwdg && cwdg->select( ) )
	    {
		selRect = selRect.united(cwdg->geometry());
		sel_cnt++;
	    }
	}
	if( sel_cnt == 0 ) return;
	if( sel_cnt == 1 ) selRect = selRect.united(rect());
	
	//- Update selected widgets position -
	for( int i_c = 0; i_c < children().size(); i_c++ )
	{
	    WdgView *cwdg = qobject_cast<WdgView*>(children().at(i_c));
	    if( cwdg && cwdg->select( ) )
	    {
		if( sact.at(1) == "left" )
		    cwdg->move(selRect.x(),cwdg->pos().y());
		else if( sact.at(1) == "right" )
		    cwdg->move(selRect.x()+selRect.width()-cwdg->width(),cwdg->pos().y());
		else if( sact.at(1) == "vcenter" )
		    cwdg->move(selRect.x()+(selRect.width()-cwdg->width())/2,cwdg->pos().y());		    
		else if( sact.at(1) == "top" )
		    cwdg->move(cwdg->pos().x(),selRect.y());
		else if( sact.at(1) == "bottom" )
		    cwdg->move(cwdg->pos().x(), selRect.y()+selRect.height()-cwdg->height());
		else if( sact.at(1) == "hcenter" )
		    cwdg->move(cwdg->pos().x(), selRect.y()+(selRect.height()-cwdg->height())/2);
	    }
	}
	saveData("");
    }
    else if( sact.at(0) == "level" )
    {
	bool is_rise = (sact.at(1) == "rise");
	bool is_up   = (sact.at(1) == "up");
	bool is_lower= (sact.at(1) == "lower");
	bool is_down = (sact.at(1) == "down");
	string sel_ws = selectChilds();
	string sel_w;
	
	int w_cnt=0;
	if( is_rise || is_up )
	    while( (sel_w=TSYS::strSepParse(sel_ws,w_cnt++,';')).size() )
	    {
		bool is_move = false;
		WdgView *cwdg = NULL;
		WdgView *ewdg = NULL;
		for( int i_c = 0; i_c < children().size(); i_c++ )
		{
		    if( !qobject_cast<WdgView*>(children().at(i_c)) )	continue;
		    ewdg = qobject_cast<WdgView*>(children().at(i_c));		    
		    if( ewdg->id() == sel_w.c_str() )	cwdg = ewdg;
		    else if( is_up && !is_move && cwdg && !ewdg->select() && 
			     ewdg->geometry().intersects(cwdg->geometry()) )
		    {
			cwdg->stackUnder(ewdg);
			ewdg->stackUnder(cwdg);
			cwdg->setZ(ewdg->z()+1);
			is_move = true;
		    }
		    else if( is_move )	ewdg->setZ(ewdg->z()+1);
		}
		if(is_rise && cwdg && ewdg && cwdg!=ewdg )
		{
		    cwdg->stackUnder(ewdg);
		    ewdg->stackUnder(cwdg);
		    cwdg->setZ(ewdg->z()+1);
		}
	    }
	w_cnt=0;
	if( is_lower || is_down )
	    while( (sel_w=TSYS::strSepParse(sel_ws,w_cnt++,';')).size() )
	    {
		bool is_move = false;
		WdgView *cwdg = NULL;
		WdgView *ewdg = NULL;
		for( int i_c = children().size()-1; i_c >= 0; i_c-- )
		{		    
		    if( !qobject_cast<WdgView*>(children().at(i_c)) )	continue;
		    ewdg = qobject_cast<WdgView*>(children().at(i_c));
		    if( ewdg->id() == sel_w.c_str() )	cwdg = ewdg;
		    else if( is_down && !is_move && cwdg && !ewdg->select() && 
			     ewdg->geometry().intersects(cwdg->geometry()) )
		    {
			cwdg->stackUnder(ewdg);
			cwdg->setZ(ewdg->z()-1);
			is_move = true;
		    }
		    else if( is_move )	ewdg->setZ(ewdg->z()-1);
		}
		if(is_lower && cwdg && ewdg && cwdg!=ewdg )
		{
		    cwdg->stackUnder(ewdg);
		    cwdg->setZ(ewdg->z()-1);
		}	    
	    }
	saveData("");
    }
}

bool WdgView::event( QEvent *event )
{
    //printf("TEST 00: Event %d \n",event->type());
    switch(event->type())
    {   
	case QEvent::Paint:
	{
	    QPainter pnt( this );
	    pnt.setWindow( rect() );
	    
	    //- Draw background for root widget -
	    if( develMode() && wLevel() == 0 )
	    {
		pnt.setPen("black");
		pnt.setBrush(QBrush(QColor("white")));
                pnt.drawRect(rect().adjusted(0,0,-1,-1));
	    }
	    //- Check widget -
	    if( !shape )
	    {
		if( develMode() )
		{
		    pnt.drawImage(rect(),QImage(":/images/attention.png"));
		    setToolTip(QString(_("Widget shape no support!")));
		}
		//event->accept();
		return true;
	    }
	    pnt.end();	//Close generic painter
	    
	    //- Self widget view -
	    shape->event(this,event);
	    
	    if( develMode() && wLevel() == 0 )
	    {	
		QRect rsel;
		for( int i_c = 0; i_c < children().size(); i_c++ )
		    if( qobject_cast<WdgView*>(children().at(i_c)) && 
			    qobject_cast<WdgView*>(children().at(i_c))->select( ) )		    
			rsel = rsel.united(qobject_cast<WdgView*>(children().at(i_c))->geometry());
		pnt_view->setSelArea(rsel);
		
		event->accept();
		return true;
	    }
	    break;
	}
	case QEvent::DragEnter:
	    if( develMode() && wLevel() == 0 )
	    {	
		QDragEnterEvent *ev = static_cast<QDragEnterEvent*>(event);
		if( ev->mimeData()->hasFormat("application/OpenSCADA-libwdg") )
		{
		    ev->acceptProposedAction();
		    emit selected(id());
		}
		return true;
	    }
	    break;
	case QEvent::Drop:
	    if( develMode() && wLevel() == 0 )
	    {
		QDropEvent *ev = static_cast<QDropEvent*>(event);
		if( ev->mimeData()->hasFormat("application/OpenSCADA-libwdg") )
		{
		    QByteArray itemData = ev->mimeData()->data("application/OpenSCADA-libwdg");
		    QDataStream dataStream(&itemData, QIODevice::ReadOnly);
		    
		    QString lwdg;
		    dataStream >> lwdg;
		    //-- Search need action --
		    QPoint curp = mapFromGlobal(cursor().pos());
		    VisDevelop *wdev = ((VisDevelop *)main_win);
		    for( int i_a = 0; i_a < wdev->actGrpWdgAdd->actions().size(); i_a++ )
			if( wdev->actGrpWdgAdd->actions().at(i_a)->objectName() == lwdg )
			    wdev->visualItAdd(wdev->actGrpWdgAdd->actions().at(i_a),curp);
		    ev->accept();				    
		}
		return true;
	    }
	    break;
	case QEvent::MouseButtonPress:
	{
	    QPoint curp = mapFromGlobal(cursor().pos());
	    //- Cancel new widget inserting -
	    if( develMode() && wLevel() == 0 )
	    {
		QAction *act = ((VisDevelop *)main_win)->actGrpWdgAdd->checkedAction();
		if( act && act->isChecked() )
		{
	    	    if( ((static_cast<QMouseEvent*>(event))->buttons()&Qt::RightButton) )
	    		act->setChecked(false);
		    else if( ((static_cast<QMouseEvent*>(event))->buttons()&Qt::LeftButton) )
			((VisDevelop *)main_win)->visualItAdd(act,curp);
		    
		    setCursor(Qt::ArrowCursor);
		    event->accept();
		    return true;
		}
	    }
	
	    //- Select widget -
	    if( develMode() && wLevel() == 0 && ((static_cast<QMouseEvent*>(event))->buttons()&Qt::LeftButton) )
	    {
		bool sh_hold = QApplication::keyboardModifiers()&Qt::ShiftModifier;	    
		if( cursor().shape() == Qt::ArrowCursor || sh_hold )
		{
		    //-- Scan childs --
		    bool sel_modif = false;
		    bool chld_sel = false;
		    WdgView *cwdg = NULL;
		    for( int i_c = children().size()-1; i_c >= 0; i_c-- )
		    {
			cwdg = qobject_cast<WdgView*>(children().at(i_c));
			if( !cwdg ) continue;
			if( cwdg->geometry().contains(curp) ) 
			{
			    if( !cwdg->select() ) { cwdg->setSelect(true); sel_modif = true; }
			    else if( cwdg->select() && sh_hold )
			    { cwdg->setSelect(false); sel_modif = true; }
			    if( cwdg->select() ) chld_sel = true;
			    break;
			}
		    }
		    //-- Select clean for childs --
		    if( !sh_hold )
			for( int i_c = 0; i_c < children().size(); i_c++ )
			{
			    WdgView *curw = qobject_cast<WdgView*>(children().at(i_c));
			    if( !curw || (chld_sel && (curw == cwdg)) )	continue;
			    if( curw->select() )	{ curw->setSelect(false); sel_modif = true; }
	    		}
		    if( sel_modif || !select() ) setSelect(true);
		    event->accept();
		    
		    upMouseCursors(mapFromGlobal(cursor().pos()));
		    
		    //-- Update status bar --
		    mainWin()->statusBar()->showMessage(QString(_("Select elements: '%1'")).
							arg(selectChilds().c_str()), 10000 );
		}
		if( cursor().shape() != Qt::ArrowCursor )
		{
		    moveHold = true;
		    hold_pnt = curp;
		}
		return true;
	    }
	    break;
	}
	case QEvent::MouseButtonRelease:
	    if( develMode() && moveHold )
	    {
		moveHold = false;
		if( cursor().shape() != Qt::ArrowCursor )
		    saveData("");
	    }
	    break;
	case QEvent::FocusIn:
	    if( develMode() && wLevel() == 0 )
	    {
		if(select()) setSelect(true);
		return true;
	    }
	    break;
	case QEvent::FocusOut:	
	    if( develMode() )
	    {
		if( cursor().shape() != Qt::ArrowCursor )
		    setCursor(Qt::ArrowCursor);
		if( QApplication::focusWidget() != this )
		{
		    setSelect(false,false);
		    //-- Unselect child widgets --
		    if( wLevel() == 0 && !((VisDevelop *)main_win)->attrInsp->hasFocus() )
			for( int i_c = 0; i_c < children().size(); i_c++ )
			    if( qobject_cast<WdgView*>(children().at(i_c)) )
				((WdgView*)children().at(i_c))->setSelect(false);
		}
		    //emit selected("");
		return true;
	    }
	    break;
	case QEvent::MouseMove:
	    if( develMode() && wLevel() == 0 )
	    {
		QPoint curp = mapFromGlobal(cursor().pos());
		
		if( ((VisDevelop *)main_win)->actGrpWdgAdd->checkedAction() && 
		    ((VisDevelop *)main_win)->actGrpWdgAdd->checkedAction()->isChecked() )
		{
		    setCursor(QCursor(((VisDevelop *)main_win)->actGrpWdgAdd->
				checkedAction()->icon().pixmap(64,64)));
		    event->accept();
		    return true;
		}
		
		upMouseCursors(curp);
		
		if( moveHold && cursor().shape() != Qt::ArrowCursor )
		{
		    if( hold_child )
		    {
			QPoint dP = curp-hold_pnt;
			//-- Update selected widgets geometry --			
			for( int i_c = 0; i_c < children().size(); i_c++ )
			{
			    WdgView *curw = qobject_cast<WdgView*>(children().at(i_c));
			    if( !curw || !curw->select() ) continue;
			    QRect  geom = curw->geometry();
			    switch(cursor().shape())
			    {
				case Qt::SizeFDiagCursor:
				    if( leftTop )
				    {
					curw->move(geom.x()+dP.x(), geom.y()+dP.y());
					curw->resize(geom.width()-dP.x(), geom.height()-dP.y());
				    }
				    else curw->resize(geom.width()+dP.x(), geom.height()+dP.y());
				    break;
				case Qt::SizeBDiagCursor:
				    if( leftTop )
					curw->setGeometry(geom.x()+dP.x(),geom.y(),
							  geom.width()-dP.x(), geom.height()+dP.y());
				    else
					curw->setGeometry(geom.x(),geom.y()+dP.y(),
					                  geom.width()+dP.x(), geom.height()-dP.y());
				    break;
				case Qt::SizeVerCursor:
				    if( leftTop )
					curw->setGeometry(geom.x(),geom.y()+dP.y(),
							  geom.width(), geom.height()-dP.y());
				    else curw->resize(geom.width(), geom.height()+dP.y());				    
				    break;				    
				case Qt::SizeHorCursor:
				    if( leftTop )
					curw->setGeometry(geom.x()+dP.x(),geom.y(),
							  geom.width()-dP.x(), geom.height());
				    else curw->resize(geom.width()+dP.x(), geom.height());
				    break;
				case Qt::PointingHandCursor:
				    curw->move(curw->pos()+dP);
				    break;
			    }
			}			
			hold_pnt = curp;
			update();
			//-- Set status bar --
			QRect srect;
			for( int i_c = 0; i_c < children().size(); i_c++ )
			    if( qobject_cast<WdgView*>(children().at(i_c)) && 
				    qobject_cast<WdgView*>(children().at(i_c))->select( ) )
				srect = srect.united(qobject_cast<WdgView*>(children().at(i_c))->geometry());
			mainWin()->statusBar()->showMessage(
			    QString(_("Elements: '%1' --- xy(%2:%3) wh[%4:%5]"))
				.arg(selectChilds().c_str())
				.arg(srect.x()).arg(srect.y())
				.arg(srect.width()).arg(srect.height()), 10000 );
		    }
		    else
		    {
			//- Change widget geometry -
			switch(cursor().shape())
			{
		    	    case Qt::SizeHorCursor:
				resize(curp.x(),size().height());
				break;
			    case Qt::SizeVerCursor:
				resize(size().width(),curp.y());
			        break;
			    case Qt::SizeFDiagCursor:
				resize(curp.x(),curp.y());
				break;	    
			}			
			//-- Set status bar --
			mainWin()->statusBar()->showMessage(
			    QString(_("Kadr: '%1' --- xy(%2:%3) wh[%4:%5]"))
				.arg(id().c_str()).arg(pos().x()).arg(pos().y())
				.arg(size().width()).arg(size().height()), 10000 );
		    }
		}
		event->accept();
        	return true;				    
	    }
	    break;
    }
    return QWidget::event(event);
}

//* Size points view widget              *
//****************************************
WdgView::SizePntWdg::SizePntWdg( QWidget* parent ) : QWidget(parent)
{
    //setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
}
			
void WdgView::SizePntWdg::setSelArea( const QRect &geom )
{
    if( geom.isValid() )
    {
	setGeometry(geom.adjusted(-3,-3,3,3));
	//- Make widget's mask -
	QRegion reg;
	for(int i_p = 0; i_p < 9; i_p++)
	    if( i_p != 4 )
		reg+=QRegion(QRect(rect().x()+(i_p%3)*((rect().width()-7)/2),
		    		   rect().y()+(i_p/3)*((rect().height()-7)/2),7,7));
	//reg = QRegion(rect()).subtracted(QRegion(rect().adjusted(7,7,-8,-8)));        
	setMask(reg);
    }
    else setGeometry(geom);	
}

void WdgView::SizePntWdg::paintEvent ( QPaintEvent *event )
{
    if( rect().isValid() )
    {
	QPainter pnt( this );
	pnt.setWindow( rect() );
	
	pnt.setPen("black");
	pnt.setBrush(QBrush(QColor("lightgreen")));
	for(int i_p = 0; i_p < 9; i_p++)
	{
	    if( i_p == 4 ) continue;
	    QRect anch(rect().x()+(i_p%3)*((rect().width()-7)/2),
		       rect().y()+(i_p/3)*((rect().height()-7)/2),6,6);
	    pnt.drawRect(anch);
	}
    }
}
