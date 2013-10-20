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

#define TRG_OPT_DATA_CHANGED 1
#define TRG_OPT_QUALITY_CHANGED 2
#define TRG_OPT_DATA_UPDATE 4
#define TRG_OPT_INTEGRITY 8
#define TRG_OPT_GI 16

 /* FCs according to IEC 61850-7-2 */
typedef enum eFunctionalConstraint {
    ST, /** Status information */
    MX, /** Measurands - analog values */
    SP, /** Setpoint */
    SV, /** Substitution */
    CF, /** Configuration */
    DC, /** Description */
    SG, /** Setting group */
    SE, /** Setting group editable */
    SR, /** Service response / Service tracking */
    OR, /** Operate received */
    BL, /** Blocking */
    EX, /** Extended definition */
    CO  /** Control */
} FunctionalConstraint;

char*
FunctionalConstrained_toString(FunctionalConstraint fc);

typedef enum {
	CONTROL_ERROR_UNKNOWN,
	CONTROL_ERROR_NOT_SUPPORTED
	//TODO ...
} ControlLastAppIError;

#endif /* IEC61850_COMMON_H_ */
