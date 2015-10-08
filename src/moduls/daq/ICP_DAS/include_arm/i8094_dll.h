#ifndef _I8094_DLL_h_
#define _I8094_DLL_h_

#include "localtypes.h"

//================================================================================
// Axis define
#define AXIS_X			0x1
#define AXIS_Y 			0x2
#define AXIS_Z 			0x4
#define AXIS_U 			0x8

#define SLOT0                     0x00
#define SLOT1                     0x01
#define SLOT2                     0x02
#define SLOT3                     0x03
#define SLOT4                     0x04
#define SLOT5                     0x05
#define SLOT6                     0x06
#define SLOT7                     0x07

#ifndef I8094_MaxModules
   #define I8094_MaxModules	8  /* for the Hardware ID */
#endif

#ifndef AXES_IN_I8094
   #define AXES_IN_I8094	4
#endif

#ifndef INVALID_SLOT_NUMBER
   #define INVALID_SLOT_NUMBER	0xFF
#endif

#ifndef PARAMETER_IGNORED
   #define PARAMETER_IGNORED	0xFF
#endif

#define MOVE_DIRECTION_FORWARD		1
#define MOVE_DIRECTION_REVERSE		0

#define FEEDBACK_SRC_LOGIC_COMMAND	0x0
#define FEEDBACK_SRC_ENCODER_POSITION	0x1

#define MOTION_DONE			1
#define MOTION_NOT_DONE			0

#define CONTINUE_INTERPOLATION_NEXT_READY		1
#define CONTINUE_INTERPOLATION_NEXT_NOT_READY		0

//---------------------------------------------------------------------------------

#define CMPSRC_LOGIC_POSITION		0x00
#define CMPSRC_ENCODE_POSITION		0x01

#define COMP_PLUS_REGISTER		0x00
#define COMP_MINUS_REGISTER		0x01

// Status Codes
#define   YES    1
#define   NO     0
#define   ON     1
#define   OFF    0

#define LOGIC_ACTIVE_LOW		0x0
#define LOGIC_ACTIVE_HIGH		0x1

#define SERVO_OFF			0x0
#define SERVO_ON			0x1

#define SERVO_AUTO_OFF			0x1
#define SERVO_MANUAL_OFF		0x0

#define DISABLE_FEATURE			0x0
#define ENABLE_FEATURE			0x1

#define MOVE_FORWARD			0x1
#define MOVE_REVERSE 			0x0

#define STOP_SLOWDOWN			0x26
#define STOP_SUDDEN			0x27

#define RATIO2_OPPOSITE_DIRECTION	0x1
#define RATIO2_SAME_DIRECTION		0x0

// Synchronous Operation 
#define ENABLE_BLOCK_OPEARTION		ENABLE_FEATURE
#define DISABLE_BLOCK_OPEARTION		DISABLE_FEATURE

// Filter Configuration
#define INT_EVENT_ENABLE_FEATURE	ENABLE_FEATURE 
#define INT_EVENT_DISABLE_FEATURE	DISABLE_FEATURE

// Output Pulse Configuration
#define PULSE_MODE_CW_CCW		0x0000
#define PULSE_MODE_PULSE_DIRECTION	0x0040

#define PULSE_LOGIC_ACTIVE_HIGH		0x0000
#define PULSE_LOGIC_ACTIVE_LOW		0x0080

#define PULSE_FORWARD_ACTIVE_LOW	0x0000
#define PULSE_FORWARD_ACTIVE_HIGH	0x0100

//Encoder Configuration
#define ENCODER_MODE_AB			0x0000  // Quadrature pulse input
#define ENCODER_MODE_AB_DIVID_2		0x0400
#define ENCODER_MODE_AB_DIVID_4		0x0800
#define ENCODER_MODE_CW_CCW		0x0200	// Up/Down pulse input

// Hardware Limit Configuration
#define LIMIT_STOP_SUDDEN		0x0000
#define LIMIT_STOP_SLOWDOWN		0x0004

#define LIMIT_LOGIC_ACTIVE_LOW		0x0000
#define LIMIT_LOGIC_ACTIVE_HIGH		0x0018


// Software Limit Configuration
#define SW_LIMIT_ENABLE_FEATURE		0x0003 
#define SW_LIMIT_DISABLE_FEATURE	0x0000

#define SW_LIMIT_CMP_SRC_LOGIC_COMMAND		0x0000
#define SW_LIMIT_CMP_SRC_ENCODER_POSITION	0x0020


// INP Configuration
#define INP_ENABLE_FEATURE		0x8000
#define INP_DISABLE_FEATURE		0x0000

#define INP_LOGIC_ACTIVE_HIGH		0x4000
#define INP_LOGIC_ACTIVE_LOW		0x0000

// ALARM Configuration
#define ALARM_ENABLE_FEATURE		0x2000
#define ALARM_DISABLE_FEATURE		0x0000

#define ALARM_LOGIC_ACTIVE_HIGH		0x1000
#define ALARM_LOGIC_ACTIVE_LOW		0x0000

// IN3 Configuration
#define IN3_ENABLE_FEATURE		0x0080
#define IN3_DISABLE_FEATURE		0x0000

#define IN3_LOGIC_ACTIVE_HIGH		0x0040
#define IN3_LOGIC_ACTIVE_LOW		0x0000

// Variable Ring Command/Encoder Counter
#define VARIABLE_RING_ENABLE_FEATURE	0x0010
#define VARIABLE_RING_DISABLE_FEATURE	0x0000

// (fixed-pulse Trapezodal motion) Avoid Triangle feature
#define AVOID_TRIANGLE_ENABLE_FEATURE	0x0008
#define AVOID_TRIANGLE_DISABLE_FEATURE	0x0000

// Filter Configuration
#define FILTER_ENABLE_FEATURE		ENABLE_FEATURE 
#define FILTER_DISABLE_FEATURE		DISABLE_FEATURE

#define FILTER_CFG_EMG_EL_ORG_NORG	0x0100
#define FILTER_CFG_ENCODER_Z_PHASE	0x0200
#define FILTER_CFG_INP_ALARM		0x0400
#define FILTER_CFG_EXP_EXPLSN		0x0800
#define FILTER_CFG_IN3			0x1000

