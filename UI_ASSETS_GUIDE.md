# UI Assets & Design Guide for ESP32-2424S012C

## 🎨 What Assets Can You Use?

### 1. Built-in TFT_eSPI Features

#### ✅ Shapes (No external assets needed!)
```cpp
// Primitives
tft.drawPixel(x, y, color);
tft.drawLine(x1, y1, x2, y2, color);
tft.drawRect(x, y, w, h, color);
tft.fillRect(x, y, w, h, color);
tft.drawCircle(x, y, radius, color);
tft.fillCircle(x, y, radius, color);
tft.drawRoundRect(x, y, w, h, radius, color);
tft.fillRoundRect(x, y, w, h, radius, color);
tft.drawTriangle(x1,y1, x2,y2, x3,y3, color);
tft.fillTriangle(x1,y1, x2,y2, x3,y3, color);

// Advanced
tft.drawArc(x, y, r1, r2, startAngle, endAngle, color);
tft.fillSmoothCircle(x, y, radius, color);
tft.drawEllipse(x, y, rx, ry, color);
```

#### ✅ Text & Fonts
```cpp
// Built-in fonts (already in library)
tft.setTextFont(1);  // Font 1: 8x8 pixels
tft.setTextFont(2);  // Font 2: 16 pixel high
tft.setTextFont(4);  // Font 4: 26 pixel high
tft.setTextFont(6);  // Font 6: 48 pixel (numbers only)
tft.setTextFont(7);  // Font 7: 7-segment 48px
tft.setTextFont(8);  // Font 8: 75 pixel (numbers only)

// Free fonts (TrueType converted)
#include "Free_Fonts.h"
tft.setFreeFont(&FreeSans9pt7b);
tft.setFreeFont(&FreeSansBold12pt7b);
tft.setFreeFont(&FreeSerif12pt7b);

// Custom smooth fonts (from .vlw files)
tft.loadFont("FontName");
```

#### ✅ Colors (16-bit RGB565)
```cpp
// Predefined colors
TFT_BLACK, TFT_WHITE, TFT_RED, TFT_GREEN, TFT_BLUE
TFT_CYAN, TFT_MAGENTA, TFT_YELLOW
TFT_ORANGE, TFT_PINK, TFT_PURPLE
TFT_NAVY, TFT_DARKGREEN, TFT_MAROON
TFT_GREENYELLOW, TFT_SKYBLUE, TFT_LIGHTGREY

// Custom colors (RGB565 format)
uint16_t color = tft.color565(r, g, b); // r,g,b = 0-255
```

### 2. Image Assets You Can Use

#### ✅ Option A: Convert Images to C Arrays (Best for ESP32-C3)
**Recommended:** Small icons, logos (< 10KB each)

**Process:**
1. Create image (PNG/JPG) → 240x240 or smaller
2. Convert to RGB565 array using online tool or script
3. Include in code as byte array

**Tools:**
- Image2cpp: http://javl.github.io/image2cpp/
- LCD Image Converter (desktop app)
- Python script with PIL

**Example:**
```cpp
// logo.h
const uint16_t logoImage[] PROGMEM = {
  0xFFFF, 0x0000, 0xF800, ... // RGB565 data
};

// In code:
tft.pushImage(x, y, width, height, logoImage);
```

#### ✅ Option B: JPEG/PNG Images (with libraries)
**Good for:** Photos, complex graphics

**Requirements:**
- JPEG: Use `TJpg_Decoder` library
- PNG: Use `PNGdec` library
- Store images in: SPIFFS, SD card, or LittleFS

**Example:**
```cpp
#include <TJpg_Decoder.h>

// Draw JPEG from array
TJpgDec.drawJpg(x, y, jpegData, sizeof(jpegData));

// Draw JPEG from file
TJpgDec.drawFsJpg(x, y, "/logo.jpg");
```

#### ✅ Option C: Sprite/Bitmap Files
**Good for:** Animations, game graphics

```cpp
TFT_eSprite sprite = TFT_eSprite(&tft);
sprite.createSprite(width, height);
sprite.fillSprite(TFT_BLACK);
sprite.drawString("Text", 0, 0);
sprite.pushSprite(x, y);
sprite.deleteSprite();
```

