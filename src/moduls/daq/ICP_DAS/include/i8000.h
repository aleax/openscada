#ifdef _WIN32_WCE //Provided for WinCon
	#define Exp8K I7000CE_API
//	#define CALLBACK NULL
#else //Provided for PC (Microsoft OS)
	#ifndef Exp8K
		#ifdef __cplusplus
			#define Exp8K extern "C" __declspec(dllimport)
		#else
			#define Exp8K __declspec(dllexport)
		#endif
	#endif
#endif

#ifdef _WIN32_WCE
//	#ifndef __I8000_H__
//	#define __I8000_H__
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


/*******************************************************************/
Exp8K WORD CALLBACK AnalogIn_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID: 0x8017/18
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[5];             // channel number for multi-channel
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string
// dwBuf[7];             // Slot Number: 0 to 3
//---------------------- output ------------------------------------
                         // fBuf[0]=analog input value

/*******************************************************************/
Exp8K WORD CALLBACK AnalogOutReadBack_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID: 0x8017/18
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[5];             // channel number for multi-channel
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string
// dwBuf[7];             // Slot Number: 0 to 3
//---------------------- output ------------------------------------
                         // fBuf[0]=analog out readback value




/************************************************************/
/*  AnalogInAll_8K											*/
/*															*/
/*  Main Unit: I-8000 series								*/
/*															*/
/*  Module: I-87013,I-8017H,I-87017,I-87018					*/
/*          ADAM-5013/17/18									*/
/************************************************************/
Exp8K WORD CALLBACK AnalogInAll_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
/*
====== Input parameter ====================================
 dwBuf[0]; RS-232 port number: 1 ~ 255
 dwBuf[1]; Module address: 0x00 ~ 0xFF
 dwBuf[2]; Module ID:	0x8013 for I-87013,
						0x8017 for I-8017H,I-87017
						0x8018 for I-87018,
						0x5017/18 for ADAM-5017/18
 dwBuf[3]; Checksum: 0=disable, 1=enable
 dwBuf[4]; TimeOut: Normal=100 (ms)
 dwBuf[6]; Debug:	0=disable
					1=enable:	copy command string (sent to module)
								to szSend and response string (received from module)
								to szReceive.
 dwBuf[7]; Slot Number: 0 to 7

====== Output parameter ===================================
 if dwBuf[2] is 0x8013/0x5013
 fBuf[0]=analog input value of channel_0
 fBuf[1]=analog input value of channel_1
 fBuf[2]=analog input value of channel_2
 fBuf[3]=analog input value of channel_3

 if dwBuf[2] is 0x8017/18 or 0x5017/18
 fBuf[0]=analog input value of channel_0
 fBuf[1]=analog input value of channel_1
 ........................................
 fBuf[7]=analog input value of channel_7

*/
            
/************************************************************************/
/*'02/6/24 added------------------------------------------------------- */
Exp8K WORD CALLBACK AnalogInFsr_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];     // module address: 0x00 to 0xFF
// dwBuf[2];     // module ID: 0x8017/0x8018
// dwBuf[3];	 // checksum: 0=disable, 1=enable
// dwBuf[4];	 // TimeOut constant: normal=100
// dwBuf[5];	 // channel number for multi-channel
// dwBuf[6];     // flag: 0=no save, 1=save send/receive string
// dwBuf[7];     // Slot Number


/* -------------------------------------------------------------------- */
Exp8K WORD CALLBACK AnalogInHex_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
  //---------------------- input -------------------------------------
// dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];     // module address: 0x00 to 0xFF
// dwBuf[2];     // module ID: 0x8017/0x8018
// dwBuf[3];	 // checksum: 0=disable, 1=enable
// dwBuf[4];     // TimeOut constant: normal=100
// dwBuf[5];     // channel number for multi-channel
// dwBuf[6];     // flag: 0=no save, 1=save send/receive string
// dwBuf[7];     // Slot Number
//---------------------- output ------------------------------------
// dwBuf[8]      //analog input value

