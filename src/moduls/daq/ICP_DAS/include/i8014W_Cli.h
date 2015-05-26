

#ifdef __cplusplus
	extern "C" {
#endif

/*#ifdef I8014W_CALIBRATE_EXPORTS
	#define I8014WCALI __declspec(dllexport) 
#else
	#define I8014WCALI __declspec(dllimport) 
#endif
*/


short EEP_WriteWord(short slot,short block, short addr, unsigned short Value);
unsigned short EEP_ReadWord(short slot,short block,short addr); // read 16 bit data
short i8014W_SaveDefaultSetting(short slot,short Gain,unsigned short GainValue, short OffsetValue,short RegistorType);                                                                                                                                 
short i8014W_QCLoadDefaultSetting(short slot,short Gain,unsigned short* GainValue, short* OffsetValue,short RegistorType);                                                                                                                          
 //short i8014W_LoadDefaultSetting(int slot,int Gain,unsigned short* GainValue, short* OffsetValue);
short i8014W_LoadDefaultSetting(short slot,short Gain,unsigned short* GainValue, short* OffsetValue,short RegistorType);
short i8014W_ReadGainOffset_Info(short slot,short Gain,unsigned short* GainValue, short* OffsetValue);
short i8014W_RecoverDefaultSetting(short slot);
short i8014W_SetGainOffset_Info(short slot,short Gain,unsigned short GainValue, short offsetValue);
void i8014W_ClearSavedMark(short slot);
void i8014W_SaveMark(short slot);
short i8014W_HasSaveMark(short slot);
 short i8014W_LoadSelectedGainOffset(short slot,short RegistorType,short Gain); 

 
#ifdef __cplusplus
 	}
#endif
