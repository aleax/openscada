/*******************************************************************/
/*    i-8080 driver head file                                      */
/*                                                                 */
/*    Note: head file version is different to lib file version.    */
/*    file name: 8080.h (mapping 93c46202.c)                                                             */
/*******************************************************************/
/*
Ver 2.0.3 [2005,Oct,17] by Kevin
    ==== English comment ==================================
    Modify comments:
        1. i8080_InitDriver 
        2. i8080_RecoverDefaultSetting

    ==== Traditional Chinese comments =====================
    修改說明
        1. i8080_InitDriver 
        2. i8080_RecoverDefaultSetting
    
---------------------------------------------------------------------
Ver 2.0.1 [2005,Jan,27] by Kevin
    ==== English comment ==================================
    Modify comments of i8080_ClrCnt
    
    ==== Traditional Chinese comments =====================
    修改 i8080_ClrCnt 的說明
    
---------------------------------------------------------------------
Ver 2.0.0 [2004,Dec,20] by Kevin
    ==== English comment ==================================
    This version is quite different to version 1.0.0
    Rename:
        i8080_GetVersion    ===> i8080_GetLibVersion
        i8080_ReadDIXor     ===> i8080_ReadDI_Xor   
        i8080_ReadDIXorLp   ===> i8080_ReadDI_XorLPF  
        i8080_ReadLowPassUs ===> i8080_ReadLowPassFilter_Us  
        i8080_SetLowPassUs  ===> i8080_SetLowPassFilter_Us
        
    Remove:
        i8080_ReadFreqConfiguration
        i8080_SetFreqConfiguration 
        i8080_ReadCntAndT
        
    Change functionarity:  
        i8080_InitDriver ===> Reads initial configurations from EEPROM.
                              The configurations includes:
                              1. XOR register
                              2. Channel mode
                              3. Low Pass Filter pulse width
                              4. Low Pass Filter status (enable/disable)
                              5. Freq mode
                              6. Freq update time
                                        
        i8080_ReadChannelMode ===> meaning of Mode is changed
        i8080_SetChannelMode  ===> meaning of Mode is changed
        
    Rename and change functionarity:
        i8080_ReadFreqConfiguration ===> i8080_ReadFreqUpdateTime
        i8080_SetFreqConfiguration  ===> i8080_SetFreqUpdateTime

    Add:
        i8080_RecoverDefaultSetting
        i8080_ReadFreqMode
        i8080_SetFreqMode
        i8080_ReadCntPulseDir:
            Pulse/Dir mode uses function i8080_ReadCntUpDown Originally.
            Now, Pulse/Dir mode uses i8080_ReadCntPuleseDir
                 Up/Down mode uses i8080_ReadCntUpDown.
        i8080_ReadLowPassFilter_Status         
        i8080_SetLowPassFilter_Status   
        
    ==== Traditional Chinese comments =====================
    這個版本和 1.0.0 版差異很多
    更改名稱:
        i8080_GetVersion    ===> GetLibVersion
        i8080_ReadDIXor     ===> i8080_ReadDI_Xor   
        i8080_ReadDIXorLp   ===> i8080_ReadDI_XorLPF  
        i8080_ReadLowPassUs ===> i8080_ReadLowPassFilter_Us  
        i8080_SetLowPassUs  ===> i8080_SetLowPassFilter_Us
        
    移除:
        i8080_ReadCntAndT
        
    改變功能:
        i8080_InitDriver ===> 由 EEPROM 將初始組態設定讀出。
                              這些組態設定包括了:
                                1. XOR register
                                2. 每個通道的模式
                                3. 低通濾波器的寬度
                                4. 低通濾波器的狀態 (啟動/關閉)
                                5. 頻率的模式
                                6. 頻率值的更新時間
                                
        i8080_ReadChannelMode ===> 改變 Mode 的意義
        i8080_SetChannelMode  ===> 改變 Mode 的意義
           
    更改名稱，並且改變功能:
        i8080_ReadFreqConfiguration ===> i8080_ReadFreqUpdateTime
        i8080_SetFreqConfiguration  ===> i8080_SetFreqUpdateTime   
     
    增加:
        i8080_RecoverDefaultSetting
        i8080_ReadFreqMode
        i8080_SetFreqMode
        i8080_ReadCntPulseDir:
            Pulse/Dir 模式原本使用 i8080_ReadCntUpDown。
            現在 Pulse/Dir 模式使用 i8080_ReadCntPuleseDir
                 Up/Down 模式使用 i8080_ReadCntUpDown.
        i8080_ReadLowPassFilter_Status         
        i8080_SetLowPassFilter_Status
       
*/
//for LinPAC-270 slot0~slot8
static int SlotAddr[9] ={0x1,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8};
extern unsigned char i8080_ChannelMode[9][8];	// slot*9, channel*8
    // 0: Dir/Pulse Counter
    // 1: Up/Down Counter
    // 2: Frequency
    // 3: Up Counter  
                                        