/**************************************/
/*------  Set Alarm Connection--------*/
/*----  for 87013, 87017, 87018  -----*/
Exp8K WORD CALLBACK SetAlarmConnect_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];      // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];      // module address: 0x00 to 0xFF
// dwBuf[2];      // module ID: 0x8017/18
// dwBuf[3];      // checksum: 0=disable, 1=enable
// dwBuf[4];      // TimeOut constant: normal=100
// dwBuf[5];      // channel
// dwBuf[6];      // flag: 0=no save, 1=save send/receive string
// dwBuf[7];      // slot number
// dwBuf[8];      // 0:connect Low Alarm, 1:connect high alarm
// dwBuf[9];      // the desired connect slot, 
//                //  when 100 means to disconnect 
// dwBuf[10];     // the desired connect channel
//---------------------- output --------------------------------
// void

/*********************************************/
/*----     Set Alarm Limit Value    ---------*/
/*----     for 8013, 8017, 8018      -----*/
Exp8K WORD CALLBACK SetAlarmLimitValue_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x8013/17/18
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7];    // slot
// dwBuf[8];    // 0: Low Alarm  1: High Alarm
// f7000[0];    // Alarm Limit Value
//---------------------- output ------------------------------------
// void

/*********************************************/
/*----     Read Alarm Limit Value   ---------*/
/*----     for 8013, 8017, 8018      -----*/
Exp8K WORD CALLBACK ReadAlarmLimitValue_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x87013/17/18
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7];    // slot
// dwBuf[8];    // 0: Low Alarm  1: High Alarm
//---------------------- output ------------------------------------
// fValue=f7000[0];      // Alarm Limit Value

/*********************************************/
/*----       Clear Latch Alarm      ---------*/
/*----     for 8013, 8017, 8018      -----*/
Exp8K WORD CALLBACK ClearLatchAlarm_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x8013/17/18
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7];    // slot
// dwBuf[8];    // 0: Low Alarm  1: High Alarm
//---------------------- output ------------------------------------
// void

/*********************************************/
/*----       Read Alarm Mode        ---------*/
/*----     for 8013, 8017, 8018      -----*/
Exp8K WORD CALLBACK ReadAlarmMode_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x8013/17/18
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7];    // slot
// dwBuf[8];    // 0: Low Alarm  1: High Alarm
//---------------------- output ------------------------------------
//  Alarm Mode:
//    dwBuf(9):0       Disable
//    dwBuf(9):1       Momentary
//    dwBuf(9):2       Latch

/*********************************************/
/*----        Set Alarm Mode        ---------*/
/*----     for 813, 8017, 8018      -----*/
Exp8K WORD CALLBACK SetAlarmMode_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x8013/17/18
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7];    // slot
// dwBuf[8];    // 0: Low Alarm  1: High Alarm
// dwBuf(9):0   // 0: Disable   1: Mentary   2: Latch
//---------------------- output -----------------------------------
// void

/**************************************/
/*------  Read Alarm Status  ---------*/
/*----  for 8013, 8017, 8018  -----*/
Exp8K WORD CALLBACK ReadAlarmStatus_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x8013/17/18
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7];    // slot
//---------------------- output ------------------------------------
// dwBuf[8]     1: High Alarm Occur    0: Don't Occur              
// dwBuf[9]     1: Low Alarm Occur    0: Don't Occur     

/*********************************************************/
/*------  Set Analog Output for 8022/8024/8026  ---------*/
Exp8K WORD CALLBACK AnalogOut_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];   // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];   // module address: 0x00 to 0xFF
// dwBuf[2];   // module ID: 0x8024,8022,8026
// dwBuf[3];   // checksum: 0=disable, 1=enable
// dwBuf[4];   // TimeOut constant: normal=100
// dwBuf[5];   // Channel No.(0 to 3) 
// dwBuf[6];   // flag: 0=no save, 1=save send/receive string
// dwBuf[7];   // slot
// f7000[0];   // analog output value
//---------------------- output ------------------------------------
// void    

