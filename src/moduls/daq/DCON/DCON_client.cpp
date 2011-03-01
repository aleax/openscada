
//OpenSCADA system module DAQ.DCON file: DCON_client.cpp
/***************************************************************************
 *   Copyright (C) 2008 by Roman Savochenko, Almaz Karimov                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#include <getopt.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <tsys.h>
#include <ttiparam.h>

#include "DCON_client.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"DCON"
#define MOD_NAME	_("DCON client")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.5.0"
#define AUTORS		_("Roman Savochenko, Almaz Karimov")
#define DESCRIPTION	_("Allow realisation of DCON client service. Supported I-7000 DCON protocol.")
#define LICENSE		"GPL2"
//*************************************************

DCONDAQ::TTpContr *DCONDAQ::mod;  //Pointer for direct access to module

extern "C"
{
	TModule::SAt module( int n_mod )
	{
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
	}

	TModule *attach( const TModule::SAt &AtMod, const string &source )
	{
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
		return new DCONDAQ::TTpContr( source );
	return NULL;
	}
}

using namespace DCONDAQ;

//******************************************************
//* TTpContr                                           *
//******************************************************
TTpContr::TTpContr( string name ) : TTipDAQ(MOD_ID)
{
	mod		= this;

	mName	= MOD_NAME;
	mType	= MOD_TYPE;
	mVers	= MOD_VER;
	mAutor	= AUTORS;
	mDescr	= DESCRIPTION;
	mLicense	= LICENSE;
	mSource	= name;
}

TTpContr::~TTpContr()
{

}

void TTpContr::postEnable( int flag )
{
	TTipDAQ::postEnable( flag );

	//> Controler's bd structure
	fldAdd( new TFld("PRM_BD",_("Parameters table"),TFld::String,TFld::NoFlag,"30","") );
	fldAdd( new TFld("PERIOD",_("Gather data period (s)"),TFld::Real,TFld::NoFlag,"6.2","1","0.01;100") );
	fldAdd( new TFld("PRIOR",_("Gather task priority"),TFld::Integer,TFld::NoFlag,"2","0","-1;99") );
	fldAdd( new TFld("ADDR",_("Serial transport"),TFld::String,TFld::NoFlag,"30","") );
	fldAdd( new TFld("REQ_TRY",_("Request tries"),TFld::Integer,TFld::NoFlag,"1","3","1;10") );

	//> Parameter type bd structure
	int t_prm = tpParmAdd("std","PRM_BD",_("Standard"));
	tpPrmAt(t_prm).fldAdd( new TFld("MOD_ADDR",_("Module address"),TFld::Integer,TFld::NoFlag|TCfg::NoVal,"20","1","0;255") );
	tpPrmAt(t_prm).fldAdd( new TFld("CRC_CTRL",_("CRC control"),TFld::Boolean,TFld::NoFlag|TCfg::NoVal,"1","1") );
	tpPrmAt(t_prm).fldAdd( new TFld("HOST_SIGNAL",_("Host signal"),TFld::Integer,TFld::Selected|TCfg::NoVal,"1","0","0;1",_("No signal;HostOK")) );
	tpPrmAt(t_prm).fldAdd( new TFld("AI_METHOD",_("AI method"),TFld::Integer,TFld::Selected|TCfg::NoVal,"1","0","0;1;2;3;4;6;8;10;20;116",_("No AI;1AI (#AA);2AI (#AA);3AI (#AA);4AI (#AA);6AI (#AA);8AI (#AA);10AI (#AA);20AI (#AA);16AI (#AA^AA)")) );
	tpPrmAt(t_prm).fldAdd( new TFld("AI_RANGE",""/*_("AI range")*/,TFld::Integer,TFld::Selected|TCfg::NoVal,"1","0","0;1;2",_("Engeneer or percent;Hexadecimal (0000 FFFF);Hexadecimal (8000 7FFF)")) );
	tpPrmAt(t_prm).fldAdd( new TFld("AO_METHOD",_("AO method"),TFld::Integer,TFld::Selected|TCfg::NoVal,"1","0","0;1;2;4",_("No AO;1AO (#AA);2AO (#AA);4AO (#AA)")) );
	tpPrmAt(t_prm).fldAdd( new TFld("AO_RANGE",""/*_("AO range")*/,TFld::Integer,TFld::Selected|TCfg::NoVal,"1","0","0;1;2;3;4;5;6;7;8;9;10",_("Engeneer (00.000 20.000);Engeneer (04.000 20.000);Engeneer (00.000 10.000);Engeneer (+00.000 +20.000);Engeneer (+04.000 +20.000);Engeneer (+00.000 +10.000);Engeneer (-10.000 +10.000);Engeneer (+00.000 +05.000);Engeneer (-05.000 +05.000);Percent (+000.00 +100.00);Hexadecimal (000 FFF)")) );
	tpPrmAt(t_prm).fldAdd( new TFld("DI_METHOD",_("DI method"),TFld::Integer,TFld::Selected|TCfg::NoVal,"1","0","0;3;4;7;8;14;16;101;201",_("No DI;3DI (@AA);4DI (@AA);7DI (@AA);8DI (@AA);14DI (@AA);16DI (@AA);1DI (@AADI);8DI (@AA,FF00)")) );
	tpPrmAt(t_prm).fldAdd( new TFld("DO_METHOD",_("DO method"),TFld::Integer,TFld::Selected|TCfg::NoVal,"1","0","0;2;3;4;5;7;8;12;13;16;102;103;202;204;306;402;504;604;608)",_("No DO;2DO (@AA,0300);3DO (@AA,7);4DO (@AA,F);5DO (@AA,1F);7DO (@AA,7F);8DO (@AA,FF);12DO (@AA,0FFF);13DO (@AA,1FFF);16DO (@AA,FFFF);2DO (^AADOVVV);3DO (^AADOVVV);2DO (@AADO);4DO (@AADO);6DO (@AADODD);2DO (@AADO0D);4DO (@(^)AADO0D);4DO (@AA,0F00);8DO (@AA,FF00)")) );
	tpPrmAt(t_prm).fldAdd( new TFld("CI_METHOD",_("CI method"),TFld::Integer,TFld::Selected|TCfg::NoVal,"1","0","0;2;3",_("No CI;2CI (#AA);3CI (#AA)")) );

}

void TTpContr::load_( )
{
	//> Load parameters from command line

}

void TTpContr::save_()
{

}

TController *TTpContr::ContrAttach( const string &name, const string &daq_db )
{
	return new TMdContr(name,daq_db,this);
}


//******************************************************
//* TMdContr                                           *
//******************************************************
TMdContr::TMdContr( string name_c, const string &daq_db, TElem *cfgelem ) :
	TController( name_c, daq_db, cfgelem ),	m_per(cfg("PERIOD").getRd()), m_prior(cfg("PRIOR").getId()),
	connTry(cfg("REQ_TRY").getId()), m_addr(cfg("ADDR").getSd()), prc_st(false), endrun_req(false), tm_gath(0)
{
	cfg("PRM_BD").setS("DCONPrm_"+name_c);
}

TMdContr::~TMdContr()
{
	if( run_st ) stop();
}

string TMdContr::getStatus( )
{
	string val = TController::getStatus( );

	if( startStat( ) && !redntUse( ) ) val += TSYS::strMess(_("Spent time: %s. "),TSYS::time2str(tm_gath).c_str());

	return val;
}


TParamContr *TMdContr::ParamAttach( const string &name, int type )
{
	return new TMdPrm( name, &owner().tpPrmAt(type) );
}

void TMdContr::disable_( )
{

}

void TMdContr::start_( )
{
	if( !prc_st )
	{
		SYS->transport().at().at("Serial").at().outAt(m_addr).at().start();

	//> Start the gathering data task
	SYS->taskCreate( nodePath('.',true), m_prior, TMdContr::Task, this, &prc_st );
	}
}

void TMdContr::stop_( )
{
	//> Stop the request and calc data task
	if( prc_st ) SYS->taskDestroy( nodePath('.',true), &prc_st, &endrun_req );
}

bool TMdContr::cfgChange( TCfg &icfg )
{
	TController::cfgChange(icfg);
	return true;
}

void TMdContr::prmEn( const string &id, bool val )
{
	ResAlloc res( en_res, true );

	unsigned i_prm;
	for(i_prm = 0; i_prm < p_hd.size(); i_prm++)
	    if(p_hd[i_prm].at().id() == id) break;

	if(val && i_prm >= p_hd.size())	p_hd.push_back(at(id));
	if(!val && i_prm < p_hd.size())	p_hd.erase(p_hd.begin()+i_prm);
}

