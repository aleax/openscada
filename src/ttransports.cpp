#include <getopt.h>

#include "tsys.h"
#include "tkernel.h"
#include "tmessage.h"
#include "tmodule.h"
#include "ttransports.h"

//================================================================
//=========== TTransportS ========================================
//================================================================

const char *TTransportS::o_name = "TTransportS";
const char *TTransportS::n_opt  = "transport";

TTransportS::TTransportS( TKernel *app ) : TGRPModule(app,"Transport")
{

}

TTransportS::~TTransportS(  )
{
    for(unsigned i_m = 0; i_m < TTransport.size(); i_m++) DelM(i_m);
}

void TTransportS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= %s options ================================\n"
    "    --TRMPath=<path>  Set moduls <path>;\n"
    "------------------ Fields <%s> sections of config file ----------------\n"
    "modules_path=<path>    set path to modules;\n"	    
    "\n",NameTMod().c_str(),n_opt);
}

void TTransportS::CheckCommandLine( )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"TRMPath" ,1,NULL,'m'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TTransportS::UpdateOpt()
{
    string opt;
    if( SYS->GetOpt(n_opt,"modules_path",opt) ) DirPath = opt;
}

int TTransportS::AddM( TModule *modul )
{
    int hd=TGRPModule::AddM(modul);
    if(hd < 0) return(hd);
    if(hd == (int)TTransport.size()) TTransport.push_back( static_cast< TTipTransport *>(modul) );
    else if(TTransport[hd]==TO_FREE) TTransport[hd] = static_cast< TTipTransport *>(modul);
    TTransport[hd]->owner = this;
    return(hd);
}

void TTransportS::DelM( unsigned hd )
{
    if(hd >= TTransport.size() || TTransport[hd]==TO_FREE) 
    	throw TError("%s: Module header %d error!",o_name,hd);	    
    TTransport[hd] = TO_FREE;
    TGRPModule::DelM(hd);
}

int TTransportS::OpenIn( string t_name, string tt_name, string address )
{
    unsigned id, id_tptrans, id_trans;
    id_tptrans = NameToId(tt_name);
    id_trans   = TTransport[id_tptrans]->OpenIn(t_name,address);
    //Find dublicate
    for(id = 0; id < TranspIn.size(); id++)
	if( TranspIn[id].use > 0 && TranspIn[id].type_tr == id_tptrans && TranspIn[id].tr == id_trans )
	    break;
    if( id < TranspIn.size() ) TranspIn[id].use++;
    else
    {
	for( id = 0; id < TranspIn.size(); id++ )
	    if(TranspIn[id].use <= 0) break;
	if( id == TranspIn.size() ) TranspIn.push_back();
	TranspIn[id].use  = 1;
	TranspIn[id].type_tr = id_tptrans;
	TranspIn[id].tr      = id_trans;
    }
    return(id);    
}

void TTransportS::CloseIn( unsigned int id )
{
    if(id > TranspIn.size() || TranspIn[id].use <= 0) 
	throw TError("%s: transport identificator error!",o_name);
    TTransport[TranspIn[id].type_tr]->CloseIn(TranspIn[id].tr);
    TranspIn[id].use--;
}

unsigned TTransportS::NameInToId( string name )
{
    for(unsigned i_in=0; i_in < TranspIn.size(); i_in++)
	if(TranspIn[i_in].use > 0 && 
		TTransport[TranspIn[i_in].type_tr]->atIn(TranspIn[i_in].tr)->Name() == name )
	    return(i_in);
    throw TError("%s: transport %s no avoid!",o_name,name.c_str());
}

TTransportIn *TTransportS::at_in( unsigned int id )
{
    if(id > TranspIn.size() || TranspIn[id].use <= 0) 
	throw TError("%s: transport identificator error!",o_name);
    return(TTransport[TranspIn[id].type_tr]->atIn(TranspIn[id].tr));
}

void TTransportS::ListIn( vector<string> &list )
{
    list.clear();
    for(unsigned id=0;id < TranspIn.size(); id++)
	if(TranspIn[id].use > 0 )
	    list.push_back( TTransport[TranspIn[id].type_tr]->atIn(TranspIn[id].tr)->Name() );
}

int TTransportS::OpenOut( string t_name, string tt_name, string address )
{
    unsigned id, id_tptrans, id_trans;
    id_tptrans = NameToId(tt_name);
    id_trans   = TTransport[id_tptrans]->OpenOut(t_name,address);
    //Find dublicate
    for(id = 0; id < TranspOut.size(); id++)
	if( TranspOut[id].use > 0 && TranspOut[id].type_tr == id_tptrans && TranspOut[id].tr == id_trans )
	    break;
    if( id < TranspOut.size() ) TranspOut[id].use++;
    else
    {
	for( id = 0; id < TranspOut.size(); id++ )
	    if(TranspOut[id].use <= 0) break;
	if( id == TranspOut.size() ) TranspOut.push_back();
	TranspOut[id].use  = 1;
	TranspOut[id].type_tr = id_tptrans;
	TranspOut[id].tr      = id_trans;
    }
    return(id);    
}

