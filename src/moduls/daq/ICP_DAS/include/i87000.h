#ifdef _WIN32_WCE //Provided for WinCon
	#define Exp87K I7000CE_API
//	#define CALLBACK NULL
#else //Provided for PC (Microsoft OS)
	#ifndef Exp87K
		#ifdef __cplusplus
			#define Exp87K extern "C" __declspec(dllimport)
		#else
			#define Exp87K __declspec(dllexport)
		#endif
	#endif
#endif 

#ifdef _WIN32_WCE
//	#ifndef __I87000_H__
//	#define __I87000_H__
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

Exp87K WORD CALLBACK DigitalOut_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID: 0x87054/55/56/57/60/63/64/65/66/68/41
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[5];             // 16-bit digital data to output
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // void



Exp87K WORD CALLBACK DigitalBitOut_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID: 0x87054/55/56/57/60/63/64/65/66/68/41
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[5];             // don't care
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string
// dwBuf[7];             // which channel to output
// dwBuf[8];             // data to output (0 or 1)
//---------------------- output ------------------------------------
                         // void



Exp87K WORD CALLBACK DigitalIn_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID:  0x87054/55/56/57/60/63/64/65/66/68/40
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[5];             // 16-bit digital data to output
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
                         // void



Exp87K WORD CALLBACK DigitalInCounterRead_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID: 0x87051/52/53/54/55/63/40
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[5];             // Channel Number
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
// dwBuf[7];			 //Counter Value of Channel N's Digital Input


Exp87K WORD CALLBACK ClearDigitalInCounter_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID: 0x87051/52/53/54/55/63/40
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[5];             // Channel Number
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string


Exp87K WORD CALLBACK DigitalInLatch_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID: 0x87051/52/53/54/55/63/40
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[5];             // 0: Latch Low,   1: Latch High
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string
//---------------------- output ------------------------------------
// dwBuf[7];             // latch value


Exp87K WORD CALLBACK ClearDigitalInLatch_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID: 0x87051/52/53/54/55/63/40
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[5];             // Don't care
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string


Exp87K WORD CALLBACK DigitalOutReadBack_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);

 //---------------------- input ---------------------------
 //dwBuf[0];		 // RS-232 port number: 1/2/3/4/..../255
 //dwBuf[1];         // module address: 0x00 to 0xFF
 //dwBuf[2];         // module ID: 0x87054/55/56/57/60/63/64/65/66/68/41
 //dwBuf[3];		 // checksum: 0=disable, 1=enable
 //dwBuf[4];		 // TimeOut constant: normal=100
 //dwBuf[6];		 // flag: 0=no save, 1=save send/receive string
 //---------------------- output -----------------------
 // void

Exp87K WORD CALLBACK AnalogIn_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];            // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];            // module address: 0x00 to 0xFF
// dwBuf[2];            // module ID: 0x87013/17/18/33
// dwBuf[3];            // checksum: 0=disable, 1=enable
// dwBuf[4];            // TimeOut constant: normal=100
// dwBuf[5];            // channel number for multi-channel
// dwBuf[6];            // flag: 0=no save, 1=save send/receive string
// dwBuf[7];			// Slot Number
//---------------------- output ------------------------------------
                        // fBuf[0]=analog input value

/************************************************************/
/*  AnalogInAll_87K											*/
/*															*/
/*  Main Unit: I-87K series									*/
/*															*/
/*  Module: I-87013,,I-87017,I-87018						*/
/************************************************************/
Exp87K WORD CALLBACK AnalogInAll_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
/*
====== Input parameter ====================================
 dwBuf[0]; RS-232 port number: 1 ~ 255
 dwBuf[1]; Module address: 0x00 ~ 0xFF
 dwBuf[2]; Module ID:	0x87013 for I-87013,
						0x87017 for I-87017,
						0x87018 for I-87018
 dwBuf[3]; Checksum: 0=disable, 1=enable
 dwBuf[4]; TimeOut: Normal=100 (ms)
 dwBuf[6]; Debug:	0=disable
					1=enable:	copy command string (sent to module)
								to szSend and response string (received from module)
								to szReceive.
 dwBuf[7];				// Data Foramt, 0:Engineer 1:FSR  2:Hex
====== Output parameter ===================================
 if dwBuf[2] is 0x87013
 fBuf[0]=analog input value of channel_0
 fBuf[1]=analog input value of channel_1
 fBuf[2]=analog input value of channel_2
 fBuf[3]=analog input value of channel_3

 if dwBuf[2] is 0x87017/18
 fBuf[0]=analog input value of channel_0
 fBuf[1]=analog input value of channel_1
 ........................................
 fBuf[7]=analog input value of channel_7
*/


