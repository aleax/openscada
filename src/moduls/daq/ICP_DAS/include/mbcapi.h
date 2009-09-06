/* ----------------------- Standard C Libs includes --------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#define SET_OFF 0
#define SET_ON  1




extern char	mbRTU_CRC_Status; 		// = SET_OFF / SET_ON (default)   
extern int cMBTCPInit(char *server,int tid, int pid, int timeout);
extern int cMBRTUInit(char *deviceName,unsigned long ulBaudRate,unsigned char parity,
						unsigned char dataBits, unsigned char stopBit);
extern void cMBTCPClose(int iSocket);
extern void cRTUClose(int iSerialFd);
extern void dumpErrMsg(int errCode);


/*
	FC1  Read multiple coils status (0xxxx)  for DO
    [Request]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=01
    Byte 2-3:     Reference number
    Byte 4-5:     Bit count

	[Response]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=01
    Byte 2:       Byte count of response (B=(bit count + 7)/8)
    Byte 3-(B+2): Bit values (least significant is first coil!)
    
  	[Return Value]
	已讀取的封包 bytes, 0 > :  Error Code, 
*/
extern int mbRTU_R_Coils(int iSerialFd,char sID,int iStartAddr, int bitCount,
						unsigned char *iRecBuf , int iRecBufLen);

extern int mbTCP_R_Coils(int s,char sID,int iStartAddr, int bitCount, 
						unsigned char *iRecBuf , int iRecBufLen);
/* 
	FC2  Read multiple input discretes (1xxxx)  for DI
	[Request]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=02
    Byte 2-3:     Reference number
    Byte 4-5:     Bit count

	[Response]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=02
    Byte 2:       Byte count of response (B=(bit count + 7)/8)
    Byte 3-(B+2): Bit values (least significant is first coil!)
 
*/
extern int mbRTU_R_Discrete(int iSerialFd,char sID,int iStartAddr, int bitCount, 
				unsigned char *iRecBuf, int iBufLen )  ;
extern int mbTCP_R_Discrete(int s,char sID,int iStartAddr, int bitCount, 
				unsigned char *iRecBuf, int iBufLen );
/* 
	FC3  Read multiple registers (4xxxx)  for AO
	[Request]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=03
    Byte 2-3:     Reference number
    Byte 4-5:     Word count

	[Response]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=03
    Byte 2:       Byte count of response (B=2 x word count)
    Byte 3-(B+2): Register values
 
*/
extern int mbRTU_R_Hold_Registers(int iSerialFd,char sID,int iStartAddr, int wordCount, 
				unsigned char *iRecBuf, int iBufLen ) ;
extern int mbTCP_R_Hold_Registers(int s,char sID,int iStartAddr, int wordCount, 
				unsigned char *iRecBuf, int iBufLen );
/* 
	FC4  Read multiple input registers (3xxxx)  for AI
	[Request]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=04
    Byte 2-3:     Reference number
    Byte 4-5:     Word count

	[Response]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=04
    Byte 2:       Byte count of response (B=2 x word count)
    Byte 3-(B+2): Register values
 
*/
extern int mbRTU_R_Registers(int iSerialFd,char sID,int iStartAddr, int wordCount, 
				unsigned char *iRecBuf, int iBufLen ) ;
extern int mbTCP_R_Registers(int s,char sID,int iStartAddr, int wordCount, 
				unsigned char *iRecBuf, int iBufLen );
/* 
	FC5  Write single coil (0xxxx)  for DO
	[Request]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=05
    Byte 2-3:     Reference number
    Byte 4:       =FF to trun ON coil, =00 to trun OFF coil
    Byte 5:       =00

	[Response]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=05
    Byte 2-3:     Reference number
    Byte 4:       =FF to trun ON coil, =00 to trun OFF coil (echoed)
    Byte 5:       =00
 
*/
extern int mbRTU_W_Coil(int iSerialFd,char sID,int iStartAddr, char OnOff,
				unsigned char *iRecBuf, int iBufLen ) ;
