#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

#define SERIAL_BAUD 115200

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320

#define TFT_ROTATION 2

#define CALIBRATION_FILE "/TouchCalData"

#define FORCE_CALIBRATION true

#define COLOR_BACKGROUND TFT_BLACK
#define COLOR_TEXT       TFT_WHITE
#define COLOR_POINT      TFT_MAGENTA
#define COLOR_SUCCESS    TFT_GREEN

#endif