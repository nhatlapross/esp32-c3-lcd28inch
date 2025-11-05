// ==================================================================================
// User_Setup for ESP32-2424S012N/C (CYD - Cheap Yellow Display)
// ESP32-C3-MINI-1U with 1.28" GC9A01 240x240 Round Display
// ==================================================================================
// Copy this file to: libraries/TFT_eSPI/User_Setup.h
// Or reference it in: libraries/TFT_eSPI/User_Setup_Select.h
// ==================================================================================

#define USER_SETUP_INFO "ESP32-2424S012"

// Display driver - GC9A01 round LCD 240x240
#define GC9A01_DRIVER

// Display size
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// ==================================================================================
// ESP32-2424S012N Pin Configuration
// ==================================================================================
// These pins are hardwired on the board - DO NOT CHANGE
#define TFT_MISO  -1    // Not connected (display is write-only)
#define TFT_MOSI  7     // GPIO7 - SPI MOSI
#define TFT_SCLK  6     // GPIO6 - SPI Clock
#define TFT_CS    10    // GPIO10 - Chip Select
#define TFT_DC    2     // GPIO2 - Data/Command
#define TFT_RST   8     // GPIO8 - Reset
#define TFT_BL    3     // GPIO3 - Backlight (PWM capable)

// Backlight control
#define TFT_BACKLIGHT_ON HIGH

// ==================================================================================
// Fonts to be available
// ==================================================================================
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// ==================================================================================
// SPI frequency for TFT (ESP32-C3 maximum)
// ==================================================================================
#define SPI_FREQUENCY  40000000  // 40MHz - safe for ESP32-C3
// #define SPI_FREQUENCY  27000000  // 27MHz - use this if display glitches

// Optional reduced SPI frequency for reading TFT (not used since MISO not connected)
#define SPI_READ_FREQUENCY  20000000

// ==================================================================================
// ESP32-C3 specific settings
// ==================================================================================
#define SUPPORT_TRANSACTIONS  // Enable SPI transaction support
// #define DISABLE_ALL_LIBRARY_WARNINGS  // Uncomment to suppress warnings

// ==================================================================================
// Notes:
// ==================================================================================
// 1. This board uses ESP32-C3 (RISC-V architecture, NOT Xtensa)
// 2. No PSRAM - use malloc() instead of ps_malloc()
// 3. No DMA support on ESP32-C3 for SPI
// 4. Display is GC9A01 driver (circular 240x240)
// 5. Pins are hardwired on the PCB - cannot be changed
// 6. GPIO2 is a strapping pin - must be HIGH during boot
// 7. For touch (C variant): Use I2C on GPIO4(SDA), GPIO5(SCL)
