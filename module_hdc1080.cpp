#include "core_web/FSWebServerLib.h"

#include "core_state/core_state.h"
#include "core_task/core_task.h"
#include "core_terminal/core_terminal.h"
#include "core_terminal/ErriezSerialTerminal.h"

#include "module_hdc1080.h"
#include "module_hdc1080_version.h"
#include "core_sys/eertos.h"

#include <Wire.h>
#include <stdio.h>

CLASS_MODULE_HDC1080 module_hdc1080;

CLASS_MODULE_HDC1080::CLASS_MODULE_HDC1080() {
    _wireStarted = false;
    _available   = false;
    _addr        = 0;
    _mfr         = 0;
    _dev         = 0;
    _temperature = 0.0f;
    _humidity    = 0.0f;
}

// ============================================================
// begin()
// ============================================================
void CLASS_MODULE_HDC1080::begin() {
    DEBUGHDC1080("%s\r\n", __FUNCTION__);

    if (!_wireStarted) { _wireStarted = true; Wire.begin(); }

    _addr       = 0;
    _mfr        = 0;
    _dev        = 0;
    _available  = false;

    if (detect()) {
        _available = initChip();
    } else {
        DEBUGHDC1080("HDC1080: not found\r\n");
    }

    terminalRegister();

    core_task.every("hdc1080_poll", pollTask, HDC1080_POLL_MS, true);
}

void CLASS_MODULE_HDC1080::begin(ModContext& ctx) {
    (void)ctx;
    begin();
}

// ============================================================
// web_Init()
// ============================================================
void CLASS_MODULE_HDC1080::web_Init() {
    DEBUGHDC1080("%s\r\n", __FUNCTION__);

    ESPHTTPServer.on("/hdc1080/info", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (!ESPHTTPServer.checkAuth(request)) { return request->requestAuthentication(); }
        this->handleInfo(request);
    });

    ESPHTTPServer.on("/hdc1080/read", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (!ESPHTTPServer.checkAuth(request)) { return request->requestAuthentication(); }
        this->handleRead(request);
    });

    ESPHTTPServer.on("/hdc1080/ver", HTTP_GET, [this](AsyncWebServerRequest *request) {
        this->html_ver_get(request);
    });
}

// ============================================================
// register_resources() — публикация ресурсов на шине core_state
// ============================================================
void CLASS_MODULE_HDC1080::register_resources() {
    DEBUGHDC1080("%s\r\n", __FUNCTION__);

    core_state.regState("available",   BusValue::BOOL, "HDC1080 present on I2C bus", false);
    core_state.regState("temperature", BusValue::F32,  "temperature (C)",            false);
    core_state.regState("humidity",    BusValue::F32,  "relative humidity (%RH)",    false);
}

// ============================================================
// Веб-обработчики
// ============================================================
void CLASS_MODULE_HDC1080::appendValues(String &values) {
    values += "hdc_available|" + String(_available ? "available" : "not available") + "|div\n";

    if (_available) {
        char tbuf[16];
        char hbuf[16];
        snprintf(tbuf, sizeof(tbuf), "%.2f", _temperature);
        snprintf(hbuf, sizeof(hbuf), "%.2f", _humidity);
        values += "hdc_temp|" + String(tbuf) + " °C|div\n";
        values += "hdc_hum|"  + String(hbuf) + " %RH|div\n";
    } else {
        values += "hdc_temp|--|div\n";
        values += "hdc_hum|--|div\n";
    }
}

void CLASS_MODULE_HDC1080::handleInfo(AsyncWebServerRequest *request) {
    DEBUGHDC1080("%s\r\n", __FUNCTION__);

    String values = "";
    char buf[16];

    values += "hdc_available|" + String(_available ? "available" : "not available") + "|div\n";

    snprintf(buf, sizeof(buf), "0x%02X", _addr);
    values += "hdc_addr|" + String(buf) + "|div\n";

    snprintf(buf, sizeof(buf), "0x%04X", _mfr);
    values += "hdc_mfr|" + String(buf) + "|div\n";

    snprintf(buf, sizeof(buf), "0x%04X", _dev);
    values += "hdc_dev|" + String(buf) + "|div\n";

    appendValues(values);
    request->send(200, "text/plain", values);
}

void CLASS_MODULE_HDC1080::handleRead(AsyncWebServerRequest *request) {
    String values = "";
    appendValues(values);
    request->send(200, "text/plain", values);
}

// ============================================================
// Версионные методы
// ============================================================
String CLASS_MODULE_HDC1080::getVersionStr() {
    return String(MODULE_HDC1080_VERSION);
}

String CLASS_MODULE_HDC1080::getGeneratedTime() {
    return String(MODULE_HDC1080_GENERATED_TIME);
}

String CLASS_MODULE_HDC1080::getCommitDateStr() {
    return String(MODULE_HDC1080_COMMIT_DATE_STR);
}

void CLASS_MODULE_HDC1080::html_ver_get(AsyncWebServerRequest *request) {
    DEBUGHDC1080("%s\r\n", __FUNCTION__);
    String values = "";
    values += "hdcversion|" + getVersionStr()    + "|div\n";
    values += "hdcgentime|" + getGeneratedTime() + "|div\n";
    values += "hdcgendate|" + getCommitDateStr() + "|div\n";
    request->send(200, "text/plain", values);
}

// ============================================================
// Публичное API
// ============================================================
bool CLASS_MODULE_HDC1080::isAvailable() const { return _available; }
float CLASS_MODULE_HDC1080::getTemperature() const { return _temperature; }
float CLASS_MODULE_HDC1080::getHumidity() const { return _humidity; }
uint8_t CLASS_MODULE_HDC1080::getAddr() const { return _addr; }
uint16_t CLASS_MODULE_HDC1080::getManufacturerId() const { return _mfr; }
uint16_t CLASS_MODULE_HDC1080::getDeviceId() const { return _dev; }
