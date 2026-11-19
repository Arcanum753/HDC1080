# module_hdc1080

Модуль датчика температуры и влажности HDC1080 для прошивки
[avr-fota](https://github.com/Arcanum753/avr-fota).

- I2C, адрес `0x40`; проверка `MANUFACTURER_ID = 0x5449`, `DEVICE_ID = 0x1000`.
- Ресурсы шины: `hdc1080.available`, `hdc1080.temperature` (°C), `hdc1080.humidity` (%RH).
- Также служит источником компенсации t/RH для `module_ccs811`.
- Веб-страница `hdc1080.html` — только показ значений.
- Терминал: `hdc`, `hdc show`, `hdc help`.
- Конфига нет; параметры датчика фиксированы.

Подключение: склонировать репозиторий в `src/module_hdc1080` и добавить
`+<module_hdc1080/>` в `src_filter` нужного env. Подробности — `AGENTS.md`.
