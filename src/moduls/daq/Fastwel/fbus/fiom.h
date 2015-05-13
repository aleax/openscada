/**
 *
 * NAME:    fiom.h
 *
 * DESCRIPTION: 
 * AUTHOR:  
 *
 *
 *******************************************************
 *      Copyright (C) 2000-2010 Fastwel Group.
 *      All rights reserved.
 ********************************************************/
#ifndef __FIOM_H__
#define __FIOM_H__

#ifndef AFFECT_FRAMEWORK
# include <fbus/stdlib/f_types.h>
# include <fbus/stdlib/f_pack.h>
#else
# ifdef PACKED_STRUCT
#   undef PACKED_STRUCT
# endif
# define PACKED_STRUCT(sstr) sstr
# include <oss/pack1.h>
#endif

//--- AIM720
typedef struct _AIM720 AIM720;
typedef struct _AIM720_INPUTS AIM720_INPUTS;
typedef struct _AIM720_CONFIGURATION AIM720_CONFIGURATION;

//--- Определение ряда значений параметра "Входной диапазон" в конфигурации модуля AIM720
#define AIM720_RANGE__0_5V	  0     // 0...5 В
#define AIM720_RANGE__0_10V	  1     // 0...10 В
#define AIM720_RANGE__M5_5V	  2     // -5...5 В
#define AIM720_RANGE__M10_10V	3     // -10...10 В

// Область конфигурационных параметров модуля AIM720
PACKED_STRUCT(
struct _AIM720_CONFIGURATION
{
  // Параметр: “Входной диапазон”
	uint8_t inputsRange;
  // Значение: 
  //  AIM720_RANGE__0_5V    (0):   0…5 В
  //  AIM720_RANGE__0_10V   (1):   0…10 В 
  //  AIM720_RANGE__M5_5V   (2):  –5…5 В 
  //  AIM720_RANGE__M10_10V (3):  –10…10 В

  // Параметр: “Период опроса”
  uint16_t scanPeriod;
  // Значение: от 0 до 65535; 
  // Период опроса = (scanPeriod * 256) мкс 
  //  1:      => (1 * 256)мкс = 256 мкс
  //  2:      => (2 * 256)мкс = 512 мкс
  //  ... 
  //  65535:  => (65535 * 256)мкс = 16,776960 c

  // Параметр: “Глубина фильтра”
  uint8_t filterDepth;
  // Значение:  от 0 до 10 
});

// Область входных параметров "Inputs" модуля AIM720
PACKED_STRUCT(
struct _AIM720_INPUTS
{
  // При равенстве 0 значения остальных каналов достоверны. 
  // Значение FFh свидетельствует об отсутствии связи с модулем
	uint8_t diagnostics;
  // Код АЦП на 1-м канале ввода напряжения. Действительны первые 12 разрядов. 
	uint16_t voltageInput0;
  // Код АЦП на 2-м канале ввода напряжения. Действительны первые 12 разрядов
	uint16_t voltageInput1;
  // Код АЦП на 3-м канале ввода напряжения. Действительны первые 12 разрядов.
	uint16_t voltageInput2;
	// Код АЦП на 1-м канале ввода тока. Действительны первые 12 разрядов. 
	uint16_t currentInput0;
	// Код АЦП на 2-м канале ввода тока. Действительны первые 12 разрядов.
	uint16_t currentInput1;
	// Код АЦП на 3-м канале ввода тока. Действительны первые 12 разрядов.
	uint16_t currentInput2;
	// Код АЦП на канале АЦП, который подключен к аналоговой «земле»
	uint16_t zeroReference;
	// Код АЦП, соответствующий опорному напряжению АЦП (около 2,5 В)
	uint16_t halfScaleReference;
});

PACKED_STRUCT(
struct _AIM720
{
	AIM720_INPUTS inputs;
	AIM720_CONFIGURATION configuration;
});

//--- Определение ряда значений параметра "Частота фильтра" в конфигурации
//    модулей: AIM721, AIM722, AIM723, AIM726, AIM727, AIM728, AIM729, AIM733
//    В каждом конкретном модуле поддерживается не полный набор частот!
//    Смотри описание области конфигурационных параметров на соответствующий модуль!
#define AIM72X_FILTER__50HZ	  0   // 50 Гц
#define AIM72X_FILTER__500HZ	1   // 500 Гц
#define AIM72X_FILTER__1000HZ	2   // 1000 Гц
#define AIM72X_FILTER__12_5HZ	3   // 12,5 Гц
#define AIM72X_FILTER__100HZ	4   // 100 Гц
#define AIM72X_FILTER__25HZ	  5   // 25 Гц

//--- AIM721
typedef struct _AIM721 AIM721;
typedef struct _AIM721_INPUTS AIM721_INPUTS;
typedef struct _AIM721_CONFIGURATION AIM721_CONFIGURATION;

// Область конфигурационных параметров модуля AIM721
PACKED_STRUCT(
struct _AIM721_CONFIGURATION
{
  // Параметр: “Частота фильтра канала 1”
  uint8_t filterOptions0;
  // Параметр: “Частота фильтра канала 2”
  uint8_t filterOptions1;
  // Параметр: “Частота фильтра канала 3”
  uint8_t filterOptions2;
  // Параметр: “Частота фильтра канала 4”
  uint8_t filterOptions3;
  // Допустимое значение параметров “Частота фильтра канала Х”: 
  //  AIM72X_FILTER__1000HZ (2):	1000 Гц
  //  AIM72X_FILTER__100HZ  (4):	100 Гц
  //  AIM72X_FILTER__50HZ   (0):	50 Гц
  //  AIM72X_FILTER__25HZ   (5):	25 Гц
});

// Область входных параметров "Inputs" модуля AIM721
PACKED_STRUCT(
struct _AIM721_INPUTS
{
  // Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем.
  // Значение 2 в первых двух битах означает, что уровень сигнала на 1-м канале измерения тока превысил 20,2 мА. 
  // Оставшиеся 3 пары битов содержат статус 2-го, 3-го и 4-го каналов соответственно.
  uint8_t diagnostics;

  // Канал: “input0”
  // Код АЦП на 1-м канале измерения тока.
  int32_t input0;
  // Канал: “input1”
  // Код АЦП на 2-м канале измерения тока
  int32_t input1;
  // Канал: “input2”
  // Код АЦП на 3-м канале измерения тока
  int32_t input2;
  // Канал: “input3”
  // Код АЦП на 4-м канале измерения тока
  int32_t input3;
});

PACKED_STRUCT(
struct _AIM721
{
	AIM721_INPUTS inputs;
	AIM721_CONFIGURATION configuration;
});

//--- AIM722
typedef struct _AIM722 AIM722;
typedef struct _AIM722_INPUTS AIM722_INPUTS;
typedef struct _AIM722_CONFIGURATION AIM722_CONFIGURATION;

// Область конфигурационных параметров модуля AIM722
PACKED_STRUCT(
struct _AIM722_CONFIGURATION
{
  // Параметр: “Частота фильтра канала 1”
  uint8_t filterOptions0;
  // Параметр: “Частота фильтра канала 2”
  uint8_t filterOptions1;
  // Допустимое значение параметров “Частота фильтра канала Х”: 
  //  AIM72X_FILTER__1000HZ (2):	1000 Гц
  //  AIM72X_FILTER__100HZ  (4):	100 Гц
  //  AIM72X_FILTER__50HZ   (0):	50 Гц
  //  AIM72X_FILTER__25HZ   (5):	25 Гц
});

// Область входных параметров "Inputs" модуля AIM722
PACKED_STRUCT(
struct _AIM722_INPUTS
{
  // Канал: “diagnostics”
  // Значение FFh: свидетельствует об отсутствии связи с модулем.
  // 2#0000_0001 : на 1-м канале напряжение на входе превысило 1 В
  // 2#0000_0010 : на 1-м канале ток превысил 20.2 мА 
  // 2#0000_0100 : на 2-м канале напряжение превысило 1 В
  // 2#0000_1000 : на 2-м канале ток превысил 20.2 мА
  // 2#0000_0000 : все в порядке
  uint8_t diagnostics;

  // Канал: “input0”
  // Код АЦП на 1-м канале измерения тока
  int32_t input0;

  // Канал: “input1”
  // Код АЦП на 2-м канале измерения тока
  int32_t input1;
});

PACKED_STRUCT(
struct _AIM722
{
	AIM722_INPUTS inputs;
	AIM722_CONFIGURATION configuration;
});

//--- AIM723
typedef struct _AIM723 AIM723;
typedef struct _AIM723_INPUTS AIM723_INPUTS;
typedef struct _AIM723_CONFIGURATION AIM723_CONFIGURATION;

// Область конфигурационных параметров модуля AIM723
PACKED_STRUCT(
struct _AIM723_CONFIGURATION
{
  // Параметр: “Частота фильтра канала 1”
  uint8_t filterOptions0;
  // Параметр: “Частота фильтра канала 2”
  uint8_t filterOptions1;
  // Параметр: “Частота фильтра канала 3”
  uint8_t filterOptions2;
  // Параметр: “Частота фильтра канала 4”
  uint8_t filterOptions3;
  // Допустимое значение параметров “Частота фильтра канала Х”: 
  //  AIM72X_FILTER__1000HZ (2):	1000 Гц
  //  AIM72X_FILTER__100HZ  (4):	100 Гц
  //  AIM72X_FILTER__50HZ   (0):	50 Гц
  //  AIM72X_FILTER__25HZ   (5):	25 Гц
});

// Область входных параметров Inputs модуля AIM722 описывается структурой:
PACKED_STRUCT(
struct _AIM723_INPUTS
{
  // Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем.
  // Значение 2 в первых двух битах означает, что уровень сигнала на 1-м канале измерения тока превысил 20,2 мА. 
  // Значение 1 в первых двух битах означает, что уровень сигнала на 1-м канале измерения тока не достигает 4,0 мА. 
  // Оставшиеся 3 пары битов содержат статус 2-го, 3-го и 4-го каналов соответственно.
  uint8_t diagnostics;

  // Канал: “input0”
  // Код АЦП на 1-м канале измерения тока. 
  int32_t input0;
  // Канал: “input1”
  // Код АЦП на 2-м канале измерения тока. 
  int32_t input1;
  // Канал: “input2”
  // Код АЦП на 3-м канале измерения тока. 
  int32_t input2;
  // Канал: “input3”
  // Код АЦП на 4-м канале измерения тока. 
  int32_t input3;
});

PACKED_STRUCT(
struct _AIM723
{
	AIM723_INPUTS inputs;
	AIM723_CONFIGURATION configuration;
});

//--- Определение ряда значений параметра "Период опроса" в конфигурации
//    модулей: AIM724, AIM725(01/02/03)
//    В каждом конкретном модуле поддерживается не полный набор определенных макроопределениями значений!
//    Смотри описание области конфигурационных параметров на соответствующий модуль!
#define AIM724_5_SCAN_RATE__RATE_200MS	0   // 200 мс
#define AIM724_5_SCAN_RATE__RATE_80MS	  1   // 80 мс

//--- AIM724
typedef struct _AIM724 AIM724;
typedef struct _AIM724_INPUTS AIM724_INPUTS;
typedef struct _AIM724_OUTPUTS AIM724_OUTPUTS;
typedef struct _AIM724_CONFIGURATION AIM724_CONFIGURATION;

//--- Определение ряда значений параметра "Компенсация холодного спая" 
//    в конфигурации модуля AIM724
#define AIM724_5_CJC_MODE__CJC_DISABLED	        0x02    // Не используется
#define AIM724_5_CJC_MODE__CJC_INTERNAL_SENSOR	0x03    // Встроенный термометр
#define AIM724_5_CJC_MODE__CJC_EXTERNAL_SOURCE	0x01    // Внешний термометр

//--- Определение ряда значений параметра "Входной диапазон" в конфигурации модуля AIM724
#define AIM724_RANGE__TC_J	        0 // TC типа J
#define AIM724_RANGE__TC_K	        1 // TC типа K
#define AIM724_RANGE__TC_N	        2 // TC типа N
#define AIM724_RANGE__TC_T	        3 // TC типа T
#define AIM724_RANGE__TC_E	        4 // TC типа E
#define AIM724_RANGE__TC_R	        5 // TC типа R
#define AIM724_RANGE__TC_S	        6 // TC типа S
#define AIM724_RANGE__TC_B	        7 // TC типа B
#define AIM724_RANGE__MV_DIFF20	    8 // -20...+20 мВ
#define AIM724_RANGE__MV_DIFF50	    9 // -50...+50 мВ
#define AIM724_RANGE__MV_DIFF100	 10 // -100...+100 мВ    
#define AIM724_RANGE__MV_DIFF200	 11 // -200...+200 мВ
#define AIM724_RANGE__TC_L	       17 // TC типа L

// Область конфигурационных параметров модуля AIM724
PACKED_STRUCT(
struct _AIM724_CONFIGURATION
{
  // Параметр: “Период опроса”
  // Значение: Зарезервировано, должно быть равно 0! Период опроса всегда равен 200 мс!
  uint8_t scanRate;

  // Параметр: “Компенсация холодного спая”
  uint8_t cjc_mode;
  // Значение:
  //  AIM724_5_CJC_MODE__CJC_DISABLED        (2): Не используется
  //  AIM724_5_CJC_MODE__CJC_INTERNAL_SENSOR (3): Встроенный термометр
  //  AIM724_5_CJC_MODE__CJC_EXTERNAL_SOURCE (1): Внешний термометр

  // Параметр: “Входной диапазон”
  uint8_t inputRange;
  // Допустимые значения: 
  //  AIM724_RANGE__TC_J        (0): TC type J
  //  AIM724_RANGE__TC_K        (1): TC type K
  //  AIM724_RANGE__TC_N        (2): TC type N
  //  AIM724_RANGE__TC_T        (3): TC type T
  //  AIM724_RANGE__TC_E        (4): TC type E
  //  AIM724_RANGE__TC_R        (5): TC type R
  //  AIM724_RANGE__TC_S        (6): TC type S
  //  AIM724_RANGE__TC_B        (7): TC type B
  //  AIM724_RANGE__MV_DIFF20   (8): -20...+20 mV
  //  AIM724_RANGE__MV_DIFF50   (9): -50...+50 mV
  //  AIM724_RANGE__MV_DIFF100  (10): -100...+100 mV
  //  AIM724_RANGE__MV_DIFF200  (11): -200...+200 mV
  //  AIM724_RANGE__TC_L        (17): TC type L
});

// Область входных параметров "Inputs" модуля AIM724
PACKED_STRUCT(
struct _AIM724_INPUTS
{
  // Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем.
  // Бит0: =1 -- показания на первом канале недостоверны; =0 -- показания на первом канале достоверны
  // Бит1: =1 -- показания на втором канале недостоверны; =0 -- показания на втором канале достоверны
  // Бит2: =1 -- неисправность измерительного тракта; =0 -- измерительный тракт в норме
  // Бит3: =0 -- если есть связь с модулем; =1 -- если нет связи с модулем
  // Бит4-5: =00 -- источник сигнала подключен к первому каналу; =01 -- обрыв цепи источника сигнала первого канала
  // Бит6-7: =00 -- источник сигнала подключен ко второму каналу; =01 -- обрыв цепи источника сигнала второго канала
  uint8_t diagnostics;

  // Канал: “channel0”
  // Значение в выбранном диапазоне на первом канале в единицах физической величины
  float channel0;
  // Канал: “channel1”
  // Значение в выбранном диапазоне на втором канале в единицах физической величины
  float channel1;
  // Канал: “cjcInput”
  // Значение температуры холодного спая, измеренное встроенным датчиком
  float cjcInput;
});

// Область выходных параметров "Outputs" модуля AIM724
PACKED_STRUCT(
struct _AIM724_OUTPUTS
{
  // Канал: “cjcExternalSource”
  // Значение температуры холодного спая, передаваемое в модуль приложением. 
  // Будет использоваться модулем, только если выбран режим «внешний термометр».
  float cjcExternalSource;
});

PACKED_STRUCT(
struct _AIM724
{
	AIM724_INPUTS inputs;
	AIM724_OUTPUTS outputs;
	AIM724_CONFIGURATION configuration;
});

//--- AIM725 Исполнение: AIM72501, AIM72502, AIM72503
typedef struct _AIM725 AIM725;
typedef struct _AIM725_INPUTS AIM725_INPUTS;
typedef struct _AIM725_CONFIGURATION AIM725_CONFIGURATION;

//--- Определение ряда значений параметра "Выбор каналов" в конфигурации модуля AIM725
#define AIM724_5_USE_BOTH_CHANNELS	  0   // Использовать оба канала
#define AIM724_5_USE_FIRST_CHANNELS	  1   // Использовать первый канал
#define AIM724_5_USE_SECOND_CHANNELS	2   // Использовать второй канал

