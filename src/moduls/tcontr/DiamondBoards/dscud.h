/*

	DSCUD.H
	
	Diamond Systems Corporation Universal Driver
	Version 5.8

	Copyright (c) Diamond Systems Corporation 2003
	All Rights Reserved.
	
	http://www.diamondsystems.com

	DSCUD currently supports these boards:

	HERCULES EBX
	DIAMOND-MM
	DIAMOND-MM-16-AT
	DIAMOND-MM-32
	DIAMOND-MM-AT
	EMERALD-MM-8
	EMERALD-MM-DIO
	EMERALD-OPTO
	MERCURY
	GARNET-MM
	IR-104
	ONYX-MM
	ONYX-MM-DIO
	OPAL-MM
	PEARL-MM
	PROMETHEUS
	QUARTZ-MM
	RUBY-MM
	RUBY-MM-416
	RUBY-MM-1612
	ZIRCON-MM

*/


#ifndef _DSCUD_H
#define _DSCUD_H

/* MS Visual C++ > 1.52 only - 1-byte structure alignment */
#ifndef MSVC_1_52
#ifdef _MSC_VER
#pragma pack(push, dscud_packing)
#pragma pack(1)
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*/////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////// Type Definitions ////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////// */

#ifndef BYTE
#define BYTE unsigned char
#endif


#ifndef SBYTE
#define SBYTE signed char
#endif


#ifndef WORD
#define WORD unsigned short
#endif


#ifndef SWORD
#define SWORD signed short
#endif


#ifndef DWORD
#define DWORD unsigned long
#endif


#ifndef SDWORD
#define SDWORD signed long
#endif


#ifndef LONG
#define LONG signed long
#endif


#ifndef FLOAT
#define FLOAT float
#endif


#ifndef DFLOAT
#define DFLOAT double
#endif


#ifndef BOOL
#define BOOL int
#endif


#ifndef TRUE
#define TRUE ((BOOL)1)
#endif


#ifndef FALSE
#define FALSE ((BOOL)0)
#endif


#ifndef DSCSAMPLE
#define DSCSAMPLE SWORD
#endif

#ifndef DSCDACODE
#define DSCDACODE DWORD
#endif

typedef void (*DSCUserInterruptFunction) (void* parameter);

#ifdef WIN32
#define DSCUDAPICALL __declspec(dllexport)
#else /* WIN32 not defined */
#define DSCUDAPICALL 
#endif /* #ifdef WIN32 */

#define DSCB SWORD


/*/////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////// Constants ////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////// */

/*///////////
// Version //
////////// */

#define DSC_VERSION  0x0580  /* 0xVVRR - Version & Revision */

/*///////////////
// Board Types //
////////////// */

#define DSC_DMM16    0		
#define DSC_RMM      1
#define DSC_TMM      2
#define DSC_OPMM     3
#define DSC_DMM      4
#define DSC_SMM      5
#define DSC_GMM      6
#define DSC_QMM      7
#define DSC_ZMM      8
#define DSC_PMM      9
#define DSC_OMM      10
#define DSC_RMM416   11
#define DSC_DMM32    12
#define DSC_DMM32AT  12
#define DSC_EMMDIO   13
#define DSC_RMM1612  14
#define DSC_DMMAT    15
#define DSC_DMM16AT  16
#define DSC_IR104    17
#define DSC_EMM8	 18
#define DSC_PROM	 19
#define DSC_HERCEBX  20
#define DSC_CPT      21
#define DSC_DMM48    22
#define DSC_DMM48AT  22
#define DSC_OMMDIO	 23
#define DSC_DIO82C55 24   
#define DSC_MRC      24   
#define DSC_EMMOPTO  24
#define DSC_TEST     126
#define DSC_RAW      127
#define DSC_DRVR     255

/*//////////////////////
// A/D Mode Constants //
///////////////////// */

#define RANGE_5		0
#define RANGE_10	1

#define BIPOLAR		0
#define UNIPOLAR	1

#define GAIN_1		0
#define GAIN_2		1
#define GAIN_4		2
#define GAIN_8		3
#define GAIN_10		1 /* higher gains for SMM boards */
#define GAIN_100	2
#define GAIN_1000	3

#define SCAN_INTERVAL_5		3	/* scan interval in microseconds constant for dscadSetSettings struct */
#define SCAN_INTERVAL_9		4
#define SCAN_INTERVAL_10	2
#define SCAN_INTERVAL_15	1
#define SCAN_INTERVAL_20	0

#define SINGLE_ENDED 0
#define DIFFERENTIAL 1


/*/////////////
// GMM modes //
//////////// */

#define MODE_0_INT11_INT12 0
#define MODE_0_INT21_INT22 1
#define MODE_1_82C55_1_C0  2
#define MODE_1_82C55_1_C3  3
#define MODE_1_82C55_2_C0  4
#define MODE_1_82C55_2_C3  5

/*///////////////
// OMM sources //
////////////// */

#define SOURCE_82C55_1_C0 0
#define SOURCE_COUNTER_0  1
#define SOURCE_82C55_2_C0 2
#define SOURCE_COUNTER_1  3
#define SOURCE_EXTERNAL   4
#define SOURCE_COUNTER_2  5

#define OMM_SOURCE_PIN 0
#define OMM_SOURCE_CLOCK 1
#define OMM_SOURCE_OUT 2


/*////////////////////////////////////////
// Prometheus Watchdog Option Constants //
/////////////////////////////////////// */

/* Look in ZFx86 Training Book for details */

#define PROM_WD_TRIGGER_SCI             0x01
#define PROM_WD_TRIGGER_NMI             0x02
#define PROM_WD_TRIGGER_SMI             0x04
#define PROM_WD_TRIGGER_RESET           0x08
#define PROM_WD_WDI_ASSERT_FALLING_EDGE 0x10
#define PROM_WD_WDO_TRIGGERED_EARLY     0x20
#define PROM_WD_ENABLE_WDI_ASSERTION    0x40


/*////////////////////////////////////////
//	Hercules Watchdog Option Constants	//
/////////////////////////////////////// */

