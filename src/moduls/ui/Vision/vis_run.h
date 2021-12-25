
//OpenSCADA module UI.Vision file: vis_run.h
/***************************************************************************
 *   Copyright (C) 2007-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef VIS_RUN_H
#define VIS_RUN_H

#include <QMainWindow>
#include <QThread>
#include <QLabel>
#include <QFileDialog>
#include <QPrinter>
#include <QMenu>

#include "tvision.h"

namespace VISION
{

//***********************************************
// SCADAHost - Host thread's control object     *
class VisRun;

class SCADAHost : public QThread
{
    Q_OBJECT

    public:
    //Methods
    SCADAHost( QObject *parent = 0 );
    ~SCADAHost( );

    void sendSIGALRM( );

    // To the thread request function, return ready status (true).
    // First-init request will cause short waiting at the condition variable and next only the ready status return
    bool reqDo( XMLNode &node, bool &done, bool glob = false );
    // Only checking to done for <node>.
    bool reqBusy( );

    //Attributes
    int inHostReq;

    protected:
    //Methods
    void run( );

    private:
    //Methods
    VisRun *owner( ) const;

    //Attributes
    ResMtx	mtx;
    CondVar	cond;

    bool	endRun, reqDone, glob;

    XMLNode	*req;
    bool	*done;
    pthread_t	pid;	//Thread id
};

class UserStBar;
class StylesStBar;
class RunPageView;
class RunWdgView;

class VisRun : public QMainWindow
{
    friend class RunPageView;
    Q_OBJECT
    public:
	//Data
	//* Notify: Generic notifying object.					*
	//***********************************************************************
	class Notify {
	    public:
		//Data
		enum IntFuncAttrIdxs { IFA_en = 0, IFA_doNtf, IFA_doRes, IFA_res, IFA_mess, IFA_lang, IFA_resTp, IFA_prcID };

		//Methods
		explicit Notify( ) : tp(-1), comIsExtScript(false),
		    f_notify(false), f_resource(false), f_queue(false), f_quietanceRet(false), delay(0), mOwner(NULL), actAlrm(NULL)	{ }
		Notify( uint8_t tp, const string &pgProps, VisRun *own );
		~Notify( );

		string	pgCrtor( );
		string	props( );

		bool hasQueue( )	{ return f_queue; }
		bool hasQuietanceRet( )	{ return f_quietanceRet; }
		string curQueueWdg( );

		void ntf( int alrmSt );	//Same notify for the alarm status
		string ntfRes( string &resTp, string &mess, string &lang );	//The notification resource request

		//Attributes
		string	pgProps;			//Page-creator and its properties
		vector<string> pgPropsQ;		//Page-creators queue, for notificators per page

	    private:
		//Methods
		void commCall( string &res, string &resTp, const string &mess = "", const string &lang = "" );

		VisRun *owner( ) const	{ return mOwner; }

		static void *Task( void *ntf );

		//Attributes
		int8_t	tp;			//Type
		unsigned alSt;			//Alarm state
		int	repDelay;		//Repeate delay, in seconds. -1 by default for disabled repeating
		unsigned comIsExtScript	:1;	//The command detected and used as some intepretator's script like BASH, Perl, PHP and so on.
		// Flags
		unsigned f_notify	:1;	//Notification enabled
		unsigned f_resource	:1;	//Request the resource for notification: sound file, text or other data
		unsigned f_queue	:1;	//Use queue of notifications by the priority-level
		unsigned f_quietanceRet	:1;	//Return quietance mode - enable/disable the notification

		unsigned toDo		:1;	//Need to do some notification doings
		unsigned alEn		:1;	//Alarm enabled
		string	comProc;		//Command procedure name

		float	delay;
		string	resFile;

		unsigned queueCurTm;
		string	queueCurPath;

		ResMtx	dataM;
		pthread_cond_t	callCV;
		VisRun	*mOwner;
		QAction	*actAlrm;
		QWidget	*ntfPlay;
	};

	//Public methods
	VisRun( const string &prjSes_it, const string &open_user, const string &user_pass,
	    const string &VCAstat, bool crSessForce = false, unsigned screen = 0 );
	~VisRun( );

	string	user( );
	string	lang( );
	string	password( );
	string	VCAStation( );
	int	period( )	{ return mPeriod; }
	string	workSess( )	{ return work_sess; }
	unsigned screen( )	{ return mScreen; }
	string	srcProject( )	{ return src_prj; }
	float	xScale( )	{ return x_scale; }
	float	yScale( )	{ return y_scale; }
	bool keepAspectRatio( )	{ return mKeepAspectRatio; }
	bool winPosCntrSave( )	{ return mWinPosCntrSave; }
	unsigned reqTm( )	{ return reqtm; }
	int	style( );
	bool	connOK( )	{ return !conErr; }
	QAction *aFullScr( )	{ return actFullScr; }

	void	setFocus( const string &addr, bool ack = false );

	bool	winMenu( );
	void	setWinMenu( bool act );

	bool userSel( const string &hint = "" );
	void setXScale( float vl )	{ x_scale = vl; }
	void setYScale( float vl )	{ y_scale = vl; }
	void setKeepAspectRatio( bool vl )	{ mKeepAspectRatio = vl; }
	void setWinPosCntrSave( bool vl)	{ mWinPosCntrSave = vl; }
	void setReqTm( unsigned rt )	{ reqtm = rt; }
	void setStyle( int istl );

	void messUpd( );
	void initSess( const string &prjSes_it, bool crSessForce = false );	//Init session for project's item path
	void callPage( const string &ses_it, bool updWdg = false );		//Call session page
	void fullUpdatePgs( );

	// Page cache commands
	void pgCacheClear( );
	void pgCacheAdd( RunPageView *wdg );
	RunPageView *pgCacheGet( const string &id );

	// Attributes commands
	string wAttrGet( const string &path, const string &attr, bool sess = false );
	bool wAttrSet( const string &path, const string &attr, const string &val, bool sess = false );

	RunPageView *masterPg( )				{ return master_pg; }
	RunPageView *findOpenPage( const string &pg );
	RunWdgView *findOpenWidget( const string &wdg );

	// Control requests
	void initHost( );
	int cntrIfCmd( XMLNode &node, bool glob = false, bool main = false );

	QString getFileName(const QString &caption, const QString &dir, const QString &filter, QFileDialog::AcceptMode mode = QFileDialog::AcceptOpen);

	void usrStatus( const string &val, RunPageView *pg = NULL );

	int cachePgSz( )	{ return cachePg.size(); }

	// Resource cache commands
	int cacheResSz( )	{ return mCacheRes.size(); }
	string cacheResGet( const string &res );
	void cacheResSet( const string &res, const string &val );

	// Alarms-notification processing
	unsigned alarmSt( )					{ return mAlrmSt; }
	char alarmTp( char tmpl, bool quietance = false )	{ return (mAlrmSt>>(quietance?16:8)) & tmpl; }
	int  alarmLev( )					{ return mAlrmSt & 0xFF; }
	void alarmSet( unsigned alarm );
	//  Notification type <tp> register for no empty <props> else unregister, from the page-creator <pgCrtor>
	void ntfReg( int8_t tp, const string &props, const string &pgCrtor, bool prior = true );

	//Public attributes
	bool winClose;					//Closing window flag
	bool isResizeManual;				//Manual resizing flag

	float	updTmMax, planePer;

    signals:
	void makeStarterMenu( );

    protected:
	//Protected methods
	void closeEvent( QCloseEvent* );		//Close run-time window event
	void resizeEvent( QResizeEvent * );		//Resize window event

    public slots:
	//Public slots
	void print( );					//Print master page
	void printPg( const string &pg = "" );		//Print select page
	void printDiag( const string &dg = "" );	//Print select diagram
	void printDoc( const string &doc = "" );	//Print select document
	void exportDef( );				//Export master page
	void exportPg( const string &pg = "" );		//Export select page
	void exportDiag( const string &dg = "" );	//Export select diagram
	void exportDoc( const string &doc = "" );	//Export select document
	void exportTable( const string &tbl = "" );	//Export select table

    private slots:
	//Private slots
	void quitSt( );					//Full quit OpenSCADA

	void fullScreen( bool vl );			//Full screen toggle

	void userChanged( const QString &oldUser, const QString &oldPass );	//User changed
	void styleChanged( );

	void about( );					//About at program
	void aboutQt( );				//About at QT library
	void enterManual( );				//Manuals call
	void enterWhatsThis( );				//What is GUI components
	void updatePage( );				//Update page data
	void endRunChk( );				//End run flag check

	void alarmAct( QAction *alrm );			//Alarm actions process

    private:
	//Data
	class CacheEl
	{
	    public:
		CacheEl( time_t itm, const string &ival ) : tm(itm), val(ival)	{ }
		CacheEl( ) : tm(0)	{ }
		time_t	tm;
		string	val;
	};
	//Private attributes
	// Menu root items
	QMenu	menuFile,			//Menu "File"
		menuAlarm,			//Menu "Alarm"
		menuView,			//Menu "View"
		menuHelp,			//Menu "Help"
		*menuPrint,			//Menu "Print"
		*menuExport;			//Menu "Export"

	// Tool bars
	QToolBar	*toolBarStatus;		//Status toolbar

	// Actions
	QAction	*actClose,			//Close
		*actQuit,			//Quit
		*actFullScr,			//Full screen
		*actPrintPg,			//Print the selected page
		*actPrintDiag,			//Print the selected diagram
		*actPrintDoc,			//Print the selected document
		*actExpPg,			//Export the selected page
		*actExpDiag,			//Export the selected diagram
		*actExpDoc,			//Export the selected document
		*actExpTable,			//Export the selected table
		*actAbout,			//About
		*actQtAbout,			//About Qt
		*actProjManual,			//The project manual
		*actManual,			//The module manual
		*actManualSYS,			//OpenSCADA manual
		*actWhatIs,			//What is
		*actAlrmLev;			//Alarm level

	// Main components
	QTimer		*endRunTimer, *updateTimer;
#ifndef QT_NO_PRINTER
	QPrinter	*prPg, *prDiag, *prDoc;
#endif
	QFileDialog	*fileDlg;

	UserStBar	*mWUser;		//User status widget
	StylesStBar	*mStlBar;		//Style status widget
	QLabel		*mWStat;		//VCA engine station
	QLabel		*mWTime;		//Run-time time display for fullscreen
	QLabel		*conErr;		//Connection error label
	bool		crSessForce;		//Force session creation flag
	bool		mKeepAspectRatio;	//Keep aspect ratio on scale
	bool		mWinPosCntrSave;	//Windows position control and save
	string 		prjSes_it,		//Project session item
			work_sess,		//Work session
			src_prj;		//Source project
	string		focusWdf;
	RunPageView	*master_pg;		//Master page of runtime session
	int		mPeriod;		//Clock's period
	int		mConId;			//The connection to VCA ID
	unsigned	mScreen,		//Work screen, possible virtual
			wPrcCnt;		//Process counter
	unsigned	reqtm;			//Requested time
	unsigned	expDiagCnt, expDocCnt, expTblCnt;

	float		x_scale, y_scale;	//RunTime scaling

	// Alarm attributes
	unsigned	mAlrmSt;		//Alarm status
	bool		alrLevSet;		//For no quietance lamp blinking
	unsigned	ntfSet;			//Allowed notificators set mask
	map<uint8_t, Notify*>	mNotify;	//Notificators
	float		alrmUpdCnt;

	vector<string>	pgList;			//Pages list

	// Page and resource cache
	deque<RunPageView*>	cachePg;	//Pages cache
	map<string,CacheEl>	mCacheRes;	//Resources cache

	bool		updPage;
	SCADAHost	*host;
};

}

#endif //VIS_RUN
