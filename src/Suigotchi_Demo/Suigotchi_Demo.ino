/*
 * Suigotchi UI Demo for ESP32-2424S012C
 * Based on working ESP32-C3 test code
 * Pixel art style Tamagotchi interface
 */

#include <Arduino.h>
#include "LCD_Test.h"

UWORD Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
UWORD *BlackImage;

// ESP32-2424S012C Pin Configuration
CST816S touch(4, 5, 1, 0);  // sda, scl, rst, irq

// Colors for Suigotchi UI (RGB565 format)
#define COLOR_BLACK     0x0000
#define COLOR_CYAN      0x07FF   // Sui water blue
#define COLOR_GRAY      0x7BEF
#define COLOR_DARKGRAY  0x39E7
#define COLOR_GREEN     0x07E0
#define COLOR_GAMEBOY   0x9E66   // Gameboy green

// Screen states
enum Screen {
  SCREEN_START,
  SCREEN_SUIGOTCHI
};

Screen currentScreen = SCREEN_START;

// Button for touch detection
struct ButtonArea {
  int x, y, w, h;
};

ButtonArea startButton = {70, 160, 100, 40};

// Function declarations
void drawStartScreen();
void drawSuigotchiScreen();
void drawPixelButton(int x, int y, int w, int h, uint16_t color);
void drawWaterDrop(int x, int y, int size, uint16_t color);
void drawSuiCharacter(int x, int y);
void drawPixelHeart(int x, int y, uint16_t color);
void drawSaveIcon(int x, int y, uint16_t color);
void drawProgressBar(int x, int y, int w, int h, int percent, uint16_t color);
void drawRefreshIcon(int x, int y, uint16_t color);
void drawCloudIcon(int x, int y, uint16_t color);
bool isTouched(ButtonArea btn, int x, int y);

void setup()
{
    Serial.begin(115200);
    Serial.println("\n\nSuigotchi Demo Starting...");

    // ESP32-C3 Memory Initialize
    Serial.println("ESP32-C3: Using regular SRAM (no PSRAM)");

    if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL){
        Serial.println("Failed to allocate memory...");
        exit(0);
    }
    Serial.printf("Allocated %d bytes\n", Imagesize);

    // Initialize hardware
    if (DEV_Module_Init() != 0)
      Serial.println("GPIO Init Fail!");
    else
      Serial.println("GPIO Init successful!");

    // Initialize touch
    Serial.println("(I2C errors during init are normal)");
    touch.begin(FALLING);
    Serial.println("Touch ready!");

    // Initialize LCD
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(BLACK);

    Paint_NewImage((UBYTE *)BlackImage, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);

    Serial.println("Display initialized!");

    // Show start screen
    drawStartScreen();
    Serial.println("Touch START button to begin!");
}

void loop()
{
    // Check for touch
    if (touch.available()) {
        int touchX = touch.data.x;
        int touchY = touch.data.y;

        Serial.printf("Touch: X=%d, Y=%d\n", touchX, touchY);

        if (currentScreen == SCREEN_START) {
            // Check if start button was touched
            if (isTouched(startButton, touchX, touchY)) {
                Serial.println("START button pressed!");
                currentScreen = SCREEN_SUIGOTCHI;
                drawSuigotchiScreen();
                delay(300);  // Debounce
            }
        } else if (currentScreen == SCREEN_SUIGOTCHI) {
            // Touch anywhere to go back
            Serial.println("Going back to start...");
            currentScreen = SCREEN_START;
            drawStartScreen();
            delay(300);  // Debounce
        }
    }

    delay(10);
}

// ========================================
// Start Screen
// ========================================
void drawStartScreen() {
    Paint_Clear(COLOR_BLACK);

    // Draw Sui water drop logo
    drawWaterDrop(90, 60, 18, COLOR_CYAN);

    // Title "Suigotchi"
    Paint_DrawString_EN(55, 50, "Suigotchi", &Font24, COLOR_BLACK, COLOR_CYAN);

    // Subtitle
    Paint_DrawString_EN(35, 90, "on Sui Blockchain", &Font16, COLOR_BLACK, COLOR_GRAY);

    // START button (pixel style)
    drawPixelButton(startButton.x, startButton.y, startButton.w, startButton.h, COLOR_CYAN);
    Paint_DrawString_EN(85, 172, "START", &Font20, COLOR_CYAN, COLOR_BLACK);

    // Instructions
    Paint_DrawString_EN(50, 210, "Touch to start", &Font16, COLOR_BLACK, COLOR_DARKGRAY);

    // Display
    LCD_1IN28_Display(BlackImage);
    Serial.println("Start screen drawn");
}

