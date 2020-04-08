
//OpenSCADA module DAQ.GPIO file: da_BCM2835.cpp
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

#include <bcm2835.h>

#include "GPIO_module.h"
#include "da_BCM2835.h"

using namespace ModGPIO;

//*************************************************
//* BCM2835					  *
//*************************************************
BCM2835::BCM2835( ) : TTypeParam("BCM2835", _("BCM2835 and newer")), use(false)
{
    fldAdd(new TFld("MOD_PRMS",_("Module addition parameters"),TFld::String,TFld::FullText|TCfg::NoVal,"100000"));
}

void BCM2835::create( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;

    p->pEl.fldAdd(new TFld("gpio0","GPIO 0: v1P1.3",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio1","GPIO 1: v1P1.5",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio2","GPIO 2: {v2P1,J8}.3",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio3","GPIO 3: {v2P1,J8}.5",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio4","GPIO 4: 7",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio5","GPIO 5: {v2P1,J8}.29",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio6","GPIO 6: {v2P1,J8}.31",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio7","GPIO 7: 26",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio8","GPIO 8: 24",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio9","GPIO 9: 21",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio10","GPIO 10: 19",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio11","GPIO 11: 23",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio12","GPIO 12: {v2P1,J8}.32",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio13","GPIO 13: {v2P1,J8}.33",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio14","GPIO 14: 8",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio15","GPIO 15: 10",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio16","GPIO 16: {v2P1,J8}.36",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio17","GPIO 17: 11",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio18","GPIO 18: 12",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio19","GPIO 19: {v2P1,J8}.35",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio20","GPIO 20: {v2P1,J8}.38",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio21","GPIO 21: v1P1.13,{v2P1,J8}.40",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio22","GPIO 22: 15",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio23","GPIO 23: 16",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio24","GPIO 24: 11",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio25","GPIO 25: 22",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio26","GPIO 26: {v2P1,J8}.37",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio27","GPIO 27: {v2P1,J8}.13",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio28","GPIO 28: P5.3",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio29","GPIO 29: P5.4",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio30","GPIO 30: P5.5",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    p->pEl.fldAdd(new TFld("gpio31","GPIO 31: P5.6",TFld::Boolean,TVal::DirRead|TVal::DirWrite));
}

void BCM2835::destroy( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;

    //Removing the attributes
    for(unsigned iEl = 0; iEl < p->pEl.fldSize(); )
	try { p->pEl.fldDel(iEl); }
	catch(TError &err) { mess_warning(err.cat.c_str(), err.mess.c_str()); iEl++; }
}

void BCM2835::enable( TParamContr *ip )
{
    TMdPrm *p = (TMdPrm *)ip;

    if(!bcm2835_init())
	throw TError(p->nodePath().c_str(), _("Init bcm2835 library error whether that is not bcm2835 or there is not access."));

    MtxAlloc res(p->owner().enRes(), true);
    if(use)	throw TError(p->nodePath().c_str(), _("BCM2835 GPIO is already used."));
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
		bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
		bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_OFF);
		break;
	    case 2:	//Input (pull up)
		cVl.at().fld().setFlg(cVl.at().fld().flg()|TFld::NoWrite);
		bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
		bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_UP);
		break;
	    case 3:	//Input (pull down)
		cVl.at().fld().setFlg(cVl.at().fld().flg()|TFld::NoWrite);
		bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
		bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_DOWN);
		break;
	    case 4:	//Output
		cVl.at().fld().setFlg(cVl.at().fld().flg()&(~TFld::NoWrite));
		bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
		break;
	}
	cVl.at().fld().setReserve(p->modPrm(TSYS::strMess("GPIOrev%d",pin)));
    }

    //Register the functions
    p->fReg(new GPIO_mode());
    p->fReg(new GPIO_get());
    p->fReg(new GPIO_put());
}

void BCM2835::disable( TParamContr *ip )
{
    bcm2835_close();
    use = false;
}

void BCM2835::vlGet( TParamContr *ip, TVal &vo )
{
    if(vo.name().compare(0,4,"gpio") == 0)
	vo.setB(bool(bcm2835_gpio_lev(s2i(vo.name().substr(4))))^bool(s2i(vo.fld().reserve())), 0, true);
}

void BCM2835::vlSet( TParamContr *ip, TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    if(vo.name().compare(0,4,"gpio") == 0)
	bcm2835_gpio_write(s2i(vo.name().substr(4)), vl.getB()^bool(s2i(vo.fld().reserve())));
}

//*************************************************
//* GPIO mode                                     *
//*************************************************
void BCM2835::GPIO_mode::calc( TValFunc *v ) {
    int pin = v->getI(1);
    switch(v->getI(2)) {
	case 1:
	    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
	    bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_OFF);
	    break;
	case 2:
	    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
	    bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_UP);
	    break;
	case 3:
	    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
	    bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_DOWN);
	    break;
	case 4:
	    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
	    break;
    }
    v->setI(0, 0);
}

//*************************************************
//* Get GPIO value                                *
//*************************************************
void BCM2835::GPIO_get::calc( TValFunc *v )	{ v->setB(0, (bool)bcm2835_gpio_lev(v->getI(1))); }

//*************************************************
//* Put value to GPIO                             *
//*************************************************
void BCM2835::GPIO_put::calc( TValFunc *v )	{ bcm2835_gpio_write(v->getI(0), v->getB(1)); }
