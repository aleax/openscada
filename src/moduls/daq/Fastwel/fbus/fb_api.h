#ifndef __FBUS_API_H__
#define __FBUS_API_H__

#include <fb_node.h>

// Результ
typedef enum _FBUS_RESULT
{
  FBUS_RES_OK = 0,
  FBUS_RES_INVALID_STATE,
  FBUS_RES_NO_SYSTEM_RESOURCE,
  FBUS_RES_INCORRECT_PARAM,
  FBUS_RES_OPEN_ADAPTER,
  FBUS_RES_GROUP_UNDEFINED,
  FBUS_RES_INVALID_GROUP_CONFIG,
  FBUS_RES_GROUP_ALLOCATION_ERR,
  FBUS_RES_GROUP_CONFIG_NOT_SINCHRONIZED,
  FBUS_RES_GROUP_NOT_CREATED,
  FBUS_RES_INVALID_GROUP_ID,
  FBUS_RES_NODE_NOT_ASSIGNED_TO_GROUP,
  FBUS_RES_INVALID_EXCHANGE_REGION,
  FBUS_RES_RPC_ERROR,
  FBUS_RES_TIMEOUT,

  // Hardware errors
  FBUS_RES_FRAME_ERROR,
  FBUS_RES_MODULE_NOT_ANSWER,
  FBUS_RES_MODULE_SAY_BAD_CRC,
  FBUS_RES_BAD_CRC,
  FBUS_RES_MODULE_BUSY,
  FBUS_RES_BYTES_MORE_THEN_NEED,
  FBUS_RES_MODULE_PENDING_ERROR,

  // Unspecified
  FBUS_RES_SYSTEM_ERROR,

} FBUS_RESULT;

// Handle объекта
typedef size_t FBUS_HANDLE;
#define FBUS_INVALID_HANDLE ((size_t)(-1))

typedef enum
{
  FIO_MODULE_UNKNOWN = 0,

  // Аналоговые модули
  FIO_MODULE_AIM720,
  FIO_MODULE_AIM721,
  FIO_MODULE_AIM722,
  FIO_MODULE_AIM723,
  FIO_MODULE_AIM724,
  FIO_MODULE_AIM725,
  FIO_MODULE_AIM726,
  FIO_MODULE_AIM727,
  FIO_MODULE_AIM728,
  FIO_MODULE_AIM729,
  FIO_MODULE_AIM730,
  FIO_MODULE_AIM731,
  FIO_MODULE_AIM732,
  FIO_MODULE_AIM733,

  // Цифровые модули
  FIO_MODULE_DIM710,
  FIO_MODULE_DIM711,
  FIO_MODULE_DIM712,
  FIO_MODULE_DIM713,
  FIO_MODULE_DIM714,
  FIO_MODULE_DIM715,
  FIO_MODULE_DIM716,
  FIO_MODULE_DIM717,
  FIO_MODULE_DIM718,
  FIO_MODULE_DIM719,
  FIO_MODULE_DIM760,
  FIO_MODULE_DIM761,
  FIO_MODULE_DIM762,
  FIO_MODULE_DIM763,
  FIO_MODULE_DIM764,

  // Интерфейсные модули
  FIO_MODULE_NIM741,
  FIO_MODULE_NIM742,

  // Модули питания
  FIO_MODULE_OM751,

}
FIO_MODULE_TYPE;

// Структура параметров описания МВВ
//----------------------------------------------------------------------------------------------------
typedef struct _FIO_MODULE_DESC
{
  FIO_MODULE_TYPE type;               // Тип МВВ
  char            typeName[21];       // Название МВВ (заканчивающаяся 0 ASCII строка)
  uint32_t        productionCode;     // Уникальный код модели МВВ
  uint32_t        serialNumber;       // Серийный номер изделия
  uint8_t         fbusVer[2];         // Версия FBUS: major, minor
  uint8_t         fwVer[2];           // Версия прошивки: major, minor

  size_t          specificRoSize;     // Размер области неизменяемых специфических параметров
  size_t          specificRwSize;     // Размер области изменяемых специфических параметров
  size_t          inputsSize;         // Размер области входных данных (текущие измерения на входах МВВ) 
  size_t          outputsSize;        // Размер области выходных данных (текущие значения для актуализации выходов МВВ)
}
FIO_MODULE_DESC, *PFIO_MODULE_DESC;

