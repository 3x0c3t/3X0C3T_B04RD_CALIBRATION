#include <Arduino.h>

#include "settings.h"
#include "display.h"
#include "touch.h"
#include "calibration.h"

static bool calibrationOK = false;
static bool testMode = false;
static unsigned long lastTouch = 0;

void setup()
{
    Serial.begin(SERIAL_BAUD);

    delay(500);

    Serial.println();
    Serial.println("[BOOT] CALIBRATION");

    displayInit();
    Serial.println("[TFT] OK");

    touchInit();

    calibrationInit();

    calibrationOK = calibrationRun();

    if (calibrationOK)
    {
        Serial.println("[CAL] OK");
        testMode = false;
    }
    else
    {
        Serial.println("[CAL] ERROR");
    }
}

void loop()
{
    if (!calibrationOK)
    {
        delay(100);
        return;
    }

    if (!testMode)
    {
        if (touchPressed())
        {
            delay(TOUCH_DEBOUNCE);

            while (touchPressed())
                delay(5);

            displayTestScreen();

            testMode = true;

            Serial.println("[TEST] START");
        }

        return;
    }

    if (millis() - lastTouch < TOUCH_DEBOUNCE)
        return;

    TouchPoint point;

    if (!touchReadRaw(point))
        return;

    lastTouch = millis();

    CalibrationData cal =
        calibrationGetData();

    int16_t x = map(
        point.x,
        cal.xMin,
        cal.xMax,
        0,
        SCREEN_WIDTH - 1
    );

    int16_t y = map(
        point.y,
        cal.yMin,
        cal.yMax,
        0,
        SCREEN_HEIGHT - 1
    );

    x = constrain(
        x,
        0,
        SCREEN_WIDTH - 1
    );

    y = constrain(
        y,
        0,
        SCREEN_HEIGHT - 1
    );

    displayTouchPoint(
        x,
        y
    );

    Serial.print("[TOUCH] X=");
    Serial.print(x);

    Serial.print(" Y=");
    Serial.println(y);
}