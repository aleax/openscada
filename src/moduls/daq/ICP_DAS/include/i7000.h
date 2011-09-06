#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32_WCE //Provided for WinCon
	#define Exp7K I7000CE_API
//	#define CALLBACK ""
#else//Provided for PC (Microsoft OS)
	#ifndef Exp7K
		#ifdef __cplusplus
			#define Exp7K extern "C" __declspec(dllimport)
		#else
			#define Exp7K __declspec(dllexport)
		#endif
	#endif
#endif

#ifdef _WIN32_WCE
//	#ifndef __I7000_H__
//	#define __I7000_H__
	#include <I7000CE.h>
#endif

// return code
#define    NoError                      0
#define    FunctionError                1
#define    PortError                    2
#define    BaudRateError                3
#define    DataError                    4
#define    StopError                    5
#define    ParityError                  6
#define    CheckSumError                7
#define    ComPortNotOpen               8
#define    SendThreadCreateError        9
#define    SendCmdError                 10
#define    ReadComStatusError           11
#define    ResultStrCheckError          12
#define    CmdError                     13
#define    TimeOut                      15
#define    ModuleIdError                17
#define    AdChannelError               18
#define    UnderInputRange              19
#define    ExceedInputRange             20
#define    InvalidateCounterNo          21
#define    InvalidateCounterValue       22
#define    InvalidateGateMode           23
#define    InvalidateChannelNo          24
#define    ComPortInUse                 25

Exp7K short CALLBACK Short_Sub_2(short nA, short nB); //for bcb demo01
Exp7K float CALLBACK Float_Sub_2(float fA, float fB); //for bcb demo01
Exp7K DWORD CALLBACK FloatToHex(float fv);
Exp7K WORD  CALLBACK Get_Dll_Version(void);
// 0x0502: Modify AnalogIn8, AnalogInAll_8K, AnalogInAll_87K,
Exp7K WORD CALLBACK Test(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);

Exp7K WORD CALLBACK HostIsOK(WORD wBuf[], float fBuf[],
      char szSend[], char szReceive[]);

