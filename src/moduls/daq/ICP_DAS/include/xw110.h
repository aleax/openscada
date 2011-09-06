/********************************************************************/
/*  7188EX/7188XB/7186EX + X305B head file                          */
/*                                                                  */
/*  [03, Dec, 2007] by Liam     version 1.00                        */
/********************************************************************/
/********************************************************************/
/*  XW110:                                                          */
/*          16 D/I channel                                           */
/*                                                 */
/********************************************************************/
/********************************************************************/
/*	[Caution]                                                       */
/*	The EEPROM block 7 on X board is used to store A/D calibration  */
/*	settings. When you use the EEPROM on X board, do not overwrite  */
/*  it.                                                             */
/********************************************************************/

#ifndef __XW110_H
#define __XW110_H

#ifdef __cplusplus
extern "C" {
#endif

#define XW110_DigitalIn      XW110_Read_All_DI


int XW110_Init(void);
/*  Return value: 0   ==> success
    Return value: <>0 ==> error
    Bit0: 1 ==> (Ch0)Reads A/D Gain falure
    Bit1: 1 ==> (Ch0)Reads A/D Offset falure    */
    
unsigned XW110_GetLibVersion(void);
/*  Current version is 1.00 (return 0x0100) */


int XW110_Read_All_DI(void);
/*  Return data =  0x0000~0xffff
    Return 1 => open
                Logic high level (+3.5V ~ +30V)
    Return 0 => close to GND
                Logic low level (0V ~ +1V)  */

int XW110_Read_One_DI(int iChannel);
/*  iChannel = 0
    Return 1 => open
                Logic high level (+3.5V ~ +30V)
    Return 0 => close to GND
                Logic low level (0V ~ +1V)  */



#ifdef __cplusplus
}
#endif

#endif
