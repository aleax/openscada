/* Definitions for ISA DAQ card series.

   Author: Reed Lai

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

/* File level history (record changes for this file here.)

   v 0.10.0 10 Aug 2011 by Golden Wang
     Give support to linux kernel 2.6.X

   v 0.9.0 26 Jun 2003 by Reed Lai
     Defines IXISA_PROC_FILE.

   v 0.8.0  2 May 2003 by Reed Lai
     Gives support to A8111.

   v 0.7.0 29 Apr 2003 by Reed Lai
     Gives support to A826.

   v 0.6.0  9 Apr 2003 by Reed Lai
     Gives support to A822.

   v 0.5.0 18 Nov 2002 by Reed Lai
     Gives support to P8R8DIO and P16R16DIO.

   v 0.4.0 29 Jan 2002 by Reed Lai
     Add A-823.
     Refine some register names.
     Use a flag type for device information (ixisa_devinfo.)
     Add ioctl commands - COUNT, DMA_CONF, DMA_STOP

   v 0.3.0 15 Jan 2002 by Reed Lai
     Add DIO-144.

   v 0.2.0 11 Jan 2002 by Reed Lai
     Rename DAQ to IXISA

   v 0.1.0 24 Jul 2001 by Reed Lai
     Add DIO-48.
     Add a flag - BOTTOM_HELF.

   v 0.0.0 2001.02.12 by Reed Lai
     create, blah blah... */

#ifndef _IXISA_H
#define _IXISA_H

#include <linux/pci.h>
#include <linux/version.h>
#include <linux/types.h>
#include "_flag.h"

#define ICPDAS_LICENSE "GPL"

/* General Definition */
#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef FAILURE
#define FAILURE -1
#endif

#ifndef SA_SHIRQ
#define SA_SHIRQ IRQF_SHARED
#endif

#ifndef SA_INTERRUPT
#define SA_INTERRUPT IRQF_DISABLED
#endif

//#define BOTTOM_HELF
	/* umark to enable the codes of bottom helf of interrupt handler,
	   this is reserved for development only */

/* Names */
#define IXISA_DRIVER_VERSION "0.17.0"
#define ORGANIZATION "icpdas"
#define FAMILY         "ixisa"	/* name of family */
#define DEVICE_NAME    FAMILY	/* device name in /proc/devices */
#define DEVICE_NAME_LEN 5
#define DEVICE_MAJOR   0	/* dynamic allocation of major number */
#define DEVICE_MINOR   0

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
#define IXISA_PROC_FILE "/proc/icpdas/ixisa"
#else
#define IXISA_PROC_FILE "/proc/ixisa"
#endif

#define CARD_NAME_LENGTH 32
#define CNL CARD_NAME_LENGTH

#define A626_NAME	"A626"		
#define A628_NAME	"A628"		
#define A8111_NAME	"A8111"
#define A812PG_NAME	"A812PG"
#define A821_NAME	"A821"
#define A822_NAME	"A822"
#define A823_NAME	"A823"
#define A826_NAME	"A826"
#define DIO24_NAME	"DIO-24"
#define DIO48_NAME	"DIO-48"
#define DIO64_NAME	"DIO-64"
#define DIO96_NAME	"DIO-96"
#define DIO144_NAME	"DIO-144"
#define ISO730_NAME	"ISO-730"	
#define ISO813_NAME	"ISO-813"
#define ISOAD32_NAME	"ISO-AD32"	
#define ISOC64_NAME	"ISO-C64"	
#define ISODA8_NAME	"ISO-DA8"
#define ISODA16_NAME	"ISO-DA16"	
#define ISOLD_NAME	"ISO-LD"	
#define ISOP32C32_NAME	"ISO-P32C32"	
#define ISOP64_NAME	"ISO-P64"	
#define P8R8DIO_NAME	"P8R8DIO"
#define P16R16DIO_NAME	"P16R16DIO"	
#define TMC10_NAME	"TMC-10"
#define ENCODER300_NAME "ENCODER-300"	
#define STEP200_NAME	"STEP-200"	
#define SERVO300_NAME	"SERVO-300"

#define KMSG(fmt, args...) printk(KERN_INFO FAMILY ": " fmt, ## args)