#define HERC_WD_TRIGGER_NMI             0x10
#define HERC_WD_TRIGGER_RESET			0x08
#define HERC_WD_WDI_ASSERT_FALLING_EDGE 0x02
#define HERC_WD_WDO_TRIGGERED_EARLY     0x04
#define HERC_WD_ENABLE_WDI_ASSERTION    0x01


/*/////////////////////////////////////////////////////
// Counter Defines Types for dscCounterSetRateSingle //
//////////////////////////////////////////////////// */

#define COUNTER_0		0x01
#define COUNTER_1		0x02
#define COUNTER_2		0x04
#define COUNTER_0_1		0x08
#define COUNTER_1_2		0x10
#define COUNTER_0_1_2	0x20


/*/////////////////////////////
// Interrupt Operation Types //
//////////////////////////// */

#define OP_TYPE_NONE 0
#define OP_TYPE_INT  1
#define OP_TYPE_DMA  2

#define INT_TYPE_AD      0x01
#define INT_TYPE_DA      0x02
#define INT_TYPE_DIOIN   0x04
#define INT_TYPE_USER    0x08
#define INT_TYPE_COUNTER 0x10
#define INT_TYPE_DIOOUT  0x20
#define INT_TYPE_OPTO	 0x40

#define INT_TYPE_INDEX_AD      0
#define INT_TYPE_INDEX_DA      1
#define INT_TYPE_INDEX_DIOIN   2
#define INT_TYPE_INDEX_USER    3
#define INT_TYPE_INDEX_COUNTER 4
#define INT_TYPE_INDEX_DIOOUT  5
#define INT_TYPE_INDEX_OPTO	   6

/*////////////////////////
// User Interrupt Modes //
/////////////////////// */

#define USER_INT_CANCEL  0
#define USER_INT_AFTER   1
#define USER_INT_INSTEAD 2

#define USER_INT_SOURCE_EXTERNAL 1
#define USER_INT_SOURCE_INTERNAL 0

/*///////////////
// Error Codes //
////////////// */

#define DE_NONE						0   /* no error  */
#define DE_HW_FAILURE				1   /* hardware failed */
#define DE_SW_FAILURE				2   /* software failed */
#define DE_HW_NOT_SUPPORTED			3   /* could not perform operation on this board  */
#define DE_SW_NOT_SUPPORTED			4   /* software does not yet support this operation */
#define DE_INVALID_PARM				5   /* a parameter to the function is invalid */
#define DE_ALTERNATE_IN_PROGRESS	6   /* alternate interrupt function in progress */
#define DE_NONE_IN_PROGRESS			7   /* no interrupt function in progress to pause/cancel/resume */
#define DE_BUFFER_ROLLOVER			8   /* pointer passed in + sizeof data to be written would roll over a segment */
#define DE_INVALID_ERROR_CODE       9   /* we used an invalid error code somewhere */
#define DE_ALREADY_PAUSED			10  /* can't pause an operation--it's already paused */
#define DE_OVERFLOW					11  /* Am9513A counter function overflowed */
#define DE_INVALID_FUNC				12  /* Function number not recognized by board type */
#define DE_DSCUDH_INVALID			13  /* Header - Library version mismatch */
#define DE_INVALID_BOARD			14  /* Invalid board-type constant */
#define DE_BOARD_LIMIT_REACHED		15  /* Tried to initialize too many boards at once */
#define DE_BOARD_BUSY				16  /* Attempt to acquire the board timed out */
#define DE_INVALID_WINDRVR_HANDLE	17  /* Windriver initialization failed */
#define DE_INVALID_WINDRVR_VERSION	18  /* Windriver version conflict */
#define DE_BAD_WINDRVR_BOARD_INIT	19  /* Windriver could not initialize the board */
#define DE_OPERATION_TIMED_OUT		20  /* Operation timed out */
#define DE_INVALID_WINDRVR_KP		21	/* Windriver kernel plugin initialization failed */
#define DE_THREAD_FAILURE			22	/* Windriver failed to initialize thread */

/*/////////////////
// Error Strings //
//////////////// */

#define STR_DE_NONE						"NO ERRORS REPORTED"
#define STR_DE_HW_FAILURE				"HARDWARE FAILURE"
#define STR_DE_SW_FAILURE				"SOFTWARE FAILURE"
#define STR_DE_HW_NOT_SUPPORTED			"OPERATION NOT SUPPORTED BY HARDWARE"
#define STR_DE_SW_NOT_SUPPORTED			"OPERATION NOT SUPPORTED BY SOFTWARE"
#define STR_DE_INVALID_PARM				"INVALID PARAMETER"
#define STR_DE_ALTERNATE_IN_PROGRESS	"ALTERNATE INTERRUPT OPERATION IN PROGRESS"
#define STR_DE_NONE_IN_PROGRESS			"NO INTERRUPT OPERATION IN PROGRESS"
#define STR_DE_BUFFER_ROLLOVER			"POINTER + SIZE OF DATA OVERRUN BUFFER"
#define STR_DE_INVALID_ERROR_CODE       "INVALID ERROR CODE, PLEASE IGNORE"
#define STR_DE_ALREADY_PAUSED			"OPERATION ALREADY PAUSED"
#define STR_DE_OVERFLOW					"AM9513A COUNTER FUNCTION OVERFLOWED OR BUFFER OVERFLOW"
#define STR_DE_INVALID_FUNC				"INVALID FUNCTION FOR SELECTED BOARD"
#define STR_DE_DSCUDH_INVALID			"LIBRARY HEADER VERSION MISMATCH"
#define STR_DE_INVALID_BOARD			"INVALID BOARD TYPE SPECIFIED"
#define STR_DE_BOARD_LIMIT_REACHED		"MAXIMUM NUMBER OF BOARDS REACHED"
#define STR_DE_BOARD_BUSY				"ATTEMPT TO ACQUIRE BOARD TIMED OUT"
#define STR_DE_INVALID_WINDRVR_HANDLE	"INVALID WINDRIVER HANDLE RETURNED"
#define STR_DE_INVALID_WINDRVR_VERSION	"INVALID WINDRIVER VERSION"
#define STR_DE_BAD_WINDRVR_BOARD_INIT	"WINDRIVER BOARD INITIALIZATION FAILURE"
#define STR_DE_OPERATION_TIMED_OUT		"OPERATION TIMED OUT"
#define STR_DE_INVALID_WINDRVR_KP		"KERNEL PLUGIN INITIALIZATION FAILURE"
#define STR_DE_THREAD_FAILURE			"THREAD INITIALIZATION FAILURE"
#define STR_DE_MUTEX_FAIL				"MUTEX CREATION OR LOCKING OPERATION FAILED"