int i8080_GetLibVersion(void);
    //Get the version number of i8080 library (Hex) Rev:1.0.00

void i8080_GetLibDate(unsigned char *LibDate);
    // Get the date of 8080 library  ,Sep 03 2003
    
    
int i8080_InitDriver(int Slot);
    /*
    Configure the 8080 with the setting stored in the EEPROM.
    If there is no settings stored in the EEPROM, the function 
    will call i8080_RecoverDefaultSetting.
    
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

int i8080_AutoScan(void);
    /*
    Auto sacn the i8080 to updates 8 channels.
    
    This function is used to update the hardware counter values.
    The hardware counter is 16-bit.
    User's code must call the function or 
        i8080_ReadCntPulseDir,
        i8080_ReadCntUpDown,
        i8080_ReadFreq,
        i8080_ReadCntUp
    before the hardware counter is overflow.
    Under very high speed signal input, for example: 450K Hz,
    the 16-bit counter is overflow round 145 ms.
    
    To avoid the overflow situation, user's code is recommended
    to call i8080_AutoScan every 70 ms.    
    */          
    
int i8080_ReadCntPulseDir(int Slot, int Channel,long *Cnt32U,int *Overflow);
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

int i8080_ReadCntUpDown(int Slot, int Channel,long *Cnt32U,int *Overflow);
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
    
int i8080_ReadFreq(int Slot, int Channel, unsigned long *Freq);
    /*
    Slot: 0~8
    Channel: 0~7
    Freq: unit=Hz
    */
    
int i8080_ReadCntUp(int Slot, int Channel, unsigned long *Cnt32U, unsigned int *OverFlow);
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

int i8080_ClrCnt(int Slot, int Channel);
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
//==== Functions to configure 8080 ==================================
//===================================================================
void i8080_RecoverDefaultSetting(int Slot);
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
int  i8080_ReadXorRegister(int Slot, int Channel, int *XorReg);
    /*
    Slot: 0~8
    Channel: 0~7
    *XorReg: 0 ==> Low active  (signal from High to Low, count changed)
             1 ==> High acitve (signal from Low to High, count changed)
    
    return 0	 --> OK
    return others --> error codes  
    */
    
int  i8080_SetXorRegister(int Slot, int Channel, int XorReg);
    /*
    Slot: 0~8
    Channel: 0~7
    XorReg: 0 ==> Low active  (signal from High to Low, count changed)
            1 ==> High acitve (signal from Low to High, count changed)
    
    return: 0 ==> No error
            1 ==> The Pulse/Dir counter has one count offset (+1)
                  (due to the input channel is high)
    */


int i8080_ReadChannelMode(int Slot, int Channel, int *Mode);
    /*
    Slot: 0~8
    Channel: 0~7
    Mode:
        0 --> Dir/Pulse Counter
        1 --> Up/Down   Counter
        2 --> Frequency 
        3 --> Up Counter
    */
    
int i8080_SetChannelMode(int Slot, int Channel, int Mode);
    /*
    Slot: 0~8
    Channel: 0~7
    Mode:
        0 --> Dir/Pulse Counter
        1 --> Up/Down   Counter
        2 --> Frequency 
        3 --> Up Counter
        
    return: 0 ==> No error
            1 ==> The Pulse/Dir counter has one count offset (+1)
                  (due to the input channel is high)   
    */

