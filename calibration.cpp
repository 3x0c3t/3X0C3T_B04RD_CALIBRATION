#include "calibration.h"
#include "settings.h"
#include "touch.h"
#include "display.h"

struct Sample
{
    uint16_t x;
    uint16_t y;
};

static CalibrationData calibration;

static bool waitForRelease()
{
    unsigned long start = millis();

    while (touchPressed())
    {
        if (millis() - start >= CALIBRATION_RELEASE_TIMEOUT)
            return false;

        delay(5);
    }

    return true;
}

static bool collectSamples(
    Sample *samples,
    uint8_t count
)
{
    uint8_t index = 0;
    unsigned long start = millis();

    while (index < count)
    {
        if (millis() - start >= CALIBRATION_TIMEOUT)
        {
            Serial.println("[CAL] TIMEOUT");
            return false;
        }

        TouchPoint point;

        if (touchReadRaw(point))
        {
            samples[index].x = point.x;
            samples[index].y = point.y;

            index++;

            delay(CALIBRATION_SAMPLE_DELAY);

            if (!waitForRelease())
                return false;
        }

        delay(5);
    }

    return true;
}

static uint16_t averageX(
    Sample *samples,
    uint8_t count
)
{
    uint32_t total = 0;

    for (uint8_t i = 0; i < count; i++)
        total += samples[i].x;

    return total / count;
}

static uint16_t averageY(
    Sample *samples,
    uint8_t count
)
{
    uint32_t total = 0;

    for (uint8_t i = 0; i < count; i++)
        total += samples[i].y;

    return total / count;
}

void calibrationInit()
{
    calibration.xMin = 0;
    calibration.xMax = 0;
    calibration.yMin = 0;
    calibration.yMax = 0;
    calibration.valid = false;
}

bool calibrationRun()
{
    Sample samples[CALIBRATION_SAMPLES];

    uint16_t rawX[CALIBRATION_POINTS];
    uint16_t rawY[CALIBRATION_POINTS];

    for (uint8_t point = 0; point < CALIBRATION_POINTS; point++)
    {
        uint8_t number = point + 1;

        Serial.print("[CAL] Point ");
        Serial.print(number);
        Serial.print("/");
        Serial.println(CALIBRATION_POINTS);

        displayCalibrationScreen(number);

        if (!collectSamples(
                samples,
                CALIBRATION_SAMPLES
            ))
        {
            calibration.valid = false;

            displayCalibrationResult(
                false,
                0,
                0,
                0,
                0
            );

            return false;
        }

        rawX[point] =
            averageX(
                samples,
                CALIBRATION_SAMPLES
            );

        rawY[point] =
            averageY(
                samples,
                CALIBRATION_SAMPLES
            );

        Serial.print("[CAL] X=");
        Serial.print(rawX[point]);

        Serial.print(" Y=");
        Serial.println(rawY[point]);

        delay(250);
    }

    calibration.xMin =
        (rawX[0] + rawX[3]) / 2;

    calibration.xMax =
        (rawX[1] + rawX[2]) / 2;

    calibration.yMin =
        (rawY[0] + rawY[1]) / 2;

    calibration.yMax =
        (rawY[2] + rawY[3]) / 2;

    if (calibration.xMin >= calibration.xMax)
    {
        Serial.println("[CAL] X ERROR");
        calibration.valid = false;
        return false;
    }

    if (calibration.yMin >= calibration.yMax)
    {
        Serial.println("[CAL] Y ERROR");
        calibration.valid = false;
        return false;
    }

    calibration.valid = true;

    Serial.print("[CAL] X=");
    Serial.print(calibration.xMin);
    Serial.print("..");
    Serial.println(calibration.xMax);

    Serial.print("[CAL] Y=");
    Serial.print(calibration.yMin);
    Serial.print("..");
    Serial.println(calibration.yMax);

    displayCalibrationResult(
        true,
        calibration.xMin,
        calibration.xMax,
        calibration.yMin,
        calibration.yMax
    );

    return true;
}

CalibrationData calibrationGetData()
{
    return calibration;
}