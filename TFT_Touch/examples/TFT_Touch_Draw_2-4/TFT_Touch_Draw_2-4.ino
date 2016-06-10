/*
  This is a simple paint sketch!

  Tested with a 2.4" ILI9341 based display of 320 x 240 pixles

  Uses font 2 only

  The example will read the current state of the touch screen and output it to the serial
  port as raw X and Y coordinates, as the current position in pixels and a zone number.

  >>>> YOU MUST CALIBRATE THE TOUCH SCREEN AND COPY THE SETTINGS TO LINE 48 OF THIS SKETCH <<<<

  >>>>        YOU MUST CHANGE THE PIN DEFINITION AT LINES 29-32 TO SUIT YOUR SETUP         <<<<

  Created by Rowboteer: 22/11/15
*/

// Call up the TFT driver library
#include <Adafruit_ILI9341.h> // Hardware-specific TFT library
#include <SPI.h>

// Call up touch screen library
#include <TFT_Touch.h>

// screen pinning
const uint8_t TFT_RST = 8, TFT_DC = 9, TFT_CS = 10, TFT_MOSI = 11, TFT_MISO = 12, TFT_CLK = 13;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

// These are the pins used to interface between the 2046 touch controller and Arduino Pro
#define DOUT A0  /* Data out pin (T_DO) of touch screen */
#define DIN  A2  /* Data in pin (T_DIN) of touch screen */
#define DCS  7  /* Chip select pin (T_CS) of touch screen */
#define DCLK 6  /* Clock pin (T_CLK) of touch screen */

/* Create an instance of the touch screen library */
TFT_Touch touch = TFT_Touch(DCS, DCLK, DIN, DOUT);

int ColorPaletteHigh = 30; // Height of palette boxes
int color = ILI9341_WHITE;     //Starting paint brush color

// Pallete button colour sequence
unsigned int colors[10] = {ILI9341_RED, ILI9341_GREEN, ILI9341_BLUE, ILI9341_BLACK, ILI9341_CYAN, ILI9341_YELLOW, ILI9341_WHITE, ILI9341_MAGENTA, ILI9341_BLACK, ILI9341_BLACK};

void setup()
{
  Serial.begin(38400);

  tft.begin();
  
  touch.setCal(3594, 733, 638, 3407, 320, 240, 1);
  
  // Set the TFT and touch screen to landscape orientation
  tft.setRotation(1);
  touch.setRotation(1);

  tft.setTextSize(1);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_GREEN);

  //Draw the pallet
  for (int i = 0; i < 10; i++)
  {
    tft.fillRect(i * 32, 0, 32, ColorPaletteHigh, colors[i]);
  }

  //Draw the clear screen button
  tft.setCursor(264, 7); // x,y,font
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Clear");
  tft.drawRect(0, 0, 319, 30, ILI9341_WHITE);

  // Plot the current colour in the screen clear box
  tft.fillRect(300, 9, 12, 12, color);
}

/* Main program */
void loop()
{
  int X_Coord;
  int Y_Coord;

  // Check if the touch screen is currently pressed
  // Raw and coordinate values are stored within library at this instant
  // for later retrieval by GetRaw and GetCoord functions.

  if (touch.Pressed()) // Note this function updates coordinates stored within library variables
  {
    // Read the current X and Y axis as co-ordinates at the last touch time
    // The values were captured when Pressed() was called!
    X_Coord = touch.X();
    Y_Coord = touch.Y();

    Serial.print(X_Coord); Serial.print(","); Serial.println(Y_Coord);
    
    // Detect  paint brush color change
    if (Y_Coord < ColorPaletteHigh + 2)
    {
      if (X_Coord / 32 > 7)
      {
        // Clear the screen to current colour
        tft.fillRect(0, 30, 399, 239, color);
      }
      else
      {
        color = colors[X_Coord / 32];
        // Update the cuurent colour in the clear box
        tft.fillRect(300, 9, 12, 12, color);
      }
    }
    else
    {
      tft.fillCircle(X_Coord, Y_Coord, 2, color);
    }
  }
}

