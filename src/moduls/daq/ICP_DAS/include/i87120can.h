#ifdef __cplusplus
extern "C" {
#endif

#define CAN87K_OK                    0
#define CAN87K_TIMEOUT              21
#define CAN87K_FIFO_EMPTY           22
#define CAN87K_FIFO_FULL            23
#define CAN87K_INIT_ERROR           24
#define CAN87K_PARATERS_ERROR       25
#define CAN87K_CAN_REG_ERROR        26

int I87KCANConfig(unsigned char Ack, unsigned char BufferSize);
int GetI87KCANConfig(unsigned char *Ack, unsigned char *BufferSize);
int SetCANBaud(unsigned long Baud, unsigned char BT0, unsigned char BT1);
int GetCANBaud(unsigned long *Baud, unsigned char *BT0, unsigned char *BT1);
int SetCANMask(unsigned long AccCode, unsigned long AccMask);
int GetCANMask(unsigned long *AccCode, unsigned long *AccMask);
int ClearStatus(void);
int GetStatus(unsigned char *CANReg, unsigned char *OverflowFlag);
int ResetI87KCAN(void);
int I87KCANInit(unsigned long CANBaud, unsigned char BT0, unsigned char BT1, 
                unsigned long CAN_AccCode, unsigned long CAN_Mask, unsigned char Ack, 
                unsigned char BufferSize);
int GetCANMsg(unsigned char *Mode, unsigned long *MsgID, unsigned char *RTR, 
              unsigned char *DataLen, unsigned char *Data);
int SendCANMsg(unsigned char Mode,unsigned long MsgID, unsigned char RTR, 
               unsigned char DataLen, unsigned char *Data);
#ifdef __cplusplus
}
#endif
