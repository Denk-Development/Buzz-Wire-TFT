#ifndef Adafruit_ILI9341
  #include "Adafruit_ILI9341.h"
#endif

// Issues: If text length changes for centered text the text is off center and the old text is not fully removed.

class Label {
public:
  Label()
    : tft(0), x(0), y(0), color(ILI9341_WHITE), bgColor(ILI9341_BLACK), textSize(2), text(""), oldTextLength(0), centered(false) {};
  
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

  void setColor(int newColor) {
    this->color = newColor;
    this->refresh();
  }

  void hide() {
    this->hidden = true;
    this->oldX = this->x;
    this->oldText = this->text;
    this->refresh();
  }

  void show() {
    this->hidden = false;
    this->refresh();
  }

  int getHeight() const {
    return 6 * this->textSize;
  }

  int getWidth() const {
    return this->textSize * 8 * this->text.length();
  }

  int getRightX() const {
    return this->getWidth() + this->x;
  }

  int getBottomY() const {
    return this->getHeight() + this->y;
  }
  
private:
  Adafruit_ILI9341 *tft;
  int x, oldX, y, color, bgColor, textSize, oldTextLength;
  String text, oldText = "";
  bool hidden, centered;

  void refresh() {
    if (this->centered) {
      // clear old area
      this->tft->fillRect(
        this->oldX,
        this->y,
        6 * this->textSize * this->oldText.length(), // width of the text
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
  }
};
