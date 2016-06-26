#include <stdlib.h>

// screen
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// touch
#include "TFT_Touch.h"

// custom classes
#include "Label.cpp"
#include "Keyboard.cpp"

#define DEBUG

enum GameState {
  Init,
  Start,
  GameTypeSelection,
  NameEntry,
  Waiting,
  Running,
  Over,
  Ranks,
  Scoreboard
};

enum GameType {
  Choice,
  Single,
  Multi
};


// PINNING

// screen pinning
const uint8_t TFT_RST = 8, TFT_DC = 9, TFT_CS = 10, TFT_MOSI = 11, TFT_MISO = 12, TFT_CLK = 13;

// touch pinning
const uint8_t DOUT = A0, DIN = A2, DCS = 7, DCLK = 6;

// buzz wire pinning
const uint8_t startStopPin = 2, mistakePin = 3;


// game constants
const unsigned long minGameTimeMillis = 1000, minTimeBetweenTwoMistakes = 1000, autoRestartMillis = 10000, minGameOverScreenMillis = minGameTimeMillis, minTimeBetweenButtonClick = 300;

// screen constants
const int paddingLeft = 10; // for non-centered labels

const int BG_COLOR = ILI9341_BLACK;


const int scoreboardLength = 4;
String currentPlayerName;
String topRanksName[scoreboardLength];
double currentPlayerTime;
double topRanksTime[scoreboardLength];

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
TFT_Touch touch = TFT_Touch(DCS, DCLK, DIN, DOUT);

Label *lblTitle, *lblSubtitle, 
  *lblPenaltyTimeTitle, *lblPenaltyTime, *lblPenaltyTimeContinue,
  *lblSingle, *lblMulti,
  *lblNameEntry, *lblNameInput,
  *lblStartByPressingButton,
  *lblTime, *lblMistakes, *lblTotal, *lblTimeValue, *lblMistakesValue, *lblTotalValue, 
  *lblGameOver,
  *lblBestPlayers;

Label *lblScoreboard[scoreboardLength], *lblScoreboardRemove[scoreboardLength];

Keyboard *kbNameEntry;
Label *lblKeyboardDelete, *lblKeyboardSpace;

const int numPenaltyTimeButtons = 5, penaltyTimeButtonsStepSize = 5, penaltyTimeMinValue = 5;
Label *penaltyTimeButtons[numPenaltyTimeButtons];

GameState gameState = GameState::GameTypeSelection, lastGameState = GameState::Init;

GameType gameType = GameType::Choice;

int penaltyTime = 5; // 5 seconds by default

// helper function to convert floating point numbers to strings
String floatToString(float x, byte precision = 2) {
  char tmp[50];
  dtostrf(x, 0, precision, tmp);
  return String(tmp);
}

int getPenaltyTimeByIndex(int i) {
  return i * penaltyTimeButtonsStepSize + penaltyTimeMinValue;
}

