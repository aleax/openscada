/*
AI
CLK=DIO4(81) DATA=DIO14(118) CS=DIO9(117)

AO
CLK=DIO4(81) DATA=DIO14(118) CS=TO1(86)
*/
#include "xboard.h"

#define SetDio9Dir(a)	((a)?setGPIOInput(117):setGPIOOutput(117,1))
#define SetDio9High()	(void) setGPIO(117,1)
#define SetDio9Low()	(void) setGPIO(117,0)

#define SetDio4Dir(a)	((a)?setGPIOInput(81):setGPIOOutput(81,1))
#define SetDio4High()	(void) setGPIO(81,1)
#define SetDio4Low()	(void) setGPIO(81,0)

#define SetDio14Dir(a)	((a)?setGPIOInput(118):setGPIOOutput(118,1))
#define SetDio14High()	(void) setGPIO(118,1)
#define SetDio14Low()	(void) setGPIO(118,0)
#define GetDio14()	(unsigned char) getGPIO(118)

#define SetTo1Dir(a)	((a)?setGPIOInput(86):setGPIOOutput(86,1))
#define SetTo1High()	(void) setGPIO(86,1)
#define SetTo1Low()	(void) setGPIO(86,0)
