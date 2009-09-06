/**********************************************************************
 *
 *  i7k.h
 *
 *  v 0.0.1 2000.1.11 by Reed Lai
 *
 *  Header of i7k.c
 *
 *  History:
 *
 *  v 0.00 1999.10.27 by Reed Lai
 *      create
 *
 **********************************************************************/
#ifndef _I7K_H
#define _I7K_H

#include "sio.h"
#include "timer.h"

#define CMD_BUF_SIZE 128
#define CMD_MAX_SIZE (CMD_BUF_SIZE - 4)

struct i7k {
	int fd;
	unsigned char mod_addr;
	unsigned short int mod_id;
	cc_t time_out;

};


extern char *_i7k_gum_cr(void *cbuf, const char *cmd);
extern char *_i7k_gum_chksum(void *cbuf, const char *cmd);
extern int _i7k_chksum(const char *str);

/* these functions have time-out facility, but less efficient */
/* _cs means check-sum test */
/* readt means read with timout facility(non-canonical mode) */
extern int i7k_send_readt_cs(int fd, const char *cmd, char *rbuf,
							 int rmax, __tod_t * texp);
extern int i7k_send_readt(int fd, const char *cmd, char *rbuf,
						  int rmax, __tod_t * texp);

extern int i7k_readt_cs(int fd, char *rbuf, int rmax, __tod_t * texp);
extern int i7k_readt(int fd, char *rbuf, int rmax, __tod_t * texp);

/* these functions haven't time-out facility, but more efficient */
/* _cs means check-sum test */
extern int i7k_send_read_cs(int fd, const char *cmd, char *rbuf, int rmax);
extern int i7k_send_read(int fd, const char *cmd, char *rbuf, int rmax);

extern int i7k_read_cs(int fd, char *rbuf, int rmax);
extern int i7k_read(int fd, char *rbuf, int rmax);

extern int i7k_send_cs(int fd, const char *cmd);
extern int i7k_send(int fd, const char *cmd);

#endif							/* _I7K_H */
