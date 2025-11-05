/*
 * Display Suigotchi Image using GUI_Paint (the working library!)
 */

#include <Arduino.h>
#include "LCD_Test.h"
#include "suigotchi_image_data.h"  // Converted image

UWORD Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
UWORD *BlackImage;
CST816S touch(4, 5, 1, 0);

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== Suigotchi Image Display ===");

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
    Serial.println("Hardware initialized");

    // Initialize LCD
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(BLACK);
    Serial.println("LCD ready!");

    // Copy the suigotchi image directly to BlackImage buffer
    Serial.println("Loading Suigotchi image...");
    memcpy(BlackImage, suigotchi_image_data, Imagesize);

    // Display it!
    LCD_1IN28_Display(BlackImage);

    Serial.println("Image displayed! Touch screen to cycle screens");
}

int screenMode = 0;

void loop()
{
    if (touch.available()) {
        Serial.println("Touch detected - switching screen");

        screenMode++;
        if (screenMode > 2) screenMode = 0;

        switch(screenMode) {
            case 0:
                // Show image
                memcpy(BlackImage, suigotchi_image_data, Imagesize);
                LCD_1IN28_Display(BlackImage);
                Serial.println("Showing Suigotchi image");
                break;

            case 1:
                // Show black screen
                LCD_1IN28_Clear(BLACK);
                Serial.println("Black screen");
                break;

            case 2:
                // Show test colors
                Paint_NewImage((UBYTE *)BlackImage, 240, 240, 0, WHITE);
                Paint_Clear(WHITE);
                Paint_DrawString_EN(40, 100, "Suigotchi!", &Font24, BLACK, 0x87CE);
                LCD_1IN28_Display(BlackImage);
                Serial.println("Text screen");
                break;
        }

        delay(300);  // Debounce
    }

    delay(10);
}