extern int mbTCP_W_Coil(int s,char sID,int iStartAddr, char OnOff,
				unsigned char *iRecBuf, int iBufLen );
/* 
	FC6  Write single register (4xxxx)  for AO
	[Request]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=06
    Byte 2-3:     Reference number
    Byte 4-5:     Register value

	[Response]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=06
    Byte 2-3:     Reference number
    Byte 4-5:     Register value
*/
/*  若用來設定 DO, 則資料放在 Byte2 
	EX: Addres = 800 (slot 8, bit 0) set DO on
	    則 regValue = 0x0100
*/
extern int mbRTU_W_Register(int iSerialFd,char sID,int iStartAddr, int regValue, 
				unsigned char *iRecBuf, int iBufLen );
extern int mbTCP_W_Register(int s,char sID,int iStartAddr, int regValue, 
				unsigned char *iRecBuf, int iBufLen );
/* 
	FC15 Force multiple coils (0xxxx)  for DO
	[Request]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=0F (hex)
    Byte 2-3:     Reference number
    Byte 4-5:     Bit count
    Byte 6:       Byte count (B=(bit count + 7)/8)
    Byte 7-(B+6): Data to be written (least significant is first coil!)

	[Response]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=0F (hex)
    Byte 2-3:     Reference number
    Byte 4-5:     Bit count 
*/
extern int mbRTU_W_Multi_Coils(int iSerialFd,char sID,int iStartAddr, int bitCount, int byteCount, 
				unsigned char *wData, int wDataLen );
extern int mbTCP_W_Multi_Coils(int s,char sID,int iStartAddr, int bitCount, int byteCount, 
				unsigned char *wData, int wDataLen );
/* 
	FC16 Write multiple registers (4xxxx)  for AO
	[Request]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=10 (hex)
    Byte 2-3:     Reference number
    Byte 4-5:     Word count
    Byte 6:       Byte count (B=2 x word count)
    Byte 7-(B+6): Register values

	[Response]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=10 (hex)
    Byte 2-3:     Reference number
    Byte 4-5:     Word count 
*/
extern int mbRTU_W_Multi_Registers(int iSerialFd,char sID,int iStartAddr, int regCount,
				unsigned char *iRecBuf, int iBufLen );
extern int mbTCP_W_Multi_Registers(int s,char sID,int iStartAddr, int regCount,
				unsigned char *iRecBuf, int iBufLen );
				
/* 
	FC108 ICPDAS read range code
	[Request]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=6c(hex)
    Byte 2:       sub FC = 0x07(hex)
    Byte 2-3:     Reference number
    Byte 4-:

	[Response]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=10 (hex)
    Byte 2-3:     Reference number
    Byte 4:       range code 
*/
extern int mbRTU_R_RangeCode(int iSerialFd,char sID,int iStartAddr,	unsigned char *iRecBuf, int iBufLen ) ;
extern int mbTCP_R_RangeCode(int s,char sID,int iStartAddr,	unsigned char *iRecBuf, int iBufLen ) ;


/* 
	FC108 ICPDAS write range code
	[Request]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=6c(hex)
    Byte 2:       sub FC = 0x08(hex)
    Byte 2-3:     Reference number
    Byte 4-:

	[Response]
    Byte 0:       Net ID (Station number)
    Byte 1:       FC=10 (hex)
    Byte 2-3:     Reference number
    Byte 4:       range code 
*/

extern int mbRTU_W_RangeCode(int iSerialFd,char sID,int iStartAddr,char rangeCode,
				unsigned char *iRecBuf, int iBufLen ) ;
extern int mbTCP_W_RangeCode(int s,char sID,int iStartAddr,char rangeCode,
				unsigned char *iRecBuf, int iBufLen ) ;
				
								
				
				
