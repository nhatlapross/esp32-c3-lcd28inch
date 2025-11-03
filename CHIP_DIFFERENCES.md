# ESP32-C3 vs ESP32-S3 - Key Differences

## Quick Comparison

| Feature | ESP32-C3 | ESP32-S3 |
|---------|----------|----------|
| **Architecture** | RISC-V (32-bit) | Xtensa (32-bit dual-core) |
| **CPU Cores** | 1 | 2 |
| **Max Frequency** | 160MHz | 240MHz |
| **SRAM** | 400KB | 512KB |
| **PSRAM** | ❌ None | ✅ 2-8MB (optional) |
| **Flash** | 4MB typical | 4-8MB typical |
| **SPI Ports** | 1 (FSPI) | 2 (HSPI, VSPI) |
| **DMA** | ❌ No | ✅ Yes |
| **USB** | USB Serial/JTAG | USB OTG |
| **Price** | $ Lower | $$ Higher |

## Code Differences Needed

### 1. Memory Allocation
```cpp
// ESP32-S3 (with PSRAM)
BlackImage = (UWORD *)ps_malloc(Imagesize);

// ESP32-C3 (no PSRAM)
BlackImage = (UWORD *)malloc(Imagesize);
```

### 2. SPI Port Selection
```cpp
// ESP32-S3
vspi = new SPIClass(VSPI);

// ESP32-C3
vspi = new SPIClass(FSPI);
```

### 3. Pin Definitions
Different boards have different pin layouts:
- ESP32-S3-Touch-LCD-1.28 uses different pins
- ESP32-2424S012C (C3) uses different pins

## Hardware Differences

### ESP32-S3-Touch-LCD-1.28
| Function | GPIO |
|----------|------|
| LCD_DC | 8 |
| LCD_CS | 9 |
| LCD_CLK | 10 |
| LCD_MOSI | 11 |
| LCD_RST | 14 |
| LCD_BL | 2 |
| Touch_SDA | 6 |
| Touch_SCL | 7 |
| Touch_RST | 13 |
| Touch_INT | 5 |

### ESP32-2424S012C (C3)
| Function | GPIO |
|----------|------|
| LCD_DC | 2 |
| LCD_CS | 10 |
| LCD_CLK | 6 |
| LCD_MOSI | 7 |
| LCD_RST | 8 |
| LCD_BL | 3 |
| Touch_SDA | 4 |
| Touch_SCL | 5 |
| Touch_RST | 1 |
| Touch_INT | 0 |

## Making Code Work for Both

### Option 1: Automatic Detection (Recommended)
Use compiler defines to detect chip:
```cpp
#if defined(CONFIG_IDF_TARGET_ESP32C3)
    // ESP32-C3 specific code
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    // ESP32-S3 specific code
#endif
```

### Option 2: Separate Folders
Keep separate versions:
- `ESP32-C3-Touch-LCD-1.28-Test/` (for C3)
- `ESP32-S3-Touch-LCD-1.28-Test/` (for S3)

### Option 3: Board Detection
Create a unified version with board auto-detection.

## Which Approach Should We Use?

I recommend **Option 1** - automatic detection. This way:
- ✅ One codebase for both chips
- ✅ Automatically adapts based on selected board
- ✅ Easy to maintain
- ✅ No manual configuration needed

Would you like me to create a universal version that works on both?
