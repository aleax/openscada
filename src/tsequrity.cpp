#include "xml.h"
#include "tsequrity.h"

const char *TSequrity::o_name = "TSequrity";
const char *TSequrity::s_name = "Sequrity";
const char *TSequrity::i_cntr = 
	"<oscada_cntr>"
	"</oscada_cntr>";

TSequrity::TSequrity( TKernel *app ) : owner(app), TContr( i_cntr ), m_hd_usr(o_name), m_hd_grp(o_name)
{
    //Add surely users, groups and set parameters
    usr_add("root");
    unsigned hd = usr_att("root");
    usr_at(hd).Descr("Administrator (superuser)!!!");
    usr_at(hd).Pass("openscada");
    usr_det(hd);    
    
    grp_add("root");
    hd = grp_att("root");
    grp_at(hd).Descr("Administrators group.");
    grp_det(hd);    
}

TSequrity::~TSequrity(  )
{
    vector<string> list;

    //Free groups
    m_hd_grp.lock();
    grp_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
        grp_del(list[i_ls]);    	
    //Free users list
    m_hd_usr.lock();
    usr_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
	usr_del(list[i_ls]);    
}

void TSequrity::usr_add( string name )
{    
    TUser *user = new TUser(this,name,usr_id_f());
    try{ m_hd_usr.obj_add( user, &user->Name() ); }
    catch(TError err) {	delete user; }
}

void TSequrity::grp_add( string name )
{
    TGroup *grp = new TGroup(this,name,grp_id_f());
    try{ m_hd_grp.obj_add( grp, &grp->Name() ); }
    catch(TError err) {	delete grp; }
}

unsigned TSequrity::usr_id_f()
{
    unsigned id = 0;
    vector<string> list;
    usr_list(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	unsigned hd = usr_att( list[i_l] );
	if( usr_at(hd).Id() == id ) { id++; i_l=-1; }
	usr_det(hd);	    
    }
    return(id);
}

unsigned TSequrity::grp_id_f()
{
    unsigned id = 0;
    vector<string> list;
    grp_list(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
    {
	unsigned hd = grp_att( list[i_l] );
	if( grp_at(hd).Id() == id ) { id++; i_l=-1; }
	grp_det(hd);	    
    }
    return(id);
}

bool TSequrity::access( string user, char mode, int owner, int group, int access )
{
    bool p_r = false, p_w = false, p_x = false;

    try
    {
    	int hd = usr_att(user);
	// Check owner permision
	if( usr_at(hd).Id() == 0 || usr_at(hd).Id() == owner )
	    if( ((mode&SEQ_RD)?access&0400:true) && 
		((mode&SEQ_WR)?access&0200:true) && 
		((mode&SEQ_XT)?access&0100:true) )
	    {
		usr_det(hd);
		return(true);
	    }	
	// Check groupe permision
	
	// Check other permision
	if( ((mode&SEQ_RD)?access&0004:true) && 
	    ((mode&SEQ_WR)?access&0002:true) && 
	    ((mode&SEQ_XT)?access&0001:true) )
	{
	    usr_det(hd);
	    return(true);
	}	
	usr_det(hd);
    }catch(...){ return(false); }

    return(false);
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TSequrity::ctr_fill_info( XMLNode *inf )
{
    inf->set_text(Name());
}

//**************************************************************
//*********************** TUser ********************************
//**************************************************************
TUser::TUser( TSequrity *owner, string name, unsigned id ) : m_name(name), m_id(id), m_owner(owner)
{

}

TUser::~TUser(  )
{

}

//**************************************************************
//*********************** TGroup *******************************
//**************************************************************
TGroup::TGroup( TSequrity *owner, string name, unsigned id ) : m_name(name), m_id(id), m_owner(owner)
{

}

TGroup::~TGroup(  )
{

}
