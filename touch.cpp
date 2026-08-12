#include "touch.h"
#include "display.h"
#include "settings.h"

#include <Arduino.h>
#include <FS.h>
#include <SPI.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

static uint16_t touchCalData[5];


// ============================================================
// BOUTON RECOMMENCER
// ============================================================

#define BUTTON_X       25
#define BUTTON_Y       270
#define BUTTON_W       190
#define BUTTON_H       40

#define BUTTON_TEXT_X  (BUTTON_X + BUTTON_W / 2)
#define BUTTON_TEXT_Y  (BUTTON_Y + BUTTON_H / 2)


// ============================================================
// INITIALISATION
// ============================================================

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
        Serial.println("[TOUCH] Calibration forcee");

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


// ============================================================
// CHARGEMENT CALIBRATION
// ============================================================

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


// ============================================================
// SAUVEGARDE CALIBRATION
// ============================================================

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


// ============================================================
// CALIBRATION
// ============================================================

bool touchCalibration()
{
    Serial.println("=== CALIBRATION TOUCH ===");

    // --------------------------------------------------------
    // ECRAN DE CALIBRATION
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // CALIBRATION TFT_eSPI
    // --------------------------------------------------------

    uint16_t calData[5];

    tft.calibrateTouch(
        calData,
        COLOR_POINT,
        COLOR_BACKGROUND,
        15
    );


    // --------------------------------------------------------
    // COPIE DES DONNEES
    // --------------------------------------------------------

    for (uint8_t i = 0; i < 5; i++)
    {
        touchCalData[i] = calData[i];
    }


    // --------------------------------------------------------
    // ACTIVATION IMMEDIATE
    // --------------------------------------------------------

    tft.setTouch(touchCalData);


    // --------------------------------------------------------
    // SAUVEGARDE
    // --------------------------------------------------------

    touchSaveCalibration();


    // --------------------------------------------------------
    // RESULTATS TERMINAL
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // RESULTATS TFT
    // --------------------------------------------------------

    tft.fillScreen(COLOR_BACKGROUND);

    tft.setTextDatum(TC_DATUM);

    tft.setTextColor(
        COLOR_SUCCESS,
        COLOR_BACKGROUND
    );

    tft.drawString(
        "CALIBRATION OK",
        SCREEN_WIDTH / 2,
        15,
        2
    );


    tft.setTextColor(
        COLOR_TEXT,
        COLOR_BACKGROUND
    );

    tft.drawString(
        "RESULTATS",
        SCREEN_WIDTH / 2,
        45,
        2
    );


    // --------------------------------------------------------
    // DONNEES
    // --------------------------------------------------------

    tft.setTextDatum(TL_DATUM);

    char buffer[40];


    sprintf(
        buffer,
        "X MIN : %u",
        touchCalData[0]
    );

    tft.drawString(
        buffer,
        15,
        75,
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
        105,
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
        135,
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
        165,
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
        195,
        2
    );


    // --------------------------------------------------------
    // MESSAGE SAUVEGARDE
    // --------------------------------------------------------

    tft.setTextDatum(TC_DATUM);

    tft.setTextColor(
        COLOR_SUCCESS,
        COLOR_BACKGROUND
    );

    tft.drawString(
        "DONNEES SAUVEGARDEES",
        SCREEN_WIDTH / 2,
        225,
        2
    );


    // --------------------------------------------------------
    // BOUTON RECOMMENCER
    // --------------------------------------------------------

    tft.fillRoundRect(
        BUTTON_X,
        BUTTON_Y,
        BUTTON_W,
        BUTTON_H,
        6,
        TFT_BLUE
    );

    tft.drawRoundRect(
        BUTTON_X,
        BUTTON_Y,
        BUTTON_W,
        BUTTON_H,
        6,
        TFT_WHITE
    );

    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(
        TFT_WHITE,
        TFT_BLUE
    );

    tft.drawString(
        "RECOMMENCER",
        BUTTON_TEXT_X,
        BUTTON_TEXT_Y,
        2
    );


    // --------------------------------------------------------
    // MESSAGE TERMINAL
    // --------------------------------------------------------

    Serial.println("[TOUCH] Calibration terminee");
    Serial.println("[TOUCH] Bouton RECOMMENCER disponible");

    return true;
}


// ============================================================
// GESTION DU BOUTON DE FIN
// ============================================================

void touchHandleEndScreen()
{
    uint16_t x = 0;
    uint16_t y = 0;

    // --------------------------------------------------------
    // Lecture tactile
    // --------------------------------------------------------

    if (!tft.getTouch(&x, &y))
    {
        return;
    }


    // --------------------------------------------------------
    // DEBUG
    // --------------------------------------------------------

    Serial.print("[TOUCH] X=");
    Serial.print(x);

    Serial.print(" Y=");
    Serial.println(y);


    // --------------------------------------------------------
    // TEST DU BOUTON
    // --------------------------------------------------------

    if (
        x >= BUTTON_X &&
        x <= BUTTON_X + BUTTON_W &&
        y >= BUTTON_Y &&
        y <= BUTTON_Y + BUTTON_H
    )
    {
        Serial.println("[TOUCH] RECOMMENCER");


        // ----------------------------------------------------
        // FEEDBACK VISUEL
        // ----------------------------------------------------

        tft.fillRoundRect(
            BUTTON_X,
            BUTTON_Y,
            BUTTON_W,
            BUTTON_H,
            6,
            TFT_DARKGREY
        );

        tft.drawRoundRect(
            BUTTON_X,
            BUTTON_Y,
            BUTTON_W,
            BUTTON_H,
            6,
            TFT_WHITE
        );

        tft.setTextDatum(MC_DATUM);

        tft.setTextColor(
            TFT_WHITE,
            TFT_DARKGREY
        );

        tft.drawString(
            "RECOMMENCER",
            BUTTON_TEXT_X,
            BUTTON_TEXT_Y,
            2
        );

        delay(150);


        // ----------------------------------------------------
        // ATTENTE DU RELACHEMENT
        // ----------------------------------------------------

        uint16_t releaseX;
        uint16_t releaseY;

        while (tft.getTouch(&releaseX, &releaseY))
        {
            delay(20);
        }


        // ----------------------------------------------------
        // NOUVELLE CALIBRATION
        // ----------------------------------------------------

        touchCalibration();
    }
}