/*/////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////// Structures ///////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////// */

/*/////////
// DSCCB //
//////// */

typedef struct
{
	/*//////////////////////////////////////////////////////////
	// These members are filled automatically by dscInitBoard //
	//    THE USER DOES NOT HAVE TO FILL THESE IN MANUALLY    //
	///////////////////////////////////////////////////////// */
	BYTE boardtype;     /* Contains the boardtype constant */
	DSCB boardnum;      /* The handle to the board */
	
	/*///////////////////////////////////////////////////////////
	// The user must fill in everything below that pertains to //
	// board before the call to dscInitBoard                   //
	////////////////////////////////////////////////////////// */	
	WORD base_address;  /* Base address of the board */

	/*////////////////////////////////////////////////////
	// Use int_level for boards with only one interrupt //
	/////////////////////////////////////////////////// */
	BYTE int_level;     /* Interrupt level of the board */

	/*///////////////////////////////
	// RMM416 and RMM1612 specific //
	////////////////////////////// */
	BOOL RMM_external_trigger;  /* Enable/disable external trigger */

	/*////////////////
	// RMM specific //
	/////////////// */
    BOOL RMM_external_trigger_c3;
    
	/*/////////////////
	// EMM specific  //
	//////////////// */
	WORD EMM_IOAddr[8];         /* IO addresses for up to eight ports */
	WORD EMM_Interrupt[8];      /* Interrupts for up to eight ports */

	/*/////////////////
	// PROM specific //
	//////////////// */
	BYTE clkfrq0;          /* 0 = 10Mhz, 1 = 1MHz */
	BYTE clkfrq1;          /* 0 = 10Mhz, 1 = 100KHz */
	BYTE clksel1;          /* 0 = internal oscillator, frequency set by ckfrq1.  1 = external clock input CLK1 */
    
	/*////////////////
	// RAW specific //
	/////////////// */
	WORD address_space; /* Size of I/O block (in bytes) to allocate.  Only needed for DSC_RAW */ 

	/*//////////////////////////////////////////////////////
	// Obsolete  Kept for backwards compatability only    //
	///////////////////////////////////////////////////// */
	LONG clock_freq;
	BYTE dma_level;
	BYTE int_level1;
	BYTE int_level2;
	BYTE int_level3;
	WORD fpga;
	BYTE SMM_AD_resolution;
	BOOL RMM_mode_16bit;
	BOOL RMM_DAC_0_3_external_enable;  
	BOOL RMM_DAC_4_7_external_enable;

} DSCCB;


/*/////////////////
// DSCADSETTINGS //
//////////////// */

typedef struct
{

	BYTE current_channel; /* INPUT: Which channel to read AD samples from */ 
	BYTE gain;            /* INPUT: 0 = gain of 1, 1 = gain of 2, 2 = gain of 4, 3 = gain of 8 */
	BYTE range;           /* INPUT: 0 = 5V range, 1 = 10V range */
	BYTE polarity;        /* INPUT: 0 = bipolar, 1 = unipolar */
	BYTE load_cal;        /* INPUT: 0 = do not load calibration values, 1 = load */
	BYTE scan_interval;   /* For DMM32, Hercules, DMM48 only. Use constants SCAN_INTERVAL_X */
	BYTE addiff;		  /* For HERCULES only, 0 = SINGLE_ENDED or 1 = DIFFERENTIAL */
	
} DSCADSETTINGS;


/*/////////////
// DSCADSCAN //
//////////// */

typedef struct
{

	BYTE      low_channel;    /* INPUT: Low channel of the scan */
	BYTE      high_channel;   /* INPUT: High channel of the scan */
	DSCSAMPLE *sample_values; /* I/O:   The user must point this to memory that has been allocated and ready to accept (high - low + 1) DSCADSAMPLE values into it */
	BYTE      gain;           /* Obsolete */

} DSCADSCAN;


/*/////////////
// REGPARAMS //
//////////// */

/* This structure is for internal driver use */

typedef struct
{

	DWORD address;
	BYTE  data;

} REGPARAMS;


/*/////////////
// ERRPARAMS //
//////////// */

typedef struct
{

	BYTE ErrCode;    /* OUTPUT: The numerical value of the last error */
	char *errstring; /* OUTPUT: A string description of the last error */

} ERRPARAMS;


/*/////////////
// DSCAIOINT //
//////////// */

typedef struct
{

	DWORD num_conversions;      /* INPUT: total # of conversions to write to sample_values, unless cycling. */
	FLOAT conversion_rate;      /* INPUT: interrupts per second  */
	FLOAT conversion_rate_final;/* OUTPUT: the actual conversion rate programmed into the clock  */
	BOOL  cycle;                /* INPUT: when done with num_conversions, restart at the beginning of the buffer?  */
	BOOL  internal_clock;       /* INPUT: use the internal clock (82c54) ? */
	BYTE  low_channel;          /* INPUT: lowest channel number to perform A/D conversion on  */
	BYTE  high_channel;         /* INPUT: highest channel number to perform A/D conversion on  */
	BOOL  external_gate_enable; /* INPUT: A/D dmm, dmm-16 only  */
	BOOL  internal_clock_gate;  /* INPUT: Set internal gating on or off (refer to manual) */
	DSCSAMPLE  *sample_values;       /* I/O:   pointer to the user's sample buffer */
	BOOL  fifo_enab;            /* INPUT: Fifo operation flag for int sample/scan modes. */
	WORD  fifo_depth;           /* INPUT: Fifo depth (if fifo mode used) */
	DWORD dump_threshold;		/* INPUT: Threshold at which to dump the sample buffer */
	BOOL  channel_align;        /* INPUT: if true, the interrupt will automatically realign the channels each cycle */
	BYTE  clksource;			/* INPUT (PROM only): 0 for 100KHz, 1 for 10MHz internal clock source.*/

} DSCAIOINT;


