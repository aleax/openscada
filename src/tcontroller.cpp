
#include "tmessage.h"
#include "tcontrollers.h"
#include "ttiparam.h"
#include "tparamcontr.h"
#include "tcontroller.h"

const char *TController::o_name = "TController";

//==== TController ====
TController::TController( string name_c, SBDS bd, TTipController *tcntr, TConfigElem *cfgelem ) : 
	    name(name_c), m_bd(bd), owner(tcntr), TConfig(cfgelem), stat(0), m_hd(o_name)
{
    cf_Set_S("NAME",name_c);    
}

TController::~TController(  )
{
    vector<string> c_list;
    
    try{ Stop( ); }catch(...){ }
    try{ Free( ); }catch(...){ }
    m_hd.lock();
    list(c_list);
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
	del(c_list[i_ls]);
    /*
    while(cntr_prm.size())
    {
	delete cntr_prm[0];
	cntr_prm.erase(cntr_prm.begin());
    }
    */
}

void TController::Load( )
{
    TBDS &bds = Owner().Owner().Owner().BD();
    if( stat&TCNTR_ENABLE ) 
    {
#if OSC_DEBUG
    	Owner().m_put("DEBUG",MESS_INFO,"%s: Load controller's configs!",Name().c_str());	
#endif   

	SHDBD i_hd = bds.open( m_bd );
	cf_Set_S("NAME",name);
	cf_LoadValBD("NAME",bds.at(i_hd));
	bds.close(i_hd);	
	
	LoadParmCfg( );
	
#if OSC_DEBUG
    	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Load controller's configs ok!",Name().c_str());	
#endif  
    }
    else throw TError("%s: Controller %s no enable!",o_name,Name().c_str());    
}

void TController::Save( )
{
    TBDS &bds = Owner().Owner().Owner().BD();
    if( stat&TCNTR_ENABLE ) 
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Save controller's configs!",Name().c_str());	
#endif 

	SaveParmCfg( );
	
        SHDBD i_hd;	
	try{ i_hd = bds.open( m_bd ); }
	catch(...){ i_hd = bds.open( m_bd, true ); }	
	owner->cfe_UpdateBDAttr( bds.at(i_hd) );
	cf_SaveValBD("NAME",bds.at(i_hd));
	bds.at(i_hd).Save();
	bds.close(i_hd);
	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Save controller's configs ok!",Name().c_str());	
#endif 
    }
    else throw TError("%s: Controller %s no enable!",o_name,Name().c_str());    
} 

void TController::Free(  )
{
    if( stat&TCNTR_ENABLE ) 
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Free controller's configs!",Name().c_str());
#endif 

	FreeParmCfg( );
	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Free controller's configs ok!",Name().c_str());
#endif 
    }
    else throw TError("%s: Controller %s no enable!",o_name,Name().c_str());
}    

void TController::Start( )
{        
    if( stat&TCNTR_ENABLE && !(stat&TCNTR_RUN) )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Start controller!",Name().c_str());
#endif 	

	//Set valid all parameter	
	/*	    
	for(unsigned i_p=0; i_p < cntr_prm.size(); i_p++)
	    cntr_prm[i_p]->Enable();
	*/
	stat|=TCNTR_RUN;
	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Start controller ok!",Name().c_str());
#endif 	
    }
    else if( stat&TCNTR_RUN ) return;
    else throw TError("%s: Controller %s no enable!",o_name,Name().c_str());
}

void TController::Stop( )
{
    if( stat&TCNTR_RUN )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Stop controller!",Name().c_str());
#endif	
    	stat&=(~TCNTR_RUN);
	//Set novalid all parameter
	/*
	for(unsigned i_p=0; i_p < cntr_prm.size(); i_p++)
	    cntr_prm[i_p]->Disable();		    
	*/
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Stop controller ok!",Name().c_str());
#endif	
    }
}

void TController::Enable( )
{
    if( !(stat&TCNTR_ENABLE) )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Enable controller!",Name().c_str());
#endif

	stat|=TCNTR_ENABLE;
    	Load( );
	RegParamS();
	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Enable controller ok!",Name().c_str());
#endif
    }
}

void TController::Disable( )
{
    if( stat&TCNTR_ENABLE )
    {
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_INFO,"%s: Disable controller!",Name().c_str());
#endif

	if( stat&TCNTR_RUN ) Stop( );
	UnRegParamS();	
	Free( );
	stat&=(~TCNTR_ENABLE);
	
#if OSC_DEBUG
	Owner().m_put("DEBUG",MESS_DEBUG,"%s: Disable controller ok!",Name().c_str());
#endif
    }
}

