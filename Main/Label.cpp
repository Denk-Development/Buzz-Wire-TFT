#ifndef Adafruit_ILI9341
  #include "Adafruit_ILI9341.h"
#endif

// Issues: If text length changes for centered text the text is off center and the old text is not fully removed.

class Label {
public:
  Label()
    : tft(0), x(0), y(0), color(ILI9341_WHITE), bgColor(ILI9341_BLACK), textSize(2), text(""), oldTextLength(0) {};
  Label(Adafruit_ILI9341* tft, int x, int y, int color, int bgColor, int textSize, String text, bool hidden)
    : tft(tft), x(x), y(y), color(color), bgColor(bgColor), textSize(textSize), text(text), hidden(hidden)
  {
    this->oldTextLength = this->text.length();
    this->refresh();  
  };

  // center text
  Label(Adafruit_ILI9341* tft, int y, int color, int bgColor, int textSize, String text, bool hidden)
    : tft(tft), y(y), color(color), bgColor(bgColor), textSize(textSize), text(text), hidden(hidden)
  {
    this->oldTextLength = this->text.length();
    int width = 6 * this->textSize * this->text.length();
    this->x = (ILI9341_TFTHEIGHT - width) / 2;
    this->refresh();  
  };

  void setText(String newText) {
    if (this->text != newText) {
      this->text = newText;
      this->refresh();
    }
  }

  void setColor(int newColor) {
    this->color = newColor;
    this->refresh();
  }

  void hide() {
    this->hidden = true;
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
  int x, y, color, bgColor, textSize, oldTextLength;
  String text;
  bool hidden;

  void refresh() {
    this->tft->setCursor(this->x, this->y);
    this->tft->setTextColor(this->color);
    this->tft->setTextSize(this->textSize);
      
    for (unsigned int i = 0; i < this->text.length(); i++) {
      // remove the old text
      this->tft->fillRect(
        this->x + i * 6 * this->textSize, 
        this->y, 
        6 * this->textSize, // height of the text
        this->textSize * 8, // width of the text
        this->bgColor);

      if (!this->hidden) {
        this->tft->print(this->text.charAt(i));
      }
    }
  }
};