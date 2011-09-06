/*******************************************************************/
/*    i-8084W driver head file                                     */
/*                                                                 */
/*    Note: head file version is different to lib file version.    */
/*    file name: i8084w.h                                           */
/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

//for LinPAC-270 slot0~slot8
static int SlotAddr[9] ={0x1,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8};

extern unsigned char i8084W_ChannelMode[8][8];	// slot*8, channel*8
                                           

int i8084W_GetLibVersion(void);
   

void i8084W_GetLibDate(unsigned char *LibDate);
    
    
//short i8084W_GetFirmwareVersion(int slot, short* ver);    


int i8084W_InitDriver(int Slot);
    /*
    Configure the 8084w with the setting stored in the EEPROM.
    If there is no settings stored in the EEPROM, the function 
    will call i8084w_RecoverDefaultSetting.
    
    Slot: 0~8
    return:  0 --> OK
            -1 --> Module not found
            >0 --> Some Pulse/Dir counters have one count offset (+1)
                   Bit0=1 ==> A0 has one count offset (+1)
                   Bit2=1 ==> A1 has one count offset (+1)
                   Bit4=1 ==> A2 has one count offset (+1)
                   Bit6=1 ==> A3 has one count offset (+1)
                   (due to the input channel is high)
    */


int i8084W_SetChannelMode(int Slot, int Channel, int Mode);
    /*
    Slot: 0~8
    Channel: 0~7
    Mode:
        0 --> Dir/Pulse Counter
        1 --> Up/Down   Counter
        2 --> Frequency 
        3 --> Up Counter
        4 --> AB Phase
        
    return: 0 ==> No error
            1 ==> The Pulse/Dir counter has one count offset (+1)
                  (due to the input channel is high)   
    */


int i8084W_AutoScan(void);
    /*
    Auto sacn the i8084w to updates 8 channels.
    
    This function is used to update the hardware counter values.
    The hardware counter is 16-bit.
    User's code must call the function or 
        i8084W_ReadCntPulseDir,
        i8084W_ReadCntUpDown,
        i8084W_ReadFreq,
        i8084W_ReadCntUp
        i8084W_ReadAB Phase
    before the hardware counter is overflow.
    Under very high speed signal input, for example: 450K Hz,
    the 16-bit counter is overflow round 145 ms.
    
    To avoid the overflow situation, user's code is recommended
    to call i8084w_AutoScan every 70 ms.    
    */          

int i8084W_ReadCntABPhase(int Slot, int Channel,long *Cnt32U,int *Overflow);        

int i8084W_ReadCntPulseDir(int Slot, int Channel,long *Cnt32U,int *Overflow);
    /*
    Read Pulse/Dir Counter
    Slot: 0~8
    Channel: 0~7
    Cnt32L = 32-bit UpDown Counter
      = Bit31=0 --> Up Count   (count >0)
      = Bit31=1 --> Down Count (count <0)
    Overflow=number of overflow
    
    Total count = over * 0x80000000 + count
    ExampleA: over=1 , count=16384, 
              total count = (1)*0x80000000 +16384 = 2147500032
    ExampleB: over=-1 , count=-8192, 
              total count = (-1)*0x80000000 -8192 = -2147491840
    */    


int i8084W_ReadCntUpDown(int Slot, int Channel,long *Cnt32U,int *Overflow);
    /*
    Read UpDown Counter
    Slot: 0~8
    Channel: 0~7
    Cnt32L = 32-bit UpDown Counter
      = Bit31=0 --> Up Count   (count >0)
      = Bit31=1 --> Down Count (count <0)
    Overflow=number of overflow
    
    Total count = over * 0x80000000 + count
    ExampleA: over=1 , count=16384, 
              total count = (1)*0x80000000 +16384 = 2147500032
    ExampleB: over=-1 , count=-8192, 
              total count = (-1)*0x80000000 -8192 = -2147491840
    */    
    

//int i8084W_ReadFreq_AVG(int Slot, int Channel,unsigned long UpdateTime, unsigned long *Freq);	
 
int i8084W_ReadFreq(int Slot, int Channel, unsigned long *Freq);
    /*
    Slot: 0~8
    Channel: 0~7
    Freq: unit=Hz
    */
    

int i8084W_ReadCntUp(int Slot, int Channel, unsigned long *Cnt32U, unsigned int *OverFlow);
    /*
    Read Up Counter
    Slot: 0~8
    Channel: 0~7
       --> 0=A0, 1=B0, ...,6=A3,7=B3
    Cnt32U = 32-bit Up Counter
    Overflow=number of Overflow
    
    Total count = over * 0x100000000 + count
    ExampleA: over=1 , count=16384, 
              total count = (1)*0x100000000 +16384 = 4294983680
    */    


int i8084W_ClrCnt(int Slot, int Channel);
    /*
    Clear Counter
    Slot: 0~8
    Channel: 0~7
    
    return: 0 ==> No error
            1 ==> The Pulse/Dir counter has one count offset (+1).
                  It is due to the pulse channel is high.
                  The correct initial situation is: 
                        pulse channel is low or open
                        dir signal is high or low.
    */
    
//===================================================================
//==== Functions to configure I8084w ==================================
//===================================================================

void i8084W_RecoverDefaultSetting(int Slot);
    /*
    Slot = 0~8
    
    Default settings:
        XOR register=0
        Channel mode= 3 (Up counter mode)
        Frequency operate mode = 0 (Auto mode)
        Frequency update time: Auto mode =330 ms
                               Low freq mode = 1000 ms
                               High freq mode = 100 ms
        Low Pass Filter status = disable
        Low Pass Filter signal width = 1 ms
    */

