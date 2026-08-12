#ifndef TOUCH_H
#define TOUCH_H

#include <Arduino.h>

void touchInit();

bool touchCalibration();

bool touchLoadCalibration();

void touchSaveCalibration();

void touchHandleEndScreen();

#endif