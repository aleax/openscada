/* <St> *******************************************************************

  FILENAME    : DPM_USER.H

  -------------------------------------------------------------------------
  CREATED BY  : Armin Beck,  Hilscher GmbH
  CREATED AT  : 10.05.96
  PROJECT     :
  =========================================================================

  FUNCTION :
    User-Interface to PROFIBUS DP-master

  =========================================================================

  CHANGES OF REVISIONS :

  Version Name      Date      Change
  -------------------------------------------------------------------------
  V1.402  AB        17.12.00  new define DPM_MAX_NUM_DEVICES
                              new command DDLM_Life_List

  V1.401  AB        19.10.00  New Message command MPI_Read_Write_DB and
                              disconnect
  V1.400  AB        22.11.99  Extention of struct DPM_PLC_PARAMETER.(Identnumber)
  V1.304  AB        27.06.99  Definition of FDL_Send_Data_Reply Service
  V1.303  AB        16.06.99  Extention of FDL-error table. CON_AD, CON_NP
                              included
  V1.302  AB        30.04.99  change the order of bAlarm_Mode and bAdd_Sl_Flag
                              in the structure DPM_SL_PRM_HEADER in accordance
                              to the DPV1-specification
  V1.301  AB        23.02.99  Extention of the DPM_SL_PRM_PRM_DATA structure
                              for DPV1 usage
                              Extention of DPM_BUS_DP structure
                              new parameter bRedundancy and bSlStateMethod
                              in structure DPM_PLC_PARAMETER
                              new DPV1 message command definitions
  V1.300  AB        22.01.99  Changing nomenclatura of service definitions
                              from DPM_... to DDLM_...
                              Insert new service DDLM_Upload
                              inser new command DDLM_Get_Cfg
  V1.202  AB        02.04.98  correction of the definition
                              of CLEAR_DATA,SYNC...
                              bTout inserted in global statusfield
  V1.201  AB        16.03.98  WatchDogTime extention in warmstart parameter
  V1.200  AB        09.11.97  extended errors; extended diagnostic structure
  V1.102  AB        09.09.97  extended errors; extended diagnostic structure
  V1.101  AB        04.12.96  new #defines included
  V1.100  AB        29.11.96  revision of all structures
  V1.000  Mayer     29.05.96  changed

  V1.000  AB        10.05.96  created

  ******************************************************************** <En> */

#if defined( _MSC_VER)          /* Microsoft C */
    #pragma pack(1)             /* Byte Alignment   */
#endif

/* ------------------------------------------------------------------------------------ */
/*   process image transfer mode definitions                                            */
/* ------------------------------------------------------------------------------------ */

#define DPM_GET_MODE_MASK                       0xf0

#define DPM_GET_MODE_NOT_SUPPORTED              0x00
#define DPM_GET_MODE_BUSSYNC_DEVICE_CONTROLLED  0x10
#define DPM_GET_MODE_BUFFERED_DEVICE_CONTROLLED 0x20
#define DPM_GET_MODE_UNCONTROLLED               0x30
#define DPM_GET_MODE_BUFFERED_HOST_CONTROLLED   0x40
#define DPM_GET_MODE_BUSSYNC_HOST_CONTROLLED    0x50


/* ======================================================================== */
/* Protocol parameter structure                                             */
/* ======================================================================== */

/* --------------------------- */
/* device warm start parameter */
/* --------------------------- */

