# ESP32-2424S012N/C Official Pin Configuration

## Board Information
- **Full Name**: CYD ESP32-2424S012 (Cheap Yellow Display)
- **MCU**: ESP32-C3-MINI-1U
- **Architecture**: RISC-V (32-bit single-core)
- **Flash**: 4MB (32Mbit)
- **Display**: 1.28" GC9A01 240x240 IPS
- **WiFi**: 802.11 b/g/n (2.4GHz)
- **Bluetooth**: 5.0 + BLE 5.0

## Display Pin Configuration (GC9A01)

Based on official sources and confirmed configurations:

| Function | GPIO | Description |
|----------|------|-------------|
| **SCK/CLK** | GPIO6 | SPI Clock |
| **MOSI/SDA** | GPIO7 | SPI Data Out |
| **CS** | GPIO10 | Chip Select |
| **DC/RS** | GPIO2 | Data/Command |
| **RST** | GPIO8 | Display Reset |
| **BL** | GPIO3 | Backlight (PWM) |

**Note**: MISO is not connected (display is write-only)

## Touch Configuration (CST816S - "C" variant only)

For **ESP32-2424S012C** (with capacitive touch):

| Function | GPIO | Description |
|----------|------|-------------|
| **SDA** | GPIO4 | I2C Data |
| **SCL** | GPIO5 | I2C Clock |
| **INT** | GPIO0 | Touch Interrupt |
| **RST** | GPIO1 | Touch Reset (or GPIO1) |

**I2C Address**: 0x15

## Available GPIO Pins

### Safe Pins for Your Projects:
- **GPIO1** - ADC, safe for general use
- **GPIO3** - Safe (but used for backlight on this board)
- **GPIO22** - UART RX (safe if not using Serial Monitor)

### Available but Currently Used by Display:
- GPIO0, GPIO4, GPIO5 - Free on "N" variant (no touch)
- GPIO8, GPIO9 - Available

### ⚠️ Pins to Avoid:
- **GPIO2** - Strapping pin (must be high during boot)
- **GPIO16-19** - Flash memory (SPI_CS, SPI_D, SPI_CLK, SPI_Q)
- **GPIO18/19** - USB (USB_D-, USB_D+)
- **GPIO20/21** - Serial (UART_TX, UART_RX)

## Pin Summary for ESP32-2424S012N (Your Board)

```
Display Pins (Used):
├── GPIO2  → DC (Data/Command)
├── GPIO3  → Backlight
├── GPIO6  → SPI Clock
├── GPIO7  → SPI MOSI
├── GPIO8  → Reset
└── GPIO10 → Chip Select

Free Pins (Available for your project):
├── GPIO0  ✅ (ADC capable)
├── GPIO1  ✅ (ADC capable)
├── GPIO4  ✅ (can be I2C SDA)
├── GPIO5  ✅ (can be I2C SCL)
└── GPIO9  ✅ (Boot button, but usable)

Reserved/Avoid:
├── GPIO18/19 → USB
├── GPIO20/21 → Serial
└── GPIO16/17 → Flash
```

## TFT_eSPI Configuration

Add this to `libraries/TFT_eSPI/User_Setup.h`:

```cpp
// Display driver
#define GC9A01_DRIVER

// Display size
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// ESP32-2424S012N Pin Configuration
#define TFT_MISO  -1    // Not connected
#define TFT_MOSI  7     // GPIO7
#define TFT_SCLK  6     // GPIO6
#define TFT_CS    10    // GPIO10
#define TFT_DC    2     // GPIO2
#define TFT_RST   8     // GPIO8
#define TFT_BL    3     // GPIO3

#define TFT_BACKLIGHT_ON HIGH
```

## Touch Configuration (If using "C" variant)

```cpp
// For ESP32-2424S012C only (with touch)
#include <Wire.h>
#include "CST816S.h"

CST816S touch(4, 5, 1, 0);  // sda, scl, rst, irq
// I2C Address: 0x15
```

## Power & Battery

- **Input**: 5V via USB or battery connector
- **Battery Support**: Lithium battery charging circuit included
- **Protection**: Overcharge and overcurrent protection
- **Operating Voltage**: 3.3V (regulated)
- **Power Consumption**: ~100mA typical

## Important Notes

### 1. ESP32-C3 Architecture
This board uses **RISC-V** (NOT Xtensa like original ESP32):
- Single-core 160MHz (not 240MHz as some docs say)
- 400KB SRAM (no PSRAM)
- 4MB Flash

### 2. Arduino IDE Board Selection
Select: **ESP32C3 Dev Module**

### 3. Memory Constraints
- Use `malloc()` instead of `ps_malloc()`
- 400KB SRAM total
- Display buffer needs ~115KB (240×240×2 bytes)
- ~285KB remaining for code

### 4. Variant Differences

| Feature | ESP32-2424S012N | ESP32-2424S012C |
|---------|-----------------|-----------------|
| Touch | ❌ No touch | ✅ CST816S capacitive |
| Free GPIOs | GPIO0,1,4,5,9 | GPIO0,1,9 |
| Use Case | Display only | Display + touch |
