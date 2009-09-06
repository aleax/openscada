#ifdef __cplusplus
extern "C" {
#endif
  
#define DNM_COMPortTimeout 9999 //Error Code


float DNM_GetLibraryVersion(void);
short DNM_ActiveBoard(BYTE Slot);
short DNM_CloseAllBoard(void);
short DNM_ResetFirmware(void);
float DNM_GetFirmwareVersion(void);
short DNM_SetMasterMACID(unsigned char MACID);
short DNM_SetBaudRate(unsigned char baudrate);
short DNM_AddDevice(unsigned char DesMACID,unsigned short Explicit_EPR);
short DNM_RemoveDevice(unsigned char DesMACID);
short DNM_RemoveIOConnection(unsigned char DesMACID,unsigned char ConnectionType);
short DNM_StartDevice(unsigned char DesMACID);
short DNM_StopDevice(unsigned char DesMACID);
short DNM_StartAllDevice(void);
short DNM_StopAllDevice(void);
short DNM_GetAttribute(unsigned char DesMACID,unsigned char ClassID,unsigned char InstanceID,unsigned char AttributeID);
short DNM_GetAttributeValue(unsigned char DesMACID,unsigned short *DataLen,unsigned char *Data);
short DNM_SetAttribute(unsigned char DesMACID,unsigned char ClassID,unsigned char InstanceID,unsigned char AttributeID,unsigned short DataCount,unsigned char *Data);
short DNM_SetAttributeResponse(unsigned char DesMACID);
short DNM_ConfigPoll(unsigned char DesMACID,unsigned short RemoteInputLen,unsigned short RemoteOutputLen,unsigned short EPR);
short DNM_ReadPollInputData(unsigned char DesMACID,unsigned short *DataLen,unsigned char *Data);
short DNM_WritePollOutputData(unsigned char DesMACID,unsigned short DataLen,unsigned char *IOData);
short DNM_ConfigBitStrobe(unsigned char DesMACID,unsigned short RempteInputLen,unsigned short EPR);
short DNM_ReadBitStrobeInputData(unsigned char DesMACID,unsigned short *DataLen,unsigned char *IOData);
short DNM_ConfigCOS(unsigned char DesMACID,unsigned short RemoteInputLen,unsigned short RemoteOutputLen,unsigned short EPR);
short DNM_ReadCOSInputData(unsigned char DesMACID,unsigned short *DataLen,unsigned char *IOData);
short DNM_WriteCOSOutputData(unsigned char DesMACID,unsigned short DataLen,unsigned char *IOData);
short DNM_ConfigCyclic(unsigned char DesMACID,unsigned short RemoteInputLen,unsigned short RemoteOutputLen,unsigned short EPR);
short DNM_ReadCyclicInputData(unsigned char DesMACID,unsigned short *DataLen,unsigned char *IOData);
short DNM_WriteCyclicOutputData(unsigned char DesMACID,unsigned short DataLen,unsigned char *IOData);
short DNM_GetExplicitStatus(unsigned char DesMACID);
short DNM_GetPollStatus(unsigned char DesMACID);
short DNM_GetBitStrobeStatus(unsigned char DesMACID);
short DNM_GetCOSStatus(unsigned char DesMACID);
short DNM_GetCyclicStatus(unsigned char DesMACID);
short DNM_GetExplicitResult(unsigned char DesMACID);
short DNM_GetPollResult(unsigned char DesMACID);
short DNM_GetBitStrobeResult(unsigned char DesMACID);
short DNM_GetCOSResult(unsigned char DesMACID);
short DNM_GetCyclicResult(unsigned char DesMACID);
short DNM_GetDeviceStatus(unsigned char DesMACID);
short DNM_AutoScanDevice(unsigned char *DesMACID,unsigned char *ConnectionType,unsigned short *InputLen,unsigned short *OutputLen,unsigned short *TotalDataCount);
short DNM_UpdateBitStrobeConfig(unsigned char DesMACID,unsigned short RemoteInputLen,unsigned short EPR);
short DNM_UpdatePollConfig(unsigned char DesMACID,unsigned short RemoteInputLen,unsigned short RemoteOutputLen,unsigned short EPR);
short DNM_UpdateCOSConfig(unsigned char DesMACID,unsigned short RemoteInputLen,unsigned short RemoteOutputLen,unsigned short EPR);
short DNM_UpdateCyclicConfig(unsigned char DesMACID,unsigned short RemoteInputLen,unsigned short RemoteOutputLen,unsigned short EPR);
short DNM_GetMasterMACID(void);
short DNM_GetBaudRate(void);
short DNM_GetMasterStatus(void);
short DNM_DebugDevice(unsigned char DesMACID);
short DNM_ClearAllConfig(void);
short DNM_GetDevicePollInfo(unsigned char DesMACID,unsigned short *RemoteInputDataLen,unsigned short *RemoteOutputDataLen);
short DNM_GetDeviceBitStrobeInfo(unsigned char DesMACID,unsigned short *RemoteInputDataLen);
short DNM_GetDeviceCOSInfo(unsigned char DesMACID,unsigned short *RemoteInputDataLen,unsigned short *RemoteOutputDataLen);
short DNM_GetDeviceCyclicInfo(unsigned char DesMACID,unsigned short *RemoteInputDataLen,unsigned short *RemoteOutputDataLen);
short DNM_GetScanList(unsigned short *ListCount, unsigned char *DesMACIDList, unsigned char *ConnectionTypeList,unsigned short *InputDataLenList,unsigned short *OutputDataLenList,unsigned short *EPR_List);
short DNM_GetDeviceInfo(unsigned char DesMACID,unsigned short *DataCount,unsigned char *ConnectionTypeList,unsigned short *InputDataLenList,unsigned short *OutputDataLenList,unsigned short *EPR_List);
short DNM_GetLastDeviceError(unsigned char *DesMACID,unsigned char *Type,short *ErrorCode);
short DNM_SetBreakOption(unsigned char DesMACID,unsigned char BreakOption);
short DNM_GetBreakOption(unsigned char DesMACID);

//ConnectionType
#define ConType_Explicit  0
#define ConType_Poll      1
#define ConType_BitStrobe 2
#define ConType_COS       3
#define ConType_Cyclic    4
#define ConType_MultiPoll 5


#define DNM_NoError 0
#define DNM_TimeOut 1

//General Error Code
#define DNM_MACIDError            1000
#define DNM_MasterMACIDError      1001
#define DNM_BaudRateError         1002
#define DNM_ConnectionTypeError   1003
#define DNM_DuplicMasterMACID     1004  
#define DNM_BoardNotActive        1005


//ScanList Error Code
#define DNM_ScanListError         1100
#define DNM_DeviceDuplicate       1101
#define DNM_DeviceNotExist        1102
#define DNM_ScanListFull          1103
#define DNM_ScanListEmpty         1104
#define DNM_ScanListDataNotFind   1105
#define DNM_ExplicitNotAllocate   1106
#define DNM_PollNotAllocate       1107
#define DNM_BitStrobeNotAllocate  1108
#define DNM_COSNotAllocate        1109
#define DNM_CyclicNotAllocate     1110
#define DNM_PollAlreadyExist      1111
#define DNM_BitStrobeAlreadyExist 1112
#define DNM_COSAlreadyExist       1113
#define DNM_CyclicAlreadyExist    1114

//MapTable Error Code
#define DNM_MapTableFull          1200
#define DNM_MapDataDuplicate      1201
#define DNM_MapDataNotFound       1202
#define DNM_MapTableError         1203

//DeviceNet Error Code
#define DNM_BoardNotOnline        1300
#define DNM_ExplicitNotEstablish  1301
#define DNM_PollNotEstablish      1302
#define DNM_BitStrobeNotEstablish 1303
#define DNM_COSNotEstablish       1304
#define DNM_CyclicNotEstablish    1305

//EEPROM Error Cdoe
#define DNM_ReadEEPROMError       1400
#define DNM_WriteEEPROMError      1401
#define DNM_EEPROMDataError       1402
#define DNM_EEPROMFull            1403
#define DNM_DeviceNotExistInEEP   1404
#define DNM_EEPROMDeviceDuplicate 1405
#define DNM_EEPROMError           1406
#define DNM_EEPROMNoData          1407

//CAN chip Error
#define DNM_SendCANMsgError       1500
#define DNM_CANConfigError        1501
#define DNM_CANRxBufferError      1502
#define DNM_CANInstallIRQError    1503


//Online Error Code
#define DNM_OnlineError                1600

//DNMXS_RemoveIOConnection Return Error Cdoe
#define DNM_ConnectionTypeNotSupport   1700

//DNMXS_ConfigBitStrobe Return Error Cdoe
#define DNM_BitStrobeAlreadyExistInEEP 1800

//DNMXS_ConfigPoll Return Error Cdoe
#define DNM_PollAlreadyExistInEEP      1900

//DNMXS_ConfigCOS Return Error Cdoe
#define DNM_COSAlreadyExistInEEP       2000

//DNMXS_ConfigCyclic Return Error Cdoe
#define DNM_CyclicAlreadyExistInEEP    2100

//DNMXS_WritePollOutputData Return Error Cdoe
#define DNM_PollOutputDataLenError     2200

//DNMXS_WriteCOSOutputData Return Error Cdoe
#define DNM_COSOutputDataLenError      2300

//DNMXS_WriteCyclicOutputData Return Error Cdoe
#define DNM_CyclicOutputDataLenError   2400

//DNMXS_GetDeviceStatus Return Error Cdoe
#define DNM_ExplicitUnConnect          2500
#define DNM_PollUnConnect              2501
#define DNM_BitStrobeUnConect          2502
#define DNM_COSUnConnect               2503
#define DNM_CyclicUnConnect            2504

//DNMXS_GetMasterStatus Return Error Cdoe
#define DNM_BusError                   2600
#define DNM_MACIDDuplicate             2601

//DNMXS_SetBreakOption
#define DNM_BreakOptionError           2700

//Debug functions
#define DNM_WithoutPoll                2800
#define DNM_WithoutBitStrobe           2801
#define DNM_WithoutCOS_Cyclic          2802

//GetLastDeviceError
#define DNM_DevicesError               2900

//////////////////////////////////////////////////////////////////////////
//ConnectionStatus Return Code 
//////////////////////////////////////////////////////////////////////////
#define Status_Initial               0  
#define Status_ExplicitEstablishOK   1  
#define Status_ExplicitEstablishing  2 
#define Status_ExplicitUnEstablish   3
#define Status_TimeoutActionUnSet    4

#define Status_TimeoutActionSetOK    5
#define Status_TimeoutActionSetting  6
#define Status_IOReady               7  
#define Status_BitStrobeEstablishOK  10 
#define Status_BitStrobeEstablishing 11 
#define Status_BitStrobeUnEstablish  12 
#define Status_BitStrobeGetingProducedSize 13 
#define Status_BitStrobeEPRSetOK     16 

#define Status_BitStrobeEPRSetting   17 
#define Status_PollEstablishOK        20 
#define Status_PollEstablishing       21 
#define Status_PollUnEstablish        22 
#define Status_PollGetingProducedSize 23  
#define Status_PollGetProducedSizeOK  24  
#define Status_PollGetingConsumedSize 25  
#define Status_PollGetConsumedSizeOK  26 
#define Status_PollEPRUnSet           27 
#define Status_PollEPRSetOK           28 
#define Status_PollEPRSetting         29 
#define Status_COS_CyclicEstablishOK  30 
#define Status_COS_CyclicEstablishing 31 
#define Status_COS_CyclicUnEstablish  32 
#define Status_COS_CyclicGetingProducedSize 33 
#define Status_COS_CyclicGetProducedSizeOK  34  
#define Status_COS_CyclicGetingConsumedSize 35  
#define Status_COS_CyclicGetConsumedSizeOK  36  
#define Status_COS_CyclicEPRUnSet     37 //COS_Cyclic Connection
#define Status_COS_CyclicEPRSetOK     38 //COS_Cyclic Connection
#define Status_COS_CyclicEPRSetting   39 //COS_Cyclic Connection
#define Status_COS_CyclicACKErr       40 //COS_Cyclic ACK 
#define Status_COS_CyclicACKTMO       41 //COS_Cyclic ACK


//////////////////////////////////////////////////////////////////////////
//Result Return Code
//////////////////////////////////////////////////////////////////////////
#define Result_None            0  // 
#define Result_AllocateResOK   1  // Allocate
#define Result_AllocateResERR  2  // Allocate
#define Result_AllocateResTMO  3  // Allocate
#define Result_SetActionResOK  4  // Set Watchdog Timeout Action
#define Result_SetActionResERR 5  // Set Watchdog Timeout Action
#define Result_SetActionResTMO 6  // Set Watchdog Timeout Action
#define Result_ReleaseResOK    7  // Release
#define Result_ReleaseResERR   8  // Release
#define Result_ReleaseResTMO   9  // Release
#define Result_GetAttrResOK    10 // Get Attribute
#define Result_GetAttrResERR   11 // Get Attribute
#define Result_GetAttrResTMO   12 // Get Attribute
#define Result_SetAttrResOK    13 // Set Attribute
#define Result_SetAttrResERR   14 // Set Attribute
#define Result_SetAttrResTMO   15 // Set Attribute
#define Result_SetAttrACKTMO   16 // Set Attribute ACK 

#define Result_BitStrobeEPRResOK  20 // Bit Strobe Set Expected Packet Rate
#define Result_BitStrobeEPRResERR 21 // Bit Strobe Set Expected Packet Rate
#define Result_BitStrobeEPRResTMO 22 // Bit Strobe Set Expected Packet Rate
#define Result_BitStrobeIOResOK   23 // Bit Strobe IO 
#define Result_BitStrobeIOResERR  24 // Bit Strobe IO 
#define Result_BitStrobeIOResTMO  25 // Bit Strobe IO 
#define Result_BitStrobeProducedResErr 26 // BitStrobe Get Produced Size
#define Result_BitStrobeProducedResTMO 27 // BitStrobe Get Produced Size
#define Result_BitStrobeProducedSizeError 28 //BitStrobe InputDataLen
#define Result_PollEPRResOK       30 // Poll Set Expected Packet Rate
#define Result_PollEPRResERR      31 // Poll Set Expected Packet Rate
#define Result_PollEPRResTMO      32 // Poll Set Expected Packet Rate
#define Result_PollIOResOK        33 // Poll IO 
#define Result_PollIOResERR       34 // Poll IO 
#define Result_PollIOResTMO       35 // Poll IO 
#define Result_PollProducedResErr 36 // Poll Get Produced Size 
#define Result_PollProducedResTMO 37 // Poll Get Produced Size 
#define Result_PollProducedSizeError 38 //Poll InputDataLen 
#define Result_PollConsumedResErr 39 // Poll Get Consumed Size
#define Result_PollConsumedResTMO 40 // Poll Get Consumed Size
#define Result_PollConsumedSizeError 41 //Poll OutputDataLen

#define Result_COS_CyclicEPRResOK    53 // COS_Cyclic Set Expected Packet Rate
#define Result_COS_CyclicEPRResERR   54 // COS_Cyclic Set Expected Packet Rate
#define Result_COS_CyclicEPRResTMO   55 // COS_Cyclic Set
#define Result_COS_CyclicIOResOK     56 // COS_Cyclic IO 
#define Result_COS_CyclicIOResERR    57 // COS_Cyclic IO 
#define Result_COS_CyclicIOResTMO    58 // COS_Cyclic IO 
#define Result_COS_CyclicACKResOK    59 // COS_Cyclic ACK 
#define Result_COS_CyclicACKResERR   60 // COS_Cyclic ACK 
#define Result_COS_CyclicACKResTMO   61 // COS_Cyclic ACK 
#define Result_COS_CyclicProducedResErr 62 // COS_Cyclic Get Produced Size 
#define Result_COS_CyclicProducedResTMO 63 // COS_Cyclic Get Produced Size 
#define Result_COS_CyclicProducedSizeError 64 //COS_Cyclic InputDataLen 
#define Result_COS_CyclicConsumedResErr 65 // COS_Cyclic Get Consumed Size 
#define Result_COS_CyclicConsumedResTMO 66 // COS_Cyclic Get Consumed Size
#define Result_COS_CyclicConsumedSizeError 67 
#define Result_ModuleNoResponse    70 //Slave Module still no response

#ifdef __cplusplus
}
#endif