typedef struct DPM_PLC_PARAMETERtag {
  unsigned char  bMode;         /* handshake of process data */
  #define DPM_SET_MODE_BUSSYNC_DEVICE_CONTROLLED  0
  #define DPM_SET_MODE_BUFFERED_DEVICE_CONTROLLED 1
  #define DPM_SET_MODE_UNCONTROLLED               2
  #define DPM_SET_MODE_BUFFERED_HOST_CONTROLLED   3
  #define DPM_SET_MODE_BUSSYNC_HOST_CONTROLLED    4
  unsigned char  bCycleTime;    /* replace the min_slave_intervall */
  unsigned char  bFormat;       /* storage format of word data */
  #define        DPM_FORMAT_INTEL   0
  #define        DPM_FORMAT_MOTROLA 1
  unsigned short usWatchDogTime; /* watchdog time in msec. for HOST-supervision */
  unsigned char  bRedundancy;    /* enables or disables the redundancy logic */
  unsigned char  bSlStateMethod; /* defines the update method od Sl_State field */
  #define        DPM_SL_STATE_NOT_SYCHRONOUS 0
  #define        DPM_SL_STATE_SYCHRONOUS     1
  struct
  {
    unsigned char  biIdentNumberActive  : 1;
    unsigned char  biHighPriorHandshake : 1;
    unsigned char  biReserved           : 6;
  } bEnableBits;
  unsigned short usIdentNumber;
  unsigned char  abReserved[6];
} DPM_PLC_PARAMETER;


/* -------------------------------- */
/* structures to set slave data set */
/* -------------------------------- */
#define DPM_MAX_NUM_DEVICES 126

/* parameter data header structure of a slave station,
   described in norm EN 50170 page 101 */
typedef struct DPM_SL_PRM_HEADERtag {
  unsigned short  usSlaveParaLen;    /* length of whole parameter data set */
  struct
  {
    unsigned char  biReserved1       : 1;
    unsigned char  biExtra_Alarm_SAP : 1;
    unsigned char  biDPV1_Data_Types : 1;
    unsigned char  biDPV1_Supported  : 1;
    unsigned char  biReserved2       : 1;
    unsigned char  biFail_Safe       : 1;
    unsigned char  biNew_Prm         : 1;
    unsigned char  biActive          : 1;
  } Sl_Flag;                          /* slave related flags */
  unsigned char    bSlave_Typ;         /* type of slave */

  unsigned char    bMax_Diag_Data_Len;
  unsigned char    bMax_Alarm_Len;
  unsigned char    bMax_Channel_Data_Length;
  unsigned char    bDiag_Upd_Delay;
  unsigned char    bAlarm_Mode;

  struct
  {
    unsigned char  biNA_to_Abort : 1;
    unsigned char  biIgnore_Aclr : 1;
    unsigned char  abiReserved   : 6;
  } bAdd_Sl_Flag;

  unsigned char    abOctet_String[6];

} DPM_SL_PRM_HEADER;


/* parameter data 'Prm_Data' structure of a slave station,
   described in norm EN 50170 page 51 */
#define  MAX_USR_PRM_LEN  244

typedef struct DPM_SL_PRM_PRM_DATAtag {
  unsigned short  usPrmDataLen;       /* length of PRM_PRM_DATA */
  struct
    {
      unsigned char   bReserved     : 3;
      unsigned char   bWD_On        : 1;
      unsigned char   bFreeze_Req   : 1;
      unsigned char   bSync_Req     : 1;
      unsigned char   bUnLock_Req   : 1;
      unsigned char   bLock_Req     : 1;
    } Station_Status;             /* status of supported functions */
  unsigned char   bWD_Fact_1;     /* watchdog factor 1 */
  unsigned char   bWD_Fact_2;     /* watchdog factor 2 */
  unsigned char   bMin_Tsdr;      /* min. station delay reponder */
  unsigned short  usIdent_Number; /* ident number of the station, motorola format */
  unsigned char   bGroup_Ident;   /* configures group number */

  struct
  {
    unsigned char   bReserved1    : 2;
    unsigned char   bWD_Base_1ms  : 1;
    unsigned char   bReserved2    : 3;
    unsigned char   bFail_Safe    : 1;
    unsigned char   bDPV1_Enable  : 1;
  } DPV1_Status_1;

  struct
  {
    unsigned char   bRun_On_Cfg_Fault         : 1;
    unsigned char   bReserved1                : 1;
    unsigned char   bEnable_Update_Alarm      : 1;
    unsigned char   bEnable_Status_Alarm      : 1;
    unsigned char   bEnable_Manufacture_Alarm : 1;
    unsigned char   bEnable_Diagnostic_Alarm  : 1;
    unsigned char   bEnable_Process_Alarm     : 1;
    unsigned char   bEnable_Pull_plug_Alarm   : 1;
  } DPV1_Status_2;

  struct
  {
    unsigned char   bAlarm_Mode              : 3;
    unsigned char   bReserved1               : 5;
  } DPV1_Status_3;

  unsigned char   abUsr_Prm_Data[MAX_USR_PRM_LEN];
} DPM_SL_PRM_PRM_DATA;