// Структура параметров МВВ в операциях группового обмена
//----------------------------------------------------------------------------------------------------
typedef struct _FIO_MODULE_GROUP_CONF
{
  // Идентификатор группы. Допустимые значения в области от [0x80 - 0xBF]. Максимальное число групп 64.
  // Значение 0xFF означает что МВВ не участвует в групповом обмене.
  uint8_t         groupID;

  uint16_t	      outputPacketDataOffset;	// Смещение данных модуля в цепочечном пакете к модулям
  uint8_t         outputModuleDataLength; // Длина данных модуля в цепочечном пакете к модулям
  uint8_t         outputModuleDataOffset;	// Смещение в области выходных данных модуля, начиная с которого модуль записывает принятую информацию
  uint16_t        outputPacketCRCOffset;	// Смещение поля общего CRC в цепочечном пакете к модулям

  uint16_t	      inputPacketDataOffset;	// Смещение данных модуля в цепочечном пакете от модулей
  uint8_t         inputModuleDataLength;  // Длина данных модуля в цепочечном пакете от модулей
  uint8_t         inputModuleDataOffset;	// Смещение в области входных данных модуля, начиная с которого модуль передает информацию
  uint16_t        inputPacketCRCOffset;	  // Смещение поля общего CRC в цепочечном пакете от модулей
} 
FIO_MODULE_GROUP_CONF, *PFIO_MODULE_GROUP_CONF;

// Структура изменяемых общих параметров МВВ
//----------------------------------------------------------------------------------------------------
typedef struct _FIO_MODULE_COMMON_CONF
{
  // Идентификатор конфигурации МВВ
  uint32_t        configurationID;

  // Для модулей ввода/вывода имеющих выходные каналы.
  // Интервал времени (сек), по истечении которого МВВ переведет свои выходы в безопасное состояние
  // при отсутствии запросов со стороны мастера (0 - функция отключена).
  // Использование в текущей версии FIO ограничено.
  uint8_t         hostWatchdogInterval;

  // Номер синхронизирующего сообщения при получении которого МВВ актуализирует свои выходы.
  // Значение FBUS_UNDEFINED_SYNC_ID (0xFF) означает, что МВВ актуализирует свои выходы асинхронно. 
  uint8_t         outputSync;
  // Номер синхронизирующего сообщения при получении которого МВВ актуализирует свои входы
  // Значение FBUS_UNDEFINED_SYNC_ID (0xFF) означает, что МВВ актуализирует свои входы асинхронно. 
  uint8_t         inputSync;

  // Параметры конфигурации группового обмена
  FIO_MODULE_GROUP_CONF groupConf;
} 
FIO_MODULE_COMMON_CONF, *PFIO_MODULE_COMMON_CONF;

