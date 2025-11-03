# 🎮 Suigotchi Demo

Pixel-art Tamagotchi-style UI for ESP32-2424S012C, based on your Suigotchi design!

## ✨ Features

### Start Screen:
- Black background
- Cyan "Suigotchi" title with water drop logo
- Blue pixel-style START button (black text)
- Touch the button to enter

### Suigotchi Screen:
- Gameboy-style green display frame
- Cute Sui character (cyan blob with eyes, arms, feet)
- Health heart icon
- Save disk icon
- Two progress bars (health 75%, happiness 40%)
- Three action icons: Feed (water), Play (refresh), Sleep (cloud)
- Touch anywhere to go back

## 🎯 How to Use

1. Open `Suigotchi_Demo.ino` in Arduino IDE
2. Board: **ESP32C3 Dev Module**
3. Upload
4. Touch START button on display
5. Touch screen again to go back

## 📦 What's Included

This folder is a **complete copy** of the working test folder, so it has:
- ✅ All libraries (CST816S, LCD_1in28, etc.)
- ✅ Correct pin configuration for ESP32-2424S012C
- ✅ Touch support enabled
- ✅ All helper functions

## 🎨 UI Elements

All drawn using the Paint library (no external images):
- Water drop icon (Sui logo)
- Pixel-style buttons
- Cute character (circles + rectangles)
- Hearts, clouds, refresh icons
- Progress bars
- Pixel fonts

## 🔧 Customize

Want to change the character? Edit `drawSuiCharacter()` function!
Want different colors? Change the `COLOR_*` defines at top!
Want more screens? Add to the `Screen` enum!

**Everything works because we copied from the tested folder!** 🎉