//--- Определение ряда значений параметра "Входной диапазон" в конфигурации модуля AIM725
//    Конкретное исполнение использует свой набор из определенных макроопределениями значений!
//    Смотри описание области конфигурационных параметров модуля!
#define AIM725_RANGE___PT50	      13  // Pt50
#define AIM725_RANGE___PT100	    0   // Pt100
#define AIM725_RANGE__PT200	      1   // Pt200
#define AIM725_RANGE__PT500	      2   // Pt500
#define AIM725_RANGE__PT1000	    3   // Pt1000
#define AIM725_RANGE__NI100	      4   // Ni100
#define AIM725_RANGE__NI120	      5   // Ni120
#define AIM725_RANGE__CU10	      6   // Cu10
#define AIM725_RANGE__CU50	      7   // Cu50
#define AIM725_RANGE__OHM_0_150	  8   // 0-150 Ом
#define AIM725_RANGE__OHM_0_300	  9   // 0-300 Ом
#define AIM725_RANGE__OHM_0_600	  10  // 0-600 Ом
#define AIM725_RANGE__OHM_0_1500	11  // 0-1500 Ом
#define AIM725_RANGE__OHM_0_3000	12  // 0-3000 Ом
#define AIM725_RANGE__CU100	      14  // Cu100

//--- Определение ряда значений параметра "Тип соединения" в конфигурации модуля AIM725
#define AIM725_CONNECTION__TWOWIRE	  0   // 2х–проводное подключение; 
#define AIM725_CONNECTION__THREEWIRE	1   // 3х–проводное подключение; 

// Область конфигурационных параметров модуля AIM725: AIM72501, AIM72502, AIM72503
PACKED_STRUCT(
struct _AIM725_CONFIGURATION
{
  // Параметр: “Период опроса”
  // Значение: Зарезервировано, должно быть равно 0! Период опроса каждаго канала равен 80 мс!
  uint8_t scanRate;

  // Параметр: “Выбор каналов”
  uint8_t channels_selection;
  // Значение: 
  //  AIM724_5_USE_BOTH_CHANNELS   (0): Использовать оба канала
  //  AIM724_5_USE_FIRST_CHANNELS  (1): Использовать первый канал
  //  AIM724_5_USE_SECOND_CHANNELS (2): Использовать второй канал

  // Параметр: “Входной диапазон”
  uint8_t inputRange;
  // Значение: AIM72501, AIM72502
  //  AIM725_RANGE___PT50      (13): PT50
  //  AIM725_RANGE___PT100      (0): PT100
  //  AIM725_RANGE__PT200       (1): PT200
  //  AIM725_RANGE__PT500       (2): PT500
  //  AIM725_RANGE__PT1000      (3): PT1000
  //  AIM725_RANGE__NI100       (4): NI100
  //  AIM725_RANGE__NI120       (5): NI120
  //  AIM725_RANGE__CU50        (7): CU50
  //  AIM725_RANGE__CU100      (14): CU100
  //  AIM725_RANGE__OHM_0_150   (8): 0-150 Ohm
  //  AIM725_RANGE__OHM_0_300   (9): 0-300 Ohm
  //  AIM725_RANGE__OHM_0_600  (10): 0-600 Ohm
  //  AIM725_RANGE__OHM_0_1500 (11): 0-1500 Ohm
  //  AIM725_RANGE__OHM_0_3000 (12): 0-3000 Ohm

  // Значение: AIM72503
  //  AIM725_RANGE___PT50      (13): ТСП 50П
  //  AIM725_RANGE___PT100      (0): ТСП 100П
  //  AIM725_RANGE__CU50        (7): ТСМ 50М
  //  AIM725_RANGE__CU100      (14): ТСМ 100М

  // Параметр: “Тип соединения”
  uint8_t connectionType;
  // Значение: 
  //  AIM725_CONNECTION__TWOWIRE   (0): 2х–проводное подключение
  //  AIM725_CONNECTION__THREEWIRE (1): 3х–проводное подключение
});

// Область входных параметров Inputs модуля AIM725 описывается структурой:
PACKED_STRUCT(
struct _AIM725_INPUTS
{
  // Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем.
  // Бит0: =1 -- показания на первом канале недостоверны; =0 -- показания на первом канале достоверны
  // Бит1: =1 -- показания на втором канале недостоверны; =0 -- показания на втором канале достоверны
  // Бит2: =1 -- неисправность измерительного тракта; =0 -- измерительный тракт в норме
  // Бит3: =0 -- если есть связь с модулем; =1 -- если нет связи с модулем
  // Бит4-5: =00 -- источник сигнала подключен к первому каналу
  //         =01 -- обрыв цепи источника сигнала первого канала
  //         =10 -- короткое замыкание в цепи источника сигнала первого канала
  //         =11 -- если нет связи с модулем
  // Бит6-7: =00 -- источник сигнала подключен ко второму каналу 
  //         =01 -- обрыв цепи источника сигнала второго канала
  //         =10 -- короткое замыкание в цепи источника сигнала второго канала
  //         =11 -- если нет связи с модулем
  uint8_t diagnostics;

  // Канал: “channel0”
  // При подключении термометра сопротивления – значение температуры в градусах Цельсия, 
  // при измерении сопротивления – сопротивление в Om на первом канале 
  float channel0;
  // Канал: “channel1”
  // При подключении термометра сопротивления – значение температуры в градусах Цельсия, 
  // при измерении сопротивления – сопротивление в Om на втором канале 
  float channel1;
});

// Структура представления модуля AIM725: AIM72501, AIM72502, AIM72503
PACKED_STRUCT(
struct _AIM725
{
	AIM725_INPUTS inputs;
	AIM725_CONFIGURATION configuration;
});

//--- AIM726
typedef struct _AIM726 AIM726;
typedef struct _AIM726_INPUTS AIM726_INPUTS;
typedef struct _AIM726_CONFIGURATION AIM726_CONFIGURATION;

// Область конфигурационных параметров модуля AIM726
PACKED_STRUCT(
struct _AIM726_CONFIGURATION
{
  // Параметр: “Частота фильтра канала 1”
  uint8_t filterOptions0;
  // Параметр: “Частота фильтра канала 2”
  uint8_t filterOptions1;
  // Допустимое значение параметров: 
  //  AIM72X_FILTER__50HZ   (0):	50 Гц
  //  AIM72X_FILTER__500НZ  (1):	500 Гц
  //  AIM72X_FILTER__1000HZ (2):	1000 Гц
});

// Область входных параметров "Inputs" модуля AIM726
PACKED_STRUCT(
struct _AIM726_INPUTS
{
  // Канал: “diagnostics”
  // При равенстве 0 значения на каналах достоверны. 
  // Значение FFh свидетельствует об отсутствии связи с модулем
  uint8_t diagnostics;
  // Канал: “input0”
  // Код АЦП на 1-м канале ввода напряжения. 
  uint32_t input0;
  // Канал: “input1”
  // Код АЦП на 2-м канале ввода напряжения. 
  uint32_t input1;
});

PACKED_STRUCT(
struct _AIM726
{
	AIM726_INPUTS inputs;
	AIM726_CONFIGURATION configuration;
});

//--- AIM727
typedef struct _AIM727 AIM727;
typedef struct _AIM727_INPUTS AIM727_INPUTS;
typedef struct _AIM727_CONFIGURATION AIM727_CONFIGURATION;

// Область конфигурационных параметров модуля AIM727
PACKED_STRUCT(
struct _AIM727_CONFIGURATION
{
  // Параметр: “Частота фильтра канала 1”
  uint8_t filterOptions0;
  // Параметр: “Частота фильтра канала 2”
  uint8_t filterOptions1;
  // Параметр: “Частота фильтра канала 3”
  uint8_t filterOptions2;
  // Параметр: “Частота фильтра канала 4”
  uint8_t filterOptions3;
  // Допустимое значение параметров: 
  //  AIM72X_FILTER__50HZ   (0):	50 Гц
  //  AIM72X_FILTER__500НZ  (1):	500 Гц
  //  AIM72X_FILTER__1000HZ (2):	1000 Гц
});

// Область входных параметров "Inputs" модуля AIM727
PACKED_STRUCT(
struct _AIM727_INPUTS
{
  // Канал: “diagnostics”
  // При равенстве 0 значения на каналах достоверны. 
  // Значение FFh свидетельствует об отсутствии связи с модулем
  uint8_t diagnostics;
  // Канал: “input0”
  // Код АЦП на 1-м канале ввода напряжения 
  uint32_t input0;
  // Канал: “input1”
  // Код АЦП на 2-м канале ввода напряжения 
  uint32_t input1;
  // Канал: “input2”
  // Код АЦП на 3-м канале ввода напряжения 
  uint32_t input2;
  // Канал: “input3”
  // Код АЦП на 4-м канале ввода напряжения 
  uint32_t input3;
});

PACKED_STRUCT(
struct _AIM727
{
	AIM727_INPUTS inputs;
	AIM727_CONFIGURATION configuration;
});

//--- AIM728
typedef struct _AIM728 AIM728;
typedef struct _AIM728_INPUTS AIM728_INPUTS;
typedef struct _AIM728_CONFIGURATION AIM728_CONFIGURATION;

// Область конфигурационных параметров модуля AIM728
PACKED_STRUCT(
struct _AIM728_CONFIGURATION
{
  // Параметр: “Частота фильтра канала 1”
  uint8_t filterOptions0;
  // Параметр: “Частота фильтра канала 2”
  uint8_t filterOptions1;
  // Параметр: “Частота фильтра канала 3”
  uint8_t filterOptions2;
  // Параметр: “Частота фильтра канала 4”
  uint8_t filterOptions3;
  // Допустимое значение параметров: 
  //  AIM72X_FILTER__50HZ   (0):	50 Гц
  //  AIM72X_FILTER__500НZ  (1):	500 Гц
  //  AIM72X_FILTER__1000HZ (2):	1000 Гц
});

// Область входных параметров "Inputs" модуля AIM728
PACKED_STRUCT(
struct _AIM728_INPUTS
{
  // Канал: “diagnostics”
  // При равенстве 0 значения на каналах достоверны. 
  // Значение FFh свидетельствует об отсутствии связи с модулем
  uint8_t diagnostics;
  // Канал: “input0”
  // Код АЦП на 1-м канале ввода напряжения 
  int32_t input0;
  // Канал: “input1”
  // Код АЦП на 2-м канале ввода напряжения 
  int32_t input1;
  // Канал: “input2”
  // Код АЦП на 3-м канале ввода напряжения 
  int32_t input2;
  // Канал: “input3”
  // Код АЦП на 4-м канале ввода напряжения 
  int32_t input3;
});

// Преобразование значения кода АЦП на канале аналогового ввода модуля AIM728 в напряжение
#define convert_aim728_channel(adc_code) (((int32_t)(adc_code)) * 2.3841861E-6)

PACKED_STRUCT(
struct _AIM728
{
	AIM728_INPUTS inputs;
	AIM728_CONFIGURATION configuration;
});

//--- AIM729
typedef struct _AIM729 AIM729;
typedef struct _AIM729_INPUTS AIM729_INPUTS;
typedef struct _AIM729_CONFIGURATION AIM729_CONFIGURATION;

// Область конфигурационных параметров модуля AIM729
PACKED_STRUCT(
struct _AIM729_CONFIGURATION
{
  // Параметр: “Частота фильтра канала 1”
  uint8_t filterOptions0;
  // Параметр: “Частота фильтра канала 2”
  uint8_t filterOptions1;
  // Допустимое значение параметров: 
  //  AIM72X_FILTER__50HZ   (0):	50 Гц
  //  AIM72X_FILTER__500НZ  (1):	500 Гц
  //  AIM72X_FILTER__1000HZ (2):	1000 Гц
});

// Область входных параметров "Inputs" модуля AIM729
PACKED_STRUCT(
struct _AIM729_INPUTS
{
  // Канал: “diagnostics”
  // При равенстве 0 значения на каналах достоверны. 
  // Значение FFh свидетельствует об отсутствии связи с модулем
  uint8_t diagnostics;
  // Канал: “input0”
  // Код АЦП на 1-м канале ввода напряжения
  int32_t input0;
  // Канал: “input1”
  // Код АЦП на 2-м канале ввода напряжения
  int32_t input1;
});

PACKED_STRUCT(
struct _AIM729
{
	AIM729_INPUTS inputs;
	AIM729_CONFIGURATION configuration;
});

//--- AIM730
typedef struct _AIM730 AIM730;
typedef struct _AIM730_INPUTS AIM730_INPUTS;
typedef struct _AIM730_OUTPUTS AIM730_OUTPUTS;
typedef struct _AIM730_CONFIGURATION AIM730_CONFIGURATION;

//--- Определение ряда значений параметра "Выходной диапазон канала" в конфигурации модуля AIM730
#define AIM730_OUTPUT_RANGE___0_20MA	0   // 0...20 мА
#define AIM730_OUTPUT_RANGE___4_20MA	1   // 4...20 мА

//--- Определение ряда значений параметра "Скорость нарастания канала" в конфигурации модуля AIM730
#define AIM730_SLEW_RATE___DAC_I_SR_DISABLED	        0 // Без ограничения (Максимум)
//                                                      диапазон 0...20 мА    /   4...20 мА 
#define AIM730_SLEW_RATE___DAC_I_SR_0_305__0_244	    1   //     0.3 мА/с     /   0.2 мА/с
#define AIM730_SLEW_RATE___DAC_I_SR_0_915__0_732	    2   //     0.9 мА/с     /   0.7 мА/с
#define AIM730_SLEW_RATE___DAC_I_SR_2_135__1_708	    3   //      2.1 мА/с    /   1.7 мА/с
#define AIM730_SLEW_RATE___DAC_I_SR_4_575__3_66	      4   //      4.5 мА/с    /   3.6 мА/с
#define AIM730_SLEW_RATE___DAC_I_SR_9_455__7_564	    5   //      9.5 мА/с    /   7.6 мА/с
#define AIM730_SLEW_RATE___DAC_I_SR_19_2__15_4	      6   //      19.2 мА/с   /   15.3 мА/с
#define AIM730_SLEW_RATE___DAC_I_SR_38_7__30_9	      7   //      38.7 мА/с   /   30.9 мА/с
#define AIM730_SLEW_RATE___DAC_I_SR_77_8__62_2	      8   //      77.8 мА/с   /   66.2 мА/с
#define AIM730_SLEW_RATE___DAC_I_SR_155_8__124_7	    9   //      155.8 мА/с  /   124.7 мА/с
#define AIM730_SLEW_RATE___DAC_I_SR_312_0__249_6	    10  //      312.0 мА/с  /   249.6 мА/с
#define AIM730_SLEW_RATE___DAC_I_SR_624_3__499_5	    11  //      624.3 мА/с  /   499.5 мА/с
#define AIM730_SLEW_RATE___DAC_I_SR_1248_9__999_2	    12  //      1.2 А/с     /   1.0 А/с
#define AIM730_SLEW_RATE___DAC_I_SR_2498_5__1998_8	  13  //      2.5 А/с     /   2.0 А/с
#define AIM730_SLEW_RATE___DAC_I_SR_4996_8__3997_4	  14  //      5.0 А/с     /   4.0 А/с
#define AIM730_SLEW_RATE___DAC_I_SR_9993_9__7995_1	  15  //      10.0 А/с    /   8.0 А/с
#define AIM730_SLEW_RATE___DAC_I_SR_19988_1__15990_5	16  //      20.0 А/с    /   16.0 А/с