/*//////////////////////////////
// DSCAUTOCAL, DSCADCALPARAMS //
///////////////////////////// */

typedef struct
{

	BYTE   adrange;             /* INPUT:  A/D Range to calibrate (use 0xFF for all ranges) */
	BYTE   boot_adrange;        /* INPUT:  When cal'ed, this adrange is stored to BOOT values */
	FLOAT  ad_offset, ad_gain;  /* OUTPUT: Difference between target and measured vals. */
	DFLOAT target_values[8];    /* OUTPUT: Target values read from EEPROM */

	BOOL   use_eeprom;          /* Obsolete, but kept in for backwards compatibility */
	                            /* To set the reference values, use dsc(S/G)etReferenceVoltages */
	
} DSCAUTOCAL, DSCADCALPARAMS;


/*////////////////
// SETCALPARAMS //
/////////////// */

/* This structure is here for backwards compatibility, but is no longer used */

typedef struct
{
  
	BYTE chan;          
	BOOL cal_enable;

} SETCALPARAMS;


/*///////////
// DSCDACS //
////////// */

#define DACS_MAX_CHANNELS 16

typedef struct
{

	BOOL channel_enable[DACS_MAX_CHANNELS]; /* INPUT: Which to update.  channel_enable[x] = 1 means that output_codes[x] is valid */
	DSCDACODE *output_codes; /* INPUT: A pointer to the user's array of DA codes */

} DSCDACS;


/*/////////////////
// DSCDASETTINGS //
//////////////// */

typedef struct
{
	BYTE polarity;      /* INPUT: 0 = BIPOLAR, 1 = UNIPOLAR */
	BYTE load_cal;	    /* INPUT: 0 = FALSE, 1= TRUE. Loads calibration from eeprom */
	FLOAT range;		/* INPUT: Absolute value of maximum voltage */

} DSCDASETTINGS;


/*////////////
// DAPARAMS //
/////////// */

typedef struct
{

	BYTE channel;          /* INPUT: Which DA channel to modify */
	DSCDACODE output_code; /* INPUT: The new output value for the DA channel */

} DAPARAMS;


/*//////////////////
// DSCDACALPARAMS //
///////////////// */

typedef struct   
{
	BOOL   fixed;       /* INPUT/OUTPUT: reports the "fixed" status that was detected.  fixed=FALSE means programmable. */
	BOOL   polarity;    /* INPUT/OUTPUT: Bipolar=FALSE, Unipolar=TRUE */
	DFLOAT darange;     /* INPUT:  D/A programmable value to calibrate */
	FLOAT  offset;      /* OUTPUT: Difference between target and measured vals. */
	FLOAT  gain;        /* OUTPUT: Difference between target and measured vals. */
	
	/*///////////////////////////
	// For DMMAT specific use: //
	////////////////////////// */
	BOOL  ch0pol, ch0prog, ch0ext;  /* cho0pol=true: bipolar, ch0prog=true: programmable, ch0ext=true: external; */
	BOOL  ch1pol, ch1prog, ch1ext;
	FLOAT ref;

} DSCDACALPARAMS; /* Single D/A channel */


/*////////
// DSCS //
/////// */

typedef struct
{

	DWORD op_type;            /* OUTPUT: reports the current operation type.  Will usually be either OP_TYPE_NONE or OP_TYPE_INT */
	DWORD transfers;          /* OUTPUT: the number of A/D transfers the operation has performed in the current cycle */
	DWORD total_transfers;    /* OUTPUT: the total number of A/D transfers performed during every cycle */
	DWORD da_transfers;       /* OUTPUT: the number of D/A transfers the operation has performed in the current cycle */
	DWORD da_total_transfers; /* OUTPUT: the total number of D/A transfers performed during every cycle */
	DWORD overflows;          /* OUTPUT: increments this value each time an overflow occurs */

	/* The following are for backwards compatibility, not used anymore */
	BYTE  paused;
	BYTE  output;
	WORD  base_address;
	BYTE  int_level;
	BOOL  analog;	

} DSCS;

/*//////////////////////
// DSCUSERINTFUNCTION //
///////////////////// */

typedef struct
{

	DSCUserInterruptFunction func; /* INPUT:  A pointer to the user's interrupt function */
	BYTE int_mode;                 /* INPUT:  Which user int mode to use */
	DWORD int_type;                /* INPUT:  Which interrupt type to attach this function to (INT_TYPE_XXX) */

} DSCUSERINTFUNCTION;


/*//////////////
// DSCUSERINT //
///////////// */

/* These input values are board specific, please refer to the user's manual */

typedef struct
{
	BYTE  intsource;
	BYTE  counter; 
	BYTE  clksource;
	FLOAT rate;	
	DWORD int_type;    /* OUTPUT: returns the "int_type" that this operation resulted in (INT_TYPE_XXX) */
	DSCUserInterruptFunction func; /* OUTPUT:  A pointer to the user's interrupt function */
	
} DSCUSERINT;

/*/////////
// DSCCS //
//////// */

typedef struct
{

	DWORD value; /* OUTPUT: Counter readback value */
	BYTE status; /* OUTPUT: Counter readback status */

} DSCCS;


/*/////////
// DSCCR //
//////// */

/* Used only in dscCounterRead, otherwise mostly for internal driver use */


typedef struct
{

	BYTE control_code;   
	BYTE counter_number; 
	DWORD counter_data;  /* Counter write data */
	DSCCS counter0;      /* Counter read data */
	DSCCS counter1;
	DSCCS counter2;

} DSCCR;


/*/////////////
// DIOPARAMS // this structure is for internal use only
//////////// */

/* This structure is for internal driver use */

typedef struct 
{	

	BYTE port;         
	BYTE digital_value;
	BYTE bit;

} DIOPARAMS;


/*/////////////
// DSCDIOINT //
//////////// */

