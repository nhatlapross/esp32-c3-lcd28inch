# Suigotchi UI Demo

A pixel-art style Tamagotchi-inspired interface for ESP32-2424S012C, based on your Suigotchi design!

## Features

✨ **Start Screen**
- Black background
- Cyan "Suigotchi" title with water drop logo
- Blue pixel-style START button with black text
- Touch to start

✨ **Suigotchi Screen**
- Gameboy-style frame (gray-green screen)
- Cute Sui character (cyan water drop creature)
- Health and happiness bars
- Action icons: Feed (water drop), Play (refresh), Sleep (cloud)
- Touch anywhere to go back

## How to Use

1. **Upload to your ESP32-2424S012C**
2. **Touch the START button** to enter Suigotchi screen
3. **Touch anywhere on Suigotchi screen** to go back to start

## Design Elements

### Colors
- Black background (`0x0000`)
- Sui cyan/water blue (`0x07FF`)
- Gray for text (`0x7BEF`)
- Dark gray for borders (`0x39E7`)
- Gameboy green for screen (`0x7BE0`)

### Pixel Art Style
- Blocky buttons with visible borders
- 8x8 pixel icons (heart, save disk, etc.)
- Simple geometric character
- Retro gameboy aesthetic

## Character Design

The Sui character is a cute water drop blob:
- Cyan circular body
- Big anime-style eyes with highlights
- Simple curved smile
- Small arms and feet
- Pink blush marks

## Next Steps

To make it interactive:
- Add animations when touching action buttons
- Animate the character (blinking, bouncing)
- Add stats system (health, happiness, hunger)
- Connect to Sui blockchain for NFT interaction
- Add feeding/playing animations
- Save state to EEPROM

## Required Hardware

- ESP32-2424S012C board
- 1.28" GC9A01 display (240x240)
- CST816S touch controller
- Already configured in your board!

## Code Structure

```
setup()
  ├─ Initialize display (TFT_eSPI)
  ├─ Initialize touch (CST816S)
  └─ Show start screen

loop()
  ├─ Check for touch input
  ├─ Detect button press
  └─ Switch screens

drawStartScreen()
  ├─ Draw title with water drop
  ├─ Draw START button
  └─ Draw instructions

drawSuigotchiScreen()
  ├─ Draw header
  ├─ Draw gameboy frame
  ├─ Draw character
  ├─ Draw status bars
  └─ Draw action icons
```

## Customization

### Change Colors
Edit the color definitions at the top:
```cpp
#define COLOR_CYAN      0x07FF   // Change to any color
```

### Change Character
Modify `drawSuiCharacter()` function to customize:
- Body shape and size
- Eye style
- Mouth expression
- Arms and feet position

### Add More Screens
Add new `Screen` enum values and create draw functions:
```cpp
enum Screen {
  SCREEN_START,
  SCREEN_SUIGOTCHI,
  SCREEN_STATS,      // Add new screen
  SCREEN_SETTINGS    // Add another
};
```

Enjoy your pixel-art Suigotchi! 🎮💧