string TMdContr::DCONCRC( string str )
{
	const string HexSymbol[16] = {"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};
	unsigned char CRC=0;
	for(unsigned i = 0; i < str.size(); i++) CRC += *str.substr(i,1).c_str();
	int iCRC=CRC;
	return HexSymbol[15 & (iCRC>>4)] + HexSymbol[15 & iCRC];
}

string TMdContr::DCONReq( string &pdu )
{
	ResAlloc res( req_res, true );
	char buf[1000];
	string rez, err;

	try
	{
	AutoHD<TTransportOut> tr = SYS->transport().at().at("Serial").at().outAt(m_addr);
	if( !tr.at().startStat() ) tr.at().start();
	pdu += "\r";

	ResAlloc resN( tr.at().nodeRes(), true );

	for( int i_tr = 0; i_tr < vmax(1,vmin(10,connTry)); i_tr++ )
	{
		int resp_len = tr.at().messIO( pdu.data(), pdu.size(), buf, sizeof(buf), 0, true );
		rez.assign(buf,resp_len);

		//> Wait tail
		while( rez.size() < 2 || rez[rez.size()-1] != '\r' )
		{
		try{ resp_len = tr.at().messIO( NULL, 0, buf, sizeof(buf), 0, true ); } catch(TError er){ break; }
		rez.append( buf, resp_len );
		}
		if( rez.size() < 2 || rez[rez.size()-1] != '\r' )	{ err = _("13:Error respond: Not full."); continue; }
		pdu = rez.substr(0,rez.size()-1);
		err = "";
		break;
	}
	}
	catch(TError er) { err = _("10:Transport error: ")+er.mess; }

	if( !err.empty() ) pdu = "";

	return err;
}

void *TMdContr::Task( void *icntr )
{
	const string HexSymbol[16] = {"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};
	string pdu;
	int acq_len;
	int n,m,i;
	long long work_tm, last_tm = 0;
	struct timespec get_tm;
	TMdContr &cntr = *(TMdContr *)icntr;

	cntr.endrun_req = false;
	cntr.prc_st = true;

	try
	{
	while( !cntr.endrun_req )
	{
		if( !cntr.redntUse( ) )
		{
		long long t_cnt = TSYS::curTime();

		//> Update controller's data
		ResAlloc res( cntr.en_res, false );
		for(unsigned i_p = 0; i_p < cntr.p_hd.size(); i_p++)
		{
			if( cntr.endrun_req ) break;
			//> Reset errors
			cntr.p_hd[i_p].at().ai_err=false;
			cntr.p_hd[i_p].at().ao_err=false;
			cntr.p_hd[i_p].at().di_err=false;
			cntr.p_hd[i_p].at().do_err=false;
			cntr.p_hd[i_p].at().ci_err=false;
			cntr.p_hd[i_p].at().ai_txterr=_("No errors");
			cntr.p_hd[i_p].at().ao_txterr=_("No errors");
			cntr.p_hd[i_p].at().di_txterr=_("No errors");
			cntr.p_hd[i_p].at().do_txterr=_("No errors");
			cntr.p_hd[i_p].at().ci_txterr=_("No errors");

			//AI
			if ((cntr.p_hd[i_p].at().ai_method==1)||(cntr.p_hd[i_p].at().ai_method==2)||(cntr.p_hd[i_p].at().ai_method==3)||(cntr.p_hd[i_p].at().ai_method==4)||(cntr.p_hd[i_p].at().ai_method==6)||(cntr.p_hd[i_p].at().ai_method==8)||(cntr.p_hd[i_p].at().ai_method==10)||(cntr.p_hd[i_p].at().ai_method==20))
				{
					n=cntr.p_hd[i_p].at().ai_method;
					//> Request with module
					pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().ai_range==0)  m=7; else m=4;
					acq_len=m*n+1;
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=acq_len+2;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";

					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().ai_err=true;
							cntr.p_hd[i_p].at().ai_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().ai_err=true;
									cntr.p_hd[i_p].at().ai_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl)
										if ((pdu.substr(m*n+1,2))!=(cntr.DCONCRC(pdu.substr(0,m*n+1))))
											{
												cntr.p_hd[i_p].at().ai_err=true;
												cntr.p_hd[i_p].at().ai_txterr=_("Invalid module CRC");
											}
									if (!cntr.p_hd[i_p].at().ai_err)
										if (pdu.substr(0,1)!=">")
											{
												cntr.p_hd[i_p].at().ai_err=true;
												cntr.p_hd[i_p].at().ai_txterr=_("Invalid module response");
											}
								}
						}
					//Set DAQ attributes
					if (!cntr.p_hd[i_p].at().ai_err)
						for (i=0; i<n; i++)
							{
								if (m==7) cntr.p_hd[i_p].at().AI[i]=atof(pdu.substr(i*m+1,m).c_str());
								if (m==4) cntr.p_hd[i_p].at().AI[i]=strtol(pdu.substr(i*m+1,m).c_str(),NULL,16);
								if ((cntr.p_hd[i_p].at().ai_range==2)&&(cntr.p_hd[i_p].at().AI[i]>0x7fff))
									cntr.p_hd[i_p].at().AI[i]=cntr.p_hd[i_p].at().AI[i]-0x10000;
							}
				}
			if (cntr.p_hd[i_p].at().ai_method==116)
				{
					n=8;
					//#AA
					//> Request with module
					pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().ai_range==0)  m=7; else m=4;
					acq_len=m*n+1;
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=acq_len+2;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";

					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().ai_err=true;
							cntr.p_hd[i_p].at().ai_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().ai_err=true;
									cntr.p_hd[i_p].at().ai_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl)
										if ((pdu.substr(m*n+1,2))!=(cntr.DCONCRC(pdu.substr(0,m*n+1))))
											{
												cntr.p_hd[i_p].at().ai_err=true;
												cntr.p_hd[i_p].at().ai_txterr=_("Invalid module CRC");
											}
									if (!cntr.p_hd[i_p].at().ai_err)
										if (pdu.substr(0,1)!=">")
											{
												cntr.p_hd[i_p].at().ai_err=true;
												cntr.p_hd[i_p].at().ai_txterr=_("Invalid module response");
											}
								}
						}
					//Set DAQ attributes
					if (!cntr.p_hd[i_p].at().ai_err)
						for (i=0; i<n; i++)
							{
								if (m==7) cntr.p_hd[i_p].at().AI[i]=atof(pdu.substr(i*m+1,m).c_str());
								if (m==4) cntr.p_hd[i_p].at().AI[i]=strtol(pdu.substr(i*m+1,m).c_str(),NULL,16);
								if ((cntr.p_hd[i_p].at().ai_range==2)&&(cntr.p_hd[i_p].at().AI[i]>0x7fff))
									cntr.p_hd[i_p].at().AI[i]=cntr.p_hd[i_p].at().AI[i]-0x10000;
							}
					//^AA
					//> Request with module
					pdu = "^" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().ai_range==0) m=7; else m=4;
					acq_len=m*n+1;
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=acq_len+2;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";

					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().ai_err=true;
							cntr.p_hd[i_p].at().ai_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().ai_err=true;
									cntr.p_hd[i_p].at().ai_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl)
										if ((pdu.substr(m*n+1,2))!=(cntr.DCONCRC(pdu.substr(0,m*n+1))))
											{
												cntr.p_hd[i_p].at().ai_err=true;
												cntr.p_hd[i_p].at().ai_txterr=_("Invalid module CRC");
											}
									if (!cntr.p_hd[i_p].at().ai_err)
										if (pdu.substr(0,1)!=">")
											{
												cntr.p_hd[i_p].at().ai_err=true;
												cntr.p_hd[i_p].at().ai_txterr=_("Invalid module response");
											}
								}
						}
					//Set DAQ attributes
					if (!cntr.p_hd[i_p].at().ai_err)
						for (i=0; i<n; i++)
							{
								if (m==7) cntr.p_hd[i_p].at().AI[i+8]=atof(pdu.substr(i*m+1,m).c_str());
								if (m==4) cntr.p_hd[i_p].at().AI[i+8]=strtol(pdu.substr(i*m+1,m).c_str(),NULL,16);
								if ((cntr.p_hd[i_p].at().ai_range==2)&&(cntr.p_hd[i_p].at().AI[i+8]>0x7fff))
									cntr.p_hd[i_p].at().AI[i+8]=cntr.p_hd[i_p].at().AI[i+8]-0x10000;
							}
				}

			//AO
			int dec, sign, ndig;
			string str;
			if ((cntr.p_hd[i_p].at().ao_method==1)||(cntr.p_hd[i_p].at().ao_method==2)||(cntr.p_hd[i_p].at().ao_method==4))
				for (i=0; i<cntr.p_hd[i_p].at().ao_method; i++)
					{
						//Control range
						switch (cntr.p_hd[i_p].at().ao_range)
						{
							case 0://Engeneer (00.000 20.000)
							{
								if (cntr.p_hd[i_p].at().AO[i]<0) cntr.p_hd[i_p].at().AO[i]=0;
								if (cntr.p_hd[i_p].at().AO[i]>20) cntr.p_hd[i_p].at().AO[i]=20;
								ndig = 3;
								str=fcvt(cntr.p_hd[i_p].at().AO[i],ndig,&dec,&sign);
								if (dec==1) str="0"+str;
								str=str.substr(0,2)+"."+str.substr(2,3);
								break;
							}
							case 1://Engeneer (04.000 20.000)
							{
								if (cntr.p_hd[i_p].at().AO[i]<4) cntr.p_hd[i_p].at().AO[i]=4;
								if (cntr.p_hd[i_p].at().AO[i]>20) cntr.p_hd[i_p].at().AO[i]=20;
								ndig = 3;
								str=fcvt(cntr.p_hd[i_p].at().AO[i],ndig,&dec,&sign);
								if (dec==1) str="0"+str;
								str=str.substr(0,2)+"."+str.substr(2,3);
								break;
							}
							case 2://Engeneer (00.000 10.000)
							{
								if (cntr.p_hd[i_p].at().AO[i]<0) cntr.p_hd[i_p].at().AO[i]=0;
								if (cntr.p_hd[i_p].at().AO[i]>10) cntr.p_hd[i_p].at().AO[i]=10;
								ndig = 3;
								str=fcvt(cntr.p_hd[i_p].at().AO[i],ndig,&dec,&sign);
								if (dec==1) str="0"+str;
								str=str.substr(0,2)+"."+str.substr(2,3);
								break;
							}
							case 3://Engeneer (+00.000 +20.000)
							{
								if (cntr.p_hd[i_p].at().AO[i]<0) cntr.p_hd[i_p].at().AO[i]=0;
								if (cntr.p_hd[i_p].at().AO[i]>20) cntr.p_hd[i_p].at().AO[i]=20;
								ndig = 3;
								str=fcvt(cntr.p_hd[i_p].at().AO[i],ndig,&dec,&sign);
								if (dec==1) str="0"+str;
								str="+"+str.substr(0,2)+"."+str.substr(2,3);
								break;
							}
							case 4://Engeneer (+04.000 +20.000)
							{
								if (cntr.p_hd[i_p].at().AO[i]<4) cntr.p_hd[i_p].at().AO[i]=4;
								if (cntr.p_hd[i_p].at().AO[i]>20) cntr.p_hd[i_p].at().AO[i]=20;
								ndig = 3;
								str=fcvt(cntr.p_hd[i_p].at().AO[i],ndig,&dec,&sign);
								if (dec==1) str="0"+str;
								str="+"+str.substr(0,2)+"."+str.substr(2,3);
								break;
							}
							case 5://Engeneer (+00.000 +10.000)
							{
								if (cntr.p_hd[i_p].at().AO[i]<0) cntr.p_hd[i_p].at().AO[i]=0;
								if (cntr.p_hd[i_p].at().AO[i]>10) cntr.p_hd[i_p].at().AO[i]=10;
								ndig = 3;
								str=fcvt(cntr.p_hd[i_p].at().AO[i],ndig,&dec,&sign);
								if (dec==1) str="0"+str;
								str="+"+str.substr(0,2)+"."+str.substr(2,3);
								break;
							}
							case 6://Engeneer (-10.000 +10.000)
							{
								if (cntr.p_hd[i_p].at().AO[i]<-10) cntr.p_hd[i_p].at().AO[i]=-10;
								if (cntr.p_hd[i_p].at().AO[i]>10) cntr.p_hd[i_p].at().AO[i]=10;
								ndig = 3;
								str=fcvt(cntr.p_hd[i_p].at().AO[i],ndig,&dec,&sign);
								if (dec==1) str="0"+str;
								if (cntr.p_hd[i_p].at().AO[i]<0) str="-"+str.substr(0,2)+"."+str.substr(2,3);
									else str="+"+str.substr(0,2)+"."+str.substr(2,3);
								break;
							}
							case 7://Engeneer (+00.000 +05.000)
							{
								if (cntr.p_hd[i_p].at().AO[i]<0) cntr.p_hd[i_p].at().AO[i]=0;
								if (cntr.p_hd[i_p].at().AO[i]>5) cntr.p_hd[i_p].at().AO[i]=5;
								ndig = 3;
								str=fcvt(cntr.p_hd[i_p].at().AO[i],ndig,&dec,&sign);
								if (dec==1) str="0"+str;
								str="+"+str.substr(0,2)+"."+str.substr(2,3);
								break;
							}
							case 8://Engeneer (-05.000 +05.000)
							{
								if (cntr.p_hd[i_p].at().AO[i]<-5) cntr.p_hd[i_p].at().AO[i]=-5;
								if (cntr.p_hd[i_p].at().AO[i]>5) cntr.p_hd[i_p].at().AO[i]=5;
								ndig = 3;
								str=fcvt(cntr.p_hd[i_p].at().AO[i],ndig,&dec,&sign);
								if (dec==1) str="0"+str;
								if (cntr.p_hd[i_p].at().AO[i]<0) str="-"+str.substr(0,2)+"."+str.substr(2,3);
									else str="+"+str.substr(0,2)+"."+str.substr(2,3);
								break;
							}
							case 9://Percent (+000.00 +100.00)
							{
								if (cntr.p_hd[i_p].at().AO[i]<0) cntr.p_hd[i_p].at().AO[i]=0;
								if (cntr.p_hd[i_p].at().AO[i]>100) cntr.p_hd[i_p].at().AO[i]=100;
								ndig = 2;
								str=fcvt(cntr.p_hd[i_p].at().AO[i],ndig,&dec,&sign);
								if (dec==1) str="00"+str;
								if (dec==2) str="0"+str;
								str="+"+str.substr(0,3)+"."+str.substr(3,2);
								break;
							}
							case 10://Hexadecimal (000 FFF)
							{
								if (cntr.p_hd[i_p].at().AO[i]<0) cntr.p_hd[i_p].at().AO[i]=0;
								if (cntr.p_hd[i_p].at().AO[i]>0xFFF) cntr.p_hd[i_p].at().AO[i]=0xFFF;
								cntr.p_hd[i_p].at().AO[i]=int(cntr.p_hd[i_p].at().AO[i]);
								str=HexSymbol[15 & (int(cntr.p_hd[i_p].at().AO[i])>>8)] + HexSymbol[15 & (int(cntr.p_hd[i_p].at().AO[i])>>4)] + HexSymbol[15 & (int(cntr.p_hd[i_p].at().AO[i]))];
								break;
							}
							default:
							{
								cntr.p_hd[i_p].at().ao_err=true;
								cntr.p_hd[i_p].at().ao_txterr=_("Undefined range");
								break;
							}
						}
						//> Request with module
						pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr]+HexSymbol[i]+str;
						if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
						if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//						cout << pdu.substr(0,pdu.size()) << " writing\n";
						cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//						cout << pdu.substr(0,pdu.size()-1) << " reading\n";

						//Check errors
						if (pdu.size()==0)
							{
								cntr.p_hd[i_p].at().ao_err=true;
								cntr.p_hd[i_p].at().ao_txterr=cntr.p_hd[i_p].at().acq_err;
							}
						else
							{
								if (pdu.size()==(acq_len+2))
									if (pdu.substr(0,1)=="?")
										{
											cntr.p_hd[i_p].at().ao_err=true;
											cntr.p_hd[i_p].at().ao_txterr=_("Module out of range");
										}
								if (!cntr.p_hd[i_p].at().ao_err)
									{
										if ((int)pdu.size()!=acq_len)
											{
												cntr.p_hd[i_p].at().ao_err=true;
												cntr.p_hd[i_p].at().ao_txterr=_("Module packet length error");
											}
										else
											{
												if (cntr.p_hd[i_p].at().crc_ctrl)
													if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
														{
															cntr.p_hd[i_p].at().ao_err=true;
															cntr.p_hd[i_p].at().ao_txterr=_("Invalid module CRC");
														}
												if (!cntr.p_hd[i_p].at().ao_err)
													if (pdu.substr(0,1)=="!")
														{
															cntr.p_hd[i_p].at().ao_err=true;
															cntr.p_hd[i_p].at().ao_txterr=_("Command ignored (host watchdog)");
														}
												if (!cntr.p_hd[i_p].at().ao_err)
													if (pdu.substr(0,1)!=">")
														{
															cntr.p_hd[i_p].at().ao_err=true;
															cntr.p_hd[i_p].at().ao_txterr=_("Invalid module response");
														}
											}
									}
							}
					}

			//DI
			unsigned int DI;
			if ((cntr.p_hd[i_p].at().di_method==3)||(cntr.p_hd[i_p].at().di_method==4)||(cntr.p_hd[i_p].at().di_method==7)||(cntr.p_hd[i_p].at().di_method==8)||(cntr.p_hd[i_p].at().di_method==14)||(cntr.p_hd[i_p].at().di_method==16)||(cntr.p_hd[i_p].at().di_method==101)||(cntr.p_hd[i_p].at().di_method==201))
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					acq_len=5;
					if (cntr.p_hd[i_p].at().di_method==101) {pdu+="DI"; acq_len=7;}
					if (cntr.p_hd[i_p].at().crc_ctrl) {pdu+=cntr.DCONCRC(pdu); acq_len=acq_len+2;}
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";

					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().di_err=true;
							cntr.p_hd[i_p].at().di_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().di_err=true;
									cntr.p_hd[i_p].at().di_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl)
										if ((pdu.substr(acq_len-2,2))!=(cntr.DCONCRC(pdu.substr(0,acq_len-2))))
											{
												cntr.p_hd[i_p].at().di_err=true;
												cntr.p_hd[i_p].at().di_txterr=_("Invalid module CRC");
											}
									if (cntr.p_hd[i_p].at().di_method==101)
										{
											if (!cntr.p_hd[i_p].at().di_err)
												if (pdu.substr(0,1)!="!")
													{
														cntr.p_hd[i_p].at().di_err=true;
														cntr.p_hd[i_p].at().di_txterr=_("Invalid module response");
													}
										}
									else
										{
											if (!cntr.p_hd[i_p].at().di_err)
												if (pdu.substr(0,1)!=">")
													{
														cntr.p_hd[i_p].at().di_err=true;
														cntr.p_hd[i_p].at().di_txterr=_("Invalid module response");
													}
										}
								}
						}
					//Set DAQ attributes
					if (!cntr.p_hd[i_p].at().di_err) switch (cntr.p_hd[i_p].at().di_method)
					{
						case 0:	break;//No DI
						case 3://3DI (@AA)
						{
							DI = strtoul(pdu.substr(1,4).c_str(),NULL,16);
//							cout << DI << "\n";
							cntr.p_hd[i_p].at().DI[0]=0x00001 & DI;
							cntr.p_hd[i_p].at().DI[1]=0x00002 & DI;
							cntr.p_hd[i_p].at().DI[2]=0x00004 & DI;
							break;
						}
						case 4://4DI (@AA)
						{
							DI = strtoul(pdu.substr(1,4).c_str(),NULL,16);
//							cout << DI << "\n";
							cntr.p_hd[i_p].at().DI[0]=0x00001 & DI;
							cntr.p_hd[i_p].at().DI[1]=0x00002 & DI;
							cntr.p_hd[i_p].at().DI[2]=0x00004 & DI;
							cntr.p_hd[i_p].at().DI[3]=0x00008 & DI;
							break;
						}
						case 7://7DI (@AA)
						{
							DI = strtoul(pdu.substr(1,4).c_str(),NULL,16);
//							cout << DI << "\n";
							cntr.p_hd[i_p].at().DI[0]=0x00001 & DI;
							cntr.p_hd[i_p].at().DI[1]=0x00002 & DI;
							cntr.p_hd[i_p].at().DI[2]=0x00004 & DI;
							cntr.p_hd[i_p].at().DI[3]=0x00008 & DI;
							cntr.p_hd[i_p].at().DI[4]=0x00010 & DI;
							cntr.p_hd[i_p].at().DI[5]=0x00020 & DI;
							cntr.p_hd[i_p].at().DI[6]=0x00040 & DI;
							break;
						}
						case 8://8DI (@AA)
						{
							DI = strtoul(pdu.substr(1,4).c_str(),NULL,16);
//							cout << DI << "\n";
							cntr.p_hd[i_p].at().DI[0]=0x00001 & DI;
							cntr.p_hd[i_p].at().DI[1]=0x00002 & DI;
							cntr.p_hd[i_p].at().DI[2]=0x00004 & DI;
							cntr.p_hd[i_p].at().DI[3]=0x00008 & DI;
							cntr.p_hd[i_p].at().DI[4]=0x00010 & DI;
							cntr.p_hd[i_p].at().DI[5]=0x00020 & DI;
							cntr.p_hd[i_p].at().DI[6]=0x00040 & DI;
							cntr.p_hd[i_p].at().DI[7]=0x00080 & DI;
							break;
						}
						case 14://14DI (@AA)
						{
							DI = strtoul(pdu.substr(1,4).c_str(),NULL,16);
//							cout << DI << "\n";
							cntr.p_hd[i_p].at().DI[0]=0x00001 & DI;
							cntr.p_hd[i_p].at().DI[1]=0x00002 & DI;
							cntr.p_hd[i_p].at().DI[2]=0x00004 & DI;
							cntr.p_hd[i_p].at().DI[3]=0x00008 & DI;
							cntr.p_hd[i_p].at().DI[4]=0x00010 & DI;
							cntr.p_hd[i_p].at().DI[5]=0x00020 & DI;
							cntr.p_hd[i_p].at().DI[6]=0x00040 & DI;
							cntr.p_hd[i_p].at().DI[7]=0x00080 & DI;
							cntr.p_hd[i_p].at().DI[8]=0x00100 & DI;
							cntr.p_hd[i_p].at().DI[9]=0x00200 & DI;
							cntr.p_hd[i_p].at().DI[10]=0x00400 & DI;
							cntr.p_hd[i_p].at().DI[11]=0x00800 & DI;
							cntr.p_hd[i_p].at().DI[12]=0x01000 & DI;
							cntr.p_hd[i_p].at().DI[13]=0x02000 & DI;
							break;
						break;
						}
						case 16://16DI (@AA)
						{
							DI = strtoul(pdu.substr(1,4).c_str(),NULL,16);
//							cout << DI << "\n";
							cntr.p_hd[i_p].at().DI[0]=0x00001 & DI;
							cntr.p_hd[i_p].at().DI[1]=0x00002 & DI;
							cntr.p_hd[i_p].at().DI[2]=0x00004 & DI;
							cntr.p_hd[i_p].at().DI[3]=0x00008 & DI;
							cntr.p_hd[i_p].at().DI[4]=0x00010 & DI;
							cntr.p_hd[i_p].at().DI[5]=0x00020 & DI;
							cntr.p_hd[i_p].at().DI[6]=0x00040 & DI;
							cntr.p_hd[i_p].at().DI[7]=0x00080 & DI;
							cntr.p_hd[i_p].at().DI[8]=0x00100 & DI;
							cntr.p_hd[i_p].at().DI[9]=0x00200 & DI;
							cntr.p_hd[i_p].at().DI[10]=0x00400 & DI;
							cntr.p_hd[i_p].at().DI[11]=0x00800 & DI;
							cntr.p_hd[i_p].at().DI[12]=0x01000 & DI;
							cntr.p_hd[i_p].at().DI[13]=0x02000 & DI;
							cntr.p_hd[i_p].at().DI[14]=0x04000 & DI;
							cntr.p_hd[i_p].at().DI[15]=0x08000 & DI;
							break;
						}
						case 101://1DI (@AADI)
						{
							if (pdu.substr(6,2)=="00") cntr.p_hd[i_p].at().DI[0]=false;
							if (pdu.substr(6,2)=="01") cntr.p_hd[i_p].at().DI[0]=true;
							break;
						}
						case 201://8DI (@AA,FF00)
						{
							DI = strtoul(pdu.substr(1,4).c_str(),NULL,16);
//							cout << DI << "\n";
							cntr.p_hd[i_p].at().DI[0]=0x00100 & DI;
							cntr.p_hd[i_p].at().DI[1]=0x00200 & DI;
							cntr.p_hd[i_p].at().DI[2]=0x00400 & DI;
							cntr.p_hd[i_p].at().DI[3]=0x00800 & DI;
							cntr.p_hd[i_p].at().DI[4]=0x01000 & DI;
							cntr.p_hd[i_p].at().DI[5]=0x02000 & DI;
							cntr.p_hd[i_p].at().DI[6]=0x04000 & DI;
							cntr.p_hd[i_p].at().DI[7]=0x08000 & DI;
							break;
						}
						default:	break;
					}
				}

			//DO
			switch (cntr.p_hd[i_p].at().do_method)
			{
				case 0:	break;//No DO
				case 2://2DO (@AA,0300)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+="0" + HexSymbol[2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])] + "00";
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 3://3DO (@AA,7)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+=HexSymbol[4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 4://4DO (@AA,F)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[3])+4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
				break;
				}
				case 5://5DO (@AA,1F)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+=HexSymbol[(1&cntr.p_hd[i_p].at().DO[4])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[3])+4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 7://7DO (@AA,7F)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+=HexSymbol[4*(1&cntr.p_hd[i_p].at().DO[6])+2*(1&cntr.p_hd[i_p].at().DO[5])+(1&cntr.p_hd[i_p].at().DO[4])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[3])+4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 8://8DO (@AA,FF)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[7])+4*(1&cntr.p_hd[i_p].at().DO[6])+2*(1&cntr.p_hd[i_p].at().DO[5])+(1&cntr.p_hd[i_p].at().DO[4])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[3])+4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 12://12DO (@AA,0FFF)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+= "0" + HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[11])+4*(1&cntr.p_hd[i_p].at().DO[10])+2*(1&cntr.p_hd[i_p].at().DO[9])+(1&cntr.p_hd[i_p].at().DO[8])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[7])+4*(1&cntr.p_hd[i_p].at().DO[6])+2*(1&cntr.p_hd[i_p].at().DO[5])+(1&cntr.p_hd[i_p].at().DO[4])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[3])+4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 13://13DO (@AA,1FFF)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+=HexSymbol[(1&cntr.p_hd[i_p].at().DO[12])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[11])+4*(1&cntr.p_hd[i_p].at().DO[10])+2*(1&cntr.p_hd[i_p].at().DO[9])+(1&cntr.p_hd[i_p].at().DO[8])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[7])+4*(1&cntr.p_hd[i_p].at().DO[6])+2*(1&cntr.p_hd[i_p].at().DO[5])+(1&cntr.p_hd[i_p].at().DO[4])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[3])+4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 16://16DO (@AA,FFFF)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[15])+4*(1&cntr.p_hd[i_p].at().DO[14])+2*(1&cntr.p_hd[i_p].at().DO[13])+(1&cntr.p_hd[i_p].at().DO[12])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[11])+4*(1&cntr.p_hd[i_p].at().DO[10])+2*(1&cntr.p_hd[i_p].at().DO[9])+(1&cntr.p_hd[i_p].at().DO[8])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[7])+4*(1&cntr.p_hd[i_p].at().DO[6])+2*(1&cntr.p_hd[i_p].at().DO[5])+(1&cntr.p_hd[i_p].at().DO[4])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[3])+4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 102://2DO (^AADOVVV)
				{
					//> Request with module
					pdu = "^" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+="DO";
					if (cntr.p_hd[i_p].at().DO[0]) pdu+="1"; else pdu+="0";
					if (cntr.p_hd[i_p].at().DO[1]) pdu+="1"; else pdu+="0";
					pdu+="0";
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size() != acq_len && pdu.size()!= (acq_len+2))
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if ((cntr.p_hd[i_p].at().crc_ctrl)&&(pdu.size()==(acq_len+2))) if ((pdu.substr(3,2))!=(cntr.DCONCRC(pdu.substr(0,3))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((cntr.p_hd[i_p].at().crc_ctrl)&&((int)pdu.size()==acq_len)) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 103://3DO (^AADOVVV)
				{
					//> Request with module
					pdu = "^" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+="DO";
					if (cntr.p_hd[i_p].at().DO[0]) pdu+="1"; else pdu+="0";
					if (cntr.p_hd[i_p].at().DO[1]) pdu+="1"; else pdu+="0";
					if (cntr.p_hd[i_p].at().DO[1]) pdu+="1"; else pdu+="0";
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if (((int)pdu.size()!=acq_len)&&(pdu.size()!=(acq_len+2)))
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if ((cntr.p_hd[i_p].at().crc_ctrl)&&(pdu.size()==(acq_len+2))) if ((pdu.substr(3,2))!=(cntr.DCONCRC(pdu.substr(0,3))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((cntr.p_hd[i_p].at().crc_ctrl)&&((int)pdu.size()==acq_len)) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 202://2DO (@AADO)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+="DO0";
					pdu+=HexSymbol[2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=5; else acq_len=3;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(3,2))!=(cntr.DCONCRC(pdu.substr(0,3))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 204://4DO (@AADO)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+="DO0";
					pdu+=HexSymbol[2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=5; else acq_len=3;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(3,2))!=(cntr.DCONCRC(pdu.substr(0,3))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+="DO1";
					pdu+=HexSymbol[2*(1&cntr.p_hd[i_p].at().DO[3])+(1&cntr.p_hd[i_p].at().DO[2])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=5; else acq_len=3;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(3,2))!=(cntr.DCONCRC(pdu.substr(0,3))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 306://6DO (@AADODD)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+="DO";
					pdu+=HexSymbol[2*(1&cntr.p_hd[i_p].at().DO[5])+(1&cntr.p_hd[i_p].at().DO[4])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[3])+4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=5; else acq_len=3;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(3,2))!=(cntr.DCONCRC(pdu.substr(0,3))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 402://2DO (@AADO0D)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+="DO0";
					pdu+=HexSymbol[2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=5; else acq_len=3;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(3,2))!=(cntr.DCONCRC(pdu.substr(0,3))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 504://4DO (@(^)AADO0D)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+="DO0";
					pdu+=HexSymbol[2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=5; else acq_len=3;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(3,2))!=(cntr.DCONCRC(pdu.substr(0,3))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					//> Request with module
					pdu = "^" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+="DO0";
					pdu+=HexSymbol[2*(1&cntr.p_hd[i_p].at().DO[3])+(1&cntr.p_hd[i_p].at().DO[2])];
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=5; else acq_len=3;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(3,2))!=(cntr.DCONCRC(pdu.substr(0,3))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 604://4DO (@AA,0F00)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+= "0" + HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[3])+4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])] + "00";
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				case 608://8DO (@AA,FF00)
				{
					//> Request with module
					pdu = "@" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[7])+4*(1&cntr.p_hd[i_p].at().DO[6])+2*(1&cntr.p_hd[i_p].at().DO[5])+(1&cntr.p_hd[i_p].at().DO[4])];
					pdu+=HexSymbol[8*(1&cntr.p_hd[i_p].at().DO[3])+4*(1&cntr.p_hd[i_p].at().DO[2])+2*(1&cntr.p_hd[i_p].at().DO[1])+(1&cntr.p_hd[i_p].at().DO[0])] + "00";
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=3; else acq_len=1;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().do_err=true;
							cntr.p_hd[i_p].at().do_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().do_err=true;
									cntr.p_hd[i_p].at().do_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(1,2))!=(cntr.DCONCRC(pdu.substr(0,1))))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)=="!")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Command ignored (host watchdog)");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().do_err))
										{
											cntr.p_hd[i_p].at().do_err=true;
											cntr.p_hd[i_p].at().do_txterr=_("Invalid module response");
										}
								}
						}
					break;
				}
				default:	break;
			}

			//CI
			switch (cntr.p_hd[i_p].at().ci_method)
			{
				case 0:	break;//No CI
				case 2://2CI (#AA)
				{
					//> Request with module
					pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr] + "0";
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=11; else acq_len=9;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().ci_err=true;
							cntr.p_hd[i_p].at().ci_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().ci_err=true;
									cntr.p_hd[i_p].at().ci_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) pdu = pdu.substr(0,1) + pdu.substr(3,8) + pdu.substr(1,2);
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(9,2))!=(cntr.DCONCRC(pdu.substr(0,9))))
										{
											cntr.p_hd[i_p].at().ci_err=true;
											cntr.p_hd[i_p].at().ci_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().ci_err))
										{
											cntr.p_hd[i_p].at().ci_err=true;
											cntr.p_hd[i_p].at().ci_txterr=_("Invalid module response");
										}
								}
						}
					//Set DAQ attributes
					if (!cntr.p_hd[i_p].at().ci_err) cntr.p_hd[i_p].at().CI[0]=strtol(pdu.substr(1,8).c_str(),NULL,16);

					//> Request with module
					pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr] + "1";
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=11; else acq_len=9;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().ci_err=true;
							cntr.p_hd[i_p].at().ci_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().ci_err=true;
									cntr.p_hd[i_p].at().ci_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) pdu = pdu.substr(0,1) + pdu.substr(3,8) + pdu.substr(1,2);
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(9,2))!=(cntr.DCONCRC(pdu.substr(0,9))))
										{
											cntr.p_hd[i_p].at().ci_err=true;
											cntr.p_hd[i_p].at().ci_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().ci_err))
										{
											cntr.p_hd[i_p].at().ci_err=true;
											cntr.p_hd[i_p].at().ci_txterr=_("Invalid module response");
										}
								}
						}
					//Set DAQ attributes
					if (!cntr.p_hd[i_p].at().ci_err) cntr.p_hd[i_p].at().CI[1]=strtol(pdu.substr(1,8).c_str(),NULL,16);
					break;
				}
				case 3://3CI (#AA)
				{
					//> Request with module
					pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr] + "0";
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=11; else acq_len=9;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().ci_err=true;
							cntr.p_hd[i_p].at().ci_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().ci_err=true;
									cntr.p_hd[i_p].at().ci_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) pdu = pdu.substr(0,1) + pdu.substr(3,8) + pdu.substr(1,2);
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(9,2))!=(cntr.DCONCRC(pdu.substr(0,9))))
										{
											cntr.p_hd[i_p].at().ci_err=true;
											cntr.p_hd[i_p].at().ci_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().ci_err))
										{
											cntr.p_hd[i_p].at().ci_err=true;
											cntr.p_hd[i_p].at().ci_txterr=_("Invalid module response");
										}
								}
						}
					//Set DAQ attributes
					if (!cntr.p_hd[i_p].at().ci_err) cntr.p_hd[i_p].at().CI[0]=strtol(pdu.substr(1,8).c_str(),NULL,16);

					//> Request with module
					pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr] + "1";
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=11; else acq_len=9;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().ci_err=true;
							cntr.p_hd[i_p].at().ci_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().ci_err=true;
									cntr.p_hd[i_p].at().ci_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) pdu = pdu.substr(0,1) + pdu.substr(3,8) + pdu.substr(1,2);
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(9,2))!=(cntr.DCONCRC(pdu.substr(0,9))))
										{
											cntr.p_hd[i_p].at().ci_err=true;
											cntr.p_hd[i_p].at().ci_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().ci_err))
										{
											cntr.p_hd[i_p].at().ci_err=true;
											cntr.p_hd[i_p].at().ci_txterr=_("Invalid module response");
										}
								}
						}
					//Set DAQ attributes
					if (!cntr.p_hd[i_p].at().ci_err) cntr.p_hd[i_p].at().CI[1]=strtol(pdu.substr(1,8).c_str(),NULL,16);

					//> Request with module
					pdu = "#" + HexSymbol[15 & (cntr.p_hd[i_p].at().mod_addr>>4)] + HexSymbol[15 & cntr.p_hd[i_p].at().mod_addr] + "2";
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
					if (cntr.p_hd[i_p].at().crc_ctrl) acq_len=11; else acq_len=9;
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					//Check errors
					if (pdu.size()==0)
						{
							cntr.p_hd[i_p].at().ci_err=true;
							cntr.p_hd[i_p].at().ci_txterr=cntr.p_hd[i_p].at().acq_err;
						}
					else
						{
							if ((int)pdu.size()!=acq_len)
								{
									cntr.p_hd[i_p].at().ci_err=true;
									cntr.p_hd[i_p].at().ci_txterr=_("Module packet length error");
								}
							else
								{
									if (cntr.p_hd[i_p].at().crc_ctrl) pdu = pdu.substr(0,1) + pdu.substr(3,8) + pdu.substr(1,2);
									if (cntr.p_hd[i_p].at().crc_ctrl) if ((pdu.substr(9,2))!=(cntr.DCONCRC(pdu.substr(0,9))))
										{
											cntr.p_hd[i_p].at().ci_err=true;
											cntr.p_hd[i_p].at().ci_txterr=_("Invalid module CRC");
										}
									if ((pdu.substr(0,1)!=">")&&(!cntr.p_hd[i_p].at().ci_err))
										{
											cntr.p_hd[i_p].at().ci_err=true;
											cntr.p_hd[i_p].at().ci_txterr=_("Invalid module response");
										}
								}
						}
					//Set DAQ attributes
					if (!cntr.p_hd[i_p].at().ci_err) cntr.p_hd[i_p].at().CI[2]=strtol(pdu.substr(1,8).c_str(),NULL,16);
					break;
				}
				default:	break;
			}

			//Host signal
			switch (cntr.p_hd[i_p].at().host_signal)
			{
				case 0:	break;//No signal
				case 1://HostOK signal
				{
					pdu = "~**";
					if (cntr.p_hd[i_p].at().crc_ctrl) pdu+=cntr.DCONCRC(pdu);
//					cout << pdu.substr(0,pdu.size()) << " writing\n";
					cntr.p_hd[i_p].at().acq_err=cntr.DCONReq(pdu);
//					cout << pdu.substr(0,pdu.size()-1) << " reading\n";
					break;
				}
				default:	break;
			}
			cntr.p_hd[i_p].at().ai_err_get=cntr.p_hd[i_p].at().ai_err;
			cntr.p_hd[i_p].at().ao_err_get=cntr.p_hd[i_p].at().ao_err;
			cntr.p_hd[i_p].at().di_err_get=cntr.p_hd[i_p].at().di_err;
			cntr.p_hd[i_p].at().do_err_get=cntr.p_hd[i_p].at().do_err;
			cntr.p_hd[i_p].at().ci_err_get=cntr.p_hd[i_p].at().ci_err;
			cntr.p_hd[i_p].at().ai_txterr_get=cntr.p_hd[i_p].at().ai_txterr;
			cntr.p_hd[i_p].at().ao_txterr_get=cntr.p_hd[i_p].at().ao_txterr;
			cntr.p_hd[i_p].at().di_txterr_get=cntr.p_hd[i_p].at().di_txterr;
			cntr.p_hd[i_p].at().do_txterr_get=cntr.p_hd[i_p].at().do_txterr;
			cntr.p_hd[i_p].at().ci_txterr_get=cntr.p_hd[i_p].at().ci_txterr;
		}
		res.release();

		//> Calc acquisition process time
		cntr.tm_gath = TSYS::curTime()-t_cnt;
		}

		//> Calc next work time and sleep
		clock_gettime( CLOCK_REALTIME, &get_tm );
		work_tm = (((long long)get_tm.tv_sec*1000000000+get_tm.tv_nsec)/(long long)(cntr.period()*1000000000) + 1)*(long long)(cntr.period()*1000000000);
		if( last_tm == work_tm )	work_tm += (long long)(cntr.period()*1000000000);	//Fix early call
		last_tm = work_tm;
		get_tm.tv_sec = work_tm/1000000000; get_tm.tv_nsec = work_tm%1000000000;
		clock_nanosleep( CLOCK_REALTIME, TIMER_ABSTIME, &get_tm, NULL );
	}
	}
	catch( TError err )	{ mess_err( err.cat.c_str(), err.mess.c_str() ); }

	cntr.prc_st = false;

	return NULL;
}