/************************************************/
/*------  Set Analog Output for 87024  ---------*/
Exp87K WORD CALLBACK AnalogOut_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];   // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];   // module address: 0x00 to 0xFF
// dwBuf[2];   // module ID: 0x87024
// dwBuf[3];   // checksum: 0=disable, 1=enable
// dwBuf[4];   // TimeOut constant: normal=100
// dwBuf[5];   // Channel No.(0 to 3) 
// dwBuf[6];   // flag: 0=no save, 1=save send/receive string
// f7000[0];   // analog output value
//---------------------- output ------------------------------------
// void    

/*******************************************************/
Exp87K WORD CALLBACK AnalogOutHex_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x87022/0x87026  (0x87024 has no Hex type)
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // Channel No.(0 to 1)                              
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7])    //analog output value(Hexadeciaml Data Foramt)

/*******************************************************************/
Exp87K WORD CALLBACK AnalogOutFsr_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
 //---------------------- input -------------------------------------
 //dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
 //dwBuf[1];    // module address: 0x00 to 0xFF
 //dwBuf[2];    // module ID: 0x87022/0x87026 (0x87024 has no Fsr type)
 //dwBuf[3];    // checksum: 0=disable, 1=enable
 //dwBuf[4];    // TimeOut constant: normal=100
 //dwBuf[5];    // Channel No.(0 to 1)
 //dwBuf[6];    // flag: 0=no save, 1=save send/receive string
  
 //fBuf[0];     // analog output value 		
 //---------------------- output ------------------------------------

/*******************************************************/
Exp87K WORD CALLBACK AnalogOutReadBack_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];             // RS-232 port number: 1/2/3/4..../255
// dwBuf[1];             // module address: 0x00 to 0xFF
// dwBuf[2];             // module ID: 0x87022/0x87026   (87024 has no FSR)
// dwBuf[3];             // checksum: 0=disable, 1=enable
// dwBuf[4];             // TimeOut constant: normal=100
// dwBuf[5];             // 0: command read back ($AA6)
                         // 1: analog output of current path read back ($AA8)
// dwBuf[6];             // flag: 0=no save, 1=save send/receive string
// dwBuf[7];             // channel number for multi-channel
//---------------------- output ------------------------------------
                         // fBuf[0]=analog out readback value

Exp87K WORD CALLBACK AnalogOutReadBackHex_87K(DWORD dwBuf[], float fBuf[],
      char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];  // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];  // module address: 0x00 to 0xFF
// dwBuf[2];  // module ID: 0x87022/0x87026   (87024 has no Hex 
// dwBuf[3];  // checksum: 0=disable, 1=enable
// dwBuf[4];  // TimeOut constant: normal=100
// dwBuf[5];  // for 8022/8026
              //     0: command read back ($AA6)
              //     1: analog output of current path read back ($AA8)
// dwBuf[6];  // flag: 0=no save, 1=save send/receive string
// dwBuf[7];  //   Channel No.(0 to 1)
              //   Not used,            else
//---------------------- output ------------------------------------
// fBuf[9];	  //analog output value read back(Hex)

Exp87K WORD CALLBACK AnalogOutReadBackFsr_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];  // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];  // module address: 0x00 to 0xFF
// dwBuf[2];  // module ID: 87022/87026
// dwBuf[3];  // checksum: 0=disable, 1=enable
// dwBuf[4];  // TimeOut constant: normal=100
// dwBuf[5];  // for 7021/7022/7024
              //     0: command read back ($AA6)
              //     1: analog output of current path read back ($AA8)
// dwBuf[6];  // flag: 0=no save, 1=save send/receive string
// dwBuf[7];  // Channel No.(0 to 3)
//---------------------- output ------------------------------------
              // fBuf[0]=analog output value read back


