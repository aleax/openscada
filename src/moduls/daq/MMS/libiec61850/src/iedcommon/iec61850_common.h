/*
 *  iec61850_common.h
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

#ifndef IEC61850_COMMON_H_
#define IEC61850_COMMON_H_

/**
 * \defgroup common_api_group libIEC61850 API common parts
 */
/**@{*/

/* Trigger options bit mask */
#define TRG_OPT_DATA_CHANGED 1
#define TRG_OPT_QUALITY_CHANGED 2
#define TRG_OPT_DATA_UPDATE 4
#define TRG_OPT_INTEGRITY 8
#define TRG_OPT_GI 16

 /* FCs according to IEC 61850-7-2 */
typedef enum eFunctionalConstraint {
    /** Status information */
    ST = 0,
    /** Measurands - analog values */
    MX = 1,
    /** Setpoint */
    SP = 2,
    /** Substitution */
    SV = 3,
    /** Configuration */
    CF = 4,
    /** Description */
    DC = 5,
    /** Setting group */
    SG = 6,
    /** Setting group editable */
    SE = 7,
    /** Service response / Service tracking */
    SR = 8,
    /** Operate received */
    OR = 9,
    /** Blocking */
    BL = 10,
    /** Extended definition */
    EX = 11,
    /** Control */
    CO = 12,
    ALL = 99,
    NONE = -1
} FunctionalConstraint;

char*
FunctionalConstraint_toString(FunctionalConstraint fc);

FunctionalConstraint
FunctionalConstraint_fromString(char* fcString);

/**@}*/

#endif /* IEC61850_COMMON_H_ */
