#ifdef __cplusplus
extern "C" {
#endif

//Module Functions
DWORD I87124_ActiveModule(void);
DWORD I87124_CloseModule(void);
float I87124_GetDLLVersion(void);

//Firmware Functions
DWORD I87124_GetFirmwareVersion(BYTE Slot, WORD *FirmVer);
DWORD I87124_ResetFirmware(BYTE Slot);

//Master Operating Functions
DWORD I87124_SetMasterMACID(BYTE Slot, BYTE MasterMACID);
DWORD I87124_GetMasterMACID(BYTE Slot, BYTE *MasterMACID);
DWORD I87124_SetBaudRate(BYTE Slot, BYTE BaudRate);
DWORD I87124_GetBaudRate(BYTE Slot, BYTE *BaudRate);
DWORD I87124_GetMasterStatus(BYTE Slot);
DWORD I87124_GetSlaveStatus(BYTE Slot, BYTE DesMACID);

DWORD I87124_AddDevice(BYTE Slot, BYTE DesMACID, WORD Explicit_EPR);
DWORD I87124_RemoveDevice(BYTE Slot, BYTE DesMACID);
DWORD I87124_StartDevice(BYTE Slot, BYTE DesMACID);
DWORD I87124_StopDevice(BYTE Slot, BYTE DesMACID);
DWORD I87124_StartAllDevice(BYTE Slot);
DWORD I87124_StopAllDevice(BYTE Slot);

DWORD I87124_AddIOConnection(BYTE Slot,BYTE DesMACID,BYTE ConType,WORD DeviceInputLen,WORD DeviceOutputLen,WORD EPR);
DWORD I87124_RemoveIOConnection(BYTE Slot,BYTE DesMACID,BYTE ConType);

DWORD I87124_GetAttribute(BYTE Slot, BYTE DesMACID, BYTE ClassID, BYTE InstanceID, BYTE AttributeID);
DWORD I87124_IsGetAttributeOK(BYTE Slot, BYTE DesMACID);
DWORD I87124_GetAttributeValue(BYTE Slot, BYTE DesMACID, WORD *DataLen, BYTE *Data);

DWORD I87124_SetAttribute(BYTE Slot, BYTE DesMACID, BYTE ClassID, BYTE InstanceID, BYTE AttributeID, WORD DataLen, BYTE *DATA);

DWORD I87124_IsSetAttributeOK(BYTE Slot,BYTE DesMACID);

DWORD I87124_ClearAllConfig(BYTE Slot);

DWORD I87124_SearchAllDevices(BYTE Slot);

DWORD I87124_SearchSpecificDevice(BYTE Slot, WORD ListCount, BYTE *DesMACIDList);

DWORD I87124_IsSearchOK(BYTE Slot);

DWORD I87124_GetSearchedDevices(BYTE Slot,WORD *TotalDevices,BYTE *DesMACID,BYTE *Type,WORD *DeviceInputLen,WORD *DeviceOutputLen);

DWORD I87124_GetDeviceInfoFromScanList(BYTE Slot,BYTE DesMACID,WORD *ListCount,BYTE *ConnectionTypeList,WORD *InputDataLenList,WORD *OutputDataLenList,WORD *EPRList);

DWORD I87124_GetScanList(BYTE Slot,WORD *TotalDevices,BYTE *DesMACIDList,BYTE *ConnectionTypeList,WORD *InputDataLenList,WORD *OutputDataLenList,WORD *EPR_List);

DWORD I87124_ImportEEPROM(BYTE Slot,WORD ListCount,BYTE *DesMACIDList,BYTE *ConnectionTypeList,WORD *InputDataLenList,WORD *OutputDataLenList,WORD *EPR_List);

DWORD I87124_SendExplicitMSG(BYTE Slot, BYTE DesMACID, BYTE ServiceID, BYTE ClassID, BYTE InstanceID, WORD DataLen, BYTE *DATA);

DWORD I87124_IsExplicitMSGRespOK(BYTE Slot, BYTE DesMACID);

DWORD I87124_GetExplicitMSGRespValue(BYTE Slot, BYTE DesMACID, WORD *DataLen, BYTE *DATA);

// I/O Configuring Functions

DWORD I87124_ReadInputData(BYTE Slot, BYTE DesMACID, BYTE ConType, WORD *IOLen, BYTE *IODATA);

DWORD I87124_WriteOutputData(BYTE Slot, BYTE DesMACID, BYTE ConType, WORD IOLen, BYTE *IODATA);

DWORD I87124_ReadbackOutputData(BYTE Slot, BYTE DesMACID, BYTE ConType, WORD *IOLen, BYTE *IODATA);

/*
EXP DWORD CALLBACK I87124_GetDeviceInfoFromScanList(BYTE cSlot,BYTE DesMACID,WORD *ListCount,BYTE *ConnectionTypeList,WORD *InputDataLenList,WORD *OutputDataLenList,WORD *EPRList);
EXP DWORD CALLBACK I87124_GetScanList(BYTE cSlot,WORD *TotalDevices,BYTE *DesMACIDList,BYTE *ConnectionTypeList,WORD *InputDataLenList,WORD *OutputDataLenList,WORD *EPR_List);
EXP DWORD CALLBACK I87124_ImportEEPROM(BYTE cSlot,WORD ListCount,BYTE *DesMACIDList,BYTE *ConnectionTypeList,WORD *InputDataLenList,WORD *OutputDataLenList,WORD *EPR_List);
*/

//Function No definition
#define _GetFirmwareVersion   1
#define _SetMasterMACID       2
#define _GetMasterMACID       3
#define _SetBaudRate          4
#define _GetBaudRate          5
#define _AddDevice            6
#define _RemoveDevice         7
#define _AddIOConnection      8
#define _RemoveIOConnection   9
#define _StartDevice          10
#define _StopDevice           11
#define _StartAllDevice       12
#define _StopAllDevice        13
#define _GetAttribute         14
#define _IsGetAttributeOK     15
#define _GetAttributeValue    16
#define _SetAttribute         17
#define _IsSetAttributeOK     18
#define _ReadInputData        19
#define _WriteOutputData      20
#define _ResetFirmware        21
#define _CheckModule          22
#define _GetMasterStatus      23
#define _GetSlaveStatus       24
#define _SearchAllDevices     25
#define _SearchSpecificDevice 26
#define _ImportEEPROM         27
#define _IsSearchOK           28
#define _GetSearchedDevices   29
#define _GetDeviceInfoFromScanList  30
#define _GetScanList          31
#define _ClearAllConfig       32
#define _SendExplicitMSG      33
#define _IsExplicitMSGRespOK  34
#define _GetExplicitMSGRespValue  35
#define _ReadbackOutputData   36
#define _GetFirmDebugWnd      50
#define _FirmDebugWndEnable   51
#define _Load_IntoDownloadMode    52

//ConnectionType
#define ConType_Explicit  0
#define ConType_Poll      1
#define ConType_BitStrobe 2
#define ConType_COS       3
#define ConType_Cyclic    4
#define ConType_MultiPoll 5

//DLL Error Code
#define I87124_NoError               0
#define I87124_TimeOut		     1	
#define I87124_SendCOMError          10001
#define I87124_ReceiveCOMError       10002
#define I87124_ModuletActiveError    10007
#define I87124_ModuleNotActive       10008
#define I87124_ModuleNoResp          10015
#define I87124_ModuleInUse           10025
#define I87124_ReStartModule         10027

//Firmware Error Code
#define DNMXS_NoError               0
#define DNMXS_UnKnowError           5000

//Master Status Error Code
#define DNMXS_BoardNotActive        1000
#define DNMXS_OnlineError           1001
#define DNMXS_CANBusError           1002
#define DNMXS_Booting               1003

//General Error Code
#define DNMXS_MACIDError            1050
#define DNMXS_BaudRateError         1051
#define DNMXS_ConnectionTypeError   1052
#define DNMXS_DuplicMasterMACID     1053 
#define DNMXS_EEPROMError           1054
#define DNMXS_NowScanning           1055
#define DNMXS_ScanListError         1056
#define DNMXS_DeviceExist           1057
#define DNMXS_DeviceNotExist        1058 
#define DNMXS_MapTableError         1059

//IOConnection Error
#define DNMXS_ExplicitNotAllocate   1100
#define DNMXS_PollNotAllocate       1101
#define DNMXS_BitStrobeNotAllocate  1102
#define DNMXS_COSNotAllocate        1103
#define DNMXS_CyclicNotAllocate     1104
#define DNMXS_PollAlreadyExist      1105
#define DNMXS_BitStrobeAlreadyExist 1106
#define DNMXS_COSAlreadyExist       1107
#define DNMXS_CyclicAlreadyExist    1108
#define DNMXS_CommunicationPause    1109

//Slave Error Code
#define DNMXS_SlaveNoResp           1150
#define DNMXS_WaitForSlaveResp      1151
#define DNMXS_SlaveRespError        1152
#define DNMXS_OutputDataLenError    1153
#define DNMXS_InputDataLenError     1154