/*******************************************************/
/*------  Set Configuration Status for 87024 ---------*/
Exp87K WORD CALLBACK ReadConfigurationStatus_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x87024
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[8]:    // Output Range: 0x30, 0x31, 0x32              
// dwBuf[9]:    //  Slew rate :  0 to 11                 
//---------------------- output ------------------------------------
// void

/*******************************************************/


/******************************************************/
/*'02'6'24 added
/*------  Reading Start-Up Value for 87024 ---------*/
Exp87K WORD CALLBACK ReadStartUpValue_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
//---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x87024
// dwBuf[3];    // checksum: 0=disable, 1=enable
// dwBuf[4];    // TimeOut constant: normal=100
// dwBuf[5];    // channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7];    // slot
//---------------------- output ------------------------------------
// fBuf[0]      // the Start-Up Value        
   
/******************************************************/
/*'02'6'24 added
/---------  Setting Start-Up Value for 87024 ---------*/
Exp87K WORD CALLBACK SetStartUpValue_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
   //---------------------- input -------------------------------------
// dwBuf[0];    // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];    // module address: 0x00 to 0xFF
// dwBuf[2];    // module ID: 0x87024
// dwBuf[3];	// checksum: 0=disable, 1=enable
// dwBuf[4];	// TimeOut constant: normal=100
// dwBuf[5];	// channel
// dwBuf[6];    // flag: 0=no save, 1=save send/receive string
// dwBuf[7];    // slot


/************************************************************************/
/*'02'6'24 added
/* -------------------------------------------------------------------- */
Exp87K WORD CALLBACK AnalogInFsr_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
  //---------------------- input -------------------------------------
// dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];     // module address: 0x00 to 0xFF
// dwBuf[2];     // module ID: 0x87017/0x87018
// dwBuf[3];     // checksum: 0=disable, 1=enable
// dwBuf[4];     // TimeOut constant: normal=100
// dwBuf[5];     // channel number for multi-channel
// dwBuf[6];     // flag: 0=no save, 1=save send/receive string
// dwBuf[7];     // Slot Number
//---------------------- output ------------------------------------
// fBuf[0];      //analog input value
                        
/************************************************************************/
/*'02'6'24 added
/* -------------------------------------------------------------------- */
Exp87K WORD CALLBACK AnalogInHex_87K(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);

// dwBuf[0];     // RS-232 port number: 1/2/3/4/..../255
// dwBuf[1];     // module address: 0x00 to 0xFF
// dwBuf[2];     // module ID: 0x87017/0x87018
// dwBuf[3];     // checksum: 0=disable, 1=enable
// dwBuf[4];     // TimeOut constant: normal=100
// dwBuf[5];     // channel number for multi-channel
// dwBuf[6];     // flag: 0=no save, 1=save send/receive string
// dwBuf[7];     // Slot Number
//---------------------- output ------------------------------------
// fBuf[0];      //analog input value                        



