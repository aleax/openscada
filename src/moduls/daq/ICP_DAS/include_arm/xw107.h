/********************************************************************/
/*  7188EX/7188XB/7186EX + X305B head file                          */
/*                                                                  */
/*  [03, Dec, 2007] by Liam     version 1.00                        */
/********************************************************************/
/********************************************************************/
/*  XW107:                                                          */
/*          8 D/I channel                                           */
/*          8 D/O channel                                           */
/********************************************************************/
/********************************************************************/
/*	[Caution]                                                       */
/*	The EEPROM block 7 on X board is used to store A/D calibration  */
/*	settings. When you use the EEPROM on X board, do not overwrite  */
/*  it.                                                             */
/********************************************************************/

#ifndef __XW107_H
#define __XW107_H

#ifdef __cplusplus
extern "C" {
#endif

#define XW107_DigitalIn      XW107_Read_All_DI
#define XW107_DigitalOut     XW107_Write_All_DO
//#define fAD_Gain    XW107_fAD_Gain
//#define fAD_Offset  XW107_fAD_Offset

int XW107_Init(void);
/*  Return value: 0   ==> success
    Return value: <>0 ==> error
    Bit0: 1 ==> (Ch0)Reads A/D Gain falure
    Bit1: 1 ==> (Ch0)Reads A/D Offset falure    */
    
unsigned XW107_GetLibVersion(void);
/*  Current version is 1.00 (return 0x0100) */


int XW107_Read_All_DI(void);
/*  Return data =  0x00~0xff
    Return 1 => open
                Logic high level (+3.5V ~ +30V)
    Return 0 => close to GND
                Logic low level (0V ~ +1V)  */

int XW107_Read_One_DI(int iChannel);
/*  iChannel = 0
    Return 1 => open
                Logic high level (+3.5V ~ +30V)
    Return 0 => close to GND
                Logic low level (0V ~ +1V)  */

void XW107_Write_All_DO(int iOutValue);
/*  iOutValue: 0x0 ~ 0xFF  */

void XW107_Write_One_DO(int iChannel, int iStatus);
/*  iChannel = 0
    iStatus = 1 => Status is ON
    iStatus = 0 => Status is OFF    */

int XW107_Read_All_DO(void);
/*  Return data =  0x00 ~ 0xff  */

int XW107_Read_One_DO(int iChannel);
/*  iChannel = 0
    Return 1 => ON
    Return 0 => OFF */


#ifdef __cplusplus
}
#endif

#endif
