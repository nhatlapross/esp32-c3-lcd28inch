#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("TFT_eSPI Simple Test");

  // Enable backlight
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  Serial.println("Backlight ON");

  Serial.println("Calling tft.init()...");
  tft.init();

  Serial.println("TFT init done!");
  Serial.print("Width: "); Serial.println(tft.width());
  Serial.print("Height: "); Serial.println(tft.height());

  Serial.println("Filling screen RED...");
  tft.fillScreen(TFT_RED);
  delay(1000);

  Serial.println("Filling screen GREEN...");
  tft.fillScreen(TFT_GREEN);
  delay(1000);

  Serial.println("Filling screen BLUE...");
  tft.fillScreen(TFT_BLUE);
  delay(1000);

  Serial.println("Drawing text...");
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("TFT_eSPI", 60, 110);

  Serial.println("Test complete!");
}

void loop() {
  delay(1000);
}
