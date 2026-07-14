# AGENTS.md

This file provides guidance to AI agents when working with code in this repository.

## What this is

Info Orbs is an ESP32-based physical display device: five small round TFT screens ("orbs") driven by one ESP32, showing rotating "widgets" (Clock, Weather, Stock, Parqet portfolio, MQTT, custom web data). Firmware is Arduino/C++ built with PlatformIO. There is no companion app/server — all logic runs on-device, with WiFiManager providing an in-browser captive-portal for WiFi setup.

## Build / flash commands

All commands run from the repo root (`platformio.ini` sets `src_dir = firmware/src`).

```bash
pio run                                    # build (env: esp32doit-devkit-v1)
pio run --target upload --upload-port /dev/ttyUSB0   # flash over serial
```

- **`firmware/config/config.h` is required and NOT checked into git.** The build fails immediately (`config_helper.h` has a `#error`) if it's missing. Before building for the first time: `cp firmware/config/config.h.template firmware/config/config.h`, then edit it (timezone, widget selection via `INCLUDE_*` macros, button pins, etc.). Because it's gitignored, an existing local `config.h` can silently drift out of sync with renamed/new macros expected by current source — if a build fails with "was not declared in this scope" for a config-looking symbol, diff local `config.h` against `config.h.template` first.
- Widgets are opt-in at compile time via `INCLUDE_WEATHER` / `INCLUDE_STOCK` / `INCLUDE_PARQET` / `INCLUDE_WEBDATA` / `INCLUDE_MQTT` in `config.h`; only Clock is unconditional (see `addWidgets()` in `firmware/src/main.cpp`).
- If the local PlatformIO `espressif32` platform package ever fails with `InvalidJSONFile: Could not load broken JSON: .../platforms/espressif32/.piopm`, the cached platform install is corrupted — `rm -rf` that platform directory under `~/.platformio/platforms/espressif32` and let `pio run` reinstall it.
- **Upload note**: if flashing fails with `Unable to verify flash chip connection (No serial data received.)` right after "Changing baud rate to 460800", the USB-serial adapter/cable can't keep up at that speed — retry with `PLATFORMIO_UPLOAD_SPEED=115200 pio run --target upload --upload-port <port>`.
- There is no unit test suite. CI (`.github/workflows/platformio.yml`) just does `cp config.h.template config.h` and `pio run` on Linux/macOS/Windows. Linting is via MegaLinter (`.mega-linter.yml`): clang-format for C++ (`.clang-format` at repo root; excludes `firmware/lib/`) and markdownlint for docs — it also runs on every push to `dev`, not just PRs.

## Architecture

**Main loop** (`firmware/src/main.cpp`): `setup()` brings up filesystem/config/buttons/screens, then blocks in `WifiWidget::setup()` to connect (or start the config portal) before any other widget is initialized. `loop()` feeds the watchdog, drives `wifiWidget` until connected, then round-robins the rest: `globalTime->updateTime()` → button checks → `widgetSet->updateCurrent()/drawCurrent()` → brightness-by-time → widget cycling → `TaskManager` processing.

**Widget lifecycle**: every screen/feature (`ClockWidget`, `WeatherWidget`, `StockWidget`, etc., in `firmware/src/widgets/<name>/`) subclasses `Widget` (`firmware/src/core/widget/Widget.h`) and implements `setup()/update()/draw()/buttonPressed()/getName()`. `WidgetSet` (`firmware/src/core/widget/WidgetSet.cpp`) owns the collection and tracks which one is "current" (cycled by button press or `WIDGET_CYCLE_DELAY`).

**Screens**: `ScreenManager` wraps a single `TFT_eSPI` instance that multiplexes 5 physical displays via separate CS pins (`SCREEN_1_CS`..`SCREEN_5_CS`), selected with `selectScreen(index)` before drawing. Widgets generally target one or more of the 5 screen indices explicitly (e.g. `WifiWidget` uses `statusScreenIndex = 3`). Software "brightness"/dimming (`ScreenManager::setBrightness`, `Utils::rgb565dim`) is a color-scaling trick applied to drawn pixels — there is no hardware backlight-enable pin; backlights are wired directly to VCC and cannot be switched off in software.

**Async work**: `TaskManager`/`TaskFactory` (`firmware/src/core/utils/`) run HTTP (and eventually MQTT) requests on a FreeRTOS task with a semaphore/queue, called from `loop()` via `processAwaitingTasks()`/`processTaskResponses()`. Widgets that need network data (Weather, Stock, Parqet, WebData) create tasks through `TaskFactory` rather than making blocking HTTP calls inline — see `docs/TaskManager Developer Guide.md` and `WeatherWidget.cpp` for the pattern (preProcess callback for filtering, response callback for updating widget state — never mutate core widget state from the preProcess callback).

**Config resolution order** (see `references/Widget Developer Guide to config.md`): a widget should define every macro it needs in its own header (optionally behind `#ifndef` so it's overridable), with `config.h` as the (required, in this repo) place for user overrides. The doc describes an additional `config.system.h` fallback layer; that file does not currently exist in this checkout — `config_helper.h` only includes `config.h` directly, so there is no fallback if a macro is entirely missing from `config.h`.

**Time**: `GlobalTime` (singleton, `firmware/src/core/globaltime/`) is the single source of truth for current time, refreshed via NTP or a timezone-lookup API depending on `CLOCK_USE_NTP`. Widgets read time through it rather than calling `millis()`/RTC directly.

**Logging**: ArduinoLog (`Log.noticeln/infoln/warningln/errorln`) is now used throughout `firmware/src/core` and most widgets instead of raw `Serial.print` — see `references/Developer Guide to Logging.md`. `LOG_LEVEL` is set in `config.h`.

**Buttons**: three physical buttons (left/middle/right) generate short/medium/long press events (`ButtonState` in `firmware/src/core/button/Button.h`) routed to the current widget's `buttonPressed()`. Pin assignment is `BUTTON_LEFT_PIN`/`BUTTON_MIDDLE_PIN`/`BUTTON_RIGHT_PIN` in `config.h`; screen/button orientation for upside-down mounting is handled by `ORB_ROTATION`, not by swapping pin numbers.

**Hardware reference**: PCB/schematic sources are under `PCBFiles+Schematics/infoorbs-v0_3/` (KiCad). The display connector only exposes `VCC/GND/CS/DC/RST` — no separate backlight or additional GPIO lines.
