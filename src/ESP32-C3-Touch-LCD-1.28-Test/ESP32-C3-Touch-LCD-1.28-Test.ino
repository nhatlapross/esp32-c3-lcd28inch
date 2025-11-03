#include <Arduino.h>
#include "LCD_Test.h"

UWORD Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
UWORD *BlackImage;

// ESP32-2424S012C Pin Configuration (WITH CAPACITIVE TOUCH)
// Touch controller: CST816S on I2C
// Pins: SDA=GPIO4, SCL=GPIO5, RST=GPIO1, IRQ=GPIO0
CST816S touch(4, 5, 1, 0);  // sda, scl, rst, irq (correct pins for ESP32-2424S012C)

void setup()
{
    Serial.begin(115200);
    Serial.println("\n\nESP32-2424S012C Starting...");

    // ESP32-C3 Memory Initialize (no PSRAM support)
    Serial.println("ESP32-C3: Using regular SRAM (no PSRAM)");

    // Use regular malloc instead of ps_malloc for ESP32-C3
    if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL){
        Serial.println("Failed to allocate memory...");
        Serial.printf("Tried to allocate %d bytes\n", Imagesize);
        exit(0);
    }
    Serial.printf("Successfully allocated %d bytes\n", Imagesize);

    // Initialize hardware (SPI, I2C, GPIO) - This sets up I2C bus
    if (DEV_Module_Init() != 0)
      Serial.println("GPIO Init Fail!");
    else
      Serial.println("GPIO Init successful!");

    // Initialize touch controller AFTER I2C is set up
    Serial.println("Initializing touch controller...");
    Serial.println("(I2C errors during init are normal - touch will still work!)");
    touch.begin(FALLING);  // Use FALLING edge interrupt
    Serial.println("Touch controller ready!");
    Serial.printf("Touch I2C Address: 0x%02X\n", 0x15);
    Serial.printf("Touch pins: SDA=%d, SCL=%d, RST=%d, IRQ=%d\n", 4, 5, 1, 0);
      LCD_1IN28_Init(HORIZONTAL);
      LCD_1IN28_Clear(WHITE);
      /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
      Paint_NewImage((UBYTE *)BlackImage, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0, WHITE);
      Paint_SetScale(65);
      Paint_SetRotate(ROTATE_0);
      Paint_Clear(WHITE);
      // /* GUI */
      Serial.println("drawing...\r\n");
      delay(2000);
      LCD_1IN28_Clear(RED);
      delay(2000);
      LCD_1IN28_Clear(GREEN);
      delay(2000);
      LCD_1IN28_Clear(BLUE);
      delay(2000);

      // /*2.Drawing on the image*/
