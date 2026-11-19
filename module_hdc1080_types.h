#ifndef _MODULE_HDC1080_TYPES_h
#define _MODULE_HDC1080_TYPES_h

// ============================================================
// module_hdc1080_types.h — адрес, регистры и константы HDC1080.
// Реализация: module_hdc1080.cpp (шаблон) и module_hdc1080_engine.cpp
// (I2C, измерение, периодический опрос).
// ============================================================

#include <Arduino.h>
#include <stdint.h>

// Адрес HDC1080 на шине I2C (фиксированный)
#define HDC1080_I2C_ADDR                0x40

// Регистры HDC1080
#define HDC1080_REG_TEMPERATURE         0x00
#define HDC1080_REG_HUMIDITY            0x01
#define HDC1080_REG_CONFIG              0x02
#define HDC1080_REG_MANUFACTURER_ID     0xFE
#define HDC1080_REG_DEVICE_ID           0xFF

// Идентификаторы
#define HDC1080_MANUFACTURER_ID_VALUE   0x5449
#define HDC1080_DEVICE_ID_VALUE         0x1000
#define HDC1080_DEVICE_ID_FAMILY        0x1000   // старший байт: 0x1000 / 0x1050 (клоны)

// Режим: 14 бит, heater off (T и RH измеряются отдельными запросами)
#define HDC1080_CONFIG_VALUE            0x0000

// Период опроса и время преобразования, мс
#define HDC1080_POLL_MS                 1000
#define HDC1080_CONVERSION_MS           20

#endif // _MODULE_HDC1080_TYPES_h
