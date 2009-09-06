/**********************************************************************
 *
 *  msw.h
 *
 *  Header for msw.c (Microsoft Windows Warp)
 *
 *  Since 1999.10.27 by Reed Lai
 *
 **********************************************************************/
/* *INDENT-OFF* */
#ifndef _MSW_H
#define _MSW_H

#include "i7k.h"
#include "common.h"
#include "slot.h"
#include "lincon.h"

#define DEFAULT_READ_BUF_SIZE 250

#define MAX_CMD_SIZE  250

#define MAX_COM_PORTS 50
#define COM1          1
#define COM2          2
#define COM3          3
#define COM4          4
#define COM5          5
#define COM6          6
#define COM7          7
#define COM8          8
#define COM9          9
#define COM10         10
#define COM11         11
#define COM12         12
#define COM13         13
#define COM14         14
#define COM15         15
#define COM16         16
#define COM17         17
#define COM18         18
#define COM19         19
#define COM20         20
#define COM21         21
#define COM22         22
#define COM23         23
#define COM24         24
#define COM25         25
#define COM26         26
#define COM27         27
#define COM28         28
#define COM29         29
#define COM30         30
#define COM31         31
#define COM32         32
#define COM33         33
#define COM34         34
#define COM35         35
#define COM36         36
#define COM37         37
#define COM38         38
#define COM39         39
#define COM40         40
#define COM41         41
#define COM42         42
#define COM43         43
#define COM44         44
#define COM45         45
#define COM46         46
#define COM47         47
#define COM48         48
#define COM49         49
#define COM50         50



#define    Data5Bit                     5
#define    Data6Bit                     6
#define    Data7Bit                     7
#define    Data8Bit                     8

#define    NonParity                    0
#define    OddParity                    1
#define    EvenParity                   2

#define    OneStopBit                   0
#define    One5StopBit                  1
#define    TwoStopBit                   2


#define __declspec(dllexport)
#define EXPORTS
#define CALLBACK
#define WINAPI
#define FAR
#define NEAR

typedef __bool_t BOOL;

typedef u_byte_t BYTE;
typedef u_word_t WORD;
typedef u_dword_t DWORD;

typedef unsigned char UCHAR;

extern int _fd[];

extern EXPORTS WORD CALLBACK Send_Receive_Cmd(char cPort, char szCmd[], char szResult[], WORD wTimeOut,
			              WORD wChksum, WORD * wT);
extern EXPORTS WORD CALLBACK Receive_Cmd(char cPort, char szResult[],WORD wTimeOut, WORD wChksum);								  
extern EXPORTS WORD CALLBACK Send_Cmd(char cPort, char szCmd[], WORD wChksum);
extern EXPORTS WORD CALLBACK Receive_Binary(char cPort, char szResult[], WORD wTimeOut, WORD wLen, WORD *wT);									  
extern EXPORTS WORD CALLBACK Send_Binary(char cPort, char szCmd[], int iLen);
extern EXPORTS WORD CALLBACK Open_Com(char port, DWORD baudrate, char cData, char cParity, char cStop);									 
extern EXPORTS BOOL CALLBACK Close_Com(char port);

/* Macros to warp MS-Windows functions */
#define Sleep(a)

/* Macros to warp Turbo C functions */
#define getch()   (int)getchar()
#define getche()  (int)getchar()
#define kbhit()   (int)getchar()

/* Macros to warp DOS functions */
#define RECEIVE_CMD(a, b, c, d) (int)Receive_Cmd((char)(a), (b), (long)(c), (int)(d))
#define SEND_CMD(a,b,c,d) (int)Send_Cmd((char)(a), (b), (WORD)(d))
#define OPEN_COM(p,b)     (int)Open_Com((char)(p), (DWORD)(b), Data8Bit, NonParity, OneStopBit)
#define CLOSE_COM(p)      (int)Close_Com((char)(p))

//add by Cindy 20090121
#define RECEIVE_BINARY(a, b, c, d ,e) (int)Receive_Binary((char)(a), (b), (long)(c), (WORD)(d), (e))
#define SEND_BINARY(a,b,c,d) (int)Send_Binary((char)(a), (b), (int)(d))


#include "i7000.h"
#include "i8000.h"
#include "i87000.h"
#endif							/* _MSW_H */
