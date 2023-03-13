/********************************************************************/

/*  Header file Name:pac_i8048W.h                                               
    
//  Match lib;pac_i8048W.lib
  
//  Match Dll:pac_i8048W.Dll                                                   

//  [Abstract]:                                                               

//    This file defines pac_i8048W.Dll functions, types and definitions.                                                          

//    These functions be used to control the I-8048W module              

//    on the WinPAC/ViewPAC/XPAC series.

//  [Caution]:                                                       

//     Please use the functions in the pac_i8048W.h to control  
       
//     the I-i8048W module .                                                          

********************************************************************/
/*  History                                                         */
/*
 Ver 1.0.2 [Oct.21,2011] Sean
  Add pac_i8048W_UnInstallISR function
 
 Ver 1.0.0 [July 7,2009]
   Created by Sean
*/   
/********************************************************************/
#ifdef PAC_I8048W_EXPORTS
	#define I8048W_API __declspec(dllexport) 
#else
	#define I8048W_API __declspec(dllimport) 
#endif

#define NoError		0
#define ID_ERROR	-1

#ifdef __cplusplus
extern "C" {
#endif


I8048W_API short I8048W_GetLibVersion(void);
/* function to get the library version of i-8048W
//
//	      return: version number. 
//        for example: 0x106; = Rev:1.0.6
*/

I8048W_API void I8048W_GetLibDate(char libDate[]);
/* function to get the library built date
//
//	      libDate: library built date 
//		    return None
*/

I8048W_API int I8048W_Init(int Slot);           
/*
 Initialize the i8048W
 slot:
     (1~7) for LinPAC series
 Return:
     0: success
     other: failure         
*/              

I8048W_API void I8048W_UnFreezeINT(int Slot,int Channel);
/*
 Clear interrupted status of an i-8048w channel
 slot:
    (1~7) for LinPAC series
 Channel: 
    0~7
 Return:
    None  

Note:
  If the interrupt of an i-8048w channel does not clear to LOW, 
  then this channel interrupts will be blocked and the CPU will not 
  be able to receive any further interrupts.
*/                   

I8048W_API short I8048W_InstallISR(int slot,void (*isr)(int slot),int Priority);
/*
  Intsall a slot interrupt service route for i-8048W with the thread priority
 slot:
  (1~7) for LinPAC series
 f:
  A callback function.
 Priority:
  Priority to set for the thread, This value can range from 0 through 255, 
   with 0 as the highest priority.
 Return:  
  None 
*/

I8048W_API void I8048W_UnInstallISR(int Slot);
/*
  Uninstall a slot interrupt service route and diables a hardware interrupt.
 slot:
  (1~7) for LinPAC series
 Return:    
  None
*/

I8048W_API int I8048W_Read_RisingEvent(int Slot,int Channel);               
/*
 Read rising interrupt status of an i-8048W channel
   slot:
    (1~7) for LinPAC series
 Channel: 
    0~7
 Return:
    1: Rising interrupt occur  
    0: No rising interrupt occur  
*/ 

I8048W_API int I8048W_Read_FallingEvent(int Slot,int Channel);              
/*
 Read falling interrupt status of an i-8048W channel
   slot:
    (1~7) for LinPAC series
 Channel: 
    0~7
 Return:
    1: falling interrupt occur  
    0: No falling interrupt occur  
*/ 

I8048W_API DWORD I8048W_Read_RisingEventCount(int Slot,int Channel);  
 /*
 Read total count values of the rising interrupt occured on an i-8048W channel
   slot:
    (1~7) for LinPAC series
 Channel: 
    0~7
 Return:
    Rising interrupt count value     
*/ 
I8048W_API DWORD I8048W_Read_FallingEventCount(int Slot,int Channel); 
 /*
 Read total count values of the falling interrupt occured on an i-8048W channel
   slot:
    (1~7) for LinPAC series
 Channel: 
    0~7
 Return:
    falling interrupt count value     
*/ 
I8048W_API void I8048W_Clear_RisingEventCount(int Slot,int Channel);
  /*
  Clear total count values of the rising interrupt occured on an i-8048W channel
   slot:
    (1~7) for LinPAC series
 Channel: 
    0~7
 Return:
    None    
*/ 
I8048W_API void I8048W_Clear_FallingEventCount(int Slot,int Channel);
 /*
 Clear total count values of the falling interrupt occured on an i-8048W channel
   slot:
    (1~7) for LinPAC series
 Channel: 
    0~7
 Return:
    None    
*/
I8048W_API void I8048W_Set_RisingReg(int Slot,int Channel,int Enable);       
/*
  Enable/Disable the rising interrupt of an i-8048W channel
   slot:
    (1~7) for LinPAC series
 Channel: 
    0~7
 Enable:
  1: Enable
  0: Disable
 Return:
    None    
*/ 
I8048W_API void I8048W_Set_FallingReg(int Slot,int Channel,int Enable);
/*
  Enable/Disable the falling interrupt of an i-8048W channel
  Slot:
    (1~7) for LinPAC series
 Channel: 
    0~7
 Enable:
  1: Enable
  0: Disable
 Return:
    None    
*/ 
I8048W_API int I8048W_Read_RisingReg(int Slot,int Channel);                 
 /*
 Read the rising interrupt setting status of an i-8048W channel
   slot:
    (1~7) for LinPAC series
 Channel: 
    0~7
 Return:
    1: Enable
    0: Disable     
*/ 
I8048W_API int I8048W_Read_FallingReg(int Slot,int Channel);                
 /*
 Read the falling interrupt setting status of an i-8048W channel
   slot:
    (1~7) for LinPAC series
 Channel: 
    0~7
 Return:
    1: Enable
    0: Disable     
*/ 

I8048W_API int I8048W_DI_ALL(int Slot);                       
/*
 Reads the DI values of all channels of an i-8048W
 Slot:
    (1~7) for LinPAC series
 Return:
    DI value  
 
    isolated:
    +---------------------------+--------------+--------------+-    
    |JPn selected isolated input|Digital Input |     LED      |     
    +---------------------------+--------------+--------------+-    
    |Input is OPEN              |     High     |     OFF      |     
    +---------------------------+--------------+--------------+     
    |Input is 3.5 ~ 30V         |     Low      |     ON       |     
    +---------------------------+--------------+--------------+     
    |Input is 0 ~ 1V            |     High     |     OFF      |     
    +---------------------------+--------------+--------------+     
    
    TTL:
    +---------------------------+--------------+--------------+-        
    |JPn selected TTL input     |Digital Input |     LED      |         
    +---------------------------+--------------+--------------+-        
    |Input is OPEN              |     High     |     OFF      |         
    +---------------------------+--------------+--------------+         
    |Input is 0 ~ 0.8V          |     Low      |     ON       |         
    +---------------------------+--------------+--------------+         
    |Input is 2 ~ 5V            |     High     |     OFF      |         
    +---------------------------+--------------+--------------+         

*/
I8048W_API int I8048W_DI_Ch(int Slot,int Channel);
//Read single DI channel status
/*
 Reads single DI channel status of an i-8048W
 Slot:
    (1~7) for LinPAC series
 Channel: 
    0~7    
 Return:
    1: DI ON
    0: DI OFF  
*/

#ifdef __cplusplus
 }
#endif

