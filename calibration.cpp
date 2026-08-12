#include "calibration.h"
#include "touch.h"

void calibrationStart()
{
    touchCalibration();
}

bool calibrationLoad()
{
    return touchLoadCalibration();
}

bool calibrationSave()
{
    touchSaveCalibration();

    return true;
}