/* configuration data 'Cfg_Data' structure of a slave station,
   described in norm EN 50170 page 55 */
#define MAX_CFG_DATA_LEN 244

typedef struct DPM_SL_PRM_CFG_DATAtag {
  unsigned short  usCfg_Data_Len;  /* length of CFG_DATA */
  unsigned char   abCfg_Data[MAX_CFG_DATA_LEN]; /* configuration data of the
                                                   slave station */
} DPM_SL_PRM_CFG_DATA;


/* i/o offset table 'Add_Tab' structure of a slave station,
   described in norm EN 50170 page 103 */
#define MAX_EA_OFFSET_LEN 244

typedef struct DPM_SL_PRM_ADD_TABtag {
  unsigned short  usAdd_Tab_Len; /* length of ADD_TAB */
  unsigned char   bInput_Count;  /* counter of following input offsets */
  unsigned char   bOutput_Count; /* counter of following ouput offsets */
  unsigned short  ausEA_Offset[MAX_EA_OFFSET_LEN]; /* user defined offsets */
} DPM_SL_PRM_ADD_TAB;


/* extented data 'Slave_User_Data' structure of a slave station,
   described in norm EN 50170 page 103 */
#define  MAX_SL_PRM_LEN  100

typedef struct DPM_SL_PRM_USR_DATAtag {
  unsigned short usSlave_Usr_Data_Len; /* length of USR_DATA */
  unsigned char  abSlave_Usr_Data[MAX_SL_PRM_LEN]; /* user paramteres */
} DPM_SL_PRM_USR_DATA;

/* --------------------------------- */
/* structure to set master parameter */
/* --------------------------------- */

/* master parameter 'DPM_Bus_Dp' data set
   described in norm DIN 19245-T3 page 98 */
typedef struct DPM_BUS_DPtag {
   unsigned short       usBus_Para_Len;   /* length of BUS_DP */
   unsigned char        bFDL_Add;         /* master address */
   unsigned char        bBaudrate;        /* baudrate, see. #defines */
   #define DP_BAUD_96      0
   #define DP_BAUD_19_2    1
   #define DP_BAUD_93_75   2
   #define DP_BAUD_187_5   3
   #define DP_BAUD_500     4
   #define DP_BAUD_1500    6
   #define DP_BAUD_3000    7
   #define DP_BAUD_6000    8
   #define DP_BAUD_12000   9
   #define DP_BAUD_31_25  10
   #define DP_BAUD_45_45  11

   unsigned short       usTSL;            /* slot-time */
   unsigned short       usMin_TSDR;       /* min. station delay responder */
   unsigned short       usMax_TSDR;       /* max. station delay responder */
   unsigned char        bTQUI;            /* quite time */
   unsigned char        bTSET;            /* setup time */
   unsigned long        ulTTR;            /* target rotation time */
   unsigned char        bG;               /* gap update */
   unsigned char        bHSA;             /* highest active station address */
   unsigned char        bMax_Retry_Limit; /* retries if error occurs */
   struct
   {
     unsigned char      bReserved          : 7;
     unsigned char      bError_Action_Flag : 1;
   } Bp_Flag;                             /* auto_clear on/off */
   unsigned short       usMin_Slave_Intervall; /* min. slave intervall time */
   unsigned short       usPoll_Timeout;        /* poll timeout */
   unsigned short       usData_Control_Time;   /* data control time */
   unsigned char        bAlarm_Max; /* maximum alarms the master supports = 32 */
   unsigned char        bMax_User_Global_Control; /* maximum parallel Global-
                                                     Controls the master supports = 1*/
   unsigned char        abOctet[4];
} DPM_BUS_DP;