void TMdContr::cntrCmdProc( XMLNode *opt )
{
	//> Get page info
	if(opt->name() == "info")
	{
	TController::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/cntr/cfg/ADDR",cfg("ADDR").fld().descr(),RWRWR_,"root",SDAQ_ID,3,"tp","str","dest","select","select","/cntr/cfg/serDevLst");
	return;
	}
	//> Process command to page
	string a_path = opt->attr("path");
	if(a_path == "/cntr/cfg/serDevLst" && ctrChkNode(opt))
	{
	    vector<string> sls;
	    if(SYS->transport().at().modPresent("Serial"))
		SYS->transport().at().at("Serial").at().outList(sls);
	    for(unsigned i_s = 0; i_s < sls.size(); i_s++)
		opt->childAdd("el")->setText(sls[i_s]);
	}
	else TController::cntrCmdProc(opt);
}

//******************************************************
//* TMdPrm                                             *
//******************************************************
TMdPrm::TMdPrm( string name, TTipParam *tp_prm ) : 
	TParamContr( name, tp_prm ), p_el("w_attr"), mod_addr(cfg("MOD_ADDR").getId()), crc_ctrl(cfg("CRC_CTRL").getBd()),
	host_signal(cfg("HOST_SIGNAL").getId()), ai_method(cfg("AI_METHOD").getId()), ai_range(cfg("AI_RANGE").getId()), ao_method(cfg("AO_METHOD").getId()),
	ao_range(cfg("AO_RANGE").getId()), di_method(cfg("DI_METHOD").getId()), do_method(cfg("DO_METHOD").getId()), ci_method(cfg("CI_METHOD").getId())
{
	// Default input/output data
	ai_err=false;
	ao_err=false;
	di_err=false;
	do_err=false;
	ci_err=false;
	for( int i = 0; i < 32; i++ ) {AI[i]=0;AO[i]=0;DI[i]=0;DO[i]=0;CI[i]=0;}
}

