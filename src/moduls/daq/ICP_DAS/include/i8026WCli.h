
#ifdef __cplusplus
	extern "C" {
#endif


void EEP_26Init(int slot);
int  EEP_26WriteWord(int slot,int block, int addr, unsigned short Value);
unsigned short  EEP_26ReadWord(int slot,int block,int addr);
short i8026W_SetRefGain(int slot ,short typeCode, unsigned short refGain);
short i8026W_WriteAIGainOffset(int slot,int ch, int Gain,unsigned short GainValue,short OffsetValue) ;

float i8026W_AD(int slot, int chIndex, short gain );
short i8026W_ADHex(int slot, int chIndex, short gain );
short i8026W_SaveAIDefaultSetting(int slot,int ch, int Gain,unsigned short GainValue,short OffsetValue);    
short i8026W_LoadAIDefaultSetting(int slot,int ch, int Gain,unsigned short* GainValue,short* OffsetValue);
short i8026W_RecoverAIDefaultSetting(int slot); 

void i8026W_AI_SaveUserDate(int slot,int ch,int gain, short year, short month, short day); 
void i8026W_AI_LoadUserDate(int slot,int ch,int gain,short *year, short *month, short *day); 
void i8026W_AI_SaveDefaultDate(int slot,int ch,int gain,short year, short month, short day); 
void i8026W_AI_LoadDefaultDate(int slot,int ch,int gain,short *year, short *month, short *day);

	// AO 校正部分
void i8026W_AO_ChGain(int slot, int chIndex, short gain); // 設定 AO CH, Gain
short i8026W_DA (int slot ,int chIndex,short gain,short hexData); // 12 bit DA
short i8026W_WriteAO_GainOffset (int slot ,int chIndex,short gain, short GainValue,short OffsetValue);

short i8026W_SaveAO_DefaultGainOffset (int slot ,int chIndex,short gain,short gainValue,short offsetValue);
void i8026W_LoadAO_DefaultGainOffset(int slot,int chIndex,short gain, unsigned short *gainValue, short *offsetValue);
short i8026W_RecoverAODefaultSetting(int slot);

void i8026W_AO_SaveUserDate(int slot,int ch,int gain,short year, short month, short day); 
void i8026W_AO_LoadUserDate(int slot,int ch,int gain,short *year, short *month, short *day); 
void i8026W_AO_SaveDefaultDate(int slot,int ch,int gain,short year, short month, short day); 
void i8026W_AO_LoadDefaultDate(int slot,int ch,int gain,short *year, short *month, short *day);
	
	 //short i8026W_SetGainOffset_Info(int slot,int Gain,unsigned short GainValue, short offsetValue);
void i8026W_SaveMark(int slot);
int i8026W_HasSaveMark(int slot);
 
#ifdef __cplusplus
 	}
#endif