/* Card's id */
enum {
	IXISA_DUMMY,
	A626,
	A628,
	A8111,
	A812PG,
	A821,
	A822,
	A823,
	A826,
	DIO24,
	DIO48,
	DIO64,
	DIO96,
	DIO144,
	ISO730,
	ISO813,
	ISOAD32,
	ISOC64,
	ISODA8,
	ISODA16,
	ISOLD,
	ISOP32C32,
	ISOP64,
	P8R8DIO,
	P16R16DIO,
	TMC10,
	ENCODER300,
	STEP200,
	SERVO300,	
	IXISA_LAST_CARD
};

/* id of registers */
enum {
	IXISA_8254_COUNTER_0,
	IXISA_8254_COUNTER_1,
	IXISA_8254_COUNTER_2,
	IXISA_8254_CONTROL_WORD_REG,
	IXISA_ANALOG_INPUT,
	IXISA_ANALOG_INPUT_L,		/* low byte */
	IXISA_ANALOG_INPUT_H,		/* high byte */
	IXISA_ANALOG_OUTPUT,
	IXISA_ANALOG_OUTPUT_L,		/* low byte */
	IXISA_ANALOG_OUTPUT_H,		/* high byte */
	IXISA_ANALOG_OUTPUT_1,
	IXISA_ANALOG_OUTPUT_1_L,	/* low byte */
	IXISA_ANALOG_OUTPUT_1_H,	/* high byte */
	IXISA_DIGITAL_INPUT,
	IXISA_DIGITAL_INPUT_L,		/* low byte */
	IXISA_DIGITAL_INPUT_H,		/* high byte */
	IXISA_CLEAR_INTERRUPT_REQUEST,
	IXISA_AD_GAIN_CONTROL_REG,
	IXISA_AD_MULTIPLEX_CONTROL_REG,
	IXISA_AD_MODE_CONTROL_REG,
	IXISA_AD_SOFTWARE_TRIGGER_CONTROL_REG,
	IXISA_AD_CONVERSION_READY,
	IXISA_DIGITAL_OUTPUT,
	IXISA_DIGITAL_OUTPUT_L,		/* low byte */
	IXISA_DIGITAL_OUTPUT_H,		/* high byte */
	IXISA_CN0_PORT_A,
	IXISA_CN0_PORT_B,
	IXISA_CN0_PORT_C,
	IXISA_CN0_CONFIGURED_WORD,
	IXISA_CN1_PORT_A,
	IXISA_CN1_PORT_B,
	IXISA_CN1_PORT_C,
	IXISA_CN1_CONFIGURED_WORD,
	IXISA_CLEAR_INTERRUPT,
	IXISA_CN2_PORT_A,
	IXISA_CN2_PORT_B,
	IXISA_CN2_PORT_C,
	IXISA_CN2_CONFIGURED_WORD,
	IXISA_SOFT_INTERRUPT_TRIGGER,
	IXISA_CN3_PORT_A,
	IXISA_CN3_PORT_B,
	IXISA_CN3_PORT_C,
	IXISA_CN3_CONFIGURED_WORD,
	IXISA_CN4_PORT_A,
	IXISA_CN4_PORT_B,
	IXISA_CN4_PORT_C,
	IXISA_CN4_CONFIGURED_WORD,
	IXISA_CN5_PORT_A,
	IXISA_CN5_PORT_B,
	IXISA_CN5_PORT_C,
	IXISA_CN5_CONFIGURED_WORD,
	IXISA_PORT_A,
	IXISA_PORT_B,
	IXISA_PORT_C,
	IXISA_CONFIGURED_WORD,
	IXISA_8254_COUNTER_3,
	IXISA_8254_COUNTER_4,
	IXISA_8254_COUNTER_5,
	IXISA_8254_CONTROL_WORD_REG_1,
	IXISA_DIO_0_TO_7,
	IXISA_DIO_8_TO_15,
	IXISA_DIO_16_TO_23,
	IXISA_DIO_24_TO_31,
	IXISA_ANALOG_OUTPUT_2,
	IXISA_ANALOG_OUTPUT_2_L,	/* low byte */
	IXISA_ANALOG_OUTPUT_2_H,	/* high byte */
	IXISA_ANALOG_OUTPUT_3,
	IXISA_ANALOG_OUTPUT_3_L,	/* low byte */
	IXISA_ANALOG_OUTPUT_3_H,	/* high byte */
	IXISA_ANALOG_OUTPUT_4,
	IXISA_ANALOG_OUTPUT_4_L,	/* low byte */
	IXISA_ANALOG_OUTPUT_4_H,	/* high byte */
	IXISA_ANALOG_OUTPUT_5,
	IXISA_ANALOG_OUTPUT_5_L,	/* low byte */
	IXISA_ANALOG_OUTPUT_5_H,	/* high byte */
	IXISA_ANALOG_OUTPUT_6,
	IXISA_ANALOG_OUTPUT_6_L,	/* low byte */
	IXISA_ANALOG_OUTPUT_6_H,	/* high byte */
	IXISA_ANALOG_OUTPUT_7,
	IXISA_ANALOG_OUTPUT_7_L,	/* low byte */
	IXISA_ANALOG_OUTPUT_7_H,	/* high byte */
	IXISA_DIGITAL_OUTPUT_0_TO_7,
	IXISA_DIGITAL_OUTPUT_8_TO_15,
	IXISA_DIGITAL_INPUT_0_TO_7,
	IXISA_DIGITAL_INPUT_8_TO_15,
	IXISA_STATUS_REGISTER,
	IXISA_READ_WRITE_COMMAND_REGISTER,
	IXISA_CLEAR_INTERRUPT_SIGNAL_REGISTER,
	IXISA_IDIO_0_TO_7,
	IXISA_IDIO_8_TO_15,
	IXISA_DIGITAL_INPUT_16_TO_23,
	IXISA_DIGITAL_INPUT_24_TO_31,
	IXISA_DIGITAL_INPUT_32_TO_39,
	IXISA_DIGITAL_INPUT_40_TO_47,
	IXISA_DIGITAL_INPUT_48_TO_55,
	IXISA_DIGITAL_INPUT_56_TO_63,
	IXISA_DIGITAL_OUTPUT_16_TO_23,
	IXISA_DIGITAL_OUTPUT_24_TO_31,
	IXISA_DIGITAL_OUTPUT_32_TO_39,
	IXISA_DIGITAL_OUTPUT_40_TO_47,
	IXISA_DIGITAL_OUTPUT_48_TO_55,
	IXISA_DIGITAL_OUTPUT_56_TO_63,
	IXISA_FIFO,
	IXISA_SELECT_8254_CHIP,
	IXISA_RSTFIFO1_REGISTER,
	IXISA_MSC_REGISTER,
	IXISA_CTRL1_REGISTER,
	IXISA_CTRL2_REGISTER,
	IXISA_CTRL3_REGISTER,
	IXISA_VALUE1_REGISTER,
	IXISA_VALUE2_REGISTER,
	IXISA_VALUE3_REGISTER,
	IXISA_INDEX_REGISTER,
	IXISA_DI1_REGISTER,
	IXISA_DI2_REGISTER,
	IXISA_STS_REGISTER,
	IXISA_LAST_REG
};

