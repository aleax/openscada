/*
 * linpac.h
 * (C) 2004 Moki Matsushima <moki@icpdas.com>
 *
 * All this is intended to be used with a ISA-like slot on the SA1100's 
 * static memory bus. 
 * This is included by slot.c.
 */
//for 8k/5k/2k by cindy 20080910
#ifdef __cplusplus
extern "C" {
#endif

extern void SetLED(unsigned int addr,unsigned int value);
//extern void SetLED(unsigned int bFlag);
extern int GetBackPlaneID(); //getport 9 0 
extern int GetSlotCount();   //getport 9 35
extern int GetDIPswitch();   //getport 9 1 
extern int GetRotaryID(int slot);
extern float GetSDKversion(void);

extern unsigned int inb(unsigned int slot,unsigned int offset);
//extern unsigned char inb(unsigned int slot,unsigned int offset);
extern void outb(unsigned int slot,unsigned int offset, unsigned int data);
//extern void outb(unsigned int slot,unsigned int offset, unsigned char data);
extern int setSignal(int slot,int signal,pid_t pid);
extern int GetModuleType(int slot);
extern void ChangeToSlot(int slot);
extern void Read_SN(unsigned char serial_num[8]);
extern int GetNameOfModule(int slot);
extern unsigned int getSystemStatus(void);
extern unsigned char Read_SRAM(int offset);
extern void Write_SRAM(int offset, unsigned char data);
extern void EnableWDT(unsigned int msecond);
extern void DisableWDT(void);
extern unsigned int WatchDogSWEven(void);
extern void ClearWDTSWEven(unsigned int rcsr);
extern void EnableTimer(unsigned int usecond);
extern void DisableTimer(void);
extern unsigned char Read_EEP(int block, int offset);
extern void Write_EEP(int block, int offset, unsigned char data);
extern void Enable_EEP(void);
extern void Disable_EEP(void);
extern int Open_SlotAll(void);
extern void Close_SlotAll(void);
//extern __bool_t Close_SlotAll(void);
extern int Open_Slot(int slot);
//extern __bool_t Close_Slot(int slot);
extern void Close_Slot(int slot);
extern void DO_8(int slot, unsigned char data);
extern void DO_16(int slot, unsigned int data);
extern void DO_32(int slot, unsigned long data);
extern void DIO_DO_8(int slot, unsigned char data);
extern void DIO_DO_16(int slot, unsigned int data);
extern unsigned char DI_8(int slot);
extern unsigned char DI_I8064(int slot);
extern unsigned int DI_16(int slot);
extern unsigned long DI_32(int slot);
extern unsigned char DIO_DI_8(int slot);
extern unsigned int DIO_DI_16(int slot);

extern unsigned char DO_8_RB(int slot);
extern unsigned int DO_16_RB(int slot);
extern unsigned long DO_32_RB(int slot);
extern unsigned char DIO_DO_8_RB(int slot);
extern unsigned int DIO_DO_16_RB(int slot);

extern void DO_8_BW(int slot, int bit, int data);
extern void DO_16_BW(int slot, int bit, int data);
extern void DO_32_BW(int slot, int bit, int data);
extern void DIO_DO_8_BW(int slot, int bit, int data);
extern void DIO_DO_16_BW(int slot, int bit, int data);

extern int DI_8_BW(int slot, int bit);
extern int DI_16_BW(int slot, int bit);
extern int DI_32_BW(int slot, int bit);

extern float HEX_TO_FLOAT_Cal(int HexValue,int slot,int gain);
extern float CalHex_TO_FLOAT(int HexValue,int gain);
extern void ARRAY_HEX_TO_FLOAT_cal(int *HexValue,float *FloatValue,int slot,int gain,int len);
extern void ARRAY_CalHex_TO_FLOAT(int *HexValue,float *FloatValue,int gain,int len);
extern void I8017_EE_WriteDisable(int slot,int offset);
extern unsigned I8017_EE_Read(int slot,int Addr);
extern unsigned I8017_EE_Write(int slot,int Addr,unsigned Data);
extern unsigned I8017_EE_Erease(int slot,int offset,int Addr);
extern void I8017_EE_WriteEnable(int slot);
extern void I8017_SetLed(int slot,unsigned led);
extern int I8017_Init(int slot);
extern void I8017_SetChannelGainMode(int slot,int ch,int gain,int mode);
extern int I8017_GetCurAdChannel_Hex(int slot);
extern int I8017_Hex_Cal(int data);
extern int I8017_Hex_Cal_Slot_Gain(int slot,int gain,int data);
extern int I8017_GetCurAdChannel_Hex_Cal(int slot);
extern float I8017_GetCurAdChannel_Float_Cal(int slot);
extern int I8017_AD_POLLING(int slot,int ch,int gain,unsigned int datacount,int *DataPtr);
extern int I8017_AD_POLLING_Cal(int slot,int ch,int gain,unsigned int datacount,int *DataPtr);
extern void I8024_EE_WriteDisable(int slot,int offset);
extern unsigned I8024_EE_Read(int slot,int Addr);
extern unsigned I8024_EE_Write(int slot,int Addr,unsigned Data);
extern unsigned I8024_EE_Erease(int slot,int offset,int Addr);
extern void I8024_EE_WriteEnable(int slot);
extern void I8024_CurrentOut(int slot,int ch,float cdata);
extern void I8024_VoltageOut(int slot,int ch,float cdata);
extern void I8024_CurrentHexOut(int slot,int ch,int hdata);
extern void I8024_VoltageHexOut(int slot,int ch,int hdata);
extern void I8024_Initial(int slot);
extern unsigned char I8090_REGISTRATION(unsigned char slot, unsigned int address);
extern void I8090_INIT_CARD(unsigned char cardNo,unsigned char x_mode,unsigned char y_mode,unsigned char z_mode);
extern unsigned int I8090_GET_ENCODER(unsigned char cardNo, unsigned char axis);
extern void I8090_RESET_ENCODER(unsigned char cardNo, unsigned char axis);
extern unsigned char I8090_GET_INDEX(unsigned char cardNo);
extern void I8090_ENCODER32_ISR(unsigned char cardNo);
extern void I8090_RESET_ENCODER32(unsigned char cardNo, unsigned char axis);
extern long I8090_GET_ENCODER32(unsigned char cardNo, unsigned char axis);
extern void I8091_reset_fifo(unsigned char cardNo);
extern unsigned char I8091_REGISTRATION(unsigned char cardNo,unsigned int address);
extern unsigned char I8091_LIMIT_X(unsigned char cardNo);
extern unsigned char I8091_LIMIT_Y(unsigned char cardNo);
extern void I8091_WAIT_X(unsigned char cardNo);
extern unsigned char I8091_IS_X_STOP(unsigned char cardNo);
extern void I8091_WAIT_Y(unsigned char cardNo);
extern unsigned char I8091_IS_Y_STOP(unsigned char cardNo);
extern void I8091_SET_VAR(unsigned char cardNo,unsigned char set_DDA_cycle,unsigned char set_Acc_Dec,unsigned int set_Low_Speed,unsigned int set_High_Speed);
extern void I8091_SET_DEFDIR(unsigned char cardNo,unsigned char defdirX,unsigned char defdirY);
extern void I8091_SET_MODE(unsigned char cardNo,unsigned char modeX,unsigned char modeY);
extern void I8091_SET_SERVO_ON(unsigned char cardNo,unsigned char sonX,unsigned char sonY);
extern void I8091_SET_NC(unsigned char cardNo, unsigned char sw);
extern void I8091_STOP_X(unsigned char cardNo);
extern void I8091_STOP_Y(unsigned char cardNo);
extern void I8091_RESET_SYSTEM(unsigned char cardNo);
extern void I8091_STOP_ALL(unsigned char cardNo);
extern void I8091_EMG_STOP(unsigned char cardNo);
extern void I8091_LSP_ORG(unsigned char cardNo,unsigned char DIR,unsigned char AXIS);
extern void I8091_HSP_ORG(unsigned char cardNo,unsigned char DIR,unsigned char AXIS);
extern void I8091_HSD_ORG(unsigned char cardNo,unsigned char DIR,unsigned char AXIS);
extern void I8091_LSP_PULSE_MOVE(unsigned char cardNo,unsigned char AXIS,long pulseN);
extern void I8091_HSP_PULSE_MOVE(unsigned char cardNo,unsigned char AXIS,long pulseN,unsigned int speed);
extern void I8091_LSP_MOVE(unsigned char cardNo,unsigned char DIR,unsigned char AXIS);
extern void I8091_HSP_MOVE(unsigned char cardNo,unsigned char DIR,unsigned char AXIS);
extern void I8091_CSP_MOVE(unsigned char cardNo,unsigned char DIR,unsigned char AXIS,unsigned int  move_speed);
extern void I8091_SLOW_DOWN(unsigned char cardNo,unsigned char AXIS);
extern void I8091_SLOW_STOP(unsigned char cardNo,unsigned char AXIS);
extern void I8091_INTP_PULSE(unsigned char cardNo,int Xpulse, int Ypulse);
extern void I8091_INTP_LINE(unsigned char cardNo,long Xpulse, long Ypulse);
extern void I8091_INTP_PROFILE(unsigned char cardNo);
extern void I8091_INTP_PROFILE_CSPD(unsigned int spd);
extern unsigned char I8091_LINE_CORE(unsigned char cardNo,long x,long y,unsigned int speed,unsigned char acc_mode);
extern unsigned char I8091_CIRCLE_CORE(unsigned char cardNo,long x,long y,unsigned char dir,unsigned int speed,unsigned char acc_mode);
extern unsigned char I8091_ARC_CORE(unsigned char cardNo,long x,long y,long R, unsigned char dir, unsigned int speed, unsigned char acc_mode);
extern void intp_state_machine();
extern void I8091_INTP_LINE02(unsigned char cardNo,long x,long y,unsigned int speed,unsigned char acc_mode);
extern void I8091_INTP_CIRCLE02(unsigned char cardNo,long x,long y,unsigned char dir,unsigned int speed, unsigned char acc_mode);
extern void I8091_INTP_ARC02(unsigned char cardNo,long x,long y,long R,unsigned char dir,unsigned int speed,unsigned char acc_mode);
extern unsigned char I8091_INTP_STOP();
extern void I8091_GET_CARD(unsigned char cardNo);
extern long GetTimeTicks_ms(void);

#define	IoRead	inb
#define	IoWrite	outb
#define	SLOT0	0
#define	SLOT1	1
#define	SLOT2	2
#define	SLOT3	3
#define	SLOT4	4
#define	SLOT5	5
#define	SLOT6	6
#define	SLOT7	7
#define	SLOT8	8
#define	SLOT9	9
#define	SLOT10	10

#ifdef __cplusplus
}
#endif