/*--- Read the module reset status, $AA5 --------*/
Exp7K WORD CALLBACK ReadModuleResetStatus(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -----------------------------------
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // don't care
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ----------------------------------
//  wBuf[5];   // wBuf[5]=0: module has not been reset
               // wBuf[5]=1: module has been reset

/*--- To setup host watchdog, ~AA3ETT --------*/
Exp7K WORD CALLBACK ToSetupHostWatchdog(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -----------------------------------
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // don't care
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[5];   // 0: Disable host watchdog
               // 1: Enable host watchdog
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string
//  wBuf[7];   // a time interval for watchdog, unit is 0.1 second,
               // e.x  when wBuf[7]=45, the time interval is 4.5 second

/*--- To read host watchdog setup value, ~AA2 --------*/
Exp7K WORD CALLBACK ToReadHostWatchdog(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -----------------------------------
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // don't care
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output -----------------------------------
//  wBuf[5];   // 0: Disable host watchdog
               // 1: Enable host watchdog
//  wBuf[7];   // a time interval for watchdog, unit is 0.1 second,
               // e.x  when wBuf[7]=45, the time interval is 4.5 second




/*--- Read the module status about the host watchdog, ~AA0 --------*/
Exp7K WORD CALLBACK ReadModuleHostWatchdogStatus(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -----------------------------------
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // don't care (for all module)
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ----------------------------------
//  wBuf[5];   // wBuf[5]=0:  module is OK
               // wBuf[5]=4:  host watchdog failure

/*--- Reset the module status about the host watchdog, ~AA1 --------*/
Exp7K WORD CALLBACK ResetModuleHostWatchdogStatus(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -----------------------------------
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // don't care
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[5];   // don't care
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string



/*--Set SafeValue for all DO module(42/43/44/50/60/65/66/67) ---*/
Exp7K WORD CALLBACK SetSafeValueForDo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input ---------------------------
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // module ID: 0x7050/60/65/66/67/42/43/44
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[5];   // safe value
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string


/*--Set PowerOn value for all DO module(42/43/44/50/60/65/66/67) ---*/
Exp7K WORD CALLBACK SetPowerOnValueForDo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // module ID: 0x7050/60/65/66/67/42/43/44
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[5];   // PowerOn value
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string

/*--Set SafeValue for all Ao module(7021/22/24), ~AA5 or ~AA5N ---*/
Exp7K WORD CALLBACK SetSafeValueForAo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input ---------------------------
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // module ID: 0x7021/0x7022/0x7024
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[5];   // Not used if module ID is 7021
			   // Channel No.(0 to 1) if module ID is 7022
               // Channel No.(0 to 3) if module ID is 7024
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string
//  fBuf[0];   // safe value

/*--Read SafeValue for all Ao module(7021/22/24), ~AA4 or ~AA4N ---*/
Exp7K WORD CALLBACK ReadSafeValueForAo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input ---------------------------
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // module ID: 0x7021/0x7022/0x7024
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[5];   // Not used if module ID is 7021
			   // Channel No.(0 to 1) if module ID is 7022
               // Channel No.(0 to 3) if module ID is 7024
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ---------------------------
//  fBuf[0];   // safe value

/*--Set PowerOn value for all Ao module(0x7021/22/24), $AA4 or $AA4N ---*/
Exp7K WORD CALLBACK SetPowerOnValueForAo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // module ID: 0x7021/0x7022/0x7024
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[5];   // Not used if module ID is 7021
			   // Channel No.(0 to 1) if module ID is 7022
               // Channel No.(0 to 3) if module ID is 7024
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string
//  fBuf[0];   // PowerOn value

/*--Read PowerOn Value for all Ao module(7024), $AA7N ---*/
/*** Notice: This function is valid for 7024 only ***/
Exp7K WORD CALLBACK ReadPowerOnValueForAo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input ---------------------------
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // module ID: 0x7024
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[5];   // Channel No.(0 to 3) if module ID is 7024
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ---------------------------
//  fBuf[0];   // safe value

/*-------- 7011/7012/7014 Set PowerOn/Safe value --------*/
Exp7K WORD CALLBACK SetPowerOnSafeValue(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//  wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//  wBuf[1];   // module address: 0x00 to 0xFF
//  wBuf[2];   // module ID: 0x7011/12/14
//  wBuf[3];   // checksum: 0=disable, 1=enable
//  wBuf[4];   // TimeOut constant: normal=100
//  wBuf[5];   // PowerOn value
//  wBuf[6];   // flag: 0=no save, 1=save send/receive string
//  wBuf[7];   // safe value

/*--Read PowerOn value for all DO module(42/43/44/50/60/65/66/67) ---*/
Exp7K WORD CALLBACK ReadPowerOnValueForDo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input ---------------------------
// wBuf[0];    // RS-232 port number: 1/2/3/4..../255
// wBuf[1];    // module address: 0x00 to 0xFF
// wBuf[2];    // module ID: 0x7050/60/65/66/67/42/43/44
// wBuf[3];    // checksum: 0=disable, 1=enable
// wBuf[4];    // TimeOut constant: normal=100
// wBuf[6];    // flag: 0=no save, 1=save send/receive string
//---------------------- output -----------------------
//   wBuf[5]:   PowerOn Value Value


/*--Read SafeValue for all DO module(42/43/44/50/60/65/66/67) ---*/
Exp7K WORD CALLBACK ReadSafeValueForDo(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input ---------------------------
// wBuf[0];    // RS-232 port number: 1/2/3/4..../255
// wBuf[1];    // module address: 0x00 to 0xFF
// wBuf[2];    // module ID: 0x7050/60/65/66/67/42/43/44
// wBuf[3];    // checksum: 0=disable, 1=enable
// wBuf[4];    // TimeOut constant: normal=100
// wBuf[6];    // flag: 0=no save, 1=save send/receive string
//---------------------- output -----------------------
//   wBuf[5]:   Safe Value Value


/*-------- Read Config Status ---------*/
Exp7K WORD CALLBACK  ReadConfigStatus(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];   // RS-232 port number: 1/2/3/4..../255
// wBuf[1];   // module address: 0x00 to 0xFF
// wBuf[2];   // module ID (for all modules)
// wBuf[3];   // checksum: 0=disable, 1=enable
// wBuf[4];   // TimeOut constant: normal=100
// wBuf[5];   // Don't Care
// wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- Output -------------------------------------
// wBuf[7]:   module address
// wBuf[8]:   module Range Code
// wBuf[9]:   module baudrate
// wBuf[10]:  module data format


Exp7K WORD CALLBACK AnalogIn(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7011/12/13/14/16/17/18/19/33
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // channel number for multi-channel
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // fBuf[0]=analog input value

Exp7K WORD CALLBACK AnalogInFsr(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7011/12/13/14/17/18/19/33
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // channel number for multi-channel
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // fBuf[0]=analog input value in %FSR format


Exp7K WORD CALLBACK AnalogInHex(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7011/12/13/14/17/18/19/33
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // channel number for multi-channel
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // wBuf[7] : analog input value



Exp7K WORD CALLBACK AnalogIn8(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7017/18/19
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // fBuf[0]=analog input value of channel_0
                         // fBuf[1]=analog input value of channel_1
                         // ........................................
                         // fBuf[7]=analog input value of channel_7

Exp7K WORD CALLBACK AnalogInAll(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7017/18/19/33
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
// wBuf[7];				// Data Foramt, 0:Engineer 1:FSR  2:Hex
//---------------------- output ------------------------------------
                         // fBuf[0]=analog input value of channel_0
                         // fBuf[1]=analog input value of channel_1
                         // ........................................
                         // fBuf[7]=analog input value of channel_7

Exp7K WORD CALLBACK In8_7017(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module Type: 08/09/0A/0B/0C/0D
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // fBuf[0]=analog input value of channel_0
                         // fBuf[1]=analog input value of channel_1
                         // ........................................
                         // fBuf[7]=analog input value of channel_7


Exp7K WORD CALLBACK AnalogOut(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];   // RS-232 port number: 1/2/3/4..../255
// wBuf[1];   // module address: 0x00 to 0xFF
// wBuf[2];   // module ID: 0x7016/21/22/24
// wBuf[3];   // checksum: 0=disable, 1=enable
// wBuf[4];   // TimeOut constant: normal=100
// wBuf[5];   // Not used if module ID is 7016/7021
			  // Channel No.(0 to 1) if module ID is 7022
              // Channel No.(0 to 3) if module ID is 7024
// wBuf[6];   // flag: 0=no save, 1=save send/receive string
// fBuf[0];   // analog output value
//---------------------- output ------------------------------------
                           // void
Exp7K WORD CALLBACK AnalogOutHex(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];   // RS-232 port number: 1/2/3/4..../255
// wBuf[1];   // module address: 0x00 to 0xFF
// wBuf[2];   // module ID: 0x7021/21P/22
// wBuf[3];   // checksum: 0=disable, 1=enable
// wBuf[4];   // TimeOut constant: normal=100
// wBuf[5];   // Not used if module ID is 7021
			  // Channel No.(0 to 1) if module ID is 7022                           
// wBuf[6];   // flag: 0=no save, 1=save send/receive string
// wBuf[7];   //analog output value(Hexadeciaml Data format)
//---------------------- output ------------------------------------
Exp7K WORD CALLBACK AnalogOutFsr(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];   // RS-232 port number: 1/2/3/4..../255
// wBuf[1];   // module address: 0x00 to 0xFF
// wBuf[2];   // module ID: 0x7021/21P/22
// wBuf[3];   // checksum: 0=disable, 1=enable
// wBuf[4];   // TimeOut constant: normal=100
// wBuf[5];   // Not used if module ID is 7021
			  // Channel No.(0 to 1) if module ID is 7022                           
// wBuf[6];   // flag: 0=no save, 1=save send/receive string
// fBuf[0];   // analog output value(% of Span data format)
//---------------------- output ------------------------------------

Exp7K WORD CALLBACK AnalogOutReadBack(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];  // RS-232 port number: 1/2/3/4..../255
// wBuf[1];  // module address: 0x00 to 0xFF
// wBuf[2];  // module ID: 0x7021 or 0x7024
// wBuf[3];  // checksum: 0=disable, 1=enable
// wBuf[4];  // TimeOut constant: normal=100
// wBuf[5];  // When 7016: Don't care
             // When 7021/7022/7024
             // 0: command read back ($AA6)
             // 1: when module ID is 0x7021 or 0x7022
             // analog output of current path read back ($AA8)
             // when module ID is 0x7024
             //       the updating value in a specific Slew rate($AA8)
             //       (see 7024 manual for more detail information)
// wBuf[6];  // flag: 0=no save, 1=save send/receive string
// wBuf[7];  // Channel No.(0 to 3), if module ID is 7024
			 // Channel No.(0 to 1), if module ID is 7022	
             // Not used,        else
//---------------------- output ------------------------------------
// fBuf[0];  //analog output value read back

Exp7K WORD CALLBACK AnalogOutReadBackHex(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//   wBuf[0];   		 RS-232 port number: 1/2/3/4/..../255
//   wBuf[1];			 module address: 0x00 to 0xFF
//   wBuf[2];            module ID: 0x7021/21P/22
//   wBuf[3];			 checksum: 0=disable, 1=enable
//   wBuf[4];			 TimeOut constant: normal=100
//   wwBuf[5];			 for 7021/7022
                         //   0: command read back ($AA6)
                         //   1: analog output of current path read back ($AA8)
//   wBuf[6];			 flag: 0=no save, 1=save send/receive string
//   wBuf[7];		     Channel No.(0 to 1),for 7022                          
//---------------------- output ------------------------------------
//   wBuf[9];            //analog output value read back(Hex)

Exp7K WORD CALLBACK AnalogOutReadBackFsr(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
   //---------------------- input -------------------------------------
   // wBuf[0];          // RS-232 port number: 1/2/3/4/..../255
   // wBuf[1];          // module address: 0x00 to 0xFF
   // wBuf[2];          // module ID: 7021/7022 (7024/7016 have no Fsr mode)
   // wBuf[3];			// checksum: 0=disable, 1=enable
   // wBuf[4];          // TimeOut constant: normal=100
   // wBuf[5];          // for 7021/7022
                        //     0: command read back ($AA6)
                        //     1: analog output of current path read back ($AA8)
   // wBuf[6];			// flag: 0=no save, 1=save send/receive string
   // wBuf[7];          // Channel No.(0 to 3)
   //---------------------- output ------------------------------------
   // fBuf[0];		    // analog output value read back     

Exp7K WORD CALLBACK DigitalOut(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7050/60/63/65/66/67/42/43/44/11/12/14/80
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // 16-bit digital data to output
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // void

Exp7K WORD CALLBACK DigitalBitOut(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7050/60/63/65/66/67/42/43/44
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // don't care
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
// wBuf[7];             // which channel to output
// wBuf[8];             // data to output (0 or 1)
//---------------------- output ------------------------------------
                         // void

Exp7K WORD CALLBACK DigitalOut_7016(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7016
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // 2-bit digital data to output
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
// wBuf[7];             // 0: Bit0, Bit1 output
                        // 1: Bit2, Bit3 output
//---------------------- output ------------------------------------
                         // void

Exp7K WORD CALLBACK DigitalOutReadBack(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7050/60/66/67/42/43/44
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // wBuf[5]=digital output data read back


Exp7K WORD CALLBACK DigitalIn(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7050/52/53/58/60/63/65/41/44/11/12/14/16
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // wBuf[5]=digital input data

Exp7K WORD CALLBACK DigitalInCounterRead(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7050/52/58/53/60/63/65/41/44
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // Channel Number
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
// wBuf[7]=Counter Value of Channel N's Digital Input


Exp7K WORD CALLBACK ClearDigitalInCounter(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7050/52/53/58/60/63/65/41/44
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // Channel Number
// wBuf[6];             // flag: 0=no save, 1=save send/receive string

Exp7K WORD CALLBACK DigitalInLatch(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7050/52/53/58/60/63/65/41/44
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // 0: Latch Low,   1: Latch High
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
// wBuf[7];             // latch value


Exp7K WORD CALLBACK ClearDigitalInLatch(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7050/52/53/58/60/63/65/41/44
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // Don't care
// wBuf[6];             // flag: 0=no save, 1=save send/receive string

/*-------- 7011 Thermocouple Open Detection ---------*/
Exp7K WORD CALLBACK ThermocoupleOpen_7011(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7011
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
//wBuf[5]   0: Close     1: Open


/*----- 7011/7012/7014 Enable Alarm ---------*/
Exp7K WORD CALLBACK EnableAlarm(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7011/7012/7014
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100  , 0.1 second
//wBuf[5];   // 0: Momentary,  1: Latch
//wBuf[6];   // flag: 0=no save, 1=save send/receive string


/*----- 7011/7012/7014 Disable Alarm ---------*/
Exp7K WORD CALLBACK DisableAlarm(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7011/0x7012/0x7014
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100  , 0.1 second
//wBuf[5];   don't care
//wBuf[6];   // flag: 0=no save, 1=save send/receive string


/*-------- 7011/7012/7014/7016 Clear Latch Alarm ---------*/
Exp7K WORD CALLBACK ClearLatchAlarm(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7011/7012/7014/7016
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100  , 0.1 second
//wBuf[5];   don't care
//wBuf[6];   // flag: 0=no save, 1=save send/receive string


/*--- 7011/7012/7014/7016 Set Hi/Lo Alarm Limit Value ------*/
/*******************************************************/
/* please get the module's range code by calling       */
/* ReadConfigStatus(.......) function before           */
/* calling SetAlarmLimitValue(......).                 */
/*******************************************************/
Exp7K WORD CALLBACK SetAlarmLimitValue(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7011/7012/7014/7016
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100  , 0.1 second
//wBuf[5];   // 0: Lo Alarm     1: Hi Alarm
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//wBuf[7];   // range code of this module
//fBuf[0]:   // Alarm Limit Value

/*-------- 7011/7012/7014/7016 Read Hi/Lo Alarm Limit Value ---------*/
Exp7K WORD CALLBACK ReadAlarmLimitValue(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input ----------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 7011/7012/7014/7016
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100  , 0.1 second
//wBuf[5];   // 0: Lo Alarm     1: Hi Alarm
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ---------------------------------
//fBuf[0]:   // Alarm Limit Value

/*---- Read Alarm State and Digital Output State ---------*/
Exp7K WORD CALLBACK ReadOutputAlarmState(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID :0x7011/12/14/16
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // Don't Care
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
//               Alarm State:
// wBuf(7):0       Disable
// wBuf(7):1       Momentary
// wBuf(7):2       Latch
//               Digital Output
// wBuf(8)    -> a decimal number to represent the digital output status


/*-------- 7011/7012/7014 Read Event Counter --------*/
Exp7K WORD CALLBACK ReadEventCounter(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID:0x7011/12/14
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // Don't Care
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
//wBuf[7];   // The Event Counter Value

/*-------- 7011/7012/7014/7016 Clear Event Counter --------*/
Exp7K WORD CALLBACK ClearEventCounter(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // Don't Care
//wBuf[6];   // flag: 0=no save, 1=save send/receive string

/*-------- Set LED channel Display for 7033 --------*/
Exp7K WORD CALLBACK SetLedDisplay_7033(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID:0x7033
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // set display channel for 7033, 0<=channel<=2
//wBuf[6];   // flag: 0=no save, 1=save send/receive string

/*-------- Get LED channel Display for 7033 --------*/
Exp7K WORD CALLBACK GetLedDisplay_7033(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID:0x7033
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//----------------------- output -----------------------------------
//wBuf[5];   // which channel display

/**********************************************************/
/*-------- Set LED channel Display for 7016, 7033 --------*/
Exp7K WORD CALLBACK SetLedDisplay(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // set display channel for 7013, 7033,7016
//wBuf[6];   // flag: 0=no save, 1=save send/receive string

/*-------- Get LED channel Display for 7016, 7033 --------*/
Exp7K WORD CALLBACK GetLedDisplay(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//----------------------- output -----------------------------------
//wBuf[5];   // which channel display


/*******************************************************/
/* please get the module's range code by calling       */
/* ReadConfigStatus(.......) function before           */
/* calling SetupLinearMapping(......).                 */
/*-------- setup linear mapping for 7014/7016 ---------*/
/*  making a linear mapping from [a,b] to [c,d]        */
/*  where:  fBuf[0]=a,  fBuf[1]=b                    */
/*          fBuf[2]=c,  fBuf[3]=d                    */
/*******************************************************/
Exp7K WORD CALLBACK SetupLinearMapping(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7014, 0x7016
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100  , 0.1 second
//wBuf[5];   // do't care
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//wBuf[7];   // range code of this module
//fBuf[0]:   // source low value,  a
//fBuf[1];   // source high value, b
//fBuf[2];   // target low value,  c
//fBuf[3];   // target high value, d

/*----- Enable linear mapping for 7014/7016 ---------*/
Exp7K WORD CALLBACK EnableLinearMapping(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7014, 0x7016
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100  , 0.1 second
//wBuf[5];   don't care
//wBuf[6];   // flag: 0=no save, 1=save send/receive string

/*----- Disable linear mapping for 7014/7016 ---------*/
Exp7K WORD CALLBACK DisableLinearMapping(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7014, 0x7016
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100  , 0.1 second
//wBuf[5];   don't care
//wBuf[6];   // flag: 0=no save, 1=save send/receive string

/*----- Reading Linear Mapping Status for 7014/7016 -----*/
Exp7K WORD CALLBACK ReadLinearMappingStatus(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7014, 0x7016
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100  , 0.1 second
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//--------------------- output -------------------------------------
//wBuf[5];   // 0: linear mapping is disable
              // 1: linear mapping is enable

/*----- Reading Source Value of Linear Mapping for 7014/7016 -----*/
Exp7K WORD CALLBACK ReadSourceValueOfLM(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7014, 0x7016
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100  , 0.1 second
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//--------------------- output -------------------------------------
//fBuf[0];   // Low Source Value
//fBuf[1];   // High Source Value

/*----- Reading Target Value of Linear Mapping for 7014/7016 -----*/
Exp7K WORD CALLBACK ReadTargetValueOfLM(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7014, 0x7016
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100  , 0.1 second
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//--------------------- output -------------------------------------
//fBuf[0];   // Low Target Value
//fBuf[1];   // High Target Value

/**************************************************************************/
Exp7K WORD CALLBACK CounterIn_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // 0: Counter 0,  1: Counter 1
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // wBuf[7]  Hi-Word of counter value
                         // wBuf[8]  Lo-Word of counter value


Exp7K WORD CALLBACK ReadCounterMaxValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // 0: Counter 0,  1: Counter 1
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // wBuf[7]  Hi-Word of counter value
                         // wBuf[8]  Lo-Word of counter value


Exp7K WORD CALLBACK SetCounterMaxValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[], double MaxValue);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // Don't Care
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
// MaxValue    Setting the Counter's Max Value


Exp7K WORD CALLBACK ReadAlarmLimitValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // 0: Counter 0,  1: Counter 1
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
// wBuf[7]  Hi-Word of counter value
// wBuf[8]  Lo-Word of counter value


Exp7K WORD CALLBACK SetAlarmLimitValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[], double AlarmValue);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // when in 7080 alarm mode(mode 0)
//                                      0:   To set Counter 0 alarm value
//                              1:   To set Counter 1 alarm value
//                               // when in 7080D alarm mode(mode 0)
//                                      0:   To set Counter 0 high alarm value
//                                      1:   To set Counter 0 high-high alarm value
// wBuf[6];             // flag: 0=no save, 1=save send/receive string


Exp7K WORD CALLBACK EnableCounterAlarm_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // 0: counter 0 enable, 1: counter 1 enable
// wBuf[6];             // flag: 0=no save, 1=save send/receive string

Exp7K WORD CALLBACK DisableCounterAlarm_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // Don't care
// wBuf[6];             // flag: 0=no save, 1=save send/receive string


Exp7K WORD CALLBACK EnableCounterAlarm_7080D(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // 0: momentary, 1: latch
// wBuf[6];             // flag: 0=no save, 1=save send/receive string

Exp7K WORD CALLBACK DisableCounterAlarm_7080D(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // Don't care
// wBuf[6];             // flag: 0=no save, 1=save send/receive string


Exp7K WORD CALLBACK ReadCounterStatus_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // 0: Counter 0,  1: Counter 1
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
// wBuf[7]  0: Counting   1: Not Counting

Exp7K WORD CALLBACK ClearCounter_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // 0: Counter 0,  1: Counter 1
// wBuf[6];             // flag: 0=no save, 1=save send/receive string


Exp7K WORD CALLBACK ReadOutputAlarmState_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232  port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // Don't Care
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
   // wBuf(7):0  for 7080 ->  Counter0:Disable   Counter1:Disable
   // wBuf(7):1               Counter0:Disable   Counter1:Enable
   // wBuf(7):2               Counter0:Enable    Counter1:Disable
   // wBuf(7):3               Counter0:Enable    Counter1:Enable
   // wBuf(7):0  for 7080D ->  Counter0:Disable
   // wBuf(7):1                Counter0:Momentary
   // wBuf(7):2                Counter0:Latch
   // wBuf(8):0    Bit:1 Disable  Bit 0: Disable
   // wBuf(8):1    Bit:1 Disable  Bit 0: Enable
   // wBuf(8):2    Bit:1 Enable   Bit 0: Disable
   // wBuf(8):3    Bit:1 Enable   Bit 0: Enable


Exp7K WORD CALLBACK SetInputSignalMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // Counter 0's and 1's signal mode
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- Input -------------------------------------
                    // wBuf(5):0    Counter:0 TTL      Counter:0  TTL
                    // wBuf(5):1    Counter:1 Photo    Counter:1  Photo
                    // wBuf(5):2    Counter:0 TTL      Counter:1  Photo
                    // wBuf(5):3    Counter:1 Photo    Counter:0  TTL


Exp7K WORD CALLBACK ReadInputSignalMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// wBuf[0];             // RS-232 port number: 1/2/3/4..../255
// wBuf[1];             // module address: 0x00 to 0xFF
// wBuf[2];             // module ID: 0x7080
// wBuf[3];             // checksum: 0=disable, 1=enable
// wBuf[4];             // TimeOut constant: normal=100
// wBuf[5];             // Don't Care
// wBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                    // wBuf(7):0    Counter:0 TTL      Counter:0  TTL
                    // wBuf(7):1    Counter:1 Photo    Counter:1  Photo
                    // wBuf(7):2    Counter:0 TTL      Counter:1  Photo
                    // wBuf(7):3    Counter:1 Photo    Counter:0  TTL


/* ------------ 7080 Preset Counter 0/1 Counter Value ---------------- */
Exp7K WORD CALLBACK PresetCounterValue_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[], double PresetValue);

/* ------------ 7080 Read Preset Counter 0/1 Counter Value ------------- */
Exp7K WORD CALLBACK ReadPresetCounterValue_7080(WORD wBuf[], float fBuf[],
                            char szSend[], char szReceive[]);
//---------------------- output ------------------------------------
                      // wBuf[7]= Hi-Word of Preset Counter Value
                      // wBuf[8]= Lo-Word of Preset Counter Value

/*-------- 7080 Start Counting ---------*/
Exp7K WORD CALLBACK StartCounting_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7080
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // 0: Counter 0   1: Counter 1
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//wBuf[7];   // 0: Stop Counting  1: Start Counting


/*-------- Set 7080 module mode  ---------*/
Exp7K WORD CALLBACK SetModuleMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7080
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // 0: 7080 mode   1: 7080D mode
//wBuf[6];   // flag: 0=no save, 1=save send/receive string

/*-------- Read 7080 module mode  ---------*/
Exp7K WORD CALLBACK ReadModuleMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7080
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // don't care
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//------------------ output ---------------------
//wBuf[5]= 0: 7080 mode,  1: 7080D mode


/*-------- 7080 Set Level Volt ---------*/
Exp7K WORD CALLBACK SetLevelVolt_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7080
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // 0: Low Level,  1: High Level
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//fBuf[0];   // Level Voltage


/*-------- 7080 Read Level Volt ---------*/
Exp7K WORD CALLBACK ReadLevelVolt_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input ------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7080
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // 0: Low Level,  1: High Level
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//----------------------- output ----------------------------------
//fBuf[0];        // Level Voltage


/* -------------- 7080 Set Min Signal Width ------------------------- */
Exp7K WORD CALLBACK SetMinSignalWidth_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[], long MinWidth);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7080
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // 0: Low Level,  1: High Level
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//MinWidth    // the Input Signal Min Width

/* -------------- 7080 Read Min Signal Width ------------------------- */
Exp7K WORD CALLBACK ReadMinSignalWidth_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input ----------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7080
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // 0: Low Level,  1: High Level
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- Output ---------------------------------
//wBuf[7]    // the Reading Input Signal Min Width


/* -------------- 7080 Set Gate Mode ------------------------- */
Exp7K WORD CALLBACK SetGateMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7080
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[5];   // 0: Low,  1: High,  2: None
//wBuf[6];   // flag: 0=no save, 1=save send/receive string

/* -------------- 7080 Read Gate Mode ------------------------- */
Exp7K WORD CALLBACK ReadGateMode_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input ---------------------------------
//wBuf[0];   // RS-232 port number: 1/2/3/4..../255
//wBuf[1];   // module address: 0x00 to 0xFF
//wBuf[2];   // module ID: 0x7080
//wBuf[3];   // checksum: 0=disable, 1=enable
//wBuf[4];   // TimeOut constant: normal=100
//wBuf[6];   // flag: 0=no save, 1=save send/receive string
//---------------------- output --------------------------------
//wBuf[5];   // 0: Low,  1: High,  2: None


/*-------- 7080 Send data to LED  ---------*/
Exp7K WORD CALLBACK DataToLED_7080(WORD wBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
//wBuf[0];	// RS-232 port number: 1/2/3/4..../255
//wBuf[1];  // module address: 0x00 to 0xFF
//wBuf[2];  // module ID: 0x7080
//wBuf[3];  // checksum: 0=disable, 1=enable
//wBuf[4];  // TimeOut constant: normal=100
//wBuf[5];  // Don't Care
//wBuf[6];  // flag: 0=no save, 1=save send/receive string
//fBuf[0];  // Data to LED


/*-------- Set configuration for 7080 ---------*/
Exp7K WORD CALLBACK SetConfiguration_7080(WORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//*** Notice: if you change the Baudrate or Checksum, please short the INIT*(pin6) to GND(pin10)
//---------------------- input -------------------------------------
//wBuf[0];	// RS-232 port number: 1/2/3/4..../255
//wBuf[1];  // module original address: 0x00 to 0xFF
//wBuf[2];  // module ID: 0x7080
//wBuf[3];  // module original checksum: 0=disable, 1=enable
//wBuf[4];  // TimeOut constant: normal=100
//wBuf[5];  // desired frequency gate time:
            //      0: 0.1 second
            //      1: 1.0 second
            // Don't care wBuf[5],if set the module into Counter mode
//wBuf[6];  // flag: 0=no save, 1=save send/receive string
//wBuf[7];  // desired new address
//wBuf[8];  // desired Type->  0:Counter mode
            //                 1:Frequency mode
//wBuf[9];  // desired Baudrate:
            //    3: 1200 BPS    4:  2400 BPS
            //    5: 4800 BPS    6:  9600 BPS
            //    7: 19200 BPS   8:  38400 BPS
            //    9: 57600 BPS   10: 115200 BPS
//wBuf[10]; // desired Checksum Address



//#ifdef _WIN32_WCE
//	#endif  //__I7000_H__
//#endif

#ifdef __cplusplus
}
#endif