typedef struct
{

	WORD num_transfers;   /* INPUT:  Number of conversions to perform */
	BYTE port;            /* INPUT:  for boards w/ >8 bits of dio */
	BYTE size;            /* INPUT:  0-7 for bits 0-7, 8 for full byte, 16 for word */
	BOOL cycle;           /* INPUT:  Set to TRUE to repeat the operation continuously */
	BOOL internal_clock;  /* INPUT:  Set to TRUE to use the internal clock as a trigger, FALSE to use an external trigger */
	BYTE mode;            /* INPUT:  for Garnet only */
	BYTE source;          /* INPUT:  for Onyx only */
	BYTE *digital_values; /* INPUT:  The array that holds the DIO values to send */
	DWORD dump_threshold; /* INPUT:  Threshold at which to dump the sample buffer */

} DSCDIOINT;

/*/////////////
//	DSCEMMDIO // this structure is for use with the EMMDIO function dscEMMDIO[G/S]etState 
//////////// */

typedef struct 
{

	BYTE DIOpins[6];           /* EMMDIO I/O:    contains values to write to pins, or values read from pins - based on use_DIOpins */
	BOOL lock_port[6];         /* EMMDIO INPUT:  values for the lock bits at base+7 */
	BYTE edge_polarity[4];     /* EMMDIO OUTPUT: contains the polarity to each pin on ports 0-2 (each BYTE is one port w/ 8 pins) 0=negative, 1=positive */
	BYTE edge_detect[4];       /* EMMDIO INPUT:  set edge detection enabled (1) or disabled (0) for the 8 pins on each port 0-2 */
	BYTE edge_detect_int[4];   /* EMMDIO OUTPUT: read-only: specifies if an edge was detected on the pin since its port was last cleared */
	BYTE edge_detect_clear[3]; /* EMMDIO INPUT:  set to 1 for ech port 0-2 to clear the edge detection values */
	BOOL use_DIOpins;          /* EMMDIO INPUT:  0 = ignore DIOpins, 1 = use DIOpins */
	BOOL use_lock_port;        /* EMMDIO INPUT:  0 = ignore lock_port, 1 = use lock_port */
	BOOL use_edge_polarity;    /* EMMDIO INPUT:  0 = ignore edge_polarity, 1 = use edge_polarity */
	BOOL use_edge_detect;      /* EMMDIO INPUT:  0 = ignore edge_detect, 1 = use edge_detect */	
	BYTE interrupt_status;     /* EMMDIO OUTPUT: read-only: corresponds to base+6, bits 0-2 are interrupt status for port 0-2 */

} DSCEMMDIO;


/*/////////////////
//	DSCOPTOSTATE // this structure is used for the dscOptoGetState/SetState
//////////////// */

#define DSCUD_MAX_OPTO 8 /* The maximum number of optos that can be reported by the above functions */

typedef struct 
{

	BYTE edge_polarity[DSCUD_MAX_OPTO];		/* READ/WRITE: contains the polarity to each pin, opto 0-3 (each opto has a programmable edge) 0=falling, 1=rising */
	BYTE edge_detect_enab[DSCUD_MAX_OPTO];	/* READ/WRITE:  set edge detection enabled (1) or disabled (0) for 4 pins, opto 0-3 */
	BYTE edge_status[DSCUD_MAX_OPTO];		/* READ: read-only, specifies if an edge was detected on the pin, opto 0-3 */
	BYTE oint_state[DSCUD_MAX_OPTO];		/* READ: read-only, the state of each opto */

	BYTE dmm48at_oint_state;                /* READ: this reports the state of the opto register, base+7, on the DMM48 when the last opto interrupt occured.  Only valid when opto interrupts are enabled. 
													For DMM48 this variable replaces the edge_status array */

} DSCOPTOSTATE;


/*/////////////////////
// DSCEMMDIORESETINT //
//////////////////// */

typedef struct
{

	BOOL use_lock_port;        /* INPUT:  0 = ignore lock_port, 1 = use lock_port */
	BOOL lock_port[6];         /* INPUT:  values for the lock bits at base+7 */
	BYTE edge_detect_clear[3]; /* INPUT:  set to 1 for ech port 0-2 to clear the edge detection values */

} DSCEMMDIORESETINT;


/*///////////////
// DSCWATCHDOG //
////////////// */

typedef struct
{

	WORD wd1;      /* INPUT: WD1 16-bit timer value (runs at ~32KHz - max 2 seconds) */
	BYTE wd2;      /* INPUT: WD2  8-bit timer value (runs at ~32KHz - max 7.2ms) */
	BYTE options;  /* INPUT: option mask (e.g. "PROM_WD_TRIGGER_SMI | PROM_WD_TRIGGER_NMI") 
												or "HERC_WD_TRIGGER_SMI | HERC_WD_TRIGGER_NMI"*/
	
} DSCWATCHDOG;


/*////////////
//	DSCPWM	//
/////////// */

typedef struct
{
	DFLOAT output_freq;  /* Freq in Hz  */
	FLOAT duty_cycle;    /* % of freq you want as duty cycle. Range 0-100 */
	BYTE polarity;       /* polarity of output pulse. 0 for active low, 1 for active high */
	BYTE pwm_circuit;    /* pwm circuit. Range 0-3 */
	BOOL output_enab;    /* 0 for disable, 1 for enable output */

} DSCPWM;

/*/////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////// API Function Prototypes //////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////// */

/*////////////////////////////
// Error-Handling Functions //
/////////////////////////// */

BYTE DSCUDAPICALL dscGetLastError(ERRPARAMS* errparams);
DSCUDAPICALL char* dscGetErrorString(BYTE error_code);

/*////////////////////////////
// Initialization Functions //
/////////////////////////// */

BYTE DSCUDAPICALL dscInit(WORD version);
BYTE DSCUDAPICALL dscFree(void);
BYTE DSCUDAPICALL dscInitBoard(BYTE boardtype, DSCCB* dsccb, DSCB* board);
BYTE DSCUDAPICALL dscFreeBoard(DSCB board);

/*/////////////////
// A/D Functions //
//////////////// */

