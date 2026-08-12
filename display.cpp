#include "display.h"
#include "settings.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

static Adafruit_ILI9341 tft(
    TFT_CS,
    TFT_DC,
    TFT_RST
);

void displayInit()
{
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);

    tft.begin();
    tft.setRotation(TFT_ROTATION);
    tft.fillScreen(ILI9341_BLACK);
}

static void drawTarget(
    int16_t x,
    int16_t y
)
{
    tft.drawCircle(
        x,
        y,
        12,
        ILI9341_WHITE
    );

    tft.drawCircle(
        x,
        y,
        6,
        ILI9341_RED
    );

    tft.drawFastHLine(
        x - 18,
        y,
        36,
        ILI9341_WHITE
    );

    tft.drawFastVLine(
        x,
        y - 18,
        36,
        ILI9341_WHITE
    );

    tft.fillCircle(
        x,
        y,
        2,
        ILI9341_WHITE
    );
}

void displayCalibrationScreen(
    uint8_t point
)
{
    tft.fillScreen(ILI9341_BLACK);

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 8);

    tft.print("CALIBRATION");

    tft.setTextSize(1);
    tft.setCursor(10, 32);

    tft.print("POINT ");
    tft.print(point);
    tft.print("/");
    tft.print(CALIBRATION_POINTS);

    int16_t x;
    int16_t y;

    switch (point)
    {
        case 1:
            x = CAL_MARGIN_X;
            y = CAL_MARGIN_Y;
            break;

        case 2:
            x = SCREEN_WIDTH - CAL_MARGIN_X;
            y = CAL_MARGIN_Y;
            break;

        case 3:
            x = SCREEN_WIDTH - CAL_MARGIN_X;
            y = SCREEN_HEIGHT - CAL_MARGIN_Y;
            break;

        default:
            x = CAL_MARGIN_X;
            y = SCREEN_HEIGHT - CAL_MARGIN_Y;
            break;
    }

    drawTarget(x, y);
}

void displayCalibrationResult(
    bool success,
    uint16_t xMin,
    uint16_t xMax,
    uint16_t yMin,
    uint16_t yMax
)
{
    tft.fillScreen(ILI9341_BLACK);

    tft.setTextSize(2);

    if (success)
    {
        tft.setTextColor(ILI9341_GREEN);
        tft.setCursor(20, 20);
        tft.print("CALIBRATION OK");
    }
    else
    {
        tft.setTextColor(ILI9341_RED);
        tft.setCursor(20, 20);
        tft.print("CALIBRATION ERROR");
    }

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);

    tft.setCursor(20, 65);
    tft.print("X MIN: ");
    tft.print(xMin);

    tft.setCursor(20, 85);
    tft.print("X MAX: ");
    tft.print(xMax);

    tft.setCursor(20, 105);
    tft.print("Y MIN: ");
    tft.print(yMin);

    tft.setCursor(20, 125);
    tft.print("Y MAX: ");
    tft.print(yMax);

    if (success)
    {
        tft.setCursor(20, 165);
        tft.print("TOUCH TO TEST");
    }
}

void displayTestScreen()
{
    tft.fillScreen(ILI9341_BLACK);

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);

    tft.setCursor(10, 8);
    tft.print("TOUCH TEST");

    tft.setTextSize(1);

    tft.drawRect(
        2,
        42,
        SCREEN_WIDTH - 4,
        SCREEN_HEIGHT - 45,
        ILI9341_DARKGREY
    );
}

void displayTouchPoint(
    int16_t x,
    int16_t y
)
{
    static int16_t lastX = -1;
    static int16_t lastY = -1;

    if (lastX >= 0 && lastY >= 0)
    {
        tft.fillCircle(
            lastX,
            lastY,
            4,
            ILI9341_BLACK
        );
    }

    tft.fillCircle(
        x,
        y,
        4,
        ILI9341_GREEN
    );

    tft.fillRect(
        5,
        SCREEN_HEIGHT - 35,
        230,
        20,
        ILI9341_BLACK
    );

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);

    tft.setCursor(
        10,
        SCREEN_HEIGHT - 30
    );

    tft.print("X=");
    tft.print(x);

    tft.print(" Y=");
    tft.print(y);

    lastX = x;
    lastY = y;
}