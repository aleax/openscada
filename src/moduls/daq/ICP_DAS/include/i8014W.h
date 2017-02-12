
#ifdef __cplusplus
extern "C" {
#endif


/*#ifdef _8014W_EXPORTS
	#define I8014WAPI __declspec(dllexport) 
#else
	#define I8014WAPI __declspec(dllimport) 
#endif
*/
typedef int (CALLBACK * PAC_CALLBACK_FUNC)();

#define NoError 					   0
#define ID_ERROR					  -1
#define SLOT_ERROR					-2
#define CHANNEL_ERROR				-3
#define GAIN_ERROR					-4
#define FIFO_EMPTY          -5
#define FIFO_LATCHED    		-6
#define FIFO_OVERFLOW       -7
#define TX_NOTREADY         -8
#define MAX_FIFO		4096


short i8014W_Init(int slot);
short i8014W_GetLibVersion(void);
void i8014W_GetLibDate(char libDate[]);
short i8014W_GetFirmwareVer_L1(int slot);
short i8014W_GetFirmwareVer_L2(int slot);
short i8014W_GetSingleEndJumper(int slot);
void i8014W_ReadGainOffset(int slot,short gain,unsigned short* gainValue, short* offsetValue);
short i8014W_ReadAIHex(int slot,int ch,int gain, short* hVal);
short i8014W_ReadAI(int slot,int ch,int gain, float* fVal);
void i8014W_ClearFIFO(int slot);
void i8014W_UnLockFIFO(int slot);
void i8014W_ConfigMagicScan(int slot,int chArr[],int gainArr[],int scanChCount, float sampleRate,int scanMode,int triggerSource,int triggerState, float* realSampleRate);

short i8014W_StartMagicScan(int slot);
short i8014W_StopMagicScan(int slot);
short i8014W_ReadFIFO_BlockMode(int slot, short hexData[], long readCount ,long* dataCountFromFIFO);
short i8014W_ReadFIFO(int slot, short hexData[], short readCount,short* dataCountFromFIFO);// <=== get data from FIFO ,long* dataCnt
short i8014W_ReadFIFO_InISR(int slot, short hexData[], short triggerLevel,short* dataCountFromFIFO);

void i8014W_CalibrateDataHex(int slot, short iGain,short dataFromFIFO, short* calibratedAI);
void i8014W_CalibrateData(int slot, int iGain,short dataFromFIFO, float* calibratedAI);

short i8014W_InstallMagicScanISR(int slot,void (*isr)(int slot),short triggerLevel); //<=== give a callback function for interrupt
short i8014W_UnInstallMagicScanISR(int slot); //<=== give a callback function for interrupt 
void i8014W_ClearInt(int slot);

#ifdef __cplusplus
}
#endif