BYTE DSCUDAPICALL dscADSetSettings(DSCB board, DSCADSETTINGS* settings);
BYTE DSCUDAPICALL dscADSetChannel(DSCB board, BYTE low_channel, BYTE high_channel);
BYTE DSCUDAPICALL dscADSample(DSCB board, DSCSAMPLE* sample);
BYTE DSCUDAPICALL dscADSampleInt(DSCB board, DSCAIOINT* dscaioint);
BYTE DSCUDAPICALL dscADSampleAvg(DSCB board, DFLOAT* average, int count);
BYTE DSCUDAPICALL dscADScan(DSCB board, DSCADSCAN* dscadscan, DSCSAMPLE* sample_values);
BYTE DSCUDAPICALL dscADScanInt(DSCB board, DSCAIOINT* dscaioint);
BYTE DSCUDAPICALL dscADScanAvg(DSCB board, DSCADSCAN* dscadscan, DFLOAT* average_values, int count);

/*/////////////////
// D/A Functions //
//////////////// */

BYTE DSCUDAPICALL dscDASetPolarity(DSCB board, BYTE polarity);
BYTE DSCUDAPICALL dscDASetSettings( DSCB board, DSCDASETTINGS * dasettings);
BYTE DSCUDAPICALL dscDAConvert(DSCB board, BYTE channel, DSCDACODE output_code);
BYTE DSCUDAPICALL dscDAConvertScan(DSCB board, DSCDACS *dscdacs);
BYTE DSCUDAPICALL dscDAConvertScanInt(DSCB board, DSCAIOINT *dscaioint);

/*/////////////////
// DIO Functions //
//////////////// */

BYTE DSCUDAPICALL dscDIOSetConfig(DSCB board, BYTE* config_bytes);
BYTE DSCUDAPICALL dscDIOInputByte(DSCB board, BYTE port, BYTE* digital_value);
BYTE DSCUDAPICALL dscDIOOutputByte(DSCB board, BYTE port, BYTE digital_value);
BYTE DSCUDAPICALL dscDIOInputBit(DSCB board,BYTE port,BYTE bit,BYTE  *digital_value);
BYTE DSCUDAPICALL dscDIOOutputBit(DSCB board,BYTE port, BYTE bit,BYTE digital_value);
BYTE DSCUDAPICALL dscDIOSetBit(DSCB board,BYTE port,BYTE bit);
BYTE DSCUDAPICALL dscDIOClearBit(DSCB board,BYTE port,BYTE bit);
BYTE DSCUDAPICALL dscDIOInputInt(DSCB board,DSCDIOINT *dscdioint);
BYTE DSCUDAPICALL dscDIOOutputInt(DSCB board,DSCDIOINT *dscdioint);
BYTE DSCUDAPICALL dscDMM32DIOOutputByte(DSCB board, BYTE port, BYTE digital_value);
BYTE DSCUDAPICALL dscDMM32DIOInputByte(DSCB board, BYTE port, BYTE  *digital_value);


/*////////////////////
//	Opto Functions	//
/////////////////// */

BYTE DSCUDAPICALL dscOptoInputByte(DSCB board, BYTE port, BYTE * optoValue);
BYTE DSCUDAPICALL dscOptoInputBit(DSCB board, BYTE port, BYTE bit, BYTE * optoValue);
BYTE DSCUDAPICALL dscOptoGetPolarity(DSCB board, BYTE * polarity);


/*////////////////
//	Opto Edge State	//
/////////////// */

BYTE DSCUDAPICALL dscOptoGetState(DSCB board, DSCOPTOSTATE* state);
BYTE DSCUDAPICALL dscOptoSetState(DSCB board, DSCOPTOSTATE* state);

/*/////////////////////
// Counter Functions //
//////////////////// */

BYTE DSCUDAPICALL dscCounterSetRateSingle(DSCB board, float hertz, DWORD ctr); // for ctr, please use the counter set rate single defines on top
BYTE DSCUDAPICALL dscCounterSetRate(DSCB board, float hertz);
BYTE DSCUDAPICALL dscCounterDirectSet(DSCB board, BYTE code, DWORD data, BYTE ctr_number);
BYTE DSCUDAPICALL dscCounterRead(DSCB board, DSCCR* dsccr);

/*/////////////////////////
// Calibration Functions //
//////////////////////// */

BYTE DSCUDAPICALL dscSetCalMux(DSCB board, BOOL on);
BYTE DSCUDAPICALL dscADAutoCal(DSCB board, DSCADCALPARAMS* params);
BYTE DSCUDAPICALL dscDAAutoCal(DSCB board, DSCDACALPARAMS* params);
BYTE DSCUDAPICALL dscADCalVerify(DSCB board, DSCADCALPARAMS* params);
BYTE DSCUDAPICALL dscDACalVerify(DSCB board, DSCDACALPARAMS* params);
BYTE DSCUDAPICALL dscGetReferenceVoltages(DSCB board, DFLOAT* refs);
BYTE DSCUDAPICALL dscSetReferenceVoltages(DSCB board, DFLOAT* refs);
BYTE DSCUDAPICALL dscDAGetOffsets(DSCB board, DFLOAT* offsets, int count);
BYTE DSCUDAPICALL dscDASetOffsets(DSCB board, DFLOAT* offsets, int count);

/*////////////////////
// Relay Functions	//
/////////////////// */

BYTE DSCUDAPICALL dscSetRelay(DSCB board, BYTE relay, BYTE value); // relays start from 0
																   // For ir104, please use dscIR104 functions
BYTE DSCUDAPICALL dscGetRelay(DSCB board, BYTE relay, BYTE* value); // relays start from 0
																	// For ir104, please use dscIR104 functions
BYTE DSCUDAPICALL dscSetRelayMulti(DSCB board, BYTE relayGroup, BYTE value); /*each relayGroup groups 8 relays together. 
																			 ex. relayGroup 0 refers to relays 0-7
																			 relayGroup 1 referes to relays 8-15, etc.
																		     for boards with relays that aren't multiples 
																			 of 8, such as IR104, relays 16-20 are part of
																			 relayGroup 2. The same applies to other boards.
																			 Parameter value is the value to set for each relay. 
																			 Bit0 of value is value set for relay 0, 
																			 bit1 of value is value set for relay 1, etc 
																															*/
BYTE DSCUDAPICALL dscGetRelayMulti(DSCB board, BYTE relayGroup, BYTE * value); /*each relayGroup groups 8 relays together. 
																			 ex. relayGroup 0 refers to relays 0-7
																			 relayGroup 1 referes to relays 8-15, etc.
																		     for boards with relays that aren't multiples 
																			 of 8, such as IR104, relays 16-20 are part of
																			 relayGroup 2. The same applies to other boards.
																			 Parameter value is the value to set for each relay. 
																			 Bit0 of value is value set for relay 0, 
																			 bit1 of value is value set for relay 1, etc */

