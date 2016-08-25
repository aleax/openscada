/***************************************************************************
 *   Copyright (C) 2011-2016 by Maxim Kochetkov                            *
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
#ifndef DA_BTE_H
#define DA_BTE_H

#include "da.h"

namespace FT3
{
    class B_BTE: public DA
    {
    public:
	//Methods
	B_BTE(TMdPrm& prm, uint16_t id, uint16_t n, bool has_params);
	~B_BTE();
	uint16_t Task(uint16_t);
	uint16_t HandleEvent(int64_t, uint8_t *);
	uint16_t setVal(TVal &val);
	string getStatus(void);

	//Attributes
	uint16_t ID;
	uint16_t count_n;
	bool with_params;

    };

} //End namespace

//*************************************************
//* BTE                                           *
//*************************************************
#endif //DA_BTE_H
