#include "touch.h"
#include "settings.h"

#include <SPI.h>

static XPT2046_Touchscreen ts(
    TOUCH_CS,
    TOUCH_IRQ
);

void touchInit()
{
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);

    pinMode(TOUCH_CS, OUTPUT);
    digitalWrite(TOUCH_CS, HIGH);

    SPI.begin();

    delay(50);

    if (ts.begin())
    {
        ts.setRotation(TFT_ROTATION);
        Serial.println("[TOUCH] OK");
    }
    else
    {
        Serial.println("[TOUCH] ERROR");
    }
}

bool touchPressed()
{
    return ts.touched();
}

bool touchReadRaw(TouchPoint &point)
{
    if (!ts.touched())
        return false;

    TS_Point p = ts.getPoint();

    point.x = p.x;
    point.y = p.y;
    point.z = p.z;

    return true;
}