/****************************************************/
/*------  Set Analog Output for 8022/8026  ---------*
/*Exp8K WORD CALLBACK AnalogOutHex_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]); //No release due to no Hex defined in firmware 8k232,8k485

//---------------------- input -------------------------------------
// dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];   // module address: 0x00 to 0xFF
// dwBuf[2];   // module ID: 0x8022/0x8026  (0x8024 has no Hex type)
// dwBuf[3];   // checksum: 0=disable, 1=enable
// dwBuf[4];   // TimeOut constant: normal=100
// dwBuf[5];   // Channel No.(0 to 1)                              
// dwBuf[6];   // flag: 0=no save, 1=save send/receive string
// dwBuf[7])   //analog output value(Hexadeciaml Data Foramt)
//---------------------- output ------------------------------------

/****************************************************/
/*------  Set Analog Output for 8022/8026  ---------*
Exp8K WORD CALLBACK AnalogOutFsr_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);//No release due to no FSr defined in firmware 8k232,8k485
//---------------------- input -------------------------------------
// dwBuf[0];   // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];   // module address: 0x00 to 0xFF
// dwBuf[2];   // module ID: 0x8022/0x8026 (0x8024 has no Fsr type)
// dwBuf[3];   // checksum: 0=disable, 1=enable
// dwBuf[4];   // TimeOut constant: normal=100
// dwBuf[5];   // Channel No.(0 to 1)
// dwBuf[6];   // flag: 0=no save, 1=save send/receive string

// fBuf[0];    // analog output value 		
//---------------------- output ------------------------------------

/****************************************************
  WORD CALLBACK AnalogOutReadBackHex_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);//No release due to no Hex defined in firmware 8k232,8k485
 //---------------------- input -------------------------------------
 // dwBuf[0];  // RS-232 port number: 1/2/3/4/..../255
 // dwBuf[1];  // module address: 0x00 to 0xFF
 // dwBuf[2];  // module ID: 0x8022/0x8026   (8024 has no Hex 
 // dwBuf[3];  // checksum: 0=disable, 1=enable
 // dwBuf[4];  // TimeOut constant: normal=100
 // dwBuf[5];  // for 8022/8026
               //     0: command read back ($AA6)
               //     1: analog output of current path read back ($AA8)
 // dwBuf[6];  // flag: 0=no save, 1=save send/receive string
 // dwBuf[7];  //   Channel No.(0 to 1)
               //   Not used,            else
 //---------------------- output ------------------------------------
 // fBuf[9]=analog output value read back(Hex)*/

/*******************************************************/
/*------  Read Configuration Status for 8024 ---------*/
Exp8K WORD CALLBACK ReadConfigurationStatus_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x8024,8022
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7];    // slot
//---------------------- output ------------------------------------
// dwBuf[8]:    // Output Range: 0x30, 0x31, 0x32              
// dwBuf[9]:    //  Slew rate             

/******************************************************/
/*------  Setting Start-Up Value for 8024 ---------*/
Exp8K WORD CALLBACK SetStartUpValue_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x8024,8022
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7];    // slot
//---------------------- output ------------------------------------
// void        

/******************************************************/
/*------  Reading Start-Up Value for 8024 ---------*/
Exp8K WORD CALLBACK ReadStartUpValue_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x8024,8022
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7];    // slot
//---------------------- output ------------------------------------
// fBuf[0]:     // the Start-Up Value       


Exp8K WORD CALLBACK DigitalOut_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID: 0x8037/41/42/50/54/55/56/57/60/63/64/65/66/68/69/77
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBif[5];             // 16 bit data to output
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string
// dwBuf[7];             // Slot Number: 0~7
//---------------------- output ------------------------------------

Exp8K WORD CALLBACK DigitalBitOut_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID:  0x8037/41/42/50/54/55/56/57/60/63/64/65/66/68/69/77
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string
// dwBuf[5];             // Out But Data 0 or 1
// dwBuf[7];             // Slot Number: 0 to 3
// dwBuf[8];             // Channel to Output
//---------------------- output ------------------------------------


Exp8K WORD CALLBACK DigitalIn_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID: 0x8040/42/50/51/52/53/54/55/58/63/77
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string
// dwBuf[7];             // Slot Number: 0 to 3
//---------------------- output ------------------------------------
                         // w7000[5]=digital input data


