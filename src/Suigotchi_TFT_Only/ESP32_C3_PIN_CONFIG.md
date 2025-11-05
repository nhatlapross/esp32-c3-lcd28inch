# ESP32-C3-MINI-1 Pin Configuration for 1.28" LCD

## ESP32-C3-MINI-1 Available GPIO Pins

Based on the datasheet you provided:

| Pin Name | GPIO | Function | Available? |
|----------|------|----------|------------|
| IO0 | GPIO0 | ADC1_CH0, XTAL_32K_P | ✅ Yes |
| IO1 | GPIO1 | ADC1_CH1, XTAL_32K_N | ✅ Yes |
| IO2 | GPIO2 | ADC1_CH2, FSPIQ | ✅ Yes |
| IO3 | GPIO3 | ADC1_CH3 | ✅ Yes |
| IO4 | GPIO4 | ADC1_CH4, FSPIHD, MTMS | ✅ Yes |
| IO5 | GPIO5 | ADC2_CH0, FSPIWP, MTDI | ✅ Yes |
| IO6 | GPIO6 | FSPICLK, MTCK | ✅ Yes |
| IO7 | GPIO7 | FSPID, MTDO | ✅ Yes |
| IO8 | GPIO8 | - | ✅ Yes |
| IO9 | GPIO9 | - | ✅ Yes |
| IO10 | GPIO10 | FSPICS0 | ✅ Yes |
| IO18 | GPIO18 | USB D- | ⚠️ USB |
| IO19 | GPIO19 | USB D+ | ⚠️ USB |
| IO20 | GPIO20 | UORXD (U0RXD) | ⚠️ Serial |
| IO21 | GPIO21 | UOTXD (U0TXD) | ⚠️ Serial |

**Note**: GPIO18/19 are used for USB, GPIO20/21 are used for Serial (don't use unless necessary)

## Typical 1.28" Round LCD Module Pin Mapping

### For GC9A01 Display (SPI):
```
LCD Pin     ESP32-C3 GPIO    SPI Function
-------     -------------    ------------
VCC         3.3V             Power
GND         GND              Ground
SCK/CLK     GPIO6            FSPICLK (SPI Clock)
MOSI/SDA    GPIO7            FSPID (SPI MOSI)
MISO        GPIO2            FSPIQ (SPI MISO) - optional
CS          GPIO10           FSPICS0 (Chip Select)
DC/RS       GPIO4            Data/Command
RST         GPIO8            Reset
BL          GPIO3            Backlight (PWM)
```

### For CST816S Touch Controller (I2C):
```
Touch Pin   ESP32-C3 GPIO    I2C Function
---------   -------------    ------------
SDA         GPIO0            I2C Data
SCL         GPIO1            I2C Clock
RST         GPIO9            Touch Reset
INT/IRQ     GPIO5            Touch Interrupt
```

## Recommended Configuration

### Option 1: Standard Configuration (Using FSPI pins)
```cpp
// TFT_eSPI User_Setup.h configuration
#define GC9A01_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// ESP32-C3 SPI pins (using FSPI)
#define TFT_MISO  2   // GPIO2 - FSPIQ (optional, can be -1 if not used)
#define TFT_MOSI  7   // GPIO7 - FSPID
#define TFT_SCLK  6   // GPIO6 - FSPICLK
#define TFT_CS    10  // GPIO10 - FSPICS0
#define TFT_DC    4   // GPIO4 - Data/Command
#define TFT_RST   8   // GPIO8 - Reset
#define TFT_BL    3   // GPIO3 - Backlight (PWM capable)

// Touch I2C pins (CST816S)
// In your .ino file:
// CST816S touch(0, 1, 9, 5); // sda, scl, rst, irq
```

### Option 2: Flexible Configuration (If you need different pins)
```cpp
// Alternative pin mapping (if hardware uses different pins)
#define TFT_MISO  -1  // Not connected
#define TFT_MOSI  7   // GPIO7
#define TFT_SCLK  6   // GPIO6
#define TFT_CS    10  // GPIO10
#define TFT_DC    2   // GPIO2
#define TFT_RST   4   // GPIO4
#define TFT_BL    3   // GPIO3

// Touch I2C
// CST816S touch(0, 1, 8, 5); // sda, scl, rst, irq
```

## Important Notes

### 1. Boot Strapping Pins (Be Careful!)
- **GPIO2**: Used for boot mode selection. Safe to use after boot.
- **GPIO8**: Used for boot mode selection. Safe to use after boot.
- **GPIO9**: Used for boot mode selection. Safe to use after boot.

If you have issues booting, these pins should be pulled high or low appropriately during boot.

### 2. USB Pins (Avoid)
- **GPIO18, GPIO19**: Used for USB communication. Don't use these for LCD!

### 3. Serial Pins (Avoid if debugging)
- **GPIO20 (RX), GPIO21 (TX)**: Used for Serial Monitor. Keep free for debugging.

### 4. SPI Configuration
ESP32-C3 has only **one SPI port (SPI2/FSPI)**:
- FSPI is the main SPI bus
- Default pins: SCLK=6, MOSI=7, MISO=2, CS=10

### 5. I2C Configuration
ESP32-C3 I2C can use any GPIO pins:
- Recommended: Use GPIO0 (SDA) and GPIO1 (SCL)
- These are also XTAL pins but work fine for I2C

## How to Verify Your Hardware Pins

### Method 1: Check Your Module's Silkscreen
Look at the printed labels on your ESP32-C3 LCD module board.

### Method 2: Check Manufacturer Documentation
If you bought from Waveshare, AliExpress, etc., check their product page.

### Method 3: Use a Multimeter (Continuity Test)
1. Find the GC9A01 display chip on the board
2. Trace the connections to ESP32-C3 pins
3. Match pin numbers to the datasheet

## Common LCD Module Pinouts

### Waveshare ESP32-C3 1.28" LCD Style:
```
TFT_MOSI  = GPIO7
TFT_SCLK  = GPIO6
TFT_CS    = GPIO10
TFT_DC    = GPIO2
TFT_RST   = GPIO8
TFT_BL    = GPIO3
Touch SDA = GPIO4
Touch SCL = GPIO5
Touch RST = GPIO9
Touch INT = GPIO0
```

### Generic ESP32-C3 Round LCD Style:
```
TFT_MOSI  = GPIO7
TFT_SCLK  = GPIO6
TFT_CS    = GPIO10
TFT_DC    = GPIO4
TFT_RST   = GPIO8
TFT_BL    = GPIO3
Touch SDA = GPIO0
Touch SCL = GPIO1
Touch RST = GPIO9
Touch INT = GPIO5
```

## Next Steps

1. **Identify your exact module's pin configuration**
2. **Update User_Setup.h** in TFT_eSPI library
3. **Update touch pin configuration** in the .ino file
4. **Compile and test**

If you know your specific module brand/model, I can provide exact pin mappings!
