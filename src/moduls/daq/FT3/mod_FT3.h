//OpenSCADA system module DAQ.FT3 file: mod_ft3.h
/***************************************************************************
 *   Copyright (C) 2011-2015 by Maxim Kochetkov                            *
 *   fido_max@inbox.ru                                                     *
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

#ifndef MOD_ft3_H
#define MOD_ft3_H

#include <string>
#include <vector>
#include <map>
#include <deque>

#include <tsys.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using std::map;
using std::deque;
using namespace OSCADA;

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"FT3"
#define MOD_NAME	_("DAQ FT3")
#define MOD_TYPE	SDAQ_ID
#define VER_TYPE	SDAQ_VER
#define MOD_VER		"0.1.1"
#define AUTHORS		_("Maxim Kothetkov, Olga Avdeyeva, Olga Kuzmickaya")
#define DESCRIPTION	_("Allow realization of FT3 master/slave service")
#define LICENSE		"GPL2"
//*************************************************

#include "da.h"

namespace FT3
{
    time_t GRSDateTimeToTime_t(uint8_t *);

    void Time_tToGRSDateTime(uint8_t *, time_t);
    time_t KADateTimeToTime_t(uint8_t *);

    void Time_tToKADateTime(uint8_t *, time_t);
    /*    struct dhm{
     uint16_t d;
     uint8_t h;
     uint16_t ms100;
     };*/
    typedef struct sMsg  // FT3 message
    {
	uint8_t D[252]; // data
	uint8_t L; // length
	uint8_t C; // command
	uint8_t A; // destination address
	uint8_t B; // source address
    } tagMsg;

    typedef enum eCodFT3
    {
	ResetChan = 0x0,
	ResData2 = 0x1,
	SetData = 0x3,
	TimSync = 0x4,
	Reset = 0x5,
	Winter = 0x6,
	Summer = 0x7,
	ReqData1 = 0xA,
	ReqData2 = 0xB,
	ReqData = 0xC,
	AddrReq = 0xD,

	GOOD2 = 0,
	BAD2 = 1,
	GOOD3 = 8,
	BAD3 = 9
    } CodFT3;
    typedef enum eModeTask
    {
	TaskNone = 0, TaskIdle = 1, TaskRefresh = 2, TaskSet = 3
    } ModeTask;

#define mlD 252
#define nBE 400

    struct blockEvents
    {
	uint16_t d; 		//date (15-9 - year, 8-0 day)
	uint8_t h; 		//hour
	uint8_t l; 		//length
	uint8_t mD[mlD - 3]; 	//data
    };

    // Event block chain element
    struct el_chBE
    {
	blockEvents BE; //Event block
	el_chBE *next;  //Next event block pointer
    };

    class chain_BE
    {
    public:
	el_chBE *head;  //First event block
	el_chBE *tail;	//Last event block
	el_chBE *temp;	//Temp pointer
//	pthread_mutex_t eventRes;

	chain_BE()
	{
	    head = NULL;
	    tail = NULL;
	    temp = NULL;
	    /*            pthread_mutexattr_t attrM;
	     pthread_mutexattr_init(&attrM);
	     pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
	     pthread_mutex_init(&eventRes, &attrM);
	     pthread_mutexattr_destroy(&attrM);*/
	}
	;
	/*        ~chain_BE(){
	 pthread_mutex_destroy(&eventRes);
	 };*/

	el_chBE* getdel()
	{
	    //MtxAlloc res(eventRes, true);
	    temp = head;
	    if(head) head = head->next;
	    if(!head) tail = NULL;
	    return temp;
	}
	;

	void insert(el_chBE *p)
	{
	    //MtxAlloc res(eventRes, true);
	    if(p) {
		if(head) {
		    tail->next = p;
		} else {
		    head = p;
		}
		tail = p;
		p->next = NULL;
	    }
	}
	;
    };

#define task_None 0
#define task_Idle 1
#define task_Refresh 2

