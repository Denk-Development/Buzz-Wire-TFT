#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// custom label class
#include "Label.cpp"

#define DEBUG

// screen pinning
const uint8_t TFT_RST = 8, TFT_DC = 9, TFT_CS = 10, TFT_MOSI = 11, TFT_MISO = 12, TFT_CLK = 13;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup()
{
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("running...");
  #endif
  
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  Label test(&tft, 50, 50, ILI9341_WHITE, ILI9341_BLACK, 2, "DJ Simsso");
  delay(2000);
  test.setText("is in da house");
  delay(2000);
  test.setColor(ILI9341_YELLOW);
}

void loop()
{
  
}