#ifdef __cplusplus
extern "C"
{
#endif

//----------------------------------------------------------------------------------------------------
/** 
 * Function	    : fbusGetVersion
 *
 * Parameters	  : pMagorVersion - указатель на переменную в которой будет возвращен первый номер версии ПО
 *                pMinorVersion - указатель на переменную в которой будет возвращен второй номер версии ПО
 *
 * Return value : FBUS_RES_OK - в случае успеха
 *                FBUS_RES_INCORRECT_PARAM - если значение pMagorVersion или pMinorVersion равно нулю
 *
 * Description  : Функция возвращает номер версии ПО FBUS API.
 **/
FDLL_EXPORT FBUS_RESULT fbusGetVersion(int* pMagorVersion, int* pMinorVersion);

//----------------------------------------------------------------------------------------------------
/** 
 * Function	    : fbusInitialize
 *
 * Parameters	  : Нет
 *
 * Return value : FBUS_RES_OK - в случае успеха
 *                FBUS_RES_INVALID_STATE - одновременный вызов из разных потоков исполнения
 *                FBUS_RES_NO_SYSTEM_RESOURCE - ошибка при выделении динамической памяти для 
 *                глобальных объектов сервиса
 *
 * Description  : Выполняет инициализацию гдобальных объектов сервиса FBUS API. Функция должна 
 *                вызываться до вызова любой другой функции FBUS API.
 **/
FDLL_EXPORT FBUS_RESULT fbusInitialize(void);

//----------------------------------------------------------------------------------------------------
/** 
 * Function	    : fbusDeInitialize
 *
 * Parameters	  : Нет
 *
 * Return value : FBUS_RES_OK - в случае успеха
 *                FBUS_RES_INVALID_STATE - одновременный вызов из разных потоков исполненния
 *                или остались сети незакрытые функцией fbusClose
 *
 * Description  : Освобождает выделенные глобальные ресурсы сервиса FBUS API.
 **/
FDLL_EXPORT FBUS_RESULT fbusDeInitialize(void);

//----------------------------------------------------------------------------------------------------
/** 
 * Function	    : fbusOpen
 *
 * Parameters	  : netNumber - номер сети FBUS
 *                pHandleNet - указатель на переменную в которой будет возвращен хэндл сети
 *
 * Return value : FBUS_RES_OK - в случае успеха
 *                FBUS_RES_INVALID_STATE - выполнение одновременно из разных потоков исполнения
 *                или при не выполненной (не удачно завершенной) инициализации сервиса FBUS API
 *                FBUS_RES_NO_SYSTEM_RESOURCE - ошибка при выделении динамической памяти для объектов сети
 *                FBUS_RES_OPEN_ADAPTER - ошибка открытия устройства адаптера сети
 *
 * Description  : Открывает и инициализирует объект сети FBUS.
 **/
FDLL_EXPORT FBUS_RESULT fbusOpen(size_t netNumber, FBUS_HANDLE* pHandleNet);

//----------------------------------------------------------------------------------------------------
/** 
 * Function	    : fbusClose
 *
 * Parameters	  : hNet - хэндл сети FBUS, полученный при успешном вызове fbusOpen
 *
 * Return value : FBUS_RES_OK - в случае успеха
 *                FBUS_RES_INVALID_STATE - выполнение одновременно из разных потоков исполнения
 *                FBUS_RES_INCORRECT_PARAM - неверное значение переданное в качестве хэндла сети
 *
 * Description  : Закрывает объект сети с указанным хэндлом и освобождает выделенные для него ресурсы.
 **/
FDLL_EXPORT FBUS_RESULT fbusClose(FBUS_HANDLE hNet);

//----------------------------------------------------------------------------------------------------
/** 
 * Function	    : fbusRescan
 *
 * Parameters	  : hNet - хэндл сети FBUS, полученный при успешном вызове fbusOpen
 *                pNodesDetected - указатель на переменную в которой будет возвращено число 
 *                обнаруженных в сети модулей ввода-выводы
 *
 * Return value : FBUS_RES_OK - в случае успеха
 *                FBUS_RES_INVALID_STATE - выполнение одновременно из разных потоков исполнения
 *                FBUS_RES_INCORRECT_PARAM - неверное значение переданное в качестве хэндла сети
 *
 * Description  : Закрывает объект сети с указанным хэндлом и освобождает выделенные для него ресурсы.
 **/
FDLL_EXPORT FBUS_RESULT fbusRescan(FBUS_HANDLE hNet, size_t* pNodesDetected);

FDLL_EXPORT FBUS_RESULT fbusGetNodesCount(FBUS_HANDLE hNet, size_t* pNodesDetected);

FDLL_EXPORT FBUS_RESULT fbusGetNodeDescription(FBUS_HANDLE hNet, uint8_t id, PFIO_MODULE_DESC pDest, size_t szDest);
FDLL_EXPORT FBUS_RESULT fbusGetNodeCommonParameters(FBUS_HANDLE hNet, uint8_t id, PFIO_MODULE_COMMON_CONF pDest, size_t szDest);
FDLL_EXPORT FBUS_RESULT fbusSetNodeCommonParameters(FBUS_HANDLE hNet, uint8_t id, PFIO_MODULE_COMMON_CONF pSrc, size_t szSrc);
FDLL_EXPORT FBUS_RESULT fbusGetNodeSpecificParameters(FBUS_HANDLE hNet, uint8_t id, void* pDest, size_t offset, size_t count);
FDLL_EXPORT FBUS_RESULT fbusSetNodeSpecificParameters(FBUS_HANDLE hNet, uint8_t id, void* pSrc, size_t offset, size_t count);

FDLL_EXPORT FBUS_RESULT fbusReadInputs(FBUS_HANDLE hNet, uint8_t id, void* pDest, size_t offset, size_t count);
FDLL_EXPORT FBUS_RESULT fbusWriteOutputs(FBUS_HANDLE hNet, uint8_t id, void* pSrc, size_t offset, size_t count);
FDLL_EXPORT FBUS_RESULT fbusReset(FBUS_HANDLE hNet, uint8_t id);
FDLL_EXPORT FBUS_RESULT fbusSendSync(FBUS_HANDLE hNet, uint8_t syncID);

FDLL_EXPORT FBUS_RESULT fbusDeleteGroup(FBUS_HANDLE hNet, uint8_t groupID);
FDLL_EXPORT FBUS_RESULT fbusDeleteAllGroups(FBUS_HANDLE hNet);
FDLL_EXPORT FBUS_RESULT fbusAssignNodeToGroup(FBUS_HANDLE hNet, uint8_t nodeID, uint8_t groupID,
                                              size_t inputOffset, size_t inputLength, 
                                              size_t outputOffset, size_t outputLength);
FDLL_EXPORT FBUS_RESULT fbusBuildGroups(FBUS_HANDLE hNet);

FDLL_EXPORT FBUS_RESULT fbusGroup_setNodeOutputs(FBUS_HANDLE hNet, uint8_t groupID, uint8_t nodeID,
                                                 size_t moduleDataOffset,
                                                 size_t moduleDataLength,
                                                 void* pSrc);
FDLL_EXPORT FBUS_RESULT fbusGroup_getNodeInputs(FBUS_HANDLE hNet, uint8_t groupID, uint8_t nodeID,
                                                size_t moduleDataOffset,
                                                size_t moduleDataLength,
                                                void* pDst);
FDLL_EXPORT FBUS_RESULT fbusProcessGroup(FBUS_HANDLE hNet, uint8_t groupID);

FDLL_EXPORT FBUS_RESULT fbusReadConfig(FBUS_HANDLE hNet, uint8_t id);
FDLL_EXPORT FBUS_RESULT fbusWriteConfig(FBUS_HANDLE hNet, uint8_t id);
FDLL_EXPORT FBUS_RESULT fbusSaveConfig(FBUS_HANDLE hNet, uint8_t id);
//====================================================================================================

#ifdef __cplusplus
}
#endif


#endif
