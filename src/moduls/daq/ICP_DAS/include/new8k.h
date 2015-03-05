#define BP_BASE				     0x9000
#define BP_VER			 	     BP_BASE
#define BP_RESET			     BP_BASE
#define BP_NETID			     1  //BP_BASE+1)
#define BP_SET_SLOT		     2	 //(BP_BASE+2)
#define BP_EOI				     3  //(BP_BASE+3)
#define BP_SLOT_INT		     3	 //(BP_BASE+3)
#define BP_HOT_PLUG		     4	 //(BP_BASE+4)
#define BP_IER				     5  //(BP_BASE+5)
#define BP_INT_RE_TRIG	   6  //(BP_BASE+6)
#define BP_PRI_IRQ			   5  //(BP_BASE+6)
#define BP_SECOND_IRQ		   7  //(BP_BASE+7)
#define BP_R_EDGE_IER		   8  //(BP_BASE+8)
#define BP_R_EDGE_IRQ		   9  //(BP_BASE+9)
#define BP_LEVEL_IER		   10 //(BP_BASE+0x0A)
#define BP_LEVEL_IRQ		   11 //(BP_BASE+0x0B)
#define BP_F_EDGE_IER		   12 //(BP_BASE+0x0C)
#define BP_F_EDGE_IRQ		   13 //(BP_BASE+0x0D)

#define BP_RTC_CTRL_STATUS 14	//(BP_BASE+0x0E)

#define BP_SRAM_BANK		   15 //(BP_BASE+0x0F)

#define BP_TIMER_OH_LOW		 16 //(BP_BASE+0x10)
#define BP_TIMER_OH_HIGH	 17 //(BP_BASE+0x11)
#define BP_TIMER_OL_LOW		 18 //(BP_BASE+0x12)
#define BP_TIMER_OL_HIGH	 19 //(BP_BASE+0x13)

#define BP_TIMETICK			   20 //(BP_BASE+0x14)

#define BP_COM2_ST			   24 //(BP_BASE+0x18)
#define BP_COM3_ST			   25 //(BP_BASE+0x19)
#define BP_COM4_ST			   26 //(BP_BASE+0x1A)

#define BP_TIMER1_LOW		   28 //(BP_BASE+0x1C)
#define BP_TIMER1_HIGH		 29 //(BP_BASE+0x1D)
#define BP_TIMER2_LOW		   30 //(BP_BASE+0x1E)
#define BP_TIMER2_HIGH		 31 //(BP_BASE+0x1F)

#define BP_ENABLE_SLOT_INT	34 //(BP_BASE+0x22)
#define BP_CONFIG			      35 //(BP_BASE+0x23)

#define BP_COM2_BASE		    40  //(BP_BASE+0x28)
#define BP_COM3_BASE	      48 //	(BP_BASE+0x30)
#define BP_COM4_BASE		    56 //(BP_BASE+0x38)

#define BP_SLOT_1	0x1000
#define BP_SLOT_2	0x2000
#define BP_SLOT_3	0x3000
#define BP_SLOT_4	0x4000
#define BP_SLOT_5	0x5000
#define BP_SLOT_6	0x6000
#define BP_SLOT_7	0x7000
#define BP_SLOT_8	0x8000

#define BP_SramBase	        0xA000
#define S256Base        	  BP_SramBase
#define S256_BlockSize		  2048	
#define S256_BlockWordSize	1024
#define S256_MaxBlock		    256
