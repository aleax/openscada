
//OpenSCADA module DAQ.GPIO file: da_SUNXI.cpp
/***************************************************************************
 *   Copyright (C) 2018 by Roman Savochenko, <rom_as@oscada.org>           *
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

#include <fcntl.h>

#include <tsys.h>

#include <gpio_lib.h>

#include "GPIO_module.h"
#include "da_SUNXI.h"

using namespace ModGPIO;

//*************************************************
//* SUNXI					  *
//*************************************************
SUNXI::SUNXI( ) : TTypeParam("SUNXI", _("SUNXI (ALLWINNER H2 and newer)")), use(false)
{
    fldAdd(new TFld("MOD_PRMS",_("Module addition parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"100000"));
}

void SUNXI::create( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;

    //GPIO Bank A
    for(int iGP = 0; iGP < SUNXI_GPIO_A_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPA(iGP)).c_str(),(iGP==0)?"GPIO PA":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //GPIO Bank B
    for(int iGP = 0; iGP < SUNXI_GPIO_B_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPB(iGP)).c_str(),(iGP==0)?"GPIO PB":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //GPIO Bank C
    for(int iGP = 0; iGP < SUNXI_GPIO_C_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPC(iGP)).c_str(),(iGP==0)?"GPIO PC":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //GPIO Bank D
    for(int iGP = 0; iGP < SUNXI_GPIO_D_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPD(iGP)).c_str(),(iGP==0)?"GPIO PD":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //GPIO Bank E
    for(int iGP = 0; iGP < SUNXI_GPIO_E_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPE(iGP)).c_str(),(iGP==0)?"GPIO PE":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //GPIO Bank F
    for(int iGP = 0; iGP < SUNXI_GPIO_F_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPF(iGP)).c_str(),(iGP==0)?"GPIO PF":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //GPIO Bank G
    for(int iGP = 0; iGP < SUNXI_GPIO_G_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPG(iGP)).c_str(),(iGP==0)?"GPIO PG":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));

    //GPIO Bank J
    for(int iGP = 0; iGP < SUNXI_GPIO_J_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPJ(iGP)).c_str(),(iGP==0)?"GPIO PJ":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //GPIO Bank K
    for(int iGP = 0; iGP < SUNXI_GPIO_K_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPK(iGP)).c_str(),(iGP==0)?"GPIO PK":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //GPIO Bank L
    for(int iGP = 0; iGP < SUNXI_GPIO_L_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPL(iGP)).c_str(),(iGP==0)?"GPIO PL":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //GPIO Bank M
    for(int iGP = 0; iGP < SUNXI_GPIO_M_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPM(iGP)).c_str(),(iGP==0)?"GPIO PM":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //GPIO Bank N
    for(int iGP = 0; iGP < SUNXI_GPIO_N_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPN(iGP)).c_str(),(iGP==0)?"GPIO PN":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    //GPIO Bank O
    for(int iGP = 0; iGP < SUNXI_GPIO_O_NR; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpio%d",SUNXI_GPO(iGP)).c_str(),(iGP==0)?"GPIO PO":"",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
}

void SUNXI::destroy( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;

    //Removing the attributes
    for(unsigned iEl = 0; iEl < p->pEl.fldSize(); )
	try { p->pEl.fldDel(iEl); }
	catch(TError &err) { mess_warning(err.cat.c_str(), err.mess.c_str()); iEl++; }
}

void SUNXI::enable( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;

    if(sunxi_gpio_init() != 0)
	throw TError(p->nodePath().c_str(),_("Init SUNXI GPIO library error whether that is not SUNXI or there is not access."));

    MtxAlloc res(p->owner().enRes(), true);
    if(use)	throw TError(p->nodePath().c_str(), _("SUNXI GPIO is already used."));
    use = true;

    //Init for loading the direction and reversion flag
    vector<string> ls;
    p->pEl.fldList(ls);
    for(unsigned iEl = 0; iEl < ls.size(); iEl++) {
	if(ls[iEl].compare(0,4,"gpio") != 0) continue;
	int pin = atoi(ls[iEl].c_str()+4);
	AutoHD<TVal> cVl = p->vlAt(ls[iEl]);
	switch(s2i(p->modPrm(TSYS::strMess("GPIOmode%d",pin)))) {
	    case 0:	//Disabled
		cVl.at().fld().setFlg(cVl.at().fld().flg()|TFld::NoWrite);
		break;
	    case 1:	//Input
		cVl.at().fld().setFlg(cVl.at().fld().flg()|TFld::NoWrite);
		sunxi_gpio_set_cfgpin(pin, SUNXI_GPIO_INPUT);
		sunxi_gpio_pullup(pin, SUNXI_PULL_NONE);
		break;
	    case 2:	//Input (pull up)
		cVl.at().fld().setFlg(cVl.at().fld().flg()|TFld::NoWrite);
		sunxi_gpio_set_cfgpin(pin, SUNXI_GPIO_INPUT);	//Or SUNXI_GPIO_PER
		sunxi_gpio_pullup(pin, SUNXI_PULL_UP);
		break;
	    case 3:	//Input (pull down)
		cVl.at().fld().setFlg(cVl.at().fld().flg()|TFld::NoWrite);
		sunxi_gpio_set_cfgpin(pin, SUNXI_GPIO_INPUT);	//Or SUNXI_GPIO_PER
		sunxi_gpio_pullup(pin, SUNXI_PULL_DOWN);
		break;
	    case 4:	//Output
		cVl.at().fld().setFlg(cVl.at().fld().flg()&(~TFld::NoWrite));
		sunxi_gpio_set_cfgpin(pin, SUNXI_GPIO_OUTPUT);
		break;
	}
	cVl.at().fld().setReserve(p->modPrm(TSYS::strMess("GPIOrev%d",pin)));
    }

    //Register the functions
    p->fReg(new GPIO_mode());
    p->fReg(new GPIO_get());
    p->fReg(new GPIO_put());
}

void SUNXI::disable( TParamContr *ip )
{
    use = false;
}

void SUNXI::vlGet( TParamContr *ip, TVal &vo )
{
    if(vo.name().compare(0,4,"gpio") == 0)
	vo.setB(bool(sunxi_gpio_input(s2i(vo.name().substr(4))))^bool(s2i(vo.fld().reserve())), 0, true);
}

void SUNXI::vlSet( TParamContr *ip, TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(vo.name().compare(0,4,"gpio") == 0)
	sunxi_gpio_output(s2i(vo.name().substr(4)), vl.getB()^bool(s2i(vo.fld().reserve())));
}

//*************************************************
//* GPIO mode                                     *
//*************************************************
void SUNXI::GPIO_mode::calc( TValFunc *v ) {
    int pin = v->getI(1);
    switch(v->getI(2)) {
        case 1:
	    sunxi_gpio_set_cfgpin(pin, SUNXI_GPIO_INPUT);
	    sunxi_gpio_pullup(pin, SUNXI_PULL_NONE);
	    break;
	case 2:
	    sunxi_gpio_set_cfgpin(pin, SUNXI_GPIO_INPUT);	//Or SUNXI_GPIO_PER
	    sunxi_gpio_pullup(pin, SUNXI_PULL_UP);
	    break;
	case 3:
	    sunxi_gpio_set_cfgpin(pin, SUNXI_GPIO_INPUT);	//Or SUNXI_GPIO_PER
	    sunxi_gpio_pullup(pin, SUNXI_PULL_DOWN);
	    break;
	case 4:
	    sunxi_gpio_set_cfgpin(pin, SUNXI_GPIO_OUTPUT);
	    break;
    }
    v->setI(0, 0);
}

//*************************************************
//* Get GPIO value                                *
//*************************************************
void SUNXI::GPIO_get::calc( TValFunc *v )	{ v->setB(0, (bool)sunxi_gpio_input(v->getI(1))); }

//*************************************************
//* Put value to GPIO                             *
//*************************************************
void SUNXI::GPIO_put::calc( TValFunc *v )	{ sunxi_gpio_output(v->getI(0), v->getB(1)); }