// Область конфигурационных параметров модуля AIM730
PACKED_STRUCT(
struct _AIM730_CONFIGURATION
{
  // Параметр: “Начальное значение канала 1”
  uint16_t outputInitialValue0;
  // Параметр: “Начальное значение канала 2”
	uint16_t outputInitialValue1;
  // Параметр: “Безопасное значение канала 1”
	uint16_t outputSafeValue0;
  // Параметр: “Безопасное значение канала 2”
	uint16_t outputSafeValue1;
  // Код для задания параметров “Начальное значение канала 1(2)”, “Безопасное значение канала 1(2)” 
  // вычисляется по значению тока I (мА) следующим образом:
  // диапазон 0...20 мА	Код = I / 0,00030517;
  // диапазон 4...20 мА	Код = (I – 4,0) / 0,00024414;

  // Зарезервировано. Не используется в текущей версии прошивки модуля.
	uint8_t temperatureZonesSupport;

  // Параметр: “Выходной диапазон канала 1”
  uint8_t outputRange0;
  // Параметр: “Выходной диапазон канала 2”
	uint8_t outputRange1;
  // Значение параметров “Выходной диапазон канала 1(2)”: 
  //  AIM730_OUTPUT_RANGE___0_20MA  (0): 0...20 мА
  //  AIM730_OUTPUT_RANGE___4_20MA  (1): 4...20 мА

  // Параметр: “Скорость нарастания канала 1”
  uint8_t slewRateLimit0;
  // Параметр: “Скорость нарастания канала 2”
	uint8_t slewRateLimit1;
  // Значение параметров “Скорость нарастания канала 1(2)”: 
  // AIM730_SLEW_RATE___DAC_I_SR_DISABLED	          (0): Без ограничения (Максимум)
  //                                     диапазон       0...20 мА    /   4...20 мА 
  // AIM730_SLEW_RATE___DAC_I_SR_0_305__0_244       (1):   0.3 мА/с  /      0.2 мА/с  
  // AIM730_SLEW_RATE___DAC_I_SR_0_915__0_732       (2):   0.9 мА/с  /      0.7 мА/с
  // AIM730_SLEW_RATE___DAC_I_SR_2_135__1_708       (3):   2.1 мА/с  /      1.7 мА/с
  // AIM730_SLEW_RATE___DAC_I_SR_4_575__3_66        (4):   4.5 мА/с  /      3.6 мА/с
  // AIM730_SLEW_RATE___DAC_I_SR_9_455__7_564       (5):   9.5 мА/с  /      7.6 мА/с
  // AIM730_SLEW_RATE___DAC_I_SR_19_2__15_4	        (6):  19.2 мА/с  /     15.3 мА/с 
  // AIM730_SLEW_RATE___DAC_I_SR_38_7__30_9	        (7):  38.7 мА/с  /     30.9 мА/с
  // AIM730_SLEW_RATE___DAC_I_SR_77_8__62_2	        (8):  77.8 мА/с  /     66.2 мА/с
  // AIM730_SLEW_RATE___DAC_I_SR_155_8__124_7       (9): 155.8 мА/с  /    124.7 мА/с
  // AIM730_SLEW_RATE___DAC_I_SR_312_0__249_6       (10): 312.0 мА/с /    249.6 мА/с
  // AIM730_SLEW_RATE___DAC_I_SR_624_3__499_5       (11): 624.3 мА/с /    499.5 мА/с
  // AIM730_SLEW_RATE___DAC_I_SR_1248_9__999_2      (12):   1.2 А/с  /      1.0 А/с
  // AIM730_SLEW_RATE___DAC_I_SR_2498_5__1998_8     (13):   2.5 А/с  /      2.0 А/с
  // AIM730_SLEW_RATE___DAC_I_SR_4996_8__3997_4     (14):   5.0 А/с  /      4.0 А/с
  // AIM730_SLEW_RATE___DAC_I_SR_9993_9__7995_1     (15):  10.0 А/с  /      8.0 А/с
  // AIM730_SLEW_RATE___DAC_I_SR_19988_1__15990_5   (16):  20.0 А/с  /     16.0 А/с
});

// Область входных параметров "Inputs" модуля AIM730
PACKED_STRUCT(
struct _AIM730_INPUTS
{
  // Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем по внутренней шине. 
  // Отдельные биты имеют следующее назначение:
  // Бит0: =1 : перегрузка на первом канале. Признак сбрасывается после каждой записи в соответствующий канал
  // Бит1: =1 : перегрузка на втором канале. Признак сбрасывается после каждой записи в соответствующий канал
  // Бит2: =0 : используется диапазон 0...20 мА для 1-го канала; =1 : используется диапазон 4...20 мА для 1-го канала
  // Бит3: =0 : используется диапазон 0...20 мА для 2-го канала; =1 : используется диапазон 4...20 мА для 2-го канала
	uint8_t diagnostics;

  // Канал: “outputValue0”
  // Текущее значение на первом канале. 
	uint16_t outputValue0;
  // Канал: “outputValue1”
  // Текущее значение на втором канале
	uint16_t outputValue1;
  // Значение тока I (мА) по коду вычисляется следующим образом:
  //  I = outputValueХ * 0,00030517;          для диапазона 0...20 мА
  //  I = 4,0 + outputValueХ * 0, 00024414;   для диапазона 4...20 мА
});

// Область выходных параметров "Outputs" модуля AIM730
PACKED_STRUCT(
struct _AIM730_OUTPUTS
{
  // Канал: “output0”
  // Устанавливаемое значение на выходе первого канала
	uint16_t output0;
  // Канал: “output1”
  //Устанавливаемое значение на выходе второго канала
	uint16_t output1;
  // Код outputХ, записываемый в канал по значению тока I (мА) вычисляется следующим образом:
  //  диапазон 0...20 мА	outputХ = I / 0,00030517;
  //  диапазон 4...20 мА	outputХ = (I – 4,0) / 0,00024414;
});

PACKED_STRUCT(
struct _AIM730
{
	AIM730_INPUTS inputs;
	AIM730_OUTPUTS outputs;
	AIM730_CONFIGURATION configuration;
});

//--- AIM731
typedef struct _AIM731 AIM731;
typedef struct _AIM731_INPUTS AIM731_INPUTS;
typedef struct _AIM731_OUTPUTS AIM731_OUTPUTS;
typedef struct _AIM731_CONFIGURATION AIM731_CONFIGURATION;

//--- Определение ряда значений параметра "Выходной диапазон канала" в конфигурации модуля AIM731
#define AIM731_OUTPUT_RANGE___0_10V	    0   // 0...10 В
#define AIM731_OUTPUT_RANGE___DIFF10V	  1   // -10...+10 В

//--- Определение ряда значений параметра "Скорость нарастания канала" в конфигурации модуля AIM731
#define AIM731_SLEW_RATE___DAC_U_SR_DISABLED	  0   // Без ограничения (Максимум)
#define AIM731_SLEW_RATE___DAC_U_SR_0_15	      1   // 0.15 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_0_50	      2   // 0.5 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_1_00	      3   // 1.0 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_2_30	      4   // 2.3 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_4_74	      5   // 4.7 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_9_64	      6   // 9.6 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_19_43	      7   // 19.4 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_39_00	      8   // 39 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_78_18	      9   // 78 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_156_52	    10  // 156 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_313_19	    11  // 313 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_626_54	    12  // 626 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_1_25_V_MS	  13  // 1250 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_2_50_V_MS	  14  // 2500 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_5_00_V_MS	  15  // 5000 В/с
#define AIM731_SLEW_RATE___DAC_U_SR_10_00_V_MS	16  // 10000 В/с

// Область конфигурационных параметров модуля AIM730
PACKED_STRUCT(
struct _AIM731_CONFIGURATION
{
  // Параметр: “Начальное значение канала 1”
	uint16_t outputInitialValue0;
  // Параметр: “Начальное значение канала 2”
	uint16_t outputInitialValue1;
  // Параметр: “Безопасное значение канала 1”
	uint16_t outputSafeValue0;
  // Параметр: “Безопасное значение канала 2”
	uint16_t outputSafeValue1;
  // Код для задания параметров “Начальное значение канала 1(2)”, “Безопасное значение канала 1(2)” 
  // вычисляется по значению напряжения U (В) следующим образом:
  // диапазон 0...10 В	   Код = U / 0,00015259
  // диапазон –10...+10 В	 Код = (U + 10,0)/0,00030518

  // Зарезервировано. Не используется в текущей версии прошивки модуля.
  uint8_t temperatureZonesSupport;

  // Параметр: “Выходной диапазон канала 1”
  uint8_t outputRange0;
  // Параметр: “Выходной диапазон канала 2”
	uint8_t outputRange1;
  // Значение параметров “Выходной диапазон канала 1(2)”: 
  // AIM731_OUTPUT_RANGE___0_10V   (0):   0...+10 В
  // AIM731_OUTPUT_RANGE___DIFF10V (1): -10...+10 В

  // Параметр: “Скорость нарастания канала 1”
	uint8_t slewRateLimit0;
  // Параметр: “Скорость нарастания канала 2”
	uint8_t slewRateLimit1;
  // Значение параметров “Скорость нарастания канала 1(2)”: 
  // AIM731_SLEW_RATE___DAC_U_SR_DISABLED	   (0): Без ограничения (Максимум)
  // AIM731_SLEW_RATE___DAC_U_SR_0_15	       (1): 0.15 В/с  
  // AIM731_SLEW_RATE___DAC_U_SR_0_50	       (2): 0.5 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_1_00	       (3): 1.0 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_2_30	       (4): 2.3 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_4_74	       (5): 4.7 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_9_64        (6): 9.6 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_19_43       (7): 19.4 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_39_00       (8): 39 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_78_18       (9): 78 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_156_52	    (10): 156 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_313_19	    (11): 313 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_626_54     (12): 626 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_1_25_V_MS	(13): 1250 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_2_50_V_MS	(14): 2500 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_5_00_V_MS	(15): 5000 В/с
  // AIM731_SLEW_RATE___DAC_U_SR_10_00_V_MS	(16): 10000 В/с
});

// Область входных параметров "Inputs" модуля AIM731
PACKED_STRUCT(
struct _AIM731_INPUTS
{
  // Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем по внутренней шине. 
  // Отдельные биты имеют следующее назначение:
  // Бит0: =1 : перегрузка на первом канале. Признак сбрасывается после каждой записи в соответствующий канал
  // Бит1: =1 : перегрузка на втором канале. Признак сбрасывается после каждой записи в соответствующий канал
  // Бит2: =0 : используется диапазон 0...10 В для 1-го канала; =1 : используется диапазон –10...+10 В для 1-го канала
  // Бит3: =0 : используется диапазон 0...10 В для 2-го канала; =1 : используется диапазон –10...+10 В для 2-го канала
	uint8_t diagnostics;

  // Канал: “outputValue0”
  // Текущее значение на первом канале
	uint16_t outputValue0;
  // Канал: “outputValue0”
  // Текущее значение на втором канале
	uint16_t outputValue1;
  // Значение напряжения U (В) по коду outputValueХ вычисляется следующим образом:
  //  U = outputValueХ * 0,00015259;          для диапазона 0...10 В
  //  U = outputValueХ * 0,00030518 – 10,0;   для диапазона –10...+10 В
});

// Область выходных параметров "Outputs" модуля AIM731
PACKED_STRUCT(
struct _AIM731_OUTPUTS
{
  // Канал: “output0”
  // Устанавливаемое значение на выходе первого канала
	uint16_t output0;
  // Канал: “output1”
  // Устанавливаемое значение на выходе второго канала
	uint16_t output1;
  // Код outputХ, записываемый в канал по значению напряжения U (В) вычисляется следующим образом:
  //  диапазон 0...10 В	    outputХ = U / 0,00015259;
  //  диапазон –10...+10 В	outputХ = (U + 10,0)/0,00030518;
});

PACKED_STRUCT(
struct _AIM731
{
	AIM731_INPUTS inputs;
	AIM731_OUTPUTS outputs;
	AIM731_CONFIGURATION configuration;
});

//--- AIM733
typedef struct _AIM733 AIM733;
typedef struct _AIM733_INPUTS AIM733_INPUTS;
typedef struct _AIM733_CONFIGURATION AIM733_CONFIGURATION;

//--- Определение ряда значений параметра "Диапазон канала" в конфигурации модуля AIM733
#define AIM733_INPUT_RANGE_RANGE733_0_5V	  0   // 0...5 В
#define AIM733_INPUT_RANGE_RANGE733_0_2D5V	1   // 0,0...2,5 В

// Область конфигурационных параметров модуля AIM733
PACKED_STRUCT(
struct _AIM733_CONFIGURATION
{
  // Параметр: “Частота фильтра канала 1”
  uint8_t filterOptions0;
  // Параметр: “Частота фильтра канала 2”
  uint8_t filterOptions1;
  // Параметр: “Частота фильтра канала 3”
  uint8_t filterOptions2;
  // Параметр: “Частота фильтра канала 4”
  uint8_t filterOptions3;
  // Значение параметров "Частота фильтра канала 1/2/3/4": 
  // AIM72X_FILTER__12_5HZ (3):	12,5 Гц
  // AIM72X_FILTER__50HZ   (0):	50 Гц
  // AIM72X_FILTER__500HZ  (1):	500 Гц
  // AIM72X_FILTER__1000HZ (2):	1000 Гц

  // Параметр: “Диапазон канала 1”
	uint8_t range0;
  // Параметр: “Диапазон канала 2”
	uint8_t range1;
  // Параметр: “Диапазон канала 3”
	uint8_t range2;
  // Параметр: “Диапазон канала 4”
	uint8_t range3;
  // Значение параметров "Диапазон канала 1/2/3/4": 
  // AIM733_INPUT_RANGE_RANGE733_0_5V   (0):	0...5 В
  // AIM733_INPUT_RANGE_RANGE733_0_2D5V (1):	0,0...2,5 В
});

// Область входных параметров "Inputs" модуля AIM733
PACKED_STRUCT(
struct _AIM733_INPUTS
{
  // Канал: “diagnostics”
  // Значение 16#FF свидетельствует об отсутствии связи с модулем
  // При равенстве 0 можно анализировать и использовать данные на каналах input0..3
  // Если значение не равно 0 и не равно 16#FF, то первые два бита имеют следующее назначение:
  // Бит 0: =1: АЦП функционирует неправильно
  // Бит 1: =1: Потеряна связь с АЦП
	uint8_t diagnostics;

  // Канал: “input0”
  // Код АЦП и диагностика 1-го канала ввода напряжения.
	uint32_t input0;
  // Канал: “input1”
  // Код АЦП и диагностика 2-го канала ввода напряжения.
	uint32_t input1;
  // Канал: “input2”
  // Код АЦП и диагностика 3-го канала ввода напряжения.
	uint32_t input2;
  // Канал: “input3”
  // Код АЦП и диагностика 4-го канала ввода напряжения.
	uint32_t input3;
  // Первые три байта (Байт0–2) поля inputХ содержат результат измерения АЦП
  // Биты четвертого байта (Байт3) отражают диагностическую информацию о канале:
  // Бит7: =1: не было выполнено ни одного измерения по данному каналу
  // Бит1: =1: напряжение на входе ниже нижней границы диапазона 
  // Бит0: =1: напряжение на входе выше верхней границы диапазона.
  // Расчет значения напряжения выполняется по формуле:
  //  в диапазоне 0...5 В:    V = (inputХ AND 16#00FFFFFF) * 5.960465188829E-007;
  //  в диапазоне 0...2,5 В:  V = (inputХ AND 16#00FFFFFF) * 2.980232594040E-007;
});

PACKED_STRUCT(
struct _AIM733
{
	AIM733_INPUTS inputs;
	AIM733_CONFIGURATION configuration;
});

//--- AIM791
typedef struct _AIM791 AIM791;
typedef struct _AIM791_INPUTS AIM791_INPUTS;
typedef struct _AIM791_CONFIGURATION AIM791_CONFIGURATION;

// Область конфигурационных параметров модуля AIM791
PACKED_STRUCT(
struct _AIM791_CONFIGURATION
{
	// Резерв! Не используется	
  uint8_t inputRange;

	// Параметр: “Период опроса” (мс)
  uint8_t scanRate;
	// Значение: от 1 до 250

  // Параметр: “Глубина фильтра”:
  // Определяет количество отсчетов, по которым будет выполняться алгоритм фильтрации 
  // методом скользящего среднего для всех каналов
  uint8_t filterDepth;
	// Значение: 
	//  0:   Без фильтра
	//  1:   2 отсчета
	//  2:   4 отсчета
	//  3:   8 отсчетов
	//  4:   16 отсчетов

  // Параметр: "Нижний предел канала (1...8)"
  uint16_t lowLimit[8];
	// Параметр: "Верхний предел канала (1...8)"
	uint16_t highLimit[8];
	// Значение параметров "Нижний/Верхний предел канала" задается кодом АЦП: от 0 до 65535
	// в соответсвующем элементе массива. 
	//   Вычисляется по формуле: 
	//   adcValue = (uint16_t)(limitValue * K);  
	//   limitValue – значение предела в мА и minValue <=limitValue<= maxValue;
	//   minValue, maxValue – нижняя и верхняя границы полного диапазона измерения;
	//   K = 65535 / (maxValue – minValue);

	// Параметр: Диапазон канала (1...8)
  uint8_t channelRanges[8];
	// Значение: 
	//  0:   0...5 mA 
	//  1:   0...20 mA
	//  2:   4...20 mA
});