int i8080_ReadLowPassFilter_Us(int Slot, int Channel, unsigned int *Us);
    /*
    Read Low Pass Filter
    Slot: 0~8
    Channel: 0~7
    Us: 1~32767, pulse width, unit=0.001 ms
    */
    
int i8080_SetLowPassFilter_Us(int Slot, int Channel, unsigned int Us);
    /*
    Set Low Pass Filter
    Slot: 0~8
    Channel: 0~7  
    Us: 1~32767, pulse width, unit=micro second
    */    
    
void i8080_ReadLowPassFilter_Status(int Slot,int Channel,int *Status);
    /*
    Slot: 0~7
    Channel:0~7
    Status: 0=disable
            1=enable
    */

void i8080_SetLowPassFilter_Status(int Slot,int Channel,int Status);
    /*
    Slot: 0~8
	Channel:0~7
	Status: 0=disable
	        1=enable
	*/    

void i8080_ReadFreqMode(int Slot, int Channel, int *Mode);
    /*
    Slot: 0~8
    Channel: 0~7
    *Mode: 0=Auto
           1=Low Frequency
           2=High Frequency
    */	
    
void i8080_SetFreqMode(int Slot, int Channel, int Mode);
    /*
    Slot: 0~8
    Channel: 0~7
    Mode: 0=Auto
          1=Low Frequency
          2=High Frequency
    */    
    
void i8080_ReadFreqUpdateTime(int Slot, int *AutoMode_UpdateTime ,
                                        int *LowMode_UpdateTime,
                                        int *HighMode_UpdateTime);
    /*			  	
    Reads the update time used by frequency measurement algorithm
    Slot: 0~8
    AutoMode_UpdateTime = time period for Auto mode, unit: ms 
    LowMode_UpdateTime  = time period for Low Frequency mode, unit: ms
    LowMode_UpdateTime  = time period for High Frequency mode, unit: ms
    */

int i8080_SetFreqUpdateTime(int Slot, int AutoMode_UpdateTime,
                                      int LowMode_UpdateTime,
                                      int HighMode_UpdateTime);
    /*			  	
    Sets the update time used by frequency measurement algorithm
    Slot: 0~8
    AutoMode_UpdateTime = time period for Auto mode, unit: ms 
    LowMode_UpdateTime  = time period for Low Frequency mode, unit: ms
    LowMode_UpdateTime  = time period for High Frequency mode, unit: ms
    */    
         

        
//===================================================================        
//===== Other Functions =============================================
//===================================================================        
int i8080_ReadDI_Xor(int Slot, int *DI);
    /*
    Slot: 0~8
    *DI: Bit0 = DI of A0 after XorControl
    *DI: Bit1 = DI of B0 after XorControl
    ...
    *DI: Bit7 = DI of B3 after XorControl
    
    return  = 0 --> OK
           <> 0 --> error codes
    */   
    
int i8080_ReadDI_XorLPF(int Slot, int *DI);
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

int  i8080_EepWriteEnable(int Slot);
    /*
    Write_Enable EEPROM
    Slot: 0~8
    return 0	 --> OK
    return others --> error codes
    */

int  i8080_EepWriteDisable(int Slot);
    /*
    Write_Disable EEPROM
    Slot: 0~8
    return 0	 --> OK
    return others --> error codes
    */

int  i8080_EepWriteWord(int Slot, int Addr, int Value);
    /*
    write 16-bit data to EEP
    Slot = 0 ~ 8  = Slot Number
    Addr:  0~39  for users 
          40~63  for 8080 configuration
    Value = two bytes integer
    
    return  0 --> OK
           -1 --> Address error
    */


int  i8080_EepReadWord(int Slot, int Addr, int *Value);
    /*
    read 16-bit data from EEP
    Slot: 0~8
    Addr:  0~39  for users 
          40~63  for 8080 configuration
    Value: two bytes integer
    return  0 --> OK
           -1 --> Address error
	*/