#if 1
      Paint_Clear(WHITE);
      Paint_DrawPoint(50, 41, BLACK, DOT_PIXEL_1X1, DOT_FILL_RIGHTUP); // 240 240
      Paint_DrawPoint(50, 46, BLACK, DOT_PIXEL_2X2, DOT_FILL_RIGHTUP);
      Paint_DrawPoint(50, 51, BLACK, DOT_PIXEL_3X3, DOT_FILL_RIGHTUP);
      Paint_DrawPoint(50, 56, BLACK, DOT_PIXEL_4X4, DOT_FILL_RIGHTUP);
      Paint_DrawPoint(50, 61, BLACK, DOT_PIXEL_5X5, DOT_FILL_RIGHTUP);

      Paint_DrawLine(60, 40, 90, 70, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
      Paint_DrawLine(60, 70, 90, 40, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

      Paint_DrawRectangle(60, 40, 90, 70, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
      Paint_DrawRectangle(100, 40, 130, 70, BLUE, DOT_PIXEL_2X2, DRAW_FILL_FULL);

      Paint_DrawLine(135, 55, 165, 55, CYAN, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
      Paint_DrawLine(150, 40, 150, 70, CYAN, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

      Paint_DrawCircle(150, 55, 15, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      Paint_DrawCircle(185, 55, 15, GREEN, DOT_PIXEL_1X1, DRAW_FILL_FULL);

      Paint_DrawNum(50, 80, 9.87654321, &Font20, 3, WHITE, BLACK);
      Paint_DrawString_EN(50, 100, "ABC", &Font20, 0x000f, 0xfff0);
      Paint_DrawString_EN(50, 161, "Waveshare", &Font16, RED, WHITE);

      // /*3.Refresh the picture in RAM to LCD*/
      LCD_1IN28_Display(BlackImage);
      DEV_Delay_ms(1000);
#endif

#if 1
      float acc[3], gyro[3];
      unsigned int tim_count = 0;
      uint16_t result;

      QMI8658_init();
      Serial.println("QMI8658_init\r\n");
      // DEV_SET_PWM(100);
      const float conversion_factor = 3.3f / (1 << 12) * 3;
      Paint_Clear(WHITE);
      Paint_DrawRectangle(0, 00, 240, 47, 0XF410, DOT_PIXEL_2X2, DRAW_FILL_FULL);
      Paint_DrawRectangle(0, 47, 240, 120, 0X4F30, DOT_PIXEL_2X2, DRAW_FILL_FULL);
      Paint_DrawRectangle(0, 120, 240, 195, 0XAD55, DOT_PIXEL_2X2, DRAW_FILL_FULL);
      Paint_DrawRectangle(0, 195, 240, 240, 0X2595, DOT_PIXEL_2X2, DRAW_FILL_FULL);

      Paint_DrawString_EN(45, 30, "LongPress Quit", &Font16, WHITE, BLACK);
      Paint_DrawString_EN(45, 50, "ACC_X = ", &Font16, WHITE, BLACK);
      Paint_DrawString_EN(45, 75, "ACC_Y = ", &Font16, WHITE, BLACK);
      Paint_DrawString_EN(45, 100, "ACC_Z = ", &Font16, WHITE, BLACK);
      Paint_DrawString_EN(45, 125, "GYR_X = ", &Font16, WHITE, BLACK);
      Paint_DrawString_EN(45, 150, "GYR_Y = ", &Font16, WHITE, BLACK);
      Paint_DrawString_EN(45, 175, "GYR_Z = ", &Font16, WHITE, BLACK);
      Paint_DrawString_EN(45, 200, "BAT(V)=", &Font16, WHITE, BLACK);
      LCD_1IN28_Display(BlackImage);
      while (true)
      {
          result = DEC_ADC_Read();
          QMI8658_read_xyz(acc, gyro, &tim_count);
          Paint_Clear(WHITE);
          Paint_DrawRectangle(120, 47,  220, 120, 0X4F30, DOT_PIXEL_2X2, DRAW_FILL_FULL);
          Paint_DrawRectangle(120, 120, 220, 195, 0XAD55, DOT_PIXEL_2X2, DRAW_FILL_FULL);
          Paint_DrawRectangle(120, 195, 220, 240, 0X2595, DOT_PIXEL_2X2, DRAW_FILL_FULL);
          Paint_DrawNum(120, 50, acc[0], &Font16, 2, BLACK, WHITE);
          Paint_DrawNum(120, 75, acc[1], &Font16, 2, BLACK, WHITE);
          Paint_DrawNum(120, 100, acc[2], &Font16, 2, BLACK, WHITE);
          Paint_DrawNum(120, 125, gyro[0], &Font16, 2, BLACK, WHITE);
          Paint_DrawNum(120, 150, gyro[1], &Font16, 2, BLACK, WHITE);
          Paint_DrawNum(120, 175, gyro[2], &Font16, 2, BLACK, WHITE);
          Paint_DrawNum(130, 200, result * conversion_factor, &Font16, 2, BLACK, WHITE);
          LCD_1IN28_DisplayWindows(120, 50, 210, 200, BlackImage);
          LCD_1IN28_DisplayWindows(130, 200, 220, 220, BlackImage);

          // Touch to quit - tap top of screen
          if (touch.available()){
            if(touch.data.y<45){
              Serial.println("Touch detected - exiting IMU test");
              break;
            }
          }

          delay(50);  // Small delay for display updates
      }
#endif

    delay(2000);

// Touch test - Draw on screen with your finger!
#if 1  // Touch is enabled for ESP32-2424S012C
    Paint_Clear(WHITE);
    Paint_DrawRectangle(0, 00, 240, 47, 0x2595, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawString_EN(45, 30, "Touch to Draw!", &Font16, WHITE, BLACK);
    LCD_1IN28_Display(BlackImage);

    Serial.println("\nTouch test started - draw on the screen!");
    Serial.println("Waiting for touch input...");
    unsigned long lastTouch = millis();
    unsigned long lastDebug = millis();
    int touchCount = 0;

    while (millis() - lastTouch < 30000)  // Run for 30 seconds
    {
        // Debug output every 2 seconds
        if (millis() - lastDebug > 2000) {
          Serial.printf("Touch test running... (touches: %d, GPIO0 state: %d)\n",
                        touchCount, digitalRead(0));
          lastDebug = millis();
        }

        if (touch.available()){
          touchCount++;
          Serial.printf("Touch #%d: X=%d, Y=%d, Event=%d\n",
                        touchCount, touch.data.x, touch.data.y, touch.data.event);
          Paint_DrawPoint(touch.data.x, touch.data.y, BLACK, DOT_PIXEL_3X3, DOT_FILL_RIGHTUP);
          LCD_1IN28_DisplayWindows(touch.data.x, touch.data.y, touch.data.x + 3, touch.data.y + 3, BlackImage);
          lastTouch = millis();  // Reset timer on touch
        }
        delay(10);  // Small delay to prevent CPU hogging
    }

    Serial.printf("Touch test complete. Total touches detected: %d\n", touchCount);
#endif

    // Test complete message
    Paint_Clear(WHITE);
    Paint_DrawString_EN(30, 100, "ESP32-2424S012C", &Font20, BLACK, WHITE);
    Paint_DrawString_EN(50, 130, "Touch Test Done!", &Font16, GREEN, WHITE);
    LCD_1IN28_Display(BlackImage);

    Serial.println("\nAll tests completed!");
    Serial.println("ESP32-2424S012C with touch running successfully!");
}

void loop()
{
  delay(2);//allow the cpu to switch to other tasks
}
