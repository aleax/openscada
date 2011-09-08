#ifdef __cplusplus
extern "C" {
#endif

#define I87123_OK                0
#define I87123_COMMAND_ERR       1
#define I87123_CONFIG_ERR        2
#define I87123_DATALEN_ERR       3
#define I87123_ADDNODE_ERR       4
#define I87123_REMOVENODE_ERR    5
#define I87123_STATUS_ERR        6
#define I87123_SETGUARD_ERR      7
#define I87123_GUARD_FAILED      8
#define I87123_NODENUMBER_ERR    9
#define I87123_COBID_ERR         10

#define I87123_SDO_WRITEDATA_ERR 11
#define I87123_SDO_SEND_ERR      12
#define I87123_SDO_SEGMENT_ERR   13

#define I87123_PDO_SEND_ERR      15
#define I87123_PDO_TYPE_ERR      16
#define I87123_RESPONSE_ERR      17
#define I87123_PDO_CHANNEL_ERR   18
#define I87123_PDO_INSTALL_ERR   19

#define I87123_TIMEOUT           21

#define I87123_PARATERS_ERR      25
#define I87123_CHANNEL_ERR       27

#define I87123_SYNC_SEND_ERR     30

#define I87123_SDO_DATA_LOSE     40

#define I87123_EMCY              45
#define I87123_RECEIVE_ERR       46

#define I87123_EMPTY             49


float I87123_GetVersion(void);
int I87123_Configure(unsigned char baudrate, int slot) ; // 0 ~ 7 ==> 10k ~ 1000K
int I87123_ShutdownMaster(void);
int I87123_AddNode(unsigned char node);
int I87123_RemoveNode(unsigned char node);
int I87123_ChangeState(unsigned char node, unsigned char state);
int I87123_GetState(unsigned char node, unsigned char *state);
int I87123_Guarding(unsigned char node, unsigned short guardtime, unsigned char lifttime);
int I87123_SendSYNC(unsigned short cobid, unsigned char cyclically, unsigned short timer);
int I87123_ChaneSYNCID(unsigned char node, unsigned short cobid);
int I87123_ChaneEMCYID(unsigned char node, unsigned short cobid);
int I87123_AobrtSDO(unsigned char node, unsigned short index, unsigned char subindex);
int I87123_ReadSDO(unsigned char node, unsigned short index, unsigned char subindex, 
                    unsigned char *len, unsigned char *rdata);
int I87123_WriteSDO(unsigned char node, unsigned short index, unsigned char subindex, 
                     unsigned char len, unsigned char *tdata, unsigned char *rlen, unsigned char *rdata);
int I87123_SetPDOResponse(unsigned char node, unsigned short cobid, unsigned char mode);
int I87123_InstallPDO(unsigned char node, unsigned short cobid, unsigned char txrxtype, 
                       unsigned char channel, unsigned char *tdata);
int I87123_RemovePDO(unsigned char node, unsigned short cobid, unsigned char txrxtype, 
                       unsigned char channel);
int I87123_WritePDO(unsigned short cobid, unsigned char offset, unsigned char dlen, unsigned char *rdata);
int I87123_RemotePDO(unsigned short cobid, unsigned char *len, unsigned char *rdata);
int I87123_PDOTxType(unsigned char node, unsigned short cobid, unsigned char txtype);
int I87123_MsgResponse(unsigned char *node, unsigned short *cobid, unsigned char *len, unsigned char *rdata);

int I87123_WriteDO(unsigned char node, unsigned char dochannel, unsigned char value);
int I87123_ReadDI(unsigned char node, unsigned char dichannel, unsigned char *value);
int I87123_WriteAO(unsigned char node, unsigned char aochannel, unsigned short value);
int I87123_ReadAI(unsigned char node, unsigned char aochannel, unsigned short *value);
#ifdef __cplusplus
}
#endif