#define FILTER_DELAY_2us		0x0000 
#define FILTER_DELAY_256us		0x2000
#define FILTER_DELAY_512us		0x4000
#define FILTER_DELAY_1024us		0x6000
#define FILTER_DELAY_2048us		0x8000
#define FILTER_DELAY_4096us		0xA000
#define FILTER_DELAY_8192us		0xC000
#define FILTER_DELAY_16384us		0xE000

// IN0/NHOME Configuration
#define NHOME_LOGIC_ACTIVE_HIGH		0x0001
#define NHOME_LOGIC_ACTIVE_LOW		0x0000

// IN1/HOME Configuration
#define HOME_LOGIC_ACTIVE_HIGH		0x0004
#define HOME_LOGIC_ACTIVE_LOW		0x0000

// IN2/INDEX Configuration
#define INDEX_LOGIC_ACTIVE_HIGH		0x0010
#define INDEX_LOGIC_ACTIVE_LOW		0x0000

// Auto-Homing Steps Configuration
#define AUTO_HOME_STEP1_DISABLE		0x0000
#define AUTO_HOME_STEP1_FORWARD		0x0001
#define AUTO_HOME_STEP1_REVERSE		0x0003

#define AUTO_HOME_STEP2_DISABLE		0x0000
#define AUTO_HOME_STEP2_FORWARD		0x0004 
#define AUTO_HOME_STEP2_REVERSE		0x000C

#define AUTO_HOME_STEP3_DISABLE		0x0000
#define AUTO_HOME_STEP3_FORWARD		0x0010
#define AUTO_HOME_STEP3_REVERSE		0x0030

#define AUTO_HOME_STEP4_DISABLE		0x0000
#define AUTO_HOME_STEP4_FORWARD		0x0040
#define AUTO_HOME_STEP4_REVERSE		0x00C0

// External Signal Configuration
#define EXP_DISABLE_FEATURE		0x0000
#define EXP_AB_PHASE_MPG		0x0018
#define EXP_FIXED_PULSE_START 		0x0010
#define EXP_CW_CCW_ACTIVE_LOW_MPG	EXP_FIXED_PULSE_START
#define EXP_CONTI_MOVE_ACTIVE		0x0008


#define CMP_SRC_LOGIC_COMMAND		0x0000
#define CMP_SRC_ENCODER_POSITION	0x0020

// Synchronous Action Configuration
#define SYNC_ENABLE_FEATURE		ENABLE_FEATURE
#define SYNC_DISABLE_FEATURE		DISABLE_FEATURE

// Synchronization Factor/Provocative
#define SYNC_CONDITION_NONE			0x0000
#define SYNC_CONDITION_EXCEED_CMP_POSITIVE	0x0001
#define SYNC_CONDITION_LESS_CMP_POSITIVE	0x0002
#define SYNC_CONDITION_LESS_CMP_NEGATIVE	0x0004
#define SYNC_CONDITION_EXCEED_CMP_NEGATIVE	0x0008
#define SYNC_CONDITION_START_DRIVING		0x0010
#define SYNC_CONDITION_END_DRIVING		0x0020
#define SYNC_CONDITION_IN3_RAISING_EDGE		0x0040
#define SYNC_CONDITION_IN3_FALLING_EDGE		0x0080

// Synchronization Action
#define SYNC_ACTION_NONE			0x0000
#define SYNC_ACTION_FIXED_FORWARD_DRIVE		0x0001
#define SYNC_ACTION_FIXED_REVERSE_DRIVE		0x0002
#define SYNC_ACTION_CONTINUE_FORWARD_DRIVE	0x0004
#define SYNC_ACTION_CONTINUE_REVERSE_DRIVE	0x0008
#define SYNC_ACTION_SLOWDOWN_STOP		0x0010
#define SYNC_ACTION_SUDDEN_STOP			0x0020
#define SYNC_ACTION_LOGIC_CMD_LATCH		0x0040
#define SYNC_ACTION_ENCODER_POS_LATCH		0x0080
#define SYNC_ACTION_WR6_WR7_TO_LOGIC_CMD	0x0100
#define SYNC_ACTION_WR6_WR7_TO_ENCORDER_POS	0x0200
#define SYNC_ACTION_WR6_WR7_TO_PULSE_REG	0x0400
#define SYNC_ACTION_WR6_TO_VELOCITY_REG		0x0800
//---------------------------------------------------------------------------------

// Interporation Arc move
#define INTERP_ARC_DIRECTION_CLOCKWISE		0x0032
#define INTERP_ARC_DIRECTION_COUNTER_CLOCKWISE  0x0033

// Interporation Continue feature
#define INTERP_CONTINUE_ENABLE_FEATURE		ENABLE_FEATURE
#define INTERP_CONTINUE_DISABLE_FEATURE		DISABLE_FEATURE

// Interporation Continue feature
#define INTERP_CONTINUE_START			0x0000
#define INTERP_NEXT_CONTINUOUS_MOTION		0x0001

#define INTERP_CONTINUE_STOP_CHECK_SKIP		0x0000
#define INTERP_CONTINUE_STOP_CHECK_NEED		0x0001

// INT Factors settings
#define INT_FACTOR_DISABLE			0x0000
#define INT_FACTOR_PULSE			0x0100
#define INT_FACTOR_EXCEED_CMP_NEGATIVE		0x0200
#define INT_FACTOR_LESS_CMP_NEGATIVE		0x0400
#define INT_FACTOR_LESS_CMP_POSITIVE		0x0800
#define INT_FACTOR_EXCEED_CMP_POSITIVE		0x1000
#define INT_FACTOR_END_CONST_SPEED_MOVE		0x2000
#define INT_FACTOR_START_CONST_SPEED_MOVE	0x4000
#define INT_FACTOR_END_DRIVING			0x8000
#define INT_FACTOR_END_AUTO_HOMING		0x0020
#define INT_FACTOR_SYNC_ACTION			0x0080 

// INT active status
#define INT_STATUS_PLUSE			0x0001
#define INT_STATUS_EXCEED_CMP_NEGATIVE		0x0002
#define INT_STATUS_LESS_CMP_NEGATIVE		0x0004
#define INT_STATUS_LESS_CMP_POSITIVE		0x0008
#define INT_STATUS_EXCEED_CMP_POSITIVE		0x0010
#define INT_STATUS_END_CONST_SPEED_MOVE		0x0020
#define INT_STATUS_START_CONST_SPEED_MOVE	0x0040
#define INT_STATUS_END_DRIVING			0x0080
#define INT_STATUS_END_AUTO_HOMING		0x0100
#define INT_STATUS_SYNC_ACTION			0x0200 

