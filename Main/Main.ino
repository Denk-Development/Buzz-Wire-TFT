#include<stdlib.h>

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

// game constants
const unsigned long minGameTimeMillis = 1000;

// screen constants
const int paddingLeft = 10; // for non-centered labels

// screen pinning
const uint8_t TFT_RST = 8, TFT_DC = 9, TFT_CS = 10, TFT_MOSI = 11, TFT_MISO = 12, TFT_CLK = 13;

// buzz wire pinning
const uint8_t startStopPin = 2;

const int BG_COLOR = ILI9341_BLACK;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

Label *lblTitle, *lblSubtitle, *lblPenaltyTimeTitle, *lblPenaltyTime, *lblTime, *lblMistakes, *lblTotal, *lblTimeValue, *lblMistakesValue, *lblTotalValue;

GameState gameState = GameState::Start;

int penaltyTime = 5; // 5 seconds by default

String floatToString(float x, byte precision = 2) {
  char tmp[50];
  dtostrf(x, 0, precision, tmp);
  return String(tmp);
}

void setup()
{
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("running...");
  #endif

  // buzz wire pins
  pinMode(startStopPin, INPUT_PULLUP);
  
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  lblTitle = new Label(&tft, 10, ILI9341_RED, BG_COLOR, 3, "KreativeKiste.de");
  lblSubtitle = new Label(&tft, lblTitle->getBottomY() + 10, ILI9341_RED, BG_COLOR, 2, "Heisser Draht");

  // start
  lblPenaltyTimeTitle = new Label(&tft, lblSubtitle->getBottomY() + 30, ILI9341_WHITE, BG_COLOR, 2, "Strafzeit einstellen", true);
  lblPenaltyTime = new Label(&tft, lblPenaltyTimeTitle->getBottomY() + 20, ILI9341_WHITE, BG_COLOR, 3, String(penaltyTime) + " s", true);

  // running
  lblTime = new Label(&tft, paddingLeft, lblSubtitle->getBottomY() + 30, ILI9341_WHITE, BG_COLOR, 3, "Zeit:", true);
  lblMistakes = new Label(&tft, paddingLeft, lblTime->getBottomY() + 10, ILI9341_WHITE, BG_COLOR, 3, "Fehler:", true);
  lblTotal = new Label(&tft, paddingLeft, lblMistakes->getBottomY() + 10, ILI9341_WHITE, BG_COLOR, 3, "Gesamt:", true);
  lblTimeValue = new Label(&tft, lblTotal->getRightX() + 10, lblSubtitle->getBottomY() + 30, ILI9341_WHITE, BG_COLOR, 3, "", true);
  lblMistakesValue = new Label(&tft, lblTotal->getRightX() + 10, lblTime->getBottomY() + 10, ILI9341_WHITE, BG_COLOR, 3, "", true);
  lblTotalValue = new Label(&tft, lblTotal->getRightX() + 10, lblMistakes->getBottomY() + 10, ILI9341_WHITE, BG_COLOR, 3, "", true);

  // over
  
  delay(2000);
}

void loop()
{
  // START
  #ifdef DEBUG
    Serial.println("GameState::Start");
  #endif
  lblPenaltyTimeTitle->show();
  lblPenaltyTime->show();
  while (gameState == GameState::Start) {
    penaltyTime = readPenaltyTime();
    lblPenaltyTime->setText(String(penaltyTime) + " s"); // updateLabel

    // exit loop
    if (!digitalRead(startStopPin)) { // LOW pin starts game
      gameState = GameState::Running;
    }
  }
  lblPenaltyTimeTitle->hide();
  lblPenaltyTime->hide();


  // RUNNING
  #ifdef DEBUG
    Serial.println("GameState::Running");
  #endif
  unsigned long startMillis = millis(); 
  unsigned int mistakesCount = 0;
  lblTime->show();
  lblMistakes->show();
  lblTotal->show();
  lblTimeValue->show();
  lblMistakesValue->show();
  lblTotalValue->show();
  while (gameState == GameState::Running) {
    double secondsElapsed = (double)(millis() - startMillis) / (double)1000;
    lblTimeValue->setText(floatToString(secondsElapsed, 1) + " s");
    lblMistakesValue->setText(String(mistakesCount));
    lblTotalValue->setText(floatToString(secondsElapsed * mistakesCount, 1) + " s");

    // exit loop
    if (!digitalRead(startStopPin) && millis() > startMillis + minGameTimeMillis) { // LOW pin stops game and min time elapsed
      gameState = GameState::Over;
    }
  }


  // OVER
  #ifdef DEBUG
    Serial.println("GameState::Over");
  #endif
  unsigned long endMillis = millis();
  while (gameState == GameState::Over) {
    
  }
}

int readPenaltyTime() {
  return 5; // TODO
}

