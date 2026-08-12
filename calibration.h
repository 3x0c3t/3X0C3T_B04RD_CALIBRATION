#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>

struct CalibrationData
{
    uint16_t xMin;
    uint16_t xMax;
    uint16_t yMin;
    uint16_t yMax;
    bool valid;
};

void calibrationInit();

bool calibrationRun();

CalibrationData calibrationGetData();

#endif