#define INT_STATUS_ENABLE_DIRECT_ACCESS		ENABLE_FEATURE
#define INT_STATUS_DISABLE_DIRECT_ACCESS	DISABLE_FEATURE

// Returned Status Definition
// ps400_motion_done();
#define DRIVE_FINISH_WITH_SW_LIMIT_POSITIVE	0x0001
#define DRIVE_FINISH_WITH_SW_LIMIT_NEGATIVE	0x0002
#define DRIVE_FINISH_WITH_STOP_COMMAND		0x0004
#define DRIVE_FINISH_OUTPUT_FIXED_PULSE		0x0080
#define DRIVE_FINISH_WITH_AUTO_HOME		0x0100

#define DRIVE_FINISH_WITH_LIMIT_POSITIVE	0x1000
#define DRIVE_FINISH_WITH_LIMIT_NEGATIVE	0x2000
#define DRIVE_FINISH_WITH_ALARM			0x4000
#define DRIVE_FINISH_WITH_EMG			0x8000

// ps400_get_error_status();
#define DRIVE_ERROR_STATUS_SLMTP		0x0001
#define DRIVE_ERROR_STATUS_SLMTM		0x0002
#define DRIVE_ERROR_STATUS_LMTP			0x0004
#define DRIVE_ERROR_STATUS_LMTM			0x0008
#define DRIVE_ERROR_STATUS_ALARM		0x0010
#define DRIVE_ERROR_STATUS_EMG			0x0020
#define DRIVE_ERROR_STATUS_HOME			0x0080

// DI Status Configuration
#define DI_STATUS_ALARM				0x0010
#define DI_STATUS_HOME				0x0020
#define DI_STATUS_NEARHOME			0x0040
#define DI_STATUS_INPUT3			0x0080
#define DI_STATUS_INP				0x0100
#define DI_STATUS_INDEX				0x0200


#define DI_STATUS_ACTIVE_DRIVING		0x0001
#define DI_STATUS_ACTIVE_LMTP			0x0002
#define DI_STATUS_ACTIVE_LMTM			0x0004
#define DI_STATUS_ACTIVE_EMG			0x0008
#define DI_STATUS_ACTIVE_ALARM			DI_STATUS_ALARM
#define DI_STATUS_ACTIVE_HOME			DI_STATUS_HOME
#define DI_STATUS_ACTIVE_NEARHOME		DI_STATUS_NEARHOME
#define DI_STATUS_ACTIVE_INP			DI_STATUS_INP
#define DI_STATUS_ACTIVE_INDEX			DI_STATUS_INDEX

#ifdef _INTERRUPT_SUPOORT_
#define FRNET_PERIODIC_READING_ENABLE_FEATURE	ENABLE_FEATURE
#define FRNET_PERIODIC_READING_DISABLE_FEATURE	DISABLE_FEATURE
#endif // _INTERRUPT_SUPOORT_

#define FRNET_ENABLE_DIRECT_ACCESS		ENABLE_FEATURE 
#define FRNET_DISABLE_DIRECT_ACCESS		DISABLE_FEATURE

// FRNET configuration
#define FRNET_RA0				0x0
#define FRNET_RA1				0x1
#define FRNET_RA2				0x2
#define FRNET_RA3				0x3
#define FRNET_RA4				0x4
#define FRNET_RA5				0x5
#define FRNET_RA6				0x6
#define FRNET_RA7				0x7

#define FRNET_SA8				0x8
#define FRNET_SA9				0x9
#define FRNET_SA10				0xA
#define FRNET_SA11				0xB
#define FRNET_SA12				0xC
#define FRNET_SA13				0xD
#define FRNET_SA14				0xE
#define FRNET_SA15				0xF


#define SUCCESS_NO_ERROR                          0

// indicate the unsuccessful routine-execution
#define ERROR_ROUTINE_FAIL_BASE                   -100
#define ERROR_GET_CARD_ID                         -101
#define ERROR_DEVICE_OPEN                         -102
#define ERROR_DEVICE_CLOSE                        -103
#define ERROR_CARD_RESET                          -104
#define ERROR_RANGE_CHANGE                        -105
#define ERROR_PULSE_MODE_SET                      -106
#define ERROR_ENCODER_MODE_SET                    -107
#define ERROR_LIMIT_SENSOR_SET                    -108
#define ERROR_INP_SIGNAL_SET                      -109
#define ERROR_ALARM_SIGNAL_SET                    -110
#define ERROR_SERVO_ON_SET                        -111
#define ERROR_IN3_SET                             -112
#define ERROR_IN3_GET                             -113
#define ERROR_FILTER_SET                          -114
#define ERROR_SW_LIMIT_SET                        -115
#define ERROR_HOME_CFG_SET                        -116
#define ERROR_HOME_LIMIT_SET                      -117
#define ERROR_START_HOME                          -118
#define ERROR_DI_STATUS_GET                       -119
#define ERROR_ERROR_STATUS_GET                    -120
#define ERROR_CMD_COUNTER_SET                     -121
#define ERROR_CMD_COUNTER_GET                     -122
#define ERROR_POS_COUNTER_SET                     -123
#define ERROR_POS_COUNTER_GET                     -124
#define ERROR_MOTION_DONE_GET                     -125
#define ERROR_SPEED_GET                           -126
#define ERROR_ACCELERATION_GET                    -127
#define ERROR_LATCH_GET                           -128
#define ERROR_MOTION_STOP_SET                     -129
#define ERROR_MOTION_STOP_ALL_SET                 -130
#define ERROR_DRIVE_START                         -131
#define ERROR_DRIVE_HOLD                          -132
#define ERROR_VRING_SET                           -133
#define ERROR_MPG_SET                             -134
#define ERROR_CMPTRIG_SET                         -135
#define ERROR_SYNCH_SET                           -136
#define ERROR_INT_FACTOR_SET                      -137
#define ERROR_INT_STATUS_GET                      -138
#define ERROR_CONTI_MOVE_START                    -139
#define ERROR_CONST_MOVE_START                    -140
#define ERROR_T_MOVE_START                        -141
#define ERROR_S_MOVE_START                        -142
#define ERROR_T_LINE2_START                       -143
#define ERROR_T_LINE3_START                       -144
#define ERROR_S_LINE2_START                       -145
#define ERROR_S_LINE3_START                       -146
#define ERROR_T_ARC2_START                        -147
#define ERROR_CONTI_INTERP_SET                    -148
#define ERROR_CONTI_INTERP_CLEAR                  -149
#define ERROR_CONTI_INTERP_NEXT_READY             -150
#define ERROR_CONTI_INTERP_LINE2_MOVE             -151
#define ERROR_CONTI_INTERP_LINE3_MOVE             -152
#define ERROR_CONTI_INTERP_ARC2_MOVE              -153
#define ERROR_T_DRIVING_SPEED_CHANGE              -154
#define ERROR_T_AVOID_TRIANGLE_SET                -155
#define ERROR_OUTPUT_PULSE_CHANGE                 -156
#define ERROR_OUT1_GET                            -157
#define ERROR_FRNET_DI_MODULE_GET                 -158
#define ERROR_FRNET_FREQUENCY_SET                 -159
#define ERROR_FRNET_INPUT                         -160
#define ERROR_FRNET_OUTPUT                        -161
#define ERROR_FRNET_RESET                         -162

