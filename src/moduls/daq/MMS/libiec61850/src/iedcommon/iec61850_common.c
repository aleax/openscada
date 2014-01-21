/*
 * iec61850_common.c
 *
 *  Copyright 2013 Michael Zillgith
 *
 *  This file is part of libIEC61850.
 *
 *  libIEC61850 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libIEC61850 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libIEC61850.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  See COPYING file for the complete license text.
 */

#include "iec61850_common.h"

#include "libiec61850_platform_includes.h"

char*
FunctionalConstraint_toString(FunctionalConstraint fc) {
    switch (fc) {
    case ST:
        return "ST";
    case MX:
        return "MX";
    case SP:
        return "SP";
    case SV:
        return "SV";
    case CF:
        return "CF";
    case DC:
        return "DC";
    case SG:
        return "SG";
    case SE:
        return "SE";
    case SR:
        return "SR";
    case OR:
        return "OR";
    case BL:
        return "BL";
    case EX:
        return "EX";
    case CO:
        return "CO";
    default:
        return NULL;
    }
}

FunctionalConstraint
FunctionalConstraint_fromString(char* fcString)
{
    if (fcString[0] == 'S') {
        if (fcString[1] == 'T')
            return ST;
        if (fcString[1] == 'P')
            return SP;
        if (fcString[1] == 'V')
            return SV;
        if (fcString[1] == 'G')
            return SG;
        if (fcString[1] == 'E')
            return SE;
        if (fcString[1] == 'R')
            return SR;

        return NONE;
    }

    if (fcString[0] == 'M') {
        if (fcString[1] == 'X')
            return MX;
        return NONE;
    }

    if (fcString[0] == 'C') {
        if (fcString[1] == 'F')
            return CF;
        if (fcString[1] == 'O')
            return CO;
        return NONE;
    }

    if (fcString[0] == 'D') {
        if (fcString[1] == 'C')
            return DC;
        return NONE;
    }

    if (fcString[0] == 'O') {
        if (fcString[1] == 'R')
            return OR;
        return NONE;
    }

    if (fcString[0] == 'B') {
        if (fcString[1] == 'L')
            return BL;
        return NONE;
    }

    if (fcString[0] == 'E') {
        if (fcString[1] == 'X')
            return EX;
        return NONE;
    }

    return NONE;
}
