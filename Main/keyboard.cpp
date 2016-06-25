#ifndef Keyboard_cpp
#define Keyboard_cpp

#include "Arduino.h"
#include "Label.cpp"

#define DEBUG

class Keyboard {
public:
  Keyboard(String chars, Adafruit_ILI9341* tft, int x, int y, int width, int color, int bgColor, int textSize, bool hidden, unsigned long millisBetweenClicks = 500) :
    chars(chars), hidden(hidden), millisBetweenClicks(millisBetweenClicks) {
      numChars = chars.length();
      //lblChars = (Label*)malloc(sizeof(Label) * numChars);

      int lblX = x, lblY = y;
      for (int i = 0; i < numChars; i++) {
        // coords
        if (i > 0) { // move to the right
          lblX = lblChars[i - 1].getRightX();
          
          // line break
          if (lblX + lblChars[i - 1].getWidth() > width) {
            lblX = x;
            lblY = lblChars[i - 1].getBottomY();
          }
        }
        lblChars[i] = Label(tft, lblX, lblY, color, bgColor, textSize, String(chars.charAt(i)), hidden);
      }
    }
    
  bool handleClick(int x, int y) {
    for (int i = 0; i < numChars; i++) {
      if (lblChars[i].clicked(x, y)) {
        lastKeyVal = lblChars[i].getText();
        bool inTime = clickInTime();
        if (inTime) {
          this->lastClickMillis = millis();
        }
        return inTime;
      }
    }
    return false;
  }
  
  String getLastKeyVal() {
    return lastKeyVal;
  }

  void  show() {
    for (int i = 0; i < numChars; i++) {
      lblChars[i].show();
    }
  }
  
  void hide() {
    for (int i = 0; i < numChars; i++) {
      lblChars[i].hide();
    }
  }

  void setLastClickMillis(unsigned long newLastClickMillis) {
    this->lastClickMillis = newLastClickMillis;
  }

  
private:
  unsigned long lastClickMillis = 0, millisBetweenClicks;
  bool hidden;
  Label lblChars[50];
  String chars, lastKeyVal;
  int numChars;

  bool clickInTime() const {
    return (millis() > this->lastClickMillis + this->millisBetweenClicks);
  }
};

#endif
