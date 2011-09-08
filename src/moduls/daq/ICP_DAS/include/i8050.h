/*******************************************************************/
/*    i-8050 driver head file                                      */
/*                                                                 */
/*    Note: head file version is different to lib file version.    */
/*    create: May,2007                                             */
/*******************************************************************/

//#define OFST_DIO_WRITE_BYTE_5   10	//8050 config offset
//=============2005/08/18 i8050 add by Stanley================
/*static unsigned uDIOconfig[8]={
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};*/

unsigned short UDIO_WriteConfig_16(int slot,unsigned short config); 
/*
  Write 8050 Config
  Set I/O type: DI or DO
  Slot: 0~7, the slot number where the I/O module is plugged into.
  Channel: 0~15
  ex: config --> 0xffff:DO, 0x0000:DI
*/

unsigned short UDIO_ReadConfig_16(int slot);
/*
  Read 8050 Config
  Read I/O type: DI or DO
  Slot: 0~7, the slot number where the I/O module is plugged into.
*/


void UDIO_DO16(int slot,unsigned short config); //1:ON , 0:OFF
/*
  This function is used to output 16-bit data to 8050.
  Slot: 0~7, the slot number where the I/O module is plugged into.
*/


unsigned short UDIO_DI16(int slot);//1:ON , 0:OFF
/*
  This function is used to obtain 16-bit input data from 8050.
  Slot: 0~7, the slot number where the I/O module is plugged into.
*/

