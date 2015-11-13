//OpenSCADA system module DAQ.FT3 file: ACCOUNT.cpp
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

#include <sys/times.h>

#include <tsys.h>

#include "mod_FT3.h"
#include "ACCOUNT.h"

using namespace FT3;

uint8_t B_ACCOUNT::SACchannel::SetNewPeriod(uint8_t addr, uint16_t prmID, uint8_t *val)
{
    uint8_t start[5] = { val[0], val[1], val[2], 0, 0 };
    StartDate.Update(da->DateTimeToTime_t(start));
    EndDate.Update(StartDate.vl + TSYS::getUnalign16(val + 3) * 3600);
    uint8_t E[6] = { addr, val[0], val[1], val[2], val[3], val[4] };
    da->PushInBE(1, sizeof(E), prmID, E);
    return 5 + 2;
}

B_ACCOUNT::B_ACCOUNT(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params) :
	DA(prm), ID(id), count_n(n), with_params(has_params)
{
    mTypeFT3 = GRS;
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");
    for(int i = 0; i < count_n; i++) {
	AddChannel(i);
    }
    loadIO(true);
}

B_ACCOUNT::~B_ACCOUNT()
{

}

void B_ACCOUNT::AddChannel(uint8_t iid)
{
    data.push_back(SACchannel(iid, this));
    AddAttr(data.back().State.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:0", iid + 1));
    AddAttr(data.back().Value.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:1", iid + 1));
    if(with_params) {
	AddAttr(data.back().Period.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:2", iid + 1));
	AddAttr(data.back().Sens.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:3", iid + 1));
	AddAttr(data.back().MinW.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:4", iid + 1));
	AddAttr(data.back().MaxW.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:4", iid + 1));
	AddAttr(data.back().MinA.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:5", iid + 1));
	AddAttr(data.back().MaxA.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:5", iid + 1));
	AddAttr(data.back().Sensors.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:6", iid + 1));
	AddAttr(data.back().ServiceQ.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:7", iid + 1));
	AddAttr(data.back().Hour.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:8", iid + 1));
	AddAttr(data.back().HourlyQ.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:9", iid + 1));
	AddAttr(data.back().Counter.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:10", iid + 1));
	AddAttr(data.back().HourQ.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:11", iid + 1));
	AddAttr(data.back().HourdP.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:11", iid + 1));
	AddAttr(data.back().HourT.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:11", iid + 1));
	AddAttr(data.back().HourP.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:11", iid + 1));
	AddAttr(data.back().HourE.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:11", iid + 1));
	AddAttr(data.back().StartDate.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:12", iid + 1));
	AddAttr(data.back().EndDate.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:12", iid + 1));
	AddAttr(data.back().AvgQ.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:13", iid + 1));
	AddAttr(data.back().AvgdP.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:13", iid + 1));
	AddAttr(data.back().AvgT.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:13", iid + 1));
	AddAttr(data.back().AvgP.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:13", iid + 1));
	AddAttr(data.back().AvgE.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:13", iid + 1));
	AddAttr(data.back().PeriodQ.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:14", iid + 1));
	AddAttr(data.back().Density.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:15", iid + 1));
	AddAttr(data.back().Asperity.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:16", iid + 1));
	AddAttr(data.back().ConcentrN.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:17", iid + 1));
	AddAttr(data.back().ConcentrCO.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:18", iid + 1));
	AddAttr(data.back().DiameterM.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:19", iid + 1));
	AddAttr(data.back().FactorM.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:20", iid + 1));
	AddAttr(data.back().DiameterP.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:21", iid + 1));
	AddAttr(data.back().FactorP.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:22", iid + 1));
	AddAttr(data.back().MethodM.lnk, TFld::Integer, TVal::DirWrite, TSYS::strMess("%d:23", iid + 1));
	AddAttr(data.back().TestdP.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:24", iid + 1));
	AddAttr(data.back().TestT.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:24", iid + 1));
	AddAttr(data.back().TestP.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:24", iid + 1));
	AddAttr(data.back().TestQ.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:25", iid + 1));
	AddAttr(data.back().RadiusM.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:26", iid + 1));
	AddAttr(data.back().PressureA.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:27", iid + 1));
	AddAttr(data.back().dP.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:28", iid + 1));
	AddAttr(data.back().T.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:28", iid + 1));
	AddAttr(data.back().P.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:28", iid + 1));
	AddAttr(data.back().E.lnk, TFld::Real, TVal::DirWrite, TSYS::strMess("%d:28", iid + 1));
    }
}

string B_ACCOUNT::getStatus(void)
{
    string rez;
    if(NeedInit) {
	rez = "20: Опрос";
    } else {
	rez = "0: Норма";
    }
    return rez;

}

void B_ACCOUNT::loadIO(bool force)
{
    if(mPrm.owner().startStat() && !force) {
	mPrm.modif(true);
	return;
    }	//Load/reload IO context only allow for stopped controllers for prevent throws

    for(int i = 0; i < count_n; i++) {
	loadLnk(data[i].State.lnk);
	loadLnk(data[i].Value.lnk);
	loadLnk(data[i].Period.lnk);
	loadLnk(data[i].Sens.lnk);
	loadLnk(data[i].MinW.lnk);
	loadLnk(data[i].MaxW.lnk);
	loadLnk(data[i].MinA.lnk);
	loadLnk(data[i].MaxA.lnk);
	loadLnk(data[i].Sensors.lnk);
	loadLnk(data[i].ServiceQ.lnk);
	loadLnk(data[i].Hour.lnk);
	loadLnk(data[i].HourlyQ.lnk);
	loadLnk(data[i].Counter.lnk);
	loadLnk(data[i].HourQ.lnk);
	loadLnk(data[i].HourdP.lnk);
	loadLnk(data[i].HourT.lnk);
	loadLnk(data[i].HourP.lnk);
	loadLnk(data[i].HourE.lnk);
	loadLnk(data[i].StartDate.lnk);
	loadLnk(data[i].EndDate.lnk);
	loadLnk(data[i].AvgQ.lnk);
	loadLnk(data[i].AvgdP.lnk);
	loadLnk(data[i].AvgT.lnk);
	loadLnk(data[i].AvgP.lnk);
	loadLnk(data[i].AvgE.lnk);
	loadLnk(data[i].PeriodQ.lnk);
	loadLnk(data[i].Density.lnk);
	loadLnk(data[i].Asperity.lnk);
	loadLnk(data[i].ConcentrN.lnk);
	loadLnk(data[i].ConcentrCO.lnk);
	loadLnk(data[i].DiameterM.lnk);
	loadLnk(data[i].FactorM.lnk);
	loadLnk(data[i].DiameterP.lnk);
	loadLnk(data[i].FactorP.lnk);
	loadLnk(data[i].MethodM.lnk);
	loadLnk(data[i].TestdP.lnk);
	loadLnk(data[i].TestT.lnk);
	loadLnk(data[i].TestP.lnk);
	loadLnk(data[i].TestQ.lnk);
	loadLnk(data[i].RadiusM.lnk);
	loadLnk(data[i].PressureA.lnk);
	loadLnk(data[i].dP.lnk);
	loadLnk(data[i].T.lnk);
	loadLnk(data[i].P.lnk);
	loadLnk(data[i].E.lnk);
    }
}

void B_ACCOUNT::saveIO()
{
    for(int i = 0; i < count_n; i++) {
	saveLnk(data[i].State.lnk);
	saveLnk(data[i].Value.lnk);
	saveLnk(data[i].Period.lnk);
	saveLnk(data[i].Sens.lnk);
	saveLnk(data[i].MinW.lnk);
	saveLnk(data[i].MaxW.lnk);
	saveLnk(data[i].MinA.lnk);
	saveLnk(data[i].MaxA.lnk);
	saveLnk(data[i].Sensors.lnk);
	saveLnk(data[i].ServiceQ.lnk);
	saveLnk(data[i].Hour.lnk);
	saveLnk(data[i].HourlyQ.lnk);
	saveLnk(data[i].Counter.lnk);
	saveLnk(data[i].HourQ.lnk);
	saveLnk(data[i].HourdP.lnk);
	saveLnk(data[i].HourT.lnk);
	saveLnk(data[i].HourP.lnk);
	saveLnk(data[i].HourE.lnk);
	saveLnk(data[i].StartDate.lnk);
	saveLnk(data[i].EndDate.lnk);
	saveLnk(data[i].AvgQ.lnk);
	saveLnk(data[i].AvgdP.lnk);
	saveLnk(data[i].AvgT.lnk);
	saveLnk(data[i].AvgP.lnk);
	saveLnk(data[i].AvgE.lnk);
	saveLnk(data[i].PeriodQ.lnk);
	saveLnk(data[i].Density.lnk);
	saveLnk(data[i].Asperity.lnk);
	saveLnk(data[i].ConcentrN.lnk);
	saveLnk(data[i].ConcentrCO.lnk);
	saveLnk(data[i].DiameterM.lnk);
	saveLnk(data[i].FactorM.lnk);
	saveLnk(data[i].DiameterP.lnk);
	saveLnk(data[i].FactorP.lnk);
	saveLnk(data[i].MethodM.lnk);
	saveLnk(data[i].TestdP.lnk);
	saveLnk(data[i].TestT.lnk);
	saveLnk(data[i].TestP.lnk);
	saveLnk(data[i].TestQ.lnk);
	saveLnk(data[i].RadiusM.lnk);
	saveLnk(data[i].PressureA.lnk);
	saveLnk(data[i].dP.lnk);
	saveLnk(data[i].T.lnk);
	saveLnk(data[i].P.lnk);
	saveLnk(data[i].E.lnk);
    }
}

void B_ACCOUNT::tmHandler(void)
{
    for(int i = 0; i < count_n; i++) {
	if(with_params) {
	    UpdateParam8(data[i].Period, PackID(ID, (i + 1), 2), 1);
	    UpdateParamFl(data[i].Sens, PackID(ID, (i + 1), 3), 1);
	    UpdateParam2Fl(data[i].MinW, data[i].MaxW, PackID(ID, (i + 1), 4), 1);
	    UpdateParam2Fl(data[i].MinA, data[i].MaxA, PackID(ID, (i + 1), 5), 1);
	    UpdateParam32(data[i].Sensors, PackID(ID, (i + 1), 6), 1);
	    UpdateParamFl(data[i].ServiceQ, PackID(ID, (i + 1), 7), 1);
	    UpdateParam8(data[i].Hour, PackID(ID, (i + 1), 2), 8);
	    UpdateParamFl(data[i].HourlyQ, PackID(ID, (i + 1), 9), 0);
	    UpdateParamFl(data[i].Counter, PackID(ID, (i + 1), 10), 0);
	    /*	    UpdateParamFl(data[i].HourQ, PackID(ID, (i + 1), 11), 1);
	     UpdateParamFl(data[i].HourdP, PackID(ID, (i + 1), 3), 1);
	     UpdateParamFl(data[i].HourT, PackID(ID, (i + 1), 3), 1);
	     UpdateParamFl(data[i].HourP, PackID(ID, (i + 1), 3), 1);
	     UpdateParamFl(data[i].HourE, PackID(ID, (i + 1), 3), 1);*/
	    UpdateParamFl(data[i].Density, PackID(ID, (i + 1), 15), 1);
	    UpdateParamFl(data[i].Asperity, PackID(ID, (i + 1), 16), 1);
	    UpdateParamFl(data[i].ConcentrN, PackID(ID, (i + 1), 17), 1);
	    UpdateParamFl(data[i].ConcentrCO, PackID(ID, (i + 1), 18), 1);
	    UpdateParamFl(data[i].DiameterM, PackID(ID, (i + 1), 19), 1);
	    UpdateParamFl(data[i].FactorM, PackID(ID, (i + 1), 20), 1);
	    UpdateParamFl(data[i].DiameterP, PackID(ID, (i + 1), 21), 1);
	    UpdateParamFl(data[i].FactorP, PackID(ID, (i + 1), 22), 1);
	    UpdateParam8(data[i].MethodM, PackID(ID, (i + 1), 23), 1);
	    UpdateParamFl(data[i].TestQ, PackID(ID, (i + 1), 25), 1);
	    UpdateParamFl(data[i].RadiusM, PackID(ID, (i + 1), 26), 1);
	    UpdateParamFl(data[i].PressureA, PackID(ID, (i + 1), 27), 1);
	}
	UpdateParamFlState(data[i].Value, data[i].State, PackID(ID, (i + 1), 1), 2);
    }
    NeedInit = false;
}

uint16_t B_ACCOUNT::Task(uint16_t uc)
{
    tagMsg Msg;
    uint16_t rc = 0;
    switch(uc) {
    case TaskRefresh:
	Msg.L = 5;
	Msg.C = AddrReq;
	*((uint16_t *) Msg.D) = PackID(ID, 0, 0); //состояние
	if(mPrm.owner().DoCmd(&Msg)) {
	    if(Msg.C == GOOD3) {
		if(with_params) {
		    for(int i = 1; i <= count_n; i++) {
			Msg.L = 49;
			Msg.C = AddrReq;
			*((uint16_t *) Msg.D) = PackID(ID, i, 1); //Расход(Q)
			*((uint16_t *) (Msg.D + 2)) = PackID(ID, i, 2); //Период вычисления
			*((uint16_t *) (Msg.D + 4)) = PackID(ID, i, 3); //Чувствительность
			*((uint16_t *) (Msg.D + 6)) = PackID(ID, i, 4); //min max предупредительный
			*((uint16_t *) (Msg.D + 8)) = PackID(ID, i, 5); //min max аварии Q
			*((uint16_t *) (Msg.D + 10)) = PackID(ID, i, 6); //Номера датчиков
			*((uint16_t *) (Msg.D + 12)) = PackID(ID, i, 7); //Расход при нештатных ситуациях
			*((uint16_t *) (Msg.D + 14)) = PackID(ID, i, 8); //Контрактный час
			*((uint16_t *) (Msg.D + 16)) = PackID(ID, i, 9); //Объем текущего часа
			*((uint16_t *) (Msg.D + 18)) = PackID(ID, i, 10); //Счетчик объема
			*((uint16_t *) (Msg.D + 20)) = PackID(ID, i, 11); //Средние Q, dP, t, P, E за предыдущий час
			*((uint16_t *) (Msg.D + 22)) = PackID(ID, i, 12); //Период
			*((uint16_t *) (Msg.D + 24)) = PackID(ID, i, 13); //Средние Q, dP, t, P, E за период
			*((uint16_t *) (Msg.D + 26)) = PackID(ID, i, 14); //Объем за период
			*((uint16_t *) (Msg.D + 28)) = PackID(ID, i, 15); //Плотность газа в нормальных условиях
			*((uint16_t *) (Msg.D + 30)) = PackID(ID, i, 16); //Эквивалентная шероховатость
			*((uint16_t *) (Msg.D + 32)) = PackID(ID, i, 17); //Молярная концентрация азота
			*((uint16_t *) (Msg.D + 34)) = PackID(ID, i, 18); //Молярная концентрация углекислого газа
			*((uint16_t *) (Msg.D + 36)) = PackID(ID, i, 19); //Диаметр диафрагмы
			*((uint16_t *) (Msg.D + 38)) = PackID(ID, i, 20); //Коэффициент теплового расширения диафрагмы
			*((uint16_t *) (Msg.D + 40)) = PackID(ID, i, 21); //Диаметр трубопровода
			*((uint16_t *) (Msg.D + 42)) = PackID(ID, i, 22); //Коэффициент теплового расширения трубопровода
			*((uint16_t *) (Msg.D + 44)) = PackID(ID, i, 23); //Метод измерения
			if(mPrm.owner().DoCmd(&Msg)) {
			    if(Msg.C == GOOD3) {
				Msg.L = 13;
				Msg.C = AddrReq;
				*((uint16_t *) (Msg.D)) = PackID(ID, i, 24); //Тестовые значения
				*((uint16_t *) (Msg.D + 2)) = PackID(ID, i, 25); //Тестовый расход
				*((uint16_t *) (Msg.D + 4)) = PackID(ID, i, 26); //Средний радиус закругления кромки диафрагмы
				*((uint16_t *) (Msg.D + 6)) = PackID(ID, i, 27); //Атмосферное давление
				*((uint16_t *) (Msg.D + 8)) = PackID(ID, i, 28); //dP, t, P, E*/
				if(mPrm.owner().DoCmd(&Msg)) {
				    if(Msg.C == GOOD3) {
					rc = 1;
				    }
				}

			    } else {
				rc = 0;
				break;
			    }
			} else {
			    rc = 0;
			    break;
			}

		    }
		} else {

		}
	    }
	}
	if(rc) NeedInit = false;
	break;
    }
    return rc;
}
uint16_t B_ACCOUNT::HandleEvent(int64_t tm, uint8_t * D)
{
    if((TSYS::getUnalign16(D) & 0xF000) != ID) return 0;
    uint16_t l = 0;
    uint16_t k = (TSYS::getUnalign16(D) >> 6) & 0x3F; // номер объекта
    uint16_t n = TSYS::getUnalign16(D) & 0x3F;  // номер параметра

    switch(k) {
    case 0:
	switch(n) {
	case 0:
	    mPrm.vlAt("state").at().setI(D[2], tm, true);
	    l = 3;
	    break;
	case 1:
	    mPrm.vlAt("state").at().setI(D[2], tm, true);
	    l = 3 + count_n * 5;
	    for(int j = 1; j <= count_n; j++) {
		mPrm.vlAt(TSYS::strMess("state_%d", j).c_str()).at().setI(D[(j - 1) * 5 + 3], tm, true);
		mPrm.vlAt(TSYS::strMess("expense_%d", j).c_str()).at().setR(TSYS::getUnalignFloat(D + (j - 1) * 5 + 4), tm, true);
	    }
	    break;
	case 2:
	    mPrm.vlAt("state").at().setI(D[2], tm, true);
	    l = 3 + count_n * 5;
	    for(int j = 1; j <= count_n; j++) {
		mPrm.vlAt(TSYS::strMess("state_%d", j).c_str()).at().setI(D[(j - 1) * 5 + 3], tm, true);
		mPrm.vlAt(TSYS::strMess("quantity_%d", j).c_str()).at().setI(D[(j - 1) * 5 + 3], tm, true);
	    }
	    break;

	}
	break;
    default:
	if(k && (k <= count_n)) {
	    switch(n) {
	    time_t t;
	case 0:
	    mPrm.vlAt(TSYS::strMess("state_%d", k).c_str()).at().setI(D[2], tm, true);
	    l = 3;
	    break;
	case 1:
	    mPrm.vlAt(TSYS::strMess("state_%d", k).c_str()).at().setI(D[2], tm, true);
	    mPrm.vlAt(TSYS::strMess("value_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);

	    l = 7;
	    break;
	case 2:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("period_%d", k).c_str()).at().setI(D[3], tm, true);
	    }
	    l = 4;
	    break;
	case 3:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("sensFV_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 4:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("minW_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("maxW_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), tm, true);
	    }
	    l = 11;
	    break;
	case 5:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("minA_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("maxA_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), tm, true);
	    }
	    l = 11;
	    break;
	case 6:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("sensors_%d", k).c_str()).at().setI(TSYS::getUnalign32(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 7:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("serviceQ_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 8:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("hour_%d", k).c_str()).at().setI(D[3], tm, true);
	    }
	    l = 4;
	    break;
	case 9:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("hourlyQ_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 10:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("counter_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 11:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("hourQ_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("hourdP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("hourT_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("hourP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("hourE_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 23;
	    break;
	case 12:
	    if(with_params) {

		string data_s;
		for(int i = 0; i < 10; i++) {
		    data_s += TSYS::int2str((uint8_t) D[i], TSYS::Hex) + " ";
		}
		//TODO
		//mPrm.vlAt(TSYS::strMess("period_date_%d", k).c_str()).at().setI(D[6], 0, true);
		D[6] = 0;
		D[7] = 0;
		t = mPrm.owner().DateTimeToTime_t(D + 3);
		//mPrm.vlAt(TSYS::strMess("dateT_%d", k).c_str()).at().setS(TSYS::time2str(t, "%d.%m.%Y %H:00:00"), 0, true);

	    }
	    l = 8;
	    break;
	case 13:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("avgQ_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("avgdP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("avgT_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("avgP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("avgE_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 23;
	    break;
	case 14:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("periodQ_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 15:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("density_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 16:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("asperity_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 17:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("concentrN_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 18:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("concentrCO_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 19:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("diameterM_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 20:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("factorM_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 21:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("diameterP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 22:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("factorP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 23:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("methodM_%d", k).c_str()).at().setI(D[3], tm, true);
	    }
	    l = 4;
	    break;
	case 24:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("testdP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("testT_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("testP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 15;
	    break;
	case 25:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("testQ_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 26:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("radiusM_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 27:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("pressureA_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 7;
	    break;
	case 28:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("dP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("T_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("P_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
		mPrm.vlAt(TSYS::strMess("E_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), tm, true);
	    }
	    l = 19;
	    break;
	    }
	}
	break;
    }
    return l;
}

uint8_t B_ACCOUNT::cmdGet(uint16_t prmID, uint8_t * out)
{
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
    int64_t tm;
    ui8fl V;
    if(ft3ID.k == 0) {
	switch(ft3ID.n) {
	case 0:
	    //state
	    out[0] = 0;
	    l = 1;
	    break;
	case 1:

	    out[0] = 0;
	    l = 1;
	    //value
	    for(uint8_t i = 0; i < count_n; i++) {
		out[i * 5 + 1] = data[i].State.vl;
		for(uint8_t j = 0; j < 4; j++)
		    out[i * 5 + 2 + j] = data[i].Value.b_vl[j];
		l += 5;
	    }
	    break;
	case 2:
	    out[0] = count_n;
	    l = 1;
	    break;
	}
    } else {
	if(ft3ID.k <= count_n) {
	    switch(ft3ID.n) {
	    case 0:
		out[0] = data[ft3ID.k - 1].State.vl;
		l = 1;
		break;
	    case 1:
		out[0] = data[ft3ID.k - 1].State.vl;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].Value.b_vl[j];
		l = 5;
		break;
	    case 2:
		out[0] = data[ft3ID.k - 1].Period.s;
		out[1] = data[ft3ID.k - 1].Period.vl;
		l = 2;
		break;
	    case 3:
		out[0] = data[ft3ID.k - 1].Sens.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].Sens.b_vl[j];
		l = 5;
		break;
	    case 4:
		out[0] = data[ft3ID.k - 1].MinW.s;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].MinW.b_vl[j];
		    out[5 + j] = data[ft3ID.k - 1].MaxW.b_vl[j];
		}
		l = 9;
		break;
	    case 5:
		out[0] = data[ft3ID.k - 1].MinA.s;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].MinA.b_vl[j];
		    out[5 + j] = data[ft3ID.k - 1].MaxA.b_vl[j];
		}
		l = 9;
		break;
	    case 6:
		out[0] = data[ft3ID.k - 1].Sensors.s;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].Sensors.b_vl[j];
		}
		l = 5;
		break;
	    case 7:
		out[0] = data[ft3ID.k - 1].ServiceQ.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].ServiceQ.b_vl[j];
		l = 5;
		break;
	    case 8:
		out[0] = data[ft3ID.k - 1].Hour.s;
		out[1] = data[ft3ID.k - 1].Hour.vl;
		l = 2;
		break;
	    case 9:
		out[0] = data[ft3ID.k - 1].HourlyQ.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].HourlyQ.b_vl[j];
		l = 5;
		break;
	    case 10:
		out[0] = data[ft3ID.k - 1].Counter.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].Counter.b_vl[j];
		l = 5;
		break;
	    case 11:
		//TODO get
		out[0] = data[ft3ID.k - 1].State.vl;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].HourQ.b_vl[j];
		    out[5 + j] = data[ft3ID.k - 1].HourdP.b_vl[j];
		    out[9 + j] = data[ft3ID.k - 1].HourT.b_vl[j];
		    out[13 + j] = data[ft3ID.k - 1].HourP.b_vl[j];
		    out[17 + j] = data[ft3ID.k - 1].HourE.b_vl[j];
		}
		l = 21;
		break;
	    case 12: //TODO period
		out[0] = data[ft3ID.k - 1].StartDate.s;
		mPrm.owner().Time_tToDateTime(out + 1, data[ft3ID.k - 1].StartDate.vl);
		ui8w hours;
		hours.w = (data[ft3ID.k - 1].EndDate.vl - data[ft3ID.k - 1].StartDate.vl) / 3600;
		l = 6;
		out[4] = hours.b[0];
		out[5] = hours.b[1];
		l = 6;
		break;
	    case 13:
		//TODO get
		out[0] = data[ft3ID.k - 1].State.vl;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].AvgQ.b_vl[j];
		    out[5 + j] = data[ft3ID.k - 1].AvgdP.b_vl[j];
		    out[9 + j] = data[ft3ID.k - 1].AvgT.b_vl[j];
		    out[13 + j] = data[ft3ID.k - 1].AvgP.b_vl[j];
		    out[17 + j] = data[ft3ID.k - 1].AvgE.b_vl[j];
		}
		l = 21;
		break;
	    case 14:
		//TODO get V period
		out[0] = data[ft3ID.k - 1].State.vl;
		tm = data[ft3ID.k - 1].EndDate.vl;
		tm *= 1000000;
		V.f = data[ft3ID.k - 1].Counter.lnk.aprm.at().getR(&tm, true);
		tm = data[ft3ID.k - 1].StartDate.vl;
		tm *= 1000000;
		V.f -= data[ft3ID.k - 1].Counter.lnk.aprm.at().getR(&tm, true);
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = V.b[j];
		l = 5;
		break;
	    case 15:
		out[0] = data[ft3ID.k - 1].Density.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].Density.b_vl[j];
		l = 5;
		break;
	    case 16:
		out[0] = data[ft3ID.k - 1].Asperity.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].Asperity.b_vl[j];
		l = 5;
		break;
	    case 17:
		out[0] = data[ft3ID.k - 1].ConcentrN.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].ConcentrN.b_vl[j];
		l = 5;
		break;
	    case 18:
		out[0] = data[ft3ID.k - 1].ConcentrCO.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].ConcentrCO.b_vl[j];
		l = 5;
		break;
	    case 19:
		out[0] = data[ft3ID.k - 1].DiameterM.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].DiameterM.b_vl[j];
		l = 5;
		break;
	    case 20:
		out[0] = data[ft3ID.k - 1].FactorM.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].FactorM.b_vl[j];
		l = 5;
		break;
	    case 21:
		out[0] = data[ft3ID.k - 1].DiameterP.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].DiameterP.b_vl[j];
		l = 5;
		break;
	    case 22:
		out[0] = data[ft3ID.k - 1].FactorP.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].FactorP.b_vl[j];
		l = 5;
		break;
	    case 23:
		out[0] = data[ft3ID.k - 1].MethodM.s;
		out[1] = data[ft3ID.k - 1].MethodM.vl;
		l = 2;
		break;
	    case 24:
		//TODO get test
		out[0] = data[ft3ID.k - 1].State.vl;
		for(uint8_t j = 0; j < 4; j++) {
		    out[1 + j] = data[ft3ID.k - 1].TestdP.b_vl[j];
		    out[5 + j] = data[ft3ID.k - 1].TestT.b_vl[j];
		    out[9 + j] = data[ft3ID.k - 1].TestP.b_vl[j];
		}
		l = 13;
		break;
	    case 25:
		//out[0] = data[ft3ID.k - 1].State.vl;
		for(uint8_t j = 0; j < 4; j++)
		    out[j] = data[ft3ID.k - 1].TestQ.b_vl[j];
		l = 4;
		break;
	    case 26:
		out[0] = data[ft3ID.k - 1].RadiusM.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].RadiusM.b_vl[j];
		l = 5;
		break;
	    case 27:
		out[0] = data[ft3ID.k - 1].PressureA.s;
		for(uint8_t j = 0; j < 4; j++)
		    out[1 + j] = data[ft3ID.k - 1].PressureA.b_vl[j];
		l = 5;
		break;
	    case 28:
		//TODO get
		for(uint8_t j = 0; j < 4; j++) {
		    V.f = data[ft3ID.k - 1].dP.Get();
		    for(uint8_t j = 0; j < 4; j++) {
			out[0 + j] = V.b[j];
		    }
		    V.f = data[ft3ID.k - 1].T.Get();
		    for(uint8_t j = 0; j < 4; j++) {
			out[4 + j] = V.b[j];
		    }
		    V.f = data[ft3ID.k - 1].P.Get();
		    for(uint8_t j = 0; j < 4; j++) {
			out[8 + j] = V.b[j];
		    }
		    V.f = data[ft3ID.k - 1].E.Get();
		    for(uint8_t j = 0; j < 4; j++) {
			out[12 + j] = V.b[j];
		    }
		}
		l = 16;
		break;

	    }

	}
    }
    return l;
}

uint8_t B_ACCOUNT::cmdSet(uint8_t * req, uint8_t addr)
{
    uint16_t prmID = TSYS::getUnalign16(req);
    FT3ID ft3ID = UnpackID(prmID);
    if(ft3ID.g != ID) return 0;
    uint l = 0;
//    mess_info(mPrm.nodePath().c_str(), "cmdSet k %d n %d", ft3ID.k, ft3ID.n);
    if((ft3ID.k > 0) && (ft3ID.k <= count_n)) {
	switch(ft3ID.n) {
	case 2:
	    l = SetNew8Val(data[ft3ID.k - 1].Period, addr, prmID, req[2]);
	    break;
	case 3:
	    l = SetNewflVal(data[ft3ID.k - 1].Sens, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 4:
	    l = SetNew2flVal(data[ft3ID.k - 1].MinW, data[ft3ID.k - 1].MaxW, addr, prmID, TSYS::getUnalignFloat(req + 2), TSYS::getUnalignFloat(req + 6));
	    break;
	case 5:
	    l = SetNew2flVal(data[ft3ID.k - 1].MinA, data[ft3ID.k - 1].MaxA, addr, prmID, TSYS::getUnalignFloat(req + 2), TSYS::getUnalignFloat(req + 6));
	    break;
	case 6:
	    l = SetNew32Val(data[ft3ID.k - 1].Sensors, addr, prmID, TSYS::getUnalign32(req + 2));
	    break;
	case 7:
	    l = SetNewflVal(data[ft3ID.k - 1].ServiceQ, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 8:
	    l = SetNew8Val(data[ft3ID.k - 1].Hour, addr, prmID, req[2]);
	    break;
	case 10:
	    l = SetNewflVal(data[ft3ID.k - 1].Counter, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 12:
	    l = data[ft3ID.k - 1].SetNewPeriod(addr, prmID, req + 2);
	    break;
	case 15:
	    l = SetNewflVal(data[ft3ID.k - 1].Density, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 16:
	    l = SetNewflVal(data[ft3ID.k - 1].Asperity, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 17:
	    l = SetNewflVal(data[ft3ID.k - 1].ConcentrN, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 18:
	    l = SetNewflVal(data[ft3ID.k - 1].ConcentrCO, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 19:
	    l = SetNewflVal(data[ft3ID.k - 1].DiameterM, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 20:
	    l = SetNewflVal(data[ft3ID.k - 1].FactorM, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 21:
	    l = SetNewflVal(data[ft3ID.k - 1].DiameterP, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 22:
	    l = SetNewflVal(data[ft3ID.k - 1].FactorP, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 23:
	    l = SetNew8Val(data[ft3ID.k - 1].MethodM, addr, prmID, req[2]);
	    break;
	case 24:
	    l = 12 + 2; //TODO test values
	    break;
	case 26:
	    l = SetNewflVal(data[ft3ID.k - 1].RadiusM, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;
	case 27:
	    l = SetNewflVal(data[ft3ID.k - 1].PressureA, addr, prmID, TSYS::getUnalignFloat(req + 2));
	    break;

	}
    }
    return l;
}

uint16_t B_ACCOUNT::setVal(TVal &val)
{
    int off = 0;
    FT3ID ft3ID;
    ft3ID.k = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0);
    ft3ID.n = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0);
    ft3ID.g = ID;

    tagMsg Msg;
    Msg.L = 0;
    Msg.C = SetData;
    *((uint16_t *) Msg.D) = PackID(ft3ID);
    switch(ft3ID.n) {
    case 2:
    case 8:
    case 23:
	Msg.L = 6;
	Msg.D[2] = val.get(NULL, true).getI();
	break;
    case 3:
    case 7:
    case 9:
    case 10:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 25:
    case 26:
    case 27:
	Msg.L = 9;
	*(float *) (Msg.D + 2) = (float) val.get(NULL, true).getR();
	break;
    case 6:
	Msg.L = 9;
	//TODO
	Msg.D[2] = (uint8_t) mPrm.vlAt(TSYS::strMess("sensor1_%d", ft3ID.k).c_str()).at().getI(0, true);
	Msg.D[3] = (uint8_t) mPrm.vlAt(TSYS::strMess("sensor2_%d", ft3ID.k).c_str()).at().getI(0, true);
	Msg.D[4] = (uint8_t) mPrm.vlAt(TSYS::strMess("sensorT_%d", ft3ID.k).c_str()).at().getI(0, true);
	Msg.D[5] = (uint8_t) mPrm.vlAt(TSYS::strMess("sensorP_%d", ft3ID.k).c_str()).at().getI(0, true);
	break;
    case 4:
	Msg.L = 13;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minW_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxW_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    case 5:
	Msg.L = 13;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minA_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxA_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    case 24:
	Msg.L = 17;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("testdP_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("testT_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 10) = (float) mPrm.vlAt(TSYS::strMess("testP_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    case 28:
	Msg.L = 21;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("dP_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("T_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 10) = (float) mPrm.vlAt(TSYS::strMess("P_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 14) = (float) mPrm.vlAt(TSYS::strMess("E_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    case 11:
	Msg.L = 25;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("hourQ_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("hourdP_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 10) = (float) mPrm.vlAt(TSYS::strMess("hourT_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 14) = (float) mPrm.vlAt(TSYS::strMess("hourP_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 18) = (float) mPrm.vlAt(TSYS::strMess("hourE_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    case 13:
	Msg.L = 25;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("avgQ_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("avgP_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 10) = (float) mPrm.vlAt(TSYS::strMess("avgT_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 14) = (float) mPrm.vlAt(TSYS::strMess("avgP_%d", ft3ID.k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 18) = (float) mPrm.vlAt(TSYS::strMess("avgE_%d", ft3ID.k).c_str()).at().getR(0, true);
	break;
    case 12:
	struct tm tm_tm;
	//TODO
	strptime(mPrm.vlAt(TSYS::strMess("dateT_%d", ft3ID.k).c_str()).at().getS().c_str(), "%d.%m.%Y %H:%M:%S", &tm_tm);
	Msg.L = 10;
	uint16_t h = (uint16_t) mPrm.vlAt(TSYS::strMess("period_date_%d", ft3ID.k).c_str()).at().getI(0, true);
	mPrm.owner().Time_tToDateTime(Msg.D + 2, mktime(&tm_tm));
	Msg.D[5] = h & 0xFF;
	Msg.D[6] = (h >> 8) & 0xFF;
	string data_s;
	for(int i = 0; i < 7; i++) {
	    data_s += TSYS::int2str((uint8_t) Msg.D[i], TSYS::Hex) + " ";
	}
	break;

    }
    if(Msg.L) mPrm.owner().DoCmd(&Msg);
    return 0;
}

//---------------------------------------------------------------------------
