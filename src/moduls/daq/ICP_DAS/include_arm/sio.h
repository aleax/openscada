/*
 *      sio.h
 *
 *      v 0.00 1999.10.8 by Reed Lai
 *
 *      header of sio.c
 *
 *      History
 *
 *              0.00    create
 */
#ifdef __cplusplus
extern "C" {
#endif
 
#ifndef _SIO_H
#define _SIO_H

#include <stdio.h>				/* Standard input/output definitions */
#include <stdlib.h>
#include <unistd.h>				/* UNIX standard function definitions */
#include <fcntl.h>				/* File control definitions */
#include <errno.h>				/* Error number definitions */
#include <termios.h>			/* POSIX terminal control definitions */

#include "codes.h"
#include "common.h"
#include "debug.h"

/* Error Codes */
#define OK_PORT_CLOSE   0
#define ERR_PORT_CLOSE  -1
#define ERR_PORT_OPEN   0		/* DO NOT CHANGE THIS!! */

/* sio devices */
#define SIO_DEV0     "/dev/ttySA2"
#define SIO_DEV1     "/dev/ttyS0"
#define SIO_DEV2     "/dev/ttyS1"
#define SIO_DEV3     "/dev/ttyS2"
#define SIO_DEV4     "/dev/ttyS3"
#define SIO_DEV5     "/dev/ttyS4"
#define SIO_DEV6     "/dev/ttyS5"
#define SIO_DEV7     "/dev/ttyS6"
#define SIO_DEV8     "/dev/ttyS7"
#define SIO_DEV9     "/dev/ttyS8"
#define SIO_DEV10    "/dev/ttyS9"
#define SIO_DEV11    "/dev/ttyS10"
#define SIO_DEV12    "/dev/ttyS11"
#define SIO_DEV13    "/dev/ttyS12"
#define SIO_DEV14    "/dev/ttyS13"
#define SIO_DEV15    "/dev/ttyS14"
#define SIO_DEV16    "/dev/ttyS15"
#define SIO_DEV17    "/dev/ttyS16"
#define SIO_DEV18    "/dev/ttyS17"
#define SIO_DEV19    "/dev/ttyS18"
#define SIO_DEV20    "/dev/ttyS19"
#define SIO_DEV21    "/dev/ttyS20"
#define SIO_DEV22    "/dev/ttyS21"
#define SIO_DEV23    "/dev/ttyS22"
#define SIO_DEV24    "/dev/ttyS23"
#define SIO_DEV25    "/dev/ttyS24"
#define SIO_DEV26    "/dev/ttyS25"
#define SIO_DEV27    "/dev/ttyS26"
#define SIO_DEV28    "/dev/ttyS27"
#define SIO_DEV29    "/dev/ttyS28"
#define SIO_DEV30    "/dev/ttyS29"
#define SIO_DEV31    "/dev/ttyS30"
#define SIO_DEV32    "/dev/ttyS31"
#define SIO_DEV33    "/dev/ttyS32"
#define SIO_DEV34    "/dev/ttyS33"
#define SIO_DEV35    "/dev/ttyS34"
#define SIO_DEV36    "/dev/ttyS35"
#define SIO_DEV37    "/dev/ttyS36"
#define SIO_DEV38    "/dev/ttyS37"
#define SIO_DEV39    "/dev/ttyS38"
#define SIO_DEV40    "/dev/ttyS39"
#define SIO_DEV41    "/dev/ttyS40"
#define SIO_DEV42    "/dev/ttyS41"
#define SIO_DEV43    "/dev/ttyS42"
#define SIO_DEV44    "/dev/ttyS43"
#define SIO_DEV45    "/dev/ttyS44"
#define SIO_DEV46    "/dev/ttyS45"
#define SIO_DEV47    "/dev/ttyS46"
#define SIO_DEV48    "/dev/ttyS47"
#define SIO_DEV49    "/dev/ttyS48"
#define SIO_DEV50    "/dev/ttyS49"

/* data bits */
#define DATA_BITS_5  CS5
#define DATA_BITS_6  CS6
#define DATA_BITS_7  CS7
#define DATA_BITS_8  CS8

/* parity check */
#define NO_PARITY    0
#define ODD_PARITY   1
#define EVEN_PARITY  2

/* stop bit(s) */
#define ONE_STOP_BIT    0
#define ONE_5_STOP_BIT  1		/* but there is no 1.5 stop bit to be */
				/* defined in Linux POSIX control mode flags */
#define ONE_P5_STOP_BIT 1		/* but there is no 1.5 stop bit defined */
				/* in Linux POSIX control mode flags */
#define TWO_STOP_BIT    2
#define TWO_STOP_BITS   2

struct sio {
	struct sio *next;			/* next opened sio device */
	int fd;						/* file descriptor of this port */
	const char *name;			/* device name */
	struct termios *options;	/* current options of this port */
	struct termios *old_options;	/* original options */
};

extern void sio_set_noncan(int fd);
extern void sio_set_canon(int fd);
extern void sio_set_timer(int fd, cc_t vmin, cc_t vtime);
extern void sio_poptions(int fd);
extern int sio_set_baud(int fd, speed_t baud);
extern int sio_open(const char *port, speed_t baud, tcflag_t data,
					tcflag_t parity, tcflag_t stop);
extern int sio_close(int fd);

#endif							/* _SIO_H */

#ifdef __cplusplus
}
#endif