Exp8K WORD CALLBACK DigitalOutReadBack_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);

 //---------------------- input ---------------------------
 //dwBuf[0];            // RS-232 port number: 1/2/3/4/..../255
 //dwBuf[1];            // module address: 0x00 to 0xFF
 //dwBuf[2];            // module ID:0x8037/41/42/54/55/56/57/60/63/64/65/66/68/69
 //dwBuf[3];            // checksum: 0=disable, 1=enable
 //dwBuf[4];            // TimeOut constant: normal=100
 //dwBuf[6];            // flag: 0=no save, 1=save send/receive string
 //dwBuf[7];            // Slot Number
 //---------------------- output -----------------------
                        // void
               
 Exp8K WORD CALLBACK DigitalInCounterRead_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
 //---------------------- input ---------------------------
 //dwBuf[0];            // RS-232 port number: 1/2/3/4/..../255
 //dwBuf[1];            // module address: 0x00 to 0xFF
 //dwBuf[2];            // module ID:0x8037/41/42/54/55/56/57/60/63/64/65/66/68/69
 //dwBuf[3];            // checksum: 0=disable, 1=enable
 //dwBuf[4];            // TimeOut constant: normal=100
 //dwBuf[6];            // flag: 0=no save, 1=save send/receive string
 //dwBuf[7];            // Slot Number
 //---------------------- output -----------------------
                       //dwBuf[8]; 

Exp8K WORD CALLBACK ClearDigitalInCounter_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
 //---------------------- input ---------------------------
 //dwBuf[0];            // RS-232 port number: 1/2/3/4/..../255
 //dwBuf[1];            // module address: 0x00 to 0xFF
 //dwBuf[2];            // module ID:0x8037/41/42/54/55/56/57/60/63/64/65/66/68/69
 //dwBuf[3];            // checksum: 0=disable, 1=enable
 //dwBuf[4];            // TimeOut constant: normal=100
 //dwBuf[6];            // flag: 0=no save, 1=save send/receive string
 //dwBuf[7];            // Slot Number
 //---------------------- output -----------------------
                        // void

Exp8K WORD CALLBACK DigitalInLatch_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
 //---------------------- input ---------------------------
 //dwBuf[0];            // RS-232 port number: 1/2/3/4/..../255
 //dwBuf[1];            // module address: 0x00 to 0xFF
 //dwBuf[2];            // module ID:0x8037/41/42/54/55/56/57/60/63/64/65/66/68/69
 //dwBuf[3];            // checksum: 0=disable, 1=enable
 //dwBuf[4];            // TimeOut constant: normal=100
 //dwBuf[6];            // flag: 0=no save, 1=save send/receive string
 //dwBuf[7];            // Slot Number
 //---------------------- output -----------------------
                         //dwBuf[8]; 

Exp8K WORD CALLBACK ClearDigitalInLatch_8K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);

 //---------------------- input ---------------------------
 //dwBuf[0];            // RS-232 port number: 1/2/3/4/..../255
 //dwBuf[1];            // module address: 0x00 to 0xFF
 //dwBuf[2];            // module ID:0x8037/41/42/54/55/56/57/60/63/64/65/66/68/69
 //dwBuf[3];            // checksum: 0=disable, 1=enable
 //dwBuf[4];            // TimeOut constant: normal=100
 //dwBuf[6];            // flag: 0=no save, 1=save send/receive string
 //dwBuf[7];            // Slot Number
 //---------------------- output -----------------------
                        // void

Exp8K WORD CALLBACK GetChCount_FR(unsigned char wPort,unsigned char wAddr,unsigned char wCheckSum,WORD wTimeOut,WORD *wDICh, WORD *wDOCh,WORD *wAICh,WORD *wAOCh);

Exp8K WORD CALLBACK DigitalOutAll_FR
(unsigned char wPort,unsigned char wAddr,unsigned char wCheckSum,WORD wTimeOut,
WORD wDOGroupCount, DWORD dwDO[]);

/*
wPort         COM Port used to communicate with 7188EF-016
wAddr         Station Address is Always 01 on 7188EF-016
wCheckSum     Checksum of 7188EF-016
wTimeOut      Timeout for command to transmit to 7188EF-016
wDOGroupCount The total Group count of FRNet DO modules
dwDO[]        The DO output array value for all FRNet DO modules,
                the array size of dwDO[] is 8 WORD for 7188EF-016 
*/                     

Exp8K WORD CALLBACK DigitalOutGroup_FR
(unsigned char wPort,unsigned char wAddr,unsigned char wCheckSum,WORD wTimeOut,
WORD wGroupIndex, DWORD dwDO);
/*
wPort         COM Port used to communicate with 7188EF-016
wAddr         Station Address is Always 01 on 7188EF-016
wCheckSum     Checksum of 7188EF-016
wTimeOut      Timeout for command to transmit to 7188EF-016
wGroupIndex   The Group Index of FRNet DO modules,wGroupIndex(0~7)for 7188EF-016
dwDO          The DO output value for certain wGroupIndex FRNet DO module  
              dwDO (0 ~ 0xFFFF)for 7188EF-016
*/