// ========================================
// Suigotchi Screen
// ========================================
void drawSuigotchiScreen() {
    Paint_Clear(COLOR_BLACK);

    // Header with logo
    drawWaterDrop(30, 20, 12, COLOR_CYAN);
    Paint_DrawString_EN(50, 15, "Suigotchi", &Font16, COLOR_BLACK, COLOR_CYAN);

    // Main gameboy-style frame
    int frameX = 20;
    int frameY = 50;
    int frameW = 200;
    int frameH = 150;

    // Outer frame (dark gray border)
    Paint_DrawRectangle(frameX, frameY, frameX + frameW, frameY + frameH,
                        COLOR_DARKGRAY, DOT_PIXEL_3X3, DRAW_FILL_FULL);

    // Inner screen (gameboy green)
    Paint_DrawRectangle(frameX + 5, frameY + 5, frameX + frameW - 5, frameY + frameH - 5,
                        COLOR_GAMEBOY, DOT_PIXEL_2X2, DRAW_FILL_FULL);

    // Draw the cute Sui character
    drawSuiCharacter(120, 120);

    // Bottom UI - Y position for icons
    int bottomY = 205;

    // Heart icon (health indicator)
    drawPixelHeart(30, bottomY, COLOR_CYAN);

    // Save/disk icon
    drawSaveIcon(60, bottomY, COLOR_DARKGRAY);

    // Progress bars (health and happiness)
    drawProgressBar(90, bottomY, 60, 8, 75, COLOR_GREEN);       // 75% health
    drawProgressBar(160, bottomY, 60, 8, 40, COLOR_DARKGRAY);   // 40% happiness

    // Bottom action icons row
    int iconY = 225;
    drawWaterDrop(50, iconY, 8, COLOR_CYAN);     // Feed action
    drawRefreshIcon(120, iconY, COLOR_CYAN);     // Play action
    drawCloudIcon(190, iconY, COLOR_CYAN);       // Sleep action

    // Instructions at bottom
    Paint_DrawString_EN(30, 235, "Touch to go back", &Font12, COLOR_BLACK, COLOR_DARKGRAY);

    // Display
    LCD_1IN28_Display(BlackImage);
    Serial.println("Suigotchi screen drawn");
}

// ========================================
// Drawing Helper Functions
// ========================================

