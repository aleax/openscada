/********************************
	i8088W Error Code definition	
*******************************/
#ifdef	__cplusplus  // for C++ compile use
extern "C" {
#endif	/*	__cplusplus	*/

#define OK 		 				        0	
#define ID_ERROR 				     -1
#define SLOT_OUT_RANGE			 -2
#define CHANNEL_OUT_RANGE	 	 -3
#define SELECT_CHANNEL_ERROR -4
#define HI_DUTY_OUT_RANGE		 -5
#define LO_DUTY_OUT_RANGE	   -6

/********************************
	i8088W_Get_LatticeVersion can get the Lattice version 
	return Lattice version	
*******************************/ 
short i8088W_GetFirmwareVersion(int slot);


/********************************
	i8088W_Get_LibVersion can get the version i8080 Lib
	return Lib version	
*******************************/
short i8088W_GetLibVersion(void);
          
  
short i8088W_GetLibDate(char *LibDate);


/********************************
	i8088W_Init can will check the basic condition of i-8088
	1. slot 1 ~ 8 
	return value: please refer to i8088 Error Code definition		
*******************************/ 
short i8088W_Init(int slot);


/********************************
	i8088W_SetPWMDuty is the standard PWM function
	1. slot 1 ~ 8
	2. ch 0 ~ 7
	3. hz 0 ~ 450K 
	4. duty High part 0~99
	5. Low part 100 - High part
	return value: please refer to i8088 Error Code definition
*******************************/
short i8088W_SetPWMDuty(int slot,int ch,unsigned long hz,unsigned int duty);


/********************************
	i8088W_SetPWMDuty_Deci is the more precission PWM function
	1. slot 1 ~ 8
	2. ch 0 ~ 7
	3. deci_Hz example:10 deci_Hz = 1 Hz
	4. deci_duty 0~999, for example: 503 deci_duty = 50.3% High Part
	5. Low Part=1000 - 503= 497 => 49.7% Low Duty cycle
	return value: please refer to i8088 Error Code definition
	
	Note: i8088W_SetPWMDuty_Deci is the same as i8088W_SetPWMDuty_float in usage
	but the i8088W_SetPWMDuty_Deci will run more fast than i8088W_SetPWMDuty_float
	for the floating calculation reason 
*******************************/
short i8088W_SetPWMDuty_Deci(int slot,int ch,unsigned long deci_Hz,unsigned int deci_duty);


/********************************
	i8088W_SetPWMDuty_float is the more precission PWM function
	1. slot 1 ~ 8
	2. ch 0 ~ 7
	3. f_Hz 10 f_Hz = 1 Hz
	4. f_Duty 0.0~99.9, for example: 50.3 means 50.3% High Part
	5. Low Part =100.0- f_Duty = 49.7 means = 49.7% 
	return value: please refer to i8088 Error Code definition
	
	Note: i8088W_SetPWMDuty_Deci is the same as i8088W_SetPWMDuty_float in usage
	but the i8088W_SetPWMDuty_Deci will run more fast than i8088W_SetPWMDuty_float
	for the floating calculation reason 
*******************************/
short i8088W_SetPWMDuty_Float(int slot,int ch,float f_Hz,float f_Duty);


/********************************
	SetPWMDutyCount is the basic PWM function
	1. slot 1 ~ 8
	2. ch 0 ~ 7
	3. highDutyCount 0 ~ 65536*128 us
	4. lowDutyCount 0 ~ 65536*128 us
	return value: please refer to i8088 Error Code definition
						 ________________	
	Note: PWM Duty Count |			     |_______________|
						  0~65536*128 us   0~65536*128 us
*******************************/
short i8088W_SetPWMDutyCount(int slot,int ch,unsigned long highDutyCount,unsigned long lowDutyCount);


/********************************
	i8088W_SetPWMCountMode to set the counting mode of i-8088
	1. slot 1 ~ 8
	2. ch 0 ~ 7
	3. countMode 1: Continuos ;0: Burst count
	return value: please refer to i8088 Error Code definition
	
	Note: if i8088W_SetPWMChannelMode set as Burst count mode 0, it must use i8088W_SetBurstCount to define the pulse counts
*******************************/
short i8088W_SetPWMCountMode(int slot,int ch,unsigned char countMode);


/********************************
	i8088W_SetBurstCount to set the counting mode of i-8088
	1. slot 1 ~ 8
	2. ch 0 ~ 7
	3. burstCount 0~65536
*******************************/
short i8088W_SetBurstCount(int slot,int ch,unsigned int burstCount);


/********************************
	i8088W_PWM_Start to start the PWM pulse
	1. slot 1 ~ 8
	2. ch 0 ~ 7
*******************************/
short i8088W_PWM_Start(int slot,int ch);



/********************************
	i8088W_PWM_Stop to stop the PWM pulse
	1. slot 1 ~ 8
	2. ch 0 ~ 7
*******************************/
short i8088W_PWM_Stop(int slot,int ch);


/********************************
	i8088W_SetSyncChannel can set the specific channel as synchrous .
	1. slot 1 ~ 8
	2. ch 0 ~ 7
	3. enBit 1: define channel as synchrous mode, 0:not synchrous mode
	return value: please refer to i8088 Error Code definition
*******************************/  
short i8088W_SetSyncChannel(int slot,int ch,int enBit);


/********************************
	i8088W_GetSyncChannel can get the specific synchrous channel as bit array.
	1. slot 1 ~ 8
	2. syncArr[] is an 8 bit array
	3. syncArr[i]=1 means channel[i] is set as synchrous
	return value: please refer to i8088 Error Code definition
*******************************/ 
short i8088W_GetSyncChannel(int slot,int syncArr[]);


/********************************
	i8088W_SetSyncStart to start the synchrous PWM pulse
	1. slot 1 ~ 8
*******************************/
short i8088W_Sync_Start(int slot);


/********************************
	i8088W_SetSyncStop to stop the synchrous PWM pulse
	1. slot 1 ~ 8
*******************************/
short i8088W_Sync_Stop(int slot);


/********************************
	HardwareTrigChannel use the same pin as DI, the DI pin can be set as hardware trigger pin or simply DI pin.
	if user call i8088W_SetHardwareTrigChannel function then the assigned channel will be acted as hardware trigger pin
	1. slot 1 ~ 8
	2. ch 0 ~ 7
	3. enBit =1 means ch have been set as hardware start trigger
	4. enBit =0 means ch have been set as hardware stop trigger	
	return value: please refer to i8088 Error Code definition
*******************************/
short i8088W_SetHardwareTrigChannel(int slot,int ch,int enBit);


/********************************
	HardwareTrigChannel use the same pin as DI, the DI pin can be set as hardware trigger pin or simply DI pin.
	1. slot 1 ~ 8
	2. startArr[] is an 8 bit array,when startArr[i]=1 means channel[i] have been set as hardware trigger
				  and when startArr[i]=1 then channel[i] begins to send PWM pulse 
				  if startArr[i]=0, it means channel[i] is not defined as start flag 
	3. stopArr[] is an 8 bit array,when stopArr[i]=1 means channel[i] have been set as hardware trigger
				  and when stopArr[i]=1 then channel[i] stop to send PWM pulse,
				  if stopArr[i]=0, it means channel[i] is not defined as stop flag 
	return value: please refer to i8088 Error Code definition
*******************************/ 
//short i8088W_GetHardwareTrigChannel(int slot,int startArr[],int stopArr[]);
short i8088W_GetHardwareTrigChannel(int slot,int ch,int triggerState[]);


/********************************
	To Get DI status
	1. slot 1 ~ 8
	2. *diVal the DI status of i-8088 value 0~0xff
	3. diArr[], the *DI value will be parse into Bit Array
	return value: please refer to i8088 Error Code definition
*******************************/
short i8088W_GetDI(int slot,unsigned int *diVal,int diArr[]);

short i8088W_GetPWMActiveState(int slot,unsigned int *State,int ActiveArr[]);
        
#ifdef	__cplusplus
}
#endif	/*	__cplusplus	*/
