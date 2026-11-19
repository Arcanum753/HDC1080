# module_hdc1080

Модуль датчика температуры и влажности HDC1080 для прошивки
[avr-fota](https://github.com/Arcanum753/avr-fota).

## Description

**RU.** Опциональный модуль HDC1080 (I2C): температура и относительная влажность.
Публикует на ресурсную шину `core_state` доступность и значения, показывает их на
веб-странице и в последовательном терминале. Также служит источником компенсации t/RH
для `module_ccs811`. Настроек нет.

**EN.** Optional HDC1080 module (I2C): temperature and relative humidity. Publishes
availability and readings to the `core_state` resource bus and exposes them via a web page
and the serial terminal. Also provides t/RH compensation data for `module_ccs811`.
No settings.

## Features

- I2C address `0x40`; manufacturer id `0x5449`, device id family `0x10xx`
  (`0x1000`, clone `0x1050`).
- Separate temperature and humidity measurements (14-bit), heater off.
- Read-only resources: `hdc1080.available`, `hdc1080.temperature`, `hdc1080.humidity`.
- Web page `hdc1080.html` with 1-second refresh; terminal command `hdc`.
- Hot-plug: re-detected on the next poll after a failure.
- No config file.

## Resources

| Resource | Type | Description |
|---|---|---|
| `hdc1080.available` | BOOL ro | sensor found, measurement successful |
| `hdc1080.temperature` | F32 ro | temperature, °C |
| `hdc1080.humidity` | F32 ro | relative humidity, %RH |

## Web and terminal

- `GET /hdc1080/info` — state, address, IDs, readings (CVT).
- `GET /hdc1080/read` — readings only (polled every second).
- `GET /hdc1080/ver` — module version.
- `hdc`, `hdc show`, `hdc help`.

## Macro example

```lua
every("0 * * * * *", function()
    print("HDC1080", hdc1080.available, hdc1080.temperature, hdc1080.humidity)
end)
```

Ready-to-use cron examples: `module_macros/web/macros/sensors_15s.lua`,
`sensors_30s.lua`, `sensors_1m.lua`, `sensors_5m.lua`.

## Installation

1. Clone this repository into `src/module_hdc1080`.
2. Add `+<module_hdc1080/>` to the target env `src_filter`.
3. Add `src/module_hdc1080/module_hdc1080.ini` to `extra_configs`.
4. Build the env; the module is registered via `[registry]`.

## Tested

2026-11-19, ESP32, env `esp32-i2c-mapper`, firmware `0.117.20261119_1754.2104`:

- Address `0x40`, manufacturer `0x5449`, device id `0x1050` (compatible clone).
- Readings: `29.1 °C`, `73.0 %RH`.
- Separate T/RH reads: the clone's sequential mode returned `0xFFFF` for humidity.
- Compensation source for CCS811 confirmed in the log.
- Web pages/endpoints and terminal commands verified.

## Documentation

See `AGENTS.md` for architecture, requirements and limitations.