/* ======================================================================== */
/* Protocol slave specific single diagnostic structure                      */
/* ======================================================================== */
#define MAX_EXT_DIAG_LEN  100
/* slave specific diagnostic structure
   described in norm DIN 19245-T3 page 41 */
typedef struct DPM_SLAVE_SINGLE_DIAGNOTICStag {
  struct
  {
    unsigned char    bStation_Non_Existent : 1; /* no response */
    unsigned char    bStation_Not_Ready    : 1; /* station not ready */
    unsigned char    bCfg_Fault            : 1; /* configuration fault */
    unsigned char    bExt_Diag             : 1; /* extended diagnostic */

    unsigned char    bNot_Supported        : 1; /* sync, freeze not supported */
    unsigned char    bInvalid_Response     : 1; /* response faulty */
    unsigned char    bPrm_Fault            : 1; /* parameters faulty */
    unsigned char    bMaster_Lock          : 1; /* locked by a master */
  } Stationstatus_1;

  struct
  {
    unsigned char    bPrm_Req              : 1; /* request new parameters */
    unsigned char    bStat_Diag            : 1; /* static diagnostic */
    unsigned char    bTrue                 : 1; /* set to 1 by a slave */
    unsigned char    bWd_On                : 1; /* watchdog function on/off */

    unsigned char    bFreeze_Mode          : 1; /* freeze mode active */
    unsigned char    bSync_Mode            : 1; /* sync mode active */
    unsigned char    bReserved             : 1; /* reserved */
    unsigned char    bDeactivated          : 1; /* slave deactivated */
  } Stationstatus_2;

  struct
  {
    unsigned char    bReserved             : 7;
    unsigned char    bExt_Diag_Overflow    : 1; /* ext. diagnostic overflow */
  } Stationstatus_3;

  unsigned char      bMaster_Add;               /* corresponding master address */
  unsigned short     usIdent_Number;            /* ident number, motorola format */

  unsigned char      abExt_Diag_Data[MAX_EXT_DIAG_LEN];
                                                /* extended diagnostic field */

} DPM_SLAVE_SINGLE_DIAGNOSTICS;


/* ======================================================================== */
/* Protocol master state structure                                          */
/* ======================================================================== */