//!!! DAQ-subsystem parameter object realisation define. Add methods and attributes for your need.
//*************************************************
//* Modft3::TMdPrm                               *
//*************************************************
    class TMdContr;

    class TMdPrm: public TParamContr, public TValFunc
    {
//	friend class DA;
    public:
	//Methods

	TMdPrm(string name, TTypeParam *tp_prm);
	~TMdPrm();

//	TCntrNode &operator=( TCntrNode &node );

	void enable();
	void disable();

	TElem &elem()
	{
	    return p_el;
	}
	TElem &prmIOE();
	TMdContr &owner();

	//!!! Get data from Logic FT3 parameter
	uint8_t cmdGet(uint16_t, uint8_t *);
	uint8_t cmdSet(uint8_t *, uint8_t);
	uint16_t Task(uint16_t);
	uint16_t HandleEvent(uint8_t *);
	void tmHandler(void);
	TElem p_el;			//Work atribute elements
//	string typeName();
	string typeDBName();

    protected:
	//Methods
	void load_();
	void save_();
	void cntrCmdProc(XMLNode *opt);

    private:
	//Methods
	void postEnable(int flag);
//	void postDisable( int flag );
	void vlGet(TVal &val);
	void vlSet(TVal &vo, const TVariant &vl, const TVariant &pvl);
	void vlArchMake(TVal &val);
	//Attributes
	//!!! Parameter's structure element
	DA *mDA;
	bool needApply;

    };

    class TFT3Channel
    {
    public:
	TFT3Channel() :
		FCB2(0xFF), FCB3(0xFF)
	{
	    BE = new el_chBE[nBE];
	    if(BE) {
		for(int i = 0; i < nBE; i++)
		    empt.insert(&BE[i]);
	    }
	    resp2.L = 0;
	    resp3.L = 0;
	}
	virtual ~TFT3Channel()
	{
	}
	uint8_t FCB2, FCB3;

	el_chBE *BE;
	chain_BE empt, C1, C2;
	tagMsg resp2, resp3;
	void PushInBE(uint8_t type, uint8_t length, uint16_t id, uint8_t *E, uint8_t *DHM);
    };


//*************************************************
//* Modft3::TMdContr                             *
//*************************************************
    class TMdContr: public TController
    {
	friend class TMdPrm;
    public:
	//Methods
	TMdContr(string name_c, const string &daq_db, TElem *cfgelem);
	~TMdContr();

	string getStatus();

	int64_t period()
	{
	    return mPer;
	}
//	string	cron( )		{ return mSched; }
//	string	addr( )		{ return mAddr; }
	int prior()
	{
	    return mPrior;
	}

	AutoHD<TMdPrm> at(const string &nm)
	{
	    return TController::at(nm);
	}

	bool isLogic();
	bool Transact(tagMsg * t);
	bool ProcessMessage(tagMsg *, tagMsg *);
	void PushInBE(uint8_t type, uint8_t length, uint16_t id, uint8_t *E);
	time_t DateTimeToTime_t(uint8_t *d)
	{
	    if(cfg("PRTTYPE").getS() == "GRS") {
		return GRSDateTimeToTime_t(d);
	    } else {
		return KADateTimeToTime_t(d);
	    }
	}

	void Time_tToDateTime(uint8_t *d, time_t t)
	{
	    if(cfg("PRTTYPE").getS() == "GRS") {
		Time_tToGRSDateTime(d, t);
	    } else {
		Time_tToKADateTime(d, t);
	    }
	}

	uint8_t devAddr;
	uint8_t nChannel;

    protected:

	//Methods
	void prmEn(TMdPrm *prm, bool val);

	//!!! Processing virtual functions for start and stop DAQ-controller
	void start_();
	void stop_();

	//!!! FT3 CRC
	uint16_t CRC(char *data, uint16_t length);
	void MakePacket(tagMsg *msg, char *io_buf, uint16_t *len);
	bool VerCRC(char *p, uint16_t l);
	uint16_t VerifyPacket(char *t, uint16_t *l);
	uint16_t ParsePacket(char *t, uint16_t l, tagMsg * msg);
	uint16_t Len(uint16_t l);

	//!!! Get data from Logic FT3 controller
	uint8_t cmdGet(uint16_t, uint8_t *);
	uint8_t cmdSet(uint8_t *, uint8_t);

    private:
	//Methods
	//!!! Processing virtual functions for self object-parameter creation.
	TParamContr *ParamAttach(const string &name, int type);
	//!!! Background task's function for periodic data acquisition.
	static void *DAQTask(void *icntr);
	static void *LogicTask(void *icntr);
	void cntrCmdProc(XMLNode *opt);

	//Attributes
	//!!! The resource for Enable parameters.
	pthread_mutex_t enRes, eventRes;
	//!!! The links to the controller's background task properties into config.
	int64_t mPer;
	int64_t &mPrior;			//Process task priority

	//!!! Background task's sync properties
	bool prc_st,		// Process task active
		endrun_req;	// Request to stop of the Process task

	bool NeedInit;

	int mNode;

	//!!! Enabled and processing parameter's links list container.
	vector<AutoHD<TMdPrm> > pHd;

	double tm_gath;	// Gathering time
	vector<TFT3Channel> Channels;
    };

//!!! Root module object define. Add methods and attributes for your need.
//*************************************************
//* Modft3::TTpContr                             *
//*************************************************
    class TTpContr: public TTypeDAQ
    {
    public:
	//Methods
	TTpContr(string name);
	~TTpContr();

	TElem &prmIOE()
	{
	    return elPrmIO;
	}
    protected:
	//Methods
	void postEnable(int flag);
	void load_();
	void save_();
	bool redntAllow()
	{
	    return true;
	}

    private:
	//Methods
	TController *ContrAttach(const string &name, const string &daq_db);
	//Attributes
	TElem elPrmIO;
    };

//!!! The module root link
    extern TTpContr *mod;

} //End namespace Modft3

#endif //MOD_ft3_H