// Область входных параметров "Inputs" модуля AIM791
PACKED_STRUCT(
struct _AIM791_INPUTS
{
	// Канал: “diagnostics”	
  // Значение 16#FF свидетельствует об отсутствии связи с модулем
  // Если значение не равно 16#FF, то биты канала имеют следующее назначение:
  // Бит 0: =0: таблица калибровочных коэффициентов в норме; 
  //        =1: несовпадение контрольной суммы таблицы калибровочных коэффициентов
  // Бит 1: =0: область специфических параметров модуля в норме
  //        =1: несовпадение контрольной суммы области специфических параметров
  // Бит 2: =0: измерительный тракт исправен
  //        =1: неисправность измерительного тракта
  uint8_t diagnostics;

	// Канал: “ channelRanges”
  // Коды текущих выбранных диапазонов для каналов измерения тока
  // Бит0-1:    =0: установлен диапазон 0…5 мА для 1-го канала
  //            =1: установлен диапазон 0…20 мА для 1-го канала
  //            =2: установлен диапазон 4…20 мА для 1-го канала
  // Бит2-3:    то же, что и Бит0-1, для 2-го канала
  // Бит4-5:    то же, что и Бит0-1, для 3-го канала
  // Бит6-7:    то же, что и Бит0-1, для 4-го канала
  // Бит8-9:    то же, что и Бит0-1, для 5-го канала
  // Бит10-11:  то же, что и Бит0-1, для 6-го канала
  // Бит12-13:  то же, что и Бит0-1, для 7-го канала
  // Бит14-15:  то же, что и Бит0-1, для 8-го канала
  uint16_t channelRanges;

  // Канал: “ channelsStatus”
  // Статус каналов измерения тока
	uint16_t channelsStatus;
  // Бит0-1:    =0: норма для 1-го канала
  //            =1: уровень сигнала ниже значения установленного параметром "Нижний предел канала" для 1-го канала
  //            =2: уровень сигнала выше значения установленного параметром "Верхний предел канала" для 1-го канала
  //            =3: обрыв цепи источника сигнала для 1-го канала
  // Бит2-3:    то же, что и Бит0-1, для 2-го канала
  // Бит4-5:    то же, что и Бит0-1, для 3-го канала
  // Бит6-7:    то же, что и Бит0-1, для 4-го канала
  // Бит8-9:    то же, что и Бит0-1, для 5-го канала
  // Бит10-11:  то же, что и Бит0-1, для 6-го канала
  // Бит12-13:  то же, что и Бит0-1, для 7-го канала
  // Бит14-15:  то же, что и Бит0-1, для 8-го канала

  // Каналы: “ values0 ... values7”
  // Код АЦП на каждом канале измерения тока
  // Значение тока в мА вычисляется по формулам:
  // для диапазона 0..5 мА: I = values[x] * 7.820248722E-005
  // для диапазонов 0..20 мА и 4..20 мА: I = values[x] * 3.128099488E-004
	uint16_t values[8];
});

PACKED_STRUCT(
struct _AIM791
{
	AIM791_INPUTS inputs;
	AIM791_CONFIGURATION configuration;
});

//--- AIM792
typedef struct _AIM792 AIM792;
typedef struct _AIM792_INPUTS AIM792_INPUTS;
typedef struct _AIM792_CONFIGURATION AIM792_CONFIGURATION;

// Область конфигурационных параметров модуля AIM792
PACKED_STRUCT(
struct _AIM792_CONFIGURATION
{
	// Резерв! Не используется
	uint8_t inputRange;    
 
	// Параметр: “Период опроса” (мс)
  uint8_t scanRate;	    
	// Значение: от 1 до 250
	
	// Параметр: “Глубина фильтра”:
	uint8_t filterDepth;    
	// Значение: 
	//  0:   Без фильтра
	//  1:   2 отсчета
	//  2:   4 отсчета
	//  3:   8 отсчетов
	//  4:   16 отсчетов

  // Параметр: Нижний предел канала (1...8)
	uint16_t lowLimit[8];

  // Параметр: Верхний предел канала (1...8)
	uint16_t highLimit[8];
	// Значение параметров "Нижний/Верхний предел канала" задается кодом АЦП: от 0 до 65535
	// в соответсвующем элементе массива. 
	//   Вычисляется по формуле: 
	//   adcValue = (uint16_t)(limitValue * K);  
	//   limitValue – значение предела в мА и minValue <=limitValue<= maxValue;
	//   minValue, maxValue – нижняя и верхняя границы полного диапазона измерения;
	//   K = 65535 / (maxValue – minValue);
	
	// Параметр: Диапазон канала (1...8)
  uint8_t channelRanges[8];
	// Значение: 
	//  0:   0...5 В 
	//  1:   0...10 В
	//  2:   -5...5 В
	//  3:   -10...10 В
});

// Область входных параметров "Inputs" модуля AIM792
PACKED_STRUCT(
struct _AIM792_INPUTS
{
	// Канал: “diagnostics”	
  // Значение 16#FF свидетельствует об отсутствии связи с модулем
  // Если значение не равно 16#FF, то биты канала имеют следующее назначение:
  // Бит 0: =0: таблица калибровочных коэффициентов в норме; 
  //        =1: несовпадение контрольной суммы таблицы калибровочных коэффициентов
  // Бит 1: =0: область специфических параметров модуля в норме
  //        =1: несовпадение контрольной суммы области специфических параметров
  uint8_t diagnostics;

  // Канал: “ channelRanges”
  // Коды текущих выбранных диапазонов для каналов измерения напряжения
  // Бит0-1:    =0: установлен диапазон 0…5 В для 1-го канала
  //            =1: установлен диапазон 0…10 В для 1-го канала
  //            =2: установлен диапазон -5…+5 В для 1-го канала
  //            =3: установлен диапазон -10…+10 В для 1-го канала
  // Бит2-3:    то же, что и Бит0-1, для 2-го канала
  // Бит4-5:    то же, что и Бит0-1, для 3-го канала
  // Бит6-7:    то же, что и Бит0-1, для 4-го канала
  // Бит8-9:    то же, что и Бит0-1, для 5-го канала
  // Бит10-11:  то же, что и Бит0-1, для 6-го канала
  // Бит12-13:  то же, что и Бит0-1, для 7-го канала
  // Бит14-15:  то же, что и Бит0-1, для 8-го канала
	uint16_t channelRanges;

  // Канал: “ channelsStatus”
  // Статус каналов измерения напряжения
	uint16_t channelsStatus;
  // Бит0-1:    =0: норма для 1-го канала
  //            =1: уровень сигнала ниже значения установленного параметром "Нижний предел канала" для 1-го канала
  //            =2: уровень сигнала выше значения установленного параметром "Верхний предел канала" для 1-го канала
  // Бит2-3:    то же, что и Бит0-1, для 2-го канала
  // Бит4-5:    то же, что и Бит0-1, для 3-го канала
  // Бит6-7:    то же, что и Бит0-1, для 4-го канала
  // Бит8-9:    то же, что и Бит0-1, для 5-го канала
  // Бит10-11:  то же, что и Бит0-1, для 6-го канала
  // Бит12-13:  то же, что и Бит0-1, для 7-го канала
  // Бит14-15:  то же, что и Бит0-1, для 8-го канала

  // Каналы: “ values0 ... values7”
  // Коды АЦП на каналах измерения напряжения.
	uint16_t values[8];
  // Значение напряжения (В) вычисляется по формуле:
  // для диапазона 0..5 В:        V = values[х] * 7.819867246E-005;
  // для диапазона 0.. 10 В:      V = values[х] * 15.6397344930E-005;
  // для диапазона от -5..+5 В:   V = (values[х] - 32768) * 15.6397344930E-005;
  // для диапазона от -10..+10 В: V = (values[х] - 32768) * 31.279468986E-005;
});

PACKED_STRUCT(
struct _AIM792
{
	AIM792_INPUTS inputs;
	AIM792_CONFIGURATION configuration;
});

//--- DIM710
typedef struct _DIM710 DIM710;
typedef struct _DIM710_INPUTS DIM710_INPUTS;
typedef struct _DIM710_CONFIGURATION DIM710_CONFIGURATION;

//--- Определение ряда значений параметра "Режим" в конфигурации модуля DIM710
#define DIM710_MODE_FREQUENCYMETERMODE	0   // Измерение частоты
#define DIM710_MODE_ENCODERMODE	        1   // Шифратор приращений

// Область конфигурационных параметров модуля DIM710
PACKED_STRUCT(
struct _DIM710_CONFIGURATION
{
  // Параметр: "Режим"
	uint8_t mode;
  // Значение: 
  //  DIM710_MODE_FREQUENCYMETERMODE (0):   Измерение частоты
  //  DIM710_MODE_ENCODERMODE (1)       :   Шифратор приращений
});

// Область входных параметров "Inputs" модуля DIM710
PACKED_STRUCT(
struct _DIM710_INPUTS
{
  // Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем
  // Если значение не равно 16#FF, то биты канала имеют следующее назначение:
  // Бит 0 -- наличии сигнала на входе 1:  =0 -- есть сигнал ; =1 -- нет сигнала;
  // Бит 1 -- наличии сигнала на входе 2:  =0 -- есть сигнал ; =1 -- нет сигнала;
  // Бит 2 -- наличии сигнала на входе 3:  =0 -- есть сигнал ; =1 -- нет сигнала;
  // Бит 3 -- наличии сигнала на входе 4:  =0 -- есть сигнал ; =1 -- нет сигнала;
  uint8_t diagnostics;

  // Канал: “channel0”
  // Значение периода входного сигнала на 1-м канале, выраженное в микросекундах. 
  // Если сигнал на вход не поступает, channel0 = FFFFFFFFh
  uint32_t channel0;

  // Канал: “channel1”
  // В режиме измерения периода, – значение периода входного сигнала на 2-м канале, выраженное в микросекундах. 
  // В режиме ввода сигнала шифратора приращений, ¬– длительность в микросекундах между передними фронтами сигналов 
  // на первом и втором каналах модуля. Если сигнал на вход не поступает, channel1 = FFFFFFFFh
  uint32_t channel1;

  // Канал: “channel2”
  // Значение периода входного сигнала на 3-м канале, выраженное в микросекундах. 
  // Если сигнал на вход не поступает, channel2 = FFFFFFFFh
  uint32_t channel2;

  // Канал: “channel3”
  // В режиме измерения периода, – значение периода входного сигнала на 4-м канале, выраженное в микросекундах. 
  // В режиме ввода сигнала шифратора приращений, ¬– длительность в микросекундах между передними фронтами сигналов 
  // на третьем и четвертом каналах модуля. Если сигнал на вход не поступает, channel3 = FFFFFFFFh
  uint32_t channel3;

  // Канал: “counter0”
  uint16_t counter0;
  // Канал: “counter1”
  uint16_t counter1;
  // Канал: “counter2”
  uint16_t counter2;
  // Канал: “counter3”
  uint16_t counter3;
  // В режиме измерения периода: нарастающий счетчик передних фронтов на соответствующем канале. 
  // При достижении 65535 сбрасывается в 0. В режиме шифратора приращений: 0
});

PACKED_STRUCT(
struct _DIM710
{
	DIM710_INPUTS inputs;
	DIM710_CONFIGURATION configuration;
});

//--- DIM711
typedef struct _DIM711 DIM711;
typedef struct _DIM711_INPUTS DIM711_INPUTS;
typedef struct _DIM711_OUTPUTS DIM711_OUTPUTS;
typedef struct _DIM711_CONFIGURATION DIM711_CONFIGURATION;

// Область конфигурационных параметров модуля DIM711
PACKED_STRUCT(
struct _DIM711_CONFIGURATION
{
  // Параметр: “Начальные состояния каналов”
  uint8_t initialOutputStates;
  // Значение: Задается значениями битов. 1 соответствует состоянию Включен
  //  Бит0: для 1-го канала
  //  Бит1: для 2-го канала
  //  Бит2: для 3-го канала
  //  Бит3: для 4-го канала

  // Параметр: “Безопасные состояния каналов”
  uint8_t outputSafeStates;
  // Значение: Задается значениями битов. 1 соответствует состоянию Включен
  //  Бит0: для 1-го канала
  //  Бит1: для 2-го канала
  //  Бит2: для 3-го канала
  //  Бит3: для 4-го канала

  // Параметры: “Режим ШИМ на каналах” и “Дискретность ШИМ”
  uint8_t mode;
  // Значение: 
  //  Младшие четыре бита поля mode определяют значение параметра
  //  “Режим ШИМ на каналах” (если =0, то Выкл; если =1, то Вкл):
  //  Бит0: для 1-го канала
  //  Бит1: для 2-го канала
  //  Бит2: для 3-го канала
  //  Бит3: для 4-го канала
  //  Старшие четыре бита поля mode определяют значение параметра
  //  “Дискретность ШИМ” (если =0, то 50 мкс; если =1, то 12,5 мкс):	
  //  Бит4: для 1-го канала
  //  Бит5: для 2-го канала
  //  Бит6: для 3-го канала
  //  Бит7: для 4-го канала
});

// Область входных параметров "Inputs" модуля DIM711
PACKED_STRUCT(
struct _DIM711_INPUTS
{
  // Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем. 
  // В противном случае логическая единица в некотором бите данного канала означает обнаружение 
  // короткого замыкания нагрузки, перегрева или перегрузки по соответствующему выходному каналу
	uint8_t diagnostics;

  // Канал: “channelsStates”
  // Биты данного канала отражают текущее состояние соответствующих выходных каналов модуля. 
  // Логическая 1 соответствует включенному состоянию канала.
	uint8_t channelsStates;

  // Канал: “firstHalfDutyState_PWM0”
  // Текущее значение длительности первой полуволны ШИМ-последовательности на 1-м канале. 
  // В единицах с весом, определяемым параметром Дискретность ШИМ.
	uint16_t firstHalfDutyState_PWM0;
  // Канал: “secondHalfDutyState_PWM0”
  // Текущее значение длительности второй полуволны ШИМ-последовательности на 1-м канале. 
  // В единицах с весом, определяемым параметром Дискретность ШИМ
	uint16_t secondHalfDutyState_PWM0;

  // Канал: “firstHalfDutyState_PWM1”
  // Текущее значение длительности первой полуволны ШИМ-последовательности на 2-м канале. 
  // В единицах с весом, определяемым параметром Дискретность ШИМ.
	uint16_t firstHalfDutyState_PWM1;
  // Канал: “secondHalfDutyState_PWM1”
  // Текущее значение длительности второй полуволны ШИМ-последовательности на 2-м канале. 
  // В единицах с весом, определяемым параметром Дискретность ШИМ
	uint16_t secondHalfDutyState_PWM1;

  // Канал: “firstHalfDutyState_PWM2”
  // Текущее значение длительности первой полуволны ШИМ-последовательности  на 3-м канале. 
  // В единицах с весом, определяемым параметром Дискретность ШИМ.
	uint16_t firstHalfDutyState_PWM2;
  // Канал: “secondHalfDutyState_PWM2”
  // Текущее значение длительности второй полуволны ШИМ-последовательности  на 3-м канале.
  // В единицах с весом, определяемым параметром Дискретность ШИМ
	uint16_t secondHalfDutyState_PWM2;

  // Канал: “firstHalfDutyState_PWM3”
  // Текущее значение длительности первой полуволны ШИМ-последовательности  на 4-м канале.
  // В единицах с весом, определяемым параметром Дискретность ШИМ.
	uint16_t firstHalfDutyState_PWM3;
  // Канал: “secondHalfDutyState_PWM3”
  // Текущее значение длительности второй полуволны ШИМ-последовательности  на 4-м канале.
  // В единицах с весом, определяемым параметром Дискретность ШИМ
	uint16_t secondHalfDutyState_PWM3;
});

// Область выходных параметров "Outputs" модуля DIM711
PACKED_STRUCT(
struct _DIM711_OUTPUTS
{
  // Канал: “digitalOutputs”
  // Биты данного канала предназначены для управления выходными каналами модуля. 
  // Логическая 1 соответствует включению канала
  uint8_t digitalOutputs;

  // Канал: “firstHalfDutyInterval_PWM0”
  // Устанавливает длительность первой полуволны ШИМ-последовательности на 1-м канале. 
  // Задается с в единицах с весом, определяемым параметром Дискретность ШИМ.
  uint16_t firstHalfDuty_PWM0;
  // Канал: “secondHalfDutyInterval_PWM0”
  // Устанавливает длительность второй полуволны ШИМ-последовательности на 1-м канале. 
  // Задается с в единицах с весом, определяемым параметром Дискретность ШИМ.
  uint16_t secondHalfDuty_PWM0;

  // Канал: “firstHalfDutyInterval_PWM1”
  // Устанавливает длительность первой полуволны ШИМ-последовательности на 2-м канале. 
  // Задается с в единицах с весом, определяемым параметром Дискретность ШИМ.
  uint16_t firstHalfDuty_PWM1;
  // Канал: “secondHalfDutyInterval_PWM1”
  // Устанавливает длительность второй полуволны ШИМ-последовательности на 2-м канале. 
  // Задается с в единицах с весом, определяемым параметром Дискретность ШИМ.
  uint16_t secondHalfDuty_PWM1;

  // Канал: “firstHalfDutyInterval_PWM2”
  // Устанавливает длительность первой полуволны ШИМ-последовательности на 3-м канале. 
  // Задается с в единицах с весом, определяемым параметром Дискретность ШИМ.
  uint16_t firstHalfDuty_PWM2;
  // Канал: “secondHalfDutyInterval_PWM2”
  // Устанавливает длительность второй полуволны ШИМ-последовательности на 3-м канале. 
  // Задается с в единицах с весом, определяемым параметром Дискретность ШИМ.
  uint16_t secondHalfDuty_PWM2;

  // Канал: “firstHalfDutyInterval_PWM3”
  // Устанавливает длительность первой полуволны ШИМ-последовательности на 4-м канале. 
  // Задается с в единицах с весом, определяемым параметром Дискретность ШИМ.
  uint16_t firstHalfDuty_PWM3;
  // Канал: “secondHalfDutyInterval_PWM3”
  // Устанавливает длительность второй полуволны ШИМ-последовательности на 4-м канале. 
  // Задается с в единицах с весом, определяемым параметром Дискретность ШИМ.
  uint16_t secondHalfDuty_PWM3;
});

