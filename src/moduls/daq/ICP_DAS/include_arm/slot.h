/*
 * slot.h
 * (C) 2004 Moki Matsushima <moki@icpdas.com>
 *
 * All this is intended to be used with a ISA-like slot on the SA1100's 
 * static memory bus. 
 * This is included by slot.c.
 */
 
#ifdef __cplusplus
extern "C" {
#endif
 
#include <linux/types.h>

typedef unsigned int slot_flags_t;
#define slot_lib_version 15
#define CARD_NAME_LENGTH 32
#define CNL CARD_NAME_LENGTH
/* General Definition */
#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef FAILURE
#define FAILURE -1
#endif

/* PIO signal */
typedef struct slot_signal {
	int sid;					/* signal id */
	pid_t pid;					/* process id */
	struct task_struct *task;	/* pointer to task structure */
	int is;						/* mask for irq source 0 disable 1 enable */
	int edge;					/* active edge for each irq source 0 for
								   negative (falling) edge 1 for positive
								   (rising) edge */
	int bedge;					/* both edges, or bipolar. 0 up to the
								   setting in variable edge. 1 does action 
								   for both negative and positive
								   edges, discards setting in variable
								   edge.  */
	/* FIXME - the edge and bedge should be merged */
} slot_signal_t;

#ifndef _DATA_T
#define _DATA_T
typedef union data {
	__u8 u8;
	__u16 u16;
	__u32 u32;
	__u64 u64;
	void *ptr;
} data_t;
#endif


typedef struct slot_devinfo {
	struct slot_devinfo *next;	/* next device (pio card) */
	struct slot_devinfo *prev;	/* previous device */
	struct slot_devinfo *next_f;	/* next device in same family */
	struct slot_devinfo *prev_f;	/* previous device in same family */
	int no;						/* device number (minor number) */
	unsigned int csid;			/* composed sub-IDs */
	unsigned int irq;			/* IRQ */
	unsigned long base;			/* base I/O address */
	unsigned int open;			/* open counter */
	struct file_operations *fops;	/* file operations for this device 
									 */
	char name[CNL];				/* card name information */
	struct slot_signal sig;	/* user signal setting for interrupt */
	unsigned int is;			/* last interrupt source */
	unsigned int is_edge;		/* edge of last interrupt source 0 for
								 * negative edge 1 for positive edge */
	slot_flags_t flags;
	data_t data;
	int tmp;
} slot_devinfo_t;

//int slot_major = 0;


/* PIO register */
typedef struct slot_reg {
	unsigned int id;			/* register's id */
	unsigned int value;			/* register's value for read/write */
	unsigned int page;
	unsigned int addr_size;
	unsigned int addr_first;
	unsigned int addr_second;
	unsigned int offset;			/* register's offset */
} slot_reg_t;


/* IDs of ioctl commands */
enum {
	SLOT_IOCTL_ID_GET_INFO,
	SLOT_IOCTL_ID_SET_SIG,
	SLOT_IOCTL_ID_READ_REG,
	SLOT_IOCTL_ID_WRITE_REG,
	SLOT_IOCTL_ID_SELECT,
	SLOT_IOCTL_ID_READ_EEP,
	SLOT_IOCTL_ID_WRITE_EEP,
	SLOT_IOCTL_ID_ENABLE_EEP,
	SLOT_IOCTL_ID_DISABLE_EEP,
	SLOT_IOCTL_ID_READ_SN,
	SLOT_IOCTL_ID_READ_MAC,
	SLOT_IOCTL_ID_WRITE_MAC,
	SLOT_IOCTL_ID_DI,
	SLOT_IOCTL_ID_DO,
	SLOT_IOCTL_ID_AI,
	SLOT_IOCTL_ID_AO,
	SLOT_IOCTL_ID_AO_PAT,
	SLOT_IOCTL_ID_AO_PAT_ADD,
	SLOT_IOCTL_ID_AO_PAT_START,
	SLOT_IOCTL_ID_AO_PAT_PAUSE,
	SLOT_IOCTL_ID_AO_PAT_STOP,
	SLOT_IOCTL_ID_AO_PAT_CLEAR,
	SLOT_IOCTL_ID_AO_PAT_STATUS,
	SLOT_IOCTL_ID_AO_PAT_RETRIEVE,
	SLOT_IOCTL_ID_KEEP_ALIVE,
	SLOT_IOCTL_ID_NO_KEEP_ALIVE,
	SLOT_IOCTL_ID_READ_SRAM,
	SLOT_IOCTL_ID_WRITE_SRAM,
	SLOT_IOCTL_ID_ENABLE_WATCHDOG,
	SLOT_IOCTL_ID_DISABLE_WATCHDOG,
	SLOT_IOCTL_ID_READ_SWEVENT,
	SLOT_IOCTL_ID_WRITE_SWEVENT,
	SLOT_IOCTL_ID_ENABLE_TIMER,
	SLOT_IOCTL_ID_DISABLE_TIMER,
	SLOT_IOCTL_ID_READBACK_REG,
	SLOT_IOCTL_ID_GPIO_READ,   // add GPIO-read/write/dirinput/diroutput by cindy 20080715
  SLOT_IOCTL_ID_GPIO_WRITE,  // for xboard lib  
  SLOT_IOCTL_ID_GPIO_DIRINPUT,
  SLOT_IOCTL_ID_GPIO_DIROUTPUT,
  SLOT_IOCTL_ID_GPIO_ALTFUNC, // Cindy 20090408
	SLOT_IOCTL_ID_LAST
};


//-------------- define parameter -----------------
#define YES    1
#define NO     0
#define ON     1
#define OFF    0

#define X_axis          1
#define Y_axis          2
#define Z_axis          3

#define ENC_QUADRANT    0x00
#define ENC_CW_CCW      0x10
#define ENC_PULSE_DIR   0x20

#define I8090  0x0d
#define I8091  0x0e
//#define I8092  0x0f
//#define NOCARD 0x00
//-------------- hardware -------------------------
#define RD1   0x01    //R
#define RD2   0x02    //R
#define RD3   0x03    //R
#define RD4   0x04    //R
#define WR1   0x00    //W
#define WR2   0x01    //W
#define WR3   0x02    //W

//------ card parameter -----------------------------------
typedef struct
{
	unsigned int  base;
	unsigned char ctrl1;
	unsigned char ctrl2;
	unsigned char ctrl3;
	unsigned short  old_pos_X;   //int ---> short ,20081230
	unsigned short  old_pos_Y;
	unsigned short  old_pos_Z;
	unsigned short  pos_X;
	unsigned short  pos_Y;
	unsigned short  pos_Z;
	long          X;
	long          Y;
	long          Z;
//for 8091
	unsigned char exist;
	unsigned char DDA_cycle;
	unsigned char Acc_Dec;
	unsigned int  Low_Speed;
	unsigned int  High_Speed;
}cardtype;
cardtype card[20];




//}I8090_cardtype;
//I8090_cardtype card[20];
//I8091 header
//

#define CW_CCW       0
#define PULSE_DIR    1

#define NORMAL_DIR   0
#define REVERSE_DIR  1

#define FW           0
#define BW           1
#define CW           0
#define CCW          1
#define X_axis       1
#define Y_axis       2
#define Z_axis       3

#define READY 0
#define BUSY  1

#define pi 3.141592654
//-------------- hardware -------------------------
#define ID    0x00    //R  value=0x0E
#define DI1   0x01    //R
#define DI2   0x02    //R
#define WRFF  0x01    //W
#define RSTFF 0x02    //W
//----state----------------------------------
#define PHASE1          1
#define PHASE2          2
#define PHASE3          3

#define sIDLE           0
#define sXYZ            1
#define sLINE           2
#define sCIRCLE         3
#define sARC            4
//----- command sets -----------------
#define sync_pattern1 0x55 //0101 0101
#define sync_pattern2 0xAA //1010 1010

  //-- motion command
  #define LSP_ORG         0x01
  #define HSP_ORG         0x02
  #define HSD_ORG         0x03
  #define LSP_PULSE_MOVE  0x04
  #define HSP_PULSE_MOVE  0x05
  #define LSP_MOVE        0x06
  #define HSP_MOVE        0x07
  #define CSP_MOVE        0x08
  #define SLOW_DOWN       0x09
  #define SLOW_STOP       0x0A
  //-- interpolation command
  #define INTP_PULSE      0x10
  #define INTP_LINE       0x11
  //-- set variable
  #define SET_VAR         0x20
  #define SET_DEFDIR      0x21
  #define SET_MODE        0x22
  #define SET_SERVO_ON    0x23
//  #define SET_REG         0x24
  #define SET_NORMAL_CLOSE 0x25
  //-- stop, break command
  #define RESET_SYSTEM    0x30
  #define STOP_AXIS       0x31
  #define STOP_ALL        0x32
  //-- testing command
  #define RQST_LS         0x40
  #define RQST_MSC        0x41
  #define TEST_FIFO       0x42
  
//******************** for interpolation ****************
#define PROFILE_STOP 1
#define PROFILE_BUSY 2

#define d_DDA_cycle 10
#define d_Acc_Dec   1
#define d_Low_Speed 10
#define d_High_Speed 100

//------ interpolation system parameter --------------------
typedef struct
{
   unsigned char command;
   unsigned char cardNo;
   long          x;
   long          y;
   long          R;
   unsigned char dir;
   unsigned int  speed;
   unsigned char acc_mode;
}system_type;
system_type sys;



/* SLOT IOCTL command */
#define SLOT_MAGIC_NUM  0x57	/* why? my office ID */
#define SLOT_GET_INFO   _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_GET_INFO, slot_devinfo_t *)

#define SLOT_SET_SIG    _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_SET_SIG, slot_signal_t *)

#define SLOT_READ_REG   _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_READ_REG, slot_reg_t *)
#define SLOT_READBACK_REG   _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_READBACK_REG, slot_reg_t *)
#define SLOT_WRITE_REG  _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_WRITE_REG, slot_reg_t *)
#define SLOT_SELECT     _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_SELECT, slot_reg_t *)
#define READ_EEP        _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_READ_EEP, slot_reg_t *)
#define WRITE_EEP       _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_WRITE_EEP, slot_reg_t *)
#define ENABLE_EEP      _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_ENABLE_EEP, slot_reg_t *)
#define DISABLE_EEP     _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_DISABLE_EEP, slot_reg_t *)
#define READ_SN         _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_READ_SN, slot_reg_t *)
#define READ_MAC         _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_READ_MAC, slot_reg_t *)
#define WRITE_MAC        _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_WRITE_MAC, slot_reg_t *)
#define READ_SRAM        _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_READ_SRAM, slot_reg_t *)
#define WRITE_SRAM       _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_WRITE_SRAM, slot_reg_t *)
#define ENABLE_WDT       _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_ENABLE_WATCHDOG, slot_reg_t *)
#define DISABLE_WDT      _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_DISABLE_WATCHDOG, slot_reg_t *)
#define READ_SWEVENT     _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_READ_SWEVENT, slot_reg_t *)
#define WRITE_SWEVENT    _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_WRITE_SWEVENT, slot_reg_t *)
#define ENABLE_TIMER     _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_ENABLE_TIMER, slot_reg_t *)
#define DISABLE_TIMER    _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_DISABLE_TIMER, slot_reg_t *)
#define GPIO_READ        _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_GPIO_READ, slot_reg_t *)
#define GPIO_WRITE       _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_GPIO_WRITE, slot_reg_t *)
#define GPIO_DIRINPUT    _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_GPIO_DIRINPUT, slot_reg_t *)
#define GPIO_DIROUTPUT   _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_GPIO_DIROUTPUT ,slot_reg_t *)
#define GPIO_ALTFUNC     _IOR(SLOT_MAGIC_NUM, SLOT_IOCTL_ID_GPIO_ALTFUNC ,slot_reg_t *)

#ifdef __cplusplus
}
#endif