void setup()
{
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("running...");
  #endif

  // buzz wire pins
  pinMode(startStopPin, INPUT_PULLUP);
  pinMode(mistakePin, INPUT_PULLUP);

  // init
  tft.begin();
  touch.setCal(3594, 733, 638, 3407, 320, 240, 1);

  // same rotation
  tft.setRotation(1);
  touch.setRotation(1);
  
  tft.fillScreen(ILI9341_BLACK);

  lblTitle = new Label(&tft, 10, ILI9341_RED, BG_COLOR, 3, "KreativeKiste.de");
  lblSubtitle = new Label(&tft, lblTitle->getBottomY() + 10, ILI9341_RED, BG_COLOR, 2, "Heisser Draht");

  // start
  lblPenaltyTimeTitle = new Label(&tft, lblSubtitle->getBottomY() + 30, ILI9341_WHITE, BG_COLOR, 2, "Strafzeit einstellen", true);
  lblPenaltyTime = new Label(&tft, lblPenaltyTimeTitle->getBottomY() + 20, ILI9341_WHITE, BG_COLOR, 3, String(getPenaltyTimeByIndex(0)) + " s", true);

  for (int i = 0; i < numPenaltyTimeButtons; i++) {
    penaltyTimeButtons[i] = new Label(&tft, 
      (i == 0) ? 10 : penaltyTimeButtons[i - 1]->getRightX() + 20, // x
      lblPenaltyTime->getBottomY() + 20, // y
      ILI9341_WHITE, BG_COLOR, 2, String(getPenaltyTimeByIndex(i)) + " s", true);
  }
  lblPenaltyTimeContinue = new Label(&tft, penaltyTimeButtons[0]->getBottomY() + 20, ILI9341_WHITE, BG_COLOR, 2, "Weiter", true);

  // game type selection
  lblSingle = new Label(&tft, lblSubtitle->getBottomY() + 30, ILI9341_WHITE, BG_COLOR, 3, "Singleplayer", true);
  lblMulti = new Label(&tft, lblSingle->getBottomY() + 30, ILI9341_WHITE, BG_COLOR, 3, "Multiplayer", true);

  // name entry
  lblNameEntry = new Label(&tft, lblSubtitle->getBottomY() + 30, ILI9341_WHITE, BG_COLOR, 2, "Bitte gib deinen Namen ein", true);
  lblNameInput = new Label(&tft, lblNameEntry->getBottomY() + 20, ILI9341_WHITE, BG_COLOR, 3, "", true);
  kbNameEntry = new Keyboard("ABCDEFGHIJKLMNOPQRSTUVWXYZ", &tft, 10, lblNameInput->getBottomY() + 20, ILI9341_TFTHEIGHT - 20, ILI9341_WHITE, BG_COLOR, 3, true);
  lblKeyboardDelete = new Label(&tft, 245, lblNameInput->getBottomY() + 44, ILI9341_WHITE, BG_COLOR, 3, "<-", true);
  lblKeyboardSpace = new Label(&tft, 220, lblNameInput->getBottomY() + 44, ILI9341_WHITE, BG_COLOR, 3, "_", true);

  // start
  lblStartByPressingButton = new Label(&tft, lblSubtitle->getBottomY() + 50, ILI9341_WHITE, BG_COLOR, 3, "Start druecken", true);
  
  // running
  lblTime = new Label(&tft, paddingLeft, lblSubtitle->getBottomY() + 30, ILI9341_WHITE, BG_COLOR, 3, "Zeit:", true);
  lblMistakes = new Label(&tft, paddingLeft, lblTime->getBottomY() + 10, ILI9341_WHITE, BG_COLOR, 3, "Fehler:", true);
  lblTotal = new Label(&tft, paddingLeft, lblMistakes->getBottomY() + 10, ILI9341_WHITE, BG_COLOR, 3, "Gesamt:", true);
  lblTimeValue = new Label(&tft, lblTotal->getRightX() + 10, lblSubtitle->getBottomY() + 30, ILI9341_WHITE, BG_COLOR, 3, "", true);
  lblMistakesValue = new Label(&tft, lblTotal->getRightX() + 10, lblTime->getBottomY() + 10, ILI9341_WHITE, BG_COLOR, 3, "", true);
  lblTotalValue = new Label(&tft, lblTotal->getRightX() + 10, lblMistakes->getBottomY() + 10, ILI9341_WHITE, BG_COLOR, 3, "", true);

  // over
  lblGameOver = new Label(&tft, lblTotal->getBottomY() + 30, ILI9341_BLUE, BG_COLOR, 3, "Game Over", true);

  // scoreboard
  lblBestPlayers = new Label(&tft, paddingLeft, lblSubtitle->getBottomY() + 30, ILI9341_WHITE, BG_COLOR, 3, "Beste Spieler", true);
  for (int i = 0; i < scoreboardLength; i++) {
    lblScoreboard[i] = new Label(&tft, paddingLeft, (i == 0) ? lblBestPlayers->getBottomY() + 20 : lblScoreboard[i - 1]->getBottomY() + 10, ILI9341_WHITE, BG_COLOR, 2, "#" + String(i + 1), true);
    lblScoreboardRemove[i] = new Label(&tft, 300, (i == 0) ? lblBestPlayers->getBottomY() + 20 : lblScoreboard[i - 1]->getBottomY() + 10, ILI9341_WHITE, BG_COLOR, 2, "X", true);
  }
}