Exp8K WORD CALLBACK DigitalOutBit_FR
(unsigned char wPort,unsigned char wAddr,unsigned char wCheckSum,WORD wTimeOut,
WORD wBitIndex, WORD wDO);
/*
wPort         COM Port used to communicate with 7188EF-016
wAddr         Station Address is Always 01 on 7188EF-016
wCheckSum     Checksum of 7188EF-016
wTimeOut      Timeout for command to transmit to 7188EF-016
wBitIndex     The bit index of FRNet DO modules,wBitIndex(0~127)for 7188EF-016
wDO           The DO output bit value for certain wChannel FRNet DO module  
*/

Exp8K WORD CALLBACK DigitalIOReadBackAll_FR
(unsigned char wPort,unsigned char wAddr,unsigned char wCheckSum,WORD wTimeOut,
DWORD dwDO[],DWORD dwDI[]);
/*
wPort         COM Port used to communicate with 7188EF-016
wAddr         Station Address is Always 01 on 7188EF-016
wCheckSum     Checksum of 7188EF-016
wTimeOut      Timeout for command to transmit to 7188EF-016
dwDO[]        The DO readback array value for all FRNet DO groups,
              the array size of dwDO[] is 8 WORD for 7188EF-016
dwDI[]        The DI readback array value for all FRNet DI groups,
              the array size of dwDI[] is 8 WORD for 7188EF-016
*/   

Exp8K WORD CALLBACK DigitalIOReadBackGroup_FR
(unsigned char wPort,unsigned char wAddr,unsigned char wCheckSum,WORD wTimeOut,
WORD wType,WORD wGroupIndex,DWORD *dwIOValue);
/*
wPort         COM Port used to communicate with 7188EF-016
wAddr         Station Address is Always 01 on 7188EF-016
wCheckSum     Checksum of 7188EF-016
wTimeOut      Timeout for command to transmit to 7188EF-016
wType         wType=0 for DI group; wType=1 for DO group
wGroupIndex   The Group Index of DI or DO group,wGroupIndex(0~7)for 7188EF-016
*dwIOValue    The dwIOValue is the readback value from certain wGroupIndex 
              of FRNet DO or DI group  
*/ 

Exp8K WORD CALLBACK DigitalIOReadBackBit_FR
(unsigned char wPort,unsigned char wAddr,unsigned char wCheckSum,WORD wTimeOut,
WORD wType,WORD wBitIndex,WORD *wIOBit);
/*
wPort         COM Port used to communicate with 7188EF-016
wAddr         Station Address is Always 01 on 7188EF-016
wCheckSum     Checksum of 7188EF-016
wTimeOut      Timeout for command to transmit to 7188EF-016
wType         wType=0 for DI group; wType=1 for DO group
wBitIndex     The total Group count of FRNet DI modules
*wIOBit       The DO readback bit value from FRNet DO groups  
*/  

Exp8K WORD CALLBACK DigitalOnLineStatus_FR
(unsigned char wPort,unsigned char wAddr,unsigned char wCheckSum,WORD wTimeOut,
WORD wType,WORD wIOStatus[]);
/*
wPort         COM Port used to communicate with 7188EF-016
wAddr         Station Address is Always 01 on 7188EF-016
wCheckSum     Checksum of 7188EF-016
wTimeOut      Timeout for command to transmit to 7188EF-016
wType         wType=0 for DI group; wType=1 for DO group 
wIOStatus[]   The all IO group's status of FRNet,wIOStatus[]=1 on line;
              wIOStatus[]=0 off line,the array size of wIOStatus is 8
Note: at present it can not get the DO module's status from FRNet  
*/

void MakeCRC16Table(void);
Exp8K unsigned GetCRC16(unsigned char *data,int length);
Exp8K int CALLBACK ShiftLeft(int data, int length);
Exp8K int CALLBACK ShiftRight(int data, int length);
Exp8K unsigned CALLBACK  IntToUnsig(int data);

//#ifdef _WIN32_WCE
//	#endif  //__I8000_H__
//#endif
