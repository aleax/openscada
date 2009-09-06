/********************************************************************/
/*  7188EX/7188XB/7186EX + X305 head file                           */
/*                                                                  */
/*  [5,July,2002] by David Version : 1.00                           */
/*  [07, Oct, 2005] by Liam                                         */
/*  [23, Mar, 2007] by Liam                                         */
/********************************************************************/
/********************************************************************/
/*  X305: 7 12-Bit A/D channels (Analog input) +/-5V                */
/*        1 12-Bit D/A channel (Analog output) +/-5V                */
/*        2 D/I channels                                            */
/*        2 D/O channels                                            */
/********************************************************************/
/********************************************************************/
/*	[Caution]                                                       */
/*	The EEPROM block 7 on X board is used to store A/D calibration  */
/*	settings. When you use the EEPROM on X board, do not overwrite  */
/*  it.                                                             */
/********************************************************************/

#ifndef __X305_H
#define __X305_H

#ifdef __cplusplus
extern "C" {
#endif

#define X305_DigitalIn      X305_Read_All_DI
#define X305_DigitalOut     X305_Write_All_DO

int X305_Init(void);
/*  Return value: 0   ==> success
    Return value: <>0 ==> error
    Bit0: 1 ==> (Ch0)Reads A/D Gain falure
    Bit1: 1 ==> (Ch0)Reads A/D Offset falure
    Bit2: 1 ==> (Ch0)Reads D/A Gain falure
    Bit3: 1 ==> (Ch0)Reads D/A Offset falure    */
    
unsigned X305_GetLibVersion(void);
/*  Current version is 2.01 (return 0x0201) */

float X305_Read_AD_CalibrationGain(void);
/*  Return 10.0 when no setting in EEPROM   */

float X305_Read_AD_CalibrationOffset(void);
/*  Return 10.0 when no setting in EEPROM   */

float X305_Read_DA_CalibrationGain(void);
/*  Return 10.0 when no setting in EEPROM   */

float X305_Read_DA_CalibrationOffset(void);
/*  Return 10.0 when no setting in EEPROM   */

float X305_AnalogIn(int iChannel);
/*  iChannel = 0~6 ----> ch1~ch7  , 
             = 7   ----> AO readback
    return data = -5.0 ~ 5.0    */
    
void X305_AnalogOut(float fValue);
/*  fValue: -5.0 ~ 5.0  */

int X305_Read_All_DI(void);
/*  Return data =  0x00~0x03
    Return 1 => open
                Logic high level (+3.5V ~ +30V)
    Return 0 => close to GND
                Logic low level (0V ~ +1V)  */

int X305_Read_One_DI(int iChannel);
/*  iChannel = 0 ~ 2
    Return 1 => open
                Logic high level (+3.5V ~ +30V)
    Return 0 => close to GND
                Logic low level (0V ~ +1V)  */

void X305_Write_All_DO(int iOutValue);
/*  iOutValue: 0x00 ~ 0x03  */

void X305_Write_One_DO(int iChannel, int iStatus);
/*  iChannel = 0 ~ 2
    iStatus = 1 => Status is ON
    iStatus = 0 => Status is OFF    */

int X305_Read_All_DO(void);
/*  Return data =  0x00 ~ 0x03  */

int X305_Read_One_DO(int iChannel);
/*  iChannel = 0 ~ 2
    Return 1 => ON
    Return 0 => OFF */

extern float    X305_fAD_Gain, X305_fAD_Offset;
extern float    X305_fDA_Gain, X305_fDA_Offset;

#ifdef __cplusplus
}
#endif

#endif