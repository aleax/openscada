
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

//#include <bcm2835.h>

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

    for(int iGP = 0; iGP < 20; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpioPA%02d",iGP).c_str(),TSYS::strMess("GPIO PA%02d",iGP).c_str(),TFld::Boolean,TVal::DirRead|TVal::DirWrite));
    for(int iGP = 0; iGP < 8; iGP++)
	p->pEl.fldAdd(new TFld(TSYS::strMess("gpioPG%02d",iGP).c_str(),TSYS::strMess("GPIO PG%02d",iGP).c_str(),TFld::Boolean,TVal::DirRead|TVal::DirWrite));
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

    //if(!BCM2835_init())
    //	throw TError(p->nodePath().c_str(),_("Init bcm2835 library error whether that is not bcm2835 or there is not access."));

    MtxAlloc res(p->owner().dataRes(), true);
    if(use)	throw TError(p->nodePath().c_str(), _("SUNXI GPIO is already used."));
    use = true;

    //Init for loading the direction and reversion flag
    /*vector<string> ls;
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
    }*/
}

void SUNXI::disable( TParamContr *ip )
{
    //bcm2835_close();
    use = false;
}

void SUNXI::vlGet( TParamContr *ip, TVal &vo )
{
    //if(vo.name().compare(0,4,"gpio") == 0)
    //	vo.setB(bool(bcm2835_gpio_lev(s2i(vo.name().substr(4))))^bool(s2i(vo.fld().reserve())), 0, true);
}

void SUNXI::vlSet( TParamContr *ip, TVal &vo, const TVariant &vl, const TVariant &pvl )
{
    //if(vo.name().compare(0,4,"gpio") == 0)
    //	bcm2835_gpio_write(s2i(vo.name().substr(4)), vl.getB()^bool(s2i(vo.fld().reserve())));
}
