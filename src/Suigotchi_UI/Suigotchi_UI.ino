/*
 * Suigotchi UI Demo for ESP32-2424S012C
 * Pixel art style interface with start button
 */

#include <TFT_eSPI.h>
#include "CST816S.h"

TFT_eSPI tft = TFT_eSPI();
CST816S touch(4, 5, 1, 0);  // sda, scl, rst, irq

// Colors
#define COLOR_BLACK     0x0000
#define COLOR_CYAN      0x07FF   // Sui water blue
#define COLOR_GRAY      0x7BEF
#define COLOR_DARKGRAY  0x39E7

// Screen states
enum Screen {
  SCREEN_START,
  SCREEN_SUIGOTCHI
};

Screen currentScreen = SCREEN_START;

// Button area for touch detection
struct Button {
  int x, y, w, h;
};

Button startButton = {70, 160, 100, 40};  // Start button position

void setup() {
  Serial.begin(115200);

  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.setSwapBytes(true);

  // Enable backlight
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);

  // Initialize touch
  touch.begin(FALLING);

  Serial.println("Suigotchi UI Starting...");

  // Show start screen
  drawStartScreen();
}

void loop() {
  // Check for touch
  if (touch.available()) {
    int touchX = touch.data.x;
    int touchY = touch.data.y;

    Serial.printf("Touch: X=%d, Y=%d\n", touchX, touchY);

    // Check if start button was touched
    if (currentScreen == SCREEN_START) {
      if (isTouched(startButton, touchX, touchY)) {
        Serial.println("Start button pressed!");
        currentScreen = SCREEN_SUIGOTCHI;
        drawSuigotchiScreen();
        delay(200);  // Debounce
      }
    } else if (currentScreen == SCREEN_SUIGOTCHI) {
      // Back to start if touch anywhere
      Serial.println("Going back to start...");
      currentScreen = SCREEN_START;
      drawStartScreen();
      delay(200);  // Debounce
    }
  }

  delay(10);
}

// ========================================
// Start Screen
// ========================================
void drawStartScreen() {
  tft.fillScreen(COLOR_BLACK);

  // Draw "Suigotchi" title with pixel font style
  tft.setTextColor(COLOR_CYAN);
  tft.setTextDatum(TC_DATUM);
  tft.setTextFont(4);  // Built-in font 4 has pixel-like appearance

  // Draw Sui water drop icon
  drawWaterDrop(90, 60, 20, COLOR_CYAN);

  // Title text
  tft.drawString("Suigotchi", 120, 50);

  // Subtitle
  tft.setTextFont(2);
  tft.setTextColor(COLOR_GRAY);
  tft.drawString("on Sui Blockchain", 120, 90);

  // Draw start button
  drawPixelButton(startButton.x, startButton.y, startButton.w, startButton.h,
                   "START", COLOR_CYAN, COLOR_BLACK);

  // Instructions
  tft.setTextFont(2);
  tft.setTextColor(COLOR_DARKGRAY);
  tft.drawString("Touch to start", 120, 210);
}

// ========================================
// Suigotchi Screen (based on your design)
// ========================================
void drawSuigotchiScreen() {
  tft.fillScreen(COLOR_BLACK);

  // Draw "Suigotchi" header with drop icon
  drawWaterDrop(30, 20, 12, COLOR_CYAN);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.drawString("Suigotchi", 50, 15);

  // Draw main display frame (like a gameboy screen)
  int frameX = 20;
  int frameY = 50;
  int frameW = 200;
  int frameH = 150;

  // Outer frame (dark)
  tft.fillRoundRect(frameX, frameY, frameW, frameH, 8, COLOR_DARKGRAY);

  // Inner screen (gray-green like original gameboy)
  tft.fillRoundRect(frameX + 5, frameY + 5, frameW - 10, frameH - 10, 5, 0x7BE0);

  // Draw the cute Sui character (simplified pixel blob)
  drawSuiCharacter(120, 120);

  // Bottom UI elements
  int bottomY = 205;

  // Heart icon (health)
  drawPixelHeart(30, bottomY, COLOR_CYAN);

  // Save icon
  drawSaveIcon(60, bottomY, COLOR_DARKGRAY);

  // Progress bars (health and happiness)
  drawProgressBar(90, bottomY, 60, 8, 75, 0x07E0);      // Green bar (75% full)
  drawProgressBar(160, bottomY, 60, 8, 40, COLOR_DARKGRAY); // Gray bar (40% full)

  // Bottom action icons
  int iconY = 225;
  drawWaterDrop(50, iconY, 10, COLOR_CYAN);    // Feed
  drawRefreshIcon(120, iconY, COLOR_CYAN);      // Play
  drawCloudIcon(190, iconY, COLOR_CYAN);        // Sleep

  // Touch anywhere message
  tft.setTextFont(1);
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(COLOR_DARKGRAY);
  tft.drawString("Touch anywhere to go back", 120, 238);
}

// ========================================
// Drawing Functions
// ========================================

