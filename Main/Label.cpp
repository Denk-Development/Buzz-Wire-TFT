#ifndef Adafruit_ILI9341
  #include "Adafruit_ILI9341.h"
#endif

class Label {
public:
  Label()
    : tft(0), x(0), y(0), color(ILI9341_WHITE), bgColor(ILI9341_BLACK), textSize(2), text("") {};
  Label(Adafruit_ILI9341* tft, int x, int y, int color, int bgColor, int textSize, String text)
    : tft(tft), x(x), y(y), color(color), bgColor(bgColor), textSize(textSize), text(text)
  {
    this->refresh();  
  };

  void setText(String newText) {
    this->text = newText;
    this->refresh();
  }

  void setColor(int newColor) {
    this->color = newColor;
    this->refresh();
  }
  
private:
  Adafruit_ILI9341 *tft;
  int x, y, color, bgColor, textSize;
  String text;

  void refresh() {
    this->tft->setCursor(this->x, this->y);
    this->tft->setTextColor(this->color);
    this->tft->setTextSize(this->textSize);
      
    for (int i = 0; i < this->text.length(); i++) {
      // remove the old text
      this->tft->fillRect(
        this->x + i * 6 * this->textSize, 
        this->y, 
        6 * this->textSize, // height of the text
        this->textSize * 8, // width of the text
        this->bgColor);
      this->tft->print(this->text.charAt(i));
    }
  }
};