#define IXISA_8254C0   IXISA_8254_COUNTER_0
#define IXISA_8254C1   IXISA_8254_COUNTER_1
#define IXISA_8254C2   IXISA_8254_COUNTER_2
#define IXISA_8254CR   IXISA_8254_CONTROL_WORD_REG
#define IXISA_AI       IXISA_ANALOG_INPUT
#define IXISA_AIL      IXISA_ANALOG_INPUT_L
#define IXISA_AIH      IXISA_ANALOG_INPUT_H
#define IXISA_AO       IXISA_ANALOG_OUTPUT
#define IXISA_AOL      IXISA_ANALOG_OUTPUT_L
#define IXISA_AOH      IXISA_ANALOG_OUTPUT_H
#define IXISA_AO0      IXISA_ANALOG_OUTPUT
#define IXISA_AO0L     IXISA_ANALOG_OUTPUT_L
#define IXISA_AO0H     IXISA_ANALOG_OUTPUT_H
#define IXISA_AO1      IXISA_ANALOG_OUTPUT_1
#define IXISA_AO1L     IXISA_ANALOG_OUTPUT_1_L
#define IXISA_AO1H     IXISA_ANALOG_OUTPUT_1_H
#define IXISA_AO2      IXISA_ANALOG_OUTPUT_2
#define IXISA_AO2L     IXISA_ANALOG_OUTPUT_2_L
#define IXISA_AO2H     IXISA_ANALOG_OUTPUT_2_H
#define IXISA_AO3      IXISA_ANALOG_OUTPUT_3
#define IXISA_AO3L     IXISA_ANALOG_OUTPUT_3_L
#define IXISA_AO3H     IXISA_ANALOG_OUTPUT_3_H
#define IXISA_AO4      IXISA_ANALOG_OUTPUT_4
#define IXISA_AO4L     IXISA_ANALOG_OUTPUT_4_L
#define IXISA_AO4H     IXISA_ANALOG_OUTPUT_4_H
#define IXISA_AO5      IXISA_ANALOG_OUTPUT_5
#define IXISA_AO5L     IXISA_ANALOG_OUTPUT_5_L
#define IXISA_AO5H     IXISA_ANALOG_OUTPUT_5_H
#define IXISA_AO6      IXISA_ANALOG_OUTPUT_6
#define IXISA_AO6L     IXISA_ANALOG_OUTPUT_6_L
#define IXISA_AO6H     IXISA_ANALOG_OUTPUT_6_H
#define IXISA_AO7      IXISA_ANALOG_OUTPUT_7
#define IXISA_AO7L     IXISA_ANALOG_OUTPUT_7_L
#define IXISA_AO7H     IXISA_ANALOG_OUTPUT_7_H
#define IXISA_DI       IXISA_DIGITAL_INPUT
#define IXISA_DIL      IXISA_DIGITAL_INPUT_L
#define IXISA_DIH      IXISA_DIGITAL_INPUT_H
#define IXISA_CIR      IXISA_CLEAR_INTERRUPT_REQUEST
#define IXISA_ADGCR    IXISA_AD_GAIN_CONTROL_REG
#define IXISA_ADMXCR   IXISA_AD_MULTIPLEX_CONTROL_REG
#define IXISA_ADMCR    IXISA_AD_MODE_CONTROL_REG
#define IXISA_ADSTCR   IXISA_AD_SOFTWARE_TRIGGER_CONTROL_REG
#define IXISA_ADCR     IXISA_AD_CONVERSION_READY
#define IXISA_DO       IXISA_DIGITAL_OUTPUT
#define IXISA_DOL      IXISA_DIGITAL_OUTPUT_L
#define IXISA_DOH      IXISA_DIGITAL_OUTPUT_H
#define IXISA_CN0PA    IXISA_CN0_PORT_A
#define IXISA_CN0PB    IXISA_CN0_PORT_B
#define IXISA_CN0PC    IXISA_CN0_PORT_C
#define IXISA_CN0CR    IXISA_CN0_CONFIGURED_WORD
#define IXISA_CN1PA    IXISA_CN1_PORT_A
#define IXISA_CN1PB    IXISA_CN1_PORT_B
#define IXISA_CN1PC    IXISA_CN1_PORT_C
#define IXISA_CN1CR    IXISA_CN1_CONFIGURED_WORD
#define IXISA_CI       IXISA_CIR
#define IXISA_CN2PA    IXISA_CN2_PORT_A
#define IXISA_CN2PB    IXISA_CN2_PORT_B
#define IXISA_CN2PC    IXISA_CN2_PORT_C
#define IXISA_CN2CR    IXISA_CN2_CONFIGURED_WORD
#define IXISA_SIT      IXISA_SOFT_INTERRUPT_TRIGGER
#define IXISA_CN3PA    IXISA_CN3_PORT_A
#define IXISA_CN3PB    IXISA_CN3_PORT_B
#define IXISA_CN3PC    IXISA_CN3_PORT_C
#define IXISA_CN3CR    IXISA_CN3_CONFIGURED_WORD
#define IXISA_CN4PA    IXISA_CN4_PORT_A
#define IXISA_CN4PB    IXISA_CN4_PORT_B
#define IXISA_CN4PC    IXISA_CN4_PORT_C
#define IXISA_CN4CR    IXISA_CN4_CONFIGURED_WORD
#define IXISA_CN5PA    IXISA_CN5_PORT_A
#define IXISA_CN5PB    IXISA_CN5_PORT_B
#define IXISA_CN5PC    IXISA_CN5_PORT_C
#define IXISA_CN5CR    IXISA_CN5_CONFIGURED_WORD
#define IXISA_PA       IXISA_PORT_A
#define IXISA_PB       IXISA_PORT_B
#define IXISA_PC       IXISA_PORT_C
#define IXISA_CR       IXISA_CONFIGURED_WORD
#define IXISA_8254C3   IXISA_8254_COUNTER_3
#define	IXISA_8254C4   IXISA_8254_COUNTER_4
#define IXISA_8254C5   IXISA_8254_COUNTER_5
#define IXISA_8254CR1  IXISA_8254_CONTROL_WORD_REG_1
#define IXISA_DIO_A    IXISA_DIO_0_TO_7
#define IXISA_DIO_B    IXISA_DIO_8_TO_15
#define IXISA_DIO_C    IXISA_DIO_16_TO_23
#define IXISA_DIO_D    IXISA_DIO_24_TO_31
#define IXISA_IDIO_A   IXISA_IDIO_0_TO_7
#define IXISA_IDIO_B   IXISA_IDIO_8_TO_15
#define IXISA_DO_A     IXISA_DIGITAL_OUTPUT_0_TO_7
#define IXISA_DO_B     IXISA_DIGITAL_OUTPUT_8_TO_15
#define IXISA_DO_C     IXISA_DIGITAL_OUTPUT_16_TO_23
#define IXISA_DO_D     IXISA_DIGITAL_OUTPUT_24_TO_31
#define IXISA_DO_E     IXISA_DIGITAL_OUTPUT_32_TO_39
#define IXISA_DO_F     IXISA_DIGITAL_OUTPUT_40_TO_47
#define IXISA_DO_G     IXISA_DIGITAL_OUTPUT_48_TO_55
#define IXISA_DO_H     IXISA_DIGITAL_OUTPUT_56_TO_63
#define IXISA_DI_A     IXISA_DIGITAL_INPUT_0_TO_7
#define IXISA_DI_B     IXISA_DIGITAL_INPUT_8_TO_15
#define IXISA_DI_C     IXISA_DIGITAL_INPUT_16_TO_23
#define IXISA_DI_D     IXISA_DIGITAL_INPUT_24_TO_31
#define IXISA_DI_E     IXISA_DIGITAL_INPUT_32_TO_39
#define IXISA_DI_F     IXISA_DIGITAL_INPUT_40_TO_47
#define IXISA_DI_G     IXISA_DIGITAL_INPUT_48_TO_55
#define IXISA_DI_H     IXISA_DIGITAL_INPUT_56_TO_63
#define IXISA_DI1      IXISA_DI1_REGISTER
#define IXISA_DI2      IXISA_DI2_REGISTER
#define IXISA_SR       IXISA_STATUS_REGISTER
#define IXISA_RWCR     IXISA_READ_WRITE_COMMAND_REGISTER
#define IXISA_CISR     IXISA_CLEAR_INTERRUPT_SIGNAL_REGISTER
#define IXISA_8254CS   IXISA_SELECT_8254_CHIP
#define IXISA_RR       IXISA_RSTFIFO1_REGISTER
#define IXISA_MR       IXISA_MSC_REGISTER
#define IXISA_CTRL1    IXISA_CTRL1_REGISTER
#define IXISA_CTRL2    IXISA_CTRL2_REGISTER
#define IXISA_CTRL3    IXISA_CTRL3_REGISTER
#define IXISA_VALUE1   IXISA_VALUE1_REGISTER
#define IXISA_VALUE2   IXISA_VALUE2_REGISTER
#define IXISA_VALUE3   IXISA_VALUE3_REGISTER
#define IXISA_INDEX    IXISA_INDEX_REGISTER
#define IXISA_STS      IXISA_STS_REGISTER

