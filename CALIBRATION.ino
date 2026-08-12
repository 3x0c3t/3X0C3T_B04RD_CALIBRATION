#include <Arduino.h>

#include "settings.h"
#include "display.h"
#include "touch.h"

void setup()
{
    Serial.begin(SERIAL_BAUD);

    delay(500);

    Serial.println();
    Serial.println("==============================");
    Serial.println("   3x0c3t BO4RD");
    Serial.println("   CALIBRATION");
    Serial.println("==============================");

    displayInit();

    touchInit();

    Serial.println();
    Serial.println("==============================");
    Serial.println("   CALIBRATION READY");
    Serial.println("==============================");
}

void loop()
{
    delay(1000);
}