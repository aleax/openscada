/**********************************************************************
 *
 *  codes.h
 *
 *  Since 1999.10.27 by Reed Lai
 *
 **********************************************************************/
/* *INDENT-OFF* */
#ifndef _CODES_H
#define _CODES_H

/***********
 *  ASCII  *
 ***********/

/* The Controls */
#define ASCII_NUL     0			/* ^@ , Null */
#define ASCII_SOH     1			/* ^A , Start of Header */
#define ASCII_STX     2			/* ^B , Start of Text */
#define ASCII_ETX     3			/* ^C , End of Test */
#define ASCII_EOT     4			/* ^D , End of Transmission */
#define ASCII_ENQ     5			/* ^E , Enquiry */
#define ASCII_ACK     6			/* ^F , Acknowledge */
#define ASCII_BEL     7			/* ^G , Bell */
#define ASCII_BS      8			/* ^H , Backspace */
#define ASCII_HT      9			/* ^I , Horizontal Tab */
#define ASCII_TAB ASCII_HT
#define ASCII_LF      10		/* ^J , Line Feed */
#define ASCII_VT      11		/* ^K , Vertical Tab */
#define ASCII_FF      12		/* ^L , Form Feed */
#define ASCII_CR      13		/* ^M , Carriage Return */
#define ASCII_SO      14		/* ^N , Shift Out (Escape Sequence) */
#define ASCII_SI      15		/* ^O , Shift In (Escape Sequence) */
#define ASCII_DLE     16		/* ^P , Data Link Escape */
#define ASCII_DC1     17		/* ^Q , Device Control 1 */
#define ASCII_DC2     18		/* ^R , Device Control 2 */
#define ASCII_DC3     19		/* ^S , Device Control 3 */
#define ASCII_DC4     20		/* ^T , Device Control 4 */
#define ASCII_NAK     21		/* ^U , Negative Acknowledge */
#define ASCII_SYN     22		/* ^V , Synchronous Idle */
#define ASCII_ETB     23		/* ^W , End of Transmission Block */
#define ASCII_CAN     24		/* ^X , Cancel */
#define ASCII_EM      25		/* ^Y , End of Medium */
#define ASCII_SUB     26		/* ^Z , Substitute */
#define ASCII_ESC     27		/* ^[ , Escape Sequence */
#define ASCII_FS      28		/* ^\ , File Separator */
#define ASCII_GS      29		/* ^] , Group Separator */
#define ASCII_RS      30		/* ^^ , Record Separator */
#define ASCII_US      31		/* ^_ , Unit Separator */

/* The Specials */
#define ASCII_SP           32	/* SPACE */
#define ASCII_SPACE        32
#define ASCII_EXCLAMATION  33	/* ! , Exclamation Mark */
#define ASCII_QUOTATION    34	/* " , Quotation Mark */
#define ASCII_NANOGRAM     35	/* # , Nine Squares */
#define ASCII_DOLLAR       36	/* $ , Dollar sign */
#define ASCII_PERCENT      37	/* % , Percent */
#define ASCII_             38	/* & , */
#define ASCII_APOSTROPHE   39	/* ' , Apostrophe, Acute accent */

#define ASCII_COMMA        44	/* , , Comma, Cedilla */

/* The Digits */
#define ASCII_0       48
#define ASCII_1       49
#define ASCII_2       50
#define ASCII_3       51
#define ASCII_4       52
#define ASCII_5       53
#define ASCII_6       54
#define ASCII_7       55
#define ASCII_8       56
#define ASCII_9       57

/* The Specials */
#define ASCII_SEMICOLON    59	/* ; , Semicolon */

#define ASCII_EQUALS       61	/* = , Equals */

#define ASCII_QUESTION     63	/* ? , Question mark */
#define ASCII_COMMERICAL   64	/* @ , Commerical at sign */

/* The Latin Alphabet */
#define ASCII_A       65
#define ASCII_B       66
#define ASCII_C       67
#define ASCII_D       68
#define ASCII_E       69
#define ASCII_F       70
#define ASCII_G       71
#define ASCII_H       72
#define ASCII_I       73
#define ASCII_J       74
#define ASCII_K       75
#define ASCII_L       76
#define ASCII_M       77
#define ASCII_N       78
#define ASCII_O       79
#define ASCII_P       80
#define ASCII_Q       81
#define ASCII_R       82
#define ASCII_S       83
#define ASCII_T       84
#define ASCII_U       85
#define ASCII_V       86
#define ASCII_W       87
#define ASCII_X       88
#define ASCII_Y       89
#define ASCII_Z       90

/* The Specials */
#define ASCII_CIRCUMFLEX   94	/* ^ , Circumflex */

#define ASCII_OPEN         96	/* ` , Opening single, Grave account */

/* The Latin Alphabet */
#define ASCII_a       97
#define ASCII_b       98
#define ASCII_c       99
#define ASCII_d       100
#define ASCII_e       101
#define ASCII_f       102
#define ASCII_g       103
#define ASCII_h       104
#define ASCII_i       105
#define ASCII_j       106
#define ASCII_k       107
#define ASCII_l       108
#define ASCII_m       109
#define ASCII_n       110
#define ASCII_o       111
#define ASCII_p       112
#define ASCII_q       113
#define ASCII_r       114
#define ASCII_s       115
#define ASCII_t       116
#define ASCII_u       117
#define ASCII_v       118
#define ASCII_w       119
#define ASCII_x       120
#define ASCII_y       121
#define ASCII_z       122

/* The Specials */
#define ASCII_TILDE        126	/* ~ , Tilde */
#define ASCII_DEL          127	/* DEL , Delete */

/* This macro turns a 4 bits integer value to ASCII character */
/* for example: i=4 then return '4' , i=11 then return 'B' */
#define ASCII(i)      ( ((i) < 10) ? ((i) + ASCII_0) : ((i) - 10 + ASCII_A) )

#ifdef ENCTRL
#undef ENCTRL
#endif
#define ENCTRL(x)    ((x) & 0x1f)

/*
   The receiver sends the XOFF character when it wishes the sender to pause
   in sending data, and an XON character when it wishes the sender to resume.
 */
#define XON     ASCII_DC1		/* release */
#define XOFF    ASCII_DC3		/* holdoff */

#endif							/* _CODES_H */
