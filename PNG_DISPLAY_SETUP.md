# Display PNG Images Directly on ESP32

## Step 1: Install PNGdec Library

**In Arduino IDE:**
1. Sketch → Include Library → Manage Libraries
2. Search: **"PNGdec"**
3. Install: **"PNGdec by Larry Bank"**

## Step 2: Store the PNG

Two options:
- **Option A**: Convert PNG to C array (embed in code)
- **Option B**: Store on SPIFFS/LittleFS filesystem

For ESP32-C3 without PSRAM, **Option A (C array)** is better!

## Step 3: I'll create the code

After you install PNGdec library, I'll create code that:
1. Includes your PNG as a C array
2. Decodes it using PNGdec
3. Displays using LCD_1in28 (the working driver!)

**Install PNGdec library now, then tell me "done"!**