/* counter */
#define _8254CW_COUNTER_0      0
#define _8254CW_COUNTER_1      0x40
#define _8254CW_COUNTER_2      0x80
#define _8254CW_READ_BACK      0xc0

#define _8254CW_COUNTER_LATCH  0
#define _8254CW_RW_LSB         0x10
#define _8254CW_RW_MSB         0x20
#define _8254CW_RW_LMB         0x30

#define _8254CW_MODE_0         0
#define _8254CW_MODE_1         2
#define _8254CW_MODE_2         4
#define _8254CW_MODE_3         6
#define _8254CW_MODE_4         8
#define _8254CW_MODE_5         0xa

typedef struct ixisa_count {
	unsigned int _8254;			/* the id number of 8254 chip */
	unsigned int id;
	unsigned int value;
	unsigned int mode;
} ixisa_count_t;

/* 16 bit general purpose buffer */
typedef struct ixisa_buf16 {
	u_int16_t *buf;				/* 16 bit buffer */
	unsigned int sz;			/* buffer size */
	unsigned int interval;		/* data at interval of (tiv / clock-frequency) seconds */
} ixisa_buf16_t;

/* dma */
typedef struct ixisa_dma {
	unsigned int nr;			/* DMA number */
	void *buf;					/* raw DMA buffer */
	int sz;						/* DMA buffer size */
	char mode;					/* custom DMA mode (register) */
} ixisa_dma_t;