void loop()
{
  int touchX, touchY;
  bool touched = false, released = false;
  
  unsigned long startMillis, endMillis, lastMistake, scoreboardMillis, lastButtonClicked = 0, waitingStateEntered;
  unsigned int mistakesCount;

  #ifdef DEBUG
    unsigned long fpsCounterStart;
  #endif
  
  while (1) {
    // fps
    #ifdef DEBUG
      //Serial.print("Time (main loop): ");
      //Serial.println(millis() - fpsCounterStart);
      fpsCounterStart = millis();
    #endif

    // touch events
    if (touch.Pressed()) {
      touched = true;
      released = false;
      
      // read coords
      touchX = touch.X(); 
      touchY = touch.Y();
    }
    
    
    // START
    if (lastGameState != GameState::Start && gameState == GameState::Start) {
      #ifdef DEBUG
        Serial.println("GameState::Start");
      #endif
      lblPenaltyTimeTitle->show();
      lblPenaltyTime->show();
      for (int i = 0; i < numPenaltyTimeButtons; i++) {
        penaltyTimeButtons[i]->show();
      }
      lblPenaltyTimeContinue->show();

      // don't enter this if the next time
      lastGameState = GameState::Start;
    }
    if (gameState == GameState::Start) {
      lblPenaltyTime->setText(String(penaltyTime) + " s"); // updateLabel

      // penalty time button click check
      for (int i = 0; i < numPenaltyTimeButtons; i++) {
        if (touched && penaltyTimeButtons[i]->clicked(touchX, touchY)) {
          penaltyTimeButtons[i]->showBorder(ILI9341_WHITE);
          penaltyTime = getPenaltyTimeByIndex(i); 
          #ifdef DEBUG
            Serial.print("penaltyTime = ");
            Serial.println(penaltyTime);
          #endif
        }
        else if (touched) {
          penaltyTimeButtons[i]->hideBorder();
        }
      }

      // exit loop
      if (touched && lblPenaltyTimeContinue->clicked(touchX, touchY)) { // LOW pin starts game
        kbNameEntry->setLastClickMillis(millis());
        setGameState((gameType == GameType::Multi) ? GameState::NameEntry : GameState::Waiting);
      }
    }
    if (lastGameState == GameState::Start && gameState != GameState::Start) {
      #ifdef DEBUG
        Serial.println("exiting GameState::Start");
      #endif
      lblPenaltyTimeTitle->hide();
      lblPenaltyTime->hide();
      for (int i = 0; i < numPenaltyTimeButtons; i++) {
        penaltyTimeButtons[i]->hide();
      }
      lblPenaltyTimeContinue->hide();
    }

    // GAME TYPE SELECTION
    if (lastGameState != GameState::GameTypeSelection && gameState == GameState::GameTypeSelection) {
      #ifdef DEBUG
        Serial.println("GameState::GameTypeSelection");
      #endif

      gameType = GameType::Choice;

      // show buttons
      lblSingle->show();
      lblMulti->show();

      // don't enter this if the next time
      lastGameState = GameState::GameTypeSelection;
    }
    if (gameState == GameState::GameTypeSelection) {
      if (gameType == GameType::Choice) { // not chosen yet
        // single clicked
        if (released && lblSingle->clicked(touchX, touchY)) {
          gameType = GameType::Single;
        }
        // multi clicked
        if (released && lblMulti->clicked(touchX, touchY)) {
          gameType = GameType::Multi;
        }
      }
      else {
        setGameState(GameState::Start);
      }
    }
    if (lastGameState == GameState::GameTypeSelection && gameState != GameState::GameTypeSelection) {
      #ifdef DEBUG
        Serial.println("exiting GameState::GameTypeSelection");
      #endif
      lblSingle->hide();
      lblMulti->hide();
    }
  
  
    // NAME ENTRY
    if (lastGameState != GameState::NameEntry && gameState == GameState::NameEntry) {
      #ifdef DEBUG
        Serial.println("GameState::NameEntry");
      #endif

      lblNameEntry->show();
      lblNameInput->setText(""); // reset player name
      lblNameInput->show();
      kbNameEntry->show();
      lblKeyboardDelete->show();
      lblKeyboardSpace->show();
      
      // don't enter this if the next time
      lastGameState = GameState::NameEntry;
    }
    if (gameState == GameState::NameEntry) {
      // name input keyboard
      if (released && kbNameEntry->handleClick(touchX, touchY)) {
        if (lblNameInput->getText().length() <= 12) { // max chars per line
          lblNameInput->appendText(kbNameEntry->getLastKeyVal());
        }
      }

      // keyboard delete
      if (released && lastButtonClicked + minTimeBetweenButtonClick < millis() && lblKeyboardDelete->clicked(touchX, touchY)) {
        lastButtonClicked = millis();
        lblNameInput->removeLastChar();
      }

      // keyboard space
      if (released && lastButtonClicked + minTimeBetweenButtonClick < millis() && lblKeyboardSpace->clicked(touchX, touchY)) {
        lastButtonClicked = millis();
        lblNameInput->appendText(" ");
      }
      
      
      // exit loop to normal match
      if (!digitalRead(startStopPin)) { // LOW pin starts game
        setGameState(GameState::Running);
        startMillis = millis(); // save start millis as soon as possible
        currentPlayerName = lblNameInput->getText();
      }
    }
    if (lastGameState == GameState::NameEntry && gameState != GameState::NameEntry) {
      #ifdef DEBUG
        Serial.println("exiting GameState::NameEntry");
      #endif
      lblNameEntry->hide();
      lblNameInput->hide();
      kbNameEntry->hide();
      lblKeyboardDelete->hide();
      lblKeyboardSpace->hide();
      
    }
  
  
    // WAITING
    if (lastGameState != GameState::Waiting && gameState == GameState::Waiting) {
      #ifdef DEBUG
        Serial.println("GameState::Waiting");
      #endif

      lblStartByPressingButton->show();
      waitingStateEntered = millis();
      
      // don't enter this if the next time
      lastGameState = GameState::Waiting;
    }
    if (gameState == GameState::Waiting) {
      // exit loop to normal match
      if (waitingStateEntered + minTimeBetweenButtonClick < millis() && !digitalRead(startStopPin)) { // LOW pin starts game
        setGameState(GameState::Running);
        startMillis = millis(); // save start millis as soon as possible
      }
    }
    if (lastGameState == GameState::Waiting && gameState != GameState::Waiting) {
      #ifdef DEBUG
        Serial.println("exiting GameState::Waiting");
      #endif
      lblStartByPressingButton->hide();
    }
  
  
    // RUNNING
    if (lastGameState != GameState::Running && gameState == GameState::Running) {
      #ifdef DEBUG
        Serial.println("GameState::Running");
      #endif
      
      lastMistake = 0;
      mistakesCount = 0;
      
      lblTime->show();
      lblMistakes->show();
      lblTotal->show();
      lblTimeValue->show();
      lblMistakesValue->show();
      lblTotalValue->show();

      // don't enter this if the next time
      lastGameState = GameState::Running;
    }
    if (gameState == GameState::Running) {
      double secondsElapsed = (double)(millis() - startMillis) / (double)1000;
      lblTimeValue->setText(floatToString(secondsElapsed, 1) + " s");
      lblMistakesValue->setText(String(mistakesCount));
      currentPlayerTime = secondsElapsed + mistakesCount * penaltyTime;
      lblTotalValue->setText(floatToString(currentPlayerTime, 1) + " s");
  
      // mistake detection
      if (!digitalRead(mistakePin) && (mistakesCount == 0 || millis() > lastMistake + minTimeBetweenTwoMistakes)) {
        mistakesCount++;
        lastMistake = millis();
      }
  
      // exit loop
      if (!digitalRead(startStopPin) && millis() > startMillis + minGameTimeMillis) { // LOW pin stops game and min time elapsed
        setGameState(GameState::Over);
      }
    }
    if (lastGameState == GameState::Running && gameState != GameState::Running) {
      #ifdef DEBUG
        Serial.println("exiting GameState::Running");
      #endif
    }
  
  
    // OVER
    if (lastGameState != GameState::Over && gameState == GameState::Over) {
      #ifdef DEBUG
        Serial.println("GameState::Over");
      #endif
      endMillis = millis();
      lblGameOver->show();

      // don't enter this if the next time
      lastGameState = GameState::Over;
    }
    if (gameState == GameState::Over) {
      if (millis() > endMillis + autoRestartMillis) {
        setGameState(gameType == GameType::Multi ? GameState::Scoreboard : GameState::GameTypeSelection);
      }
      // exit loop
      if (!digitalRead(startStopPin) && millis() > endMillis + minGameOverScreenMillis) { // LOW pin stops game over screen
        setGameState(gameType == GameType::Multi ? GameState::Scoreboard : GameState::Waiting);
      }
    }
    if (lastGameState == GameState::Over && gameState != GameState::Over) {
      #ifdef DEBUG
        Serial.println("exiting GameState::Over");
      #endif
      lblTime->hide();
      lblMistakes->hide();
      lblTotal->hide();
      lblTimeValue->hide();
      lblMistakesValue->hide();
      lblTotalValue->hide();
      lblGameOver->hide();
    }


    // SCOREBOARD
    if (lastGameState != GameState::Scoreboard && gameState == GameState::Scoreboard) {
      #ifdef DEBUG
        Serial.println("GameState::Scoreboard");
        Serial.println(currentPlayerName);
        Serial.println(currentPlayerTime);
      #endif

      scoreboardMillis = millis();

      bool highscore = addScore(currentPlayerName, currentPlayerTime);

      // show labels
      lblBestPlayers->show();
      for (int i = 0; i < scoreboardLength; i++) {
        lblScoreboard[i]->show();
        lblScoreboardRemove[i]->show();
      }

      // don't enter this if the next time
      lastGameState = GameState::Scoreboard;
    }
    if (gameState == GameState::Scoreboard) {
      // remove scoreboard list item click
      if (released && lastButtonClicked + minTimeBetweenButtonClick < millis()) {
        for (int i = 0; i < scoreboardLength; i++) {
          if (lblScoreboardRemove[i]->clicked(touchX, touchY)) {
            #ifdef DEBUG
              Serial.print("remove score ");
              Serial.println(i);
            #endif
            lastButtonClicked = millis();
            spliceScore(i);
            updateScoreboardLabels();
          }
        }
      }

      // exit state
      if (!digitalRead(startStopPin) && millis() > scoreboardMillis + minGameOverScreenMillis) { // LOW pin stops game over screen
        kbNameEntry->setLastClickMillis(millis());
        setGameState(GameState::NameEntry);
      }
    }
    if (lastGameState == GameState::Scoreboard && gameState != GameState::Scoreboard) {
      #ifdef DEBUG
        Serial.println("exiting GameState::Scoreboard");
      #endif
      
      lblBestPlayers->hide();
      for (int i = 0; i < scoreboardLength; i++) {
        lblScoreboard[i]->hide();
        lblScoreboardRemove[i]->hide();
      }
    }


    // end of loop
    released = touched; // temporarily set (be sure you understand the implementation before editing this var)
    touched = false; // prevent multiple clicks with old coords
  }
}

