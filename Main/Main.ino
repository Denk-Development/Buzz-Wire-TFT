#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// custom label class
#include "Label.cpp"

#define DEBUG

enum GameState {
  Start,
  Running,
  Over
};

// screen pinning
const uint8_t TFT_RST = 8, TFT_DC = 9, TFT_CS = 10, TFT_MOSI = 11, TFT_MISO = 12, TFT_CLK = 13;

const int BG_COLOR = ILI9341_BLACK;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

Label *title, *subtitle, *penaltyTimeTitle, *penaltyTime;

GameState gameState = GameState::Start;

int penaltyTimeValue = 5; // 5 seconds by default

void setup()
{
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("running...");
  #endif
  
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  title = new Label(&tft, 10, ILI9341_RED, BG_COLOR, 3, "KreativeKiste.de", false);
  subtitle = new Label(&tft, title->getBottomY() + 10, ILI9341_RED, BG_COLOR, 2, "Heisser Draht", false);
  penaltyTimeTitle = new Label(&tft, subtitle->getBottomY() + 30, ILI9341_WHITE, BG_COLOR, 2, "Strafzeit einstellen", true);
  penaltyTime = new Label(&tft, penaltyTimeTitle->getBottomY() + 20, ILI9341_WHITE, BG_COLOR, 3, String(penaltyTimeValue) + " s", true);
  delay(2000);
}

void loop()
{
  penaltyTimeTitle->show();
  penaltyTime->show();
  while (gameState == GameState::Start) {
    penaltyTime->setText(String(analogRead(A0)) + " s"); // updateLabel
  }
}