/* Signal */
typedef struct ixisa_signal {
	int sid;					/* signal id */
	pid_t pid;					/* process id */
	struct task_struct *task;	/* pointer to task structure */
	int is;						/* mask for irq source 0 disable 1 enable */
	int edge;					/* active edge for each irq source
								   0 for negative (falling) edge
								   1 for positive (rising) edge */
	int bedge;					/* both edges, or bipolar. 0 up to the
								   setting in variable edge. 1 does action 
								   for both negative and positive
								   edges, discards setting in variable edge. */
} ixisa_signal_t;

/* Register */
typedef struct ixisa_reg {
	unsigned int id;			/* register's id */
	unsigned int value;			/* register's value for read/write */
	int mode;					/* operation mode */
} ixisa_reg_t;

/* register operation mode */
enum {
	IXISA_RM_NORMAL,			/* read/write directly */
	IXISA_RM_READY,				/* blocked before ready */
	IXISA_RM_TRIGGER,			/* do software trigger before read (also blocked) */
	IXISA_RM_LAST_MODE
};

/* Card definition */
struct ixisa_carddef {
	unsigned int id;			/* card's id */
	unsigned int present;		/* card's present counter */
	char *module;				/* module name, if card is present then
								   load module in this name */
	char *name;					/* card's name */
};

