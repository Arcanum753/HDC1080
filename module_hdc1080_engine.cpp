#include "core_web/FSWebServerLib.h"

#include "core_state/core_state.h"

#include "module_hdc1080.h"

#include <Wire.h>

// ============================================================
// I2C — низкоуровневые операции
// ============================================================

bool CLASS_MODULE_HDC1080::readRegs(uint8_t reg, uint8_t *buf, uint8_t len) {
    if (_addr == 0) { return false; }

    Wire.beginTransmission(_addr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) { return false; }
    if (Wire.requestFrom((int)_addr, (int)len) != (int)len) { return false; }

    for (uint8_t i = 0; i < len; i++) {
        if (Wire.available() < 1) { return false; }
        buf[i] = (uint8_t)Wire.read();
    }
    return true;
}

bool CLASS_MODULE_HDC1080::writeRegs(uint8_t reg, const uint8_t *buf, uint8_t len) {
    if (_addr == 0) { return false; }

    Wire.beginTransmission(_addr);
    Wire.write(reg);
    for (uint8_t i = 0; i < len; i++) { Wire.write(buf[i]); }
    return (Wire.endTransmission() == 0);
}

// ============================================================
// Поиск и инициализация датчика
// ============================================================

bool CLASS_MODULE_HDC1080::detect() {
    if (_addr == 0) { _addr = HDC1080_I2C_ADDR; }

    Wire.beginTransmission(_addr);
    if (Wire.endTransmission() != 0) {
        _addr = 0;
        return false;
    }

    uint8_t m[2];
    uint8_t d[2];
    if (!readRegs(HDC1080_REG_MANUFACTURER_ID, m, 2) ||
        !readRegs(HDC1080_REG_DEVICE_ID, d, 2)) {
        _addr = 0;
        return false;
    }

    _mfr = ((uint16_t)m[0] << 8) | (uint16_t)m[1];
    _dev = ((uint16_t)d[0] << 8) | (uint16_t)d[1];

    // Встречаются совместимые клоны HDC1080 с device id 0x1050 (вместо 0x1000).
    // Manufacturer ID у них совпадает (0x5449), поэтому проверяем семейство 0x10xx.
    bool idOk = (_mfr == HDC1080_MANUFACTURER_ID_VALUE) &&
                ((_dev & 0xFF00) == HDC1080_DEVICE_ID_FAMILY);
    if (!idOk) {
        DEBUGHDC1080("unexpected IDs: mfr=0x%04X dev=0x%04X\r\n", _mfr, _dev);
        _addr = 0;
        return false;
    }

    DEBUGHDC1080("HDC1080 found at 0x%02X\r\n", _addr);
    return true;
}

bool CLASS_MODULE_HDC1080::initChip() {
    // 0x0000: последовательное измерение T+RH, 14 бит, heater off.
    uint8_t cfg[2];
    cfg[0] = (uint8_t)((HDC1080_CONFIG_VALUE >> 8) & 0xFF);
    cfg[1] = (uint8_t)(HDC1080_CONFIG_VALUE & 0xFF);

    if (!writeRegs(HDC1080_REG_CONFIG, cfg, 2)) {
        DEBUGHDC1080("config write failed\r\n");
        return false;
    }
    return true;
}

// ============================================================
// Измерение
// ============================================================

bool CLASS_MODULE_HDC1080::readMeasurement(uint16_t &rawTemp, uint16_t &rawHum) {
    if (_addr == 0) { return false; }

    // Измеряем температуру и влажность отдельными запросами: клоны HDC1080
    // (например device id 0x1050) в последовательном режиме отдают RH как 0xFFFF.
    Wire.beginTransmission(_addr);
    Wire.write(HDC1080_REG_TEMPERATURE);
    if (Wire.endTransmission() != 0) { return false; }

    delay(HDC1080_CONVERSION_MS);

    if (Wire.requestFrom((int)_addr, 2) != 2) { return false; }
    if (Wire.available() < 2) { return false; }
    rawTemp = ((uint16_t)Wire.read() << 8);
    rawTemp |= (uint16_t)Wire.read();

    Wire.beginTransmission(_addr);
    Wire.write(HDC1080_REG_HUMIDITY);
    if (Wire.endTransmission() != 0) { return false; }

    delay(HDC1080_CONVERSION_MS);

    if (Wire.requestFrom((int)_addr, 2) != 2) { return false; }
    if (Wire.available() < 2) { return false; }
    rawHum = ((uint16_t)Wire.read() << 8);
    rawHum |= (uint16_t)Wire.read();

    return true;
}

void CLASS_MODULE_HDC1080::publish(bool available, float temperature, float humidity) {
    _available = available;
    if (available) {
        _temperature = temperature;
        _humidity    = humidity;
    } else {
        _temperature = 0.0f;
        _humidity    = 0.0f;
    }

    core_state.signal("hdc1080.available",   BusValue::bo(_available));
    core_state.signal("hdc1080.temperature", BusValue::f32(_temperature));
    core_state.signal("hdc1080.humidity",    BusValue::f32(_humidity));
}

// ============================================================
// Опрос датчика
// ============================================================

void CLASS_MODULE_HDC1080::pollStep() {
    if (!_wireStarted) { _wireStarted = true; Wire.begin(); }

    if (_addr == 0) {
        if (!detect() || !initChip()) {
            _addr = 0;
            publish(false, 0.0f, 0.0f);
            return;
        }
    }

    uint16_t rawTemp = 0;
    uint16_t rawHum  = 0;
    if (!readMeasurement(rawTemp, rawHum)) {
        DEBUGHDC1080("measurement failed, re-detect on next poll\r\n");
        _addr = 0;
        publish(false, 0.0f, 0.0f);
        return;
    }

    float t = (float)rawTemp * 165.0f / 65536.0f - 40.0f;
    float h = (float)rawHum  * 100.0f / 65536.0f;

    publish(true, t, h);
}

void CLASS_MODULE_HDC1080::pollTask() {
    module_hdc1080.pollStep();
}