int  i8084W_ReadXorRegister(int Slot, int Channel, int *XorReg);
    /*
    Slot: 0~8
    Channel: 0~7
    *XorReg: 0 ==> Low active  (signal from High to Low, count changed)
             1 ==> High acitve (signal from Low to High, count changed)
    
    return 0	 --> OK
    return others --> error codes  
    */
    

int  i8084W_SetXorRegister(int Slot, int Channel, int XorReg);
    /*
    Slot: 0~8
    Channel: 0~7
    XorReg: 0 ==> Low active  (signal from High to Low, count changed)
            1 ==> High acitve (signal from Low to High, count changed)
    
    return: 0 ==> No error
            1 ==> The Pulse/Dir counter has one count offset (+1)
                  (due to the input channel is high)
    */


int i8084W_ReadChannelMode(int Slot, int Channel, int *Mode);
    /*
    Slot: 0~8
    Channel: 0~7
    Mode:
        0 --> Dir/Pulse Counter
        1 --> Up/Down   Counter
        2 --> Frequency 
        3 --> Up Counter
        4 --> AB Phase
    */
    



int i8084W_ReadLowPassFilter_Us(int Slot, int Channel, unsigned int *Us);
    /*
    Read Low Pass Filter
    Slot: 0~8
    Channel: 0~7
    Us: 1~32767, pulse width, unit=0.001 ms
    */
    

int i8084W_SetLowPassFilter_Us(int Slot, int Channel, unsigned int Us);
    /*
    Set Low Pass Filter
    Slot: 0~8
    Channel: 0~7  
    Us: 1~32767, pulse width, unit=micro second
    */    
    

void i8084W_ReadLowPassFilter_Status(int Slot,int Channel,int *Status);
    /*
    Slot: 0~7
    Channel:0~7
    Status: 0=disable
            1=enable
    */


void i8084W_SetLowPassFilter_Status(int Slot,int Channel,int Status);
    /*
    Slot: 0~8
	Channel:0~7
	Status: 0=disable
	        1=enable
	*/    


void i8084W_ReadFreqMode(int Slot, int Channel, int *Mode);
    /*
    Slot: 0~8
    Channel: 0~7
    *Mode: 0=Auto
           1=Low Frequency
           2=High Frequency
    */	
    

void i8084W_SetFreqMode(int Slot, int Channel, int Mode);
    /*
    Slot: 0~8
    Channel: 0~7
    Mode: 0=Auto
          1=Low Frequency
          2=High Frequency
    */    
    

void i8084W_ReadFreqUpdateTime(int Slot, int *AutoMode_UpdateTime ,
                                        int *LowMode_UpdateTime,
                                        int *HighMode_UpdateTime);                                        
    /*			  	
    Reads the update time used by frequency measurement algorithm
    Slot: 0~8
    AutoMode_UpdateTime = time period for Auto mode, unit: ms 
    LowMode_UpdateTime  = time period for Low Frequency mode, unit: ms
    LowMode_UpdateTime  = time period for High Frequency mode, unit: ms
    */


int i8084W_SetFreqUpdateTime(int Slot, int AutoMode_UpdateTime,
                                      int LowMode_UpdateTime,
                                      int HighMode_UpdateTime);                                      
    /*			  	
    Sets the update time used by frequency measurement algorithm
    Slot: 0~8
    AutoMode_UpdateTime = time period for Auto mode, unit: ms 
    LowMode_UpdateTime  = time period for Low Frequency mode, unit: ms
    LowMode_UpdateTime  = time period for High Frequency mode, unit: ms
    */    
         
//unsigned short i8084W_ReadFreqTimeoutValue(int Slot, int Channel );

//void i8084W_SetFreqTimeoutValue(int Slot, int Channel, unsigned short TimeOutValue );

        
//===================================================================        
//===== Other Functions =============================================
//===================================================================        

int i8084W_ReadDI_Xor(int Slot, int *DI);
    /*
    Slot: 0~8
    *DI: Bit0 = DI of A0 after XorControl
    *DI: Bit1 = DI of B0 after XorControl
    ...
    *DI: Bit7 = DI of B3 after XorControl
    
    return  = 0 --> OK
           <> 0 --> error codes
    */   
    

int i8084W_ReadDI_XorLPF(int Slot, int *DI);
    /*
    Slot: 0~8
    *DI: Bit0 = DI of A0 after XorControl & Low Pass Filter
    *DI: Bit1 = DI of B0 after XorControl & Low Pass Filter
    ...
    *DI: Bit7 = DI of B3 after XorControl & Low Pass Filter
    
    return  = 0 --> OK
           <> 0 --> error codes
    */        
    



//===================================================================
//============ Functions to read/write EEPROM =======================
//===================================================================

int  i8084W_EepWriteEnable(int Slot);
    /*
    Write_Enable EEPROM
    Slot: 0~8
    return 0	 --> OK
    return others --> error codes
    */


int  i8084W_EepWriteDisable(int Slot);
    /*
    Write_Disable EEPROM
    Slot: 0~8
    return 0	 --> OK
    return others --> error codes
    */


int  i8084W_EepWriteWord(int Slot, int Addr, int Value);
    /*
    write 16-bit data to EEP
    Slot = 0 ~ 8  = Slot Number
    Addr:  0~39  for users 
          40~63  for 8080 configuration
    Value = two bytes integer
    
    return  0 --> OK
           -1 --> Address error
    */



int  i8084W_EepReadWord(int Slot, int Addr, int *Value);
    /*
    read 16-bit data from EEP
    Slot: 0~8
    Addr:  0~39  for users 
          40~63  for 8080 configuration
    Value: two bytes integer
    return  0 --> OK
           -1 --> Address error
	*/
	
#ifdef __cplusplus
}
#endif