extern struct ixisa_carddef ixisa_card[];

/* flags for device information */
enum {
	IXISA_FLAG_ATTEND,			/* device has been attended by service module */
	IXISA_FLAG_DMA,				/* DMA is configured */
	IXISA_FLAG_LAST_MODE
};

/* Device information */
typedef struct ixisa_devinfo {
	unsigned int id;			/* device or driver id */
	struct ixisa_devinfo *next;	/* next device (ixisa card) */
	struct ixisa_devinfo *prev;	/* previous device */
	struct ixisa_devinfo *next_f;	/* next device in family */
	struct ixisa_devinfo *prev_f;	/* previous device in family */
	unsigned int no;			/* device number (minor number) */
	unsigned int irq;			/* IRQ number */
	ixisa_dma_t dma;			/* DMA number, buffer, size */
	unsigned long base;			/* base I/O address */
	unsigned char admcr;		/* value of A/D Mode Control Register */
	unsigned int open;			/* open counter */
	ixisa_flags_t flags;		/* flags for this device */

//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
//        struct cdev *cdev;
//#else
        struct file_operations *fops;   /* file operations for this device */
//#endif
	char name[CNL];				/* card name information */
	struct ixisa_signal sig;	/* user signal setting for interrupt */
	ixisa_buf16_t *buf16;		/* 16 bit general purpose buffer */
} ixisa_devinfo_t;

/* IDs of ioctl commands */
enum {
	IXISA_IOCTL_ID_GET_INFO,
	IXISA_IOCTL_ID_SET_SIG,
	IXISA_IOCTL_ID_READ_REG,
	IXISA_IOCTL_ID_WRITE_REG,
	IXISA_IOCTL_ID_ADD_DEV,
	IXISA_IOCTL_ID_DEL_DEV,
	IXISA_IOCTL_ID_COUNT_CONF,
	IXISA_IOCTL_ID_COUNT_STOP,
	IXISA_IOCTL_ID_AIDMA_CONF,
	IXISA_IOCTL_ID_AIDMA_CONF_AUTOINIT,
	IXISA_IOCTL_ID_AIDMA_NEXT,
	IXISA_IOCTL_ID_AIDMA_STOP,
	IXISA_IOCTL_ID_AIDMA_HARVEST,
	IXISA_IOCTL_ID_CLEAR_BOARD_IRQ,
	IXISA_IOCTL_ID_LAST
};

/* IOCTL command */
#define IXISA_MAGIC_NUM  0x7e	/* Why? ASCII codes 'I' + 'X' + 'I' + 'S' + 'A' */

#define IXISA_GET_INFO   _IOR(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_GET_INFO, ixisa_devinfo_t *)

#define IXISA_SET_SIG    _IOR(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_SET_SIG, ixisa_signal_t *)

#define IXISA_READ_REG   _IOR(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_READ_REG, ixisa_reg_t *)
#define IXISA_WRITE_REG  _IOR(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_WRITE_REG, ixisa_reg_t *)