// for internal-use functions
#define ERROR_CPLD_VERSION_GET                    -191
#define ERROR_DIP_SW_GET                          -192

// Parameters Error
#define ERROR_INVALID_PARAMETER_BASE              -200
#define ERROR_INVALID_CARD_ID                     -201
#define ERROR_INVALID_SCANNED_INDEX               -202
#define ERROR_CARD_ID_DUPLICATED                  -203
#define ERROR_INVALID_RANGE                       -204
#define ERROR_INVALID_PULSE_MODE                  -205
#define ERROR_INVALID_PULSE_LEVEL                 -206
#define ERROR_INVALID_PULSE_DIRECTION             -207
#define ERROR_INVALID_ENCODER_MODE                -208
#define ERROR_INVALID_LIMIT_LOGIC                 -209
#define ERROR_INVALID_STOP_MODE                   -210
#define ERROR_INVALID_INP_ENABLE                  -211
#define ERROR_INVALID_INP_LOGIC_LEVEL             -212
#define ERROR_INVALID_ALARM_ENABLE                -213
#define ERROR_INVALID_ALARM_LOGIC_LEVEL           -214
#define ERROR_INVALID_SERVO_SETTING               -215
#define ERROR_INVALID_IN3_ENABLE                  -216
#define ERROR_INVALID_IN3_LOGIC_LEVEL             -217
#define ERROR_INVALID_FILTER_ENABLE               -218
#define ERROR_INVALID_FILTER_CONFIGURATION        -219
#define ERROR_INVALID_FILTER_DELAY_TIME           -220
#define ERROR_INVALID_SOFTWARE_LIMIT_ENABLE       -221
#define ERROR_INVALID_SOFTWARE_LIMIT_COMPARATOR_SOURCE	-222
#define ERROR_INVALID_MOVE_DIRECTION              -223
#define ERROR_INVALID_HOME_LOGIC_LEVEL            -224
#define ERROR_INVALID_NEAR_HOME_LOGIC_LEVEL       -225
#define ERROR_INVALID_INDEX_LOGIC_LEVEL           -226
#define ERROR_INVALID_AUTO_HOME_STEP              -227
#define ERROR_INVALID_BLOCK_OPEARTION_MODE        -228
#define ERROR_INVALID_AVOID_TRIANGLE_CONFIG       -229
#define ERROR_INVALID_MPG_EXP_CONFIG              -230
#define ERROR_INVALID_NHOME_SEARCH_SPEED          -231
#define ERROR_INVALID_HOME_SEARCH_SPEED           -232
#define ERROR_INVALID_ACCELERATION                -233
#define ERROR_INVALID_DECELERATION                -234
#define ERROR_INVALID_JERK                        -235
#define ERROR_INVALID_DECELERATION_RATE           -236
#define ERROR_INVALID_RING_COUNTER                -237
#define ERROR_INVALID_RING_ENABLE	          -238
#define ERROR_INVALID_AXIS                        -239
#define ERROR_INVALID_CONST_PITCH                 -240
#define ERROR_INVALID_OFFSET_BUFFER               -241
#define ERROR_INVALID_OFFSET_LEN                  -242
#define ERROR_INVALID_OFFSET_DATA                 -243
#define ERROR_INVALID_START_SPEED                 -244
#define ERROR_INVALID_DRIVING_SPEED               -245
#define ERROR_INVALID_MANUAL_DECELERATION_POINT   -246
#define ERROR_START_SPEED_EXCEED_DRIVING_SPEED    -247
#define ERROR_MULTI_AXES_ASSIGNED                 -248
#define ERROR_NO_VALID_AXIS_ASSIGNED              -249
#define ERROR_INVALID_INTERPOLATION_SLAVE_AXES    -250
#define ERROR_INTERPOLATION_SLAVE_AXES_DUPLICATED -251
#define ERROR_INVALID_SYNCHRONOUS_AXES            -252
#define ERROR_INVALID_INTERPOLATION_ARC_DIRECTION	-253
#define ERROR_INVALID_CONTINUE_INTERPOLATION_MOTION	-254
#define ERROR_INVALID_FRNET_PERIODIC_ENABLE       -255
#define ERROR_INVALID_FRNET_PERIODIC_FACTOR       -256
#define ERROR_INVALID_FRNET_SA_GROUP_ADDRESS      -257
#define ERROR_INVALID_FRNET_RA_GROUP_ADDRESS      -258
#define ERROR_INVALID_FRNET_ACCESS_MODE		  -259
#define ERROR_INVALID_COMPARE_SOURCE	          -260
#define ERROR_INVALID_MPG_SPEED                   -261
#define ERROR_INVALID_CMPTRIG_ENABLE		  -262
#define ERROR_INVALID_CMPTRIG_TRIGGER_MODE	  -263
#define ERROR_INVALID_CMPTRIG_LOGIC_LEVEL	  -264
#define ERROR_INVALID_CMPTRIG_PULSE_WIDTH	  -265
#define ERROR_INVALID_SYNCH_ENABLE		  -266
#define ERROR_INVALID_SYNCH_CONDITION		  -267
#define ERROR_INVALID_SYNCH_ACTION		  -268
#define ERROR_INVALID_EVENT_ENABLE		  -269
#define ERORR_INVALID_INT_FACTOR                  -270
#define ERORR_INVALID_FEEDBACK_SOURCE             -271

