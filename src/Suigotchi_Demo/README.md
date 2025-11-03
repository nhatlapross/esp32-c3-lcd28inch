# ESP32-C3 Touch LCD 1.28" Test

This is the ESP32-C3 version of the LCD test sketch. The main difference from the ESP32-S3 version is the removal of PSRAM support.

## Key Changes from ESP32-S3 Version

### 1. Memory Allocation
- **ESP32-S3**: Uses `ps_malloc()` for PSRAM allocation
- **ESP32-C3**: Uses regular `malloc()` for SRAM allocation

### 2. No Additional Libraries Needed!
All libraries already work with ESP32-C3:
- ✅ TFT_eSPI (has ESP32-C3 support built-in)
- ✅ Custom drivers (LCD_1in28, CST816S, QMI8658)
- ✅ Arduino Core libraries (SPI, Wire)

## Hardware Configuration

### Current Pin Configuration (verify with your module):
```cpp
CST816S touch(6, 7, 13, 5);  // sda, scl, rst, irq
```

### Pin Mapping to Verify:
- **I2C (Touch)**: SDA=GPIO6, SCL=GPIO7, RST=GPIO13, IRQ=GPIO5
- **SPI (LCD)**: Check your module's pinout documentation
- **Display**: GC9A01 driver, 240x240 pixels

## How to Use

### 1. Arduino IDE Setup
1. Install ESP32 board support (if not already):
   - Add to Board Manager URLs: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
2. Select board: **ESP32C3 Dev Module**
3. Select port: Your USB port

### 2. Upload the Sketch
1. Open `ESP32-C3-Touch-LCD-1.28-Test.ino`
2. Click Upload
3. Open Serial Monitor at 115200 baud

### 3. Expected Behavior
The sketch runs three test sequences:
1. **Graphics Test**: Draws points, lines, rectangles, circles, and text
2. **IMU Test**: Displays accelerometer and gyroscope data (touch top to quit)
3. **Touch Test**: Draw on screen by touching (infinite loop)

## Memory Usage

ESP32-C3 has **400KB SRAM** (no PSRAM):
- Image buffer: 240 × 240 × 2 = **115,200 bytes** (~113KB)
- Remaining: ~287KB for code and variables

This is sufficient for the 1.28" display!

## Troubleshooting

### If compilation fails:
1. Check that ESP32-C3 board is selected
2. Verify TFT_eSPI library is in the libraries folder
3. Make sure all .cpp and .h files are in the same folder

### If display doesn't work:
1. Check pin connections match your hardware
2. Verify SPI pins in User_Setup.h (see parent libraries/TFT_eSPI/)
3. Check power supply is adequate (5V/1A minimum)

### If touch doesn't work:
1. Verify I2C pins (SDA, SCL)
2. Check touch controller is CST816S
3. Test with I2C scanner sketch

## Next Steps for SuiLock Project

Once this test works:
1. Add WiFi connectivity
2. Integrate MicroSui library for blockchain signing
3. Add lock control (relay/servo output)
4. Implement Tunnel payment channel logic

## ESP32-C3 vs ESP32-S3 Comparison

| Feature | ESP32-S3 | ESP32-C3 | Impact |
|---------|----------|----------|--------|
| CPU | Dual-core Xtensa | Single-core RISC-V | Slightly slower |
| SRAM | 512KB | 400KB | Less memory but OK |
| PSRAM | 8MB available | None | Use malloc() |
| DMA | Yes | No | Slower LCD updates |
| WiFi | Yes | Yes | ✅ Same |
| Bluetooth | BLE 5.0 | BLE 5.0 | ✅ Same |
| SPI | Multiple | 1 port | ✅ Works fine |

**Conclusion**: ESP32-C3 works perfectly for this project! The lack of PSRAM and DMA makes it slightly slower, but still very usable.