#define IXISA_ADD_DEV    _IOR(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_ADD_DEV, ixisa_devinfo_t *)
#define IXISA_DEL_DEV    _IOR(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_DEL_DEV, ixisa_devinfo_t *)

#define IXISA_COUNT_CONF   _IOR(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_COUNT_CONF, ixisa_count_t *)

#define IXISA_AIDMA_CONF   _IOR(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_AIDMA_CONF, ixisa_buf16_t *)
#define IXISA_AIDMA_CONF_AUTOINIT   _IOR(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_AIDMA_CONF_AUTOINIT, ixisa_buf16_t *)
#define IXISA_AIDMA_START  IXISA_AIDMA_CONF
#define IXISA_AIDMA_START_AUTOINIT  IXISA_AIDMA_CONF_AUTOINIT
#define IXISA_AIDMA_NEXT   _IO(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_AIDMA_NEXT)
#define IXISA_AIDMA_STOP   _IO(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_AIDMA_STOP)
#define IXISA_AIDMA_HARVEST   _IO(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_AIDMA_HARVEST)

#define IXISA_CLEAR_BOARD_IRQ    _IOR(IXISA_MAGIC_NUM, IXISA_IOCTL_ID_CLEAR_BOARD_IRQ, ixisa_devinfo_t *)

/* Exported Symbols */
#ifdef __KERNEL__

/* ixisa.o */
void *(_isa_cardname) (int);
void (ixisa_copy_devinfo) (ixisa_devinfo_t *, ixisa_devinfo_t *);
extern ixisa_devinfo_t *ixisa_dev;
extern int ixisa_major;