void drawPixelButton(int x, int y, int w, int h, const char* text,
                     uint16_t bgColor, uint16_t textColor) {
  // Outer border (darker for pixel effect)
  tft.fillRect(x, y, w, h, COLOR_DARKGRAY);

  // Inner button
  tft.fillRect(x + 3, y + 3, w - 6, h - 6, bgColor);

  // Pixel-style border lines
  tft.drawFastHLine(x, y, w, COLOR_CYAN);
  tft.drawFastHLine(x, y + h - 1, w, COLOR_DARKGRAY);
  tft.drawFastVLine(x, y, h, COLOR_CYAN);
  tft.drawFastVLine(x + w - 1, y, h, COLOR_DARKGRAY);

  // Text
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(textColor);
  tft.drawString(text, x + w/2, y + h/2);
}

void drawWaterDrop(int x, int y, int size, uint16_t color) {
  // Pixel-style water drop (like Sui logo)
  tft.fillCircle(x, y + size/4, size * 0.7, color);
  tft.fillTriangle(x, y - size/2, x - size/2, y, x + size/2, y, color);
}

void drawSuiCharacter(int x, int y) {
  // Main body (cyan circle)
  tft.fillCircle(x, y, 35, COLOR_CYAN);

  // Black outline (pixel style)
  tft.drawCircle(x, y, 35, COLOR_BLACK);
  tft.drawCircle(x, y, 36, COLOR_BLACK);

  // Eyes (cute anime style)
  tft.fillRect(x - 15, y - 8, 8, 8, COLOR_BLACK);  // Left eye
  tft.fillRect(x + 7, y - 8, 8, 8, COLOR_BLACK);   // Right eye

  // Eye highlights (to make it cuter)
  tft.fillRect(x - 12, y - 6, 3, 3, 0x7BEF);       // Left highlight
  tft.fillRect(x + 10, y - 6, 3, 3, 0x7BEF);       // Right highlight

  // Mouth (simple smile curve using pixels)
  for (int i = 0; i < 15; i++) {
    int mx = x - 7 + i;
    int my = y + 5 + (i < 8 ? i/2 : (14-i)/2);
    tft.fillRect(mx, my, 2, 2, COLOR_BLACK);
  }

  // Cute blush marks
  tft.fillRect(x - 25, y + 2, 6, 3, 0xF9E6);       // Left blush
  tft.fillRect(x + 19, y + 2, 6, 3, 0xF9E6);       // Right blush

  // Arms (small stubs)
  tft.fillCircle(x - 35, y + 5, 8, COLOR_CYAN);    // Left arm
  tft.drawCircle(x - 35, y + 5, 8, COLOR_BLACK);

  tft.fillCircle(x + 35, y + 5, 8, COLOR_CYAN);    // Right arm
  tft.drawCircle(x + 35, y + 5, 8, COLOR_BLACK);

  // Feet
  tft.fillCircle(x - 15, y + 35, 8, COLOR_CYAN);   // Left foot
  tft.drawCircle(x - 15, y + 35, 8, COLOR_BLACK);

  tft.fillCircle(x + 15, y + 35, 8, COLOR_CYAN);   // Right foot
  tft.drawCircle(x + 15, y + 35, 8, COLOR_BLACK);
}

void drawPixelHeart(int x, int y, uint16_t color) {
  // Pixel heart (8x8 style)
  tft.fillRect(x + 2, y, 2, 2, color);
  tft.fillRect(x + 6, y, 2, 2, color);
  tft.fillRect(x, y + 2, 10, 4, color);
  tft.fillRect(x + 2, y + 6, 6, 2, color);
  tft.fillRect(x + 4, y + 8, 2, 2, color);
}

void drawSaveIcon(int x, int y, uint16_t color) {
  // Floppy disk icon (pixel style)
  tft.fillRect(x, y, 10, 10, color);
  tft.fillRect(x + 2, y, 6, 3, COLOR_BLACK);
  tft.fillRect(x + 2, y + 6, 6, 2, COLOR_BLACK);
}

void drawProgressBar(int x, int y, int w, int h, int percent, uint16_t color) {
  // Border
  tft.drawRect(x, y, w, h, COLOR_BLACK);

  // Background
  tft.fillRect(x + 1, y + 1, w - 2, h - 2, COLOR_DARKGRAY);

  // Progress
  int fillW = (w - 2) * percent / 100;
  tft.fillRect(x + 1, y + 1, fillW, h - 2, color);
}

void drawRefreshIcon(int x, int y, uint16_t color) {
  // Circular arrow (simplified)
  tft.drawCircle(x, y, 8, color);
  tft.fillTriangle(x + 6, y - 4, x + 8, y - 2, x + 6, y, color);
}

void drawCloudIcon(int x, int y, uint16_t color) {
  // Simple cloud shape
  tft.fillCircle(x - 4, y + 2, 4, color);
  tft.fillCircle(x, y, 5, color);
  tft.fillCircle(x + 4, y + 2, 4, color);
  tft.fillRect(x - 8, y + 2, 16, 4, color);
}

// ========================================
// Touch Helper
// ========================================
bool isTouched(Button btn, int x, int y) {
  return (x >= btn.x && x <= btn.x + btn.w &&
          y >= btn.y && y <= btn.y + btn.h);
}