//------------------------- ReadCounter -----------------------
Exp87K WORD CALLBACK ReadCounter(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
   
//   ---------------------- input -------------------------------------
//dwBuf[0];		// RS-232 port number: 1/2/3/4/..../255
//dwBuf[1];		// module address: 0x00 to 0xFF
//dwBuf[2];		// module ID: 0x7080 ,0x87082 ,0x8080
//dwBuf[3];		// checksum: 0=disable, 1=enable
//dwBuf[4];		// TimeOut constant: normal=100
//dwBuf[5];		// channel number
//dwBuf[6];		// flag: 0=no save, 1=save send/receive string
//dwBuf[7];		// wSlot: Slot number 0 ~ 7
//dwBuf[8];		// wReadMode 0: read counter 1: read counter and clear counter
				//			 2: read counter and overflow
//   ---------------------- output ------------------------------------
//dwBuf[9] 		//Counter Value
//dwBuf[10]		//Overflow Value

Exp87K WORD CALLBACK ReadUpDownDirPulse(long LBuf[], float fBuf[],
        char szSend[], char szReceive[]);


Exp87K WORD CALLBACK ReadUpDownDirPulse_All(long LBuf[], float fBuf[],
        char szSend[], char szReceive[]);

//------------------------- ReadCounter_All -----------------------
Exp87K WORD CALLBACK ReadCounter_All(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
   
//   ---------------------- input -------------------------------------
//dwBuf[0];		// RS-232 port number: 1/2/3/4/..../255
//dwBuf[1];		// module address: 0x00 to 0xFF
//dwBuf[2];		// module ID: 0x7080 ,0x87082 ,0x8080
//dwBuf[3];		// checksum: 0=disable, 1=enable
//dwBuf[4];		// TimeOut constant: normal=100
//dwBuf[5];		// channel number
//dwBuf[6];		// flag: 0=no save, 1=save send/receive string
//dwBuf[7];		// wSlot: Slot number 0 ~ 7
//dwBuf[8];		// wReadMode 0: read counter 1: read counter and overflow
						 
//   ---------------------- output ------------------------------------
//dwBuf[9];		//Counter Value for channel 0
//dwBuf[10];	//Counter Value for channel 1
//dwBuf[11];	//Counter Value for channel 2
//dwBuf[12];	//Counter Value for channel 3
//dwBuf[13];	//Counter Value for channel 4
//dwBuf[14];	//Counter Value for channel 5
//dwBuf[15];	//Counter Value for channel 6
//dwBuf[16];	//Counter Value for channel 7
//dwBuf[17];	//Overflow Value for channel 0
//dwBuf[18];	//Overflow Value for channel 1
//dwBuf[19];	//Overflow Value for channel 2
//dwBuf[20];	//Overflow Value for channel 3
//dwBuf[21];	//Overflow Value for channel 4
//dwBuf[22];	//Overflow Value for channel 5
//dwBuf[23];	//Overflow Value for channel 6
//dwBuf[24];	//Overflow Value for channel 7



//------------------------- ReadCounter -----------------------
Exp87K WORD CALLBACK ReadFreq(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
   
//   ---------------------- input -------------------------------------
//dwBuf[0];		// RS-232 port number: 1/2/3/4/..../255
//dwBuf[1];		// module address: 0x00 to 0xFF
//dwBuf[2];		// module ID: 0x7080 ,0x87082 ,0x8080
//dwBuf[3];		// checksum: 0=disable, 1=enable
//dwBuf[4];		// TimeOut constant: normal=100
//dwBuf[5];		// channel number
//dwBuf[6];		// flag: 0=no save, 1=save send/receive string
//dwBuf[7];		// wSlot: Slot number 0 ~ 7

//   ---------------------- output ------------------------------------
//dwBuf[9] 		//Frequency Value


Exp87K WORD CALLBACK ReadFreq_All(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
   
//---------------------- input -------------------------------------
//dwBuf[0];		// RS-232 port number: 1/2/3/4/..../255
//dwBuf[1];		// module address: 0x00 to 0xFF
//dwBuf[2];		// module ID: 0x7080 ,0x87082 ,0x8080
//dwBuf[3];		// checksum: 0=disable, 1=enable
//dwBuf[4];		// TimeOut constant: normal=100
//dwBuf[5];		// channel number
//dwBuf[6];		// flag: 0=no save, 1=save send/receive string
//dwBuf[7];		// wSlot: Slot number 0 ~ 7
//   ---------------------- output ------------------------------------
//dwBuf[9];		//Frequency Value for channel 0
//dwBuf[10];	//Frequency Value for channel 1
//dwBuf[11];	//Frequency Value for channel 2
//dwBuf[12];	//Frequency Value for channel 3
//dwBuf[13];	//Frequency Value for channel 4
//dwBuf[14];	//Frequency Value for channel 5
//dwBuf[15];	//Frequency Value for channel 6
//dwBuf[16];	//Frequency Value for channel 7
                                          
/******************************* ReadOverflow **********************************/
//Exp87K WORD CALLBACK ReadOverflow(DWORD dwBuf[], float fBuf[],
//        char szSend[], char szReceive[]);
   
//   ---------------------- input -------------------------------------
//dwBuf[0];		// RS-232 port number: 1/2/3/4/..../255
//dwBuf[1];		// module address: 0x00 to 0xFF
//dwBuf[2];		// module ID: 0x7080 ,0x87082 ,0x8080
//dwBuf[3];		// checksum: 0=disable, 1=enable
//dwBuf[4];		// TimeOut constant: normal=100
//dwBuf[5];		// channel number
//dwBuf[6];		// flag: 0=no save, 1=save send/receive string
//dwBuf[7];		// wSlot: Slot number 0 ~ 7
//   ---------------------- output ------------------------------------
//dwBuf[8];		//Overflow Value


//Exp87K WORD CALLBACK ReadOverflow_All(DWORD dwBuf[], float fBuf[],
//        char szSend[], char szReceive[]);
   
//---------------------- input -------------------------------------
//dwBuf[0];		// RS-232 port number: 1/2/3/4/..../255
//dwBuf[1];		// module address: 0x00 to 0xFF
//dwBuf[2];		// module ID: 0x7080 ,0x87082 ,0x8080
//dwBuf[3];		// checksum: 0=disable, 1=enable
//dwBuf[4];		// TimeOut constant: normal=100
//dwBuf[5];		// channel number
//dwBuf[6];		// flag: 0=no save, 1=save send/receive string
//dwBuf[7];		// wSlot: Slot number 0 ~ 7
//   ---------------------- output ------------------------------------
//dwBuf[8];		//Overflow Value for channel 0
//dwBuf[9];		//Overflow Value for channel 1
//dwBuf[10];	//Overflow Value for channel 2
//dwBuf[11];	//Overflow Value for channel 3
//dwBuf[12];	//Overflow Value for channel 4
//dwBuf[13];	//Overflow Value for channel 5
//dwBuf[14];	//Overflow Value for channel 6
//dwBuf[15];	//Overflow Value for channel 7


/*-------- ClearCounter    Clear the counter---------*/
Exp87K WORD CALLBACK  ClearCounter(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
   
//---------------------- input -------------------------------------
//dwBuf[0];		// RS-232 port number: 1/2/3/4/..../255
//dwBuf[1];		// module address: 0x00 to 0xFF
//dwBuf[2];		// module ID: 0x7080 ,0x87082 ,0x8080
//dwBuf[3];		// checksum: 0=disable, 1=enable
//dwBuf[4];		// TimeOut constant: normal=100
//dwBuf[5];		// channel number
//dwBuf[6];		// flag: 0=no save, 1=save send/receive string
//dwBuf[7];		// wSlot: Slot number 0 ~ 7                          


/*-------- SetCounterStatus Start\Stop the counter---------*/
Exp87K WORD CALLBACK  SetCounterStatus(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
   
//---------------------- input -------------------------------------
//dwBuf[0];		// RS-232 port number: 1/2/3/4/..../255
//dwBuf[1];		// module address: 0x00 to 0xFF
//dwBuf[2];		// module ID: 0x7080 ,0x87082 ,0x8080
//dwBuf[3];		// checksum: 0=disable, 1=enable
//dwBuf[4];		// TimeOut constant: normal=100
//dwBuf[5];		// channel number
//dwBuf[6];		// flag: 0=no save, 1=save send/receive string
//dwBuf[7];		// wSlot: Slot number 0 ~ 7 


/*-------- ReadCounterStatus S=0 Stop S=1 Start the counter---------*/
Exp87K WORD CALLBACK  ReadCounterStatus(DWORD dwBuf[], float fBuf[],
        char szSend[], char szReceive[]);
   
//---------------------- input -------------------------------------
//dwBuf[0];		// RS-232 port number: 1/2/3/4/..../255
//dwBuf[1];		// module address: 0x00 to 0xFF
//dwBuf[2];		// module ID: 0x7080 ,0x87082 ,0x8080
//dwBuf[3];		// checksum: 0=disable, 1=enable
//dwBuf[4];		// TimeOut constant: normal=100
//dwBuf[5];		// channel number
//dwBuf[6];		// flag: 0=no save, 1=save send/receive string
//dwBuf[7];		// wSlot: Slot number 0 ~ 7
// ----------------------- output -----------------------------------------
//dwBuf[8];		// Counter or Frequency Status
            
                           

//#ifdef _WIN32_WCE
//	#endif  //__I87000_H__
//#endif



    