/* ixtmc10.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixtmc10_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixtmc10_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixtmc10_release(struct inode *, struct file *);
#else
void ixtmc10_release(struct inode *, struct file *);
#endif
int ixtmc10_open(struct inode *, struct file *);

/* ixstep200.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixstep200_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixstep200_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixstep200_release(struct inode *, struct file *);
#else
void ixstep200_release(struct inode *, struct file *);
#endif
int ixstep200_open(struct inode *, struct file *);

/* ixenc300.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixenc300_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixenc300_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixenc300_release(struct inode *, struct file *);
#else
void ixenc300_release(struct inode *, struct file *);
#endif
int ixenc300_open(struct inode *, struct file *);

/* ixservo300.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixservo300_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixservo300_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif
					
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixservo300_release(struct inode *, struct file *);
#else
void ixservo300_release(struct inode *, struct file *);
#endif

int ixservo300_open(struct inode *, struct file *);

/* ixp8r8dio.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixp8r8dio_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixp8r8dio_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif
					
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixp8r8dio_release(struct inode *, struct file *);
#else
void ixp8r8dio_release(struct inode *, struct file *);
#endif

int ixp8r8dio_open(struct inode *, struct file *);

/* ixp16r16dio.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixp16r16dio_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixp16r16dio_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixp16r16dio_release(struct inode *, struct file *);
#else
void ixp16r16dio_release(struct inode *, struct file *);
#endif

int ixp16r16dio_open(struct inode *, struct file *);

/* ixa626.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixa626_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixa626_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixa626_release(struct inode *, struct file *);
#else
void ixa626_release(struct inode *, struct file *);
#endif

int ixa626_open(struct inode *, struct file *);

/* ixa628.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixa628_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixa628_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixa628_release(struct inode *, struct file *);
#else
void ixa628_release(struct inode *, struct file *);
#endif

int ixa628_open(struct inode *, struct file *);

/* ixa8111.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixa8111_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixa8111_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixa8111_release(struct inode *, struct file *);
#else
void ixa8111_release(struct inode *, struct file *);
#endif

int ixa8111_open(struct inode *, struct file *);

/* ixa812pg.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixa812pg_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixa812pg_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixa812pg_release(struct inode *, struct file *);
#else
void ixa812pg_release(struct inode *, struct file *);
#endif

int ixa812pg_open(struct inode *, struct file *);

/* ixa822.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixa822_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixa822_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixa822_release(struct inode *, struct file *);
#else
void ixa822_release(struct inode *, struct file *);
#endif

int ixa822_open(struct inode *, struct file *);

/* ixa823.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixa823_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixa823_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixa823_release(struct inode *, struct file *);
#else
void ixa823_release(struct inode *, struct file *);
#endif

int ixa823_open(struct inode *, struct file *);

/* ixa821.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixa821_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixa821_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixa821_release(struct inode *, struct file *);
#else
void ixa821_release(struct inode *, struct file *);
#endif

int ixa821_open(struct inode *, struct file *);

/* ixa826.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixa826_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixa826_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixa826_release(struct inode *, struct file *);
#else
void ixa826_release(struct inode *, struct file *);
#endif

int ixa826_open(struct inode *, struct file *);

/* ixdio24.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixdio24_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixdio24_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixdio24_release(struct inode *, struct file *);
#else
void ixdio24_release(struct inode *, struct file *);
#endif

int ixdio24_open(struct inode *, struct file *);

/* ixdio48.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixdio48_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixdio48_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixdio48_release(struct inode *, struct file *);
#else
void ixdio48_release(struct inode *, struct file *);
#endif

int ixdio48_open(struct inode *, struct file *);

/* ixdio64.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixdio64_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixdio64_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixdio64_release(struct inode *, struct file *);
#else
void ixdio64_release(struct inode *, struct file *);
#endif

int ixdio64_open(struct inode *, struct file *);

/* ixdio96.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixdio96_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixdio96_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixdio96_release(struct inode *, struct file *);
#else
void ixdio96_release(struct inode *, struct file *);
#endif

int ixdio96_open(struct inode *, struct file *);

/* ixdio144.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixdio144_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixdio144_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixdio144_release(struct inode *, struct file *);
#else
void ixdio144_release(struct inode *, struct file *);
#endif

int ixdio144_open(struct inode *, struct file *);

/* ixiso730.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixiso730_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixiso730_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixiso730_release(struct inode *, struct file *);
#else
void ixiso730_release(struct inode *, struct file *);
#endif

int ixiso730_open(struct inode *, struct file *);

/* ixiso813.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixiso813_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixiso813_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixiso813_release(struct inode *, struct file *);
#else
void ixiso813_release(struct inode *, struct file *);
#endif

int ixiso813_open(struct inode *, struct file *);

/* ixisoad32.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixisoad32_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixisoad32_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixisoad32_release(struct inode *, struct file *);
#else
void ixisoad32_release(struct inode *, struct file *);
#endif

int ixisoad32_open(struct inode *, struct file *);

/* ixisoda16.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixisoda16_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixisoda16_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixisoda16_release(struct inode *, struct file *);
#else
void ixisoda16_release(struct inode *, struct file *);
#endif

int ixisoda16_open(struct inode *, struct file *);

/* ixisoc64.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixisoc64_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixisoc64_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixisoc64_release(struct inode *, struct file *);
#else
void ixisoc64_release(struct inode *, struct file *);
#endif

int ixisoc64_open(struct inode *, struct file *);

/* ixisold.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixisold_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixisold_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixisold_release(struct inode *, struct file *);
#else
void ixisold_release(struct inode *, struct file *);
#endif

int ixisold_open(struct inode *, struct file *);

/* ixisop32c32.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixisop32c32_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixisop32c32_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixisop32c32_release(struct inode *, struct file *);
#else
void ixisop32c32_release(struct inode *, struct file *);
#endif

int ixisop32c32_open(struct inode *, struct file *);

/* ixisop64.o */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
long ixisop64_ioctl(struct file *, unsigned int, unsigned long);
#else
int ixisop64_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int ixisop64_release(struct inode *, struct file *);
#else
void ixisop64_release(struct inode *, struct file *);
#endif

int ixisop64_open(struct inode *, struct file *);

/* _isa.o */
void (ixisa_copy_devinfo) (ixisa_devinfo_t *, ixisa_devinfo_t *);
extern ixisa_devinfo_t *ixisa_dev;
extern int ixisa_major;

/* _proc.o */
int ixisa_proc_init(void);
void ixisa_proc_exit(void);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
#define devfs_register_chrdev(a,b,c) module_register_chrdev(a,b,c)
#define devfs_unregister_chrdev(a,b) module_unregister_chrdev(a,b)
#define ixisa_init(a) init_module(a)
#define ixisa_cleanup(a) cleanup_module(a)
#endif

#endif							/* __KERNEL__ */
#endif							/* _IXISA_H */
