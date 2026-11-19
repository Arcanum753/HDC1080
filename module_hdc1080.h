#ifndef _MODULE_HDC1080_h
#define _MODULE_HDC1080_h

#include "main.h"

#include "mod_context.h"

#ifdef DEBUG_HDC1080
#define DEBUGHDC1080(...) DBG_MOD("[M_HDC1080] ", __VA_ARGS__)
#else
#define DEBUGHDC1080(...)
#endif

#include "module_hdc1080_types.h"

// ============================================================
// module_hdc1080 — датчик температуры и влажности HDC1080 (I2C).
// Публикует на шину только доступность и значения; настроек нет.
// ============================================================
class CLASS_MODULE_HDC1080 {
public:
    CLASS_MODULE_HDC1080();
    void begin();
    void begin(ModContext& ctx);
    void web_Init();
    void register_resources();

    // ===== Публичное API =====
    void pollStep();
    bool isAvailable() const;
    float getTemperature() const;
    float getHumidity() const;
    uint8_t getAddr() const;
    uint16_t getManufacturerId() const;
    uint16_t getDeviceId() const;

private:
    // Терминальные команды: статические методы-члены, поэтому имеют
    // прямой доступ к приватному состоянию модуля (см. _terminal.cpp).
    void terminalRegister();
    static void s_terminalCmd();
    static void s_cmdShow();

    // Версионные методы
    String getVersionStr();
    String getGeneratedTime();
    String getCommitDateStr();
    void html_ver_get(AsyncWebServerRequest *request);

    // Веб-обработчики
    void handleInfo(AsyncWebServerRequest *request);
    void handleRead(AsyncWebServerRequest *request);
    void appendValues(String &values);

    // I2C и датчик
    bool detect();
    bool initChip();
    bool readMeasurement(uint16_t &rawTemp, uint16_t &rawHum);
    bool readRegs(uint8_t reg, uint8_t *buf, uint8_t len);
    bool writeRegs(uint8_t reg, const uint8_t *buf, uint8_t len);
    void publish(bool available, float temperature, float humidity);

    // EERTOS-задача
    static void pollTask();

protected:
    bool     _wireStarted;
    bool     _available;
    uint8_t  _addr;
    uint16_t _mfr;
    uint16_t _dev;
    float    _temperature;
    float    _humidity;
};

extern CLASS_MODULE_HDC1080 module_hdc1080;

#endif // _MODULE_HDC1080_h