PACKED_STRUCT(
struct _DIM711
{
	DIM711_INPUTS inputs;
	DIM711_OUTPUTS outputs;
	DIM711_CONFIGURATION configuration;
});

//--- DIM712
typedef struct _DIM712 DIM712;
typedef struct _DIM712_INPUTS DIM712_INPUTS;
typedef struct _DIM712_OUTPUTS DIM712_OUTPUTS;
typedef struct _DIM712_CONFIGURATION DIM712_CONFIGURATION;

// Область конфигурационных параметров модуля DIM712
PACKED_STRUCT(
struct _DIM712_CONFIGURATION
{
	// Параметр: “Начальные состояния каналов”
  uint8_t initialOutputStates;
	// Значение: Задается значениями битов. 1 соответствует состоянию Включен
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала

	// Параметр: “Безопасные состояния каналов”
  uint8_t outputSafeStates;
	// Значение: Задается значениями битов. 1 соответствует состоянию Включен
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала
});

// Область входных параметров "Inputs" модуля DIM712
PACKED_STRUCT(
struct _DIM712_INPUTS
{
	// Канал: “diagnostics”  
  // Значение FFh свидетельствует об отсутствии связи с модулем
  uint8_t diagnostics;

	// Канал: “inputStates”
	//  Первые два бита данного канала отражают текущее состояние соответствующих каналов релейной коммутации. 
  // Логическая 1 соответствует состоянию включенного канала.
	uint8_t inputStates;
});

// Область выходных параметров "Outputs" модуля DIM712
PACKED_STRUCT(
struct _DIM712_OUTPUTS
{
	// Канал: “outputsControl”
  // Первые два бита данного канала предназначены для управления каналами релейной коммутации модуля. 
  // Логическая 1 соответствует состоянию включенного канала
	uint8_t outputsControl;
});

PACKED_STRUCT(
struct _DIM712
{
	DIM712_INPUTS inputs;
	DIM712_OUTPUTS outputs;
	DIM712_CONFIGURATION configuration;
});

//--- DIM713
typedef struct _DIM713 DIM713;
typedef struct _DIM713_INPUTS DIM713_INPUTS;
typedef struct _DIM713_OUTPUTS DIM713_OUTPUTS;
typedef struct _DIM713_CONFIGURATION DIM713_CONFIGURATION;

// Область конфигурационных параметров модуля DIM713
PACKED_STRUCT(
struct _DIM713_CONFIGURATION
{
	// Параметр: “Начальные состояния каналов”
	uint8_t initialOutputStates;
	// Значение: Задается значениями битов. 1 соответствует состоянию Включен
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала

	// Параметр: “Безопасные состояния каналов”
	uint8_t outputSafeStates;
	// Значение: Задается значениями битов. 1 соответствует состоянию Включен
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала
});

// Область входных параметров "Inputs" модуля DIM713
PACKED_STRUCT(
struct _DIM713_INPUTS
{
	// Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем
	uint8_t diagnostics;
	// Канал: “inputStates”
  // Первые два бита данного канала отражают текущее состояние соответствующих каналов релейной коммутации. 
  // Логическая 1 соответствует замкнутому состоянию контактов реле
	uint8_t inputStates;
});

// Область выходных параметров "Outputs" модуля DIM713
PACKED_STRUCT(
struct _DIM713_OUTPUTS
{
	// Канал: "outputsControl"
  // Первые два бита данного канала предназначены для управления каналами релейной коммутации модуля. 
  // Логическая 1 соответствует замкнутому состоянию контактов реле.
	uint8_t outputsControl;
});

PACKED_STRUCT(
struct _DIM713
{
	DIM713_INPUTS inputs;
	DIM713_OUTPUTS outputs;
	DIM713_CONFIGURATION configuration;
});

//---
//--- Определение ряда значений параметра "Устранение дребезга контактов" 
//    в конфигурации модулей: DIM714, DIM716, DIM717, DIM760, DIM761, DIM762
#define DIM_DEBOUNCE_NODEBOUNCE	    0   // антидребезг не используется
#define DIM_DEBOUNCE_DEBOUNCE_200US	1   // время устранения дребезга около 200 мкс
#define DIM_DEBOUNCE_DEBOUNCE_3MS	  2   // время устранения дребезга около 3 мс

//--- Определение ряда значений параметра "Конфигурация счетчика". 
//    Смотри описание структуры _COUNTER_INPUT_SETTINGS
// Выбор активного фронта сигнала 
#define EDGE_SETTINGS_RISINGEDGE	      0   // Передний фронт
#define EDGE_SETTINGS_FALLINGEDGE	      1   // Задний фронт
// Направление счета
#define COUNTER_DIRECTION_UPWARDCOUNT	  0   // Счет с суммированием
#define COUNTER_DIRECTION_DOWNWARDCOUNT	1   // Счет с вычитанием
// Тип поведения счетчика при достижении предельного значения
#define COUNTER_MODE_CONTINUOUSUPDATE	0     // Прекращение счета 
#define COUNTER_MODE_CYCLICUPDATE	    1     // Циклический счетчик

// Структура описания параметра "Конфигурация счетчика"
PACKED_STRUCT(
struct _COUNTER_INPUT_SETTINGS
{
  // Выбор активного фронта сигнала 
	uint8_t edge;
	// Значение: 
	//  EDGE_SETTINGS_RISINGEDGE  (0):   Передний фронт
	//  EDGE_SETTINGS_FALLINGEDGE (1):   Задний фронт

  // Направление счета
	uint8_t direction;
	// Значение: 
	//  COUNTER_DIRECTION_UPWARDCOUNT   (0):   Счет с суммированием
	//  COUNTER_DIRECTION_DOWNWARDCOUNT (1):   Счет с вычитанием

  // Тип поведения счетчика при достижении предельного значения
	uint8_t countingMode;
	// Значение: 
	//  COUNTER_MODE_CONTINUOUSUPDATE   (0):   Прекращение счета 
	//  COUNTER_MODE_CYCLICUPDATE       (1):   Циклический счетчик

  // Предустановленное значение
  uint16_t presetValue;
});
typedef struct _COUNTER_INPUT_SETTINGS COUNTER_INPUT_SETTINGS;
//---

//--- DIM714
typedef struct _DIM714 DIM714;
typedef struct _DIM714_INPUTS DIM714_INPUTS;
typedef struct _DIM714_CONFIGURATION DIM714_CONFIGURATION;

// Область конфигурационных параметров модуля DIM714
PACKED_STRUCT(
struct _DIM714_CONFIGURATION
{
	// Параметр: “Устранение дребезга контактов”
  uint8_t debounce;
	// Значение: 
	//  DIM_DEBOUNCE_NODEBOUNCE (0)    :   Не устраняется
	//  DIM_DEBOUNCE_DEBOUNCE_200US (1):   Период 200 мкс
	//  DIM_DEBOUNCE_DEBOUNCE_3MS (2)  :   Период 3000 мкс

	// Параметр: “Режим счетчика”
  uint8_t enableCounting;
	// Значение: 
	//  0:   Запретить на 1х двух каналах
	//  1:   Разрешить на 1х двух каналах

	// Параметр: "Конфигурация счетчика 1..2"
  // Зарезервировано/Не используется в текущей версии прошивки модуля
  COUNTER_INPUT_SETTINGS countingParameters[2];
});

// Область входных параметров "Inputs" модуля DIM714
PACKED_STRUCT(
struct _DIM714_INPUTS
{
	// Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем
  uint8_t diagnostics;

	// Канал: “inputStates”
  // Первые четыре бита данного канала отражают текущее состояние соответствующих входных каналов. 
  // Логическая 1 соответствует логической 1 на входе
  uint8_t inputStates;

	// Канал: “counter0”	
  uint16_t counter0;
	// Канал: “counter1”
	uint16_t counter1;
  // Значения суммирующих счетчиков на первом и втором каналах. 
  // После переполнения счетчик сбрасывается в 0 и продолжает счет
	
  // Зарезервировано/Не используется в текущей версии прошивки модуля
	uint8_t countersState;
});

//--- DIM715
typedef struct _DIM715 DIM715;
typedef struct _DIM715_INPUTS DIM715_INPUTS;

// Область входных параметров "Inputs" модуля DIM715
PACKED_STRUCT(
struct _DIM715_INPUTS
{
	// Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем
	uint8_t diagnostics;

	// Канал: “inputStates”
  // Первые два бита данного канала отражают текущее состояние соответствующих входных каналов. 
  // Логическая 1 соответствует логической 1 на входе
	uint8_t inputStates;
});

PACKED_STRUCT(
struct _DIM715
{
	DIM715_INPUTS inputs;
});

//--- DIM716
typedef struct _DIM716 DIM716;
typedef struct _DIM716_INPUTS DIM716_INPUTS;
typedef struct _DIM716_CONFIGURATION DIM716_CONFIGURATION;

// Область конфигурационных параметров модуля DIM716
PACKED_STRUCT(
struct _DIM716_CONFIGURATION
{
	// Параметр: “Устранение дребезга контактов”
	// Значение: 
	//  DIM_DEBOUNCE_NODEBOUNCE (0)    :   Не устраняется
	//  DIM_DEBOUNCE_DEBOUNCE_200US (1):   Период 200 мкс
	//  DIM_DEBOUNCE_DEBOUNCE_3MS (2)  :   Период 3000 мкс
	uint8_t debounce;

  // Параметр: “Режим счетчика”
	// Значение: 
	//  0:   Запретить на 1х двух каналах
	//  1:   Разрешить на 1х двух каналах
  uint8_t enableCounting;

	// Параметр: "Конфигурация счетчика 1..2"
  // Зарезервировано/Не используется в текущей версии прошивки модуля
  COUNTER_INPUT_SETTINGS countingParameters[2];
});

// Область входных параметров "Inputs" модуля DIM716
PACKED_STRUCT(
struct _DIM716_INPUTS
{
	// Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем
  uint8_t diagnostics;

	// Канал: “inputStates”
  // Первые два бита данного канала отражают текущее состояние соответствующих входных каналов. 
  // Логическая 1 соответствует логической 1 на входе
	uint8_t inputStates;

	// Канал: “counter0”
	uint16_t counter0;
	// Канал: “counter1”
	uint16_t counter1;
  // Значения суммирующих счетчиков на первом и втором каналах. 
  // После переполнения счетчик сбрасывается в 0 и продолжает счет

  // Зарезервировано/Не используется в текущей версии прошивки модуля
	uint8_t countersState;
});

PACKED_STRUCT(
struct _DIM716
{
	DIM716_INPUTS inputs;
	DIM716_CONFIGURATION configuration;
});

//--- DIM717
typedef struct _DIM717 DIM717;
typedef struct _DIM717_INPUTS DIM717_INPUTS;
typedef struct _DIM717_CONFIGURATION DIM717_CONFIGURATION;

// Область конфигурационных параметров модуля DIM717
PACKED_STRUCT(
struct _DIM717_CONFIGURATION
{
	// Параметр: “Устранение дребезга контактов”
  uint8_t debounce;
	// Значение: 
	//  DIM_DEBOUNCE_NODEBOUNCE (0)    :   Не устраняется
	//  DIM_DEBOUNCE_DEBOUNCE_200US (1):   Период 200 мкс
	//  DIM_DEBOUNCE_DEBOUNCE_3MS (2)  :   Период 3000 мкс

  // Параметр: “Режим счетчика”
	uint8_t enableCounting;
	// Значение: 
	//  0:   Запретить на 1х двух каналах
	//  1:   Разрешить на 1х двух каналах

	// Параметр: "Конфигурация счетчика 1..2"
	// Зарезервировано/Не используется в текущей версии прошивки модуля
  COUNTER_INPUT_SETTINGS countingParameters[2];
});

// Область входных параметров "Inputs" модуля DIM717
PACKED_STRUCT(
struct _DIM717_INPUTS
{
	// Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем
	uint8_t diagnostics;

	// Канал: “inputStates”
  // Биты данного канала отражают текущее состояние соответствующих входных каналов. 
  // Логическая 1 соответствует логической 1 на входе
	uint8_t inputStates;

	// Канал: “counter0”
	uint16_t counter0;
	// Канал: “counter1”
	uint16_t counter1;
  // Значения суммирующих счетчиков на первом и втором каналах. 
  // После переполнения счетчик сбрасывается в 0 и продолжает счет

  // Зарезервировано/Не используется в текущей версии прошивки модуля
	uint8_t countersState;
});

PACKED_STRUCT(
struct _DIM717
{
	DIM717_INPUTS inputs;
	DIM717_CONFIGURATION configuration;
});

//--- DIM718
typedef struct _DIM718 DIM718;
typedef struct _DIM718_INPUTS DIM718_INPUTS;
typedef struct _DIM718_OUTPUTS DIM718_OUTPUTS;
typedef struct _DIM718_CONFIGURATION DIM718_CONFIGURATION;

// Область конфигурационных параметров модуля DIM718
PACKED_STRUCT(
struct _DIM718_CONFIGURATION
{
	// Параметр: “Начальные состояния каналов”
	uint8_t initialOutputStates;
	// Значение: Задается значениями битов. 1 соответствует состоянию Включен
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала
	//  ...
	//  Бит7: для 8-го канала

  // Параметр: “Безопасные состояния каналов”
  uint8_t outputSafeStates;
	// Значение: Задается значениями битов. 1 соответствует состоянию Включен
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала
	//  ...
	//  Бит7: для 8-го канала

  // Параметр: “Режим ШИМ на каналах”
  uint8_t mode;
	// Значение: 
	//  Младшие четыре бита поля mode определяют значение параметра
	//  “Режим ШИМ на каналах” (если =0, то Выкл; если =1, то Вкл):
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала
	//  Бит2: для 3-го канала
	//  Бит3: для 4-го канала
});

// Область входных параметров "Inputs" модуля DIM718
PACKED_STRUCT(
struct _DIM718_INPUTS
{
	// Канал: “diagnostics”
  // Значение байта, равное FFh (255) означает отсутствие связи с модулем по шине.
  // В противном случае логическая единица в битах с 0-го по 3-й означает обнаружение 
  // короткого замыкания нагрузки, перегрева или перегрузки по соответствующей паре  
  // (1-2, 3-4, 5-6, 7-8) выходных каналов. 
	uint8_t diagnostics;

  // Канал: “channelsStates”
  // Биты данного канала отражают текущее состояние соответствующих выходных каналов модуля. 
  // Логическая 1 соответствует включенному состоянию канала
  uint8_t channelsStates;

  // Канал: “firstHalfDutyState_PWM0”
  // Текущее значение длительности первой полуволны ШИМ-последовательности на 1-м канале 
  // в единицах с весом 50 мкс.
	uint16_t firstHalfDutyState_PWM0;
	// Канал: “secondHalfDutyState_PWM0”
  // Текущее значение длительности второй полуволны ШИМ-последовательности на 1-м канале 
  // в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM0;

  // Канал: “firstHalfDutyState_PWM1”
  // Текущее значение длительности первой полуволны ШИМ-последовательности на 2-м канале 
  // в единицах с весом 50 мкс.
	uint16_t firstHalfDutyState_PWM1;
	// Канал: “secondHalfDutyState_PWM1”
  // Текущее значение длительности второй полуволны ШИМ-последовательности на 2-м канале 
  // в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM1;

	// Канал: “firstHalfDutyState_PWM2”
  // Текущее значение длительности первой полуволны ШИМ-последовательности на 3-м канале 
  // в единицах с весом 50 мкс.
  uint16_t firstHalfDutyState_PWM2;
	// Канал: “secondHalfDutyState_PWM2”
  // Текущее значение длительности второй полуволны ШИМ-последовательности на 3-м канале 
  // в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM2;

	// Канал: “firstHalfDutyState_PWM3”
  // Текущее значение длительности первой полуволны ШИМ-последовательности на 4-м канале 
  // в единицах с весом 50 мкс.
	uint16_t firstHalfDutyState_PWM3;
	// Канал: “secondHalfDutyState_PWM3”
  // Текущее значение длительности второй полуволны ШИМ-последовательности на 4-м канале 
  // в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM3;
});

