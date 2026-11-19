#include "core_web/FSWebServerLib.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include "module_hdc1080.h"

#include "core_terminal/core_terminal.h"

// ============================================================
// Терминальные команды module_hdc1080
// ============================================================

static const char* const kHdcCommands[] = {
    "show", nullptr
};

static const char kHdcHelp[] PROGMEM =
    "hdc - HDC1080 temperature/humidity sensor\r\n"
    "  hdc        show availability, address and readings\r\n"
    "  hdc show   same as 'hdc'\r\n"
    "  hdc help   this help\r\n";

void CLASS_MODULE_HDC1080::terminalRegister() {
    TerminalRegisterModule("module_hdc1080", "hdc", "HDC1080 temperature/humidity sensor",
                           &CLASS_MODULE_HDC1080::s_terminalCmd,
                           kHdcCommands, kHdcHelp, false);
}

void CLASS_MODULE_HDC1080::s_terminalCmd() {
    char* sub = term.getNext();
    if (sub == nullptr) {
        s_cmdShow();
        return;
    }
    if (strcmp(sub, "help") == 0) {
        TerminalPrintText(kHdcHelp);
        return;
    }

    if (strcmp(sub, "show") == 0) {
        s_cmdShow();
        return;
    }

    Serial.printf("Unknown hdc subcommand '%s'. Type 'hdc help'.\r\n", sub);
}

void CLASS_MODULE_HDC1080::s_cmdShow() {
    if (term.getNext() != nullptr) {
        Serial.println("Usage: hdc [show]");
        return;
    }

    Serial.printf("\r\nHDC1080 @0x%02X: %s\r\n",
                  module_hdc1080._addr,
                  module_hdc1080._available ? "available" : "not available");

    if (!module_hdc1080._available) { return; }

    Serial.printf("Manufacturer: 0x%04X\r\n", module_hdc1080._mfr);
    Serial.printf("Device ID:    0x%04X\r\n", module_hdc1080._dev);
    Serial.printf("Temperature:  %.2f C\r\n", module_hdc1080._temperature);
    Serial.printf("Humidity:     %.2f %%RH\r\n", module_hdc1080._humidity);
}
