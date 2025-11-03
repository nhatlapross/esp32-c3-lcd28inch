// Simple TFT test - no LVGL
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n\nTFT Test Starting...");

  // Backlight
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  Serial.println("Backlight ON");

  // Init TFT
  tft.init();
  Serial.println("TFT init done");

  tft.fillScreen(TFT_RED);
  Serial.println("RED");
  delay(1000);

  tft.fillScreen(TFT_GREEN);
  Serial.println("GREEN");
  delay(1000);

  tft.fillScreen(TFT_BLUE);
  Serial.println("BLUE");
  delay(1000);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2);
  tft.drawString("Works!", 80, 110);
  Serial.println("Test complete!");
}

void loop() {
  delay(1000);
}