// RunTime Error
#define ERROR_RUNTIME_BASE                        -300
#define ERROR_OCCURS_IN_AXIS_X                    -301
#define ERROR_OCCURS_IN_AXIS_Y                    -302
#define ERROR_OCCURS_IN_AXIS_XY                   -303
#define ERROR_OCCURS_IN_AXIS_Z                    -304
#define ERROR_OCCURS_IN_AXIS_XZ                   -305
#define ERROR_OCCURS_IN_AXIS_YZ                   -306
#define ERROR_OCCURS_IN_AXIS_XYZ                  -307
#define ERROR_OCCURS_IN_AXIS_U                    -308
#define ERROR_OCCURS_IN_AXIS_XU                   -309
#define ERROR_OCCURS_IN_AXIS_YU                   -310
#define ERROR_OCCURS_IN_AXIS_XYU                  -311
#define ERROR_OCCURS_IN_AXIS_ZU                   -312
#define ERROR_OCCURS_IN_AXIS_XZU                  -313
#define ERROR_OCCURS_IN_AXIS_YZU                  -314
#define ERROR_OCCURS_IN_AXIS_XYZU                 -315

#define	ERROR_NO_CARD_FOUND                       -316
#define ERROR_MEMORY_MAP                          -317
#define ERROR_MEMORY_UNMAP                        -318
#define ERROR_ACCESS_VIOLATION_DATA_COPY          -319
#define ERROR_VARIABLE_PITCH_SET                  -320
#define	ERROR_INT_EVENT_ATTACH                    -321
#define ERROR_INT_EVENT_DETTACH                   -322
#define ERROR_INT_EVENT_CREATE                    -323
#define ERROR_CONFIG_IS_NEEDED                    -324
#define ERROR_MOTION_NOT_COMPLETE                 -325
#define ERROR_CONFLICT_WITH_SOFTLIMIT             -326
#define ERROR_CONFLICT_WITH_CMPTRIG               -327
#define ERROR_CONFLICT_WITH_VRING                 -328
#define ERROR_CONFLICT_WITH_SYNCH_ACTION          -329
#define ERROR_ARC_DECELERATION_POINT_CALCULATE    -330
#define ERROR_REASSIGN_SYNCH_MODE_COMMAND         -331
#define ERROR_OVERLAP_EVENT_CREATE                -332
#define ERROR_INTERPOLATION_NOT_COMPLETE          -333
#define ERROR_CONTI_INTERP_INTERRUPTED            -334
#define ERROR_CONTI_INTERP_INCORRECT_CONFIG       -335
#define ERROR_CONTI_INTERP_NEXT_NOT_READY         -336
#define ERROR_SPEED_CHANGE_FAIL_IN_ACC_DEC        -337
#define ERROR_INVALID_OPERATION_IN_S_CURVE        -338
#define ERROR_NOT_CONSTANT_SPEED_IN_T_MOVE        -339
#define ERROR_MOTION_IS_COMPLETED                 -340
#define ERROR_CONFLICT_WITH_INTERPOLATION_MOVE    -341
#define ERROR_HOLD_AXES_NOT_MATCH                 -342
#define ERROR_BLOCK_OP_CONFLICT_WITH_CMPTRIG      -343
#define ERROR_CONFLICT_WITH_MPG			  -344
#define ERROR_HOLD_AXES_NOT_RELEASE               -345
#define ERROR_ZPHASE_ACTIVE_AT_STEP3              -346
#define ERROR_BLOCK_OP_CONFLICT_WITH_DRV_HOLD     -347

#define ERROR_AXES_MOVE_CHECK                     -360
#define ERROR_IOCTL_FAILED                        -361
#define ERROR_UNDEFINED_EXCEPTION                 -362

#define ERROR_CONFIG_FILE_LOAD                    -370
#define ERROR_CONFLICT_IN_CONFIG_FILE             -371
#define ERROR_INVALID_FILE_HANDLE                 -372

#define ERROR_ACCESS_REGISTRY_ACTIVE_ROOT         -375
#define ERROR_ACCESS_REGISTRY_ACTIVE_NUMBER       -376

#define ERROR_UNSUPPORTED_FUNCTION                -380

#ifdef _ABSOLUTE_POSITION_SUPPORT_
#define ERROR_INVALID_ABS_ADDRESS_RECORD          -390
#endif

#ifndef _DEF_I8094_AXIS_RANGE_SETTINGS
#define _DEF_I8094_AXIS_RANGE_SETTINGS

typedef struct _AXIS_RANGE_SETTINGS_
{
   DWORD AcceRate_Max; // for Jerk & Deceleration
   DWORD AcceRate_Min;
   DWORD Acce_Min; // for Acceleration & Deceleration
   DWORD Acce_Max;
   DWORD Speed_Min; // for Initial Speed & Driving Speed
   DWORD Speed_Max;
} AXIS_RANGE_SETTINGS, *PAXIS_RANGE_SETTINGS;

#endif // _DEF_I8094_AXIS_RANGE_SETTINGS


#ifdef __cplusplus
extern "C"
{
#endif

//============================================================================
//====================== I8094 DLL (V3.0) Release Functions===================
//============================================================================
// OS Configuration Routines
short i8094_scan
    (
    short* pModuleNum,    /* pointer to available ICPDAS-I8094 cards */
    BYTE* pAvailModules   /* pointer to available module array */
    );

short i8094_get_cardinfo
    (
    int ScannedIndex,     /* PISO-VS600 scanned index, start with 0 */
    BYTE* pSlotNum,	  /* pointer to Slot-Number */
    WORD* pModuleID       /* pointer to ModuleID */
    );

short i8094_open
    (
    BYTE bSlotNum		/* the specific Slot-Number */
    );

short i8094_close
    (
    BYTE bSlotNum		/* the specific Slot-Number */
    );

short i8094_reset
    (
    BYTE bSlotNum		/* the specific Slot-Number */
    );

short i8094_get_range_settings
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    AXIS_RANGE_SETTINGS* pAxisRangeSetting  /* the point to I8094_AXIS_CONFIG structure  */
    );