// Область входных параметров "Outputs" модуля DIM718
PACKED_STRUCT(
struct _DIM718_OUTPUTS
{
	// Канал: “digitalOutputs”
  // Биты данного канала предназначены для управления выходными каналами модуля. 
  // Логическая 1 соответствует включению канала
	uint8_t digitalOutputs;

	// Канал: “firstHalfDutyInterval_PWM0”
  // Длительность первой полуволны ШИМ-последовательности на 1-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t firstHalfDuty_PWM0;
	// Канал: “secondHalfDutyInterval_PWM0”
  // Длительность второй полуволны ШИМ-последовательности 1-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t secondHalfDuty_PWM0;

	// Канал: “firstHalfDutyInterval_PWM1”
  // Длительность первой полуволны ШИМ-последовательности на 2-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t firstHalfDuty_PWM1;
	// Канал: “secondHalfDutyInterval_PWM1”
  // Длительность второй полуволны ШИМ-последовательности 2-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t secondHalfDuty_PWM1;

	// Канал: “firstHalfDutyInterval_PWM2”
  // Длительность первой полуволны ШИМ-последовательности на 3-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t firstHalfDuty_PWM2;
	// Канал: “secondHalfDutyInterval_PWM2”
  // Длительность второй полуволны ШИМ-последовательности 3-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t secondHalfDuty_PWM2;

	// Канал: “firstHalfDutyInterval_PWM3”
  // Длительность первой полуволны ШИМ-последовательности на 4-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t firstHalfDuty_PWM3;
	// Канал: “secondHalfDutyInterval_PWM3”
  // Длительность второй полуволны ШИМ-последовательности 4-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t secondHalfDuty_PWM3;
});

PACKED_STRUCT(
struct _DIM718
{
	DIM718_INPUTS inputs;
	DIM718_OUTPUTS outputs;
	DIM718_CONFIGURATION configuration;
});

//--- DIM719
typedef struct _DIM719 DIM719;
typedef struct _DIM719_INPUTS DIM719_INPUTS;
typedef struct _DIM719_OUTPUTS DIM719_OUTPUTS;
typedef struct _DIM719_CONFIGURATION DIM719_CONFIGURATION;

// Область конфигурационных параметров модуля DIM719
PACKED_STRUCT(
struct _DIM719_CONFIGURATION
{
	// Параметр: “Начальные состояния каналов”
	uint8_t initialOutputStates;
	// Значение: Задается значениями битов. 1 соответствует состоянию Включен
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала
	//  ...
	//  Бит7: для 8-го канала

  // Параметр: “Безопасные состояния каналов”
  uint8_t outputSafeStates;
	// Значение: Задается значениями битов. 1 соответствует состоянию Включен
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала
	//  ...
	//  Бит7: для 8-го канала

  // Параметр: “Режим ШИМ на каналах”
  uint8_t mode;
	// Значение: 
	//  Младшие четыре бита поля mode определяют значение параметра
	//  “Режим ШИМ на каналах” (если =0, то Выкл; если =1, то Вкл):
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала
	//  Бит2: для 3-го канала
	//  Бит3: для 4-го канала
});

// Область входных параметров "Inputs" модуля DIM719
PACKED_STRUCT(
struct _DIM719_INPUTS
{
	// Канал: “diagnostics”
  // Значение байта, равное FFh означает отсутствие связи с модулем по шине.
  // В противном случае логическая единица в битах с 0-го по 3-й означает обнаружение 
  // короткого замыкания нагрузки, перегрева или перегрузки по соответствующей паре  
  // (1-2. 3-4, 5-6, 7-8) выходных каналов
	uint8_t diagnostics;

  // Канал: “channelsStates”
  // Биты данного канала отражают текущее состояние соответствующих выходных каналов модуля. 
  // Логическая 1 соответствует включенному состоянию канала
  uint8_t channelsStates;

  // Канал: “firstHalfDutyState_PWM0”
  // Текущее значение длительности первой полуволны ШИМ-последовательности  на 1-м канале 
  // в единицах с весом 50 мкс
	uint16_t firstHalfDutyState_PWM0;
	// Канал: “secondHalfDutyState_PWM0”
  // Текущее значение длительности второй полуволны ШИМ-последовательности  на 1-м канале 
  // в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM0;

  // Канал: “firstHalfDutyState_PWM1”
  // Текущее значение длительности первой полуволны ШИМ-последовательности  на 2-м канале 
  // в единицах с весом 50 мкс
	uint16_t firstHalfDutyState_PWM1;
	// Канал: “secondHalfDutyState_PWM1”
  // Текущее значение длительности второй полуволны ШИМ-последовательности  на 2-м канале 
  // в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM1;

	// Канал: “firstHalfDutyState_PWM2”
  // Текущее значение длительности первой полуволны ШИМ-последовательности  на 3-м канале 
  // в единицах с весом 50 мкс
  uint16_t firstHalfDutyState_PWM2;
	// Канал: “secondHalfDutyState_PWM2”
  // Текущее значение длительности второй полуволны ШИМ-последовательности  на 3-м канале 
  // в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM2;

	// Канал: “firstHalfDutyState_PWM3”
  // Текущее значение длительности первой полуволны ШИМ-последовательности  на 4-м канале 
  // в единицах с весом 50 мкс
	uint16_t firstHalfDutyState_PWM3;
	// Канал: “secondHalfDutyState_PWM3”
  // Текущее значение длительности второй полуволны ШИМ-последовательности  на 4-м канале 
  // в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM3;
});

// Область выходных параметров "Outputs" модуля DIM719
PACKED_STRUCT(
struct _DIM719_OUTPUTS
{
	// Канал: “digitalOutputs”
  // Биты данного канала предназначены для управления выходными каналами модуля. 
  // Логическая 1 соответствует включению канала
	uint8_t digitalOutputs;

  // Канал: “firstHalfDutyInterval_PWM0”
  // Длительность первой полуволны ШИМ-последовательности на 1-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t firstHalfDuty_PWM0;
	// Канал: “secondHalfDutyInterval_PWM0”
  // Длительность второй полуволны ШИМ-последовательности 1-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t secondHalfDuty_PWM0;

	// Канал: “firstHalfDutyInterval_PWM1”
  // Длительность первой полуволны ШИМ-последовательности на 2-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t firstHalfDuty_PWM1;
	// Канал: “secondHalfDutyInterval_PWM1”
  // Длительность второй полуволны ШИМ-последовательности 2-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t secondHalfDuty_PWM1;

	// Канал: “firstHalfDutyInterval_PWM2”
  // Длительность первой полуволны ШИМ-последовательности на 3-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t firstHalfDuty_PWM2;
	// Канал: “secondHalfDutyInterval_PWM2”
  // Длительность второй полуволны ШИМ-последовательности 3-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t secondHalfDuty_PWM2;

	// Канал: “firstHalfDutyInterval_PWM3”
  // Длительность первой полуволны ШИМ-последовательности на 4-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t firstHalfDuty_PWM3;
	// Канал: “secondHalfDutyInterval_PWM3”
  // Длительность второй полуволны ШИМ-последовательности 4-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, не должно быть менее 8 
  // (соответствует примерно 400 мкс плюс длительность включения и выключения ключа)
	uint16_t secondHalfDuty_PWM3;
});

PACKED_STRUCT(
struct _DIM719
{
	DIM719_INPUTS inputs;
	DIM719_OUTPUTS outputs;
	DIM719_CONFIGURATION configuration;
});

//--- DIM760
typedef struct _DIM760 DIM760;
typedef struct _DIM760_INPUTS DIM760_INPUTS;
typedef struct _DIM760_CONFIGURATION DIM760_CONFIGURATION;

// Область конфигурационных параметров модуля DIM760
PACKED_STRUCT(
struct _DIM760_CONFIGURATION
{
	// Параметр: “Устранение дребезга контактов”
  uint8_t debounce;
	// Значение: 
	//  DIM_DEBOUNCE_NODEBOUNCE (0)    :   Не устраняется
	//  DIM_DEBOUNCE_DEBOUNCE_200US (1):   Период 200 мкс
	//  DIM_DEBOUNCE_DEBOUNCE_3MS (2)  :   Период 3000 мкс

  // Параметр: “Режим счетчика”
	uint8_t enableCounting;
	// Значение: 
	//  0:   Запретить на 1х двух каналах
	//  1:   Разрешить на 1х двух каналах

	// Параметр: "Конфигурация счетчика 1..2"
	// Зарезервировано/Не используется в текущей версии прошивки модуля
  COUNTER_INPUT_SETTINGS countingParameters[2];
});

// Область входных параметров "Inputs" модуля DIM760
PACKED_STRUCT(
struct _DIM760_INPUTS
{
	// Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем
	uint8_t diagnostics;

	// Канал: “inputStates”
  // Первые четыре бита данного канала отражают текущее состояние соответствующих входных каналов. 
  // Логическая 1 соответствует логической 1 на входе
	uint8_t inputStates;

	// Канал: “counter0”
  // Значения суммирующего счетчика на 1-м канале. 
  // После переполнения счетчик сбрасывается в 0 и продолжает счет
	uint16_t counter0;
	// Канал: “counter1”
  // Значения суммирующего счетчика на 2-м канале. 
  // После переполнения счетчик сбрасывается в 0 и продолжает счет
	uint16_t counter1;

	// Зарезервировано/Не используется в текущей версии прошивки модуля
	uint8_t countersState;
});

PACKED_STRUCT(
struct _DIM760
{
	DIM760_INPUTS inputs;
	DIM760_CONFIGURATION configuration;
});

//--- DIM761
typedef struct _DIM761 DIM761;
typedef struct _DIM761_INPUTS DIM761_INPUTS;
typedef struct _DIM761_CONFIGURATION DIM761_CONFIGURATION;

// Область конфигурационных параметров модуля DIM761
PACKED_STRUCT(
struct _DIM761_CONFIGURATION
{
	// Параметр: “Устранение дребезга контактов”
  uint8_t debounce;
	// Значение: 
	//  DIM_DEBOUNCE_NODEBOUNCE (0)    :   Не устраняется
	//  DIM_DEBOUNCE_DEBOUNCE_200US (1):   Период 200 мкс
	//  DIM_DEBOUNCE_DEBOUNCE_3MS (2)  :   Период 3000 мкс

  // Параметр: “Режим счетчика”
	uint8_t enableCounting;
	// Значение: 
	//  0:   Запретить на 1х двух каналах
	//  1:   Разрешить на 1х двух каналах

	// Параметр: "Конфигурация счетчика 1..2"
	// Зарезервировано/Не используется в текущей версии прошивки модуля
  COUNTER_INPUT_SETTINGS countingParameters[2];
});

// Область входных параметров "Inputs" модуля DIM761
PACKED_STRUCT(
struct _DIM761_INPUTS
{
	// Канал: “diagnostics”
  // Значение FFh свидетельствует об отсутствии связи с модулем
	uint8_t diagnostics;

	// Канал: “inputStates”
  // Первые четыре бита данного канала отражают текущее состояние соответствующих входных каналов. 
  // Логическая 1 соответствует логической 1 на входе.
	uint8_t inputStates;

	// Канал: “counter0”
  // Значения суммирующего счетчика на 1-м канале. 
  // После переполнения счетчик сбрасывается в 0 и продолжает счет
	uint16_t counter0;
	// Канал: “counter1”
  // Значения суммирующего счетчика на 2-м канале. 
  // После переполнения счетчик сбрасывается в 0 и продолжает счет
	uint16_t counter1;

	// Зарезервировано/Не используется в текущей версии прошивки модуля
	uint8_t countersState;
});

PACKED_STRUCT(
struct _DIM761
{
	DIM761_INPUTS inputs;
	DIM761_CONFIGURATION configuration;
});

//--- DIM762
typedef struct _DIM762 DIM762;
typedef struct _DIM762_INPUTS DIM762_INPUTS;
typedef struct _DIM762_CONFIGURATION DIM762_CONFIGURATION;

// Область конфигурационных параметров модуля DIM762
PACKED_STRUCT(
struct _DIM762_CONFIGURATION
{
	// Параметр: “Устранение дребезга контактов”
  uint8_t debounce;
	// Значение: 
	//  DIM_DEBOUNCE_NODEBOUNCE (0)    :   Не устраняется
	//  DIM_DEBOUNCE_DEBOUNCE_200US (1):   Период 200 мкс
	//  DIM_DEBOUNCE_DEBOUNCE_3MS (2)  :   Период 3000 мкс

  // Параметр: “Режим счетчика”
	uint8_t enableCounting;
	// Значение: 
	//  0:   Запретить на 1х двух каналах
	//  1:   Разрешить на 1х двух каналах

	// Параметр: "Конфигурация счетчика 1..2"
	// Зарезервировано/Не используется в текущей версии прошивки модуля
  COUNTER_INPUT_SETTINGS countingParameters[2];
});

// Область входных параметров "Inputs" модуля DIM762
PACKED_STRUCT(
struct _DIM762_INPUTS
{
	// Диагностический виртуальный канал модуля. В модуле физически не реализован. 
  // Значение FFh свидетельствует об отсутствии связи с модулем. 
  // Значение 0 свидетельствует о наличии связи с модулем.
	uint8_t diagnostics;
	// Биты данного канала отражают текущее состояние соответствующих физических каналов. 
  // Логическая 1 соответствует логической 1 на входе
	uint8_t inputStates;
	// Значения суммирующего счетчика на первом канале. 
  // После переполнения счетчик сбрасывается в 0 и продолжает счет
	uint16_t counter0;
	// Значения суммирующего счетчиков на втором каналах. 
  // После переполнения счетчик сбрасывается в 0 и продолжает счет
	uint16_t counter1;
	// Зарезервировано/Не используется в текущей версии прошивки модуля
	uint8_t countersState;
});

PACKED_STRUCT(
struct _DIM762
{
	DIM762_INPUTS inputs;
	DIM762_CONFIGURATION configuration;
});

//--- DIM763
typedef struct _DIM763 DIM763;
typedef struct _DIM763_INPUTS DIM763_INPUTS;
typedef struct _DIM763_OUTPUTS DIM763_OUTPUTS;
typedef struct _DIM763_CONFIGURATION DIM763_CONFIGURATION;

// Область конфигурационных параметров модуля DIM763
PACKED_STRUCT(
struct _DIM763_CONFIGURATION
{
	// Параметр: “Начальные состояния каналов”
	uint8_t initialOutputStates;
	// Значение: Задается значениями битов. 1 соответствует состоянию Включен
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала
	//  Бит2: для 3-го канала
	//  Бит3: для 4-го канала
  //  Сразу после включения модуля (до получения модулем запросов от контроллера)
  //  каналы модуля будут переведены в установленное состояние 

	// Параметр: “Безопасные состояния каналов”
  uint8_t outputSafeStates;
	// Значение: Задается значениями битов. 1 соответствует состоянию Включен
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала
	//  Бит2: для 3-го канала
	//  Бит3: для 4-го канала
  //  Если значение параметра Интервал сторожевого таймера мастера отлично от нуля, 
  //  то при отсутствии запросов к модулю по внутренней шине в течение времени, 
  //  превышающего значение интервала сторожевого таймера, каналы модуля будут переведены 
  //  в установленное состояние.

	// Параметр: “Режим ШИМ на каналах”
  uint8_t mode;
	// Значение: 
	//  Младшие четыре бита поля mode определяют значение параметра
	//  “Режим ШИМ на каналах” (если =0, то Выкл; если =1, то Вкл):
	//  Бит0: для 1-го канала
	//  Бит1: для 2-го канала
	//  Бит2: для 3-го канала
	//  Бит3: для 4-го канала
});

// Область входных параметров "Inputs" модуля DIM763
PACKED_STRUCT(
struct _DIM763_INPUTS
{
	// Диагностический канал
  // Значение FFh свидетельствует об отсутствии связи с модулем. 
  // Значение 0 свидетельствует о наличии связи с модулем.
	uint8_t diagnostics;
	
  // Биты данного канала отражают текущее состояние соответствующих выходных каналов модуля. 
  // Логическая 1 соответствует включенному состоянию канала.
	uint8_t channelsStates;

	// Текущее значение длительности первой полуволны ШИМ-последовательности  
  // на 1-м канале модуля в единицах с весом 50 мкс.
	uint16_t firstHalfDutyState_PWM0;
	// Текущее значение длительности второй полуволны ШИМ-последовательности  
  // на 1-м канале модуля в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM0;

	// Текущее значение длительности первой полуволны ШИМ-последовательности  
  // на 2-м канале модуля в единицах с весом 50 мкс.
	uint16_t firstHalfDutyState_PWM1;
	// Текущее значение длительности второй полуволны ШИМ-последовательности  
  // на 2-м канале модуля в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM1;

	// Текущее значение длительности первой полуволны ШИМ-последовательности  
  // на 3-м канале модуля в единицах с весом 50 мкс.
	uint16_t firstHalfDutyState_PWM2;
	// Текущее значение длительности второй полуволны ШИМ-последовательности  
  // на 3-м канале модуля в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM2;

	// Текущее значение длительности первой полуволны ШИМ-последовательности  
  // на 4-м канале модуля в единицах с весом 50 мкс.
	uint16_t firstHalfDutyState_PWM3;
	// Текущее значение длительности второй полуволны ШИМ-последовательности  
  // на 4-м канале модуля в единицах с весом 50 мкс.
	uint16_t secondHalfDutyState_PWM3;
});