void drawPixelButton(int x, int y, int w, int h, uint16_t color) {
    // Outer dark border
    Paint_DrawRectangle(x, y, x + w, y + h, COLOR_DARKGRAY, DOT_PIXEL_3X3, DRAW_FILL_FULL);

    // Inner colored area
    Paint_DrawRectangle(x + 3, y + 3, x + w - 3, y + h - 3, color, DOT_PIXEL_2X2, DRAW_FILL_FULL);

    // Pixel-style highlight lines
    Paint_DrawLine(x, y, x + w, y, COLOR_CYAN, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(x, y, x, y + h, COLOR_CYAN, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
}

void drawWaterDrop(int x, int y, int size, uint16_t color) {
    // Water drop shape - circle for main body
    Paint_DrawCircle(x, y + size/4, size * 0.7, color, DOT_PIXEL_2X2, DRAW_FILL_FULL);

    // Top point of drop (filled circle for simplicity)
    Paint_DrawCircle(x, y - size/4, size/3, color, DOT_PIXEL_2X2, DRAW_FILL_FULL);
}

void drawSuiCharacter(int x, int y) {
    // Main body (big cyan circle)
    Paint_DrawCircle(x, y, 35, COLOR_CYAN, DOT_PIXEL_3X3, DRAW_FILL_FULL);
    Paint_DrawCircle(x, y, 36, COLOR_BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

    // Eyes (rectangular, anime style)
    Paint_DrawRectangle(x - 15, y - 8, x - 7, y, COLOR_BLACK, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawRectangle(x + 7, y - 8, x + 15, y, COLOR_BLACK, DOT_PIXEL_2X2, DRAW_FILL_FULL);

    // Eye highlights (small white dots)
    Paint_DrawPoint(x - 12, y - 6, WHITE, DOT_PIXEL_2X2, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(x + 10, y - 6, WHITE, DOT_PIXEL_2X2, DOT_FILL_RIGHTUP);

    // Smile (simple line)
    Paint_DrawLine(x - 10, y + 8, x + 10, y + 8, COLOR_BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(x - 7, y + 10, x + 7, y + 10, COLOR_BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

    // Arms (small circles on sides)
    Paint_DrawCircle(x - 35, y + 5, 7, COLOR_CYAN, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawCircle(x - 35, y + 5, 8, COLOR_BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

    Paint_DrawCircle(x + 35, y + 5, 7, COLOR_CYAN, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawCircle(x + 35, y + 5, 8, COLOR_BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

    // Feet (small circles at bottom)
    Paint_DrawCircle(x - 15, y + 35, 7, COLOR_CYAN, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawCircle(x - 15, y + 35, 8, COLOR_BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

    Paint_DrawCircle(x + 15, y + 35, 7, COLOR_CYAN, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawCircle(x + 15, y + 35, 8, COLOR_BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
}

void drawPixelHeart(int x, int y, uint16_t color) {
    // Pixel heart shape
    Paint_DrawPoint(x + 2, y, color, DOT_PIXEL_2X2, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(x + 6, y, color, DOT_PIXEL_2X2, DOT_FILL_RIGHTUP);
    Paint_DrawRectangle(x, y + 2, x + 10, y + 6, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawRectangle(x + 2, y + 6, x + 8, y + 8, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawPoint(x + 4, y + 8, color, DOT_PIXEL_2X2, DOT_FILL_RIGHTUP);
}

void drawSaveIcon(int x, int y, uint16_t color) {
    // Floppy disk icon
    Paint_DrawRectangle(x, y, x + 10, y + 10, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawRectangle(x + 2, y, x + 8, y + 3, COLOR_BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawRectangle(x + 2, y + 6, x + 8, y + 8, COLOR_BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

void drawProgressBar(int x, int y, int w, int h, int percent, uint16_t color) {
    // Border
    Paint_DrawRectangle(x, y, x + w, y + h, COLOR_BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

    // Background
    Paint_DrawRectangle(x + 1, y + 1, x + w - 1, y + h - 1,
                        COLOR_DARKGRAY, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Progress fill
    int fillW = (w - 2) * percent / 100;
    if (fillW > 0) {
        Paint_DrawRectangle(x + 1, y + 1, x + 1 + fillW, y + h - 1,
                            color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }
}

void drawRefreshIcon(int x, int y, uint16_t color) {
    // Circular arrow (simplified)
    Paint_DrawCircle(x, y, 8, color, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    // Arrow head (using lines instead of triangle)
    Paint_DrawLine(x + 6, y - 4, x + 8, y - 2, color, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(x + 6, y, x + 8, y - 2, color, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
}

void drawCloudIcon(int x, int y, uint16_t color) {
    // Cloud shape (three overlapping circles)
    Paint_DrawCircle(x - 4, y + 2, 4, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawCircle(x, y, 5, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawCircle(x + 4, y + 2, 4, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    // Bottom fill
    Paint_DrawRectangle(x - 8, y + 2, x + 8, y + 6, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

// ========================================
// Touch Helper
// ========================================
bool isTouched(ButtonArea btn, int x, int y) {
    bool hit = (x >= btn.x && x <= btn.x + btn.w &&
                y >= btn.y && y <= btn.y + btn.h);
    if (hit) {
        Serial.printf("Button hit! Area: (%d,%d,%d,%d), Touch: (%d,%d)\n",
                      btn.x, btn.y, btn.w, btn.h, x, y);
    }
    return hit;
}
