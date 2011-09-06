#ifdef __cplusplus
extern "C" {
#endif
 

extern int MBServerInit(void );
extern int MBServerProcStart(void );
extern int MBServerRelease(void );

extern void sMBRTUClose(void);
extern void sMBTCPClose(void);
extern void sMBASCClose(void);

#ifdef __cplusplus
}
#endif