// Область выходных параметров "Outputs" модуля DIM763
PACKED_STRUCT(
struct _DIM763_OUTPUTS
{
  // Биты данного канала предназначены для управления выходными каналами модуля. 
  // Логическая 1 соответствует включению канала.
	uint8_t digitalOutputs;

  // Длительность первой полуволны ШИМ-последовательности на 1-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, 
  // должно быть больше либо равным 20 (соответствует примерно 1 мс)
  uint16_t firstHalfDuty_PWM0;
	// Длительность второй полуволны ШИМ-последовательности 1-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, 
  // должно быть больше либо равным 60 (соответствует примерно 3 мс)
  uint16_t secondHalfDuty_PWM0;

  // Длительность первой полуволны ШИМ-последовательности на 2-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, 
  // должно быть больше либо равным 20 (соответствует примерно 1 мс)
	uint16_t firstHalfDuty_PWM1;
	// Длительность второй полуволны ШИМ-последовательности 2-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, 
  // должно быть больше либо равным 60 (соответствует примерно 3 мс)
	uint16_t secondHalfDuty_PWM1;

  // Длительность первой полуволны ШИМ-последовательности на 3-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, 
  // должно быть больше либо равным 20 (соответствует примерно 1 мс)
  uint16_t firstHalfDuty_PWM2;
	// Длительность второй полуволны ШИМ-последовательности 3-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, 
  // должно быть больше либо равным 60 (соответствует примерно 3 мс)
	uint16_t secondHalfDuty_PWM2;

  // Длительность первой полуволны ШИМ-последовательности на 4-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, 
  // должно быть больше либо равным 20 (соответствует примерно 1 мс)
  uint16_t firstHalfDuty_PWM3;
	// Длительность второй полуволны ШИМ-последовательности 4-м канале. 
  // Задается с в единицах с весом 50 мкс. Значение, передаваемое в канал, 
  // должно быть больше либо равным 60 (соответствует примерно 3 мс)
	uint16_t secondHalfDuty_PWM3;
});

PACKED_STRUCT(
struct _DIM763
{
	DIM763_INPUTS inputs;
	DIM763_OUTPUTS outputs;
	DIM763_CONFIGURATION configuration;
});

//--- DIM764-0-0
typedef struct _DIM764_0_0 DIM764_0_0;
typedef struct _DIM764_0_0_INPUTS DIM764_0_0_INPUTS;
typedef struct _DIM764_0_0_OUTPUTS DIM764_0_0_OUTPUTS;
typedef struct _DIM764_0_0_CHANNEL_CFG DIM764_0_0_CHANNEL_CFG;
typedef struct _DIM764_0_0_CONFIGURATION DIM764_0_0_CONFIGURATION;

// Число физических дискретных каналов модуля DIM764-0-0
#define DIM764_0_0_CHANNELS_COUNT 8

// Частота дискретизации фильтра, применяемого к сигналам на входных каналах
// Возможные значения:
#define DIM764_0_0_FILTER_OPTIONS_50MHz  0  // 50 МГц
#define DIM764_0_0_FILTER_OPTIONS_5MHz   1  // 5 МГц
#define DIM764_0_0_FILTER_OPTIONS_2_5MHz 2  // 2.5 МГц

// Режим работы дискретного канала
// Возможные значения:
//  Измерение периода входного сигнала
#define DIM764_0_0_INPUT_MODE_PERIOD    0
//  Измерение интервала между двумя последовательными импульсами входного сигнала
#define DIM764_0_0_INPUT_MODE_INTERVAL  1
//  Измерение сдвига фазы сигнала в паре ведущий-ведомый. 
//  Данный режим доступен только у ведомого канала. 
//  Ведущий должен быть настроен на режим измерения периода.
#define DIM764_0_0_INPUT_MODE_PHASE     2
//  Счётчик импульсов
#define DIM764_0_0_INPUT_MODE_COUNTER   3
//  Канал отключен
#define DIM764_0_0_INPUT_MODE_DISABLED  4

// Активный фронт / либо уровень входного сигнала 
// Возможные значения:
//  передний фронт/высокий уровень
#define DIM764_0_0_EDGE_RISING  0
//  задний фронт/низкий уровень
#define DIM764_0_0_EDGE_FALLING 1

// Режим счета для ведущего канала
// Возможные значения:
//  режим условного счета выключен
#define DIM764_0_0_ABSOLUTE_COUNTER     0
//  режим условного счета включен
#define DIM764_0_0_CONDITIONAL_COUNTER  1

PACKED_STRUCT(
struct _DIM764_0_0_CHANNEL_CFG
{
  // Режим работы дискретного канала
  // Возможные значения:   смотри DIM764_0_0_INPUT_MODE macros
  uint8_t  inputMode;

  // Настройки параметров: условного счета, фильтра, входного сигнала:
  //  bit0: Выключает/Включает режим условного счета на канале.
  //        Устанавливается только для ведущего канала (0,2,4,6)
  //        =DIM764_0_0_ABSOLUTE_COUNTER(0)    -- режим условного счета выключен
  //        =DIM764_0_0_CONDITIONAL_COUNTER(1) -- режим условного счета включен
  //  bit1: Выключает/Включает режим фильтрация входного сигнала
  //        =0 -- фильтрация выключена
  //        =1 -- фильтрация включена
  //  bit2: Устанавливает активный фронт/уровень входного сигнала для ведомого канала
  //        =DIM764_0_0_EDGE_RISING(0) -- передний фронт/высокий уровень
  //        =DIM764_0_0_EDGE_FALLING(1) -- задний фронт/низкий уровень
  uint8_t  paramsMask;
});

// Область конфигурационных параметров модуля DIM764-0-0
PACKED_STRUCT(
struct _DIM764_0_0_CONFIGURATION
{
  // Код типа прошивки FPGA. Должен быть 0!
  uint8_t version;
  // Код исполнения прошивки FPGA. Должен быть 0!
  uint8_t subVersion;
  // Параметр: “Частота фильтра”
  uint8_t filterOption;
  // Возможные значения:
  //  DIM764_0_0_FILTER_OPTIONS_50MHz  (0):   50 МГц
  //  DIM764_0_0_FILTER_OPTIONS_5MHz   (1):   5 МГц
  //  DIM764_0_0_FILTER_OPTIONS_2_5MHz (2):   2.5 МГц

  // Настройки каналов 1…8 (задаются отдельно по каждому каналу)
  DIM764_0_0_CHANNEL_CFG channelsConfig[DIM764_0_0_CHANNELS_COUNT];
});

// Область входных параметров "Inputs" модуля DIM764-0-0
PACKED_STRUCT(
struct _DIM764_0_0_INPUTS
{
  // Диагностический канал
  uint8_t  diagnostics;
  // Значение FFh свидетельствует об отсутствии связи с модулем.
  // В противном случае служит для индикации состояния загрузки и конфигурации FPGA:
  //  0 - свидетельствует о нормальном функционировании FPGA;
  //  1 – инициализация записи firmware в FPGA;
  //  2 – запись firmware в FPGA;
  //  3 – подготовка к запуску FPGA;
  //  4 – FPGA не сконфигурирована;
  //  5 – не найдено требуемое firmware для FPGA

  // Контрольный код последнего обновления выходов модуля.
  // Смотри поле "control" в структуре выходов модуля
  uint8_t  controlState;

  // Состояние каналов. Двоичная единица в битовой позиции, соответствующей номеру входа, 
  // сигнализирует о неопределенном состоянии канала: в режимах измерения периода, интервала 
  // и сдвига фазы при частоте входного сигнала от 0,000 до 0,763 Гц
  uint8_t channelsState;

  // Массив значений на каналах. Ниже описывается назначение пары элементов массива values 
  // для смежных каналов с номерами 0..1. Назначение остальных пар (2..3; 4..5; 6..7) аналогично
  uint32_t values[DIM764_0_0_CHANNELS_COUNT];
  // values[0]: 
  //   В режиме счетчика импульсов: количество импульсов на канале;
  //   В режиме измерения периода: количество импульсов частоты заполнения 50 МГц,
  //   укладывающихся в период входного сигнала;
  //   В режиме измерения интервала: количество импульсов частоты заполнения 50 МГц, 
  //   укладывающихся между соседними фронтами входного сигнала;
  //   В режиме измерения сдвига фазы: значение на данном канале используется в качестве 
  //   делителя при вычислении сдвига фазы в градусах;
  // values[1]: 
  //   В режимах измерения периода, интервала и счета импульсов: соответствует тому, 
  //   что указано для values[0];
  //   В режиме измерения фазового сдвига: значение сдвига между сигналами на каналах 0 и 1 
  //   вычисляется по формуле: 
  //   double phaseShift = 360.0 * ((double)values[1])/ ((double)values[0]);
});

// Область выходных параметров "Outputs" модуля DIM764-0-0
PACKED_STRUCT(
struct _DIM764_0_0_OUTPUTS
{
  // Контрольный код обновления данных выходов. 
  // Должен быть равен: controlState + 1.
  // Смотри поле "controlState" в структуре входов модуля
  uint8_t control;

  // Команда сброса счётчиков каналов (только для каналов функционирующих в режиме счёта 
  // входных импульсов).  Двоичная единица в битовой позиции, соответствующей номеру 
  // дискретного канала, сбрасывает счётчик данного канала. 
  // При формировании очередной команды сброса канал control должен содержать значение, 
  // равное controlState + 1
  uint8_t resetCounters;
});

PACKED_STRUCT(
struct _DIM764_0_0
{
  DIM764_0_0_INPUTS         inputs;
  DIM764_0_0_OUTPUTS        outputs;
  DIM764_0_0_CONFIGURATION  configurations;
});

//--- DIM765 / DIM766
typedef struct _DIM765 DIM765;
typedef struct _DIM766 DIM766;
typedef struct _DIM7656_INPUTS DIM7656_INPUTS;
typedef struct _DIM7656_CONFIGURATION DIM7656_CONFIGURATION;

//--- Режим работы канала и номинал шунтирующего резистора
// Возможные значения:
//   Без контроля цепи– контроль обрыва цепи не осуществляется. 
//   Оценка состояния входной цепи может быть произведена с использованием 
//   показаний каналов измерения напряжения и тока
#define DIM7656_Type_No_disconnect_detection   0
//   Тип 1: шунт <...> кОм – для канала задан режим входа типа 1, 
//   и  параллельно датчику, подключенному к каналу, 
//   включен шунт заданного номинала.
#define DIM7656_Type_1_shunt_1_8K         1   // Тип 1: шунт 1.8 кОм
#define DIM7656_Type_1_shunt_2_2K         2   // Тип 1: шунт 2.2 кОм
#define DIM7656_Type_1_shunt_2_7K         3   // Тип 1: шунт 2.7 кОм
#define DIM7656_Type_1_shunt_3_3K         4   // Тип 1: шунт 3.3 кОм
#define DIM7656_Type_1_shunt_3_9K         5   // Тип 1: шунт 3.9 кОм
#define DIM7656_Type_1_shunt_4_7K         6   // Тип 1: шунт 4.7 кОм
#define DIM7656_Type_1_shunt_5_6K         7   // Тип 1: шунт 5.6 кОм
#define DIM7656_Type_1_shunt_6_8K         8   // Тип 1: шунт 6.8 кОм
#define DIM7656_Type_1_shunt_8_2K         9   // Тип 1: шунт 8.2 кОм
#define DIM7656_Type_1_shunt_10K          10  // Тип 1: шунт 10 кОм
#define DIM7656_Type_1_shunt_12K          11  // Тип 1: шунт 12 кОм
#define DIM7656_Type_1_shunt_15K          12  // Тип 1: шунт 15 кОм
#define DIM7656_Type_1_shunt_18K          13  // Тип 1: шунт 18 кОм
#define DIM7656_Type_1_shunt_20K          14  // Тип 1: шунт 20 кОм
#define DIM7656_Type_1_shunt_22K          15  // Тип 1: шунт 22 кОм
#define DIM7656_Type_1_shunt_24K          16  // Тип 1: шунт 24 кОм
#define DIM7656_Type_1_shunt_27K          17  // Тип 1: шунт 27 кОм
#define DIM7656_Type_1_shunt_30K          18  // Тип 1: шунт 30 кОм
#define DIM7656_Type_1_shunt_33K          19  // Тип 1: шунт 33 кОм
//   Тип 2: без шунта – для канала установлен режим входа типа 2 
//   и предполагается подключить датчик с током утечки в выключенном 
//   состоянии от 0,25 до 1,50 мА (для DIM765) или от 0,25 до 1,50 мА (для DIM766).
#define DIM7656_Type_2_no_shunt           255

//--- Задержка включения/выключения канала
// Возможные значения:
#define DIM7656_Delay_no          0   // Без задержки
#define DIM7656_Delay_1_4_ms      1   // Задержка 1.4 мс
#define DIM7656_Delay_2_8_ms      2   // Задержка 2.8 мс
#define DIM7656_Delay_4_2_ms      3   // Задержка 4.2 мс
#define DIM7656_Delay_9_8_ms      4   // Задержка 9.8 мс
#define DIM7656_Delay_20_ms       5   // Задержка 20 мс
#define DIM7656_Delay_50_ms       6   // Задержка 50 мс
#define DIM7656_Delay_100_ms      7   // Задержка 100 мс
#define DIM7656_Delay_200_ms      8   // Задержка 200 мс
#define DIM7656_Delay_300_ms      9   // Задержка 300 мс

// Области конфигурационных параметров модулей DIM765, DIM766
PACKED_STRUCT(
struct _DIM7656_CONFIGURATION
{
  // shunt0..7 -- Параметр "Шунт канала 1…8"
  // Определяет режим работы канала 1…8 и номинал шунтирующего резистора.
  // Возможные значения: смотри макроопределения DIM7656_Type_XXXXX 
	uint8_t shunt0;
	uint8_t shunt1;
	uint8_t shunt2;
	uint8_t shunt3;
	uint8_t shunt4;
	uint8_t shunt5;
	uint8_t shunt6;
	uint8_t shunt7;

  // switchOnDelay0..7 -- Параметр "Задержка включения канала 1…8"
  // Определяет интервал времени, в течение которого в цепи канала должны сохраняться условия, 
  // определяющие уровень логической «1».
  // Возможные значения: смотри макроопределения DIM7656_Delay_XXXXX 
	uint8_t switchOnDelay0;
	uint8_t switchOnDelay1;
	uint8_t switchOnDelay2;
	uint8_t switchOnDelay3;
	uint8_t switchOnDelay4;
	uint8_t switchOnDelay5;
	uint8_t switchOnDelay6;
	uint8_t switchOnDelay7;

  // switchOffDelay0..7 -- Параметр "Задержка выключения канала 1…8"
  // Определяет интервал времени, в течение которого в цепи канала должны сохраняться условия, 
  // определяющие уровень логического «0».
  // Возможные значения: смотри макроопределения DIM7656_Delay_XXXXX 
	uint8_t switchOffDelay0;
	uint8_t switchOffDelay1;
	uint8_t switchOffDelay2;
	uint8_t switchOffDelay3;
	uint8_t switchOffDelay4;
	uint8_t switchOffDelay5;
	uint8_t switchOffDelay6;
	uint8_t switchOffDelay7;
});

// Области входных параметров "Inputs" модулей DIM765, DIM766
PACKED_STRUCT(
struct _DIM7656_INPUTS
{
  // Диагностический виртуальный канал модуля. 
  // Логическая единица в младшем бите означает отсутствие напряжения питания от 15 до 30 В 
  // на ножах распределения питания модуля либо неисправность измерительного тракта входных цепей. 
  // Равенство 255 означает отсутствие связи с модулем по шине.
	uint8_t diagnostics;

  // Биты данного канала отражают статус обнаружения обрыва цепи подключения датчиков входных каналов модуля. 
  // Логическая «1» в некотором разряде означает обрыв цепи соответствующего канала.
  uint8_t wireBreaks;

  // Текущее логическое состояние входных каналов. 
  // При логической «1» в младшем разрядке канала diagnostics канал недействителен.
  // При наличии установленной логической «1» в некотором разряде wireBreaks, 
  // соответствующий бит channelsState недействителен.
	uint8_t channelsState;

  // Код текущего значения напряжения во входной цепи каналов с номерами от 1 до 8 соответственно.
  //  Значение напряжения вычисляется по формуле:
  //  V = Vcode * 7,4655 (мВ),
  //  где Vcode – код на канале.
  //  V для DIM765 – падение напряжения между выходом источника питания датчика и входом модуля;
  //  V для DIM766 – падение напряжения между входом модуля и общим проводом источника питания датчика.
  uint16_t v0;
	uint16_t v1;
	uint16_t v2;
	uint16_t v3;
	uint16_t v4;
	uint16_t v5;
	uint16_t v6;
	uint16_t v7;

  // Код текущего значения тока во входной цепи каналов с номерами от 1 до 8 соответственно.
  //  Значение тока вычисляется по формуле:
  //  DIM765: I = Icode * 4,0161 (мкА)
  //  DIM766: I = Icode * 3,7422 (мкА)
  //  где Icode – код на канале
  uint16_t i0;
	uint16_t i1;
	uint16_t i2;
	uint16_t i3;
	uint16_t i4;
	uint16_t i5;
	uint16_t i6;
	uint16_t i7;
});

