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

B_ACCOUNT::B_ACCOUNT(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params) :
	DA(prm), ID(id << 12), count_n(n), with_params(has_params) //, numReg(0)

{
    TFld * fld;
    mPrm.p_el.fldAdd(fld = new TFld("state", _("State"), TFld::Integer, TFld::NoWrite));
    fld->setReserve("0:0");

    for(int i = 1; i <= count_n; i++) {
	mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("state_%d", i).c_str(), TSYS::strMess(_("State %d"), i).c_str(), TFld::Integer, TFld::NoWrite));
	fld->setReserve(TSYS::strMess("%d:0", i));
	if(with_params) {
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("value_%d", i).c_str(), TSYS::strMess(_("Current flow %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:1", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("period_%d", i).c_str(), TSYS::strMess(_("Measure period %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:2", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("sens_%d", i).c_str(), TSYS::strMess(_("Sensitivity %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:3", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("minW_%d", i).c_str(), TSYS::strMess(_("Warning minimum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:4", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("maxW_%d", i).c_str(), TSYS::strMess(_("Warning maximum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:4", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("minA_%d", i).c_str(), TSYS::strMess(_("Alarm minimum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:5", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("maxA_%d", i).c_str(), TSYS::strMess(_("Alarm maximum %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:5", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("sensor1_%d", i).c_str(), TSYS::strMess(_("Sensor 1 %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:6", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("sensor2_%d", i).c_str(), TSYS::strMess(_("Sensor 2 %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:6", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("sensorT_%d", i).c_str(), TSYS::strMess(_("Sensor T %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:6", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("sensorP_%d", i).c_str(), TSYS::strMess(_("Sensor P %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:6", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("serviceQ_%d", i).c_str(), TSYS::strMess(_("Service flow %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:7", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("hour_%d", i).c_str(), TSYS::strMess(_("Contract hour %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:8", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("hourlyQ_%d", i).c_str(), TSYS::strMess(_("Hourly flow %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:9", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("counter_%d", i).c_str(), TSYS::strMess(_("Counter Q %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:10", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("hourQ_%d", i).c_str(), TSYS::strMess(_("Hour Q %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:11", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("hourdP_%d", i).c_str(), TSYS::strMess(_("Hour dP %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:11", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("hourt_%d", i).c_str(), TSYS::strMess(_("Hour t %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:11", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("hourP_%d", i).c_str(), TSYS::strMess(_("Hour P %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:11", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("hourE_%d", i).c_str(), TSYS::strMess(_("Hour E %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:11", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("datet_%d", i).c_str(), TSYS::strMess(_("Datetime %d"), i).c_str(), TFld::String, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:12", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("period_date_%d", i).c_str(), TSYS::strMess(_("Period %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:12", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("avgQ_%d", i).c_str(), TSYS::strMess(_("Avg Q %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:13", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("avgdP_%d", i).c_str(), TSYS::strMess(_("Avg dP %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:13", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("avgt_%d", i).c_str(), TSYS::strMess(_("Avg t %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:13", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("avgP_%d", i).c_str(), TSYS::strMess(_("Avg P %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:13", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("avgE_%d", i).c_str(), TSYS::strMess(_("Avg E %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:13", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("periodQ_%d", i).c_str(), TSYS::strMess(_("Periodicly Q %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:14", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("density_%d", i).c_str(), TSYS::strMess(_("Density %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:15", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("asperity_%d", i).c_str(), TSYS::strMess(_("Equivalence asperity %d"), i).c_str(), TFld::Real,
			    TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:16", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("concentrN_%d", i).c_str(), TSYS::strMess(_("N concentration %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:17", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("concentrCO_%d", i).c_str(), TSYS::strMess(_("CO concentration %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:18", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("diameterM_%d", i).c_str(), TSYS::strMess(_("Membrane diameter %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:19", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("factorM_%d", i).c_str(), TSYS::strMess(_("Membrane factor %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:20", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("diameterP_%d", i).c_str(), TSYS::strMess(_("Pipe diameter %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:21", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("factorC_%d", i).c_str(), TSYS::strMess(_("Pipe factor %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:22", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("methodM_%d", i).c_str(), TSYS::strMess(_("Measure method %d"), i).c_str(), TFld::Integer, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:23", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("test_%d", i).c_str(), TSYS::strMess(_("Test dP/TI %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:24", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("testt_%d", i).c_str(), TSYS::strMess(_("Test t %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:24", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("testP_%d", i).c_str(), TSYS::strMess(_("Test P %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:24", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("testR_%d", i).c_str(), TSYS::strMess(_("Test rate %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:25", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("radius_%d", i).c_str(), TSYS::strMess(_("Membrane radius %d"), i).c_str(), TFld::Real, TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:26", i));
	    mPrm.p_el.fldAdd(
		    fld = new TFld(TSYS::strMess("pressure_%d", i).c_str(), TSYS::strMess(_("Atmospheric pressure %d"), i).c_str(), TFld::Real,
			    TVal::DirWrite));
	    fld->setReserve(TSYS::strMess("%d:27", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("dP_%d", i).c_str(), TSYS::strMess(_("dP %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:28", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("t_%d", i).c_str(), TSYS::strMess(_("t %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:28", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("P_%d", i).c_str(), TSYS::strMess(_("P %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:28", i));
	    mPrm.p_el.fldAdd(fld = new TFld(TSYS::strMess("E_%d", i).c_str(), TSYS::strMess(_("E %d"), i).c_str(), TFld::Real, TFld::NoWrite));
	    fld->setReserve(TSYS::strMess("%d:28", i));

	}
    }

}

B_ACCOUNT::~B_ACCOUNT()
{

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

uint16_t B_ACCOUNT::Task(uint16_t uc)
{
    tagMsg Msg;
    uint16_t rc = 0;
    switch(uc) {
    case TaskRefresh:
	Msg.L = 5;
	Msg.C = AddrReq;
	*((uint16_t *) Msg.D) = ID | (0 << 6) | (0); //состояние
	if(mPrm.owner().Transact(&Msg)) {
	    if(Msg.C == GOOD3) {
		mPrm.vlAt("state").at().setI(Msg.D[7], 0, true);
		if(with_params) {
		    for(int i = 1; i <= count_n; i++) {
			Msg.L = 49;
			Msg.C = AddrReq;
			*((uint16_t *) Msg.D) = ID | (i << 6) | (1); //Расход(Q)
			*((uint16_t *) (Msg.D + 2)) = ID | (i << 6) | (2); //Период вычисления
			*((uint16_t *) (Msg.D + 4)) = ID | (i << 6) | (3); //Чувствительность
			*((uint16_t *) (Msg.D + 6)) = ID | (i << 6) | (4); //min max предупредительный
			*((uint16_t *) (Msg.D + 8)) = ID | (i << 6) | (5); //min max аварии Q
			*((uint16_t *) (Msg.D + 10)) = ID | (i << 6) | (6); //Номера датчиков
			*((uint16_t *) (Msg.D + 12)) = ID | (i << 6) | (7); //Расход при нештатных ситуациях
			*((uint16_t *) (Msg.D + 14)) = ID | (i << 6) | (8); //Контрактный час
			*((uint16_t *) (Msg.D + 16)) = ID | (i << 6) | (9); //Объем текущего часа
			*((uint16_t *) (Msg.D + 18)) = ID | (i << 6) | (10); //Счетчик объема
			*((uint16_t *) (Msg.D + 20)) = ID | (i << 6) | (11); //Средние Q, dP, t, P, E за предыдущий час
			*((uint16_t *) (Msg.D + 22)) = ID | (i << 6) | (12); //Период
			*((uint16_t *) (Msg.D + 24)) = ID | (i << 6) | (13); //Средние Q, dP, t, P, E за период
			*((uint16_t *) (Msg.D + 26)) = ID | (i << 6) | (14); //Объем за период
			*((uint16_t *) (Msg.D + 28)) = ID | (i << 6) | (15); //Плотность газа в нормальных условиях
			*((uint16_t *) (Msg.D + 30)) = ID | (i << 6) | (16); //Эквивалентная шероховатость
			*((uint16_t *) (Msg.D + 32)) = ID | (i << 6) | (17); //Молярная концентрация азота
			*((uint16_t *) (Msg.D + 34)) = ID | (i << 6) | (18); //Молярная концентрация углекислого газа
			*((uint16_t *) (Msg.D + 36)) = ID | (i << 6) | (19); //Диаметр диафрагмы
			*((uint16_t *) (Msg.D + 38)) = ID | (i << 6) | (20); //Коэффициент теплового расширения диафрагмы
			*((uint16_t *) (Msg.D + 40)) = ID | (i << 6) | (21); //Диаметр трубопровода
			*((uint16_t *) (Msg.D + 42)) = ID | (i << 6) | (22); //Коэффициент теплового расширения трубопровода
			*((uint16_t *) (Msg.D + 44)) = ID | (i << 6) | (23); //Метод измерения
			if(mPrm.owner().Transact(&Msg)) {
			    if(Msg.C == GOOD3) {
				mPrm.vlAt(TSYS::strMess("state_%d", i).c_str()).at().setI(Msg.D[7], 0, true);
				mPrm.vlAt(TSYS::strMess("value_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 8), 0, true);
				mPrm.vlAt(TSYS::strMess("period_%d", i).c_str()).at().setI(Msg.D[17], 0, true);
				mPrm.vlAt(TSYS::strMess("sens_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 23), 0, true);
				mPrm.vlAt(TSYS::strMess("minW_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 32), 0, true);
				mPrm.vlAt(TSYS::strMess("maxW_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 36), 0, true);
				mPrm.vlAt(TSYS::strMess("minA_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 45), 0, true);
				mPrm.vlAt(TSYS::strMess("maxA_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 49), 0, true);
				mPrm.vlAt(TSYS::strMess("sensor1_%d", i).c_str()).at().setI(Msg.D[58], 0, true);
				mPrm.vlAt(TSYS::strMess("sensor2_%d", i).c_str()).at().setI(Msg.D[59], 0, true);
				mPrm.vlAt(TSYS::strMess("sensorT_%d", i).c_str()).at().setI(Msg.D[60], 0, true);
				mPrm.vlAt(TSYS::strMess("sensorP_%d", i).c_str()).at().setI(Msg.D[61], 0, true);
				mPrm.vlAt(TSYS::strMess("serviceQ_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 67), 0, true);
				mPrm.vlAt(TSYS::strMess("hour_%d", i).c_str()).at().setI(Msg.D[76], 0, true);
				mPrm.vlAt(TSYS::strMess("hourlyQ_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 82), 0, true);
				mPrm.vlAt(TSYS::strMess("counter_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 91), 0, true);
				mPrm.vlAt(TSYS::strMess("hourQ_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 100), 0, true);
				mPrm.vlAt(TSYS::strMess("hourdP_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 104), 0, true);
				mPrm.vlAt(TSYS::strMess("hourt_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 108), 0, true);
				mPrm.vlAt(TSYS::strMess("hourP_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 112), 0, true);
				mPrm.vlAt(TSYS::strMess("hourE_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 116), 0, true);
				mPrm.vlAt(TSYS::strMess("period_date_%d", i).c_str()).at().setI(TSYS::getUnalign16(Msg.D + 128), 0, true);
				time_t t = mPrm.owner().DateTimeToTime_t(Msg.D + 125);
				mPrm.vlAt(TSYS::strMess("datet_%d", i).c_str()).at().setS(TSYS::time2str(t, "%d.%m.%Y %H:%M:%S"), 0, true);
				string data_s;
				mPrm.vlAt(TSYS::strMess("avgQ_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 135), 0, true);
				mPrm.vlAt(TSYS::strMess("avgdP_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 139), 0, true);
				mPrm.vlAt(TSYS::strMess("avgt_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 143), 0, true);
				mPrm.vlAt(TSYS::strMess("avgP_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 147), 0, true);
				mPrm.vlAt(TSYS::strMess("avgE_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 151), 0, true);
				mPrm.vlAt(TSYS::strMess("periodQ_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 160), 0, true);
				mPrm.vlAt(TSYS::strMess("density_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 169), 0, true);
				mPrm.vlAt(TSYS::strMess("asperity_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 178), 0, true);
				mPrm.vlAt(TSYS::strMess("concentrN_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 187), 0, true);
				mPrm.vlAt(TSYS::strMess("concentrCO_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 196), 0, true);
				mPrm.vlAt(TSYS::strMess("diameterM_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 205), 0, true);
				mPrm.vlAt(TSYS::strMess("factorM_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 214), 0, true);
				mPrm.vlAt(TSYS::strMess("diameterP_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 223), 0, true);
				mPrm.vlAt(TSYS::strMess("factorC_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 232), 0, true);
				mPrm.vlAt(TSYS::strMess("methodM_%d", i).c_str()).at().setI(Msg.D[241], 0, true);
				Msg.L = 13;
				Msg.C = AddrReq;
				*((uint16_t *) (Msg.D)) = ID | (i << 6) | (24); //Тестовые значения
				*((uint16_t *) (Msg.D + 2)) = ID | (i << 6) | (25); //Тестовый расход
				*((uint16_t *) (Msg.D + 4)) = ID | (i << 6) | (26); //Средний радиус закругления кромки диафрагмы
				*((uint16_t *) (Msg.D + 6)) = ID | (i << 6) | (27); //Атмосферное давление
				*((uint16_t *) (Msg.D + 8)) = ID | (i << 6) | (28); //dP, t, P, E*/
				if(mPrm.owner().Transact(&Msg)) {
				    if(Msg.C == GOOD3) {
					mPrm.vlAt(TSYS::strMess("test_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 8), 0, true);
					mPrm.vlAt(TSYS::strMess("testt_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 12), 0, true);
					mPrm.vlAt(TSYS::strMess("testP_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 16), 0, true);
					mPrm.vlAt(TSYS::strMess("testR_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 24), 0, true);
					mPrm.vlAt(TSYS::strMess("radius_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 33), 0, true);
					mPrm.vlAt(TSYS::strMess("pressure_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 42), 0, true);
					mPrm.vlAt(TSYS::strMess("dP_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 50), 0, true);
					mPrm.vlAt(TSYS::strMess("t_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 54), 0, true);
					mPrm.vlAt(TSYS::strMess("P_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 58), 0, true);
					mPrm.vlAt(TSYS::strMess("E_%d", i).c_str()).at().setR(TSYS::getUnalignFloat(Msg.D + 62), 0, true);
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
uint16_t B_ACCOUNT::HandleEvent(uint8_t * D)
{
    if((TSYS::getUnalign16(D) & 0xF000) != ID) return 0;
    uint16_t l = 0;
    uint16_t k = (TSYS::getUnalign16(D) >> 6) & 0x3F; // номер объекта
    uint16_t n = TSYS::getUnalign16(D) & 0x3F;  // номер параметра

    switch(k) {
    case 0:
	switch(n) {
	case 0:
	    mPrm.vlAt("state").at().setI(D[2], 0, true);
	    l = 3;
	    break;
	case 1:
	    mPrm.vlAt("state").at().setI(D[2], 0, true);
	    l = 3 + count_n * 5;
	    for(int j = 1; j <= count_n; j++) {
		mPrm.vlAt(TSYS::strMess("state_%d", j).c_str()).at().setI(D[(j - 1) * 5 + 3], 0, true);
		mPrm.vlAt(TSYS::strMess("expense_%d", j).c_str()).at().setR(TSYS::getUnalignFloat(D + (j - 1) * 5 + 4), 0, true);
	    }
	    break;
	case 2:
	    mPrm.vlAt("state").at().setI(D[2], 0, true);
	    l = 3 + count_n * 5;
	    for(int j = 1; j <= count_n; j++) {
		mPrm.vlAt(TSYS::strMess("state_%d", j).c_str()).at().setI(D[(j - 1) * 5 + 3], 0, true);
		mPrm.vlAt(TSYS::strMess("quantity_%d", j).c_str()).at().setI(D[(j - 1) * 5 + 3], 0, true);
	    }
	    break;

	}
	break;
    default:
	if(k && (k <= count_n)) {
	    switch(n) {
	    time_t t;
	case 0:
	    mPrm.vlAt(TSYS::strMess("state_%d", k).c_str()).at().setI(D[2], 0, true);
	    l = 3;
	    break;
	case 1:
	    mPrm.vlAt(TSYS::strMess("state_%d", k).c_str()).at().setI(D[2], 0, true);
	    mPrm.vlAt(TSYS::strMess("value_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);

	    l = 7;
	    break;
	case 2:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("period_%d", k).c_str()).at().setI(D[3], 0, true);
	    }
	    l = 4;
	    break;
	case 3:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("sensFV_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 4:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("minW_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("maxW_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), 0, true);
	    }
	    l = 11;
	    break;
	case 5:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("minA_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("maxA_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 7), 0, true);
	    }
	    l = 11;
	    break;
	case 6:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("sensor1_%d", k).c_str()).at().setI(D[3], 0, true);
		mPrm.vlAt(TSYS::strMess("sensor2_%d", k).c_str()).at().setI(D[3], 0, true);
		mPrm.vlAt(TSYS::strMess("sensorT_%d", k).c_str()).at().setI(D[3], 0, true);
		mPrm.vlAt(TSYS::strMess("sensorP_%d", k).c_str()).at().setI(D[3], 0, true);
	    }
	    l = 7;
	    break;
	case 7:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("serviceQ_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 8:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("hour_%d", k).c_str()).at().setI(D[3], 0, true);
	    }
	    l = 4;
	    break;
	case 9:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("hourlyQ_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 10:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("counter_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 11:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("hourQ_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("hourdP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("hourt_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("hourP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("hourE_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 23;
	    break;
	case 12:
	    if(with_params) {

		string data_s;
		for(int i = 0; i < 10; i++) {
		    data_s += TSYS::int2str((uint8_t) D[i], TSYS::Hex) + " ";
		}
		mPrm.vlAt(TSYS::strMess("period_date_%d", k).c_str()).at().setI(D[6], 0, true);
		D[6] = 0;
		D[7] = 0;
		t = mPrm.owner().DateTimeToTime_t(D + 3);
		mPrm.vlAt(TSYS::strMess("datet_%d", k).c_str()).at().setS(TSYS::time2str(t, "%d.%m.%Y %H:00:00"), 0, true);

	    }
	    l = 8;
	    break;
	case 13:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("avgQ_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("avgdP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("avgt_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("avgP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("avgE_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 23;
	    break;
	case 14:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("periodQ_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 15:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("density_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 16:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("asperity_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 17:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("concentrN_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 18:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("concentrCO_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 19:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("diameterM_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 20:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("factorM_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 21:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("diameterP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 22:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("factorC_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 23:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("methodM_%d", k).c_str()).at().setI(D[3], 0, true);
	    }
	    l = 4;
	    break;
	case 24:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("test_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("testt_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("testP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 15;
	    break;
	case 25:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("testR_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 26:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("radius_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 27:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("pressure_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 7;
	    break;
	case 28:
	    if(with_params) {
		mPrm.vlAt(TSYS::strMess("dP_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("t_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("P_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
		mPrm.vlAt(TSYS::strMess("E_%d", k).c_str()).at().setR(TSYS::getUnalignFloat(D + 3), 0, true);
	    }
	    l = 19;
	    break;
	    }
	}
	break;
    }
    return l;
}

uint16_t B_ACCOUNT::setVal(TVal &val)
{
    int off = 0;
    uint16_t k = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер объекта
    uint16_t n = strtol((TSYS::strParse(val.fld().reserve(), 0, ":", &off)).c_str(), NULL, 0); // номер параметра
    uint16_t addr = ID | (k << 6) | n;

    tagMsg Msg;
    switch(n) {
    case 2:
    case 8:
    case 23:
	Msg.L = 6;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	Msg.D[2] = val.get(NULL, true).getI();
	mPrm.owner().Transact(&Msg);
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
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) val.get(NULL, true).getR();
	mPrm.owner().Transact(&Msg);
	break;
    case 6:
	Msg.L = 9;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	Msg.D[2] = (uint8_t) mPrm.vlAt(TSYS::strMess("sensor1_%d", k).c_str()).at().getI(0, true);
	Msg.D[3] = (uint8_t) mPrm.vlAt(TSYS::strMess("sensor2_%d", k).c_str()).at().getI(0, true);
	Msg.D[4] = (uint8_t) mPrm.vlAt(TSYS::strMess("sensorT_%d", k).c_str()).at().getI(0, true);
	Msg.D[5] = (uint8_t) mPrm.vlAt(TSYS::strMess("sensorP_%d", k).c_str()).at().getI(0, true);
	mPrm.owner().Transact(&Msg);
	break;
    case 4:
	Msg.L = 13;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minW_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxW_%d", k).c_str()).at().getR(0, true);
	mPrm.owner().Transact(&Msg);
	break;
    case 5:
	Msg.L = 13;
	Msg.C = SetData;
	*(float *) (Msg.D + 14) = (float) mPrm.vlAt(TSYS::strMess("hourP_%d", k).c_str()).at().getR(0, true);
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("minA_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("maxA_%d", k).c_str()).at().getR(0, true);
	mPrm.owner().Transact(&Msg);
	break;
    case 24:
	Msg.L = 17;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("test_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("testt_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 10) = (float) mPrm.vlAt(TSYS::strMess("testP_%d", k).c_str()).at().getR(0, true);
	mPrm.owner().Transact(&Msg);
	break;
    case 28:
	Msg.L = 21;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("dP_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("t_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 10) = (float) mPrm.vlAt(TSYS::strMess("P_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 14) = (float) mPrm.vlAt(TSYS::strMess("E_%d", k).c_str()).at().getR(0, true);
	mPrm.owner().Transact(&Msg);
	break;
    case 11:
	Msg.L = 25;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("hourQ_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("hourdP_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 10) = (float) mPrm.vlAt(TSYS::strMess("hourt_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 14) = (float) mPrm.vlAt(TSYS::strMess("hourP_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 18) = (float) mPrm.vlAt(TSYS::strMess("hourE_%d", k).c_str()).at().getR(0, true);
	mPrm.owner().Transact(&Msg);
	break;
    case 13:
	Msg.L = 25;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	*(float *) (Msg.D + 2) = (float) mPrm.vlAt(TSYS::strMess("avgQ_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 6) = (float) mPrm.vlAt(TSYS::strMess("avgP_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 10) = (float) mPrm.vlAt(TSYS::strMess("avgt_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 14) = (float) mPrm.vlAt(TSYS::strMess("avgP_%d", k).c_str()).at().getR(0, true);
	*(float *) (Msg.D + 18) = (float) mPrm.vlAt(TSYS::strMess("avgE_%d", k).c_str()).at().getR(0, true);
	mPrm.owner().Transact(&Msg);
	break;
    case 12:
	struct tm tm_tm;
	strptime(mPrm.vlAt(TSYS::strMess("datet_%d", k).c_str()).at().getS().c_str(), "%d.%m.%Y %H:%M:%S", &tm_tm);
	Msg.L = 10;
	Msg.C = SetData;
	Msg.D[0] = addr & 0xFF;
	Msg.D[1] = (addr >> 8) & 0xFF;
	uint16_t h = (uint16_t) mPrm.vlAt(TSYS::strMess("period_date_%d", k).c_str()).at().getI(0, true);
	mPrm.owner().Time_tToDateTime(Msg.D + 2, mktime(&tm_tm));
	Msg.D[5] = h & 0xFF;
	Msg.D[6] = (h >> 8) & 0xFF;
	mPrm.owner().Transact(&Msg);
	string data_s;
	for(int i = 0; i < 7; i++) {
	    data_s += TSYS::int2str((uint8_t) Msg.D[i], TSYS::Hex) + " ";
	}
	break;

    }

    return 0;
}

//---------------------------------------------------------------------------
