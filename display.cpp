#include "display.h"
#include "settings.h"

TFT_eSPI tft = TFT_eSPI();

void displayInit()
{
    Serial.println("[TFT] Initialisation...");

    tft.init();

    tft.setRotation(TFT_ROTATION);

    tft.fillScreen(COLOR_BACKGROUND);

    Serial.println("[TFT] OK");
}