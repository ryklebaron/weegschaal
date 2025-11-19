# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a PlatformIO project for ESP8266 microcontroller (WeMos D1 Mini Lite board) using the Arduino framework. The project name "weegschaalV1" suggests this is a scale/weighing application (version 1).

## Build and Development Commands

### Building
```bash
pio run
```

### Uploading to Device
```bash
pio run --target upload
```

### Cleaning Build Files
```bash
pio run --target clean
```

### Monitoring Serial Output
```bash
pio device monitor
```

### Build and Upload in One Command
```bash
pio run --target upload && pio device monitor
```

## Project Structure

- `src/main.cpp` - Main application code with `setup()` and `loop()` functions (Arduino paradigm)
- `include/` - Project-specific header files (.h)
- `lib/` - Project-specific private libraries (each in its own subdirectory)
- `platformio.ini` - PlatformIO configuration (platform, board, framework, dependencies)
- `.pio/` - Build artifacts and installed dependencies (git-ignored)

## Architecture Notes

### Hardware Platform
- **Board**: WeMos D1 Mini Lite (ESP8266-based)
- **Platform**: espressif8266
- **Framework**: Arduino (uses Arduino.h and standard Arduino functions)

### Code Organization
- The Arduino framework requires `setup()` for initialization (runs once) and `loop()` for continuous execution
- Custom libraries should be placed in `lib/` subdirectories
- Header files go in `include/`
- External dependencies are managed via PlatformIO's library system (add to platformio.ini)

## Important Considerations

- ESP8266 has limited memory (RAM/Flash) - be mindful of variable sizes and avoid excessive dynamic allocation
- Serial output defaults to 115200 baud rate (configurable in platformio.ini)
- WiFi functionality available via ESP8266WiFi library if needed