void TTransportS::CloseOut( unsigned int id )
{
    if(id > TranspOut.size() || TranspOut[id].use <= 0) 
	throw TError("%s: transport identificator error!",o_name);
    TTransport[TranspOut[id].type_tr]->CloseOut(TranspOut[id].tr);
    TranspOut[id].use--;
}

unsigned TTransportS::NameOutToId( string name )
{
    for(unsigned i_in=0; i_in < TranspOut.size(); i_in++)
	if(TranspOut[i_in].use > 0 && 
		TTransport[TranspOut[i_in].type_tr]->atOut(TranspOut[i_in].tr)->Name() == name )
	    return(i_in);
    throw TError("%s: transport %s no avoid!",o_name,name.c_str());
}

TTransportOut *TTransportS::at_out( unsigned int id )
{
    if(id > TranspOut.size() || TranspOut[id].use <= 0) 
	throw TError("%s: transport identificator error!",o_name);
    return(TTransport[TranspOut[id].type_tr]->atOut(TranspOut[id].tr));
}

void TTransportS::ListOut( vector<string> &list )
{
    list.clear();
    for(unsigned id=0;id < TranspOut.size(); id++)
	if(TranspOut[id].use > 0 )
	    list.push_back( TTransport[TranspOut[id].type_tr]->atOut(TranspOut[id].tr)->Name() );
}
//================================================================
//=========== TTipTransport ======================================
//================================================================
const char *TTipTransport::o_name = "TTipTransport";

TTipTransport::TTipTransport()
{
    hd_res = SYS->ResCreate();
}
    
TTipTransport::~TTipTransport()
{
    SYS->ResRequest(hd_res);
    for(unsigned id=0; id < i_tr.size(); id++)
	if(i_tr[id].use > 0) { delete i_tr[id].tr; i_tr[id].use = 0; }
    for(unsigned id=0; id < o_tr.size(); id++)
	if(o_tr[id].use > 0) { delete o_tr[id].tr; o_tr[id].use = 0; }
    SYS->ResDelete(hd_res);    
}

TTransportIn *TTipTransport::atIn( unsigned int id )
{
    if(id > i_tr.size() || i_tr[id].use <= 0 ) 
	throw TError("%s: input transport id = %d - error!",o_name,id);
    return(i_tr[id].tr);
}


TTransportOut *TTipTransport::atOut( unsigned int id )
{
    if(id > o_tr.size() || o_tr[id].use <= 0 ) 
	throw TError("%s: output transport id = %d - error!",o_name,id);
    return(o_tr[id].tr);
}

unsigned TTipTransport::OpenIn(string name, string address )
{
    unsigned id;
    
    TTransportIn *tr_in = In(name,address);
    SYS->ResRequest(hd_res);
    for(id=0; id < i_tr.size(); id++) if(i_tr[id].use > 0 && i_tr[id].tr == tr_in ) break;
    if(id < i_tr.size()) i_tr[id].use++;
    else
    {
	for(id=0; id < i_tr.size(); id++) 
	    if(i_tr[id].use <= 0) break;
	if(id == i_tr.size()) i_tr.push_back();
	i_tr[id].use = 1;
	i_tr[id].tr  = tr_in;
    }    
    SYS->ResRelease(hd_res);
}

void TTipTransport::CloseIn( unsigned int id )
{
    SYS->ResRequest(hd_res);
    if(id > i_tr.size() || i_tr[id].use <= 0 )
    {
	SYS->ResRelease(hd_res);
	throw TError("%s: input transport id = %d - error!",o_name,id);
    }
    if((--i_tr[id].use) <= 0 ) delete i_tr[id].tr;
    SYS->ResRelease(hd_res);
}

unsigned TTipTransport::OpenOut(string name, string address )
{
    unsigned id;
    
    TTransportOut *tr_out = Out(name,address);
    SYS->ResRequest(hd_res);
    for(id=0; id < o_tr.size(); id++) if(o_tr[id].use > 0 && o_tr[id].tr == tr_out ) break;
    if(id < o_tr.size()) o_tr[id].use++;
    else
    {
	for(id=0; id < o_tr.size(); id++) 
	    if(o_tr[id].use <= 0) break;
	if(id == o_tr.size()) o_tr.push_back();
	o_tr[id].use = 1;
	o_tr[id].tr  = tr_out;
    }    
    SYS->ResRelease(hd_res);
}

void TTipTransport::CloseOut( unsigned int id )
{
    SYS->ResRequest(hd_res);
    if(id > o_tr.size() || o_tr[id].use <= 0 ) 
    {
	SYS->ResRelease(hd_res);	
	throw TError("%s: output transport id = %d - error!",o_name,id);
    }
    if((--o_tr[id].use) <= 0 ) delete o_tr[id].tr;    
    SYS->ResRelease(hd_res);	
}
//================================================================
//=========== TTransportIn =======================================
//================================================================
const char *TTransportIn::o_name = "TTransportIn";

TTransportIn::~TTransportIn()
{
    
}

//================================================================
//=========== TTransportOut ======================================
//================================================================
const char *TTransportOut::o_name = "TTransportOut";

TTransportOut::~TTransportOut()
{
    
}