PACKED_STRUCT(
struct _DIM765
{
	DIM7656_INPUTS inputs;
	DIM7656_CONFIGURATION configuration;
});

PACKED_STRUCT(
struct _DIM766
{
	DIM7656_INPUTS inputs;
	DIM7656_CONFIGURATION configuration;
});

//--- NIM741 / NIM742
typedef struct _NIM741 NIM741;
typedef struct _NIM742 NIM742;
typedef struct _NIM74X_INPUTS NIM74X_INPUTS;
typedef struct _NIM74X_OUTPUTS NIM74X_OUTPUTS;
typedef struct _NIM74X_CONFIGURATION NIM74X_CONFIGURATION;

//--- Скорость обмена
#define UART_BAUDRATE__300BPS	    0   // 300 бит/с
#define UART_BAUDRATE__1200BPS	  1   // 1200 бит/с
#define UART_BAUDRATE__2400BPS	  2   // 2400 бит/с
#define UART_BAUDRATE__4800BPS	  3   // 4800 бит/с
#define UART_BAUDRATE__9600BPS	  4   // 9600 бит/с
#define UART_BAUDRATE__14400BPS	  5   // 14400 бит/с
#define UART_BAUDRATE__19200BPS	  6   // 19200 бит/с
#define UART_BAUDRATE__38400BPS	  7   // 38400 бит/с
#define UART_BAUDRATE__57600BPS	  8   // 57600 бит/с
#define UART_BAUDRATE__115200BPS	9   // 115200 бит/с

//--- Количество бит данных
#define UART_DATABITS__8BITS	0   // 8  бит
#define UART_DATABITS__7BITS	1   // 7  бит

//--- Количество стоповых бит
#define UART_STOPBITS__1BIT	  0   // 1  бит
#define UART_STOPBITS__2BITS	1   // 2  бита

//--- Четность
#define UART_PARITY_NONE	    0   // None 
#define UART_PARITY_EVEN	    1   // Even
#define UART_PARITY_ODD	      2   // Odd

// Области конфигурационных параметров модулей NIM741, NIM742
PACKED_STRUCT(
struct _NIM74X_CONFIGURATION
{
  // Зарезервировано!
  // Значение: Должно быть равным 0!
	uint8_t mode;

  // Скорость обмена
  // Возможные значения: смотри макроопределения UART_BAUDRATE__XXXXX 
	uint8_t baudrate;
  
  // Количество бит данных
  // Возможные значения: смотри макроопределения UART_DATABITS__XXXXX 
	uint8_t databits;

  // Количество стоповых бит
  // Возможные значения: смотри макроопределения UART_STOPBITS__XXXXX 
  uint8_t stopbits;

  // Четность
  // Возможные значения: смотри макроопределения UART_PARITY_XXXXX 
  uint8_t parity;
});

// Области входных параметров "Inputs" модулей NIM741, NIM742
PACKED_STRUCT(
struct _NIM74X_INPUTS
{
  // Канал диагностики.
  // Значение FFh свидетельствует об отсутствии связи с модулем
	uint8_t diagnostics;
  // Битовая маска статусных признаков модуля:
  //  Бит0: Статус буфера приема: =0 – нет данных; =1 – есть данные
  //  Бит1: Статус буфера передачи: =0 – нет данных; =1 – есть данные
  //  Бит2: Признак переполнения буфера приема:  =0 – нет переполнения; =1 – есть переполнение
  //  Бит3: Признак переполнения буфера передачи: =0 – нет переполнения; =1 – есть переполнение
  //  Бит4: Признак переполнения UART: =0 – нет переполнения; =1 – микропрограмма модуля 
  //        не успевает извлекать данные из UART
  //  Бит5: Признак ошибки кадра: =1 – Frame Error
  //  Бит6: Признак ошибки четности: =1 – Parity Error
  //  Бит7: Бит синхронизации команд управления. Последнее значение бита управления передачей команд модулю
  //        (Бит0 в канале управления модулем control), полученное модулем от приложения, для которого
  //        модуль выполнил команду управления
  //  Бит8...15: Последнее значение tx_Control области выходных данных,                
  //             полученное модулем, для записи данных в буфер передачи
	uint16_t status;
  // Количество байт в буфере приема модуля
	uint16_t FIFOLength;
  // Канал статуса управления передачей данных из приемного буфера. 
  // Модуль при очередной передаче данных из буфера приема в массив rx_Data 
  // изменяет значение rx_Control (увеличивает по кругу)
	uint8_t rx_Control;
  // Количество байт данных передаваемых модулем из внутреннего буфера приема в текущем цикле обмена по FBUS. 
  // Диапазон значений: от 0 до 32. При нулевом значении программе пользователя не требуется забирать данные из
  // окна-массива rx_Data[32].
	uint8_t rx_Length;
  // Окно для передачи данных из внутреннего буфера приема модуля. 
  // Количество байт, передаваемых модулем в окне в текущей транзакции, 
  // определяется значением rx_Length.
	uint8_t rx_Data[32];
});

// Области выходных параметров "Outputs" модулей NIM741, NIM742
PACKED_STRUCT(
struct _NIM74X_OUTPUTS
{
  // Битовая маска команд управления
  //  Бит0: Бит управления передачей команд модулю. Изменение данного бита прикладной программой 
  //        сигнализирует модулю о том, что он должен выполнить команды управления. 
  //        Признаки команд содержатся в битах Бит1…15 данного канала. 
  //        После выполнения команд, модуль копирует его значение в бит синхронизации команд управления 
  //        статусного канала (бит 7 поля status области входных данных).
  //  Бит1: Признак команды передачи данных из FIFO приема модуля в область rx_Data[32]. 
  //        Если модуль определяет поступление новой команды управления по изменению бита 0 поля control, 
  //        и данный бит установлен, модуль переписывает данные из FIFO приема в rx_Data[32] и устанавливает 
  //        новое значение rx_Length. В противном случае содержимое rx_Data[32] и rx_Length  не изменяется
  //  Бит2: Признак команды запрещения/разрешения передачи данных из FIFO передачи в UART. 
  //        Если модуль определяет поступление новой команды управления по изменению бита 0 поля control, 
  //        производится анализ значения данного бита. Если он установлен, передача исходящих данных из 
  //        FIFO передачи в UART запрещается, а если сброшен – разрешается
  //  Бит3: Признак команды сброса FIFO приема. Если модуль определяет поступление новой команды управления 
  //        (изменен бит управления передачей команд – бит0 поля control) и данный бит установлен, 
  //        модуль удаляет из буфера приема все принятые на данный момент данные.
  //  Бит4: Признак команды сброса FIFO передачи. Если модуль определяет поступление новой команды управления 
  //        (изменен бит управления передачей команд – бит0 поля control) и данный бит установлен, 
  //        модуль удаляет из FIFO передачи все находящиеся в нем на данный момент данные.
  //  Бит5..15: Резерв
	uint16_t control;
  // Канал управления записью данных в буфер передачи. 
  // Содержит значение счетчика, изменение которого сигнализирует модулю о том, 
  // что он должен взять байты из массива tx_Data в количестве, заданном полем tx_Length, 
  // и записать в FIFO передачи
	uint8_t tx_Control;
  // Количество байт, передаваемых модулю в  массиве tx_Data для записи в FIFO передачи
	uint8_t tx_Length;
  // Данные для записи в FIFO передачи. Количество «полезных» байт определяется значением tx_Length
	uint8_t tx_Data[32];
});

PACKED_STRUCT(
struct _NIM741
{
	NIM74X_INPUTS inputs;
	NIM74X_OUTPUTS outputs;
	NIM74X_CONFIGURATION configuration;
});

PACKED_STRUCT(
struct _NIM742
{
	NIM74X_INPUTS inputs;
	NIM74X_OUTPUTS outputs;
	NIM74X_CONFIGURATION configuration;
});

//--- OM751
typedef struct _OM751 OM751;
typedef struct _OM751_INPUTS OM751_INPUTS;

// Область входных параметров "Inputs" модуля OM751
PACKED_STRUCT(
struct _OM751_INPUTS
{
  // Диагностический виртуальный канал модуля. В модуле физически не реализован. 
  // Значение FFh свидетельствует об отсутствии связи с модулем. 
	uint8_t diagnostics;
  // Битовая маска статусных каналов
  //   Бит 0: =1: нет/сгорел предохранитель, уровень входного напряжения ниже 12 В
  //   Бит 1: =1: предохранитель исправен и уровень входного напряжения выше 15 В
  //   Бит 2: =1: уровень входного напряжения выше 30 В
	uint8_t inputStates;
});

PACKED_STRUCT(
struct _OM751
{
	OM751_INPUTS inputs;
});

/*-------------------------------------------------------------------------*/
/* --- Определения типов FBUS SDK 2.2 -------------------------------------*/ 

// Сообщение о недопустимом типе.
// Типы некоторых полей определенные в DEPRICATED_INPUTS_TYPE структуре 
// могут неправильнол представлять типы данных соответствующих им 
// входных каналов модуля.
#define DEPRICATED_INPUTS_TYPE Depricated_Inputs_Type_Please_Use__ModuleTypeName_INPUTS

// Может использоваться в качестве общего типа для представления 
// области входных каналов модулей: DIM712, DIM713, DIM715, OM751 
PACKED_STRUCT(
struct _DIM_INPUTS
{
	uint8_t diagnostics;
	uint8_t inputStates;
});
typedef struct _DIM_INPUTS DIM_INPUTS;

// Рекомендуется использовать новые определения
#ifndef ENABLE_DEPRICATED_2_2_TYPES

# define _AIM72X_2_INPUTS    DEPRICATED_INPUTS_TYPE
# define AIM72X_2_INPUTS     DEPRICATED_INPUTS_TYPE

#else

// ВНИМАНИЕ! 
// В модулях AIM722, AIM729 входные каналы input0, input1 
// содержат значения типа int32_t!
// AIM72X_2_INPUTS Inputs;
// int32_t AIM722_input0 = *((int32_t*)&(Inputs.input0));
PACKED_STRUCT(
struct _AIM72X_2_INPUTS
{
	uint8_t diagnostics;
	uint32_t input0;
	uint32_t input1;
});
typedef struct _AIM72X_2_INPUTS AIM72X_2_INPUTS;

#endif

// Может использоваться в качестве общего типа для представления 
// области конфигурационных параметров модулей: AIM722, AIM726, AIM729
PACKED_STRUCT(
struct _AIM72X_2_CONFIGURATION
{
	uint8_t filterOptions0;
	uint8_t filterOptions1;
});
typedef struct _AIM72X_2_CONFIGURATION AIM72X_2_CONFIGURATION;

// Рекомендуется использовать новые определения
#ifndef ENABLE_DEPRICATED_2_2_TYPES

# define _AIM72X_4_INPUTS    DEPRICATED_INPUTS_TYPE
# define AIM72X_4_INPUTS     DEPRICATED_INPUTS_TYPE

#else

// ВНИМАНИЕ! 
// В модулях AIM721, AIM723, AIM728 входные каналы input0, input1, input2, input3 
// содержат значения типа int32_t!
// AIM72X_4_INPUTS Inputs;
// int32_t AIM728_input0 = *((int32_t*)&(Inputs.input0));
PACKED_STRUCT(
struct _AIM72X_4_INPUTS
{
	uint8_t diagnostics;
	uint32_t input0;
	uint32_t input1;
	uint32_t input2;
	uint32_t input3;
});
typedef struct _AIM72X_4_INPUTS AIM72X_4_INPUTS;

#endif

// Может использоваться в качестве общего типа для представления 
// области конфигурационных параметров модулей: AIM721, AIM723, AIM727, AIM728 
PACKED_STRUCT(
struct _AIM72X_4_CONFIGURATION
{
	uint8_t filterOptions0;
	uint8_t filterOptions1;
	uint8_t filterOptions2;
	uint8_t filterOptions3;
});
typedef struct _AIM72X_4_CONFIGURATION AIM72X_4_CONFIGURATION;

// Может использоваться в качестве общего типа для представления 
// области входных каналов модулей: DIM716, DIM714, DIM717, DIM760, DIM761, DIM762 
PACKED_STRUCT(
struct _DIM_INPUTS_COUNTERS
{
	uint8_t diagnostics;
	uint8_t inputStates;
	uint16_t counter0;
	uint16_t counter1;
	uint8_t countersState;
});
typedef struct _DIM_INPUTS_COUNTERS DIM_INPUTS_COUNTERS;

// Может использоваться в качестве общего типа для представления 
// области конфигурационных параметров модулей: DIM716, DIM714, DIM717, DIM760, DIM761, DIM762
PACKED_STRUCT(
struct _DIM_CONFIGURATION
{
	uint8_t debounce;
	uint8_t enableCounting;
	COUNTER_INPUT_SETTINGS countingParameters[2];
});
typedef struct _DIM_CONFIGURATION DIM_CONFIGURATION;

// Может использоваться в качестве общего типа для представления 
// области выходных каналов модулей: DIM713, DIM712
PACKED_STRUCT(
struct _DIM_OUTPUTS
{
	uint8_t outputsControl;
});
typedef struct _DIM_OUTPUTS DIM_OUTPUTS;

// Может использоваться в качестве общего типа для представления 
// области конфигурационных параметров модулей: DIM713, DIM712
PACKED_STRUCT(
struct _DIGITAL_OUTPUT_CONFIGURATION
{
	uint8_t initialOutputStates;
	uint8_t outputSafeStates;
});
typedef struct _DIGITAL_OUTPUT_CONFIGURATION DIGITAL_OUTPUT_CONFIGURATION;

#ifndef ENABLE_DEPRICATED_2_2_TYPES

// Рекомендуется использовать новые определения
# define _AIM724_5_INPUTS    DEPRICATED_INPUTS_TYPE
# define AIM724_5_INPUTS     DEPRICATED_INPUTS_TYPE

#else

// ВНИМАНИЕ! 
// В модуле AIM724 входные каналы channel0, channel1, cjcInput 
// содержат значения типа float!
// AIM724_5_INPUTS Inputs;
// float temperature0 = *((float*)&(Inputs.channel0));
// float temperature1 = *((float*)&(Inputs.channel1));
// float temperatureCJC = *((float*)&(Inputs.cjcInput));
PACKED_STRUCT(
struct _AIM724_5_INPUTS
{
	uint8_t diagnostics;
	uint32_t channel0;
	uint32_t channel1;
	uint32_t cjcInput;
});
typedef struct _AIM724_5_INPUTS AIM724_5_INPUTS;

#endif

#ifndef ENABLE_DEPRICATED_2_2_TYPES

// Рекомендуется использовать новые определения
# define _AIM724_5_OUTPUTS    DEPRICATED_INPUTS_TYPE
# define AIM724_5_OUTPUTS     DEPRICATED_INPUTS_TYPE

#else

// ВНИМАНИЕ! 
// В модуле AIM724 в выходной каналы cjcExternalSource 
// необходимо записывать значения типа float!
// AIM724_5_OUTPUTS Outputs;
// float cjcExt;
// *((float*)&(Outputs.cjcExternalSource)) = cjcExt;
PACKED_STRUCT(
struct _AIM724_5_OUTPUTS
{
	uint32_t cjcExternalSource;
});
typedef struct _AIM724_5_OUTPUTS AIM724_5_OUTPUTS;

#endif

// Может использоваться в качестве общего типа для представления 
// области входных параметров модулей: AIM730, AIM731
PACKED_STRUCT(
struct _AIM73X_INPUTS
{
	uint8_t diagnostics;
	uint16_t outputValue0;
	uint16_t outputValue1;
});
typedef struct _AIM73X_INPUTS AIM73X_INPUTS;

// Может использоваться в качестве общего типа для представления 
// области выходных параметров модулей: AIM730, AIM731
PACKED_STRUCT(
struct _AIM73X_OUTPUTS
{
	uint16_t output0;
	uint16_t output1;
});
typedef struct _AIM73X_OUTPUTS AIM73X_OUTPUTS;

// Может использоваться в качестве общего типа для представления 
// области входных параметров модулей: AIM791, AIM792
PACKED_STRUCT(
struct _AIM7912_INPUTS
{
	uint8_t diagnostics;
	uint16_t channelRanges;
	uint16_t channelsStatus;
	uint16_t values[8];
});
typedef struct _AIM7912_INPUTS AIM7912_INPUTS;

// Может использоваться в качестве общего типа для представления 
// области конфигурационных параметров модулей: AIM791, AIM792
PACKED_STRUCT(
struct _AIM7912_CONFIGURATION
{
	uint8_t inputRange;
	uint8_t scanRate;
	uint8_t filterDepth;
	uint16_t lowLimit[8];
	uint16_t highLimit[8];
	uint8_t channelRanges[8];
});
typedef struct _AIM7912_CONFIGURATION AIM7912_CONFIGURATION;

/*-------------------------------------------------------------------------*/
#ifdef AFFECT_FRAMEWORK
# include <oss/pack.h>
#endif

#endif