/*//////////////////////
// WATCHDOG Functions //
///////////////////// */

BYTE DSCUDAPICALL dscWatchdogEnable(DSCB board, WORD timer16, BYTE timer8, SDWORD options);
BYTE DSCUDAPICALL dscWatchdogDisable(DSCB board);
BYTE DSCUDAPICALL dscWatchdogTrigger(DSCB board);


/*////////////////////////////////
//	PULSE WIDTH MOD Functions	//
/////////////////////////////// */

BYTE DSCUDAPICALL dscPWMLoad(DSCB board, BYTE pwm_circuit, BYTE counter, DWORD value);
BYTE DSCUDAPICALL dscPWMConfig(DSCB board, BYTE * config_byte);
BYTE DSCUDAPICALL dscPWMClear(DSCB board, BYTE pwm_circuit);
BYTE DSCUDAPICALL dscPWMFunction(DSCB board, DSCPWM * dscpwm);

/*//////////////////////
// Register Functions //
///////////////////// */

BYTE DSCUDAPICALL dscRegisterRead(DSCB board, WORD address, BYTE* data);
BYTE DSCUDAPICALL dscRegisterWrite(DSCB board, WORD address, BYTE data);
BYTE DSCUDAPICALL dscInp(DWORD address, BYTE* value);
BYTE DSCUDAPICALL dscInpw(DWORD address, WORD* value);
BYTE DSCUDAPICALL dscInpl(DWORD address, DWORD* value);
BYTE DSCUDAPICALL dscInpws(DWORD address, WORD* buffer, WORD n);
BYTE DSCUDAPICALL dscOutp(DWORD address, BYTE value);
BYTE DSCUDAPICALL dscOutpw(DWORD address, WORD value);
BYTE DSCUDAPICALL dscOutpl(DWORD address, DWORD value);
BYTE DSCUDAPICALL dscOutpws(DWORD address, WORD* buffer, WORD n);

/*///////////////////
// Misc. Functions //
////////////////// */

BYTE DSCUDAPICALL dscSleep(DWORD ms);
BYTE DSCUDAPICALL dscGetTime(DWORD* ms);
BYTE DSCUDAPICALL dscGetEEPROM(DSCB board, DWORD address, BYTE* data);
BYTE DSCUDAPICALL dscSetEEPROM(DSCB board, DWORD address, BYTE data);
BYTE DSCUDAPICALL dscSetTrimDac(DSCB board, DWORD trimDac, BYTE value);
BYTE DSCUDAPICALL dscGetStatus(DSCB board, DSCS* status);
BYTE DSCUDAPICALL dscCancelOp(DSCB board);
BYTE DSCUDAPICALL dscCancelOpType(DSCB board, DWORD int_type);
BYTE DSCUDAPICALL dscPauseOp(DSCB board);
BYTE DSCUDAPICALL dscResumeOp(DSCB board);
BYTE DSCUDAPICALL dscSetUserInterruptFunction(DSCB board, DSCUSERINTFUNCTION * dscuserintfunc);
BYTE DSCUDAPICALL dscSetUserInterruptFunctionType(DSCB board, DSCUSERINTFUNCTION * dscuserintfunc, DWORD int_type);
BYTE DSCUDAPICALL dscUserInt(DSCB board, DSCUSERINT * dscuserint, DSCUserInterruptFunction function);
BYTE DSCUDAPICALL dscClearUserInterruptFunction(DSCB board);
BYTE DSCUDAPICALL dscClearUserInterruptFunctionType(DSCB board, DWORD int_type);
BYTE DSCUDAPICALL dscGetBoardMacro(char* boardtype, BYTE* macro);
BYTE DSCUDAPICALL dscGetFPGARev(DSCB board, WORD* fpga);
BYTE DSCUDAPICALL dscSetSystemPriority(DWORD priority);

/*/////////////////////////////
// Unit Conversion Functions //
//////////////////////////// */
BYTE DSCUDAPICALL dscADCodeToVoltage(DSCB board, DSCADSETTINGS adsettings, DSCSAMPLE adcode, DFLOAT *voltage);
BYTE DSCUDAPICALL dscVoltageToADCode(DSCB board, DSCADSETTINGS adsettings, DFLOAT voltage, DSCSAMPLE *adcode);
BYTE DSCUDAPICALL dscDACodeToVoltage(DSCB board, DSCDASETTINGS dasettings, DSCDACODE dacode, DFLOAT *voltage);
BYTE DSCUDAPICALL dscVoltageToDACode(DSCB board, DSCDASETTINGS dasettings, DFLOAT voltage, DSCDACODE *dacode);

/*///////////////
//	IR104	   //
////////////// */

BYTE DSCUDAPICALL dscIR104ClearRelay(DSCB board,BYTE relay);	/*relay range 1-20*/
BYTE DSCUDAPICALL dscIR104SetRelay(DSCB board,BYTE relay);		/*relay range 1-20*/
BYTE DSCUDAPICALL dscIR104RelayInput(DSCB board, BYTE relay, BYTE* value);	/*relay range 1-20*/
BYTE DSCUDAPICALL dscIR104OptoInput(DSCB board,BYTE opto, BYTE* value);	/*opto range 1-20*/

/*////////////////////
// EMMDIO Functions //
/////////////////// */

BYTE DSCUDAPICALL dscEMMDIOGetState(DSCB board, DSCEMMDIO* state);
BYTE DSCUDAPICALL dscEMMDIOSetState(DSCB board, DSCEMMDIO* state);
BYTE DSCUDAPICALL dscEMMDIOResetInt(DSCB board, DSCEMMDIORESETINT* edge);

/*/////////////////////
// QMM SECTION BELOW //
//////////////////// */

/* QMM counter group */
#define QMM_COUNTER_GROUP_1             1
#define QMM_COUNTER_GROUP_2             2