typedef struct DPM_DIAGNOSTICStag {
  /* unsigned char field to show bus and master main errors */
  struct
  {
    unsigned char bCtrl     : 1; /* wrong parameterization                  */
    unsigned char bAClr     : 1; /* auto_clear activated                    */
    unsigned char bNonExch  : 1; /* no data exchange to at least on station */
    unsigned char bFatal    : 1; /* fatal error occured                     */
    unsigned char bEvent    : 1; /* bus error events occured                */
    unsigned char bNRdy     : 1; /* host program not ready                  */
    unsigned char bTout     : 1; /* PROFIBUS timeout event detected         */
    unsigned char bReserved : 1;
  } bGlobalBits;

  /* global state for the different master main states */
  unsigned char   bDPM_state;
  #define OFFLINE  0x00
  #define STOP     0x40
  #define CLEAR    0x80
  #define OPERATE  0xC0

  /* location of error and error code */
  struct
  {
    unsigned char bErr_Rem_Adr; /* 0-125, 255 */
    #define MST_ERR  0xFF

    unsigned char bErr_Event;   /* see #defines */
    #define TASK_F_NO_USR_TASK        50  /* usr task not found */
    #define TASK_F_NO_GLOBAL_DATA     51  /* no global entry in data base */
    #define TASK_F_NO_FDL             52  /* fdl task not found */
    #define TASK_F_NO_PLC             53  /* plc task not found */
    #define TASK_F_NO_BUS_DP          54  /* no bus parameters */
    #define TASK_F_INVALID_BUS_DP     55  /* bus parameters faulty */
    #define TASK_F_NO_SL_TAB          56  /* no slave data sets */
    #define TASK_F_INVALID_SL_TAB     57  /* slave data set faulty */
    #define TASK_F_REM_ADR_DOUBLE     58  /* double station adress detected */
    #define TASK_F_A_OFFSET_MAX       59  /* maximum output offset reached */
    #define TASK_F_E_OFFSET_MAX       60  /* maximum input offset reached */
    #define TASK_F_A_OVERLAP          61  /* overlap in output area */
    #define TASK_F_E_OVERLAP          62  /* overlap in input area */
    #define TASK_F_DPM_UNKNOWN_MODE   63  /* warm start with unknown mode */
    #define TASK_F_RAM_OVERRUN        64  /* extended ram exeeded */
    #define TASK_F_SL_PRM_FAULT       65  /* slave data set faulty */
    #define TASK_F_WATCHDOG          220  /* user watchdog failure */
    #define TASK_F_NO_DATA_ACK       221  /* mode0 no acknowledge */
    #define TASK_F_AUTO_CLEAR        222  /* auto clear activated */
    #define TASK_F_FATAL_ASIC_ERROR  223  /* fatal error occured */
    #define TASK_F_UNKNOWN_EVENT     224  /* unknown event detected */
    #define TASK_F_SEGMENT_CNT       225  /* no segments to communicate */
    #define CON_NA     0x11   /* no reaction of the remote station         */
    #define CON_DS     0x12   /* cocal-FDL/PHY not in logical token ring   */
    #define CON_RS     0x03   /* master request not supported by the slave */
    #define CON_NR     0x09   /* no response data                          */
    #define CON_DS     0x12   /* local FDL not in logical token ring       */

  } tError;

  /* counter for the bus error events */
  unsigned short  usBus_Error_Cnt;

  /* Zhler fr die BusTimeouts */
  unsigned short  usTime_Out_Cnt;

  /* reserved area */
  unsigned char   abReserved[8];

  /* Bit-Ready, Cfg-Ready and diagnostic display of slave devices */
  unsigned char   abSl_cfg  [16];  /* Slave configuration area */
  unsigned char   abSl_state[16];  /* Slave state information area */
  unsigned char   abSl_diag [16];  /* Slave diagnostic area */
} DPM_DIAGNOSTICS;


/* ======================================================================== */
/* Master Class2 definitions                                                */
/* ======================================================================== */


#define UNDEFINED 1

typedef struct
{
  unsigned char abNetwork_Address[6];    /* network address according ISO/OSI */
  unsigned char abMAC_Address[UNDEFINED];/* MAC_Address */
} ADDR_EXTENTION;

typedef struct
{
  unsigned char bAPI; /* application process instance */
  unsigned char bSCL; /* access level */
} ADDR;

#define MAX_ADD_TABLE_LEN 235

typedef struct
{
  #define  TYPE_EXT_OFF 0 /* no optional Network/MAC address */
  #define  TYPE_EXT_ON  1 /* optional Network/MAC address available */

  unsigned char bS_Type;
  unsigned char bS_Len;  /* length of S_Addr subparameter */
  unsigned char bD_Type;
  unsigned char bD_Len;  /* length of D_Addr subparameter */

  unsigned char abAddParam[ MAX_ADD_TABLE_LEN ];

} ADD_ADDR_PARAM;

