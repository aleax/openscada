

#ifdef __cplusplus
extern "C" {
#endif

#define NoError 					  0
#define ID_ERROR					 -1
#define SLOT_OUT_RANGE		 -2
#define CHANNEL_OUT_RANGE	 -3
#define MODE_ERROR				 -4
#define INT_MODE_ERROR  	 -5
#define NOT_SUPPORT_ERROR	 -6


int i8093W_Init(int slot);
short i8093W_GetFirmwareVersion(int slot);
short i8093W_GetLibVersion(void);
void i8093W_GetLibDate(char libDate[]);

int i8093W_SetMode(int slot, int ch, int Mode);
int i8093W_GetMode(int slot, int ch, int* Mode);
int i8093W_GetLineStatus(int slot, int ch, int* A_Status,int* B_Status,int* C_Status);
int i8093W_GetIndex(int slot, int ch,int* index);
int i8093W_SetXOR(int slot, int ch, int Xor);
int i8093W_GetXOR(int slot, int ch, int* Xor);

int i8093W_Read32BitEncoder(int slot,int ch, long* EnCode32);
int i8093W_ResetEncoder(int slot, int ch);


#ifdef __cplusplus
 }
#endif

