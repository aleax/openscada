#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tuis.h"

//================================================================
//================== TUIS ========================================
//================================================================
const char *TUIS::o_name = "TUIS";
const char *TUIS::i_cntr = 
    "<area id='a_gn' acs='0440'>"
    " <fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
    "</area>";
	
TUIS::TUIS( TKernel *app ) : TGRPModule(app,"UI") 
{
    s_name = "User interfaces"; 
}

string TUIS::opt_descr( )
{
    return(Mess->I18N(
    	"===================== The user interface subsystem options ================\n"
	"    --GUIModPath=<path>  Set moduls <path>;\n"));
}

void TUIS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );

    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"GUIModPath" ,1,NULL,'m'},
	{NULL         ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
//    if(optind < App->argc) pr_opt_descr(stdout);
}

void TUIS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();

}

void TUIS::gmd_Start( )
{
    vector<string> list;
    gmd_list(list);
    for(unsigned i_sp = 0; i_sp < list.size(); i_sp++)
    {
	unsigned hd = gmd_att(list[i_sp]);
	gmd_at(hd).start( );
	gmd_det(hd);
    }
}

void TUIS::gmd_Stop( )
{
    vector<string> list;
    gmd_list(list);
    for(unsigned i_sp = 0; i_sp < list.size(); i_sp++)
    {
	unsigned hd = gmd_att(list[i_sp]);
	gmd_at(hd).stop( );
	gmd_det(hd);
    }
}

//=========== Control ==========================================
void TUIS::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    TGRPModule::ctr_fill_info( inf );
    
    XMLNode *n_add = inf->add_child();
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("Subsystem control"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Options help"));
}

void TUIS::ctr_din_get_( string a_path, XMLNode *opt )
{
    TGRPModule::ctr_din_get_( a_path, opt );
    
    string t_id = ctr_path_l(a_path,0);
    if( t_id == "a_gn" )
    {
	t_id = ctr_path_l(a_path,1);
	if( t_id == "g_help" ) ctr_opt_setS( opt, opt_descr() );       
    }   
}
//================================================================
//================== TUI =========================================
//================================================================
const char *TUI::o_name = "TUI";
const char *TUI::i_cntr = 
    "<area id='a_prm'>"
    " <fld id='r_st' acs='0664' tp='bool'/>"
    "</area>";

TUI::TUI() : run_st(false)
{

}
    
//================== Controll functions ========================
void TUI::ctr_fill_info( XMLNode *inf )
{
    char *dscr = "dscr";
    TModule::ctr_fill_info( inf );
    
    XMLNode *n_add = inf->add_child();
    n_add->load_xml(i_cntr);
    n_add->set_attr(dscr,Mess->I18N("User interface control"));
    n_add->get_child(0)->set_attr(dscr,Mess->I18N("Runing"));
}

void TUI::ctr_din_get_( string a_path, XMLNode *opt )
{
    TModule::ctr_din_get_( a_path, opt );
    
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "r_st" )  ctr_opt_setB( opt, run_st );
    }    
}

void TUI::ctr_din_set_( string a_path, XMLNode *opt )
{
    TModule::ctr_din_set_( a_path, opt );
    
    string t_id = ctr_path_l(a_path,0);    
    if( t_id == "a_prm" )
    {
    	t_id = ctr_path_l(a_path,1);
    	if( t_id == "r_st" )
	    if( ctr_opt_getB( opt ) ) start(); else stop();
    }    
}