short i8094_set_range
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    DWORD dwRange         	/* the valid dwRange value is 16000 ~ 8000000  */
    );

// Hardware Configuration Routines (Required)
short i8094_set_pls_cfg
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD wPulseMode,      	/* pulse mode: the settings will be PULSE_MODE_CW_CCW or PULSE_MODE_PULSE_DIRECTION  */
    WORD wPulseLogic,     	/* pulse polarity: the settings will be PULSE_LOGIC_ACTIVE_HIGH or PULSE_LOGIC_ACTIVE_LOW */
    WORD wDirectionLogic  	/* direction polarity: the settings will be PULSE_FORWARD_ACTIVE_HIGH or PULSE_FORWARD_ACTIVE_LOW. This setting is valid for PULSE_MODE_PULSE_DIRECTION mode */
    );

short i8094_set_enc_cfg
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD wEncoderMode,    	/* encoder mode: the settings will be ENCODER_MODE_AB, ENCODER_MODE_AB_DIVID_2, ENCODER_MODE_AB_DIVID_4 or ENCODER_MODE_CW_CCW  */
    BYTE bCounterSource   	/* this setting is reserved and ignored */
    );

short i8094_set_limit
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD wLimitLogic,    	/* EL(Limit Sensor) polarity: the settings will be LIMIT_LOGIC_ACTIVE_HIGH or LIMIT_LOGIC_ACTIVE_LOW */
    WORD wStopMode	  	/* stop mode: the setting will be LIMIT_STOP_SUDDEN or LIMIT_STOP_SLOWDOWN */
    );

// Hardware Configuration Routines (Optional)
short i8094_set_inp
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD wINPEnable,      	/* INP_ENABLE_FEATURE/INP_DISABLE_FEATURE to enable/disable in-position signal */
    WORD wINPLogic	  	/* in-position signal polarity: the setting will be INP_LOGIC_ACTIVE_HIGH or INP_LOGIC_ACTIVE_LOW */
    );

short i8094_set_alarm
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD wAlarmEnable,    	/* ALARM_ENABLE_FEATURE/ALARM_DISABLE_FEATURE to enable/disable in-position signal */
    WORD wAlarmLogic	  	/* in-position signal polarity: the setting will be ALARM_LOGIC_ACTIVE_HIGH or ALARM_LOGIC_ACTIVE_LOW */
    );

short i8094_servo_on
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    BYTE bServoON,	  	/* SERVO_ON/SERVO_OFF to switch on/off servo*/
    BYTE bAutoOFF	  	/* i8094_close()/i8094_close_all() Turn off Servo */
    );

short i8094_load_config
    (
    char* FileName        	/* pointer to pathname of configuration file */
    );

short i8094_set_filter
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD wFilterEnable,	  	/* FILTER_ENABLE_FEATURE/FILTER_DISABLE_FEATURE to enable/disable filter feature */
    WORD wFilterCfg,	  	/* combanation of FILTER_CFG_EMG_EL_ORG_NORG, ... FILTER_CFG_IN3  */
    WORD wDelayTime	  	/* FILTER_DELAY_2us, FILTER_DELAY_256us, ... FILTER_DELAY_8192us*/
    );

short i8094_set_softlimit
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD SWLimitEnable,	  	/* SW_LIMIT_ENABLE_FEATURE/SW_LIMIT_DISABLE_FEATURE to enable/disable filter feature */
    WORD wCmpSource,	  	/* CMP_SRC_LOGIC_COMMAND/CMP_SRC_ENCODER_POSITION */
    long LimitPositive,	  	/* -2147483647 ~ 2147483647 */
    long LimitNegaitive   	/* -2147483647 ~ 2147483647 */
    );

// Home Operation
short i8094_set_home_cfg
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD wHomeLogic,	  	/* HOME_LOGIC_ACTIVE_HIGH/HOME_LOGIC_ACTIVE_LOW  */
    WORD wNHomeLogic,	  	/* NHOME_LOGIC_ACTIVE_HIGH/NHOME_LOGIC_ACTIVE_LOW */
    WORD wIndexLogic,	  	/* INDEX_LOGIC_ACTIVE_HIGH/INDEX_LOGIC_ACTIVE_LOW */
    WORD wHomeSteps,	  	/* AUTO_HOME_STEP1_DISABLE, ... AUTO_HOME_STEP4_REVERSE */
    DWORD dwStep4Offset   	/* the offset driving in Step4 */
    );

short i8094_home_start
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,			/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    DWORD dwStartSpeed,		/* Start Speed */
    DWORD dwAcceleration,	/* Acceleration */
    DWORD dwDeceleration,	/* Deceleration */
    DWORD dwNHomeSearchSpeed,	/* Driving Speed when Near-Home searching */
    DWORD dwHomeSearchSpeed,	/* Driving Speed after Near-Home sensor is active */
    WORD  wBlockMode		/* ENABLE_BLOCK_OPEARTION, DISABLE_BLOCK_OPEARTION */
    );

short i8094_velocity_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,			/* can be combination of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwStartSpeed,		/* Start Speed */
    DWORD dwDriveSpeed,		/* Driving Speed */
    DWORD dwAcceleration,	/* Acceleration */
    BYTE  bDirection		/* MOVE_DIRECTION_FORWARD/MOVE_DIRECTION_REVERSE */
    );

short i8094_const_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,			/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwDriveSpeed,		/* Driving Speed */
    long  FixedPulse		/* total driving pulse */
    );

short i8094_t_set_avtri
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,			/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    WORD wAvTriCfg		/* Driving Speed */
    );

short i8094_t_change_v
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,			/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwDriveSpeed,		/* Driving Speed */
    DWORD dwAcceleration,	/* Acceleration */
    DWORD dwDeceleration	/* Deceleration */
    );

short i8094_t_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,			/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwStartSpeed,		/* Start Speed */
    DWORD dwDriveSpeed,		/* Driving Speed */
    DWORD dwAcceleration,	/* Acceleration */
    DWORD dwDeceleration,	/* Deceleration */
    long FixedPulse,		/* total driving pulse */
    short wAccCntOffset		/* Acceleration Counter Offset */
    );

