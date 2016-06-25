#ifndef Label_cpp
#define Label_cpp

#ifndef Adafruit_ILI9341
  #include "Adafruit_ILI9341.h"
#endif

class Label {
public:
  Label()
    : tft(0), x(0), y(0), color(ILI9341_WHITE), bgColor(ILI9341_BLACK), textSize(2), text("empty"), oldTextLength(0), centered(false) {};
  
  Label(Adafruit_ILI9341* tft, int x, int y, int color, int bgColor, int textSize, String text, bool hidden = false)
    : tft(tft), x(x), y(y), color(color), bgColor(bgColor), textSize(textSize), text(text), hidden(hidden), centered(false)
  {
    if (!hidden) {
      this->refresh();  
    }
  };

  // center text
  Label(Adafruit_ILI9341* tft, int y, int color, int bgColor, int textSize, String text, bool hidden = false)
    : tft(tft), y(y), color(color), bgColor(bgColor), textSize(textSize), text(text), hidden(hidden), centered(true)
  {
    int width = 6 * this->textSize * this->text.length();
    this->x = (ILI9341_TFTHEIGHT - width) / 2;
    if (!hidden) {
      this->refresh();  
    }
  };

  void setText(String newText) {
    if (this->text != newText) {
      this->oldText = this->text;
      this->text = newText;
      
      if (this->centered) {
        this->oldX = this->x;
        int width = 6 * this->textSize * this->text.length();
        this->x = (ILI9341_TFTHEIGHT - width) / 2;
      }
      this->refresh();
    }
  }

  void appendText(String appendix) {
    this->setText(this->text + appendix);
  }

  void removeLastChar() {
    this->setText(this->text.substring(0, this->text.length() - 1));
  }

  String getText() const {
    return this->text;
  }

  void setColor(int newColor) {
    this->color = newColor;
    this->refresh();
  }

  void hide() {
    this->hidden = true;
    this->oldX = this->x;
    this->oldText = this->text;
    this->refresh();
    this->hideBorder();
  }

  void show() {
    this->oldX = 0;
    this->oldText = "";
    this->hidden = false;
    this->refresh();
  }

  int getHeight() const {
    return 8 * this->textSize;
  }

  int getWidth() const {
    return this->textSize * 6 * this->text.length();
  }

  int getRightX() const {
    return this->getWidth() + this->x;
  }

  int getBottomY() const {
    return this->getHeight() + this->y;
  }

  void showBorder(int borderColor) {
    this->borderColor = borderColor;
    this->borderShown = true;
    this->drawBorder();
  }

  void hideBorder() {
    this->borderShown = false;
    this->tft->drawRect(this->x, this->y, this->getRightX() - this->x, this->getBottomY() - this->y, this->bgColor);
  }

  bool clicked(int x, int y) const {
    return (this->x <= x && this->getRightX() >= x &&
      this->y <= y && this->getBottomY() >= y);
  }
  
private:
  Adafruit_ILI9341 *tft;
  int x, oldX, y, color, bgColor, textSize, oldTextLength, borderColor;
  String text, oldText = "";
  bool hidden, centered, borderShown = false;

  void refresh() {
    this->hideBorder();
    // ISSUE: centerd text would not fully remove the border

    #ifdef DEBUG
      if (this->hidden) {
        Serial.println("Label hidden");
        Serial.print("x = "); Serial.println(this->oldX);
        Serial.print("y = "); Serial.println(this->y);
        Serial.print("width = "); Serial.println(6 * this->textSize * this->oldText.length());
        Serial.print("height = "); Serial.println(this->textSize * 8);
        Serial.print("oldText = "); Serial.println(this->oldText);
        Serial.print("text = "); Serial.println(this->text);
      }
    #endif
    int textLength = this->oldText.length();
    if (this->oldText.length() == 0) textLength = this->text.length();
    if (this->centered) {
      // clear old area
      this->tft->fillRect(
        this->oldX,
        this->y,
        6 * this->textSize * textLength, // width of the text
        this->textSize * 8, // height of the text
        this->bgColor);
    }
    
    this->tft->setCursor(this->x, this->y);
    this->tft->setTextColor(this->color);
    this->tft->setTextSize(this->textSize);

    unsigned int charsCount = (this->text.length() > this->oldText.length()) ? this->text.length() : this->oldText.length();
    
    for (unsigned int i = 0; i < charsCount; i++) {
      if (!this->hidden && !this->centered) {
        // overwriting is not necessary if the text is not centered
        if (i < this->oldText.length() && i < this->text.length() && this->oldText.charAt(i) == this->text.charAt(i)) {
          this->tft->setCursor(this->x + (i + 1) * 6 * this->textSize, this->y);
          continue;
        }
      }
      
      if (!this->centered) { // already done if centered
        // remove the old text
        this->tft->fillRect(
          this->x + i * 6 * this->textSize, 
          this->y, 
          6 * this->textSize, // width of the text
          this->textSize * 8, // height of the text
          this->bgColor);
      }

      if (!this->hidden && i < this->text.length()) {
        this->tft->print(this->text.charAt(i));
      }
    }

    if (this->borderShown) {
      this->drawBorder();
    }
  }

  void drawBorder() {
    this->tft->drawRect(this->x, this->y, this->getRightX() - this->x, this->getBottomY() - this->y, this->borderColor);
  }
};

#endif
