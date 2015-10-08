#ifdef __cplusplus
extern "C" {
#endif


extern int MBServerInit(void);
extern int MBServerProc(void); 
extern int MBServerRelease(void);


extern int sMBRTUInit(unsigned char slaveAddr,unsigned char portIndex,unsigned long ulBaudRate,unsigned char parity,
                      unsigned char dataBits, unsigned char stopBit);
extern int sMBTCPInit(unsigned short port);				

extern void sMBRTUClose(void);
extern void sMBTCPClose(void);

				
extern int MB_enable_DIOBuffer(unsigned char *DI,unsigned char *DO,int SIZE);
extern int MB_enable_AIOBuffer(int *AI, int *AO,int SIZE);

#ifdef __cplusplus
}
#endif
