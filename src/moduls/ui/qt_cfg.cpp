/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>
#include <unistd.h>

#include <qpushbutton.h>
#include <qvbox.h>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "qt_cfg.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "QT_cfg"
#define NAME_TYPE   "GUI"
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "QT based OpenSCADA configurator."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    TModule *attach( char *FName, int n_mod )
    {
	QT_cfg::TCfgMod *self_addr;
	if(n_mod==0) self_addr = new QT_cfg::TCfgMod( FName );
	else         self_addr = NULL;
	return ( self_addr );
    }
}

using namespace QT_cfg;

//==============================================================================
//================= QT_cfg::TCfgMod ============================================
//==============================================================================

TCfgMod::TCfgMod(char *name) : run_st(false)
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = strdup(name);
}

TCfgMod::~TCfgMod()
{
    free(FileName);	
}

void TCfgMod::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "==================== %s options =================================\n"
    "\n",NAME_MODUL);
}

void TCfgMod::mod_CheckCommandLine( )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TCfgMod::mod_connect(  )
{
    TModule::mod_connect(  );
}

void TCfgMod::Start()
{
    pthread_attr_t pthr_attr;
    
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    sleep(1);    
    if(run_st == false) throw TError("%s: Configurator start error!",NAME_MODUL);
}

void TCfgMod::Stop()
{

}

void *TCfgMod::Task( void *CfgM )
{
    TCfgMod *Cfg = (TCfgMod *)CfgM;

    Cfg->run_st = true;

    QApplication app1(0, NULL);

    NewWidget NWidg;
    NWidg.setGeometry( 100, 100, 200, 120 );
    app1.setMainWidget( &NWidg );
    NWidg.show();
    
    app1.exec();
}


NewWidget::NewWidget( QWidget *parent, const char *name ) : QWidget( parent, name )
{
    setMinimumSize( 500, 120 );
    setMaximumSize( 500, 120 );

    QPushButton *quit = new QPushButton( "First testing use QT lib!", this, "test" );
    quit->setGeometry( 62, 40, 300, 30 );
    quit->setFont( QFont( "Times", 14, QFont::Bold ) );

    connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );
}

