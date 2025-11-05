# Setup Instructions for ESP32-2424S012N

## 🎯 Quick Start Guide

### Step 1: Install Arduino IDE (if not already)
Download from: https://www.arduino.cc/en/software

### Step 2: Install ESP32 Board Support
1. Open Arduino IDE
2. Go to: **File → Preferences**
3. Add to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to: **Tools → Board → Boards Manager**
5. Search for "esp32"
6. Install: **esp32 by Espressif Systems** (version 2.0.11 or newer)

### Step 3: Configure TFT_eSPI Library
You have **two options**:

#### Option A: Replace User_Setup.h (Recommended)
1. Navigate to: `libraries/TFT_eSPI/`
2. **Backup** the original `User_Setup.h` (rename it to `User_Setup_backup.h`)
3. **Copy** `User_Setup_ESP32_2424S012.h` to `libraries/TFT_eSPI/User_Setup.h`

#### Option B: Use User_Setup_Select.h
1. Open: `libraries/TFT_eSPI/User_Setup_Select.h`
2. Comment out the default setup:
   ```cpp
   // #include <User_Setup.h>
   ```
3. Add your custom setup:
   ```cpp
   #include <User_Setups/User_Setup_ESP32_2424S012.h>  // ESP32-2424S012
   ```
4. Copy `User_Setup_ESP32_2424S012.h` to `libraries/TFT_eSPI/User_Setups/`

### Step 4: Open Test Sketch
1. Open: `ESP32-C3-Touch-LCD-1.28-Test.ino`
2. This file is in: `src/ESP32-C3-Touch-LCD-1.28-Test/`

### Step 5: Select Board
1. Go to: **Tools → Board → ESP32 Arduino**
2. Select: **ESP32C3 Dev Module**

### Step 6: Configure Board Settings
Set these options in **Tools** menu:

| Setting | Value |
|---------|-------|
| **Board** | ESP32C3 Dev Module |
| **USB CDC On Boot** | Enabled |
| **CPU Frequency** | 160MHz |
| **Flash Frequency** | 80MHz |
| **Flash Mode** | QIO |
| **Flash Size** | 4MB (32Mb) |
| **Partition Scheme** | Default 4MB with spiffs |
| **Upload Speed** | 921600 |
| **Port** | (Select your USB port) |

### Step 7: Compile & Upload
1. Click the **Verify** button (checkmark) to compile
2. Click the **Upload** button (arrow) to upload
3. Open **Serial Monitor** (115200 baud) to see output

## ✅ What You Should See

### On Serial Monitor:
```
ESP32-C3: Using regular SRAM (no PSRAM)
Successfully allocated 115200 bytes
GPIO Init successful!
drawing...

QMI8658_init

All tests completed!
ESP32-2424S012N running successfully!
```

### On Display:
1. **Color Test**: Red → Green → Blue screens
2. **Graphics Test**: Points, lines, shapes, text
3. **IMU Test**: Shows accelerometer and gyroscope data
4. **Completion**: "ESP32-2424S012N Test Complete!"

## 🔧 Troubleshooting

### Problem: Compilation Error - "TFT_eSPI.h not found"
**Solution**: Make sure TFT_eSPI library is in the correct folder:
- Should be: `arduino/libraries/TFT_eSPI/`
- Copy from: `esp32-c3-lcd28inch/libraries/TFT_eSPI/`

### Problem: Compilation Error - "User_Setup not configured"
**Solution**: Follow Step 3 above to configure TFT_eSPI properly

### Problem: Upload Failed - "No serial port found"
**Solution**:
1. Check USB cable (must be data cable, not charge-only)
2. Install CH340/CP210x USB driver
3. Press and hold **BOOT button** while connecting USB

### Problem: Display shows nothing
**Solution**:
1. Check power LED is on
2. Verify `User_Setup.h` has correct pin configuration
3. Try different USB cable/power source
4. Verify board is ESP32-2424S012N (not S3 version)

### Problem: Display shows garbage/wrong colors
**Solution**:
1. Lower SPI frequency in `User_Setup.h`:
   ```cpp
   #define SPI_FREQUENCY  27000000  // Try 27MHz instead of 40MHz
   ```
2. Check if you selected correct board (ESP32C3, not ESP32)

### Problem: "Out of memory" error
**Solution**:
1. This is expected if code tries to allocate PSRAM
2. Verify you're using the modified ESP32-C3 version (with `malloc()`)
3. ESP32-C3 has only 400KB SRAM, so large buffers won't fit

### Problem: IMU test shows zeros
**Solution**:
1. Check if your board has QMI8658 IMU (some variants don't)
2. Verify I2C connections
3. This is not critical - display should still work

## 📝 Next Steps

Once basic test works:
1. ✅ Display is working
2. ✅ SPI communication is correct
3. ✅ Memory allocation works
4. ✅ Ready to start your SuiLock project!

### For SuiLock Development:
- [ ] Add WiFi connectivity
- [ ] Integrate MicroSui library for blockchain signing
- [ ] Add lock control circuit (relay on available GPIO)
- [ ] Implement Tunnel payment channel
- [ ] Create UI for booking and access control

## 📚 Additional Resources

- **ESP32-C3 Datasheet**: https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf
- **GC9A01 Driver Info**: https://github.com/Bodmer/TFT_eSPI
- **Board Info**: https://espboards.dev/boards/esp32-2424s012
- **Arduino ESP32**: https://docs.espressif.com/projects/arduino-esp32/

## 🆘 Still Having Issues?

Check:
1. Pin configuration in `ESP32_2424S012N_CONFIG.md`
2. Serial Monitor output for error messages
3. Arduino IDE console for compilation errors
4. USB cable quality (try a different one)
5. Board selection (must be ESP32C3, not ESP32 or ESP32S3)

**Your board model**: ESP32-2424S012N (without touch)
**Correct chip**: ESP32-C3-MINI-1U
**Display**: GC9A01 240x240
