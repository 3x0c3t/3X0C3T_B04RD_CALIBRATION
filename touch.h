#ifndef TOUCH_H
#define TOUCH_H

#include <Arduino.h>
#include <XPT2046_Touchscreen.h>

struct TouchPoint
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
};

void touchInit();

bool touchReadRaw(TouchPoint &point);

bool touchPressed();

#endif