typedef struct
{
  unsigned short usSend_Timeout; /* control time for supervision */

  struct
  {
    unsigned char  biDPV1_RW   : 1; /* support of MSAC2_READ and WRITE */
    unsigned char  abiReserved : 7;
  } bFeatures_Supported_1;

  struct
  {
    unsigned char  abiReserved : 8;
  } bFeatures_Supported_2;

  unsigned char  bProfile_Features_Supported_1;
  unsigned char  bProfile_Features_Supported_2;
  unsigned short usProfile_Ident_Number; /* 0 to 65535 */
  #define NO_PROFILE 0

  ADD_ADDR_PARAM tAdd_Addr_Param;
} DPM_MSAC2M_INITIATE_REQ;


typedef struct
{
  struct
  {
    unsigned char  biDPV1_RW   : 1; /* support of MSAC2_READ and WRITE */
    unsigned char  abiReserved : 7;
  } bFeatures_Supported_1;

  struct
  {
    unsigned char  abiReserved : 8;
  } bFeatures_Supported_2;

  unsigned char  bProfile_Features_Supported_1;
  unsigned char  bProfile_Features_Supported_2;
  unsigned short usProfile_Ident_Number; /* 0 to 65535 */
  #define NO_PROFILE 0

  ADD_ADDR_PARAM tAdd_Addr_Param;
} DPM_MSAC2M_INITIATE_CON;


/* ======================================================================== */
/* Protocol definition                                                      */
/* ======================================================================== */

/* DPV1 Class1 primitives */
#define MSAC1M_Read_Write      0x11

#define MSAL1M_Alarm           0x12

#define MSAC1M_Abort           0x1f

/* DPV1 Class2 primitives */
#define MSAC2M_Initiate        0x20

#define MSAC2M_Read_Write_Data 0x21

#define MSAC2M_Fault           0x2d

#define MSAC2M_Close           0x2e

#define MSAC2M_Abort           0x2f

/* MPI primitives */
#define MPI_Read_Write_DB      0x31

#define MPI_Get_OP_Status      0x32

#define MPI_Disconnect         0x3f


#define DPM_MAX_LEN_DATA_UNIT    240

/* ------------------------------- */
/* message: shared_memory          */
/* ------------------------------- */

/* message command msg.b msg.a */
#define DPM_Shared_Memory   0x11


/* ------------------------------- */
/* message: slave_diag             */
/* ------------------------------- */

/* message command msg.b msg.a */
#define DDLM_Slave_Diag      0x42

typedef struct DDLM_SLAVE_DIAG_CONFIRMtag {
  DPM_SLAVE_SINGLE_DIAGNOSTICS tDiagData;
} DDLM_SLAVE_DIAG_CONFIRM;

/* ------------------------------- */
/* message: set_prm                */
/* ------------------------------- */

/* message command msg.b msg.a */
#define DDLM_Set_Prm 0x4a

typedef struct DDLM_SET_PRM_REQUESTtag {
  unsigned char        bRem_Add;
  unsigned char        abUsr_Prm_Data[DPM_MAX_LEN_DATA_UNIT];
} DDLM_SET_PRM_REQUEST;

typedef struct DDLM_SET_PRM_CONFIRMtag {
  unsigned char        bRem_Add;
} DDLM_SET_PRM_CONFIRM;

/* ------------------------------- */
/* message: get_cfg                */
/* ------------------------------- */

/* message command msg.b msg.a */
#define DDLM_Get_Cfg 0xe8

/* ------------------------------- */
/* message: start_seq              */
/* ------------------------------- */

/* message command msg.b msg.a */
#define DDLM_Start_Seq       0x43

typedef struct DDLM_START_SEQ_REQUESTtag {
   unsigned char        bReq_Add;   /* 0 */
   unsigned char        bArea_Code; /* 0 - 125 */
   unsigned short       usTimeout;  /* 0 - 65535 */
} DDLM_START_SEQ_REQUEST;

typedef struct DDLM_START_SEQ_CONFIRMtag {
   unsigned char        bMax_Len_Data_Unit;   /* 240 */
} DDLM_START_SEQ_CONFIRM;


