#ifndef QT_CFG_H
#define QT_CFG_H

#include <qapplication.h>

#include "../../tguis.h"

namespace QT_cfg
{
    class TCfgMod: public TGUI
    {
	public:
	    TCfgMod(char *name);
	    ~TCfgMod();

	    void Start();
	    void Stop();

	    void mod_connect(  );
	    void mod_CheckCommandLine( );
	public:
    
	private:
	    void pr_opt_descr( FILE * stream );

	    static void *Task(void *);
	private:
	    pthread_t pthr_tsk;
	
    	    bool      run_st;      // Stat of task
    };


    class NewWidget : public QWidget
    {
	public:
    	    NewWidget( QWidget *parent=0, const char *name=0 );
    };
    
}
#endif //QT_CFG_H

