
#ifdef __cplusplus
extern "C" {
#endif

#define NoError			0
#define ID_ERROR		-1
#define SLOT_ERROR		-2
#define CHANNEL_ERROR		-3
#define GAIN_ERROR		-4

void  i8026W_Set_AVGCnt(int slot, unsigned short cnt);
void  i8026W_SetDelay( short dly);
short i8026W_GetDelay( );
short i8026W_TEST(int slot, int chIndex, short gain, float * f );
// basic api 
short i8026W_Init(int slot);
short i8026W_GetLibVersion(void);
void  i8026W_GetLibDate(char libDate[]);
short i8026W_GetFirmwareVer(int slot);


// AO output API
void  i8026W_ReadAO_GainOffset(int slot,int chIndex,short gain,unsigned short *gainValue, short *offsetValue);
short i8026W_WriteAOHex (int slot ,int chIndex,short gain,int hexData);
short i8026W_WriteAO (int slot ,int chIndex,short gain,float aoData);
short i8026W_ReadbackAO (int slot ,int chIndex, float* aoFVal);
short i8026W_ReadbackAOHex (int slot ,int chIndex, short* aoHVal);


// DIO API
void  i8026W_WriteDO(int slot, short doVal);
void  i8026W_WriteDOBit(int slot,int chIndex, short bitVal);
short i8026W_ReadDIO(int slot, short* diVal, short* doVal,unsigned char diBitArr[], unsigned char doBitArr[]);


// AI API
short i8026W_ReadAIGainOffset(int slot,int ch, int gain,unsigned short *gainValue,short *offsetValue);
short i8026W_ReadAIHex(int slot, int chIndex, short Gain, short* hVal);
short i8026W_ReadAI(int slot, int chIndex, short gain, float* fVal);
void  i8026W_Set_AVGCnt(int slot, unsigned short cnt);

#ifdef __cplusplus
}
#endif

