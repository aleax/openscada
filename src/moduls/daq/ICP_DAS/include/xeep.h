#ifdef __cplusplus
	extern "C" {
#endif

static int SlotAddr[9] ={0x1,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8};
int  EEP_WriteWord(int slot,int block, int addr, unsigned short Value);


unsigned short  EEP_ReadWord(int slot,int block,int addr); // read 16 bit data


#ifdef __cplusplus
 }
#endif