TMdPrm::~TMdPrm( )
{
	nodeDelAll( );
}

void TMdPrm::postEnable( int flag )
{
	TParamContr::postEnable(flag);
	if( !vlElemPresent(&p_el) )	vlElemAtt(&p_el);
}

TMdContr &TMdPrm::owner( )	{ return (TMdContr&)TParamContr::owner(); }

void TMdPrm::enable()
{
	if( enableStat() )	return;

	TParamContr::enable();

	//> Delete DAQ parameter's attributes
	for(unsigned i_f = 0; i_f < p_el.fldSize(); )
	{
//	    if( atoi(p_el.fldAt(i_f).reserve().c_str()) == mod_tp ) continue;
	    try { p_el.fldDel(i_f); continue; }
	    catch(TError err) { mess_warning(err.cat.c_str(),err.mess.c_str()); }
	    i_f++;
	}

	string sai_method = TSYS::int2str(ai_method);
	string sao_method = TSYS::int2str(ao_method);
	string sdi_method = TSYS::int2str(di_method);
	string sdo_method = TSYS::int2str(do_method);
	string sci_method = TSYS::int2str(ci_method);

	//> Make DAQ parameter's attributes

	switch(ai_method)
	{
	//"No AI"
	case 0:
	{
		break;
	}
	//"1AI (#AA)"
	case 1:
	{
		p_el.fldAdd( new TFld("ai_err","ai_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		for( int i_p = 0; i_p < 1; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AI%d",i_p).c_str(),TSYS::strMess("AI%d",i_p).c_str(),
			TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		break;
	}
	//"2AI (#AA)"
	case 2:
	{
		p_el.fldAdd( new TFld("ai_err","ai_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		for( int i_p = 0; i_p < 2; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AI%d",i_p).c_str(),TSYS::strMess("AI%d",i_p).c_str(),
			TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		break;
	}
	//"3AI (#AA)"
	case 3:
	{
		p_el.fldAdd( new TFld("ai_err","ai_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		for( int i_p = 0; i_p < 3; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AI%d",i_p).c_str(),TSYS::strMess("AI%d",i_p).c_str(),
			TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		break;
	}
	//"4AI (#AA)"
	case 4:
	{
		p_el.fldAdd( new TFld("ai_err","ai_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		for( int i_p = 0; i_p < 4; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AI%d",i_p).c_str(),TSYS::strMess("AI%d",i_p).c_str(),
			TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		break;
	}
	//"6AI (#AA)"
	case 6:
	{
		p_el.fldAdd( new TFld("ai_err","ai_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		for( int i_p = 0; i_p < 6; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AI%d",i_p).c_str(),TSYS::strMess("AI%d",i_p).c_str(),
			TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		break;
	}
	//"8AI (#AA)"
	case 8:
	{
		p_el.fldAdd( new TFld("ai_err","ai_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		for( int i_p = 0; i_p < 8; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AI%d",i_p).c_str(),TSYS::strMess("AI%d",i_p).c_str(),
			TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		break;
	}
	//"10AI (#AA)"
	case 10:
	{
		p_el.fldAdd( new TFld("ai_err","ai_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		for( int i_p = 0; i_p < 10; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AI%d",i_p).c_str(),TSYS::strMess("AI%d",i_p).c_str(),
			TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		break;
	}
	//"20AI (#AA)"
	case 20:
	{
		p_el.fldAdd( new TFld("ai_err","ai_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		for( int i_p = 0; i_p < 20; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AI%d",i_p).c_str(),TSYS::strMess("AI%d",i_p).c_str(),
			TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		break;
	}
	//"16AI (#AA^AA)"
	case 116:
	{
		p_el.fldAdd( new TFld("ai_err","ai_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		for( int i_p = 0; i_p < 16; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AI%d",i_p).c_str(),TSYS::strMess("AI%d",i_p).c_str(),
			TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sai_method.c_str()) );
		break;
	}
	default:
	{
		break;
	}
	}

	string ao_d;
	switch(ao_range)
	{
	//"Engeneer (00.000 20.000)"
	case 0:{ao_d="0;20"; break;}
	//"Engeneer (04.000 20.000)"
	case 1:{ao_d="4;20"; break;}
	//"Engeneer (00.000 10.000)"
	case 2:{ao_d="0;10"; break;}
	//"Engeneer (+00.000 +20.000)"
	case 3:{ao_d="0;20"; break;}
	//"Engeneer (+04.000 +20.000)"
	case 4:{ao_d="4;20"; break;}
	//"Engeneer (+00.000 +10.000)"
	case 5:{ao_d="0;10"; break;}
	//"Engeneer (-10.000 +10.000)"
	case 6:{ao_d="-10;10"; break;}
	//"Engeneer (+00.000 +05.000)"
	case 7:{ao_d="0;5"; break;}
	//"Engeneer (-05.000 +05.000)"
	case 8:{ao_d="-5;5"; break;}
	//"Percent (+000.00 +100.00)"
	case 9:{ao_d="0;100"; break;}
	//"Hexadecimal (000 FFF)"
	case 10:{ao_d="0;4095"; break;}
	default:{ao_d=""; break;}
	}

	switch(ao_method)
	{
	//"No AO"
	case 0:
	{
		break;
	}
	//"1AO (#AA)"
	case 1:
	{
		p_el.fldAdd( new TFld("ao_err","ao_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sao_method.c_str()) );
		for( int i_p = 0; i_p < 1; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AO%d",i_p).c_str(),TSYS::strMess("AO%d",i_p).c_str(),
			TFld::Real,TVal::DirRead|TVal::DirWrite,"20","1",ao_d.c_str(),"",sao_method.c_str()) );
		break;
	}
	//"2AO (#AA)"
	case 2:
	{
		p_el.fldAdd( new TFld("ao_err","ao_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sao_method.c_str()) );
		for( int i_p = 0; i_p < 2; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AO%d",i_p).c_str(),TSYS::strMess("AO%d",i_p).c_str(),
			TFld::Real,TVal::DirRead|TVal::DirWrite,"20","1",ao_d.c_str(),"",sao_method.c_str()) );
		break;
	}
	//"4AO (#AA)"
	case 4:
	{
		p_el.fldAdd( new TFld("ao_err","ao_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sao_method.c_str()) );
		for( int i_p = 0; i_p < 4; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("AO%d",i_p).c_str(),TSYS::strMess("AO%d",i_p).c_str(),
			TFld::Real,TVal::DirRead|TVal::DirWrite,"20","1",ao_d.c_str(),"",sao_method.c_str()) );
		break;
	}
	default:
	{
		break;
	}
	}

	switch(di_method)
	{
	//"No DI"
	case 0:
	{
		break;
	}
	//"3DI (@AA)"
	case 3:
	{
		p_el.fldAdd( new TFld("di_err","di_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		for( int i_p = 0; i_p < 3; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DI%d",i_p).c_str(),TSYS::strMess("DI%d",i_p).c_str(),
			TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		break;
	}
	//"4DI (@AA)"
	case 4:
	{
		p_el.fldAdd( new TFld("di_err","di_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		for( int i_p = 0; i_p < 4; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DI%d",i_p).c_str(),TSYS::strMess("DI%d",i_p).c_str(),
			TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		break;
	}
	//"7DI (@AA)"
	case 7:
	{
		p_el.fldAdd( new TFld("di_err","di_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		for( int i_p = 0; i_p < 7; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DI%d",i_p).c_str(),TSYS::strMess("DI%d",i_p).c_str(),
			TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		break;
	}
	//"8DI (@AA)"
	case 8:
	{
		p_el.fldAdd( new TFld("di_err","di_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		for( int i_p = 0; i_p < 8; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DI%d",i_p).c_str(),TSYS::strMess("DI%d",i_p).c_str(),
			TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		break;
	}
	//"14DI (@AA)"
	case 14:
	{
		p_el.fldAdd( new TFld("di_err","di_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		for( int i_p = 0; i_p < 14; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DI%d",i_p).c_str(),TSYS::strMess("DI%d",i_p).c_str(),
			TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		break;
	}
	//"16DI (@AA)"
	case 16:
	{
		p_el.fldAdd( new TFld("di_err","di_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		for( int i_p = 0; i_p < 16; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DI%d",i_p).c_str(),TSYS::strMess("DI%d",i_p).c_str(),
			TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		break;
	}
	//"1DI (@AADI)"
	case 101:
	{
		p_el.fldAdd( new TFld("di_err","di_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		for( int i_p = 0; i_p < 1; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DI%d",i_p).c_str(),TSYS::strMess("DI%d",i_p).c_str(),
			TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		break;
	}
	//"8DI (@AA,FF00)"
	case 201:
	{
		p_el.fldAdd( new TFld("di_err","di_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		for( int i_p = 0; i_p < 8; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DI%d",i_p).c_str(),TSYS::strMess("DI%d",i_p).c_str(),
			TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdi_method.c_str()) );
		break;
	}
	default:
	{
		break;
	}
	}


	switch(do_method)
	{
	//"No DO"
	case 0:
	{
		break;
	}
	//"2DO (@AA,0300)"
	case 2:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 2; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"3DO (@AA,7)"
	case 3:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 3; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"4DO (@AA,F)"
	case 4:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 4; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"5DO (@AA,1F)"
	case 5:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 5; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"7DO (@AA,7F)"
	case 7:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 7; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"8DO (@AA,FF)"
	case 8:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 8; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"12DO (@AA,0FFF)"
	case 12:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 12; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"13DO (@AA,1FFF)"
	case 13:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 13; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"16DO (@AA,FFFF)"
	case 16:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 16; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"2DO (^AADOVVV)"
	case 102:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 2; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"3DO (^AADOVVV)"
	case 103:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 3; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"2DO (@AADO)"
	case 202:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 2; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"4DO (@AADO)"
	case 204:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 4; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"6DO (@AADODD)"
	case 306:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 6; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"2DO (@AADO0D)"
	case 402:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 2; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"4DO (@(^)AADO0D)"
	case 504:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 4; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"4DO (@AA,0F00)"
	case 604:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 4; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	//"8DO (@AA,FF00)"
	case 608:
	{
		p_el.fldAdd( new TFld("do_err","do_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sdo_method.c_str()) );
		for( int i_p = 0; i_p < 8; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("DO%d",i_p).c_str(),TSYS::strMess("DO%d",i_p).c_str(),
			TFld::Boolean,TVal::DirRead|TVal::DirWrite,"","","","",sdo_method.c_str()) );
		break;
	}
	default:
	{
		break;
	}
	}

	switch(ci_method)
	{
	//"No CI"
	case 0:
	{
		break;
	}
	//"2CI (#AA)"
	case 2:
	{
		p_el.fldAdd( new TFld("ci_err","ci_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sci_method.c_str()) );
		for( int i_p = 0; i_p < 2; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("CI%d",i_p).c_str(),TSYS::strMess("CI%d",i_p).c_str(),
			TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sci_method.c_str()) );
		break;
	}
	//"3CI (#AA)"
	case 3:
	{
		p_el.fldAdd( new TFld("ci_err","ci_err",TFld::Boolean,TFld::NoWrite|TVal::DirRead,"","","","",sci_method.c_str()) );
		for( int i_p = 0; i_p < 3; i_p++ )
		p_el.fldAdd( new TFld(TSYS::strMess("CI%d",i_p).c_str(),TSYS::strMess("CI%d",i_p).c_str(),
			TFld::Real,TFld::NoWrite|TVal::DirRead,"","","","",sci_method.c_str()) );
		break;
	}
	default:
	{
		break;
	}
	}

	owner().prmEn( id(), true );
}

void TMdPrm::disable()
{
	if( !enableStat() )  return;

	owner().prmEn( id(), false );

	TParamContr::disable();

	//- Set EVAL to parameter attributes -
	vector<string> ls;
	elem().fldList(ls);
	for(unsigned i_el = 0; i_el < ls.size(); i_el++)
	    vlAt(ls[i_el]).at().setS( EVAL_STR, 0, true );
}

void TMdPrm::vlGet( TVal &val )
{
	if( !enableStat() || !owner().startStat() )
	{
	if( val.name() == "err" )
	{
		if( !enableStat() )			val.setS(_("1:Parameter is disabled."),0,true);
		else if(!owner().startStat())	val.setS(_("2:Acquisition is stoped."),0,true);
	}
	else val.setS(EVAL_STR,0,true);
	return;
	}

	if( owner().redntUse( ) ) return;
//	cout<<val.name().substr(0,2)<<"  "<<atoi(val.name().substr(2,val.name().size()-2).c_str())<<"\n";
	if (val.name()=="ai_err") val.setB(ai_err_get,0,true);
	if (val.name()=="ao_err") val.setB(ao_err_get,0,true);
	if (val.name()=="di_err") val.setB(di_err_get,0,true);
	if (val.name()=="do_err") val.setB(do_err_get,0,true);
	if (val.name()=="ci_err") val.setB(ci_err_get,0,true);
	if (val.name()=="err")
		{
			if (ai_err_get) val.setS(ai_txterr_get,0,true);
				else if (ao_err_get) val.setS(ao_txterr_get,0,true);
					else if (di_err_get) val.setS(di_txterr_get,0,true);
						else if (do_err_get) val.setS(do_txterr_get,0,true);
							else if (ci_err_get) val.setS(ci_txterr_get,0,true);
								else val.setS(_("No errors"),0,true);
		}
	if (val.name().substr(0,2)=="AI") val.setR(AI[atoi(val.name().substr(2,val.name().size()-2).c_str())],0,true);
	if (val.name().substr(0,2)=="AO") val.setR(AO[atoi(val.name().substr(2,val.name().size()-2).c_str())],0,true);
	if (val.name().substr(0,2)=="DI") val.setB(DI[atoi(val.name().substr(2,val.name().size()-2).c_str())],0,true);
	if (val.name().substr(0,2)=="DO") val.setB(DO[atoi(val.name().substr(2,val.name().size()-2).c_str())],0,true);
	if (val.name().substr(0,2)=="CI") val.setR(CI[atoi(val.name().substr(2,val.name().size()-2).c_str())],0,true);
}

void TMdPrm::vlSet( TVal &valo, const TVariant &pvl )
{
	if( !enableStat() )	valo.setI( EVAL_INT, 0, true );

	//> Send to active reserve station
	if( owner().redntUse( ) )
	{
	if( valo.getS() == pvl.getS() ) return;
	XMLNode req("set");
	req.setAttr("path",nodePath(0,true)+"/%2fserv%2fattr")->childAdd("el")->setAttr("id",valo.name())->setText(valo.getS());
	SYS->daq().at().rdStRequest(owner().workId(),req);
	return;
	}

	//> Direct write
//	if (valo.name().substr(0,2)=="AI") AI[atoi(valo.name().substr(2,valo.name().size()-2).c_str())]=valo.getR(NULL,true);
	if (valo.name().substr(0,2)=="AO") AO[atoi(valo.name().substr(2,valo.name().size()-2).c_str())]=valo.getR(NULL,true);
//	if (valo.name().substr(0,2)=="DI") DI[atoi(valo.name().substr(2,valo.name().size()-2).c_str())]=valo.getB(NULL,true);
	if (valo.name().substr(0,2)=="DO") DO[atoi(valo.name().substr(2,valo.name().size()-2).c_str())]=valo.getB(NULL,true);
//	if (valo.name().substr(0,2)=="CI") CI[atoi(valo.name().substr(2,valo.name().size()-2).c_str())]=valo.getR(NULL,true);
}

void TMdPrm::vlArchMake( TVal &val )
{
	if( val.arch().freeStat() ) return;
	val.arch().at().setSrcMode( TVArchive::ActiveAttr, val.arch().at().srcData() );
	val.arch().at().setPeriod( (long long)(owner().period()*1000000) );
	val.arch().at().setHardGrid( true );
	val.arch().at().setHighResTm( true );
}

void TMdPrm::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
        TParamContr::cntrCmdProc(opt);
        if(ai_method == 0) ctrRemoveNode(opt,"/prm/cfg/AI_RANGE");
        if(ao_method == 0) ctrRemoveNode(opt,"/prm/cfg/AO_RANGE");
        return;
    }
    //> Process command to page
    TParamContr::cntrCmdProc(opt);
}