### 3. Icons & Symbols

#### ✅ Unicode Characters (Built-in)
Many fonts support Unicode:
```cpp
tft.print("♥");  // Heart
tft.print("⚡");  // Lightning
tft.print("🔒"); // Lock (might not render on all fonts)
tft.print("✓");  // Checkmark
tft.print("✗");  // X mark
```

#### ✅ Custom Icon Font (Recommended!)
Create icon font from SVG icons:
1. Use FontForge or online tools
2. Convert SVG icons to font
3. Load as custom font

#### ✅ Simple Drawn Icons (Best!)
Draw icons using shapes:
```cpp
// Lock icon
void drawLockIcon(int x, int y, uint16_t color) {
  tft.drawRoundRect(x, y+10, 30, 25, 5, color);
  tft.drawCircle(x+15, y+5, 10, color);
}

// WiFi icon
void drawWiFiIcon(int x, int y, uint16_t color) {
  tft.drawArc(x, y, 20, 18, 225, 315, color);
  tft.drawArc(x, y, 12, 10, 225, 315, color);
  tft.fillCircle(x, y-5, 2, color);
}
```

## 🎨 Design Constraints

### Display Specs:
- **Resolution**: 240x240 pixels
- **Shape**: Round (use circular layouts!)
- **Color Depth**: 16-bit (65,536 colors)
- **Memory**: ~115KB for full frame buffer

### Memory Guidelines:
| Asset Type | Size | Recommendation |
|------------|------|----------------|
| Full screen image | 115KB | Use 1-2 max |
| Small icon (32x32) | 2KB | Use many |
| Font | 1-20KB | Use 2-3 fonts |
| Sprites | Variable | Create/delete as needed |

### Performance Tips:
- ✅ Use `fillScreen()` sparingly (slow)
- ✅ Use sprites for animations
- ✅ Draw only changed areas
- ✅ Use `tft.setSwapBytes()` for faster image display

## 🎯 Recommended UI Library Options

### Option 1: TFT_eSPI Only (What we're using now)
**Pros:** Lightweight, direct control, fast
**Cons:** More manual coding
**Best for:** Simple UIs, custom designs

### Option 2: LVGL (Advanced GUI)
**Pros:** Professional widgets, animations, themes
**Cons:** Uses more memory (~200KB+)
**Best for:** Complex apps with many screens

**Setup:**
```cpp
#include <lvgl.h>
// See: https://github.com/lvgl/lvgl
```

### Option 3: TFT_eSPI_Widgets (Middle ground)
Custom widgets built on TFT_eSPI:
- Buttons
- Sliders
- Progress bars
- Graphs

## 🚀 For Your SuiLock Project

### Recommended Asset Strategy:

1. **Draw UI elements directly** (shapes, text)
   - Lock status indicator (circle + icon)
   - Time remaining (text + arc)
   - QR code (use QR library)

2. **Small icons as C arrays** (< 5KB each)
   - Lock icon
   - WiFi icon
   - Battery icon
   - Blockchain icon

3. **No photos/complex images**
   - Keep it simple and fast
   - Saves memory for blockchain code

### Example Icon Sizes:
```
16x16  = 512 bytes    ✅ Perfect for small icons
32x32  = 2KB          ✅ Good for main icons
64x64  = 8KB          ⚠️ Use sparingly
128x128 = 32KB        ❌ Too large (unless compressed)
```

## 📦 Asset Sources

### Free Icon Libraries:
- Material Icons: https://fonts.google.com/icons
- Font Awesome: https://fontawesome.com/icons
- Feather Icons: https://feathericons.com/
- Bootstrap Icons: https://icons.getbootstrap.com/

### Image Conversion Tools:
- Image2cpp: http://javl.github.io/image2cpp/
- GIMP (export as C array)
- Python PIL scripts

### QR Code Generation:
- `qrcode` library for Arduino
- Generate on ESP32, display on screen

## 💡 Quick Start

Want me to create:
1. **Simple card-based UI** (status cards, buttons)
2. **Circular watch-style UI** (for round display)
3. **SuiLock specific UI** (lock status, booking info)

Which style interests you? 🎨
