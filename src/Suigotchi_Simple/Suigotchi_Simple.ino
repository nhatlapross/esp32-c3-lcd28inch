/*
 * Suigotchi - Display your image using raw RGB565 data!
 */

#include <Arduino.h>
#include "LCD_Test.h"
#include "suigotchi_raw.h"

UWORD Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
UWORD *BlackImage;
CST816S touch(4, 5, 1, 0);

// No callback needed - we're using raw data!

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== Suigotchi PNG Display ===");

    // Initialize touch
    touch.begin(FALLING);

    // Allocate memory
    if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL){
        Serial.println("Memory allocation failed!");
        while(1);
    }
    Serial.println("Memory allocated");

    // Initialize hardware
    if (DEV_Module_Init() != 0) {
        Serial.println("Init failed!");
        while(1);
    }

    Serial.println("Calling LCD_1IN28_Init...");
    LCD_1IN28_Init(HORIZONTAL);
    Serial.println("Init done, clearing to BLACK...");
    LCD_1IN28_Clear(BLACK);
    Serial.println("LCD initialized!");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());

    // Copy image data from PROGMEM to BlackImage with byte swap
    Serial.println("Loading Suigotchi image from memory...");
    for (int i = 0; i < 57600; i++) {
        uint16_t pixel = pgm_read_word(&suigotchi_img[i]);
        // Swap bytes for correct color order
        BlackImage[i] = (pixel >> 8) | (pixel << 8);
    }
    Serial.println("Image loaded with byte swap!");

    // Display it!
    LCD_1IN28_Display(BlackImage);
    Serial.println("Image displayed! Touch to cycle screens");
}

int screenMode = 0;

void loop()
{
    if (touch.available()) {
        Serial.println("Touch detected");
        screenMode++;
        if (screenMode > 1) screenMode = 0;

        if (screenMode == 0) {
            // Show image with byte swap
            for (int i = 0; i < 57600; i++) {
                uint16_t pixel = pgm_read_word(&suigotchi_img[i]);
                BlackImage[i] = (pixel >> 8) | (pixel << 8);
            }
            LCD_1IN28_Display(BlackImage);
            Serial.println("Showing Suigotchi image");
        } else {
            // Show black screen with text
            LCD_1IN28_Clear(BLACK);
            Paint_NewImage((UBYTE *)BlackImage, 240, 240, 0, BLACK);
            Paint_DrawString_EN(30, 110, "Touch to show", &Font16, BLACK, 0x87CE);
            Paint_DrawString_EN(60, 130, "image again!", &Font16, BLACK, 0x87CE);
            LCD_1IN28_Display(BlackImage);
            Serial.println("Text screen");
        }

        delay(300);
    }
    delay(10);
}