/* QMM fout/counter source */
#define QMM_SOURCE_E1_TC_NM1            0
#define QMM_SOURCE_SRC1                 1
#define QMM_SOURCE_SRC2                 2
#define QMM_SOURCE_SRC3                 3
#define QMM_SOURCE_SRC4                 4
#define QMM_SOURCE_SRC5                 5
#define QMM_SOURCE_GATE1                6
#define QMM_SOURCE_GATE2                7
#define QMM_SOURCE_GATE3                8
#define QMM_SOURCE_GATE4                9
#define QMM_SOURCE_GATE5                10
#define QMM_SOURCE_F1_4MHZ              11
#define QMM_SOURCE_F2_400KHZ            12
#define QMM_SOURCE_F3_40KHZ             13
#define QMM_SOURCE_F4_4KHZ              14
#define QMM_SOURCE_F5_400HZ             15

/* QMM time of day mode */
#define QMM_TOD_DISABLED                0
#define QMM_TOD_DIVIDE_BY_5             1
#define QMM_TOD_DIVIDE_BY_6             2
#define QMM_TOD_DIVIDE_BY_10            3

/* QMM gating control */
#define QMM_NO_GATING                   0
#define QMM_ACTIVE_HIGH_TC_NM1          1
#define QMM_ACTIVE_HIGH_LEVEL_GATE_NP1  2
#define QMM_ACTIVE_HIGH_LEVEL_GATE_NM1  3
#define QMM_ACTIVE_HIGH_LEVEL_GATE_N    4
#define QMM_ACTIVE_LOW_LEVEL_GATE_N     5
#define QMM_ACTIVE_HIGH_EDGE_GATE_N     6
#define QMM_ACTIVE_LOW_EDGE_GATE_N      7

/* QMM output control */
#define QMM_INACTIVE_OUTPUT_LOW         0
#define QMM_ACTIVE_HIGH_PULSE_ON_TC     1
#define QMM_TOGGLE_ON_TC                2
#define QMM_INACTIVE_OUTPUT_HIGH        4
#define QMM_ACTIVE_LOW_PULSE_ON_TC      5

/* QMM counter actions */
#define QMM_ACTION_NONE                 0
#define QMM_ACTION_ARM                  1
#define QMM_ACTION_LOAD                 2
#define QMM_ACTION_LOAD_AND_ARM         3
#define QMM_ACTION_DISARM_AND_SAVE      4
#define QMM_ACTION_SAVE                 5
#define QMM_ACTION_DISARM               6

/* QMM special counter actions */
#define QMM_SPECIAL_CLEAR_TOGGLE_OUTPUT 0
#define QMM_SPECIAL_SET_TOGGLE_OUTPUT   1
#define QMM_SPECIAL_STEP_COUNTER        2
#define QMM_SPECIAL_PROGRAM_ALARM       3

/* QMM frequency intervals */
#define QMM_INTERVAL_1MS_1KHZ           0
#define QMM_INTERVAL_10MS_100HZ         1
#define QMM_INTERVAL_100MS_10HZ         2
#define QMM_INTERVAL_1S_1HZ             3
#define QMM_INTERVAL_10S_01HZ           4

/* QMM master mode register */
typedef struct
{

	BYTE counter_group;
	BYTE fout_divider;
	BYTE fout_source;
	BYTE compare1_enable;
	BYTE compare2_enable;
	BYTE tod_mode;

} DSCQMM_MMR;

/* QMM counter mode register */
typedef struct
{

	BYTE counter;
	BYTE gating_control;
	BYTE active_source_edge;
	BYTE count_source;
	BYTE special_gate;
	BYTE reload_source;
	BYTE cycle;
	BYTE count_type;
	BYTE count_direction;
	BYTE output_control;

} DSCQMM_CMR;

/* QMM multiple counter control */
typedef struct
{

	BYTE group1_action;
	BYTE group1_counter_select;
	BYTE group2_action;
	BYTE group2_counter_select;
	
} DSCQMM_MCC;

/* QMM special counter functions */
typedef struct
{

	BYTE counter;
	BYTE action;
	WORD alarm_value;

} DSCQMM_SCF;



/* QMM pulse width modulation control */
typedef struct
{

	BYTE  init;
	BYTE  counter;
	FLOAT output_freq;
	FLOAT duty_cycle;
	
	DWORD input_freq;
	WORD  load_reg;
	WORD  hold_reg;	
	BYTE  hit_extreme;

} DSCQMM_PWM;



/* QMM Function Prototypes */

BYTE DSCUDAPICALL dscQMMReset(DSCB board);

BYTE DSCUDAPICALL dscQMMSetMMR(DSCB board, DSCQMM_MMR* dscqmmmmr);
BYTE DSCUDAPICALL dscQMMSetCMR(DSCB board, DSCQMM_CMR* dscqmmcmr);
BYTE DSCUDAPICALL dscQMMCounterControl(DSCB board, DSCQMM_MCC* dscqmmmcc, BYTE * status);
BYTE DSCUDAPICALL dscQMMSingleCounterControl(DSCB board, BYTE counter, BYTE action);

BYTE DSCUDAPICALL dscQMMSetLoadRegister(DSCB board, BYTE counter, WORD value);
BYTE DSCUDAPICALL dscQMMSetHoldRegister(DSCB board, BYTE counter, WORD value);
BYTE DSCUDAPICALL dscQMMReadHoldRegister(DSCB board, BYTE counter, WORD* value);

BYTE DSCUDAPICALL dscQMMSpecialCounterFunction(DSCB board, DSCQMM_SCF* dscqmmscf);
BYTE DSCUDAPICALL dscQMMMeasureFrequency(DSCB board, BYTE interval, BYTE source, FLOAT* freq);
BYTE DSCUDAPICALL dscQMMMeasurePeriod(DSCB board, BYTE frequency, FLOAT* period);

BYTE DSCUDAPICALL dscQMMPulseWidthModulation(DSCB board, DSCQMM_PWM* dscqmmpwm);

/* END QMM SECTION */


#ifdef __cplusplus
} /* Closes the extern "C" */
#endif

/* MS Visual C++ only - end 1-byte structure alignment */
#ifndef MSVC_1_52
#ifdef _MSC_VER
#pragma pack(pop, dscud_packing)
#endif
#endif


#endif /* #ifndef _DSCUD_H */