short i8094_s_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,			/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwStartSpeed,		/* Start Speed */
    DWORD dwDriveSpeed,		/* Driving Speed */
    DWORD dwAccelerationRate,	/* AccelerationRate, Jerk (K) */
    DWORD dwDecelerationRate,	/* Deceleration (L) */
    long FixedPulse,		/* total driving pulse */
    short wAccCntOffset		/* Acceleration Counter Offset */
    );

short i8094_t_line2_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wMainAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    WORD wSlaveAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwStartSpeed,		/* Start Speed */
    DWORD dwDriveSpeed,		/* Driving Speed */
    DWORD dwAcceleration,	/* Acceleration */
    DWORD dwDeceleration,	/* Deceleration */
    long MainAxisFinishPoint,	/* Finish point of MainAxis */
    long SlaveAxisFinishPoint,	/* Finish point of SlaveAxis */
    short wAccCntOffset,	/* Acceleration Counter Offset, only valid for IntepStop = TRUE */
    WORD wBlockMode		/* ENABLE_BLOCK_OPEARTION/DISABLE_BLOCK_OPEARTION */
    );

short i8094_t_line3_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wMainAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    WORD wSecondAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    WORD wThirdAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwStartSpeed,		/* Start Speed */
    DWORD dwDriveSpeed,		/* Driving Speed */
    DWORD dwAcceleration,	/* Acceleration */
    DWORD dwDeceleration,	/* Deceleration */
    long MainAxisFinishPoint,	/* Finish point of MainAxis */
    long SecondAxisFinishPoint,	/* Finish point of 2nd Axis */
    long ThirdAxisFinishPoint,	/* Finish point of 3rd Axis */
    short wAccCntOffset,	/* Acceleration Counter Offset, only valid for IntepStop = TRUE */
    WORD wBlockMode		/* ENABLE_BLOCK_OPEARTION/DISABLE_BLOCK_OPEARTION */
    );

short i8094_s_line2_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wMainAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    WORD wSlaveAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwStartSpeed,		/* Start Speed */
    DWORD dwDriveSpeed,		/* Driving Speed */
    DWORD dwAccelerationRate,	/* Acceleration */
    DWORD dwDecelerationRate,	/* Deceleration */
    long MainAxisFinishPoint,	/* Finish point of MainAxis */
    long SlaveAxisFinishPoint,	/* Finish point of SlaveAxis */
    short wAccCntOffset,	/* Acceleration Counter Offset, only valid for IntepStop = TRUE */
    WORD wBlockMode		/* ENABLE_BLOCK_OPEARTION/DISABLE_BLOCK_OPEARTION */
    );

short i8094_s_line3_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wMainAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    WORD wSecondAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    WORD wThirdAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwStartSpeed,		/* Start Speed */
    DWORD dwDriveSpeed,		/* Driving Speed */
    DWORD dwAccelerationRate,	/* Acceleration */
    DWORD dwDecelerationRate,	/* Deceleration */
    long MainAxisFinishPoint,	/* Finish point of MainAxis */
    long SecondAxisFinishPoint,	/* Finish point of 2nd Axis */
    long ThirdAxisFinishPoint,	/* Finish point of 3rd Axis */
    short wAccCntOffset,	/* Acceleration Counter Offset, only valid for IntepStop = TRUE */
    WORD wBlockMode		/* ENABLE_BLOCK_OPEARTION/DISABLE_BLOCK_OPEARTION */
    );

short i8094_t_arc2_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wMainAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    WORD wSlaveAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwStartSpeed,		/* Start Speed */
    DWORD dwDriveSpeed,		/* Driving Speed */
    DWORD dwAcceleration,	/* Acceleration */
    WORD wArcDirection,		/* INTERP_ARC_DIRECTION_CLOCKWISE/INTERP_ARC_DIRECTION_COUNTER_CLOCKWISE */
    long MainAxisCenterPoint,	/* Center point of MainAxis */
    long SlaveAxisCenterPoint,	/* Center point of SlaveAxis */
    long MainAxisFinishPoint,	/* Finish point of MainAxis */
    long SlaveAxisFinishPoint,	/* Finish point of SlaveAxis */
    short wAccCntOffset,	/* Acceleration Counter Offset, only valid for IntepStop = TRUE */
    WORD wBlockMode		/* ENABLE_BLOCK_OPEARTION/DISABLE_BLOCK_OPEARTION */
    );

short i8094_conti_interp_begin
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wMainAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    WORD wSecondAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    WORD wThirdAxis,		/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwConstSpeed		/* Constant Speed */
    );

short i8094_conti_interp_next_ready
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    BYTE *pReady		/* the pointer to indicate the Next-Interpolation is ready to set */ 
    );


short i8094_conti_line2_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    long MainAxisFinishPoint,	/* Finish point of MainAxis */
    long SlaveAxisFinishPoint,	/* Finish point of SlaveAxis */
    WORD wContiInterpMoveMode	/* INTERP_CONTINUE_START/INTERP_NEXT_CONTINUOUS_MOTION */		
    );

short i8094_conti_line3_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    long MainAxisFinishPoint,	/* Finish point of MainAxis */
    long SecondAxisFinishPoint,	/* Finish point of 2nd Axis */
    long ThirdAxisFinishPoint,	/* Finish point of 3rd Axis */
    WORD wContiInterpMoveMode	/* INTERP_CONTINUE_START/INTERP_NEXT_CONTINUOUS_MOTION */		
    );

short i8094_conti_arc2_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wArcDirection,		/* INTERP_ARC_DIRECTION_CLOCKWISE/INTERP_ARC_DIRECTION_COUNTER_CLOCKWISE */
    long MainAxisCenterPoint,	/* Center point of MainAxis */
    long SlaveAxisCenterPoint,	/* Center point of SlaveAxis */
    long MainAxisFinishPoint,	/* Finish point of MainAxis */
    long SlaveAxisFinishPoint,	/* Finish point of SlaveAxis */
    WORD wContiInterpMoveMode	/* INTERP_CONTINUE_START/INTERP_NEXT_CONTINUOUS_MOTION */		
    );

short i8094_conti_interp_end
    (
    BYTE bSlotNum		/* the specific Slot-Number */
    ); 

short i8094_set_cmdcounter
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    long lData			/* the position count to be set into Logic Posistion Counter*/
    );

short i8094_set_enccounter
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    long lData			/* the position count to be set into Logic Posistion Counter*/
    );

short i8094_get_cmdcounter
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    long* pData			/* the pointer to the Logic Posistion Counter*/
    );

