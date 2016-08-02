#ifdef __cplusplus
	extern "C" {
#endif

//unsigned int SlotAddr[9] ={0x00000000, 0x01000000, 0x02000000, 0x03000000,0x04000000, 0x05000000, 0x06000000, 0x07000000, 0x08000000};

int EEP_WriteWord(int slot,int block, int addr, unsigned short Value);

unsigned short EEP_ReadWord(int slot,int block,int addr); // read 16 bit data

#ifdef __cplusplus
 }
#endif
