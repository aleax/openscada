#ifndef QT_GUI_H
#define QT_GUI_H

//#include <qapplication.h>
#include <qmainwindow.h>

#include "../../tuis.h"

namespace QT_GUI
{
    class TUIMod: public TUI
    {
	public:
	    TUIMod( string name );
	    ~TUIMod();

	    void Start();
	    void Stop();

	    void mod_connect(  );
	    void mod_CheckCommandLine( );
	public:
    
	private:
	    void pr_opt_descr( FILE * stream );
	    string mod_info( const string name );
	    void   mod_info( vector<string> &list );

	    static void *Task(void *);
	private:
	    pthread_t pthr_tsk;
	
    	    bool      run_st;      // Stat of task
    };

    class ConfApp: public QMainWindow
    {
        Q_OBJECT

	public:
	    ConfApp( );
	    ~ConfApp();

	protected:
	    void closeEvent( QCloseEvent* );

	private slots:
	    void about();
	    void aboutQt();
	    void conf();
	    void w_place();
    };
    
    class AWPApp: public QMainWindow
    {
        Q_OBJECT

	public:
	    AWPApp( );
	    ~AWPApp();

	protected:
	    void closeEvent( QCloseEvent* );

	private slots:
	    void about();
	    void aboutQt();
	    void conf();
	    void w_place();
    };
    /*
    class NewWidget : public QWidget
    {
	public:
    	    NewWidget( QWidget *parent=0, const char *name=0 );
	    ~NewWidget();
    };
    */
}
#endif //QT_GUI_H

