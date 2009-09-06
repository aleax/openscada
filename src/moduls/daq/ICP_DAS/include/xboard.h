
//#define SLOT_MAX 8
#define SLOT_MAX 9   //modify for linpac by cindy 20080912

extern int slot_fd[SLOT_MAX];

//#define _X200_	200 
#define _X201_	201 
#define _X202_	202 
#define _X203_	203
#define _X205_	205
#define _X206_	206

#define _X300_	300 
#define _X301_	301 
#define _X302_	302 
#define _X303_	303 
#define _X304_	304 
#define _X305_	305 
#define _X306_	306 
//#define _X307_	307 
#define _X308_	308 
#define _X309_	309 
#define _X310_	310 
#define _X314_	314 

#define outp(a,b)	outb(1, a, b) 
#define inp(a)		inb(1, a) 

// 20090408
#define GPIO_IN   0X000
#define GPIO_OUT  0X080
#define GPIO_ALT_FN_1_IN  0X100
#define GPIO_ALT_FN_1_OUT 0X180
#define GPIO_ALT_FN_2_IN  0X200
#define GPIO_ALT_FN_2_OUT 0X280
#define GPIO_ALT_FN_3_IN  0X300
#define GPIO_ALT_FN_3_OUT 0X380

unsigned char getGPIO(unsigned int gpio);
void setGPIO(unsigned int gpio, unsigned int value);
void setGPIOInput(unsigned int gpio);
void setGPIOOutput(unsigned int gpio, unsigned int value);
void setGPIOAltFunc(unsigned int gpio, unsigned int mode);

int XEE_Init(int clk_pin,int sda_pin,int wp_pin,int need_pullhigh);
int XEE_InitByName(int Xboard);
void XEE_WriteEnable(void);
void XEE_WriteProtect(void);
int XEE_RandomRead(int Block,int Addr);
int XEE_ReadNext(int Block);
int XEE_MultiRead(int StartBlock,int StartAddr,int no,char *databuf);
int XEE_RandomWrite(int Block,int Addr,int Data);
int XEE_MultiWrite(int Block,int Addr,int no,char *Data);
