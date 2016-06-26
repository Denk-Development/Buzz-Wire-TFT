# Buzz-Wire-TFT
Arduino powered Version of the game Buzz Wire with an TFT touch screen. This project was developed for [Denk Development](http://development.timodenk.com/)'s customer [Kreative Kiste](http://kreativekiste.de/).

<img src="https://cloud.githubusercontent.com/assets/6556307/16361660/b8320366-3b97-11e6-9721-599fb8ce1a02.jpg" width="400" />

## Features
 * Single- and multiplayer
 * Multiplayer scoreboard
 * Touchscreen name input

## Components
 * [Teensy 3.2 microcontroller](https://www.pjrc.com/store/teensy32.html)
 * TFT touch screen (e.g. 2.4" 320*240 TFT LCD Touchscreen with PCB ILI9341)
 * Buzz-Wire hardware

## Libraries
 * [SPI](https://github.com/PaulStoffregen/SPI)
 * [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)
 * [Adafruit_ILI9341](https://github.com/adafruit/Adafruit_ILI9341)
 * [TFT_Touch](https://github.com/Bodmer/TFT_Touch)

## Wiring
Teensy | Screen | Buzz-Wire | Description
--- | --- | --- | ---
0|||Software Serial for debugging
1|||Software Serial for debugging
2||Start / Stop|Main button
3||Mistake|
4||Buzzer|Sound signal
6|DCLK||Touch clock
7|DCS||Touch chip select
8|TFT_RST||Reset
9|TFT_DC||
10|TFT_CS||Chip select
11|TFT_MOSI||
12|TFT_MISO||
13|TFT_CLK||Clock
A0|DOUT||Touch
A2|DIN||Touch

### Teensy 3.2 Pin Assignment Font Side
[Image source](https://www.pjrc.com/teensy/pinout.html)

![card7a_rev1](https://cloud.githubusercontent.com/assets/6556307/16361677/4e9e319e-3b98-11e6-8892-3bbfdd0ec22e.png)

## System States

```C++
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
```

## Photos
![System](https://cloud.githubusercontent.com/assets/6556307/16361660/b8320366-3b97-11e6-9721-599fb8ce1a02.jpg)
![Screen close up](https://cloud.githubusercontent.com/assets/6556307/16361644/7b66b332-3b97-11e6-9503-c388896adc2b.jpg)
