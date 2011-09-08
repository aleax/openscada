/********************************************************************/
/*  7188EX/7188XB/7186EX + X305B head file                          */
/*                                                                  */
/*  [03, Dec, 2007] by Liam     version 1.00                        */
/********************************************************************/
/********************************************************************/
/*  X305B:  7 12-Bit A/D channels (Analog input) 0~20mA             */
/*          1 D/I channel                                           */
/*          1 D/O channel                                           */
/*          1 RS-232 channels                                       */
/********************************************************************/
/********************************************************************/
/*	[Caution]                                                       */
/*	The EEPROM block 7 on X board is used to store A/D calibration  */
/*	settings. When you use the EEPROM on X board, do not overwrite  */
/*  it.                                                             */
/********************************************************************/

#ifndef __XW305B_H
#define __XW305B_H

#ifdef __cplusplus
extern "C" {
#endif

#define XW305B_DigitalIn      XW305B_Read_All_DI
#define XW305B_DigitalOut     XW305B_Write_All_DO
#define fAD_Gain    XW305B_fAD_Gain
#define fAD_Offset  XW305B_fAD_Offset
 
int XW305B_Init(void);
/*  Return value: 0   ==> success
    Return value: <>0 ==> error
    Bit0: 1 ==> (Ch0)Reads A/D Gain falure
    Bit1: 1 ==> (Ch0)Reads A/D Offset falure    */
    
unsigned XW305B_GetLibVersion(void);
/*  Current version is 1.00 (return 0x0100) */

float XW305B_Read_AD_CalibrationGain(void);
/*  Return 10.0 when no setting in EEPROM   */

float XW305B_Read_AD_CalibrationOffset(void);
/*  Return 10.0 when no setting in EEPROM   */

float XW305B_AnalogIn(int iChannel);
/*  iChannel = 0~6 ----> ch1~ch7
    return data = 0.0mA ~ 20.0mA    */

int XW305B_Read_All_DI(void);
/*  Return data =  0x00~0x03
    Return 1 => open
                Logic high level (+3.5V ~ +30V)
    Return 0 => close to GND
                Logic low level (0V ~ +1V)  */

int XW305B_Read_One_DI(int iChannel);
/*  iChannel = 0
    Return 1 => open
                Logic high level (+3.5V ~ +30V)
    Return 0 => close to GND
                Logic low level (0V ~ +1V)  */

void XW305B_Write_All_DO(int iOutValue);
/*  iOutValue: 0x0 ~ 0x1  */

void XW305B_Write_One_DO(int iChannel, int iStatus);
/*  iChannel = 0
    iStatus = 1 => Status is ON
    iStatus = 0 => Status is OFF    */

int XW305B_Read_All_DO(void);
/*  Return data =  0x00 ~ 0x01  */

int XW305B_Read_One_DO(int iChannel);
/*  iChannel = 0
    Return 1 => ON
    Return 0 => OFF */

extern float    XW305B_fAD_Gain, XW305B_fAD_Offset;

#ifdef __cplusplus
}
#endif

#endif
