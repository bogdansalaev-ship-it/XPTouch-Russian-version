# Среда разработки

## Инструменты

- [Visual Studio Code](https://code.visualstudio.com/);
- расширение **PlatformIO IDE**;
- Python 3.7 или новее;
- Node.js LTS 14 или новее.

PlatformIO устанавливается как расширение VS Code. Библиотека LovyanGFX уже находится в `lib/LovyanGFX`, остальные зависимости PlatformIO скачает из `platformio.ini`.

## Пользовательская плата ESP32-S3

Для экрана JC8048W550 скопируйте описание платы:

```bash
mkdir -p ~/.platformio/boards
cp docs/boards/esp32-s3-devkitc1-n16r8.json ~/.platformio/boards/
```

После этого перезапустите VS Code и откройте проект из корня репозитория.

## Сборка и прошивка

2.8-дюймовый экран:

```bash
python3 -m platformio run -e esp32dev
python3 -m platformio run -e esp32dev -t upload
```

5-дюймовый экран:

```bash
python3 -m platformio run -e esp32-s3dev
python3 -m platformio run -e esp32-s3dev -t upload
```

Для OTA задайте IP устройства и используйте кнопку Upload в PlatformIO либо задачи VS Code `XPTouch: OTA 2.8` и `XPTouch: OTA 5.0`:

```bash
export XTOUCH_OTA_IP=192.168.1.50
python3 -m platformio run -e esp32dev-ota -t upload
python3 -m platformio run -e esp32-s3dev-ota -t upload
```

Устройство и компьютер должны быть в одной Wi-Fi-сети. ArduinoOTA запускается после успешного подключения XPTouch к Wi-Fi. Во время прошивки не отключайте питание.

## Конфигурация

Для Cloud Mode устройство использует `provisioning.json`, для LAN Only Mode - `xtouch.json` и `config.json` на microSD. Секреты Wi-Fi, токены и access code нельзя добавлять в Git.

Скрипты в `scripts/` добавляют версию прошивки и выполняют post-build обработку. Перед отправкой изменений проверьте оба аппаратных профиля.

## HTTPS и Cloud API

Клиент HTTPS в `src/xtouch/cloud.hpp` общий, поэтому запросы Cloud API выполняются последовательно. При добавлении нового HTTPS-вызова сохраняйте существующую синхронизацию и не подставляйте фиктивные значения при неполных данных.