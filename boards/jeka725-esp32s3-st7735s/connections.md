# ESP32-S3 N16R8 + ST7735S 1.8" + microSD

## Display

| ST7735S | ESP32-S3 |
|---|---:|
| VCC | 3.3V |
| GND | GND |
| SCK/CLK | GPIO5 |
| MOSI/SDA | GPIO6 |
| DC/A0 | GPIO7 |
| RST/RES | GPIO15 |
| CS | GPIO16 |
| BL/LED | GPIO4 |

## Buttons

| Function | GPIO |
|---|---:|
| Left | 12 |
| Right | 13 |
| Up | 9 |
| Down | 11 |
| Select | 14 |

## microSD SPI reader

| SD reader | ESP32-S3 |
|---|---:|
| 3V3 | 3.3V |
| GND | GND |
| CLK | GPIO5 |
| MOSI | GPIO6 |
| MISO | GPIO17 |
| CS | GPIO18 |

The TFT and SD share SCK/MOSI and use separate chip-select pins.

## Notes

- Flash: 16 MB.
- PSRAM: 8 MB OPI.
- Display: 128x160 ST7735S.
- ROTATION=3 targets the requested landscape physical orientation.
- No battery/PMIC/gauge is assumed.
- GPIO17/18 are reserved here for the SD SPI bus; verify they are physically free on the exact ESP32-S3 board before wiring.