short i8094_get_enccounter
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    long* pData			/* the pointer to the Logic Posistion Counter*/
    );

short i8094_stop_move
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD wStopMode		/* can be STOP_SLOWDOWN or STOP_SUDDEN */
    );

short i8094_drv_start
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxes            	/* can be any commbination of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    );

short i8094_drv_hold
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxes            	/* can be any commbination of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    );

short i8094_set_vring
    (
    BYTE   bSlotNum,		/* the specific Slot-Number */
    WORD   wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD   wVRINGEnable,    	/* VARIABLE_RING_ENABLE_FEATURE/VARIABLE_RING_DISABLE_FEATURE*/
    DWORD  dwRingValue		/* the maximum value of Ring-Counter (start from 0 and 0xFFFFFFFF is the max. value) */ 
    );

short i8094_set_mpg
    (
    BYTE   bSlotNum,		/* the specific Slot-Number */
    WORD   wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD   wEXPConfig,      	/* EXP_DISABLE_FEATURE/EXP_AB_PHASE_MPG//EXP_CW_CCW_FIXED_PULSE/EXP_CW_CCW_CONTI_MOVE*/
    DWORD  dwFixedPulse,    	/* the output pulse for MPG trigger for EXP_AB_PHASE_MPG/EXP_CW_CCW_FIXED_PULSE modes*/
    DWORD  dwSpeed,		/* the speed for constant pulse output */ 
    DWORD  dwMaxMPGFreq		/* the maximum frequency of Manual-Pulse-Generator */ 
    );

short i8094_set_synch
    (
    BYTE   bSlotNum,		/* the specific Slot-Number */
    WORD   wMainAxis,		/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U */
    WORD   wSyncEnable,		/* SYNC_ENABLE_FEATURE/SYNC_DISABLE_FEATURE*/
    WORD   wSyncAxes,		/* can be combination of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U */
    WORD   wSyncCondition,	/* SYNC_CONDITION_EXCEED_CMP_POSITIVE ... SYNC_CONDITION_IN3_FALLING_EDGE */
    WORD   wSyncActionMainAxis,	/* the Sync. Action of own-Axis, SYNC_ACTION_FIXED_FORWARD_DRIVE ... SYNC_ACTION_WR6_TO_VELOCITY_REG */
    WORD   wSyncActionOtherAxes,/* the Sync. Action of other-Axes, SYNC_ACTION_FIXED_FORWARD_DRIVE ... SYNC_ACTION_WR6_TO_VELOCITY_REG */
    WORD   wCmpSource,		/* CMP_SRC_LOGIC_COMMAND/CMP_SRC_ENCODER_POSITION */
    DWORD  dwComparatorPositive,/* the value to be set to COMP+, only for SYNC_CONDITION_EXCEED_CMP_POSITIVE and SYNC_CONDITION_LESS_CMP_POSITIVE */
    DWORD  dwComparatorNegative	/* the value to be set to COMP-, only for SYNC_CONDITION_LESS_CMP_NEGATIVE and SYNC_CONDITION_EXCEED_CMP_NEGATIVE */
    );

short i8094_synch_t_move_cfg
    (
    BYTE  bSlotNum,		/* the specific Slot-Number */
    WORD  wAxis,		/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U */
    DWORD dwStartSpeed,		/* Start Speed */
    DWORD dwDriveSpeed,		/* Driving Speed */
    DWORD dwAcceleration,	/* Acceleration */
    DWORD dwDeceleration,	/* Deceleration */
    long  FixedPulse		/* total driving pulse */
    );

short i8094_synch_s_move_cfg
    (
    BYTE  bSlotNum,		/* the specific Slot-Number */
    WORD  wAxis,		/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U */
    DWORD dwStartSpeed,		/* Start Speed */
    DWORD dwDriveSpeed,		/* Driving Speed */
    DWORD dwAccelerationRate,	/* AccelerationRate, Jerk (K) */
    DWORD dwDecelerationRate,	/* Deceleration (L) */
    long FixedPulse		/* total driving pulse */    
    );

short i8094_set_int_factor
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,			/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U */
    WORD wIntFactor		/*INT_FACTOR_PULSE ... INT_FACTOR_SYNC_ACTION */
    );

short i8094_motion_done
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    BYTE* pDone,
    WORD* pStopStatus	  	/* the pointer to the Stop Status*/
    );

short i8094_get_speed
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U */
    DWORD* pSpeed		/* the pointer to the current speed */
    );

short i8094_get_acc
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U */
    DWORD* pAcc		  	/* the pointer to the current speed */
    );

short i8094_get_latch
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* can be one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U */
    long* pLatchData	  	/* the pointer to the current speed */
    );

// Digital IO Status
short i8094_get_in3
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* only one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    BYTE* pIN3Status	  	/* the pointer to the IN3 Status */
    );

short i8094_get_out1_status
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* only one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    BYTE* pOut1Status	  	/* the pointer to the OU1 (Servo-On) Status */
    );

short i8094_get_mdi_status
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* only one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD* pDIStatus	 	/* the pointer to the DI Status (active status is based on polarity configuration) */
    );

short i8094_get_int_status
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* only one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD* pIntStatus	  	/* the pointer to the Interrupt Status */
    );

short i8094_get_error_status
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,           	/* only one of AXIS_X, AXIS_Y, AXIS_Z or AXIS_U*/
    WORD* pErrorStatus	  	/* the pointer to the Error Status */
    );

short i8094_change_p
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wAxis,			/* only one of AXIS_X, AXIS_Y, AXIS_Z and AXIS_U*/
    DWORD dwP			/* total output pulse */
    );

short i8094_get_FRnet_DI
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wSA,			/* Group Address FRNET_SA8 ~ FRNET_SA15 */
    WORD *pStatus,		/* the pointer to the FRnet DI status */
    WORD wEnableDirectAccess 	/* FRNET_ENABLE_DIRECT_ACCESS/FRNET_DISABLE_DIRECT_ACCESS, access the DI module directly, or read the stored status in driver */
    );

short i8094_set_FRnet_DO
    (
    BYTE bSlotNum,		/* the specific Slot-Number */
    WORD wRA,			/* Group Address FRNET_RA0 ~ FRNET_RA7 */
    WORD wDOData		/* the data to DO module */
    );

#ifdef __cplusplus
}
#endif

#endif