/* ------------------------------- */
/* message: download               */
/* ------------------------------- */

/* message command msg.b msg.a */
#define DDLM_Download        0x44


#define DPM_DEVICE_PRM            127
/* message data structure */
typedef struct DDLM_DOWNLOAD_REQUESTtag {
   unsigned char        bReq_Add;    /* 0 */
   unsigned char        bArea_Code;  /* 0 - 125, 127 */
   unsigned short       usAdd_Offset; /* 0 - 760 */
   unsigned char        abData[DPM_MAX_LEN_DATA_UNIT];
} DDLM_DOWNLOAD_REQUEST;

/* ------------------------------- */
/* message: upload                 */
/* ------------------------------- */

/* message command msg.b msg.a */
#define DDLM_Upload        0x50

/* message data structure */
typedef struct DDLM_UPLOAD_REQUESTtag {
   unsigned char        bReq_Add;     /* 0 */
   unsigned char        bArea_Code;   /* 0 - 125, 127 */
   unsigned short       usAdd_Offset; /* 0 - 760 */
   unsigned char        bData_Len;    /* 1-240 */
} DDLM_UPLOAD_REQUEST;


/* ------------------------------- */
/* message: end_sequence           */
/* ------------------------------- */

/* message command msg.b msg.a */
#define DDLM_End_Seq         0x45

/* message data structure */
typedef struct DDLM_END_SEQ_REQUESTtag {
   unsigned char        bReq_Add;   /* 0 */
} DDLM_END_SEQ_REQUEST;


/* ------------------------------- */
/* message: global_control         */
/* ------------------------------- */

/* message command msg.b msg.a */
#define DDLM_Global_Control  0x46

/* message data structure */
typedef struct DDLM_GLOBAL_CONTROL_REQUESTtag {
   unsigned char        bRem_Add;   /* 0 - 127 */
   unsigned char        bControl_Command;
   #define CLEAR_DATA   0x02
   #define UNFREEZE     0x04
   #define FREEZE       0x08
   #define UNSYNC       0x10
   #define SYNC         0x20  /* unsigned chars can be combined with an 'or' command */

   unsigned char        bGroup_Select;
} DDLM_GLOBAL_CONTROL_REQUEST;

typedef struct DDLM_GLOBAL_CONTROL_CONFIRMtag {
   unsigned char        bRem_Add;   /* 0 - 127 */
} DDLM_GLOBAL_CONTROL_CONFIRM;

/* ------------------------------- */
/* message: life_list              */
/* ------------------------------- */

#define DDLM_Life_List 0x96


/* --------------------------------- */
/* message: FDL_Send_Data_With_Reply */
/* --------------------------------- */

#define FDL_Send_Data_Reply_Req 0x82
#define FDL_Send_Data_Reply_Con 0x82

#define DATA_STU_SIZE  244

typedef struct octet_strtag {
  unsigned char  bLen; /* length of data field 0 to 244 */
  unsigned char  bValue[ DATA_STU_SIZE ]; /* data field */
} octet_str;

typedef struct FDL_DATA_REPLY_REQ_STUtag {
  unsigned char  bSsap;      /* Source Service Access Point 0 to 126 */
  unsigned char  bDsap;      /* Destination Service Access Point 0 to 127 */
  unsigned char  bRem_add_da;/* Remote Address in PROFIBUS 0 to 127 */
  unsigned char  bServ_class;/* Priority of service = 0 Low */
  octet_str      tL_sdu;     /* data field structure */
} FDL_DATA_REPLY_REQ_STU;

typedef struct FDL_DATA_REPLY_CON_STUtag {
  unsigned char  bSsap;      /* Source Service Access Point 0 to 126 */
  unsigned char  bDsap;      /* Destination Service Access Point 0 to 127 */
  unsigned char  bRem_add_da;/* Remote Address in PROFIBUS 0 to 127 */
  unsigned char  bServ_class;/* Priority of service = 0 Low */
  unsigned char  bL_status;  /* Error code */
  octet_str      tL_sdu;     /* data field structure */
} FDL_DATA_REPLY_CON_STU;


