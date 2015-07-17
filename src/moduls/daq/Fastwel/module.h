//OpenSCADA system module DAQ.Fastwel file: module.h
/***************************************************************************
 *   Copyright (C) 2014 by Maxim Kochetkov                                 *
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

#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>

#include <tcontroller.h>
#include <ttypedaq.h>
#include <tparamcontr.h>

#include <fbus.h>
#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

#define FBUS_MAX_NET 64

namespace ModFastwel {

//*************************************************
//* ModFastwel::TMdPrm                           *
//*************************************************
    class TMdContr;

    class TMdPrm:public TParamContr {
      public:
	enum State {
	    // Generic
	    StateWork = 0,
	    StateWrongType = 1,
	    StateUndef = 0xFF,
	};
	//Methods
	TMdPrm(string name, TTypeParam * tp_prm);
	~TMdPrm();

	TElem & elem() {
	    return p_el;
	} void enable();
	void disable();

	void getVals();

	TMdContr & owner();
	bool InitDI(DIM_CONFIGURATION* pConfigDIM);

      protected:
	//Methods
	void load_();
	void save_();

      private:
	//Methods
	void postEnable(int flag);
	void vlGet(TVal & vo);
	void vlSet(TVal & vo, const TVariant & vl, const TVariant & pvl);
	void cntrCmdProc(XMLNode * opt);
	void vlArchMake(TVal & val);

	//Attributes
	TElem p_el;		//Work atribute elements
	FIO_MODULE_DESC mModDesc;
	TCfg & mID;		// Schedule
	uint32_t mState;
	string mTypeName;

	uint32_t nDI, nAI, nDO, nAO;
	double kAI, kAO, dAO;
	uint8_t mDOState;

	uint8_t mModConfig[64];
	FIO_MODULE_COMMON_CONF mModComConfig;

    };

//*************************************************
//* ModFastwel::TMdContr                             *
//*************************************************
    class TMdContr:public TController {
	friend class TMdPrm;
      public:
	//Methods
	TMdContr(string name_c, const string & daq_db,::TElem * cfgelem);
	 ~TMdContr();

	string getStatus();

	int64_t period() {
	    return mPer;
	} string cron() {
	    return mSched;
	}
	int prior() {
	    return mPrior;
	}

	AutoHD < TMdPrm > at(const string & nm) {
	    return TController::at(nm);
	}

	void GetNodeDescription(int, PFIO_MODULE_DESC);
	int ReadInputs(int, void *, size_t, size_t);
	int WriteOutputs(int, void *, size_t, size_t);
	int SetNodeSpecificParameters(int, void *, size_t, size_t);
	int GetNodeSpecificParameters(int, void *, size_t, size_t);
	int GetNodeCommonParameters(int, PFIO_MODULE_COMMON_CONF, size_t);
	int SetNodeCommonParameters(int, PFIO_MODULE_COMMON_CONF, size_t);
	int WriteConfig(int);
	int ReadConfig(int);
	int SaveConfig(int);
      protected:
	//Methods
	void prmEn(const string & id, bool val);
	void enable_();
	void start_();
	void stop_();

      private:
	//Methods
	TParamContr * ParamAttach(const string & name, int type);
	void cntrCmdProc(XMLNode * opt);
	static void *Task(void *icntr);

	//Attributes
	Res en_res;		// Resource for enable params
	TCfg & mSched,		// Schedule
	    &mPrior,		// Process task priority
	    &mNet;		// Network number

	int64_t mPer;

	bool prcSt,		// Process task active
	    callSt,		// Calc now stat
	    endrunReq;		// Request to stop of the Process task

	vector < AutoHD < TMdPrm > >p_hd;

	double tmGath;		// Gathering time
    };

//*************************************************
//* ModFastwel::TTpContr                             *
//*************************************************
    class TTpContr:public TTypeDAQ {
      public:
	//Methods
	TTpContr(string name);
	~TTpContr();

	void FBUS_Start();
	void FBUS_finish();
	void FBUS_fbusGetVersion();
	void FBUS_fbusOpen(int);
	void FBUS_fbusClose(int);
	void FBUS_fbusRescan(int);
	void FBUS_fbusGetNodeDescription(int, int, PFIO_MODULE_DESC);
	int FBUS_fbusReadInputs(int, int, void *, size_t, size_t);
	int FBUS_fbusWriteOutputs(int, int, void *, size_t, size_t);
	int FBUS_fbusWriteConfig(int, int);
	int FBUS_fbusReadConfig(int, int);
	int FBUS_fbusSaveConfig(int, int);
	int FBUS_fbusGetNodeSpecificParameters(int, int, void *, size_t, size_t);
	int FBUS_fbusSetNodeSpecificParameters(int, int, void *, size_t, size_t);
	int FBUS_fbusGetNodeCommonParameters(int, int, PFIO_MODULE_COMMON_CONF, size_t);
	int FBUS_fbusSetNodeCommonParameters(int, int, PFIO_MODULE_COMMON_CONF, size_t);

      protected:
	//Methods
	void postEnable(int flag);

	void load_();
	void save_();

	bool redntAllow() {
	    return true;
	} int verMajor, verMinor;

      private:
	//Methods
	TController * ContrAttach(const string & name, const string & daq_db);

	//Attributes
	bool FBUS_initOK;
	Res FBUSRes;
	FBUS_HANDLE hNet[FBUS_MAX_NET];
	size_t modCount[FBUS_MAX_NET];

    };

    extern TTpContr *mod;

}				//End namespace ModFastwel

#endif //MODULE_H
