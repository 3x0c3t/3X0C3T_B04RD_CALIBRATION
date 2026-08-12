#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

void displayInit();

void displayCalibrationScreen(
    uint8_t point
);

void displayCalibrationResult(
    bool success,
    uint16_t xMin,
    uint16_t xMax,
    uint16_t yMin,
    uint16_t yMax
);

void displayTestScreen();

void displayTouchPoint(
    int16_t x,
    int16_t y
);

#endif