/* --------------------------------- */
/* message: FDL_Activate_SSap         */
/* --------------------------------- */

#define FDL_SSap_Activate_req    0x97
#define FDL_SSap_Activate_con    0x97

typedef struct FMA_SAP_ACTIVATE_REQ_STUtag {
  unsigned char bSsap;                /* Source Service Access Point 0 to 126 */
  unsigned char bAccess;              /* Accessright = 255    */
  unsigned char bService_list_length;  /* Length of followed list = 4  */

  #define SERVICE_SDA  0
  #define SERVICE_SDN  1
  #define SERVICE_SRD  2

  unsigned char abService_activate[4]; /* Service that shall be activated*/

  #define ROLE_IN_BOTH        1  /* Role_IN-Service Both */
  #define ROLE_IN_RESPONDER   2
  #define ROLE_IN_INITIATOR   3

  unsigned char abRole_in_service [4]; /* Configuration for the service  */

  struct {
    unsigned char bReq_low;  /* Maximum length of buffer = 240 */
    unsigned char bReq_high; /* Maximum length of buffer = 240 */
    unsigned char bInd_low;  /* Maximum length of buffer = 240 */
    unsigned char bInd_high; /* Maximum length of buffer = 240 */
  } tNon_cyclic_L_sdu_length_list[3];

} FMA_SAP_ACTIVATE_REQ_STU;

typedef struct FMA_SAP_ACTIVATE_CON_STUtag {
  unsigned char  bSsap;      /* Src. Service Access Pnt              */
  unsigned char  bM_status;
} FMA_SAP_ACTIVATE_CON_STU;

/* --------------------------------- */
/* message: FDL_Activate_RSap         */
/* --------------------------------- */

#define FDL_RSap_Activate_req       0x98
#define FDL_RSap_Activate_con       0x98

typedef struct FMA_RSAP_ACTIVATE_REQ_STUtag {
  unsigned char ssap;           /* Source Service Access Point 0 to 126 */
  unsigned char access;         /* Accessright = 255    */
  unsigned char indication_mode;

  unsigned char req_low;        /* Maximum length of buffer = 240 */
  unsigned char req_high;       /* Maximum length of buffer = 240 */
  unsigned char ind_low;        /* Maximum length of buffer = 240 */
  unsigned char ind_high;       /* Maximum length of buffer = 240 */

} FMA_RSAP_ACTIVATE_REQ_STU;

typedef struct FMA_RSAP_ACTIVATE_CON_STUtag {
  unsigned char  bSsap;      /* Src. Service Access Pnt              */
  unsigned char  bM_status;
} FMA_RSAP_ACTIVATE_CON_STU;


/* ======================================================================== */
/* message error numbers                                                    */
/* ======================================================================== */

#define  CON_OK         0x00    /* no error                                 */
#define  CON_RS         0x03    /* Profibus-DP service not accessible       */
#define  CON_NO         0x13    /* no access in current master mode         */
#define  CON_IV         0x15    /* parameter fault in request               */
#define  CON_NP         0x19    /* no plausible reaction from remote station*/
#define  CON_TO         0x30    /* function timeout                         */
#define  CON_NI         0x34    /* function not available on responder      */
#define  CON_EA         0x35    /* area overflow                            */
#define  CON_AD         0x36    /* access denied                            */
#define  CON_IP         0x37    /* parameter error                          */
#define  CON_SE         0x39    /* sequence fault                           */
#define  CON_NE         0x3A    /* requested data not available             */
#define  CON_DI         0x3B    /* data fault or not complete               */



#if defined( _MSC_VER)          /* Microsoft C */
    #pragma pack()              /* Byte Alignment   */
#endif

/* === eof 'USER.H' === */