void TController::LoadParmCfg(  )
{
    string      parm_bd;
    TParamContr *PrmCntr;

    TBDS    &bds  = Owner().Owner().Owner().BD();    
    TParamS &prms = Owner().Owner().Owner().Param();    
    
    time_t tm = time(NULL);
    for(unsigned i_tp = 0; i_tp < owner->SizeTpPrm(); i_tp++)
    {
	SHDBD t_hd = bds.open( SBDS( m_bd.tp, m_bd.bd, cf_Get_S(owner->at_TpPrm(i_tp).BD()) ) );	
	for(unsigned i=0; i < (unsigned)bds.at(t_hd).NLines( ); i++)
	{
	    try
	    {
		string n_prm = bds.at(t_hd).GetCellS( bds.at(t_hd).ColumNameToId("SHIFR"),i);
		add( n_prm, owner->at_TpPrm(i_tp).Name() );
		int p_hd = att(n_prm);
		at(p_hd).cf_LoadValBD(i,bds.at(t_hd));
		at(p_hd).UpdateVAL( );
		det(p_hd);		
	    }catch(...) { }
	}
	bds.close(t_hd);
    }
    /*
    //Check freeing param
    for(unsigned i_prm=0; i_prm < cntr_prm.size(); i_prm++)
	if( tm > cntr_prm[i_prm]->t_sync )
	{
	    prms.Del(cntr_prm[i_prm]);
	    HdFree(i_prm);		
	    delete cntr_prm[i_prm];
	    cntr_prm.erase(cntr_prm.begin()+i_prm);
	    i_prm--;
	}
    */
}

void TController::SaveParmCfg(  )
{
    TBDS    &bds  = Owner().Owner().Owner().BD();    
    for(unsigned i_tp = 0; i_tp < owner->SizeTpPrm(); i_tp++)
    {
    	string parm_tbl = cf_Get_S(owner->at_TpPrm(i_tp).BD());
    
	//Update BD (resize, change atributes ..
	SHDBD t_hd;
	try{ t_hd = bds.open( SBDS(m_bd.tp, m_bd.bd, parm_tbl) ); }
	catch(...){ t_hd = bds.open( SBDS(m_bd.tp, m_bd.bd, parm_tbl),true ); }    
	
    	owner->at_TpPrm(i_tp).cfe_UpdateBDAttr(bds.at(t_hd));
	//Clear BD
	while(bds.at(t_hd).NLines( )) bds.at(t_hd).DelLine(0);

	vector<string> c_list;    
    	list(c_list);
	for( unsigned i_ls = 0, i_bd=0; i_ls < c_list.size(); i_ls++)
	{
	    int p_hd = att(c_list[i_ls]);
	    if(at(p_hd).Type().Name() == owner->at_TpPrm(i_tp).Name() )
	    {	
    		i_bd = bds.at(t_hd).AddLine(i_bd);
    		at(p_hd).cf_SaveValBD(i_bd,bds.at(t_hd));
		i_bd++;
	    }
	    det(p_hd);
	}
	
	bds.at(t_hd).Save( );
	bds.close(t_hd);
    }
}

void TController::FreeParmCfg(  )
{
    vector<string> c_list;    
    list(c_list);
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
    {
    	try{ UnRegParam(c_list[i_ls]); } catch(...){ }
	del( c_list[i_ls] );
    }
}

void TController::RegParam( string name )
{
    Owner().Owner().Owner().Param().add( SCntrS( Owner().mod_Name(), Name()), name ); 
}

void TController::RegParamS()
{
    vector<string> c_list;    
    list(c_list);
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
	RegParam(c_list[i_ls]);
}

void TController::UnRegParam( string name )
{ 
    Owner().Owner().Owner().Param().del( SCntrS( Owner().mod_Name(), Name()), name);
}

void TController::UnRegParamS()
{
    vector<string> c_list;    
    list(c_list);
    for( unsigned i_ls = 0; i_ls < c_list.size(); i_ls++)
       UnRegParam(c_list[i_ls]);
}

void TController::add( string name, string type, int pos )
{
    TParamContr *PrmCntr = ParamAttach( name, owner->NameTpPrmToId(type) );
    try
    {
	m_hd.obj_add( PrmCntr, &PrmCntr->Name(), pos );
    }
    catch(TError err) { delete PrmCntr; }
}

void TController::del( string name )
{ 
    try{ UnRegParam(name); } catch(...){ }
    delete (TParamContr *)m_hd.obj_del( name ); 
}

TParamContr *TController::ParamAttach( string name, int type)
{ 
    return(new TParamContr(name, &owner->at_TpPrm(type), this)); 
}


