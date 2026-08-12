#include "touch.h"
#include "display.h"
#include "settings.h"

#include <Arduino.h>
#include <FS.h>
#include <SPI.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

static uint16_t touchCalData[5];

void touchInit()
{
    Serial.println("[TOUCH] Initialisation...");

    if (!SPIFFS.begin())
    {
        Serial.println("[TOUCH] SPIFFS absent");
        Serial.println("[TOUCH] Formatage SPIFFS...");

        SPIFFS.format();

        if (!SPIFFS.begin())
        {
            Serial.println("[TOUCH] ERREUR SPIFFS");
            return;
        }
    }

    if (FORCE_CALIBRATION)
    {
        touchCalibration();
        return;
    }

    if (touchLoadCalibration())
    {
        Serial.println("[TOUCH] Calibration chargee");
    }
    else
    {
        Serial.println("[TOUCH] Aucune calibration");
        touchCalibration();
    }
}

bool touchLoadCalibration()
{
    if (!SPIFFS.exists(CALIBRATION_FILE))
    {
        return false;
    }

    fs::File file = SPIFFS.open(
        CALIBRATION_FILE,
        "r"
    );

    if (!file)
    {
        return false;
    }

    size_t size = file.readBytes(
        (char *)touchCalData,
        sizeof(touchCalData)
    );

    file.close();

    if (size != sizeof(touchCalData))
    {
        return false;
    }

    tft.setTouch(touchCalData);

    return true;
}

void touchSaveCalibration()
{
    fs::File file = SPIFFS.open(
        CALIBRATION_FILE,
        "w"
    );

    if (!file)
    {
        Serial.println("[TOUCH] ERREUR sauvegarde");

        return;
    }

    file.write(
        (const uint8_t *)touchCalData,
        sizeof(touchCalData)
    );

    file.close();

    Serial.println("[TOUCH] Calibration sauvegardee");
}

bool touchCalibration()
{
    // ------------------------------------------------------------
    // TITRE UNIQUE
    // ------------------------------------------------------------

    Serial.println("=== CALIBRATION TOUCH ===");

    // ------------------------------------------------------------
    // ECRAN
    // ------------------------------------------------------------

    tft.fillScreen(COLOR_BACKGROUND);

    tft.setTextDatum(TC_DATUM);

    tft.setTextColor(
        COLOR_TEXT,
        COLOR_BACKGROUND
    );

    tft.drawString(
        "CALIBRATION TOUCH",
        SCREEN_WIDTH / 2,
        20,
        2
    );

    tft.drawString(
        "Touchez les points",
        SCREEN_WIDTH / 2,
        50,
        2
    );

    delay(1000);

    // ------------------------------------------------------------
    // CALIBRATION
    // ------------------------------------------------------------

    uint16_t calData[5];

    tft.calibrateTouch(
        calData,
        COLOR_POINT,
        COLOR_BACKGROUND,
        15
    );

    // Copie des donnees
    for (uint8_t i = 0; i < 5; i++)
    {
        touchCalData[i] = calData[i];
    }

    // Activation immediate
    tft.setTouch(touchCalData);

    // ------------------------------------------------------------
    // SAUVEGARDE
    // ------------------------------------------------------------

    touchSaveCalibration();

    // ------------------------------------------------------------
    // RESULTATS TERMINAL
    // ------------------------------------------------------------

    Serial.println();
    Serial.println("=== RESULTATS CALIBRATION ===");

    Serial.print("CAL_DATA[0] = ");
    Serial.println(touchCalData[0]);

    Serial.print("CAL_DATA[1] = ");
    Serial.println(touchCalData[1]);

    Serial.print("CAL_DATA[2] = ");
    Serial.println(touchCalData[2]);

    Serial.print("CAL_DATA[3] = ");
    Serial.println(touchCalData[3]);

    Serial.print("CAL_DATA[4] = ");
    Serial.println(touchCalData[4]);

    Serial.println("==============================");

    // ------------------------------------------------------------
    // RESULTATS ECRAN
    // ------------------------------------------------------------

    tft.fillScreen(COLOR_BACKGROUND);

    tft.setTextDatum(TC_DATUM);

    tft.setTextColor(
        COLOR_SUCCESS,
        COLOR_BACKGROUND
    );

    tft.drawString(
        "CALIBRATION OK",
        SCREEN_WIDTH / 2,
        20,
        2
    );

    tft.setTextColor(
        COLOR_TEXT,
        COLOR_BACKGROUND
    );

    tft.drawString(
        "RESULTATS",
        SCREEN_WIDTH / 2,
        50,
        2
    );

    tft.setTextDatum(TL_DATUM);

    char buffer[32];

    sprintf(
        buffer,
        "X MIN : %u",
        touchCalData[0]
    );

    tft.drawString(
        buffer,
        15,
        85,
        2
    );

    sprintf(
        buffer,
        "X MAX : %u",
        touchCalData[1]
    );

    tft.drawString(
        buffer,
        15,
        115,
        2
    );

    sprintf(
        buffer,
        "Y MIN : %u",
        touchCalData[2]
    );

    tft.drawString(
        buffer,
        15,
        145,
        2
    );

    sprintf(
        buffer,
        "Y MAX : %u",
        touchCalData[3]
    );

    tft.drawString(
        buffer,
        15,
        175,
        2
    );

    sprintf(
        buffer,
        "ROTATION : %u",
        touchCalData[4]
    );

    tft.drawString(
        buffer,
        15,
        205,
        2
    );

    tft.setTextDatum(TC_DATUM);

    tft.setTextColor(
        COLOR_SUCCESS,
        COLOR_BACKGROUND
    );

    tft.drawString(
        "DONNEES SAUVEGARDEES",
        SCREEN_WIDTH / 2,
        255,
        2
    );

    delay(1500);

    Serial.println("[TOUCH] Calibration terminee");

    return true;
}