void setGameState(GameState newState) {
  lastGameState = gameState;
  gameState = newState;
}


// return true if the score is a new highscore
bool addScore(String name, double time) {
  bool pushed = false;
  for (int i = 0; i < scoreboardLength; i++) {
    if (topRanksName[i].length() == 0 || topRanksTime[i] >= time) { // no rank exisiting or new score better
      pushScore(i, name, time);
      pushed = true;
      break;
    }
  }
  if (pushed) {
    updateScoreboardLabels();
  }
  return pushed;
}

void updateScoreboardLabels() {
  for (int i = 0; i < scoreboardLength; i++) {
    // update labels
    lblScoreboard[i]->setText("#" + String(i + 1) + ((topRanksName[i].length() == 0) ? ("") : (" " + topRanksName[i] + " - " + floatToString(topRanksTime[i], 1))));
  }
}

void pushScore(int index, String name, double time) {
  for (int i = scoreboardLength - 1; i >= index; i--) {
    if (i == index) {
      topRanksName[i] = name;
      topRanksTime[i] = time;
    }
    else {
      topRanksName[i] = topRanksName[i - 1];
      topRanksTime[i] = topRanksTime[i - 1];
    }
  }
}

void spliceScore(int index) {
  for (int i = index; i < scoreboardLength; i++) {
    if (i != scoreboardLength - 1) {
      topRanksName[i] = topRanksName[i + 1];
      topRanksTime[i] = topRanksTime[i + 1];
    }
    else {
      topRanksName[i] = "";
      topRanksTime[i] = 0;
    }
  }
}

